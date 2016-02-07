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


/************* Include Files ****************/
#include "dx_vos_basetypes.h"
#include "crys_defs.h"
#include "crys_version.h"
#include "crys_rsa_types.h"

/************************ Defines *********************************/


/* the CRYS release definitions */

#define LLF_ECPKI_RELEASE_TYPE          CRYS_DEFS_CC6_PKA_ENGINE_TYPE  
#define LLF_ECPKI_MAGOR_VERSION_NUM     6
#define LLF_ECPKI_MINOR_VERSION_NUM     0
#define LLF_ECPKI_SUB_VERSION_NUM       0
#define LLF_ECPKI_INTERNAL_VERSION_NUM  0

/************************ Enums ************************************/

/************************ Typedefs *********************************/


/************************ Global Data ******************************/

/************* Private function prototype **************************/


/************************ Public Functions *************************/

/**
 * @brief This Api returnes the LLF ECPKI version.
 *
 * The version containes the following:
 *
 * component string - a string describing the nature of the release.
 * release type : 0 / SW 
 *
 * major , minor , sub , internal - the release digits.
 * 
 * each component : CRYS , LLF machines receives this database. 
 *
 * @param[in] version_ptr - a pointer to the version structure.
 *
 */

 void  LLF_ECPKI_GetVersion(CRYS_ComponentVersion_t *version_ptr)
 {
    /* LOCAL DECLERATIONS */
    
    /* FUNCTION LOGIC */
    
    /* .............. seting the CRYS version .................. */
    
    version_ptr->compName[0] = 'E';
    version_ptr->compName[1] = 'C';
    version_ptr->compName[2] = 'P';
    version_ptr->compName[3] = 'K';
    
    version_ptr->type     = LLF_ECPKI_RELEASE_TYPE;
    version_ptr->major    = LLF_ECPKI_MAGOR_VERSION_NUM;
    version_ptr->minor    = LLF_ECPKI_MINOR_VERSION_NUM;
    version_ptr->sub      = LLF_ECPKI_SUB_VERSION_NUM;
    version_ptr->internal = LLF_ECPKI_INTERNAL_VERSION_NUM; 
    
    return;     
    
 }/* END OF LLF_PKI_GetVersion */   
 
