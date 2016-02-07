/*
******************************************************************************
*                     HEADER FILE
*
*     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_tags.h,v $ $Revision: 1.8 $
*     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_tags.h,v $
*     $Author: lob $ $Date: 2009-03-09 16:56:34 $
*     $State: Exp $ $Locker:  $
*
*     INTERPEAK_COPYRIGHT_STRING
*     Design and implementation by Lennart Bang <lob@interpeak.se>
******************************************************************************
*/
#ifndef IPCOM_TAGS_H
#define IPCOM_TAGS_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
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

#include "ipcom_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* IP_TAG tag array terminator. */
#define IP_TAG_END          0xffffffffL

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         IP_TAG
 *===========================================================================
 * Address-size bit tag array terminated with the end tag IP_TAG_END.
 * format:   tag type, tag value, ..., tag type ,tag value, IP_TAG_END
 */
typedef Ip_ptrdiff_t Ip_tag;


/*
 *===========================================================================
 *                         IP_VTAG
 *===========================================================================
 * IP_VTAG - Variable sized tags. A 'count' variable is required to determine
 *          how many vtags are used.
 */

#ifndef __cplusplus
union Ip_vtag;
#endif

/* Ip_vtag header. */
typedef struct Ip_vtag_hdr_struct
{
  Ip_u32    type;         /* Tag type. */
  Ip_s16    status;       /* 0 for valid tag, else error code. */
  Ip_u16    size;         /* Number of data bytes. */
}
Ip_vtag_hdr;

/* Ip_vtag u32 */
typedef struct Ip_vtag_u32_struct
{
  Ip_vtag_hdr   hdr;
  Ip_u32        data;
}
Ip_vtag_u32;

/* Ip_vtag s32 */
typedef struct Ip_vtag_s32_struct
{
  Ip_vtag_hdr   hdr;
  Ip_s32        data;
}
Ip_vtag_s32;

/* Ip_vtag u64 */
typedef struct Ip_vtag_u64_struct
{
  Ip_vtag_hdr   hdr;
  Ip_u64        data;
}
Ip_vtag_u64;

#define IP_VTAG_NEXT(xxvtagp)  ((union Ip_vtag *)((Ip_u8 *)xxvtagp + IP_ROUNDUP(xxvtagp->hdr.size,sizeof(Ip_u32)) + sizeof(Ip_vtag_hdr)))

#define IP_VTAG_ADD_U32(xxvtagp, xxtype, xxval, xxcount)  do { \
  ((Ip_vtag_u32 *)(xxvtagp))->hdr.type = xxtype; \
  ((Ip_vtag_u32 *)(xxvtagp))->hdr.size = sizeof(Ip_u32); \
  ((Ip_vtag_u32 *)(xxvtagp))->data = (Ip_u32)xxval; \
  (xxvtagp) = (void *)((Ip_u8 *)(xxvtagp) + sizeof(Ip_vtag_u32)); \
  (xxcount)++; \
  } while((0))

#define IP_VTAG_ADD_S32(xxvtagp, xxtype, xxval, xxcount)  do { \
  ((Ip_vtag_s32 *)(xxvtagp))->hdr.type = xxtype; \
  ((Ip_vtag_s32 *)(xxvtagp))->hdr.size = sizeof(Ip_s32); \
  ((Ip_vtag_s32 *)(xxvtagp))->data = (Ip_s32)xxval; \
  (xxvtagp) = (void *)((Ip_u8 *)(xxvtagp) + sizeof(Ip_vtag_s32)); \
  (xxcount)++; \
  } while((0))

#define IP_VTAG_ADD_U64(xxvtagp, xxtype, xxval, xxcount)  do { \
  ((Ip_vtag_u64 *)(xxvtagp))->hdr.type = xxtype; \
  ((Ip_vtag_u64 *)(xxvtagp))->hdr.size = sizeof(Ip_u64); \
  IP_U64_COPY(((Ip_vtag_u64 *)(xxvtagp))->data, xxval); \
  (xxvtagp) = (void *)((Ip_u8 *)(xxvtagp) + sizeof(Ip_vtag_u64)); \
  (xxcount)++; \
  } while((0))


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

