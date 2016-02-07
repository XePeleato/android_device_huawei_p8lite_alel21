/*
 * =====================================================================================
 *
 *       Filename:  ext4_dump_info.h
 *
 *    Description:  dump ext4 infomation
 *
 *        Version:  1.0
 *        Created:  07/17/2014 09:31:55 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  MaYaohui (m00194513), stesen.ma@huawei.com
 *   Organization:  Huawei
 *
 * =====================================================================================
 */

#ifndef EXT4_DUMP_INFO_H
#define EXT4_DUMP_INFO_H

static inline int is_ext4_fs_dirty(struct super_block *sb)
{
	return !(EXT4_SB(sb)->s_es->s_state & EXT4_VALID_FS);
}

void dump_ext4_sb_info(struct super_block *sb);

#endif
