/*!
 *****************************************************************************
 *
 * @File       img_soc_dmac_linked_list_io2.h
 * @Description    This file contains the IMG_SOC_DMAC_LINKED_LIST_IO2_H Definitions.
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


#if !defined (__IMG_SOC_DMAC_LINKED_LIST_IO2_H__)
#define __IMG_SOC_DMAC_LINKED_LIST_IO2_H__

#ifdef __cplusplus
extern "C" {
#endif


#define DMAC_LL_WD0_OFFSET		(0x0000)

/* DMAC_LL, WD0, RESERVED_01
*/
#define DMAC_LL_WD0_RESERVED_01_MASK		(0x0FFFFFFF)
#define DMAC_LL_WD0_RESERVED_01_LSBMASK		(0x0FFFFFFF)
#define DMAC_LL_WD0_RESERVED_01_SHIFT		(0)
#define DMAC_LL_WD0_RESERVED_01_LENGTH		(28)
#define DMAC_LL_WD0_RESERVED_01_DEFAULT		(0)
#define DMAC_LL_WD0_RESERVED_01_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD0, PW
*/
#define DMAC_LL_WD0_PW_MASK		(0x30000000)
#define DMAC_LL_WD0_PW_LSBMASK		(0x00000003)
#define DMAC_LL_WD0_PW_SHIFT		(28)
#define DMAC_LL_WD0_PW_LENGTH		(2)
#define DMAC_LL_WD0_PW_DEFAULT		(0)
#define DMAC_LL_WD0_PW_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD0, DIR
*/
#define DMAC_LL_WD0_DIR_MASK		(0x40000000)
#define DMAC_LL_WD0_DIR_LSBMASK		(0x00000001)
#define DMAC_LL_WD0_DIR_SHIFT		(30)
#define DMAC_LL_WD0_DIR_LENGTH		(1)
#define DMAC_LL_WD0_DIR_DEFAULT		(0)
#define DMAC_LL_WD0_DIR_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD0, BSWAP
*/
#define DMAC_LL_WD0_BSWAP_MASK		(0x80000000)
#define DMAC_LL_WD0_BSWAP_LSBMASK		(0x00000001)
#define DMAC_LL_WD0_BSWAP_SHIFT		(31)
#define DMAC_LL_WD0_BSWAP_LENGTH		(1)
#define DMAC_LL_WD0_BSWAP_DEFAULT		(0)
#define DMAC_LL_WD0_BSWAP_SIGNED_FIELD	(IMG_FALSE)

#define DMAC_LL_WD1_OFFSET		(0x0004)

/* DMAC_LL, WD1, LEN
*/
#define DMAC_LL_WD1_LEN_MASK		(0x0000FFFF)
#define DMAC_LL_WD1_LEN_LSBMASK		(0x0000FFFF)
#define DMAC_LL_WD1_LEN_SHIFT		(0)
#define DMAC_LL_WD1_LEN_LENGTH		(16)
#define DMAC_LL_WD1_LEN_DEFAULT		(0)
#define DMAC_LL_WD1_LEN_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD1, INCR
*/
#define DMAC_LL_WD1_INCR_MASK		(0x00010000)
#define DMAC_LL_WD1_INCR_LSBMASK		(0x00000001)
#define DMAC_LL_WD1_INCR_SHIFT		(16)
#define DMAC_LL_WD1_INCR_LENGTH		(1)
#define DMAC_LL_WD1_INCR_DEFAULT		(0)
#define DMAC_LL_WD1_INCR_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD1, PI
*/
#define DMAC_LL_WD1_PI_MASK		(0x00060000)
#define DMAC_LL_WD1_PI_LSBMASK		(0x00000003)
#define DMAC_LL_WD1_PI_SHIFT		(17)
#define DMAC_LL_WD1_PI_LENGTH		(2)
#define DMAC_LL_WD1_PI_DEFAULT		(0)
#define DMAC_LL_WD1_PI_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD1, RESERVED_02
*/
#define DMAC_LL_WD1_RESERVED_02_MASK		(0x3FF80000)
#define DMAC_LL_WD1_RESERVED_02_LSBMASK		(0x000007FF)
#define DMAC_LL_WD1_RESERVED_02_SHIFT		(19)
#define DMAC_LL_WD1_RESERVED_02_LENGTH		(11)
#define DMAC_LL_WD1_RESERVED_02_DEFAULT		(0)
#define DMAC_LL_WD1_RESERVED_02_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD1, LIST_INT
*/
#define DMAC_LL_WD1_LIST_INT_MASK		(0x40000000)
#define DMAC_LL_WD1_LIST_INT_LSBMASK		(0x00000001)
#define DMAC_LL_WD1_LIST_INT_SHIFT		(30)
#define DMAC_LL_WD1_LIST_INT_LENGTH		(1)
#define DMAC_LL_WD1_LIST_INT_DEFAULT		(0)
#define DMAC_LL_WD1_LIST_INT_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD1, LIST_FIN
*/
#define DMAC_LL_WD1_LIST_FIN_MASK		(0x80000000)
#define DMAC_LL_WD1_LIST_FIN_LSBMASK		(0x00000001)
#define DMAC_LL_WD1_LIST_FIN_SHIFT		(31)
#define DMAC_LL_WD1_LIST_FIN_LENGTH		(1)
#define DMAC_LL_WD1_LIST_FIN_DEFAULT		(0)
#define DMAC_LL_WD1_LIST_FIN_SIGNED_FIELD	(IMG_FALSE)

#define DMAC_LL_WD2_OFFSET		(0x0008)

/* DMAC_LL, WD2, ADDR
*/
#define DMAC_LL_WD2_ADDR_MASK		(0xFFFFFFFF)
#define DMAC_LL_WD2_ADDR_LSBMASK		(0xFFFFFFFF)
#define DMAC_LL_WD2_ADDR_SHIFT		(0)
#define DMAC_LL_WD2_ADDR_LENGTH		(32)
#define DMAC_LL_WD2_ADDR_DEFAULT		(0)
#define DMAC_LL_WD2_ADDR_SIGNED_FIELD	(IMG_FALSE)

#define DMAC_LL_WD3_OFFSET		(0x000C)

/* DMAC_LL, WD3, EXT_SA
*/
#define DMAC_LL_WD3_EXT_SA_MASK		(0x0000000F)
#define DMAC_LL_WD3_EXT_SA_LSBMASK		(0x0000000F)
#define DMAC_LL_WD3_EXT_SA_SHIFT		(0)
#define DMAC_LL_WD3_EXT_SA_LENGTH		(4)
#define DMAC_LL_WD3_EXT_SA_DEFAULT		(0)
#define DMAC_LL_WD3_EXT_SA_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD3, RESERVED_04
*/
#define DMAC_LL_WD3_RESERVED_04_MASK		(0x0000FFF0)
#define DMAC_LL_WD3_RESERVED_04_LSBMASK		(0x00000FFF)
#define DMAC_LL_WD3_RESERVED_04_SHIFT		(4)
#define DMAC_LL_WD3_RESERVED_04_LENGTH		(12)
#define DMAC_LL_WD3_RESERVED_04_DEFAULT		(0)
#define DMAC_LL_WD3_RESERVED_04_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD3, EXT_BURST
*/
#define DMAC_LL_WD3_EXT_BURST_MASK		(0x000F0000)
#define DMAC_LL_WD3_EXT_BURST_LSBMASK		(0x0000000F)
#define DMAC_LL_WD3_EXT_BURST_SHIFT		(16)
#define DMAC_LL_WD3_EXT_BURST_LENGTH		(4)
#define DMAC_LL_WD3_EXT_BURST_DEFAULT		(0)
#define DMAC_LL_WD3_EXT_BURST_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD3, RESERVED_03
*/
#define DMAC_LL_WD3_RESERVED_03_MASK		(0x03F00000)
#define DMAC_LL_WD3_RESERVED_03_LSBMASK		(0x0000003F)
#define DMAC_LL_WD3_RESERVED_03_SHIFT		(20)
#define DMAC_LL_WD3_RESERVED_03_LENGTH		(6)
#define DMAC_LL_WD3_RESERVED_03_DEFAULT		(0)
#define DMAC_LL_WD3_RESERVED_03_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD3, BURST
*/
#define DMAC_LL_WD3_BURST_MASK		(0x1C000000)
#define DMAC_LL_WD3_BURST_LSBMASK		(0x00000007)
#define DMAC_LL_WD3_BURST_SHIFT		(26)
#define DMAC_LL_WD3_BURST_LENGTH		(3)
#define DMAC_LL_WD3_BURST_DEFAULT		(0)
#define DMAC_LL_WD3_BURST_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD3, ACC_DEL
*/
#define DMAC_LL_WD3_ACC_DEL_MASK		(0xE0000000)
#define DMAC_LL_WD3_ACC_DEL_LSBMASK		(0x00000007)
#define DMAC_LL_WD3_ACC_DEL_SHIFT		(29)
#define DMAC_LL_WD3_ACC_DEL_LENGTH		(3)
#define DMAC_LL_WD3_ACC_DEL_DEFAULT		(0)
#define DMAC_LL_WD3_ACC_DEL_SIGNED_FIELD	(IMG_FALSE)

#define DMAC_LL_WD4_OFFSET		(0x0010)

/* DMAC_LL, WD4, REP_COUNT
*/
#define DMAC_LL_WD4_REP_COUNT_MASK		(0x000007FF)
#define DMAC_LL_WD4_REP_COUNT_LSBMASK		(0x000007FF)
#define DMAC_LL_WD4_REP_COUNT_SHIFT		(0)
#define DMAC_LL_WD4_REP_COUNT_LENGTH		(11)
#define DMAC_LL_WD4_REP_COUNT_DEFAULT		(0)
#define DMAC_LL_WD4_REP_COUNT_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD4, RESERVED_05
*/
#define DMAC_LL_WD4_RESERVED_05_MASK		(0x0000F800)
#define DMAC_LL_WD4_RESERVED_05_LSBMASK		(0x0000001F)
#define DMAC_LL_WD4_RESERVED_05_SHIFT		(11)
#define DMAC_LL_WD4_RESERVED_05_LENGTH		(5)
#define DMAC_LL_WD4_RESERVED_05_DEFAULT		(0)
#define DMAC_LL_WD4_RESERVED_05_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD4, MODE_2D
*/
#define DMAC_LL_WD4_MODE_2D_MASK		(0x00010000)
#define DMAC_LL_WD4_MODE_2D_LSBMASK		(0x00000001)
#define DMAC_LL_WD4_MODE_2D_SHIFT		(16)
#define DMAC_LL_WD4_MODE_2D_LENGTH		(1)
#define DMAC_LL_WD4_MODE_2D_DEFAULT		(0)
#define DMAC_LL_WD4_MODE_2D_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD4, RESERVED_06
*/
#define DMAC_LL_WD4_RESERVED_06_MASK		(0xFFFE0000)
#define DMAC_LL_WD4_RESERVED_06_LSBMASK		(0x00007FFF)
#define DMAC_LL_WD4_RESERVED_06_SHIFT		(17)
#define DMAC_LL_WD4_RESERVED_06_LENGTH		(15)
#define DMAC_LL_WD4_RESERVED_06_DEFAULT		(0)
#define DMAC_LL_WD4_RESERVED_06_SIGNED_FIELD	(IMG_FALSE)

#define DMAC_LL_WD5_OFFSET		(0x0014)

/* DMAC_LL, WD5, ROW_LENGTH
*/
#define DMAC_LL_WD5_ROW_LENGTH_MASK		(0x000003FF)
#define DMAC_LL_WD5_ROW_LENGTH_LSBMASK		(0x000003FF)
#define DMAC_LL_WD5_ROW_LENGTH_SHIFT		(0)
#define DMAC_LL_WD5_ROW_LENGTH_LENGTH		(10)
#define DMAC_LL_WD5_ROW_LENGTH_DEFAULT		(0)
#define DMAC_LL_WD5_ROW_LENGTH_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD5, RESERVED_07
*/
#define DMAC_LL_WD5_RESERVED_07_MASK		(0x0000FC00)
#define DMAC_LL_WD5_RESERVED_07_LSBMASK		(0x0000003F)
#define DMAC_LL_WD5_RESERVED_07_SHIFT		(10)
#define DMAC_LL_WD5_RESERVED_07_LENGTH		(6)
#define DMAC_LL_WD5_RESERVED_07_DEFAULT		(0)
#define DMAC_LL_WD5_RESERVED_07_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD5, LINE_ADD_OFF
*/
#define DMAC_LL_WD5_LINE_ADD_OFF_MASK		(0x03FF0000)
#define DMAC_LL_WD5_LINE_ADD_OFF_LSBMASK		(0x000003FF)
#define DMAC_LL_WD5_LINE_ADD_OFF_SHIFT		(16)
#define DMAC_LL_WD5_LINE_ADD_OFF_LENGTH		(10)
#define DMAC_LL_WD5_LINE_ADD_OFF_DEFAULT		(0)
#define DMAC_LL_WD5_LINE_ADD_OFF_SIGNED_FIELD	(IMG_FALSE)

/* DMAC_LL, WD5, RESERVED_08
*/
#define DMAC_LL_WD5_RESERVED_08_MASK		(0xFC000000)
#define DMAC_LL_WD5_RESERVED_08_LSBMASK		(0x0000003F)
#define DMAC_LL_WD5_RESERVED_08_SHIFT		(26)
#define DMAC_LL_WD5_RESERVED_08_LENGTH		(6)
#define DMAC_LL_WD5_RESERVED_08_DEFAULT		(0)
#define DMAC_LL_WD5_RESERVED_08_SIGNED_FIELD	(IMG_FALSE)

#define DMAC_LL_WD6_OFFSET		(0x0018)

/* DMAC_LL, WD6, SA
*/
#define DMAC_LL_WD6_SA_MASK		(0xFFFFFFFF)
#define DMAC_LL_WD6_SA_LSBMASK		(0xFFFFFFFF)
#define DMAC_LL_WD6_SA_SHIFT		(0)
#define DMAC_LL_WD6_SA_LENGTH		(32)
#define DMAC_LL_WD6_SA_DEFAULT		(0)
#define DMAC_LL_WD6_SA_SIGNED_FIELD	(IMG_FALSE)

#define DMAC_LL_WD7_OFFSET		(0x001C)

/* DMAC_LL, WD7, LISTPTR
*/
#define DMAC_LL_WD7_LISTPTR_MASK		(0xFFFFFFFF)
#define DMAC_LL_WD7_LISTPTR_LSBMASK		(0xFFFFFFFF)
#define DMAC_LL_WD7_LISTPTR_SHIFT		(0)
#define DMAC_LL_WD7_LISTPTR_LENGTH		(32)
#define DMAC_LL_WD7_LISTPTR_DEFAULT		(0)
#define DMAC_LL_WD7_LISTPTR_SIGNED_FIELD	(IMG_FALSE)

/*
 * size of the memory structure DMAC_LL
 * Computed from WD7 offset and size
 */
#define DMAC_LL_BYTE_SIZE		(0x0020)


#ifdef __cplusplus
}
#endif

#endif /* __IMG_SOC_DMAC_LINKED_LIST_IO2_H__ */
