/*************************************************************************/ /*!
@File           reloc_api.h
@Title          
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@Description    This file contains the header file information for the RELOC API.
@License        Strictly Confidential.
*/ /**************************************************************************/
#ifndef RELOC_API_H
#define RELOC_API_H

#include "img_include.h"

#if defined (__cplusplus)
extern "C" {
#endif


/*
 *  Enumeration of functions in the framework which can be invoked by modules.
 * These must correspond with the functions in apfnFrameworkEntry (reloc_api.c).
 */
typedef enum
{
    RELOC_RENDEC_SLICESTART,
    RELOC_RENDEC_CHUNKSTART,
    RELOC_RENDEC_QUEUEWORD,
    RELOC_RENDEC_SLICEEND,
    RELOC_RENDEC_INSERTSECONDPASSMARKER,
    RELOC_RENDEC_WRITEBUF,
    RELOC_SR_SETMASTER,
    RELOC_SR_SEEKSCPOREOD,
    RELOC_SR_READSCP,
    RELOC_SR_CHECKSCPOREOD,
    RELOC_SR_OUTPUTCMDCHECKRESP,
    RELOC_SR_OUTPUTCMDCHECKRESPEXPGOULOMB,
    RELOC_SR_CONFIG,
    RELOC_SR_WAITFORVALID,
#if defined (MTXG) && defined (FW_PRINT)
    RELOC_VDECFW_PRINT_S,
    RELOC_VDECFW_PRINT_SV,
#endif
#ifndef NO_BOOL_CODER
    RELOC_SR_BOOLCODERREADBITPROB,
    RELOC_SR_BOOLCODERREADBITS128,
    RELOC_SR_SETBYTECOUNT,
    RELOC_SR_BOOLCODERCONFIG,
    RELOC_SR_BOOLCODERGETCONTEXT,
    RELOC_SR_BOOLCODERSETMASTER,
    RELOC_SR_QUERYBYTECOUNT,
#endif
    RELOC_DMA_FLUSHCHANNELLINKEDLISTNOTCB,
    RELOC_DMA_MODIFYLINKEDLISTHEAD, 
    RELOC_DMA_SETSECURESYNCTRANSFER,
    RELOC_DMA_SETINSECURESYNCTRANSFER,
    RELOC_FRAMEWORL_VDECFW_GETEXCBUF,
	RELOC_FRAMEWORK_ENTRY_POINTS
} RELOC_eFunction;


/*!
******************************************************************************

 @Function                RELOC_Initialise

 @Description

 This function is used to initialise the RELOC module.

 @Return   None

******************************************************************************/
extern IMG_VOID RELOC_Initialise(IMG_VOID);


/*!
******************************************************************************

 @Function                RELOC_CallModule

 @Description

 This function is used to call a relocatable module function.

 @Input    ui32Param1  : first parameter of the function to be called

 @Input    ui32Param2  : second parameter of the function to be called

 @Input    ui32Param3  : third parameter of the function to be called

 @Input    ui32Param4  : fourth parameter of the function to be called

 @Input    ui32Param5  : fifth parameter of the function to be called

 @Input    ui32ModFunc : id of the function to be called

 @Return   IMG_UINT32 : This function returns either IMG_SUCCESS or an error code.

******************************************************************************/
extern IMG_UINT32 RELOC_CallModule(
    IMG_UINT32  ui32Param1,
    IMG_UINT32  ui32Param2,
    IMG_UINT32  ui32Param3,
    IMG_UINT32  ui32Param4,
    IMG_UINT32  ui32Param5,
    IMG_UINT32  ui32ModFunc
);


/*!
******************************************************************************

 @Function              RELOC_FrameworkEntry

 @Description

 This function is used to call a framework function.

 @Input    ui32Param1 : first parameter of the function to be called

 @Input    ui32Param2 : second parameter of the function to be called

 @Input    ui32Param3 : third parameter of the function to be called

 @Input    ui32Param4 : fourth parameter of the function to be called

 @Input    ui32Param5 : fifth parameter of the function to be called

 @Input    eFunction  : id of the function to be called

 @Return   IMG_UINT32 : This function returns either IMG_SUCCESS or an error code.

******************************************************************************/
extern IMG_UINT32 RELOC_FrameworkEntry(
    IMG_UINT32       ui32Param1,
    IMG_UINT32       ui32Param2,
    IMG_UINT32       ui32Param3,
    IMG_UINT32       ui32Param4,
    IMG_UINT32       ui32Param5,
    RELOC_eFunction  eFunction
);


/*!
******************************************************************************

 @Function              RELOC_SetAddress

 @Description

 This function is used to set data and text addresses of a relocatable module.

 @Input    ui32TextAddr : Relocatable module text address.

 @Input    ui32DataAddr : Relocatable module data address.

 @Return   None

******************************************************************************/
extern IMG_VOID RELOC_SetAddress(
    IMG_UINT32 ui32TextAddr,
    IMG_UINT32 ui32DataAddr
);


#if defined (__cplusplus)
}
#endif

#endif /* RELOC_API_H */
