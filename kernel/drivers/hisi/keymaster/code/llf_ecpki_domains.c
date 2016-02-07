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

 
 
/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "dx_vos_basetypes.h"
#include "crys_hash.h"
#include "crys_ecpki_types.h"

/* canceling the lint warning:
   Info 651: Potentially confusing initializer */
/*lint --e{651} */
/* canceling the lint warning:
   Info 785: Too few initializers for aggregate */
/*lint --e{785} */

  
/**************** The domain structure describing *************/ 	
/**  
       The elliptic curve EC: Y^2 = X^3 + A*X + B over prime fild GFp is defined by
	   following domain structure:
		   
	    typedef  const  struct {  	
			/ Field modulus:  GF_Modulus = P/ 
			DxUint32_t	GF_Modulus [CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
			/ EC equation parameters a, b /
			DxUint32_t		EC_Param_A [CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
			DxUint32_t		EC_Param_B [CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
			/ Order of generator: EC_GenerOrder/
			DxUint32_t		EC_GenerOrder [CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
			/ Generator (EC base point) coordinates in projective form  /
			DxUint32_t		EC_Gener_X [CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
			DxUint32_t		EC_Gener_Y [CRYS_ECPKI_MODUL_MAX_LENGTH_IN_WORDS];
			/ EC cofactor EC_Cofactor_K /
			DxUint32_t      EC_Cofactor_K;
		    / Size of field modulus  in bits and in words /
			DxUint16_t      EC_ModulusSizeInBits;
		    DxUint16_t      EC_ModulusSizeInWords;
		    / Sizes of order of generator in bits and in words /
			DxUint16_t      EC_OrderSizeInBits;
			DxUint16_t  	EC_OrderSizeInWords;

		    / include the specific fields that are used by the low level:
		      Barrett tags NP,RP /
		    #include "LLF_ECPKI_Domains_db_def.h" 
		    
			/ EC Domain identifier /
			CRYS_ECPKI_DomainID_t	DomainID;
			/ EC finite field identifier /
			CRYS_ECPKI_FieldID_t	FieldID;

		}  CRYS_ECPKI_Domain_t ; 
*/
	
	
/***********************************************************************************
 *   Data base of EC Domains:  Array of structures of type  CRYS_ECPKI_Domain_t    *
 *       All data is given in little endian order of words in arrays               *
 ***********************************************************************************/

 const CRYS_ECPKI_Domain_t LLF_EC_DOMAINS_ARRAY [] = {

	 /******  EC Domain: CRYS_ECPKI_DomainID_secp160k1 = 0  ****/
 {
	/* Field modulus :  GF_Modulus = P = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFAC73*/ 
	{0xFFFFAC73,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
	/* EC equation parameters a, b  */
	/* a =  0  - big end  from SEC2 */
	{0x00000000},
	/* b = 7  - big end  from SEC2 */
	{0x00000007},
	/* Order of generator: 100000000000000000001B8FA16DFAB9ACA16B6B3  - big end  from SEC2 */
	{0xCA16B6B3,0x16DFAB9A,0x0001B8FA,0x00000000,0x00000000,0x00000001},

	/* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
	/* X: 3B4C382CE37AA192A4019E763036F4F5DD4D7EBB - big end  from SEC2 */
	{0xDD4D7EBB,0x3036F4F5,0xA4019E76,0xE37AA192,0x3B4C382C},  
	/* Y: 938CF935318FDCED6BC28286531733C3F03C4FEE   Y - big end  from SEC2 */
	{0xF03C4FEE,0x531733C3,0x6BC28286,0x318FDCED,0x938CF935}, 

	1, /* EC cofactor K */	
	/* EC cofactor inverse EC_CofactorInv !!!!!!  Not inserted   !!!! */
	{0x00000000},
	
	160, /* Size of field modulus in bits */
	5,   /* Size of field modulus in words*/
	161, /* Size of generator in bits */
	6,   /* Size of generator in words*/
    
	/* Barrett tags NP,RP */
	{0x00000010,0x00000000,0x00000000,0x00000000,0x00000010,
	0x240A8CA6,0xFFC8E0BD,0xFFFFFFFF,0xFFFFFFFF,0x0000001F},	
	
	CRYS_ECPKI_DomainID_secp160k1,  /* EC Domain identifier - enum */
	GFp    /* EC finite field identifier - enum */   
},

	/****** EC Domain: CRYS_ECPKI_DomainID_secp160r1 = 1  ****/
{	
	/* Field modulus :  GF_Modulus = P = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFFFF */ 
	{0x7FFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x00000000},
	/* EC equation parameters a, b  */
	/* a = -3 = FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF 7FFFFFFC  - big end  from SEC2 */
	{0x7FFFFFFC,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x00000000},
	/* b = 1C97BEFC 54BD7A8B 65ACF89F 81D4D4AD C565FA45  - big end  from SEC2 */
	{0xC565FA45,0x81D4D4AD,0x65ACF89F,0x54BD7A8B,0x1C97BEFC,0x00000000},
	{0xCA752257,0xF927AED3,0x0001F4C8,0x00000000,0x00000000,0x00000001,0x00000000},

	/* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
	  /* X - liitle end */   /* 4A96B568 8EF57328 46646989 68C38BB9 13CBFC82   X - big end  from SEC2 */
	{0x13CBFC82,0x68C38BB9,0x46646989,0x8EF57328,0x4A96B568,0x00000000},  
	  /* Y - liitle end */   /* 23A62855 3168947D 59DCC912 04235137 7AC5FB32   Y - big end  from SEC2 */
	{0x7AC5FB32,0x04235137,0x59DCC912,0x3168947D,0x23A62855,0x00000000}, 

	1, /* EC cofactor K */	
	/* EC cofactor inverse EC_CofactorInv !!!!!!  Not inserted   !!!! */
	{0x00000000},
	
	160, /* Size of field modulus in bits */
	5,   /* Size of field modulus in words*/
	161, /* Size of generator in bits */
	6,   /* Size of generator in words*/
	
	
	/* Barrett tags NP,RP */
	{0x00000008,0x00000000,0x00000000,0x00000000,0x00000010,
	0xDB0A2586,0xFFC166E0,0xFFFFFFFF,0xFFFFFFFF,0x0000001F},
	
	
	CRYS_ECPKI_DomainID_secp160r1,  /* EC Domain identifier - enum */
	GFp    /* EC finite field identifier - enum */
   
},

	/****** EC Domain: CRYS_ECPKI_DomainID_secp160r2 = 2   ****/

{	/* Field modulus :  GF_Modulus = P = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFAC73*/ 
	{0xFFFFAC73,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
	/* EC equation parameters a, b  */
	/* a = -3 = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFAC70 = 0  - big end  from SEC2 */
	{0xFFFFAC70,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
	/* b = B4E134D3FB59EB8BAB57274904664D5AF50388BA  - big end  from SEC2 */
	{0xF50388BA,0x04664D5A,0xAB572749,0xFB59EB8B,0xB4E134D3},
	/* Order of generator: 100000000000000000000351EE786A818F3A1A16B  - big end  from SEC2 */
	{0xF3A1A16B,0xE786A818,0x0000351E,0x00000000,0x00000000,0x00000001},

	/* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
	/* X: 52DCB034293A117E1F4FF11B30F7199D3144CE6D - big end  from SEC2 */
	{0x3144CE6D,0x30F7199D,0x1F4FF11B,0x293A117E,0x52DCB034},  
	/* Y: FEAFFEF2E331F296E071FA0DF9982CFEA7D43F2E   Y - big end  from SEC2 */
	{0xA7D43F2E,0xF9982CFE,0xE071FA0D,0xE331F296,0xFEAFFEF2}, 

	1, /* EC cofactor K */	
	/* EC cofactor inverse EC_CofactorInv !!!!!!  Not inserted   !!!! */
	{0x00000000},
	
	160, /* Size of field modulus in bits */
	5,   /* Size of field modulus in words*/
	161, /* Size of generator in bits */
	6,   /* Size of generator in words*/
	

	/* Barrett tags NP,RP */
	{0x00000010,0x00000000,0x00000000,0x00000000,0x00000010,
	0x0F2AFCE1,0xFFF95C23,0xFFFFFFFF,0xFFFFFFFF,0x0000001F},
	
	CRYS_ECPKI_DomainID_secp160r2,  /* EC Domain identifier - enum */
	GFp    /* EC finite field identifier - enum */   
},

	/****** EC Domain: CRYS_ECPKI_DomainID_secp192k1 = 3   ****/
	{	
	/* Field modulus :  GF_Modulus =  FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFEE37 - big end*/ 
	{0xFFFFEE37,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
	/* EC equation parameters a, b  */
	/* a = 0  - big end  from SEC2 */
	{0x0},
	/* b = 3 - big end  from SEC2 */	
	{0x3},
	/* Order of generator: FFFFFFFFFFFFFFFFFFFFFFFE26F2FC170F69466A74DEFD8D big end  from SEC2 */
	{0x74DEFD8D,0x0F69466A,0x26F2FC17,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF},
	/* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
	/* DB4FF10EC057E9AE26B07D0280B7F4341DA5D1B1EAE06C7D   X - big end  from SEC2 */
	{0xEAE06C7D,0x1DA5D1B1,0x80B7F434,0x26B07D02,0xC057E9AE,0xDB4FF10E},  
	/* 9B2F2F6D9C5628A7844163D015BE86344082AA88D95E2F9D  Y - big end  from SEC2 */
	{0xD95E2F9D,0x4082AA88,0x15BE8634,0x844163D0,0x9C5628A7,0x9B2F2F6D}, 
	1, /* EC cofactor K */	
	/* EC cofactor inverse EC_CofactorInv !!!!!!  Not inserted   !!!! */
	{0x00000000},	
	192, /* Size of field modulus in bits */
	6,   /* Size of field modulus in words*/
	192, /* Size of order of generator in bits */
	6,   /* Size of order of generator in words*/	


	/* Barrett tags NP,RP */
	{0x00000000,0x00000000,0x00000000,0x00000000,0x00000010,
	0x90D03E8F,0x0000001D,0x00000000,0x00000000,0x00000010},
    
	CRYS_ECPKI_DomainID_secp192k1,  /* EC Domain identifier - enum */
	GFp    /* EC finite field identifier - enum */   
},

	/****** EC Domain: CRYS_ECPKI_DomainID_secp192r1 = 4  ****/
	{	
	/* Field modulus :  GF_Modulus =  FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF - big end*/ 
	{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
	/* EC equation parameters a, b  */
	/* a = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFC  - big end  from SEC2 */
	{0xFFFFFFFC,0xFFFFFFFF,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
	/* b = 64210519E59C80E70FA7E9AB72243049FEB8DEECC146B9B1 - big end  from SEC2 */
	{0xC146B9B1,0xFEB8DEEC,0x72243049,0x0FA7E9AB,0xE59C80E7,0x64210519},

	/* Order of generator: FFFFFFFFFFFFFFFFFFFFFFFE26F2FC170F69466A74DEFD8D big end  from SEC2 */
	{0xB4D22831,0x146BC9B1,0x99DEF836,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},

	/* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
	/* 188DA80EB03090F67CBF20EB43A18800F4FF0AFD82FF1012   X - big end  from SEC2 */
	{0x82FF1012,0xF4FF0AFD,0x43A18800,0x7CBF20EB,0xB03090F6,0x188DA80E},  
	/* 7192B95FFC8DA78631011ED6B24CDD573F977A11E794811  Y - big end  from SEC2 */
	{0x1E794811,0x73F977A1,0x6B24CDD5,0x631011ED,0xFFC8DA78,0x07192B95}, 

	1, /* EC cofactor K */	
	/* EC cofactor inverse EC_CofactorInv !!!!!!  Not inserted   !!!! */
	{0x00000000},
	
	192, /* Size of field modulus in bits */
	6,   /* Size of field modulus in words*/
	192, /* Size of order of generator in bits */
	6,   /* Size of order of generator in words*/
	
	
	/* Barrett tags NP,RP */
	{0x00000010,0x00000000,0x00000000,0x00000000,0x00000010,
	0x62107C9E,0x00000006,0x00000000,0x00000000,0x00000010},
	
	CRYS_ECPKI_DomainID_secp192r1,  /* EC Domain identifier - enum */
	GFp    /* EC finite field identifier - enum */   
},
 
	/****** EC Domain: CRYS_ECPKI_DomainID_secp224k1 = 5   ****/
	{	
	/* Field modulus :  GF_Modulus =  FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFE56D - big end*/ 
	{0xFFFFE56D,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
	/* EC equation parameters a, b  */
	/* a = -0  - big end  from SEC2 */
	{0},
	/* b = 5 - big end  from SEC2 */
	{5},
	/* Order of generator: 10000000000000000000000000001DCE8D2EC6184CAF0A971769FB1F7 big end  from SEC2 */
	{0x769FB1F7,0xCAF0A971,0xD2EC6184,0x0001DCE8,0x00000000,0x00000000,0x00000000,0x00000001},

	/* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
	/* A1455B334DF099DF30FC28A169A467E9E47075A90F7E650EB6B7A45C   X - big end  from SEC2 */
	{0xB6B7A45C,0x0F7E650E,0xE47075A9,0x69A467E9,0x30FC28A1,0x4DF099DF,0xA1455B33},  
	 /* 7E089FED7FBA344282CAFBD6F7E319F7C0B0BD59E2CA4BDB556D61A5  Y - big end  from SEC2 */
	{0x556D61A5,0xE2CA4BDB,0xC0B0BD59,0xF7E319F7,0x82CAFBD6,0x7FBA3442,0x7E089FED}, 

	1, /* EC cofactor K */	
	/* EC cofactor inverse EC_CofactorInv !!!!!!  Not inserted   !!!! */
	{0x00000000},
	
	224, /* Size of field modulus in bits */
	7,   /* Size of field modulus in words*/
	225, /* Size of order of generator in bits */
	8,   /* Size of order of generator in words*/
	
	
	/* Barrett tags NP,RP */
	{0x00000000,0x00000000,0x00000000,0x00000000,0x00000010,
	0xFFC462E5,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x0000001F},
	
	CRYS_ECPKI_DomainID_secp224k1,  /* EC Domain identifier - enum */
	GFp    /* EC finite field identifier - enum */   
},

	/****** EC Domain: CRYS_ECPKI_DomainID_secp224r1 = 6 ****/
	{	
	/* Field modulus :  GF_Modulus =  FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000001  - big end*/ 
	{0x00000001,0x00000000,0x00000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
	/* EC equation parameters a, b  */
	/* a = -3 = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFE  - big end  from SEC2 */
	{0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
	/* b = B4050A850C04B3ABF54132565044B0B7D7BFD8BA270B39432355FFB4 - big end  from SEC2 */
	{0x2355FFB4,0x270B3943,0xD7BFD8BA,0x5044B0B7,0xF5413256,0x0C04B3AB,0xB4050A85},

	/* Order of generator: ord= FFFFFFFFFFFFFFFFFFFFFFFFFFFF16A2E0B8F03E13DD29455C5C2A3D big end  from SEC2 */
	{0x5C5C2A3D,0x13DD2945,0xE0B8F03E,0xFFFF16A2,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},

	/* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
	/* B70E0CBD6BB4BF7F321390B94A03C1D356C21122343280D6115C1D21   X - big end  from SEC2 */
	{0x115C1D21,0x343280D6,0x56C21122,0x4A03C1D3,0x321390B9,0x6BB4BF7F,0xB70E0CBD},  
	/* BD376388B5F723FB4C22DFE6CD4375A05A07476444D5819985007E34  Y - big end  from SEC2 */
	{0x85007E34,0x44D58199,0x5A074764,0xCD4375A0,0x4C22DFE6,0xB5F723FB,0xBD376388}, 

	1, /* EC cofactor K */	
	/* EC cofactor inverse EC_CofactorInv !!!!!!  Not inserted   !!!! */
	{0x00000000},
	
	224, /* Size of field modulus in bits */
	7,   /* Size of field modulus in words*/
	224, /* Size of order of generator in bits */
	7,   /* Size of order of generator in words*/
	

	/* Barrett tags NP,RP */
	{0x0000000F,0x00000000,0x00000000,0x00000000,0x00000010,
	0x000E95D1/*0x00095D1F*/,0x00000000,0x00000000,0x00000000,0x00000010},
	
	CRYS_ECPKI_DomainID_secp224r1,  /* EC Domain identifier - enum */
	GFp    /* EC finite field identifier - enum */   
},
  

	/****** EC Domain: CRYS_ECPKI_DomainID_secp256k1 = 7 ****/
{	
	/* Field modulus :  GF_Modulus =  FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F - big end*/ 
	{0xFFFFFC2F,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
	/* EC equation parameters a, b  */
	/* a = 0  - big end  from SEC2 */
	{0x00000000},
	/* b = 7 - big end  from SEC2 */
	{0x00000007},

	/* Order of generator: FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141 big end  from SEC2 */
	{0xD0364141,0xBFD25E8C,0xAF48A03B,0xBAAEDCE6,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},

	/* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
	/* 79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798   X - big end  from SEC2 */
	{0x16F81798,0x59F2815B,0x2DCE28D9,0x029BFCDB,0xCE870B07,0x55A06295,0xF9DCBBAC,0x79BE667E},  
	/* 483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8  Y - big end  from SEC2 */
	{0xFB10D4B8,0x9C47D08F,0xA6855419,0xFD17B448,0x0E1108A8,0x5DA4FBFC,0x26A3C465,0x483ADA77}, 

	1, /* EC cofactor K */	
	/* EC cofactor inverse EC_CofactorInv !!!!!!  Not inserted   !!!! */
	{0x00000000},
	
	256, /* Size of field modulus in bits */
	8,   /* Size of field modulus in words*/
	256, /* Size of order of generator in bits */
	8,   /* Size of order of generator in words*/

	
	/* Barrett tags NP,RP */
	{0x00000000,0x00000000,0x00000000,0x00000000,0x00000010,
	0x00000014,0x00000000,0x00000000,0x00000000,0x00000010},
	
	CRYS_ECPKI_DomainID_secp256k1,  /* EC Domain identifier - enum */
	GFp    /* EC finite field identifier - enum */   
},
	/****** EC Domain: CRYS_ECPKI_DomainID_secp256r1 = 8   ******/ 

	{	/* Field modulus :  GF_Modulus =  FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFF - big end*/ 
	{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x00000000,0x00000000,0x00000000,0x00000001,0xFFFFFFFF},
	/* EC equation parameters a, b  */
	/* a = FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFC  - big end  from SEC2 */
	{0xFFFFFFFC,0xFFFFFFFF,0xFFFFFFFF,0x00000000,0x00000000,0x00000000,0x00000001,0xFFFFFFFF},
	/* b = 5AC635D8AA3A93E7B3EBBD55769886BC651D06B0CC53B0F63BCE3C3E27D2604B - big end  from SEC2 */
	{0x27D2604B,0x3BCE3C3E,0xCC53B0F6,0x651D06B0,0x769886BC,0xB3EBBD55,0xAA3A93E7,0x5AC635D8},

	/* Order of generator: FFFFFFFF00000000FFFFFFFFFFFFFFFFBCE6FAADA7179E84F3B9CAC2FC632551 big end  from SEC2 */
	{0xFC632551,0xF3B9CAC2,0xA7179E84,0xBCE6FAAD,0xFFFFFFFF,0xFFFFFFFF,0x00000000,0xFFFFFFFF},

	/* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
	/* 6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296   X - big end  from SEC2 */
	{0xD898C296,0xF4A13945,0x2DEB33A0,0x77037D81,0x63A440F2,0xF8BCE6E5,0xE12C4247,0x6B17D1F2},  
	/* 4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5  Y - big end  from SEC2 */
	{0x37BF51F5,0xCBB64068,0x6B315ECE,0x2BCE3357,0x7C0F9E16,0x8EE7EB4A,0xFE1A7F9B,0x4FE342E2}, 

	1, /* EC cofactor K */	
	/* EC cofactor inverse EC_CofactorInv !!!!!!  Not inserted   !!!! */
	{0x00000000},	
	256, /* Size of field modulus in bits */
	8,   /* Size of field modulus in words*/
	256, /* Size of order of generator in bits */
	8,   /* Size of order of generator in words*/
	

	/* Barrett tags NP,RP */
	{0xFFFFFFEF,0xFFFFFFEF,0xFFFFFFFF,0x0000000F,0x00000010,
	0xFFFFFFF4,0xFFFFFFEF,0xFFFFFFFF,0x0000000F,0x00000010},
    	
	CRYS_ECPKI_DomainID_secp256r1,  /* EC Domain identifier - enum */
	GFp    /* EC finite field identifier - enum */   
},

	/******    EC Domain: CRYS_ECPKI_DomainID_secp384r1 = 9  ****/
{	
	/* Field modulus :  GF_Modulus =  FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFF - big end*/ 
	{0xFFFFFFFF,0x00000000,0x00000000,0xFFFFFFFF,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
	/* EC equation parameters a, b  */
	/* a = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFC   - big end  from SEC2 */
	{0xFFFFFFFC,0x00000000,0x00000000,0xFFFFFFFF,0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},
	/* b = B3312FA7E23EE7E4988E056BE3F82D19181D9C6EFE8141120314088F5013875AC656398D8A2ED19D2A85C8EDD3EC2AEF  - big end  from SEC2 */
	{0xD3EC2AEF,0x2A85C8ED,0x8A2ED19D,0xC656398D,0x5013875A,0x0314088F,0xFE814112,0x181D9C6E,0xE3F82D19,0x988E056B,0xE23EE7E4,0xB3312FA7},

	/* Order of generator: FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC7634D81F4372DDF581A0DB248B0A77AECEC196ACCC52973  big end  from SEC2 */
	{0xCCC52973,0xECEC196A,0x48B0A77A,0x581A0DB2,0xF4372DDF,0xC7634D81,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},

	/* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
	/* AA87CA22BE8B05378EB1C71EF320AD746E1D3B628BA79B9859F741E082542A385502F25DBF55296C3A545E3872760AB7    X - big end  from SEC2 */
	{0x72760AB7,0x3A545E38,0xBF55296C,0x5502F25D,0x82542A38,0x59F741E0,0x8BA79B98,0x6E1D3B62,0xF320AD74,0x8EB1C71E,0xBE8B0537,0xAA87CA22},  
	/* 3617DE4A96262C6F5D9E98BF9292DC29F8F41DBD289A147CE9DA3113B5F0B8C00A60B1CE1D7E819D7A431D7C90EA0E5F   Y - big end  from SEC2 */
	{0x90EA0E5F,0x7A431D7C,0x1D7E819D,0x0A60B1CE,0xB5F0B8C0,0xE9DA3113,0x289A147C,0xF8F41DBD,0x9292DC29,0x5D9E98BF,0x96262C6F,0x3617DE4A}, 

	1, /* EC cofactor K */	
	/* EC cofactor inverse EC_CofactorInv !!!!!!  Not inserted   !!!! */
	{0x00000001},
	
	384, /* Size of field modulus in bits */
	12,   /* Size of field modulus in words*/
	384, /* Size of order of generator in bits */
	12,   /* Size of order of generator in words*/
	
	
	/* Barrett tags NP,RP */
	{0x00000000,0x00000000,0x00000000,0x00000000,0x00000010,
	0x00000000,0x00000000,0x00000000,0x00000000,0x00000010},
	
	CRYS_ECPKI_DomainID_secp384r1,  /* EC Domain identifier - enum */
	GFp    /* EC finite field identifier - enum */   
},

	/****** EC Domain: CRYS_ECPKI_DomainID_secp521r1 = 10 ****/ 

{	/* Field modulus :  GF_Modulus =  P= 1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF */ 
	{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
	 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x000001FF},
	/* EC equation parameters a, b  */
	/* a = 1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC  - big end  from SEC2 */
	{0xFFFFFFFC,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
	 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x000001FF},
	/* b = Bec= 51953EB9618E1C9A1F929A21A0B68540EEA2DA725B99B315F3B8B489918EF109E156193951EC7E937B1652C0BD3BB1BF073573DF883D2C34F1EF451FD46B503F00  */
	{0x6B503F00,0xEF451FD4,0x3D2C34F1,0x3573DF88,0x3BB1BF07,0x1652C0BD,0xEC7E937B,0x56193951,0x8EF109E1,0xB8B48991,0x99B315F3,0xA2DA725B,0xB68540EE,0x929A21A0,0x8E1C9A1F,0x953EB961,0x00000051},

	/* Order of generator: 1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFA51868783BF2F966B7FCC0148F709A5D03BB5C9B8899C47AEBB6FB71E91386409 big end  from SEC2 */
	{0x91386409,0xBB6FB71E,0x899C47AE,0x3BB5C9B8,0xF709A5D0,0x7FCC0148,0xBF2F966B,0x51868783,0xFFFFFFFA,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x000001FF},

	/* Generator  coordinates in affine form: EC_Gener_X, EC_Gener_Y (in ordinary representation) */
	/* Gx= C6858E06B70404E9CD9E3ECB662395B4429C648139053FB521F828AF606B4D3DBAA14B5E77EFE75928FE1DC127A2FFA8DE3348B3C1856A429BF97E7E31C2E5BD66  */
	{0xC2E5BD66,0xF97E7E31,0x856A429B,0x3348B3C1,0xA2FFA8DE,0xFE1DC127,0xEFE75928,0xA14B5E77,0x6B4D3DBA,0xF828AF60,0x053FB521,0x9C648139,0x2395B442,0x9E3ECB66,0x0404E9CD,0x858E06B7,0x000000C6},  
	/* Gy= 11839296A789A3BC0045C8A5FB42C7D1BD998F54449579B446817AFBD17273E662C97EE72995EF42640C550B9013FAD0761353C7086A272C24088BE94769FD16650  Y - big end  from SEC2 */
	{0x9FD16650,0x88BE9476,0xA272C240,0x353C7086,0x3FAD0761,0xC550B901,0x5EF42640,0x97EE7299,0x273E662C,0x17AFBD17,0x579B4468,0x98F54449,0x2C7D1BD9,0x5C8A5FB4,0x9A3BC004,0x39296A78,0x00000118}, 
	1, /* EC cofactor K */	
	/* EC cofactor inverse EC_CofactorInv !!!!!!  Not inserted   !!!! */
	{0x00000001},	
	521, /* Size of field modulus in bits */
	17,   /* Size of field modulus in words*/
	521, /* Size of order of generator in bits */
	17,   /* Size of order of generator in words*/	

    
	/* Barrett tags NP,RP */
	{0x00000000,0x00000000,0x00000000,0x00000000,0x00000010,
	0x00000008,0x00000000,0x00000000,0x00000000,0x00000010},
    
	CRYS_ECPKI_DomainID_secp521r1,  /* EC Domain identifier - enum */
	GFp    /* EC finite field identifier - enum */   
},


/********************************************************************************************************************/

}; /* end of LLF_EC_DOMAINS_ARRAY[] */

/********************************************************************************************************************/


