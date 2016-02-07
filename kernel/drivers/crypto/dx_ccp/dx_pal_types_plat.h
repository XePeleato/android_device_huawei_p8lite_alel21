/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This software is protected by copyright, international           *
* treaties and patents, and distributed under multiple licenses.   *
* Any use of this Software as part of the Discretix CryptoCell or  *
* Packet Engine products requires a commercial license.            *
* Copies of this Software that are distributed with the Discretix  *
* CryptoCell or Packet Engine product drivers, may be used in      *
* accordance with a commercial license, or at the user's option,   *
* used and redistributed under the terms and conditions of the GNU *
* General Public License ("GPL") version 2, as published by the    *
* Free Software Foundation.                                        *
* This program is distributed in the hope that it will be useful,  *
* but WITHOUT ANY LIABILITY AND WARRANTY; without even the implied *
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. *
* See the GNU General Public License version 2 for more details.   *
* You should have received a copy of the GNU General Public        *
* License version 2 along with this Software; if not, please write *
* to the Free Software Foundation, Inc., 59 Temple Place - Suite   *
* 330, Boston, MA 02111-1307, USA.                                 *
* Any copy or reproduction of this Software, as permitted under    *
* the GNU General Public License version 2, must include this      *
* Copyright Notice as well as any other notices provided under     *
* the said license.                                                *
********************************************************************/


#ifndef DX_PAL_TYPES_PLAT_H
#define DX_PAL_TYPES_PLAT_H
/* Linux kernel types */

#include <linux/types.h>

#define DX_NULL NULL

typedef unsigned int            DxUint_t;
typedef uint8_t                 DxUint8_t;
typedef uint16_t                DxUint16_t;
typedef uint32_t                DxUint32_t;
typedef uint64_t                DxUint64_t;

typedef int			DxInt_t;
typedef int8_t                  DxInt8_t;
typedef int16_t                 DxInt16_t;
typedef int32_t                 DxInt32_t;
typedef int64_t	                DxInt64_t;

typedef char                    DxChar_t;
typedef short                   DxWideChar_t;

#endif /*DX_PAL_TYPES_PLAT_H*/
