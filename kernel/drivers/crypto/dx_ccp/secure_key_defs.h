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


#ifndef __SECURE_KEY_DEFS_H__
#define __SECURE_KEY_DEFS_H__


/******************************************************************************
*				DEFINITIONS
******************************************************************************/

#define DX_SECURE_KEY_PACKAGE_BUF_SIZE_IN_BYTES		88

#define DX_SECURE_KEY_NONCE_SIZE_IN_BYTES	12

#define DX_SECURE_KEY_MULTI2_MIN_ROUNDS		8
#define DX_SECURE_KEY_MULTI2_MAX_ROUNDS		128


/******************************************************************************
*				TYPE DEFINITIONS
******************************************************************************/
enum secure_key_type {
	DX_SECURE_KEY_AES_KEY128  = 0,
	DX_SECURE_KEY_AES_KEY256  = 1,
	DX_SECURE_KEY_MULTI2	  = 2,
	DX_SECURE_KEY_MAINTENANCE  = 7,
	DX_SECURE_KEY_KSIZE_RESERVE32B = INT32_MAX
};

enum secure_key_cipher_mode {
	DX_SECURE_KEY_CIPHER_CBC = 1,
	DX_SECURE_KEY_CIPHER_CTR = 2,
	DX_SECURE_KEY_CIPHER_OFB = 6,
	DX_SECURE_KEY_CIPHER_CBC_CTS = 11,
	DX_SECURE_KEY_CIPHER_RESERVE32B = INT32_MAX
};

enum secure_key_direction {
	DX_SECURE_KEY_DIRECTION_ENCRYPT = 0,
	DX_SECURE_KEY_DIRECTION_DECRYPT = 1,
	DX_SECURE_KEY_DIRECTION_RESERVE32B = INT32_MAX
};


typedef uint8_t skeyPackageBuf_t[DX_SECURE_KEY_PACKAGE_BUF_SIZE_IN_BYTES];

typedef uint8_t skeyNonceBuf_t[DX_SECURE_KEY_NONCE_SIZE_IN_BYTES];


#endif /*__SECURE_KEY_DEFS_H__*/
