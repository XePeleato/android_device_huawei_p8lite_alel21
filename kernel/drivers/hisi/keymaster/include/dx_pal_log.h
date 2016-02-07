/*******************************************************************  
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This software is protected by copyright, international           *
* treaties and patents.                                            *
* Use of this Software as part of or with the Discretix CryptoCell *
* or Packet Engine products is governed by the products'           *
* commercial end user license agreement ("EULA").                  *
* It is possible that copies of this Software might be distributed *
* under some type of GNU General Public License ("GPL").           *
* Notwithstanding any such distribution under the terms of GPL,    *
* GPL does not govern the use of this Software as part of or with  *
* the Discretix CryptoCell or Packet Engine products, for which a  *
* EULA is required.                                                *
* If the product's EULA allows any copy or reproduction of this    *
* Software, then such copy or reproduction must include this       *
* Copyright Notice as well as any other notices provided           *
* thereunder.                                                      *
********************************************************************/

#ifndef _DX_PAL_LOG_H_
#define _DX_PAL_LOG_H_

#include "dx_pal_types.h"
#include "dx_pal_log_plat.h"
#include "log.h"

/* PAL log levels (to be used in DX_PAL_logLevel) */
#define DX_PAL_LOG_LEVEL_NULL      -1 /* Disable logging */
#define DX_PAL_LOG_LEVEL_ERR       0
#define DX_PAL_LOG_LEVEL_WARN      1
#define DX_PAL_LOG_LEVEL_INFO      2
#define DX_PAL_LOG_LEVEL_DEBUG     3
#define DX_PAL_LOG_LEVEL_TRACE     4
#define DX_PAL_LOG_LEVEL_DATA      5

#ifndef DX_PAL_LOG_CUR_COMPONENT
/* Setting default component mask in case caller did not define */
/* (a mask that is always on for every log mask value but full masking) */
#define DX_PAL_LOG_CUR_COMPONENT 0xFFFFFFFF
#endif

/* Select compile time log level */
#ifndef DX_PAL_MAX_LOG_LEVEL /* Can be overriden by external definition of this constant */
#ifdef DEBUG
#define DX_PAL_MAX_LOG_LEVEL DX_PAL_LOG_LEVEL_DEBUG
#else /* Disable logging */
#define DX_PAL_MAX_LOG_LEVEL DX_PAL_LOG_LEVEL_NULL
#endif
#endif /*DX_PAL_MAX_LOG_LEVEL*/

#if DX_PAL_MAX_LOG_LEVEL > DX_PAL_LOG_LEVEL_NULL
void DX_PAL_LogInit();
void DX_PAL_LogLevelSet(int setLevel);
void DX_PAL_LogMaskSet(DxUint32_t setMask);
extern int DX_PAL_logLevel;
extern DxUint32_t DX_PAL_logMask;
#else /* Log facilities are disabled - dx_pal_log.c is not linked in */
#define DX_PAL_LogInit() do {} while (0)
#define DX_PAL_LogLevelSet(setLevel) do {} while (0)
#define DX_PAL_LogMaskSet(setMask) do {} while (0)
#endif
#define _DX_PAL_LOG LOGI
#define DX_PAL_LOG_ERR LOGE
#define DX_PAL_LOG_WARN LOGI
#define DX_PAL_LOG_INFO LOGI
#define DX_PAL_LOG_DEBUG LOGI
#define DX_PAL_LOG_DUMP_BUF(msg, buf, size)	do {} while (0)
#define DX_PAL_LOG_TRACE LOGI
#define DX_PAL_LOG_DATA LOGI
/* Filter logging based on logMask and dispatch to platform specific logging mechanism */
/*
#define _DX_PAL_LOG(level, format, ...)  \
	if (DX_PAL_logMask & DX_PAL_LOG_CUR_COMPONENT) \
		__DX_PAL_LOG_PLAT(DX_PAL_LOG_LEVEL_ ## level, "%s: " format, __func__, ##__VA_ARGS__)

#if (DX_PAL_MAX_LOG_LEVEL >= DX_PAL_LOG_LEVEL_ERR)
#define DX_PAL_LOG_ERR(format, ...) \
	_DX_PAL_LOG(ERR, format, ##__VA_ARGS__)
#else
#define DX_PAL_LOG_ERR(format, arg...) do {} while (0)
#endif

#if (DX_PAL_MAX_LOG_LEVEL >= DX_PAL_LOG_LEVEL_WARN)
#define DX_PAL_LOG_WARN(format, ...) \
	if (DX_PAL_logLevel >= DX_PAL_LOG_LEVEL_WARN) \
		_DX_PAL_LOG(WARN, format, ##__VA_ARGS__)
#else
#define DX_PAL_LOG_WARN(format, arg...) do {} while (0)
#endif

#if (DX_PAL_MAX_LOG_LEVEL >= DX_PAL_LOG_LEVEL_INFO)
#define DX_PAL_LOG_INFO(format, ...) \
	if (DX_PAL_logLevel >= DX_PAL_LOG_LEVEL_INFO) \
		_DX_PAL_LOG(INFO, format, ##__VA_ARGS__)
#else
#define DX_PAL_LOG_INFO(format, arg...) do {} while (0)
#endif

#if (DX_PAL_MAX_LOG_LEVEL >= DX_PAL_LOG_LEVEL_DEBUG)
#define DX_PAL_LOG_DEBUG(format, ...) \
	if (DX_PAL_logLevel >= DX_PAL_LOG_LEVEL_DEBUG) \
		_DX_PAL_LOG(DEBUG, format, ##__VA_ARGS__)

#define DX_PAL_LOG_DUMP_BUF(msg, buf, size)		\
	do {						\
	int i;						\
	uint8_t	*pData = (uint8_t*)buf;			\
							\
	PRINTF("%s (%d):\n", msg, size);		\
	for (i = 0; i < size; i++) {			\
		PRINTF("0x%02X ", pData[i]);		\
		if ((i & 0xF) == 0xF) {			\
			PRINTF("\n");			\
		}					\
	}						\
	PRINTF("\n");					\
	} while (0)
#else
#define DX_PAL_LOG_DEBUG(format, arg...) do {} while (0)
#define DX_PAL_LOG_DUMP_BUF(msg, buf, size)	do {} while (0)
#endif

#if (DX_PAL_MAX_LOG_LEVEL >= DX_PAL_LOG_LEVEL_TRACE)
#define DX_PAL_LOG_TRACE(format, ...) \
	if (DX_PAL_logLevel >= DX_PAL_LOG_LEVEL_TRACE) \
		_DX_PAL_LOG(TRACE, format, ##__VA_ARGS__)
#else
#define DX_PAL_LOG_TRACE(format, arg...) do {} while (0)
#endif

#if (DX_PAL_MAX_LOG_LEVEL >= DX_PAL_LOG_LEVEL_TRACE)
#define DX_PAL_LOG_DATA(format, ...) \
	if (DX_PAL_logLevel >= DX_PAL_LOG_LEVEL_TRACE) \
		_DX_PAL_LOG(DATA, format, ##__VA_ARGS__)
#else
#define DX_PAL_LOG_DATA(format, arg...) do {} while (0)
#endif
*/
#endif /*_DX_PAL_LOG_H_*/
