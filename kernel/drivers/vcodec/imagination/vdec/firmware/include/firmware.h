/*************************************************************************/ /*!
@File           firmware.h
@Title          
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@Description    
@License        Strictly Confidential.
*/ /**************************************************************************/
#ifndef _FIRMWARE_H_
#define _FIRMWARE_H_

#include "msvdxapi_consts.h"

typedef struct
{
	const char* psVersion;
	const char* psBuildDate;
	IMG_UINT32 uiCoreSize;
	IMG_UINT32 uiBlobSize;
	const IMG_UINT8* pui8Blob;
	IMG_UINT32 ui32VideoNodeCapabilities;
	IMG_UINT32 uiFirmwareID;
} FIRMWARE;

#endif /* _FIRMWARE_H_ */
