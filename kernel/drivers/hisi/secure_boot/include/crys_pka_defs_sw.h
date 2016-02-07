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


#ifndef _CRYS_PKA_DEFS_H_
#define _CRYS_PKA_DEFS_H_


#define CRYS_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS 5

/*The size CRYS_PKA_RSA_HASH_CTX_SIZE_IN_WORDS should ALWAYS be >= CRYS_HASH_USER_CTX_SIZE_IN_WORDS (with the value of the SEP side)*/
#define CRYS_PKA_RSA_HASH_CTX_SIZE_IN_WORDS 33

/* size of buffer for Barrett modulus tag NP, used in PKI algorithms */
#define CRYS_PKA_ECPKI_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS  CRYS_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS   

#define CRYS_PKA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS

#define CRYS_PKA_DOMAIN_BUFF_SIZE_IN_WORDS (2*CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS +1)

#define CRYS_PKA_INT_PUB_KEY_DB_BUFF_SIZE_IN_WORDS 1	

#define CRYS_PKA_PUB_KEY_BUFF_SIZE_IN_WORDS 2*CRYS_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS

#define CRYS_PKA_PRIV_KEY_BUFF_SIZE_IN_WORDS 1

#define CRYS_PKA_KGDATA_BUFF_SIZE_IN_WORDS   2*CRYS_PKA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS

#define CRYS_PKA_ECDH_BUFF_MAX_LENGTH_IN_WORDS (CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS + 1 + 36*CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS + 2*CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS)

#define CRYS_PKA_RSA_PRIME_INT_BUFF_SIZE_IN_WORDS 1

#define CRYS_PKA_CONV_KEY_CRT_BUFF_SIZE_IN_WORDS 1

#define CRYS_PKA_ECPKI_TMP_BUFF_MAX_LENGTH_IN_WORDS 1

#define CRYS_PKA_RND_ENTR_ESTIM_BUFF_MAX_LENGTH_IN_WORDS 514

/*! The valid maximum EC modulus size in 32-bit words */
#define CRYS_PKA_ECPKI_MODUL_MAX_LENGTH_IN_WORDS  18 /*(CRYS_ECPKI_MODUL_MAX_LENGTH_IN_BITS + 31)/(sizeof(DxUint32_t))*/

#define CRYS_PKA_KG_BUFF_MAX_LENGTH_IN_WORDS (40*CRYS_PKA_ECPKI_MODUL_MAX_LENGTH_IN_WORDS+4)

#define CRYS_PKA_EL_GAMAL_BUFF_MAX_LENGTH_IN_WORDS (CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS*30+5+ECPKI_EXP_TEMP_BUFFER_SIZE_IN_WORDS)

#define CRYS_PKA_ECDSA_SIGNE_BUFF_MAX_LENGTH_IN_WORDS (45*CRYS_PKA_ECPKI_MODUL_MAX_LENGTH_IN_WORDS)

#define CRYS_PKA_ECDSA_VERIFY_BUFF_MAX_LENGTH_IN_WORDS (45*CRYS_PKA_ECPKI_MODUL_MAX_LENGTH_IN_WORDS)

/************************************************************************/
/* the following defintions are only relevant for RSA code on SW */
/************************************************************************/

/* Define the maximal allowed width of the exponentiation sliding window 
in range 2...6. This define is actual for projects on soft platform.
To minimize code size use the minimum value. To optimize performance 
choose the maximum value */
#define PKI_EXP_SLIDING_WINDOW_MAX_VALUE  2

/* Define the size of the exponentiation temp buffer, used in LLF_PKI exponentiation and NON DEPENDED on
   width of the sliding window. The size defined in units equaled to maximal RSA modulus size */
#define PKI_EXP_CONST_TEMP_BUFF_SIZE_IN_MODULUS_UNITS  7

/* Define the size of the exponentiation temp buffer, used in LLF_PKI and NON DEPENDED on
width of the sliding window. The size defined in units equaled to maximal RSA modulus size */
#define PKI_CONV_CRT_CONST_TEMP_BUFF_SIZE_IN_MODULUS_UNITS  16

/**************  Calculation of buffers sizes in words *******************************/

/* Size of buffers for sliding window exponents */
#if (PKI_EXP_SLIDING_WINDOW_MAX_VALUE == 6)
#define PKI_EXP_WINDOW_TEMP_BUFFER_SIZE_IN_MODULUS_UNITS  34
#else
#define PKI_EXP_WINDOW_TEMP_BUFFER_SIZE_IN_MODULUS_UNITS  (3 + (1 << (PKI_EXP_SLIDING_WINDOW_MAX_VALUE-1)))
#endif

/* Define the size of the temp buffer, used in LLF_PKI exponentiation and DEPENDED on
   width of the sliding window in words */
#if (PKI_EXP_CONST_TEMP_BUFF_SIZE_IN_MODULUS_UNITS > PKI_EXP_WINDOW_TEMP_BUFFER_SIZE_IN_MODULUS_UNITS )
#define PKI_EXP_TEMP_BUFFER_SIZE_IN_WORDS  \
	(PKI_EXP_CONST_TEMP_BUFF_SIZE_IN_MODULUS_UNITS * CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS + 2 )
#else
#define PKI_EXP_TEMP_BUFFER_SIZE_IN_WORDS  \
	(PKI_EXP_WINDOW_TEMP_BUFFER_SIZE_IN_MODULUS_UNITS * CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS + 2 )
#endif


/* Define the size of the temp buffer, used in LLF_PKI_CONVERT_TO_CRT and DEPENDED on
   width of the sliding window in words */
#if (PKI_CONV_CRT_CONST_TEMP_BUFF_SIZE_IN_MODULUS_UNITS > PKI_EXP_WINDOW_TEMP_BUFFER_SIZE_IN_MODULUS_UNITS )
#define PKI_CONV_CRT_TEMP_BUFFER_SIZE_IN_WORDS  \
	(PKI_CONV_CRT_CONST_TEMP_BUFF_SIZE_IN_MODULUS_UNITS * CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS + 2 )
#else
#define PKI_CONV_CRT_TEMP_BUFFER_SIZE_IN_WORDS  \
	(PKI_EXP_WINDOW_TEMP_BUFFER_SIZE_IN_MODULUS_UNITS * CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS + 2 )
#endif

/* definition of PKI_KEY_GEN_TEMP_BUFF_SIZE_WORDS IS DEPENDED on width of the sliding window*/
#if( PKI_EXP_SLIDING_WINDOW_MAX_VALUE > 2 )
#define PKI_KEY_GEN_TEMP_BUFF_SIZE_WORDS \
	((4 + (1<<(PKI_EXP_SLIDING_WINDOW_MAX_VALUE-2))) * CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS)
#else
#define PKI_KEY_GEN_TEMP_BUFF_SIZE_WORDS \
	(16 * CRYS_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS)
#endif

/************************************************************************/
/* the following defintions are only relevant for ECC code on SW */
/************************************************************************/
/* Define the maximal allowed width of the exponentiation sliding window 
in range 1...6 for ECC algorithms. This define is actual for projects on 
soft platform. To minimize code size use the minimum value. To optimize 
the performance choose the maximum value */
#define ECPKI_EXP_SLIDING_WINDOW_MAX_VALUE  PKI_EXP_SLIDING_WINDOW_MAX_VALUE

/* Define the size of the exponentiation temp buffer, used in LLF_PKI exponentiation and NON DEPENDED on
   width of the sliding window. The size defined in units equaled to maximal RSA modulus size */
#define ECPKI_EXP_CONST_TEMP_BUFF_SIZE_IN_MODULUS_UNITS  PKI_EXP_CONST_TEMP_BUFF_SIZE_IN_MODULUS_UNITS


/**************  Calculation of buffers sizes in words *******************************/

/* Size of buffer for precomputation of sliding window exponents */
#if (ECPKI_EXP_SLIDING_WINDOW_MAX_VALUE == 6)
#define ECPKI_EXP_WINDOW_TEMP_BUFFER_SIZE_IN_MODULUS_UNITS  34
#else
#define ECPKI_EXP_WINDOW_TEMP_BUFFER_SIZE_IN_MODULUS_UNITS  (3 + (1 << (ECPKI_EXP_SLIDING_WINDOW_MAX_VALUE-1)))
#endif

/* Define the size of the temp buffer, used in LLF_PKI exponentiation and DEPENDED on
   width of the sliding window in words */
#if (ECPKI_EXP_CONST_TEMP_BUFF_SIZE_IN_MODULUS_UNITS > ECPKI_EXP_WINDOW_TEMP_BUFFER_SIZE_IN_MODULUS_UNITS )
#define ECPKI_EXP_TEMP_BUFFER_SIZE_IN_WORDS  \
	(ECPKI_EXP_CONST_TEMP_BUFF_SIZE_IN_MODULUS_UNITS * CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS + 2 )
#else
#define ECPKI_EXP_TEMP_BUFFER_SIZE_IN_WORDS  \
	(ECPKI_EXP_WINDOW_TEMP_BUFFER_SIZE_IN_MODULUS_UNITS * CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS + 2 )
#endif


#endif /*_CRYS_PKA_DEFS_SW_H_*/
