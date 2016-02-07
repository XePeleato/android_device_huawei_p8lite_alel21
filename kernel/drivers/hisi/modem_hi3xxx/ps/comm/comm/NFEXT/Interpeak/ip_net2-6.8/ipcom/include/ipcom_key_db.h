/* ipcom_key_db.h - Public API of IPCOM key database */
/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_key_db.h,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_key_db.h,v $
 *   $Author: ulf $ $Date: 2009-06-08 11:04:41 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Roger Boden <roger.boden@windriver.com>
 ******************************************************************************
 */
#ifndef IPCOM_KEY_DB_H
#define IPCOM_KEY_DB_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
This library contains the API for the IPCOM key database.
*/

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */


/* Note: Changing IPCOM_KEY_DB_MAX_BUF will affect stack usage. */
#ifndef IPCOM_KEY_DB_MAX_BUF
#define IPCOM_KEY_DB_MAX_BUF 128
#endif

#ifndef IPCOM_KEY_DB_MAX_ID
#define IPCOM_KEY_DB_MAX_ID 32
#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */
    /* Forward declarations */
    struct evp_pkey_st;
    struct x509_st;

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*******************************************************************************
*
* ipcom_key_db_pkey_get - get key with specified key_id
*
* This routine returns the public key with the specified key ID
*
* Parameter:
* \is
* \i <key_id>
* The key ID of the key to get
* \ie
*
* RETURNS: A pointer to a public key or IP_NULL if the key does not exist.
*
* ERRNO:
*/
    IP_PUBLIC struct evp_pkey_st* ipcom_key_db_pkey_get
    (
     const char* key_id
    );


/*******************************************************************************
*
* ipcom_key_db_pkey_get2 - get key matching an X509 certificate
*
* This routine searches the key database for a public key pair matching the
* the public key in the specified X509 certificate.
*
* Parameter:
* \is
* \i <x509>
* holds the X509 certificate whose private key should be returned
* \i <key_id> pointer to buffer to put the key id in.
* \i <key_id_len> length of the key_id buffer.
* \ie
*
* RETURNS: A pointer to a public key or IP_NULL if no matching key could be found.
*
* ERRNO:
*/
    IP_PUBLIC struct evp_pkey_st* ipcom_key_db_pkey_get2
    (
     struct x509_st* x509,
     char* key_id,
     int* key_id_len
    );

/*******************************************************************************
*
* ipcom_key_db_pkey_import - store a public key pair in the key database
*
* This routine imports a public key pair into the key key database.
*
* Parameter:
* \is
* \i <key_id>
* key id of the key being imported
* \i <key>
* the actual key to import
* \ie
*
* RETURNS: IPCOM_SUCCESS if successful, IPCOM_ERR_INVALID_ARG if an invalid key
* is given as argument, IPCOM_ERR_ALREADY_INSTALLED if key with the same key ID
* is already stored in the key database.
*
* ERRNO:
*/
    IP_PUBLIC Ip_err ipcom_key_db_pkey_import
    (
     const char* key_id,
     const struct evp_pkey_st* key
    );


/*******************************************************************************
*
* ipcom_key_db_pkey_del - delete public key pair
*
* Delete public key pair with ID key_id from the key db.
*
* Parameter:
* \is
* \i <key_id>
* key ID of the key to delete
* \ie
*
* RETURNS:
* IPCOM_SUCCESS if the key was deleted
* IPCOM_ERR_NOT_FOUND if key ID does not exist
* IPCOM_ERR_FAILED for general errors
*
* ERRNO:
*/

    IP_PUBLIC Ip_err ipcom_key_db_pkey_del
    (
     const char* key_id
    );


/*******************************************************************************
*
* ipcom_key_db_pkey_opendb - open the database for reading
*
* This routine returns a handle that can be used to iterate all key IDs in the
* database. Note that public keys and buffers/pre-shared keys have different
* name spaces.
*
* RETURNS: A handle to be used in subsequent calls to ipcom_key_db_pkey_readdb
* and ipcom_key_db_pkey_closedb.
*
* ERRNO:
*/
    IP_PUBLIC void* ipcom_key_db_pkey_opendb
    (
     void
     );

/*******************************************************************************
*
* ipcom_key_db_pkey_readdb - return next key ID from the database
*
* This routine returns the next key ID from the database.
*
* Parameter:
* \is
* \i <handle>
* handle returned from ipcom_key_db_pkey_opendb
* \i <name>
* buffer to store the key ID in
* \i <name>
* length of name buffer
* \ie
*
* RETURNS:
* IPCOM_SUCCESS if successful
* IPCOM_ERR_END if no more entries can be read
* IPCOM_ERR_FAILED in case of error
*
* ERRNO:
*/
    IP_PUBLIC Ip_err ipcom_key_db_pkey_readdb
    (
     void* handle,
     char* name,
     int name_len
    );


/*******************************************************************************
*
* ipcom_key_db_pkey_closedb - close handle
*
* This routine closes a handle used to iterate all key IDs in the
* database.
*
* Parameter:
* \is
* \i <handle>
* handle returned from ipcom_key_db_pkey_opendb
* \ie
*
* RETURNS: A handle to be used in subsequent calls to ipcom_key_db_pkey_readdb
* and ipcom_key_db_pkey_closedb.
*
* ERRNO:
*/
    IP_PUBLIC Ip_err ipcom_key_db_pkey_closedb
    (
     void* handle
    );


/*******************************************************************************
*
* ipcom_key_db_buf_import - import buffer into the key db and give it ID key_id
*
* Import buffer into the key db and give it ID key_id. The buffer is typically a
* password or a pre-shared key.
*
* Parameter:
* \is
* \i <key_id>
* key ID of the key to import
* \i <key>
* the key/buffer to import
* \i <key_len>
* length of the key
* \ie
*
* RETURNS:
* IPCOM_SUCCESS if the key was successfully imported
* IPCOM_ERR_INVALID_ARG if the key is too long
* IPCOM_ERR_ALREADY_INSTALLED if the key ID already exists
* IPCOM_ERR_FAILED for general errors
*
* ERRNO:
*/
    IP_PUBLIC Ip_err ipcom_key_db_buf_import
    (
     const char* key_id,
     const Ip_u8* key,
     int key_len
    );


/*******************************************************************************
*
* ipcom_key_db_buf_get - get key
*
* Get buffer with specified key_id
*
* Parameter:
* \is
* \i <key_id>
* key ID of the key to fetch
* \i <key>
* pointer to the buffer to store the key in
* \i <key_len>
* length of the key buffer
* \ie
*
* RETURNS:
* IPCOM_SUCCESS if the key was succesfully retrieved
* IPCOM_ERR_NOT_FOUND if the key ID does not exist
* IPCOM_ERR_NO_MEMORY if not enough memory is available to complete the request
* IPCOM_ERR_FAILED for general errors
*
* ERRNO:
*/

    IP_PUBLIC Ip_err ipcom_key_db_buf_get
    (
     const char* key_id,
     Ip_u8* key,
     int* key_len
    );


/*******************************************************************************
*
* ipcom_key_db_buf_del - delete buffer from key database
*
* Delete buffer with ID key_id from the key db
*
* Parameter:
* \is
* \i <key_id>
* Key ID of key to delete
* \ie
*
* RETURNS:
* IPCOM_SUCCESS if the key was deleted
* IPCOM_ERR_NOT_FOUND if key ID does not exist
* IPCOM_ERR_FAILED for general errors
*
* ERRNO:
*/
    IP_PUBLIC Ip_err ipcom_key_db_buf_del
    (
     const char* key_id
    );


/*******************************************************************************
*
* ipcom_key_db_buf_opendb - open the database for reading
*
* This routine returns a handle that can be used to iterate all key IDs in the
* database. Note that public keys and buffers/pre-shared keys have different
* name spaces.
*
* RETURNS: A handle to be used in subsequent calls to ipcom_key_db_buf_readdb
* and ipcom_key_db_buf_closedb.
* NULL in case of error.
*
* ERRNO:
*/
    IP_PUBLIC void* ipcom_key_db_buf_opendb
    (
     void
    );


/*******************************************************************************
*
* ipcom_key_db_buf_readdb - return next key ID from the database
*
* This routine returns the next key ID from the database.
*
* Parameter:
* \is
* \i <handle>
* handle returned from ipcom_key_db_pkey_opendb
* \i <name>
* buffer to store the key ID in
* \i <name>
* length of name buffer
* \ie
*
* RETURNS:
* IPCOM_SUCCESS if successful
* IPCOM_ERR_END if no more entries can be read
* IPCOM_ERR_FAILED in case of error
*
* ERRNO:
*/
    IP_PUBLIC Ip_err ipcom_key_db_buf_readdb
    (
     void* handle,
     char* name,
     int name_len
    );


/*******************************************************************************
*
* ipcom_key_db_buf_closedb - close handle
*
* This routine closes a handle used to iterate all key IDs in the
* database.
*
* Parameter:
* \is
* \i <handle>
* handle returned from ipcom_key_db_buf_opendb
* \ie
*
* RETURNS:
* IPCOM_SUCCESS if the handle was closed
* IPCOM_ERR_FAILED if the handle could not be closed;
*
* ERRNO:
*/
    IP_PUBLIC Ip_err ipcom_key_db_buf_closedb
    (
     void* handle
    );



#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
