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

 
 #ifndef LLF_ECPKI_ERROR_H
#define LLF_ECPKI_ERROR_H
/*
 *  Object % LLF_ECPKI_error.h    : %
 *  State           :  %state%
 *  Creation date   :  08/12/2005
 *  Last modified   :  %modify_time%
 */
/** @file
 *  \brief The error definitions of the LLF AES module
 *
 *  \version CRYS_ECPKI_error.h#1:incl:1
 *  \author R.Levin
 */

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "crys_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

/************************************    Defines     ************************************************/

/* base address: LLF_ECPKI_MODULE_ERROR_BASE = 0x00F10800 */

/****************************************************************************************************
 * LLF ECPKI COMMON ERRORS
 ****************************************************************************************************/
#define LLF_ECPKI_EC_FIELD_NOT_SUPPORTED                        (LLF_ECPKI_MODULE_ERROR_BASE + 0x00UL)
#define LLF_ECPKI_INIT_PKA_INVALID_PTR_ERROR                    (LLF_ECPKI_MODULE_ERROR_BASE + 0x01UL)
#define LLF_ECPKI_INVALID_POINT_ID_ERROR                        (LLF_ECPKI_MODULE_ERROR_BASE + 0x02UL) 
#define LLF_ECPKI_INVALID_SCALAR_IN_POINT_MULT_ERROR            (LLF_ECPKI_MODULE_ERROR_BASE + 0x03UL)  


/****************************************************************************************************
 * LLF ECPKI BUILD  ERRORS
 ****************************************************************************************************/
#define LLF_ECPKI_DOMAIN_INFO_INVALID_PRIV_KEY_DATA_ERROR       (LLF_ECPKI_MODULE_ERROR_BASE + 0x11UL)
#define LLF_ECPKI_DOMAIN_INFO_INVALID_PRIV_KEY_SIZE_ERROR       (LLF_ECPKI_MODULE_ERROR_BASE + 0x12UL)
#define LLF_ECPKI_DOMAIN_INFO_INVALID_PUBL_KEY_DATA_ERROR       (LLF_ECPKI_MODULE_ERROR_BASE + 0x13UL)
#define LLF_ECPKI_DOMAIN_INFO_INVALID_PUBL_KEY_SIZE_ERROR       (LLF_ECPKI_MODULE_ERROR_BASE + 0x14UL)
#define LLF_ECPKI_DOMAIN_INFO_INVALID_DOMAIN_ID_ERROR           (LLF_ECPKI_MODULE_ERROR_BASE + 0x15UL)
#define LLF_ECPKI_DOMAIN_INFO_EMPTY_DOMAIN_NOT_ALLOWED_ERROR    (LLF_ECPKI_MODULE_ERROR_BASE + 0x16UL)
#define LLF_ECPKI_DOMAIN_INFO_PUBL_KEY_ON_INFINITY_ERROR        (LLF_ECPKI_MODULE_ERROR_BASE + 0x17UL)



/****************************************************************************************************
 * LLF ECDSA MODULE ERRORS
 ****************************************************************************************************/
/* The LLF_ECDSA_VerifyCalc  function errors */
#define	 LLF_ECDSA_SIGN_CALC_CANNOT_CREATE_SIGNATURE 	        (LLF_ECPKI_MODULE_ERROR_BASE + 0x20UL)
#define	 LLF_ECDSA_VERIFY_CALC_SIGN_C_INVALID_ERROR    	        (LLF_ECPKI_MODULE_ERROR_BASE + 0x21UL)
#define	 LLF_ECDSA_VERIFY_CALC_SIGN_D_INVALID_ERROR    	        (LLF_ECPKI_MODULE_ERROR_BASE + 0x22UL)
#define	 LLF_ECDSA_VERIFY_CALC_POINT_ON_INFINITY_ERROR 	        (LLF_ECPKI_MODULE_ERROR_BASE + 0x23UL)
#define	 LLF_ECDSA_VERIFY_CALC_SIGNATURE_IS_INVALID 	        (LLF_ECPKI_MODULE_ERROR_BASE + 0x24UL)


/****************************************************************************************************
 * LLF ECPKI ELGAMAL  MODULE ERRORS
 ****************************************************************************************************/

/* The LLF ECPKI ELGAMAL Encrypt function errors */
#define LLF_ECPKI_ELGAM_ENCR_RECEIVER_PUBL_KEY_IS_INVALID_ERROR    (LLF_ECPKI_MODULE_ERROR_BASE + 0x30UL)
	
/* The LLF ECPKI ELGAMAL Decrypt function errors */ 
#define LLF_ECPKI_ELGAMAL_DECR_SENDER_PUBL_KEY_V_IS_INVALID_ERROR  (LLF_ECPKI_MODULE_ERROR_BASE + 0x38UL)


/****************************************************************************************************
 * LLF ECPKI Diffie-Hellman  MODULE ERRORS
 ****************************************************************************************************/

/* The LLF_ECPKI_SVDP_DH function errors */
#define LLF_ECPKI_SVDP_DH_SHARED_VALUE_IS_ON_INFINITY_ERROR	       (LLF_ECPKI_MODULE_ERROR_BASE + 0x41UL)
#define LLF_ECPKI_SHIFT_ARR_WORD_FALSE_DIRECTION_ERROR             (LLF_ECPKI_MODULE_ERROR_BASE + 0x42UL)
#define LLF_ECPKI_SHIFT_WORD_ARRAY_SHIFT_ERROR                     (LLF_ECPKI_MODULE_ERROR_BASE + 0x43UL)

/****************************************************************************************************
 * LLF ECPKI Arithmetic  MODULE ERRORS
 ****************************************************************************************************/

/* The LLF_ECPKI_GFpAddMont function errors */
#define LLF_ECPKI_GFP_ADD_MONT_INVALID_DOMAIN_ID_ERROR             (LLF_ECPKI_MODULE_ERROR_BASE + 0x50UL)
#define LLF_ECPKI_GFP_ADD_MONT_INVALID_POINTS_ID_ERROR             (LLF_ECPKI_MODULE_ERROR_BASE + 0x51UL)

/* The LLF_ECPKI_GFpDoubleMont function errors */
#define LLF_ECPKI_GFP_DOUBLE_INVALID_DOMAIN_ID_ERROR               (LLF_ECPKI_MODULE_ERROR_BASE + 0x58UL)
#define LLF_ECPKI_GFP_DOUBLE_INVALID_POINTS_ID_ERROR               (LLF_ECPKI_MODULE_ERROR_BASE + 0x59UL)

/* The LLF_ECPKI_FullAdd function errors */
#define LLF_ECPKI_GFP_FULL_ADD_INVALID_DOMAIN_ID_ERROR		       (LLF_ECPKI_MODULE_ERROR_BASE + 0x60UL)
#define LLF_ECPKI_GFP_FULL_ADD_POINTS_NOT_JACOBIAN_ERROR           (LLF_ECPKI_MODULE_ERROR_BASE + 0x61UL)
#define LLF_ECPKI_GFP_FULL_ADD_ARITHM_OPERATIONS_OFF_MODE_ERROR    (LLF_ECPKI_MODULE_ERROR_BASE + 0x62UL)

/* The LLF_ECPKI_SubtractPoints function errors */
#define LLF_ECPKI_SUBTRACT_POINTS_INVALID_DOMAIN_ID_ERROR          (LLF_ECPKI_MODULE_ERROR_BASE + 0x68UL)						   
#define LLF_ECPKI_SUBTRACT_POINTS_NOT_JACOBIAN_ERROR               (LLF_ECPKI_MODULE_ERROR_BASE + 0x69UL)
#define LLF_ECPKI_SUBTRACT_POINTS_ARITHM_OPERATIONS_OFF_MODE_ERROR (LLF_ECPKI_MODULE_ERROR_BASE + 0x6AUL)

/* The LLF_ECPKI_ScalarMult function errors */ 
#define LLF_ECPKI_SCALAR_MULT_INVALID_DOMAIN_ID_ERROR              (LLF_ECPKI_MODULE_ERROR_BASE + 0x70UL)
#define LLF_ECPKI_SCALAR_MULT_INVALID_SCALAR_SIGN_ERROR            (LLF_ECPKI_MODULE_ERROR_BASE + 0x71UL)
#define LLF_ECPKI_SCALAR_MULT_INVALID_POINTS_ID_ERROR              (LLF_ECPKI_MODULE_ERROR_BASE + 0x72UL)
#define LLF_ECPKI_SCALAR_MULT_ARITHM_OPERATIONS_OFF_MODE_ERROR     (LLF_ECPKI_MODULE_ERROR_BASE + 0x73UL)  
#define LLF_ECPKI_SCALAR_MULT_BARRET_REDUCT_NOT_SUPPORTED_ERROR    (LLF_ECPKI_MODULE_ERROR_BASE + 0x74UL)  
#define LLF_ECPKI_SCALAR_MULT_POINTS_NOT_JACOBIAN_ERROR            (LLF_ECPKI_MODULE_ERROR_BASE + 0x75UL)

/* The LLF_ECPKI_CONVERT ARRAY TO EC POINT AFFINE function errors */
#define LLF_ECPKI_CONV_TO_EC_AFFINE_CAN_NOT_CONVERT_ERROR          (LLF_ECPKI_MODULE_ERROR_BASE + 0x78UL)

/* The LLF_ECPKI_IsPointOnCurve function errors */ 
#define LLF_ECPKI_BARRET_REDUCT_NOT_SUPPORTED_ERROR                (LLF_ECPKI_MODULE_ERROR_BASE + 0x7AUL)
#define LLF_ECPKI_EC_POINT_IS_NOT_ON_CURVE_ERROR                   (LLF_ECPKI_MODULE_ERROR_BASE + 0x7BUL)


/*****************************************************************************************************/

/************************ Enums ********************************/


/************************ Typedefs  ****************************/


/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/

#ifdef __cplusplus
}
#endif

#endif

