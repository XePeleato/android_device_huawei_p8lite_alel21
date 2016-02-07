/*!
 *****************************************************************************
 *
 * @File       include_all_fw_variants.h
 * @Title      Firmware variant header
 * @Description    Header file that includes all firmware variants
 * ---------------------------------------------------------------------------
 *
 * Copyright (c) Imagination Technologies Ltd.
 * 
 * The contents of this file are subject to the MIT license as set out below.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 * 
 * Alternatively, the contents of this file may be used under the terms of the 
 * GNU General Public License Version 2 ("GPL")in which case the provisions of
 * GPL are applicable instead of those above. 
 * 
 * If you wish to allow use of your version of this file only under the terms 
 * of GPL, and not to allow others to use your version of this file under the 
 * terms of the MIT license, indicate your decision by deleting the provisions 
 * above and replace them with the notice and other provisions required by GPL 
 * as set out in the file called "GPLHEADER" included in this distribution. If 
 * you do not delete the provisions above, a recipient may use your version of 
 * this file under the terms of either the MIT license or GPL.
 * 
 * This License is also included in this distribution in the file called 
 * "MIT_COPYING".
 *
 *****************************************************************************/


#ifndef __INCLUDE_ALL_VARIANTS_INC_INCLUDED__
#define __INCLUDE_ALL_VARIANTS_INC_INCLUDED__

#define INCLUDE_ALL_VARIANTS_TEMPLATE_VERSION   (1)

#define FW_BIN_FORMAT_VERSION (2)

struct IMG_COMPILED_FW_BIN_RECORD {

    IMG_UINT32 ui32TextSize, ui32DataSize;
    IMG_UINT32 ui32DataOrigin, ui32TextOrigin;
    IMG_UINT32 ui32TextRelocSize, ui32DataRelocSize;

    IMG_UINT32 ui32Pipes;
    IMG_CHAR   *sFormat, *rcMode;
    IMG_UINT32 ui32FormatsMask, ui32HwConfig;

    IMG_UINT32 ui32IntDefineCount;
    IMG_CHAR   **pscIntDefineNames;
    IMG_UINT32 *pui32IntDefines;

    IMG_UINT32 *pui32Text, *pui32Data;
    IMG_UINT32 *pui32TextReloc, *pui32DataDeloc;
    IMG_UINT32 *pui32TextRelocFullAddr, *pui32TextRelocType;
};

#include "H264MVC_FW_NO_RC_pipes_2_contexts_2_hwconfig_0_bin.c"
#include "H264MVC_FW_CBR_pipes_2_contexts_2_hwconfig_0_bin.c"
#include "H264MVC_FW_VBR_pipes_2_contexts_2_hwconfig_0_bin.c"
#include "H264_FW_ALL_pipes_2_contexts_2_hwconfig_0_bin.c"
#include "H264MVC_FW_NO_RC_pipes_4_contexts_2_hwconfig_0_bin.c"
#include "JPEG_FW_NO_RC_pipes_4_contexts_2_hwconfig_0_bin.c"
#include "H264MVC_FW_CBR_pipes_4_contexts_2_hwconfig_0_bin.c"
#include "H264MVC_FW_VBR_pipes_4_contexts_2_hwconfig_0_bin.c"
#include "H264_FW_ALL_pipes_4_contexts_2_hwconfig_0_bin.c"
#include "LEGACY_VIDEO_FW_ALL_pipes_4_contexts_2_hwconfig_0_bin.c"
#include "JPEG_H264_FW_ALL_pipes_4_contexts_2_hwconfig_0_bin.c"
#include "H264_FW_ALL_pipes_3_contexts_3_hwconfig_0_bin.c"
#include "ALL_CODECS_FW_ALL_pipes_2_contexts_8_hwconfig_1_bin.c"

IMG_UINT32 ui32AllFirmwareBinariesCount = 13;
struct IMG_COMPILED_FW_BIN_RECORD *sAllFirmwareBinaries[] = {
	&sIMG_COMPILED_H264MVC_FW_NO_RC_pipes_2_contexts_2_hwconfig_0,
	&sIMG_COMPILED_H264MVC_FW_CBR_pipes_2_contexts_2_hwconfig_0,
	&sIMG_COMPILED_H264MVC_FW_VBR_pipes_2_contexts_2_hwconfig_0,
	&sIMG_COMPILED_H264_FW_ALL_pipes_2_contexts_2_hwconfig_0,
	&sIMG_COMPILED_H264MVC_FW_NO_RC_pipes_4_contexts_2_hwconfig_0,
	&sIMG_COMPILED_JPEG_FW_NO_RC_pipes_4_contexts_2_hwconfig_0,
	&sIMG_COMPILED_H264MVC_FW_CBR_pipes_4_contexts_2_hwconfig_0,
	&sIMG_COMPILED_H264MVC_FW_VBR_pipes_4_contexts_2_hwconfig_0,
	&sIMG_COMPILED_H264_FW_ALL_pipes_4_contexts_2_hwconfig_0,
	&sIMG_COMPILED_LEGACY_VIDEO_FW_ALL_pipes_4_contexts_2_hwconfig_0,
	&sIMG_COMPILED_JPEG_H264_FW_ALL_pipes_4_contexts_2_hwconfig_0,
	&sIMG_COMPILED_H264_FW_ALL_pipes_3_contexts_3_hwconfig_0,
	&sIMG_COMPILED_ALL_CODECS_FW_ALL_pipes_2_contexts_8_hwconfig_1,
};

#endif
