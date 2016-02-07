/*!
 *****************************************************************************
 *
 * @File       msvdx_ext_reg.h
 * @Title      Low-level MSVDX interface component
 * @Description    This file contains the interface to communicate with an MSVDX core.
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

#if !defined (__MSVDX_EXT_REG_H__)
#define __MSVDX_EXT_REG_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "img_include.h"
#ifdef SECURE_MEDIA_SUPPORT
    #include "secure_defs.h"
    #include "secureapi.h"
#endif

#ifdef SECURE_MEDIA_SUPPORT
    //Using Secure Interface
    #define MSVDXIO_WriteRegisterXX(h, m, o, v)         \
    {                                                   \
        WRITENREGS  sWriteReg;                          \
                                                        \
        sWriteReg.ui32Offset = o;                       \
        sWriteReg.ui32Value = v;                        \
        sWriteReg.ui32ValidMask = -1;                   \
        sWriteReg.ui8Pipe = 0;                          \
                                                        \
        SECURE_WriteNRegs(h, 1, m, &sWriteReg);\
    }

    static IMG_UINT32 MSVDXIO_ReadRegisterXX(
        IMG_HANDLE          hSecure,
        MEM_REGIONS         eMemRegion,
        IMG_UINT32          ui32Offset
    )
    {
        READNREGS  sReadReg;

        sReadReg.ui32Offset = ui32Offset;
        sReadReg.ui8Pipe = 0;

        //printf("MSVDXIO_ReadRegisterX hSecure = 0x%08x\n",hSecure);
        SECURE_ReadNRegs(hSecure, 1, eMemRegion, &sReadReg);

        return sReadReg.ui32Result;
    }

    #define MSVDXIO_WriteRegisterX(m, r, o, v)      MSVDXIO_WriteRegisterXX(m->hSecure, r, o, v)
    #define MSVDXIO_ReadRegisterX(m, r, o)          MSVDXIO_ReadRegisterXX(m->hSecure, r, o)
#else

    #define MSVDXIO_WriteRegisterX(m, r, o, v)      VXDIO_WriteRegister(m->hMsvdxIo, r, o, v, -1, 0)
    #define MSVDXIO_ReadRegisterX(m, r, o)          VXDIO_ReadRegister(m->hMsvdxIo, r, o, 0)

#endif

#define MSVDX_CORE_WriteRegister(m, o, v)   MSVDXIO_WriteRegisterX(m, REGION_VXDSYS_REGSPACE, o, v)
#define MSVDX_CORE_ReadRegister(m, o)       MSVDXIO_ReadRegisterX(m, REGION_VXDSYS_REGSPACE, o)

#define MSVDX_VEC_WriteRegister(m, o, v)    MSVDXIO_WriteRegisterX(m, REGION_VEC_REGSPACE, o, v)
#define MSVDX_VEC_ReadRegister(m, o)        MSVDXIO_ReadRegisterX(m, REGION_VEC_REGSPACE, o)

#define DMAC_WriteRegister(m, o, v)         MSVDXIO_WriteRegisterX(m, REGION_DMAC_REGSPACE, o, v)
#define DMAC_ReadRegister(m, o)             MSVDXIO_ReadRegisterX(m, REGION_DMAC_REGSPACE, o)

#define MSVDX_VDMC_WriteRegister(m, o, v)   MSVDXIO_WriteRegisterX(m, REGION_VDMC_REGSPACE, o, v)
#define MSVDX_VDMC_ReadRegister(m, o)       MSVDXIO_ReadRegisterX(m, REGION_VDMC_REGSPACE, o)

#define MSVDX_VDEB_WriteRegister(m, o, v)   MSVDXIO_WriteRegisterX(m, REGION_VDEB_REGSPACE, o, v)
#define MSVDX_VDEB_ReadRegister(m, o)       MSVDXIO_ReadRegisterX(m, REGION_VDEB_REGSPACE, o)

#define MSVDX_VLR_WriteRegister(m, o, v)    MSVDXIO_WriteRegisterX(m, REGION_VLR_REGSPACE, o, v)
#define MSVDX_VLR_ReadRegister(m, o)        MSVDXIO_ReadRegisterX(m, REGION_VLR_REGSPACE, o)

#define MTX_CORE_WriteRegister(m, o, v)     MSVDXIO_WriteRegisterX(m, REGION_VXDMTX_REGSPACE, o, v)
#define MTX_CORE_ReadRegister(m, o)         MSVDXIO_ReadRegisterX(m, REGION_VXDMTX_REGSPACE, o)

#define MSVDX_TEST_WriteRegister(m, o, v)   MSVDXIO_WriteRegisterX(m, REGION_VXDTEST_REGSPACE, o, v)
#define MSVDX_TEST_ReadRegister(m, o)       MSVDXIO_ReadRegisterX(m, REGION_VXDTEST_REGSPACE, o)

#if defined(__cplusplus)
}
#endif

#endif /* __MSVDX_EXT_REG_H__ */



