/*
 * record the data to rdr. (RDR: kernel run data recorder.)
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/vmalloc.h>
#include <linux/zlib.h>
#include <linux/mutex.h>

#define COMPR_LEVEL 3

static DEFINE_MUTEX(compr_mutex);
static struct z_stream_s stream;

int rdr_compress(void *in, void *out, size_t inlen, size_t outlen)
{
	int err, ret;

	ret = -EIO;
	mutex_lock(&compr_mutex);
	err = zlib_deflateInit(&stream, COMPR_LEVEL);
	if (err != Z_OK)
		goto error;

	stream.next_in = in;
	stream.avail_in = inlen;
	stream.total_in = 0;
	stream.next_out = out;
	stream.avail_out = outlen;
	stream.total_out = 0;

	err = zlib_deflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END)
		goto error;

	err = zlib_deflateEnd(&stream);
	if (err != Z_OK)
		goto error;

	if (stream.total_out >= stream.total_in)
		goto error;

	ret = stream.total_out;
error:
	mutex_unlock(&compr_mutex);
	return ret;
}

int rdr_uncompress(void *in, void *out, size_t inlen, size_t outlen)
{
	int err, ret;

	ret = -EIO;
	mutex_lock(&compr_mutex);
	err = zlib_inflateInit(&stream);
	if (err != Z_OK)
		goto error;

	stream.next_in = in;
	stream.avail_in = inlen;
	stream.total_in = 0;
	stream.next_out = out;
	stream.avail_out = outlen;
	stream.total_out = 0;

	err = zlib_inflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END)
		goto error;

	err = zlib_inflateEnd(&stream);
	if (err != Z_OK)
		goto error;

	ret = 0;
error:
	mutex_unlock(&compr_mutex);
	return ret;
}

int rdr_compr_init(void)
{
/*lint -e666*/
	size_t size = max(zlib_deflate_workspacesize(MAX_WBITS, MAX_MEM_LEVEL),
			zlib_inflate_workspacesize());
/*lint +e666*/
	stream.workspace = vmalloc(size);
	if (!stream.workspace)
		return -ENOMEM;
	return 0;
}

void rdr_compr_exit(void)
{
	vfree(stream.workspace);
}
