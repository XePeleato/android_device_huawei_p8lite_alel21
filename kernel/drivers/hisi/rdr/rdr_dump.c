/*
 * dump the rdr data. (RDR: kernel run data recorder.)
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>
#include <linux/vmalloc.h>
#include <linux/sort.h>
#include <huawei_platform/log/log_exception.h>

#include <linux/huawei/rdr.h>
#include <linux/huawei/rdr_private.h>
#include "rdr_internal.h"

struct linux_dirent {
	unsigned long	d_ino;
	unsigned long	d_off;
	unsigned short	d_reclen;
	char		d_name[1];
};

void rdr_file_close(int fd)
{
	(void)sys_close((unsigned int)fd);
	return;
}

int rdr_wait4partition(char *path, int timeouts)
{
	struct kstat m_stat;
	mm_segment_t old_fs;
	int timeo = timeouts;

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	rdr_debug("wait for %s ready ...", path);

	while (0 != vfs_stat(path, &m_stat)) {
		current->state = TASK_INTERRUPTIBLE;
		(void)schedule_timeout(HZ); /*wait for 1 second*/
		if (timeouts-- == 0) {
			pr_err("rdr:%s wait %ds fail.skip!\n", path, timeo);
			set_fs(old_fs);
			return 1;
		}
	}
	set_fs(old_fs);
	rdr_debug("%s is ready!", path);
	return 0;
}

int wait_for_fs_ready(void)
{
	int ret = 0;

	ret += rdr_wait4partition("/data/lost+found", 30);
	ret += rdr_wait4partition("/dev/block", 30);

	return ret;
}

int rdr_open_dir(const char *dirname)
{
	int fd;

	struct kstat m_stat;
	int error = vfs_stat("/data/lost+found", &m_stat);
	if (error) {
		DUMP_LOG(error);
		return error;
	}

	rdr_create_dir(dirname);
	fd = sys_open(dirname, O_DIRECTORY, 0);
	if (fd < 0) { /* if dir is not exist,then create new dir */
		DUMP_LOG(fd);
		fd  = sys_mkdir(dirname, 0774);
		if (fd < 0) {
			DUMP_LOG(fd);
			return fd;
		}
		fd = sys_open(dirname, O_DIRECTORY, 0);
	}

	return fd;
}

int del_old_file(const char *dirname, const char *file_header,
		unsigned int fd, const char *header)
{
	long read_bytes;
	size_t head_len, j, min;
	void *buf;
	int index, i;
	struct linux_dirent *dir;
	char fname[RDR_FILE_MAX_CNT][RDR_FNAME_LEN+1] = { {0} };
	char temp_fname[RDR_FNAME_LEN+1];
	char str2int_buf[16];
	char *p_dname;

	buf = kmalloc(1024, GFP_KERNEL);
	if (NULL == buf)
		return -1;

	read_bytes = sys_getdents(fd, (struct linux_dirent *)buf, 1024);
	if ((read_bytes == 0) || (read_bytes < 0)) { /* -1 or 0 */
		kfree(buf);
		return (int)read_bytes;
	}

	head_len = strlen(header);
	for (i = 0; i < read_bytes; ) { /*begin to poll dir : */
		dir = (struct linux_dirent *)((char *)buf + i);
		i += dir->d_reclen;

		/* delete old and error file: */
		if (0 == strncmp((char *)dir->d_name, header, head_len)) {
			min = strlen(dirname) + 1;
			if (min > sizeof(temp_fname))
				min = sizeof(temp_fname) - 1;
			memset(temp_fname, 0, sizeof(temp_fname));
			memcpy(temp_fname, dirname, min);
			min = sizeof(temp_fname) - min;
			if (min > 1)
				strncat(temp_fname, dir->d_name, min);
			else {
				pr_err("rdr:file path is too long!\n");
				kfree(buf);
				return -1;
			}

			p_dname = dir->d_name + head_len;
			for (j = 0; j < sizeof(str2int_buf); j++) {
				if ((*p_dname == '\0') || (*p_dname > '9') ||
						(*p_dname < '0')) {
					str2int_buf[j] = '\0';
					break;
				}
				str2int_buf[j] = *p_dname;
				p_dname++;
			}
			if (kstrtoint(str2int_buf, 10, &index)) {
				DUMP_LOG((int)index);
				kfree(buf);
				return -1;
			}

			if ((index >= RDR_FILE_MAX_CNT - 1)
					|| (0 != fname[index][0])) {
				if (sys_unlink(temp_fname)) {
					DUMP_LOG(0);
					kfree(buf);
					return -1;
				}
			} else
				strncpy(fname[index], temp_fname,
					RDR_FNAME_LEN);
		}
	}

	for (i = RDR_FILE_MAX_CNT - 2; i >= 0; i--) { /* rename file: */
		if (fname[i][0]) {
			snprintf(temp_fname, sizeof(temp_fname),
				"%s%s%02d.bin", dirname, file_header, i + 1);
			if (sys_rename(fname[i], temp_fname)) {
				DUMP_LOG(0);
				kfree(buf);
				return -1;
			}
		}
	}

	kfree(buf);
	return 0;
}

char *rdr_get_file_postfix(char *filename)
{
	char *p = filename;
	if (*p == '\0')
		return NULL;
	while (*p)
		p++;
	while ((*p != '.') && (p != filename))
		p--;
	if (*p == '.') {
		p++;
		return p;
	}

	return NULL;
}

char *rdr_get_file_name(char *filename_withpath)
{
	char *p = filename_withpath;
	if (*p == '\0')
		return NULL;
	while (*p)
		p++;
	while ((*p != '/') && (p != filename_withpath))
		p--;
	if (*p == '/') {
		p++;
		return p;
	}

	return NULL;
}

char *rdr_get_mainname(char *filename, char *mainname, unsigned int len)
{
	unsigned int str_len;
	unsigned int min;
	char *p;
	memset(mainname, 0, len);
	if ((filename == NULL) || (*filename == '\0'))
		return NULL;
	str_len = strlen(filename);
	min = str_len < len ? str_len : len;
	strncpy(mainname, filename, min);
	p = mainname;
	while (*p)
		p++;
	while ((p != mainname) && (*p != '.'))
		p--;
	if (*p == '.')
		*p = '\0';
	return mainname;
}

char *rdr_get_parent_dir(char *path, char *parent, size_t len)
{
	char *p = parent;
	size_t path_len = strlen(path);
	size_t min = (path_len < len) ? path_len : len;

	memset(parent, 0, len);
	strncpy(parent, path, min);
	if (*p == '\0')
		return NULL;
	while (*p)
		p++;
	p--;
	if (*p == '/')
		p--;
	while ((*p != '/') && (p != parent))
		p--;
	if (*p == '/') {
		p++;
		*p = '\0';
		return parent;
	}

	return NULL;
}

int rdr_mov2up(char *fullname, char *dstfullname, u32 arg2)
{
	/* whether the fullname is rdr/rdx/dfx file */
	char *p = rdr_get_file_postfix(fullname);
	char *name;
	int shouldmov = 0;
	struct kstat m_stat;

	if (p == NULL)
		return -1;
	if (!strncmp("rdr", p, 3) || !strncmp("rdx", p, 3) ||
		!strncmp("dfx", p, 3)) {
		if (0 == vfs_stat(fullname, &m_stat)) {
			if (m_stat.size == sizeof(struct rdr_struct_s))
				shouldmov = 1;
		}
	}

	if (shouldmov == 1) {
		char xname[64];
		name = rdr_get_file_name(fullname);
		if (name == NULL)
			return -1;
		memset(xname, 0, sizeof(xname));
		strncpy(xname, dstfullname, sizeof(xname) - 1);
		strncat(xname, name, sizeof(xname) - strlen(dstfullname) - 1);
		xname[sizeof(xname) - 1] = '\0';
		rdr_debug("move %s to %s", fullname, xname);
		if (0 != sys_rename(fullname, xname))
			pr_err("rdr:mv from %s to %s fail\n", fullname, xname);
	}
	return 0;
}

#define RDRDIRSIZ 1024

int rdr_dir_list(char *path, rdr_funcptr_3 f, u64 arg1, u64 arg2, int *cnt, int type)
{
	int fd = -1, nread, bpos, ret = 0, tmp_cnt = 0;
	char *buf;
	struct linux_dirent *d;
	char d_type;
	mm_segment_t old_fs;
	char fullname[RDR_FNAME_LEN];

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	fd = sys_open(path, O_RDONLY, 0664);/*create file */
	if (fd < 0) {
		pr_err("rdr:%s(),open %s fail,r:%d\n", __func__, path, fd);
		ret = -1;
		goto out;
	}

	buf = vmalloc(RDRDIRSIZ);
	if (buf == NULL) {
		pr_err("rdr:%s():vmalloc failed\n", __func__);
		ret = -1;
		goto out;
	}

	for (;;) {
		nread = sys_getdents(fd, (struct linux_dirent *)buf, RDRDIRSIZ);
		if (nread == -1) {
			pr_err("rdr:%s():sys_getdents failed\n", __func__);
			ret = -1;
			break;
		}

		if (nread == 0) {
			ret = 0;
			break;
		}

		for (bpos = 0; bpos < nread;) {
			d = (struct linux_dirent *)(buf + bpos);
			d_type = *(buf + bpos + d->d_reclen - 1);
			if ((d_type == type) && (f != NULL)) {
				snprintf(fullname, sizeof(fullname), "%s%s", path, d->d_name);
				rdr_debug("fullname:%s", fullname);
				f((u64)fullname, arg1, arg2);
			}
			if (d_type == type)
				tmp_cnt++;
			bpos += d->d_reclen;
		}
	}
	if (cnt != (int *)0)
		*cnt = tmp_cnt;
	vfree(buf);
out:
	if (fd >= 0)
		sys_close(fd);
	set_fs(old_fs);
	return ret;
}

static int rdr_zip_init(int in)
{
	static int v_rdr_compr_init;
	int ret;
	if (in) {
		if (v_rdr_compr_init == 0) {
			ret = rdr_compr_init();
			if (ret == 0) {
				v_rdr_compr_init = 1;
				return 0;
			} else {
				pr_err("rdr:%s() init error\n", __func__);
				return -1;
			}
		} else
			return 0;
	} else {
		if (v_rdr_compr_init == 1) {
			rdr_compr_exit();
			v_rdr_compr_init = 0;
		}
	}
	return 0;
}

int rdr_rm_8m0(char *fullname, u32 arg1, u32 arg2)
{
	struct kstat m_stat;
	int ret;
	char *p = rdr_get_file_postfix(fullname);
	if (p == NULL) {
		pr_info("rdr:rdr_get_file_postfix failed\n");
		return -1;
	}
	if (!strncmp("rdr", p, 3) || !strncmp("rdx", p, 3) ||
		!strncmp("dfx", p, 3)) {
		rdr_debug("rm %s", fullname);
		sys_unlink(fullname);
		return 0;
	}

	ret = vfs_stat(fullname, &m_stat);
	if (ret == 0) {
		size_t s = m_stat.size;
		if ((s == 0) || (s == sizeof(struct rdr_struct_s))) {
			rdr_debug("rm %s(0 size)", fullname);
			sys_unlink(fullname);
		}
	}
	return 0;
}

int rdr_rm_file(char *fullname, u32 arg1, u32 arg2)
{
	sys_unlink(fullname);
	return 0;
}

int rdr_rm_dir(char *path)
{
	char *pdst = path;
	int ret = 0;

	while (*pdst)
		pdst++;
	pdst--;
	if (*pdst == '/')
		*pdst = '\0';
	ret = sys_rmdir(path);
	if (ret != 0)
		pr_err("rdr:%s():del %s failed\n", __func__, path);

	return ret;
}

#define RDR_RGZNAME_SZ 64

int rdr_get_8m_name(char *fullname, u64 rgz_name_tab, u32 cnt)
{
	int i;
	char *tab = (char *)rgz_name_tab;
	char *p;
	char *name = rdr_get_file_name(fullname);

	if ((name == NULL) || (tab == NULL))
		return -1;
	if (strncmp(name, "rdr.bin", 7) &&
		strncmp(name, "rdx.bin", 7) &&
		strncmp(name, "dfx.bin", 7))
		return -1;

	for (i = 0; i < cnt; i++) {
		p = tab + RDR_RGZNAME_SZ * i;
		if (*p == '\0') {
			strncpy(p, name, RDR_RGZNAME_SZ);
			break;
		}
	}
	return 0;
}

#define RDR_TIME_DIR_FORMAT "20110101105748_00085.128125"
#define RDR_DIR_LEN strlen(RDR_TIME_DIR_FORMAT)

int rdr_get_timedir_name(char *fullname, u64 rgz_name_tab, u32 cnt)
{
	int i;
	char *tab = (char *)rgz_name_tab;
	char *p;
	char *name = rdr_get_file_name(fullname);

	if ((name == NULL) || (tab == NULL))
		return -1;
	if ((strlen(name) != RDR_DIR_LEN) || (name[14] != '_'))
		return -1;

	for (i = 0; i < cnt; i++) {
		p = tab + RDR_RGZNAME_SZ * i;
		if (*p == '\0') {
			strncpy(p, name, RDR_RGZNAME_SZ);
			break;
		}
	}
	return 0;
}

int rdr_getname(char *fullname, u64 rgz_name_tab, u32 cnt)
{
	int i;
	char *tab = (char *)rgz_name_tab;
	char *p;
	char *name = rdr_get_file_name(fullname);

	if ((name == NULL) || (tab == NULL))
		return -1;

	for (i = 0; i < cnt; i++) {
		p = tab + RDR_RGZNAME_SZ * i;
		if (*p == '\0') {
			strncpy(p, name, RDR_RGZNAME_SZ);
			break;
		}
	}
	return 0;
}

int rdr_file2zfile(char *fullname, u64 zipfd, u64 zip_head)
{
	int ret, fd, i, j, out_cnt, err = 0, in_len;
	struct rdr_zip_file *zfile = (struct rdr_zip_file *)zip_head;
	struct rdr_zip_head *head = &zfile->head;
	u32 off = sizeof(struct rdr_zip_file) +
			sizeof(struct rdr_zip_head) * (zfile->file_number - 1);
	char *zip_buf = (char *)(zip_head + off);
	char *name = rdr_get_file_name(fullname);
	char *postfix;
	int block_seq = 0;
	if (name == NULL) {
		pr_err("rdr:%s() name is null...return.\n", __func__);
		return -1;
	}

	if (rdr_tmp_pbb == NULL) {
		pr_err("rdr:%s() rdr_tmp_pbb == NULL\n", __func__);
		return -1;
	}
	postfix = rdr_get_file_postfix(name);
	if (postfix != NULL) {
		if (!strncmp("rgz", postfix, 3))
			return -3;
	}

	fd = sys_open(fullname, O_RDONLY, 0664);
	if (fd < 0) {
		pr_err("rdr:%s() open bin file failed\n", __func__);
		return -1;
	}

	for (i = 0; i < zfile->file_number; i++) {
		if (*head[i].filename == '\0')
			break;
	}
	while (1) {
		in_len = sys_read(fd, (char *)rdr_tmp_pbb, sizeof(struct rdr_struct_s));
		if (in_len <= 0) {
			rdr_debug("read the file to zip end.");
			err = 0;
			goto out;
		}
		out_cnt = rdr_compress(rdr_tmp_pbb, zip_buf,
			in_len, sizeof(struct rdr_struct_s));
		if (out_cnt < 0) {
			pr_err("rdr:%s() rdr_compress get %d, inlen:%d.\n",
				__func__, out_cnt, in_len);
			if (in_len < 100) {
				memcpy(zip_buf, (char *)rdr_tmp_pbb, in_len);
				out_cnt = in_len;
			} else {
				err = -1;
				goto out;
			}
		}
		ret = sys_write(zipfd, zip_buf, out_cnt);
		if (ret < out_cnt) {
			pr_err("rdr:%s() sys_write return %d.\n", __func__, ret);
			err = -1;
			goto out;
		}

		/* fill record: */
		if (block_seq != 0) {
			head[i].z[block_seq].off = head[i].z[block_seq - 1].off
				+ head[i].z[block_seq - 1].zip_len;
		} else {
			if (i == 0)
				head[i].z[block_seq].off = off;
			else {
				j = 0;
				while (j < RDR_ZIP_MAX_BLOCK) {
					if (head[i - 1].z[j].off == 0)
						break;
					else
						j++;
				}
				if (j != 0)
					j--;

				head[i].z[block_seq].off = head[i - 1].z[j].off
					+ head[i - 1].z[j].zip_len;
			}
		}

		strncpy(head[i].filename, name, RDR_ZIP_FNAME_LEN - 1);
		head[i].filename[RDR_ZIP_FNAME_LEN - 1] = '\0';
		head[i].z[block_seq].orig_len = in_len;
		head[i].z[block_seq].zip_len = out_cnt;
		block_seq++;
		if (block_seq == RDR_ZIP_MAX_BLOCK) {
			pr_err("rdr:file %s is too big, stop\n", fullname);
			err = -4;
			goto out;
		}
	}
out:
	sys_close(fd);
	return err;
}

int rdr_zip_dir(char *path, char *rgz, size_t rgz_flen)
{
	struct rdr_zip_file *dst;
	int zipfd, ret;
	struct kstat m_stat;

	rdr_debug("zip upload path to archiver file");
	/* check wether path is exist */
	if (path == NULL)
		return -1;
	ret = vfs_stat(path, &m_stat);
	if (ret) {
		DUMP_LOG(1);
		pr_err("rdr:%s():dir not exist, dont archive\n", __func__);
		return -1;
	}

	/* check path file count, if count=0, del and return, else fill field */
	rdr_dir_list(path, NULL, 0, 0, &ret, DT_REG);
	rdr_debug("file cnt is %d", ret);
	if (ret > 0) {
		char fname[RDR_FNAME_LEN];
		char timebuf[RDR_TIME_LEN];
		size_t siz = sizeof(struct rdr_zip_file) +
				sizeof(struct rdr_zip_head) * (ret - 1);
		size_t siz2 = siz + sizeof(struct rdr_struct_s);
		/* fill field */
		dst = vmalloc(siz2);
		if (dst == NULL) {
			pr_err("rdr:%s():vmalloc dst failed\n", __func__);
			return -1;
		}
		memset(dst, 0, siz);
		dst->zip_file_magic = RDR_ZIP_FILE_MAGIC;
		dst->file_number = ret;
		/* init zip env */
		rdr_zip_init(1);

		/* open zipfd */
		get_time_stamp(timebuf, RDR_TIME_LEN);
		snprintf(fname, sizeof(fname), "%s%s.rgz", path, timebuf);
		rdr_debug("archiver to %s", fname);
		zipfd = sys_open(fname, O_CREAT | O_RDWR, 0664);
		if (zipfd < 0) {
			vfree(dst);
			rdr_zip_init(0);
			return -1;
		}

		/* get dst file path and name, and open it. */
		ret = sys_lseek(zipfd, siz, SEEK_SET);
		if (ret < siz) {
			pr_err("rdr:%s():lseek to %u failed\n", __func__, siz);
			sys_close(zipfd);
			vfree(dst);
			rdr_zip_init(0);
			return -1;
		}
		rdr_dir_list(path, (rdr_funcptr_3)rdr_file2zfile,
					(u64)zipfd, (u64)dst, &ret, DT_REG);

		/* exit zip env */
		rdr_zip_init(0);

		/* write head to file: */
		ret = sys_lseek(zipfd, 0, SEEK_SET);
		if (ret != 0) {
			pr_err("rdr:%s():lseek failed\n", __func__);
			sys_close(zipfd);
			vfree(dst);
			return -1;
		}
		ret = sys_write(zipfd, (char *)dst, siz);
		if (ret < siz) {
			pr_err("rdr:%s():write head failed\n", __func__);
			sys_close(zipfd);
			vfree(dst);
			return -1;
		}

		/* close and free */
		strncpy(rgz, fname, rgz_flen);
		sys_close(zipfd);
		vfree(dst);
	} else {
		rdr_debug("delete empty dir %s", path);
		ret = sys_rmdir(path);/* del this dir */
		if (ret != 0)
			pr_err("rdr:%s():delete dir %s fail\n", __func__, path);
	}

	return 0;
}

int rdr_copy_file(char *dst, char *src)
{
	int fddst, fdsrc, ret = 0;
	char *buf;
	struct kstat m_stat;
	long cnt;

	if (0 != vfs_stat(src, &m_stat)) {
		pr_err("rdr:%s():stat failed\n", __func__);
		return -1;
	}

	buf = vmalloc(SZ_4K);
	if (buf == NULL) {
		pr_err("rdr:%s():malloc 4K failed\n", __func__);
		return -1;
	}
	fdsrc = sys_open(src, O_RDONLY, 0664);
	if (fdsrc < 0) {
		pr_err("rdr:%s():open src failed\n", __func__);
		ret = -2;
		goto out;
	}
	fddst = sys_open(dst, O_CREAT | O_WRONLY, 0664);
	if (fddst < 0) {
		pr_err("rdr:%s():open dst failed\n", __func__);
		sys_close(fdsrc);
		ret = -3;
		goto out;
	}
	while (1) {
		cnt = sys_read(fdsrc, buf, SZ_4K);
		if (cnt == 0)
			break;
		cnt = sys_write(fddst, buf, cnt);
		if (cnt <= 0) {
			pr_err("rdr:%s():write dst failed\n", __func__);
			ret = -4;
			goto close;
		}
	}

close:
	sys_close(fdsrc);
	sys_close(fddst);
out:
	vfree(buf);
	return ret;
}

/*#define CONFIG_SRECORDER 1*/

#ifdef CONFIG_SRECORDER
#define SRECORDER_PATH "/data/log/dbox/srecorder/"
#define SRECORDER_FILE "/data/log/dbox/srecorder/rdr.log"
#endif

int rdr_cmp(const void *a, const void *b)
{
	return strncmp((char *)a, (char *)b, RDR_RGZNAME_SZ);
}

int strnswp(char *a, char *b, size_t len)
{
	char c;
	size_t i;
	rdr_debug("swap:%s, %s", a, b);
	for (i = 0; i < len; i++) {
		c = a[i];
		a[i] = b[i];
		b[i] = c;
	}
	rdr_debug("swap result:%s, %s", a, b);
	return 0;
}

void rdr_up2apr(char *up_path, char *ecore)
{
	char cmd[ARCH_NAME_MAX];
	char time[RDR_TIME_LEN];
	int ret = 0;

	if (ecore == NULL)
		return;
	pr_info("rdr:%s():upload to %s exception class\n", __func__, ecore);
	if (!strncmp(ecore, RDR_STR_CORE_CP, strlen(RDR_STR_CORE_CP))) {
		ret = log_to_exception("rild", "modem_exception balong reset");
		if (ret < 0)
			pr_err("rdr:%s():logexception sysfs err.\n", __func__);
		return;
	}

	get_time_stamp(time, RDR_TIME_LEN);
	if (!strncmp(ecore, RDR_STR_CORE_HIFI, strlen(RDR_STR_CORE_HIFI))) {
		snprintf(cmd, ARCH_NAME_MAX,
			"archive -i %s -i %s -i %s -o %s_%s -z zip",
			 up_path, "/data/android_logs/kmsgcat-log",
			 "/data/hisi_logs/hifi_log", time, "hificrash");
		ret = log_to_exception("balong_hifi", cmd);
	} else {
		snprintf(cmd, ARCH_NAME_MAX,
			"archive -i %s -i %s -o %s_%s -z zip", up_path,
			"/data/android_logs/kmsgcat-log", time, "apanic");
		ret = log_to_exception("balong_apanic", cmd);
	}

	if (ret < 0)
		pr_err("rdr:%s():logexception sysfs err.\n", __func__);
	return;
}

void rdr_rm_over3_file(char *path)
{
	int ret, i, j;
	mm_segment_t old_fs;
	struct kstat m_stat;
	char fn[RDR_FNAME_LEN];

	char *name_tab;
	size_t tab_siz;
	char *pn;

	if (path == NULL)
		return;
	old_fs = get_fs();
	set_fs(KERNEL_DS);

	pr_info("rdr:%s():begin to del too many dir\n", __func__);
	/* check path , if path isnt exist, return. */
	ret = vfs_stat(path, &m_stat);
	if (ret) {
		rdr_debug("dir not exist, dont upload and archive");
		goto out;
	}
	rdr_dir_list(path, NULL, 0, 0, &ret, DT_DIR);
	rdr_debug("the filefolder in %s dir :%d", path, ret);

	if (ret > 3) {
		tab_siz = ret * RDR_RGZNAME_SZ;
		name_tab = vmalloc(tab_siz);
		if (name_tab == NULL) {
			pr_err("rdr:%s():name_tab alloc fail.\n", __func__);
			goto out;
		}
		memset(name_tab, 0, tab_siz);
		rdr_dir_list(path, (rdr_funcptr_3)rdr_get_timedir_name,
				(u64)name_tab, (u64)ret, (int *)0, DT_DIR);

		for (i = 0; i < ret; i++) {
			pn = name_tab + i * RDR_RGZNAME_SZ;
			if (*pn == '\0')
				break;
			rdr_debug("dir:%s, seq:%d", pn, i);
		}
		rdr_debug("has %d time dir", i);
		if (i > 3) {
			sort(name_tab, i, RDR_RGZNAME_SZ, rdr_cmp, NULL);
			for (j = 0; j < i - 3; j++) {
				pn = name_tab + j * RDR_RGZNAME_SZ;
				snprintf(fn, sizeof(fn), "%s%s/", path, pn);
				pr_info("rdr:rm folder %s and all files\n", fn);
				rdr_dir_list(fn, (rdr_funcptr_3)rdr_rm_file,
							0, 0, (int *)0, DT_REG);
				rdr_rm_dir(fn);
			}
		}

		vfree(name_tab);
	}
out:
	set_fs(old_fs);
}

void rdr_upload(char *path, char *timedir, char *ecore)
{
	int ret, i;
	mm_segment_t old_fs;
	struct kstat m_stat;
	char dst[RDR_FNAME_LEN] = {0};
	char fn[RDR_FNAME_LEN];
	int rdrbin_tag = 0;

	char *name_tab;
	size_t tab_siz;
	char *pn;
	char rgz[RDR_FNAME_LEN];

	if (path == NULL)
		return;
	old_fs = get_fs();
	set_fs(KERNEL_DS);

	/* check path , if path isnt exist, return. */
	ret = vfs_stat(path, &m_stat);
	if (ret) {
		rdr_debug("dir not exist, dont upload and archive");
		goto out;
	}

	/* check rdr/rdx/dfx, select the best one, delete others. */
	snprintf(dst, sizeof(dst), "%s%s/", path, timedir);
	ret = vfs_stat(dst, &m_stat);
	if (ret) {
		rdr_debug("dir %s not exist", dst);
		goto out;
	}
	rdr_dir_list(dst, NULL, 0, 0, &ret, DT_REG);
	if (ret > 1) {
		tab_siz = ret * RDR_RGZNAME_SZ;
		rdr_debug("the files in %s dir :%d", dst, ret);

		name_tab = vmalloc(tab_siz);
		if (name_tab == NULL) {
			pr_err("rdr:%s():name_tab alloc fail.\n", __func__);
			goto out;
		}
		memset(name_tab, 0, tab_siz);
		rdr_dir_list(dst, (rdr_funcptr_3)rdr_get_8m_name,
				(u64)name_tab, (u64)ret, (int *)0, DT_REG);

		for (i = 0; i < ret; i++) {
			pn = name_tab + i * RDR_RGZNAME_SZ;
			if (*pn == '\0')
				break;
			if (!strncmp("rdr.bin", pn, 7)) {
				pr_info("rdr:find rdr.bin, del other bin\n");
				snprintf(fn, sizeof(fn), "%srdx.bin", dst);
				sys_unlink(fn);
				snprintf(fn, sizeof(fn), "%sdfx.bin", dst);
				sys_unlink(fn);
				rdrbin_tag = 1;
				break;
			}
		}
		if (rdrbin_tag == 0) {
			for (i = 0; i < ret; i++) {
				pn = name_tab + i * RDR_RGZNAME_SZ;
				if (*pn == '\0')
					break;
				if (!strncmp("dfx.bin", pn, 7)) {
					pr_info("rdr:find dfx.bin,rm others\n");
					snprintf(fn, sizeof(fn), "%srdx.bin", dst);
					sys_unlink(fn);
					break;
				}
			}
		}
		vfree(name_tab);
	}

	memset(rgz, 0, sizeof(rgz));
	/* zip or upload it. */
#ifdef CONFIG_SRECORDER
	rdr_zip_dir(dst, rgz, sizeof(rgz));
	rdr_create_dir(SRECORDER_PATH);
	rdr_debug("rgz:%s", rgz);
	ret = sys_rename(rgz, SRECORDER_FILE);
	if (ret < 0)
		pr_err("rdr:rename %s to %s failed\n", rgz, SRECORDER_FILE);
#else
	if ((ecore != NULL) &&
		(!strncmp(ecore, RDR_STR_CORE_CP, strlen(RDR_STR_CORE_CP)))) {
		rdr_zip_dir(dst, rgz, sizeof(rgz));
		snprintf(fn, sizeof(fn), "%sup/cp.rgz", OM_ROOT_PATH);
		rdr_create_dir(fn);
		sys_unlink(fn);
		ret = sys_rename(rgz, fn);
		if (ret < 0)
			pr_err("rdr:rename %s to %s failed\n", rgz, fn);
		else
			pr_info("rdr:mv from %s to %s OK\n", rgz, fn);
	}
	rdr_up2apr(dst, ecore);
#endif
	/* if the file counts of path dir over 3, delete it. */
	rdr_rm_over3_file(path);
out:
	set_fs(old_fs);
}

void rdr_upload_and_archive(char *path, char *timedir, char *ecore)
{
	int ret, i, j;
	mm_segment_t old_fs;
	struct kstat m_stat;
	char dst[RDR_FNAME_LEN] = {0};
	char rgz[RDR_ZIP_FNAME_LEN];
	char dstname[RDR_ZIP_FNAME_LEN];
	char pjname[RDR_ZIP_FNAME_LEN];
	char *name;

	if (path == NULL)
		return;
	old_fs = get_fs();
	set_fs(KERNEL_DS);

	/* check path , if path isnt exist, return. */
	ret = vfs_stat(path, &m_stat);
	if (ret) {
		rdr_debug("dir not exist, dont upload and archive");
		goto out;
	}

	/* prepare dest dir : upload */
	snprintf(dst, RDR_FNAME_LEN, "%sup/", path);
	rdr_create_dir(dst);
	/* Maybe up dir has already existed and has some files.so delete it. */
	rdr_dir_list(dst, (rdr_funcptr_3)rdr_rm_file, 0, 0, (int *)0, DT_REG);

	/* find dfx/rdr/rdx file and mov to up dir */
	rdr_dir_list(path, (rdr_funcptr_3)rdr_mov2up, (u64)dst, 0, (int *)0, DT_REG);
	rdr_dir_list(dst, NULL, 0, 0, &ret, DT_REG);
	if (ret > 1) {
		char *name_tab;
		size_t tab_siz = ret * RDR_RGZNAME_SZ;
		rdr_debug("the files in up dir :%d", ret);

		name_tab = vmalloc(tab_siz);
		if (name_tab == NULL) {
			pr_err("rdr:%s():name_tab alloc fail.\n", __func__);
			goto out;
		}
		memset(name_tab, 0, tab_siz);
		rdr_dir_list(dst, (rdr_funcptr_3)rdr_getname,
				(u64)name_tab, (u64)ret, (int *)0, DT_REG);
		sort(name_tab, ret, RDR_RGZNAME_SZ, rdr_cmp, NULL);

		for (i = 0; i < ret; i++) {
			for (j = i + 1; j < ret; j++) {
				char *mi = name_tab + RDR_RGZNAME_SZ * i;
				char *mj = name_tab + RDR_RGZNAME_SZ * j;
				char *pi = rdr_get_mainname(mi, dstname,
							RDR_ZIP_FNAME_LEN);
				char *pj = rdr_get_mainname(mj, pjname,
							RDR_ZIP_FNAME_LEN);
				char *fi = rdr_get_file_postfix(mi);
				char *fj = rdr_get_file_postfix(mj);
				if ((pi == NULL) || (pj == NULL) || (fi == NULL)
					|| (fj == NULL))
					continue;
				rdr_debug("cmp filename:pi:%s,pj:%s", pi, pj);
				if (!strncmp(pi, pj, RDR_RGZNAME_SZ)) {
					if (!strncmp(fi, "rdr", 3) ||
						(!strncmp(fi, "dfx", 3) &&
						!strncmp(fj, "rdx", 3)))
						strnswp(mi, mj, RDR_RGZNAME_SZ);
					snprintf(rgz, RDR_ZIP_FNAME_LEN, "%s%s",
						dst, mi);
					rdr_debug("del same file: %s", rgz);
					sys_unlink(rgz);
				}
			}
		}

		vfree(name_tab);
	} else if (ret == 0) {
		rdr_debug("up dir has no file, return");
		return;
	}

	/* zip upload dir */
	memset(rgz, 0, sizeof(rgz));
	rdr_zip_dir(dst, rgz, sizeof(rgz));

	/* delete up dfx/rdr/rdx file in up, and the file that size is 0 */
	rdr_dir_list(dst, (rdr_funcptr_3)rdr_rm_8m0, 0, 0, (int *)0, DT_REG);

	/* cp zip from up to bk dir */
	snprintf(dst, RDR_FNAME_LEN, "%sbk/", path);
	rdr_create_dir(dst);
	name = rdr_get_file_name(rgz);
	snprintf(dstname, RDR_ZIP_FNAME_LEN, "%s%s", dst, name);
	rdr_copy_file(dstname, rgz);
	pr_info("rdr:%s:copy last words to %s OK!\n", __func__, dstname);

	snprintf(dst, RDR_FNAME_LEN, "%sup/", path);
#ifdef CONFIG_SRECORDER
	/* cp zip to srecorder dir */
	rdr_create_dir(SRECORDER_PATH);
	/*snprintf(dstname, RDR_ZIP_FNAME_LEN, "%s%s", SRECORDER_PATH, name);*/
	ret = rdr_copy_file(SRECORDER_FILE, rgz);
	if (ret != 0)
		pr_err("rdr:%s():cp to srecorder fail,ret:%d\n", __func__, ret);
	rdr_debug("copy to srecorder dir ok");
	rdr_dir_list(dst, (rdr_funcptr_3)rdr_rm_file, 0, 0, (int *)0, DT_REG);
#else
	/* upload rgz in up dir and delete */
	rdr_up2apr(dst, ecore);
	/*not delete up now, we'll del it nexttime for log_exception is async */
	/*rdr_dir_list(dst, (rdr_funcptr_3)rdr_rm_file, 0, 0, (int *)0, DT_REG);*/
#endif
	/*rdr_rm_dir(dst); NOTE:up dir is offen used, so dont delete it. */

	/* check bk dir, delete extra file. */
	snprintf(dst, RDR_FNAME_LEN, "%sbk/", path);
	rdr_dir_list(dst, NULL, 0, 0, &ret, DT_REG);
	if (ret > 3) {
		char *rgz_name_tab;
		size_t tab_siz = ret * RDR_RGZNAME_SZ;
		rdr_debug("the files cnt in bk dir :%d", ret);

		rgz_name_tab = vmalloc(tab_siz);
		if (rgz_name_tab == NULL) {
			pr_err("rdr:%s():rgz_name_tab alloc fail.\n", __func__);
			goto out;
		}
		memset(rgz_name_tab, 0, tab_siz);
		rdr_dir_list(dst, (rdr_funcptr_3)rdr_getname,
				(u64)rgz_name_tab, (u64)ret, (int *)0, DT_REG);
		sort(rgz_name_tab, ret, RDR_RGZNAME_SZ, rdr_cmp, NULL);

		for (i = 0; i < (ret - 3); i++) {
			snprintf(rgz, RDR_ZIP_FNAME_LEN, "%s%s",
				dst, rgz_name_tab + RDR_RGZNAME_SZ * i);
			rdr_debug("del %s", rgz);
			sys_unlink(rgz);
		}

		vfree(rgz_name_tab);
	}

out:
	set_fs(old_fs);
}

int rdr_save_8m(const char *dir, const char *name, char *postfix,
			const void *address, u32 length)
{
	int fd;
	long bytes;
	mm_segment_t old_fs;
	char xname[RDR_FNAME_LEN] = {0};

	if (NULL == dir || NULL == name || postfix == NULL) {
		pr_err("rdr:%s():file name and dir shouldnt null\n", __func__);
		return -1;
	}

	if ((strlen((const char *)dir) + strlen((const char *)name) +
		strlen(postfix) + 1) >= RDR_FNAME_LEN) {
		pr_err("rdr:%s():error:dir is too long, exit.\n", __func__);
		return -1;
	}

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	memset(xname, 0, sizeof(xname));
	snprintf(xname, sizeof(xname), "%s%s/", dir, name);
	rdr_create_dir(xname);
	memset(xname, 0, sizeof(xname));
	snprintf(xname, sizeof(xname), "%s%s/%s.bin", dir, name, postfix);

	fd = sys_creat(xname, 0644);
	if (fd < 0) {
		pr_err("rdr:%s():create file %s err.\n", __func__, xname);
		set_fs(old_fs);
		return -1;
	}

	bytes = sys_write((unsigned int)fd, (const char *)address, length);
	if ((u32)bytes != length) {
		DUMP_LOG((int)bytes);
		rdr_file_close(fd);
		set_fs(old_fs);
		return -1;
	}
	sys_fsync(fd);
	rdr_file_close(fd);
	set_fs(old_fs);
	pr_info("rdr:save file %s success ...", xname);

	return 0;
}

/*
 * Will be obsoleted ...
 */
int rdr_save_file(const char *dir, const char *file_header,
			const void *address, u32 length)
{
	int fd, ret;
	long bytes;
	mm_segment_t old_fs;
	char new_filename[RDR_FNAME_LEN] = {0};

	if (NULL == dir || NULL == file_header) {
		DUMP_LOG(0);
		return -1;
	}

	if ((strlen((const char *)dir) + strlen((const char *)file_header))
				>= RDR_FNAME_LEN) {
		DUMP_LOG(0);
		return -1;
	}

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	fd = rdr_open_dir(dir);
	if (fd < 0) {
		DUMP_LOG(fd);
		set_fs(old_fs);
		return -1;
	}

	ret = del_old_file(dir, file_header, (unsigned int)fd, file_header);
	if (0 != ret) {
		DUMP_LOG(ret);
		rdr_file_close(fd);
		set_fs(old_fs);
		return -1;
	}

	rdr_file_close(fd);

	memset(new_filename, 0, sizeof(new_filename));
	snprintf(new_filename, sizeof(new_filename), "%s%s%02d.bin",
			dir, file_header, 0);

	fd = sys_creat(new_filename, 0644);
	if (fd < 0) {
		DUMP_LOG(fd);
		set_fs(old_fs);
		return -1;
	}

	bytes = sys_write((unsigned int)fd, (const char *)address, length);
	if ((u32)bytes != length) {
		DUMP_LOG((int)bytes);
		rdr_file_close(fd);
		set_fs(old_fs);
		return -1;
	}
	sys_fsync(fd);
	rdr_file_close(fd);
	set_fs(old_fs);
	pr_info("rdr:save file %s success ...", new_filename);

	return 0;
}

static int __rdr_create_dir(char *path)
{
	int fd;

	mm_segment_t old_fs;
	old_fs = get_fs();
	set_fs(KERNEL_DS);

	fd = sys_access(path, 0);
	if (0 != fd) {
		pr_info("rdr: need create dir %s !\n", path);
		fd  = sys_mkdir(path, 0755);
		if (fd < 0) {
			pr_err("rdr: create dir %s failed! ret = %d\n",
					path, fd);
			set_fs(old_fs);
			return fd;
		}
		pr_info("rdr: create dir %s successed [%d]!!!\n", path, fd);
	}

	set_fs(old_fs);
	return 0;
}

int rdr_create_dir(const char *path)
{
	char cur_path[RDR_FNAME_LEN];
	int index = 0;
	memset(cur_path, 0, RDR_FNAME_LEN);
	if (*path != '/')
		return -1;
	cur_path[index++] = *path++;

	while (*path != '\0') {
		if (*path == '/')
			__rdr_create_dir(cur_path);
		cur_path[index] = *path;
		path++;
		index++;
	}
	return 0;
}


int rdr_remove_file(char *filename)
{
	int ret;
	if (filename == NULL)
		return 0;
	ret = sys_access(filename, 0);
	if (0 == ret) {
		if (sys_unlink(filename)) {
			DUMP_LOG(0);
			return -1;
		}
	}
	return 0;
}

/*this function used in little file , only write once.  */
int rdr_append_file(char *filename, void *address, u32 length, u32 max_size)
{
	int ret = 0;
	int fd;
	int bytes;
	int len;
	mm_segment_t old_fs;
	old_fs = get_fs();
	set_fs(KERNEL_DS);

	ret = rdr_create_dir(OM_ROOT_PATH);
	if (0 != ret) {
		pr_err("<%s()>, create dir [%s] failed! ret = %d\n",
				__func__, OM_ROOT_PATH, ret);
		goto out;
	}

	ret = sys_access(filename, 0);
	if (0 != ret) {
		fd = sys_open(filename, O_CREAT | O_RDWR, 0664);/*create file */
		if (fd < 0) {
			pr_err("<%s()>,createOopen fail,r:%d\n",
				__func__, fd);
			goto out;
		}
	} else {
		fd = sys_open(filename, O_APPEND | O_RDWR, 0664);
		if (fd < 0) {
			pr_err("<%s()>,appendOpen failed:r:%d\n", __func__, fd);
			goto out;
		}
	}

	len = sys_lseek(fd, 0, SEEK_END);
	if (len < 0) {
		pr_err("<%s()>, seek failed! ret = %d\n", __func__, len);
		ret = sys_close(fd);
		goto out;
	}

	if (len > max_size) {
		sys_close(fd);
		ret = sys_unlink(filename);
		if (0 != ret) {
			pr_err("<%s()>, remove failed!ret:%d\n", __func__, ret);
			goto out;
		}

		/*rebuild reset file*/
		fd = sys_open(filename, O_CREAT | O_RDWR, 0664);
		if (fd < 0) {
			pr_err("<%s()>, create failed! ret:%d\n", __func__, fd);
			goto out;
		}
	}

	bytes = sys_write(fd, address, length);
	if (bytes != length) {
		pr_err("<%s()>, write data failed! ret:%d\n", __func__, bytes);
		ret = sys_close(fd);
		goto out;
	}

	sys_fsync(fd);
	ret = sys_close(fd);
	if (0 != ret) {
		pr_err("<%s()>, close failed! ret = %d\n", __func__, ret);
		ret = -1;
		goto out;
	}

	ret = 0;

out:
	set_fs(old_fs);
	/*pr_info("rdr: <%s()>, save end. ret = %d\n", __func__, ret);*/
	return ret;
}

void rdr_looprw_set_state(int in)
{
	static int is_in;
	static mm_segment_t oldfs;

	if ((in != 0) && (in != 1))
		return;
	if (in == is_in)
		return;
	if (in == 1) {
		oldfs = get_fs();
		set_fs(KERNEL_DS);
		is_in = 1;
	} else { /* in == 0 */
		set_fs(oldfs);
		is_in = 0;
	}
	return;
}

int rdr_loopwrite_open(char *filename, int *fd)
{
	int ret = 0;

	ret = rdr_create_dir(filename);
	if (0 != ret) {
		pr_err("<%s()>, create dir [%s] failed! ret = %d\n",
				__func__, filename, ret);
		goto out;
	}

	ret = sys_access(filename, 0);
	if (ret == 0) {
		ret = sys_unlink(filename);
		if (0 != ret) {
			pr_err("<%s()>, remove failed!ret:%d\n", __func__, ret);
			goto out;
		}
	}
	*fd = sys_open(filename, O_CREAT | O_RDWR, 0664);/*create file */
	if (*fd < 0) {
		pr_err("<%s()>,createOopen fail,r:%d\n", __func__, *fd);
		goto out;
	}
	return 0;
out:
	return ret;
}

int rdr_loopwrite_append(int fd, void *address, u32 length)
{
	int bytes;

	bytes = sys_write(fd, address, length);
	if (bytes != length) {
		pr_err("<%s()>, write data failed! ret:%d\n", __func__, bytes);
		return -1;
	}

	return 0;
}

int rdr_loopwrite_close(int fd)
{
	int ret;
	sys_fsync(fd);
	ret = sys_close(fd);
	if (0 != ret) {
		pr_err("<%s()>, close failed! ret = %d\n", __func__, ret);
		ret = -1;
		goto out;
	}
out:
	return ret;
}

int rdr_loopread_open(char *filename, int *fd)
{
	*fd = sys_open(filename, O_RDONLY, 0664);/*create file */
	if (*fd < 0) {
		pr_err("<%s()>,open fail,r:%d\n", __func__, *fd);
		return *fd;
	}
	return 0;
}

int rdr_loopread_read(int fd, void *address, u32 length)
{
	int bytes;

	bytes = sys_read(fd, address, length);
	return bytes;
}

int rdr_loopread_close(int fd)
{
	int ret;

	ret = sys_close(fd);
	if (0 != ret) {
		pr_err("<%s()>, close failed! ret = %d\n", __func__, ret);
		ret = -1;
		goto out;
	}
out:
	return ret;
}
