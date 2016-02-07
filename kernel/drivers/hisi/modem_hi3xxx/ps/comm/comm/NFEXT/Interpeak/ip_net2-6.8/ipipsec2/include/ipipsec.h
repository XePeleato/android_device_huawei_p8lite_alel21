/* ipipsec.h - API of IPIPSEC */

/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipipsec.h,v $ $Revision: 1.62 $
 *   $Source: /home/interpeak/CVSRoot/ipipsec2/include/ipipsec.h,v $
 *   $Author: lob $ $Date: 2009-09-08 13:40:30 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPIPSEC_H
#define IPIPSEC_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
This library contains the API for IPIPSEC implementation
*/

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipipsec_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_sock.h>
#include <ipcom_inet.h>
#include <ipcom_netif.h>
#include <ipcom_pkt.h>
#include <ipcom_tags.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         IPIPSEC2_RELEASE
 *===========================================================================
 * IPIPSEC2 release version.
 */
#define IPIPSEC2_RELEASE 60701


/*
 *===========================================================================
 *                         IPIPSEC2_LAS_SUPPORT
 *===========================================================================
 * This version of IPIPSEC has LAS support.
 */
#define IPIPSEC2_LAS_SUPPORT        1

/*
 *===========================================================================
 *                         IPIPSEC_SPI
 *===========================================================================
 * Reserved SPI numbers.
 */
#define IPIPSEC_SPI_LOCAL_USE       0
#define IPIPSEC_SPI_RESERVED_MIN    1
#define IPIPSEC_SPI_RESERVED_MAX    255


/*
 *===========================================================================
 *                         IPIPSEC bypass
 *===========================================================================
 */

/* Reserved bypass flow */
#define IPIPSEC_BYPASS_FLOW   ((void *)0xffffffff)

/* Bypass domain value (in Ipipsec_sa_handle 'domain') */
#define IPIPSEC_AF_BYPASS     0xff


/*
 *===========================================================================
 *                         IPSEC ioctls
 *===========================================================================
 * IPSEC IO controls
 */
#ifndef IPIPSEC_PORT_IOCTL
#define IP_SIOCXIPSEC_SELECTOR       IP_IOWR(BASE, 80, W, Ipipsec_ctrl_selector)
#define IP_SIOCXIPSEC_NEXT_SA        IP_IOWR(BASE, 81, W, Ipipsec_ioctl_sactrl)
#define IP_SIOCXIPSEC_Grouped        IP_IOWR(BASE, 82, W, Ipipsec_ioctl_sactrl)
#define IP_SIOCXIPSEC_InUserOctets   IP_IOWR(BASE, 83, W, Ipipsec_ioctl_sactrl)
#define IP_SIOCXIPSEC_OutUserOctets  IP_IOWR(BASE, 84, W, Ipipsec_ioctl_sactrl)
#define IP_SIOCXIPSEC_InUserPackets  IP_IOWR(BASE, 85, W, Ipipsec_ioctl_sactrl)
#define IP_SIOCXIPSEC_OutUserPackets IP_IOWR(BASE, 86, W, Ipipsec_ioctl_sactrl)
#endif

/*
 *===========================================================================
 *                         IPIPSEC I/O events
 *===========================================================================
 * IPIPSEC events are use to call stack functions from IPIPSEC.
 */
#define IPIPSEC_EIOXFDTOSOCK     0x3002    /* &fd; Get sock pointer. */
#define IPIPSEC_EIOXIP4ID        0x3005    /* -; Next IPv4 ID. */
#define IPIPSEC_EIOXOUTPUT       0x3006    /* Ipipsec_eio_output; Output to key socket(s). */


/*
 *===========================================================================
 *                 ipipsec_pfkeyv2_output send modes
 *===========================================================================
 */
#define IPIPSEC_SNDMODE_UNICAST         1
#define IPIPSEC_SNDMODE_BROADCAST       2
#define IPIPSEC_SNDMODE_REGISTERED      3


/*
 *===========================================================================
 *                         TRAPS
 *===========================================================================
 * MIB trap types. 2nd argument in the Ipipsec_trap_func callback.
 */
#define IPIPSEC_TRAP_NUM                               8

#define IPIPSEC_TRAPTYPE_espAuthFailureTrap            0
#define IPIPSEC_TRAPTYPE_ahAuthFailureTrap             1
#define IPIPSEC_TRAPTYPE_espReplayFailureTrap          2
#define IPIPSEC_TRAPTYPE_ahReplayFailureTrap           3
#define IPIPSEC_TRAPTYPE_espPolicyFailureTrap          4
#define IPIPSEC_TRAPTYPE_ahPolicyFailureTrap           5
#define IPIPSEC_TRAPTYPE_invalidSpiTrap                6
#define IPIPSEC_TRAPTYPE_otherPolicyFailureTrap        7


/*
 *===========================================================================
 *                         Mask lengths
 *===========================================================================
 */
#if defined(IPCOM_USE_INET6) && defined(IPCOM_USE_INET)
#define ipipsec_domain_masklen(d)   ((d) == IP_AF_INET? (32) : ((d) == IP_AF_INET6? (128) : (0)))
#elif defined(IPCOM_USE_INET6)
#define ipipsec_domain_masklen(d)   ((d) == IP_AF_INET6? (128) : (0))
#else
#define ipipsec_domain_masklen(d)   ((d) == IP_AF_INET? (32) : (0))
#endif

#define ipipsec_addrmasklen(d,m)    ipcom_mask_to_prefixlen((m), ipipsec_domain_masklen(d))


/*
 *===========================================================================
 *                         Return codes for ipipsec_input/output
 *===========================================================================
 *
 */
#define IPIPSEC_OK         0
#define IPIPSEC_TUNNELED   1
#define IPIPSEC_KEPT       2
#define IPIPSEC_DUMMY_PKT  3 /* Dummy packet discard in daemon mode */


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipipsec_sa_handle
 *===========================================================================
 * Handle to uniquely identify a SA.
 */
typedef struct Ipipsec_sa_handle_struct
{
    union Ip_in_addr_union   dst;   /* destination address in network endian. */
    union Ip_in_addr_union   src;   /* source address in network endian. */

    Ip_u32   spi_n;        /* SPI in network endian. */

    Ip_u8    protocol;     /* IP_IPPROTO_ESP, IP_IPPROTO_AH or 0 for any. */
    Ip_u8    direction;    /* See valid values below. */
    Ip_u8    domain;       /* IP_AF_INET, IP_AF_INET6 or IPIPSEC_AF_BYPASS */
    Ip_u8    satype;       /* SADB_SATYPE_xx. */
    char     priority;     /* -128 to 127, -128 is highest and 0 default */
    Ip_u8    dscp;         /* DSCP value (6 highest bits) */
}
Ipipsec_sa_handle;

/* 'direction' values: */
#define IPIPSEC_SADIR_INPUT   0
#define IPIPSEC_SADIR_OUTPUT  1
#define IPIPSEC_SADIR_ANY     2


/*
 *===========================================================================
 *                         Ipipsec_ioctl_sactrl
 *===========================================================================
 * Used in all IOCTLS that replace the ipipsec_sactrl() tags.
 */
typedef struct Ipipsec_ioctl_sactrl_struct
{
    Ipipsec_sa_handle    sa;

    union arg_union
    {
        Ipipsec_sa_handle  next;
        Ipipsec_sa_handle  group;
        Ip_u64             u64;
    } arg;

}
Ipipsec_ioctl_sactrl;


/*
 *===========================================================================
 *                         Ipipsec_ctrl_selector
 *===========================================================================
 * Used in ipipsec_ctrl() to get selector/flow information.
 */
typedef struct Ipipsec_ctrl_selector_struct
{
   /* SelectorEntry */
    Ip_u32            Index;      /* Set to 0 to get first selector (flow).
                                   * Updated by ipipsec_ctrl().
                                   */
    union Ip_in_addr_union     LocalId;
    union Ip_in_addr_union     LocalMaxId;
    union Ip_in_addr_union     RemoteId;
    union Ip_in_addr_union     RemoteMaxId;
    Ip_u16                     LocalPort;     /* in network endian. */
    Ip_u16                     LocalMaxPort;  /* in network endian. */
    Ip_u16                     RemotePort;    /* in network endian. */
    Ip_u16                     RemoteMaxPort; /* in network endian. */
    Ip_u8                      Protocol;      /* IP protocol */
    Ip_u8                      domain;
    Ip_u8                      direction;     /* 0=in, 1=out, 2=any */
    Ip_u8                      flowtype;      /* SADB_X_FLOW_TYPE_xx */
    char                       flowpri;       /* Flow priority -128 to 127 */
    Ip_u8                      ports[2];      /* [0]=ICMP type, [1]=ICMP code */
    Ip_u8                      pad[1];        /* unused */

    /* Masks */
    union Ip_in_addr_union     local_mask;
    union Ip_in_addr_union     remote_mask;

    /* SA */
    Ipipsec_sa_handle          sa;
}
Ipipsec_ctrl_selector;


/*
 *===========================================================================
 *                         Ipipsec_ip_header
 *===========================================================================
 */
typedef union Ipipsec_ipheader_union
{
#ifdef IPCOM_USE_INET
    Ipcom_ip_header       inet;
#endif
#ifdef IPCOM_USE_INET6
    Ipcom_ip6_header      inet6;
#endif
}
Ipipsec_ip_header;


/*
 *===========================================================================
 *                         Ipipsec_key
 *===========================================================================
 */
typedef struct Ipipsec_key_struct
{
    Ip_u32               policy_id; /* Policy id (set to current in lookup) */
    union Ip_in_addr_union   src;   /* Source address in network endian. */
    union Ip_in_addr_union   dst;   /* Destination address in network endian. */
    Ip_u32                   srcscope_n;   /* IPv6 only */
    Ip_u32                   dstscope_n;   /* IPv6 only */

    Ip_u8                proto;     /* ESP, AH, UDP etc */
    Ip_u8                domain;    /* IP_AF_INET or IP_AF_INET6 */
    Ip_u8                direction; /* 0=in, 1=out */
    char                 priority;  /* -128 (highest) to 127 (lowest) */
    Ip_u8                dscp;      /* DSCP value (6 highest bits) */
    char                 pad[3];    /* currently unused, set to 0 */

    union ports_union
    {
        Ip_u32           espah_spi_n;
        Ip_u16           udptcp_srcdst_n[2]; /* [0]=source, [1]=destination */
        Ip_u8            icmp_typecode[4];   /* [0]=type, [1]=code [2]=[3]=n/a */
        Ip_u8            mh_type[4];         /* [0]=type, [1]=[2]=[3]=n/a */
    } ports;
}
Ipipsec_key;


/*
 *===========================================================================
 *                    Ipipsec_param
 *===========================================================================
 * Used by ipipsec_output() and ipipsec_input() functions.
 */
typedef struct Ipipsec_param_struct
{
    Ipipsec_key     key;    /* Packet lookup key */


    /***** IPIPSEC Internal params *****/
    void   *output_flow;       /* Output flow cache */

    /* Original start and end of packet (used in ipipsec_input()) */
    int     pkt_org_start;
    int     pkt_org_end;

    /* Original IP header pointer or new outer-IP header cache */
    Ipipsec_ip_header  *iph;   /* Pointer to IP header (original or store-holder below) */
    Ipipsec_ip_header   ip;    /* Store-holder for created(outer) IP-header */
    int                 iph_length;  /* IP header length */

    /* UDP Encapsulation */
    Ipcom_udp_header    udp;   /* src_port != 0 if active */

    Ip_u8   tos_class;
    Ip_u8   ttl;
    Ip_u8   pad[2];

#if defined(IPHWCRYPTO) && defined(IPIPSEC_USE_ESN)
    Ip_u32  seqh;     /* needed in HWCRYPTO async mode */
    Ip_u8   calc_digest[20]; /* [ IPIPSEC_AUTH_MAXKEYLEN ] */
#endif
}
Ipipsec_param;


/*
 *===========================================================================
 *                         Ipipsec_eio_output
 *===========================================================================
 */
typedef struct Ipipsec_eio_output_struct
{
    Ip_u32        sndmode;
    void         *sock;
    Ipcom_pkt    *pkt;

    Ip_u16        sadb_msg_type;    /* REGISTER, ACQUIRE etc. */
    Ip_u16        sadb_msg_satype;  /* ESP, AH etc. */
}
Ipipsec_eio_output;


/*
 *===========================================================================
 *                         Ipipsec_eio_addr
 *===========================================================================
 */
typedef struct Ipipsec_eio_addr_struct
{
    Ip_u32     addr[4];  /* room for 128-bit IPv6 addr. */
    Ip_u8      domain;
}
Ipipsec_eio_addr;


/*
 ****************************************************************************
 * 6                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*******************************************************************************
*
* ipipsec_ctrl - get global IPsec statistics, trap, or flow information
*
* Get global IPsec statistics, trap, or flow information.
*
* Parameter:
* \is
* \i <ctrl_tags>
* An array of tag/value pairs terminated by IP_TAG_END. Valid parameter tags are
* defined below. Values are 32-bit parameter values or pointers.
* \ie
*
* \ts
* IPIPSEC_CTRLT_GET_SELECTOR                    | Ipipsec_ctrl_selector*
* IPIPSEC_CTRLT_GET_ipsecEspCurrentInboundSAs   | Ip_u32
* IPIPSEC_CTRLT_GET_ipsecEspTotalInboundSAs     | Ip_u32
* IPIPSEC_CTRLT_GET_ipsecEspCurrentOutboundSAs  | Ip_u32
* IPIPSEC_CTRLT_GET_ipsecEspTotalOutboundSAs    | Ip_u32
* IPIPSEC_CTRLT_GET_ipsecAhCurrentInboundSAs    | Ip_u32
* IPIPSEC_CTRLT_GET_ipsecAhTotalInboundSAs      | Ip_u32
* IPIPSEC_CTRLT_GET_ipsecAhCurrentOutboundSAs   | Ip_u32
* IPIPSEC_CTRLT_GET_ipsecAhTotalOutboundSAs     | Ip_u32
* IPIPSEC_CTRLT_GET_ipsecDecryptionErrors       | Ip_u32
* IPIPSEC_CTRLT_GET_ipsecAuthenticationErrors   | Ip_u32
* IPIPSEC_CTRLT_GET_ipsecReplayErrors           | Ip_u32
* IPIPSEC_CTRLT_GET_ipsecPolicyErrors           | Ip_u32
* IPIPSEC_CTRLT_GET_ipsecOtherReceiveErrors     | Ip_u32
* IPIPSEC_CTRLT_GET_ipsecSendErrors             | Ip_u32
* IPIPSEC_CTRLT_GET_ipsecUnknownSpiErrors       | Ip_u32
* IPIPSEC_CTRLT_GET_espAuthFailureTrapEnable    | Ip_u32
* IPIPSEC_CTRLT_GET_ahAuthFailureTrapEnable     | Ip_u32
* IPIPSEC_CTRLT_GET_espReplayFailureTrapEnable  | Ip_u32
* IPIPSEC_CTRLT_GET_ahReplayFailureTrapEnable   | Ip_u32
* IPIPSEC_CTRLT_GET_espPolicyFailureTrapEnable  | Ip_u32
* IPIPSEC_CTRLT_GET_ahPolicyFailureTrapEnable   | Ip_u32
* IPIPSEC_CTRLT_GET_invalidSpiTrapEnable        | Ip_u32
* IPIPSEC_CTRLT_GET_otherPolicyFailureTrapEnable| Ip_u32
* IPIPSEC_CTRLT_SET_espAuthFailureTrapEnable    | Ip_u32
* IPIPSEC_CTRLT_SET_ahAuthFailureTrapEnable     | Ip_u32
* IPIPSEC_CTRLT_SET_espReplayFailureTrapEnable  | Ip_u32
* IPIPSEC_CTRLT_SET_ahReplayFailureTrapEnable   | Ip_u32
* IPIPSEC_CTRLT_SET_espPolicyFailureTrapEnable  | Ip_u32
* IPIPSEC_CTRLT_SET_ahPolicyFailureTrapEnable   | Ip_u32
* IPIPSEC_CTRLT_SET_invalidSpiTrapEnable        | Ip_u32
* IPIPSEC_CTRLT_SET_otherPolicyFailureTrapEnable| Ip_u32
* IPIPSEC_CTRLT_SET_TRAP_FUNCTION               | void*
* IPIPSEC_TRABCBT_NUM_ERRORS                    | Ip_u32
* IPIPSEC_TRABCBT_LOCAL_ADDR                    | union Ip_in_addr_union*
* IPIPSEC_TRABCBT_PEER_ADDR                     | union Ip_in_addr_union*
* IPIPSEC_TRABCBT_SECURITY_PROTOCOL             | Ip_u32
* IPIPSEC_TRABCBT_SPI                           | Ip_u32
* IPIPSEC_TRABCBT_DOMAIN                        | Ip_u32
* \te
*
* \cs
* typedef struct Ipipsec_ctrl_selector_struct
* {
*    /@ SelectorEntry @/
*     Ip_u32            Index;      /@ Set to 0 to get first selector (flow).
*                                      Updated by ipipsec_ctrl(). @/
*     union Ip_in_addr_union     LocalId;
*     union Ip_in_addr_union     RemoteId;
*     Ip_u16                     LocalPort;     /@ in network endian. @/
*     Ip_u16                     LocalMaxPort;  /@ in network endian. @/
*     Ip_u16                     RemotePort;    /@ in network endian. @/
*     Ip_u16                     RemoteMaxPort; /@ in network endian. @/
*     Ip_u8                      Protocol;
*     Ip_u8                      domain;
*     Ip_u8                      direction;
*     Ip_u8                      flowtype;    /@ SADB_X_FLOW_TYPE_xx @/
*
*     /@ Masks @/
*     union Ip_in_addr_union     local_mask;
*     union Ip_in_addr_union     remote_mask;
*
*     /@ SA @/
*     Ipipsec_sa_handle          sa;
* }
* Ipipsec_ctrl_selector;
*
* typedef Ip_u32 (*Ipipsec_trap_func)(Ipipsec_sa_handle *sa, Ip_u32 traptype, Ip_tag *tags);
* \ce
*
* EXAMPLE
* \cs
* int example(void)
* {
*    Ip_tag  tags[3];
*    Ip_err  retval;
*    Ipipsec_ctrl_selector   sel;
*
*    sel.Index = 0;  /@ get first selector. @/
*    tags[0] = IPIPSEC_CTRLT_GET_SELECTOR;
*    tags[1] = (Ip_tag)&sel;
*    tags[2] = IP_TAG_END;
*
*    retval = ipipsec_ctrl(tags);
*    if(retval != IPCOM_SUCCESS)
*    {
*        ipcom_printf("error: Selector print aborted: error = %ld"IP_LF,
* 		   retval);
*        return -1;
*    }
*
*    ipcom_printf("[%ld] ::  ", sel.Index);
* }
* \ce
*
* RETURNS: Either 'IPCOM_SUCCESS' or one of the following errors:
* 'IPCOM_ERR_INVALID_TAG_VALUE', 'IPCOM_ERR_UNSUPPORTED_TAG'.
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipipsec_ctrl
(
    Ip_tag *ctrl_tags
);


/****** IPsec control tags. ******/

/* ****************** Selectors **********************
 * Selectors (also named 'flows') are used to direct output to a certain SA
 * or for input access list.
 *
 * This tag is used to get first/next lexicographic selector based on a 32-bit
 * 'id' in the Ipipsec_ctrl_selector.
 * The 'id' value should be set to 0 for the first selector. Setting 'id' to
 * a non-zero index results in the retrieval of the next lexicographic
 * id or 0 if none available.
 */
#define IPIPSEC_CTRLT_GET_SELECTOR                    0xff050001  /* Ipipsec_ctrl_selector* */

/* -- MIB entity IPsec statistics */
#define IPIPSEC_CTRLT_GET_ipsecEspCurrentInboundSAs   0xff050010  /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ipsecEspTotalInboundSAs     0xff050011  /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ipsecEspCurrentOutboundSAs  0xff050012  /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ipsecEspTotalOutboundSAs    0xff050013  /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ipsecAhCurrentInboundSAs    0xff050014  /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ipsecAhTotalInboundSAs      0xff050015  /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ipsecAhCurrentOutboundSAs   0xff050016  /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ipsecAhTotalOutboundSAs     0xff050017  /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ipsecDecryptionErrors       0xff050018  /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ipsecAuthenticationErrors   0xff050019  /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ipsecReplayErrors           0xff05001a  /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ipsecPolicyErrors           0xff05001b  /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ipsecOtherReceiveErrors     0xff05001c  /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ipsecSendErrors             0xff05001d  /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ipsecUnknownSpiErrors       0xff05001e  /* Ip_u32 */

/* -- MIB trap control */

/* check if trap enabled/disabled */
#define IPIPSEC_CTRLT_GET_espAuthFailureTrapEnable     0xff050020 /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ahAuthFailureTrapEnable      0xff050021 /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_espReplayFailureTrapEnable   0xff050022 /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ahReplayFailureTrapEnable    0xff050023 /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_espPolicyFailureTrapEnable   0xff050024 /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_ahPolicyFailureTrapEnable    0xff050025 /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_invalidSpiTrapEnable         0xff050026 /* Ip_u32 */
#define IPIPSEC_CTRLT_GET_otherPolicyFailureTrapEnable 0xff050027 /* Ip_u32 */

/* enable/disable trap */
#define IPIPSEC_CTRLT_SET_espAuthFailureTrapEnable     0xff050028 /* Ip_u32 */
#define IPIPSEC_CTRLT_SET_ahAuthFailureTrapEnable      0xff050029 /* Ip_u32 */
#define IPIPSEC_CTRLT_SET_espReplayFailureTrapEnable   0xff05002a /* Ip_u32 */
#define IPIPSEC_CTRLT_SET_ahReplayFailureTrapEnable    0xff05002b /* Ip_u32 */
#define IPIPSEC_CTRLT_SET_espPolicyFailureTrapEnable   0xff05002c /* Ip_u32 */
#define IPIPSEC_CTRLT_SET_ahPolicyFailureTrapEnable    0xff05002d /* Ip_u32 */
#define IPIPSEC_CTRLT_SET_invalidSpiTrapEnable         0xff05002e /* Ip_u32 */
#define IPIPSEC_CTRLT_SET_otherPolicyFailureTrapEnable 0xff05002f /* Ip_u32 */

/* MIB trap function. If not set, no SA traps will be generated.
 */
#define IPIPSEC_CTRLT_SET_TRAP_FUNCTION               0xff05000a  /* void * */
typedef Ip_u32 (*Ipipsec_trap_func)(Ipipsec_sa_handle *sa, Ip_u32 traptype, Ip_tag *tags);

/* The trap callback function use the following tags: */
#define IPIPSEC_TRABCBT_NUM_ERRORS                     0xffff0001 /* Ip_u32 */
#define IPIPSEC_TRABCBT_LOCAL_ADDR                     0xffff0002 /* union Ip_in_addr_union* */
#define IPIPSEC_TRABCBT_PEER_ADDR                      0xffff0003 /* union Ip_in_addr_union* */
#define IPIPSEC_TRABCBT_SECURITY_PROTOCOL              0xffff0004 /* Ip_u32 */
#define IPIPSEC_TRABCBT_SPI                            0xffff0005 /* Ip_u32 */
#define IPIPSEC_TRABCBT_DOMAIN                         0xffff0007 /* Ip_u32 */


/*******************************************************************************
*
* ipipsec_sactrl - get SA-specific information and configuration values
*
* Get SA-specific information and configuration values.
*
* Parameters:
* \is
* \i <sah>
* An 'ipipsec_sa_handle' structure identifying the SA.
* \i <sactrl_tags>
* An array of tag/value pairs terminated by IP_TAG_END. Valid parameter tags are
* defined below. Values are 32-bit parameter values or pointers.
* \ie
*
* \ts
* IPIPSEC_SACTRLT_GET_NEXT_SA             | Ipipsec_sa_handle*
* IPIPSEC_SACTRLT_GET_SaSelector          | Ip_u32
* IPIPSEC_SACTRLT_GET_SaCreator           | Ip_u32
* IPIPSEC_SACTRLT_GET_Encapsulation       | Ip_u32
* IPIPSEC_SACTRLT_GET_EncAlg              | Ip_u32
* IPIPSEC_SACTRLT_GET_EncKeyLength        | Ip_u32
* IPIPSEC_SACTRLT_GET_AuthAlg             | Ip_u32
* IPIPSEC_SACTRLT_GET_AuthKeyLength       | Ip_u32
* IPIPSEC_SACTRLT_GET_RepWinSize          | Ip_u32
* IPIPSEC_SACTRLT_GET_LimitSeconds        | Ip_u32
* IPIPSEC_SACTRLT_GET_LimitKbytes         | Ip_u32
* IPIPSEC_SACTRLT_GET_AccSeconds          | Ip_u32
* IPIPSEC_SACTRLT_GET_InAccKbytes         | Ip_u32
* IPIPSEC_SACTRLT_GET_OutAccKbytes        | Ip_u32
* IPIPSEC_SACTRLT_GET_InUserOctets        | Ip_u64*
* IPIPSEC_SACTRLT_GET_OutUserOctets       | Ip_u64*
* IPIPSEC_SACTRLT_GET_InPackets           | Ip_u64*
* IPIPSEC_SACTRLT_GET_OutPackets          | Ip_u64*
* IPIPSEC_SACTRLT_GET_DecryptErrors       | Ip_u32
* IPIPSEC_SACTRLT_GET_AuthErrors          | Ip_u32
* IPIPSEC_SACTRLT_GET_ReplayErrors        | Ip_u32
* IPIPSEC_SACTRLT_GET_PolicyErrors        | Ip_u32
* IPIPSEC_SACTRLT_GET_PadErrors           | Ip_u32
* IPIPSEC_SACTRLT_GET_OtherReceiveErrors  | Ip_u32
* IPIPSEC_SACTRLT_GET_OutSendErrors       | Ip_u32
* IPIPSEC_SACTRLT_GET_Grouped             | Ipipsec_sa_handle*
* IPIPSEC_SACTRLT_GET_DstAddr             | size = sizeof(Ip_addr4)
* IPIPSEC_SACTRLT_GET_SrcAddr             | size = sizeof(Ip_addr4)
* IPIPSEC_SACTRLT_GET_State               | size = sizeof(Ip_u32)
* IPIPSEC_SACTRLT_GET_DstAddr6            | size = 16
* IPIPSEC_SACTRLT_GET_SrcAddr6            | size = 16
* \te
*
* \cs
* typedef struct Ipipsec_sa_handle_struct
* {
*    union Ip_in_addr_union   dst;   /@ address in network endian. @/
*
*    Ip_u32   spi_n;        /@ SPI in network endian. @/
*
*    Ip_u8    protocol;     /@ IP_IPPROTO_ESP, IP_IPPROTO_AH or 0 for any. @/
*    Ip_u8    direction;    /@ See valid values below. @/
*    Ip_u8    domain;       /@ IP_AF_INET or IP_AF_INET6 @/
*    Ip_u8    satype;       /@ SADB_SATYPE_xx. @/
* }
* Ipipsec_sa_handle;
*
* /@ 'direction' values: @/
* #define IPIPSEC_SADIR_INPUT   0
* #define IPIPSEC_SADIR_OUTPUT  1
* #define IPIPSEC_SADIR_ANY     2
* \ce
*
* EXAMPLE
* \cs
*
* int example(void)
* {
*     Ipipsec_sa_handle   sa, sa_next, sa_group;
*     int       num;
*     Ip_err    retval;
*     Ip_tag    tags[7];
* #ifdef IPCOM_USE_INET6
*     char str[40];
* #else
*     char str[16];
* #endif
*
*     (void)vars;
*
*     /@ Print all SAs. @/
*     ipcom_printf("SA - Security Associations:"IP_LF);
*
*     sa_next.spi_n = 0;  /@ get first SA. @/
*
*     for(num = 0;;)
*     {
*         ipcom_memcpy(&sa, &sa_next, sizeof(Ipipsec_sa_handle));
*         sa_next.direction = IPIPSEC_SADIR_ANY;
*         sa_next.protocol = 0;
*         sa_next.domain = 0;
*         tags[0] = IPIPSEC_SACTRLT_GET_NEXT_SA;
*         tags[1] = (Ip_tag)&sa_next;
*         tags[2] = IP_TAG_END;
*         retval = ipipsec_sactrl(&sa, tags);
*         if(retval != IPCOM_SUCCESS)
*         {
*             ipcom_printf("ipsecctrl: ipipsec_sactrl(next) failed, error =
* 				%ld"IP_LF, retval);
*             return -1;
*         }
*
*         if(sa_next.spi_n == 0)
*             break;
*
*         /@ Get additional information. @/
*         tags[0] = IPIPSEC_SACTRLT_GET_AuthAlg;
*         tags[2] = IPIPSEC_SACTRLT_GET_EncAlg;
*         tags[4] = IPIPSEC_SACTRLT_GET_Grouped;
*         tags[5] = (Ip_tag)&sa_group;
*         tags[6] = IP_TAG_END;
*
*         retval = ipipsec_sactrl(&sa_next, tags);
*         if(retval != IPCOM_SUCCESS)
*         {
*             ipcom_printf("ipsecctrl: ipipsec_sactrl(info) failed, error =
* 				%ld"IP_LF, retval);
*             return -1;
*         }
*
*         ipcom_printf("[%2d] ::  %-6s %s spi=0x%-5lx dst=%-15s auth=%s",
*                      ++num,
*                      sa_next.direction == 0 ? "input" : sa_next.direction ==
* 						 1 ? "output" : "any",
*                      ipcom_ipproto_name(sa_next.protocol),
*                      ip_ntohl(sa_next.spi_n),
*                      ipcom_inet_ntop(sa_next.domain, &sa_next.dst, str,
* 						  sizeof(str)),
*                      mib_AuthAlg[tags[1]]);
*         if(sa_next.protocol == IP_IPPROTO_ESP)
*             ipcom_printf(" enc=%s", mib_EncAlg[tags[3]]);
*         ipcom_printf(IP_LF);
*         if(sa_group.spi_n)
*             ipcom_printf("           ->   %s spi=0x%-5lx dst=%-15s"IP_LF,
*                          ipcom_ipproto_name(sa_group.protocol),
*                          ip_ntohl(sa_group.spi_n),
*                          ipcom_inet_ntop(sa_group.domain, &sa_next.dst, str,
* 							  sizeof(str)));
*     }
*
*     ipcom_printf("Total of %d SAs."IP_LF, num);
*     return 0;
* }
* \ce
*
* RETURNS: 'IPCOM_SUCCESS', or one of the following errors:
* 'IPCOM_ERR_INVALID_TAG_VALUE', 'IPCOM_ERR_UNSUPPORTED_TAG'
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipipsec_sactrl
(
    Ipipsec_sa_handle *sah,
    Ip_tag *sactrl_tags
);


/* Get first/next lexicographic SA relative to the specified SA 'sa'.
 *
 * Set sah->spi_n to 0 when retrieving the first SA.
 * In order to get the next SA, use the IPIPSEC_SACTRLT_GET_NEXT_SA tag.
 * If no additional SAs are found, the spi_n will be set to 0 in the
 * tag value. Check src/ipipsec_cmd_ipsecctrl.c for an example.
 *
 * For additional info on 'sa' entries, check the text on Ipipsec_sa_handle.
 *
 */
#define IPIPSEC_SACTRLT_GET_NEXT_SA             0xff070001  /* Ipipsec_sa_handle* */

/* -- MIB SA : selector */
#define IPIPSEC_SACTRLT_GET_SaSelector          0xff070010  /* Ip_u32 */

/* -- MIB SA : how created */
#define IPIPSEC_SACTRLT_GET_SaCreator           0xff070011  /* Ip_u32 */

/* -- MIB SA : security services description */
#define IPIPSEC_SACTRLT_GET_Encapsulation       0xff070012  /* Ip_u32 */
#define IPIPSEC_SACTRLT_GET_EncAlg              0xff070013  /* Ip_u32 */
#define IPIPSEC_SACTRLT_GET_EncKeyLength        0xff070014  /* Ip_u32 */
#define IPIPSEC_SACTRLT_GET_AuthAlg             0xff070015  /* Ip_u32 */
#define IPIPSEC_SACTRLT_GET_AuthKeyLength       0xff070016  /* Ip_u32 */
#define IPIPSEC_SACTRLT_GET_RepWinSize          0xff070017  /* Ip_u32 */

/* -- MIB SA : expiration limits */
#define IPIPSEC_SACTRLT_GET_LimitSeconds        0xff070018  /* Ip_u32 */
#define IPIPSEC_SACTRLT_GET_LimitKbytes         0xff070019  /* Ip_u32 */

/* -- MIB SA : current operating statistics */
#define IPIPSEC_SACTRLT_GET_AccSeconds          0xff07001a  /* Ip_u32 */
#define IPIPSEC_SACTRLT_GET_InAccKbytes         0xff07001b  /* Ip_u32 */
#define IPIPSEC_SACTRLT_GET_OutAccKbytes        0xff07001c  /* Ip_u32 */
#define IPIPSEC_SACTRLT_GET_InUserOctets        0xff07001d  /* Ip_u64* */
#define IPIPSEC_SACTRLT_GET_OutUserOctets       0xff07001e  /* Ip_u64* */
#define IPIPSEC_SACTRLT_GET_InPackets           0xff07001f  /* Ip_u64* */
#define IPIPSEC_SACTRLT_GET_OutPackets          0xff070020  /* Ip_u64* */

/* -- MIB SA : error statistics */
#define IPIPSEC_SACTRLT_GET_DecryptErrors       0xff070021  /* Ip_u32 */
#define IPIPSEC_SACTRLT_GET_AuthErrors          0xff070022  /* Ip_u32 */
#define IPIPSEC_SACTRLT_GET_ReplayErrors        0xff070023  /* Ip_u32 */
#define IPIPSEC_SACTRLT_GET_PolicyErrors        0xff070024  /* Ip_u32 */
#define IPIPSEC_SACTRLT_GET_PadErrors           0xff070025  /* Ip_u32 */
#define IPIPSEC_SACTRLT_GET_OtherReceiveErrors  0xff070026  /* Ip_u32 */
#define IPIPSEC_SACTRLT_GET_OutSendErrors       0xff070027  /* Ip_u32 */

/* -- Interpeak proprietary (i.e. non MIB) SA tags: */
#define IPIPSEC_SACTRLT_GET_Grouped             0xff070030  /* Ipipsec_sa_handle* */
#define IPIPSEC_SACTRLT_GET_DstAddr             0xff070031  /* size = sizeof(Ip_addr4) */
#define IPIPSEC_SACTRLT_GET_SrcAddr             0xff070032  /* size = sizeof(Ip_addr4) */
#define IPIPSEC_SACTRLT_GET_State               0xff070033  /* size = sizeof(Ip_u32) */
#define IPIPSEC_SACTRLT_GET_DstAddr6            0xff070034  /* size = 16 */
#define IPIPSEC_SACTRLT_GET_SrcAddr6            0xff070035  /* size = 16 */
#define IPIPSEC_SACTRLT_GET_LAST                0xff070035  /* Must be set to last! */


/*
 *===========================================================================
 *                    ipipsec_conf
 *===========================================================================
 * Description:   Get/set global information/configuration
 * Parameters:    conf_tags  A tag array.
 * Returns:       IPCOM_ERR_UNSUPPORTED_TAG - The optional configuration
 *                          tag array contained an unknown tag.
 *                IPCOM_SUCCESS - Success.
 *
 */
IP_PUBLIC Ip_err ipipsec_conf(Ip_tag *conf_tags);

#define IPIPSEC_CONF_GET_ENABLE_OUTFILTER       0xfffff001 /* Ip_u32 */
#define IPIPSEC_CONF_SET_ENABLE_OUTFILTER       0xfffff002 /* Ip_u32 */
#define IPIPSEC_CONF_GET_ENABLE_INFILTER        0xfffff003 /* Ip_u32 */
#define IPIPSEC_CONF_SET_ENABLE_INFILTER        0xfffff004 /* Ip_u32 */
#define IPIPSEC_CONF_GET_ENABLE_ESP             0xfffff005 /* Ip_u32 */
#define IPIPSEC_CONF_SET_ENABLE_ESP             0xfffff006 /* Ip_u32 */
#define IPIPSEC_CONF_GET_ENABLE_AH              0xfffff007 /* Ip_u32 */
#define IPIPSEC_CONF_SET_ENABLE_AH              0xfffff008 /* Ip_u32 */
#define IPIPSEC_CONF_GET_ALLOW_IPIP             0xfffff009 /* Ip_u32 */
#define IPIPSEC_CONF_SET_ALLOW_IPIP             0xfffff00a /* Ip_u32 */
#define IPIPSEC_CONF_GET_NUM_DAEMONS_ACTIVE     0xfffff00b /* Ip_u32 */
#define IPIPSEC_CONF_SET_NUM_DAEMONS_ACTIVE     0xfffff00c /* Ip_u32 */



/*
 ****************************************************************************
 * 6                    IPNET/IPLITE FUNCTIONS
 ****************************************************************************
 */

typedef void (*Ipipsec_timeout_handler)(void *cookie);


/*
 *===========================================================================
 *                    ipcom_ipsec_ioevent
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *ipcom_ipsec_ioevent(int command, void *data);


/*
 *===========================================================================
 *                     ipipsec_timeout_schedule
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void ipipsec_timeout_schedule(Ip_u32 sec,
                                        Ipipsec_timeout_handler handler,
                                        void *cookie,
                                        void **ptmo);


/*
 *===========================================================================
 *                    ipipsec_timeout_cancel
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void ipipsec_timeout_cancel(void *tmo);


/*
 *===========================================================================
 *                    ipipsec_seconds_now
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_u32 ipipsec_seconds_now(void);


/*
 *===========================================================================
 *                    ipipsec_timeout_to_string
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPNET
typedef int (*Ipipsec_timeout_to_string_f)(void *tmo_elem, char *buf, Ip_size_t buf_len);

IP_PUBLIC void ipipsec_timeout_to_string(Ipipsec_timeout_handler handler,
                                         Ipipsec_timeout_to_string_f to_str);
#endif


/*
 ****************************************************************************
 * 7                    PUBLIC FUNCTIONS
 ****************************************************************************
 * Functions that IPNET/IPLITE must call to use IPIPSEC.
 */

/*
 *===========================================================================
 *                    ipipsec_output_hdrspace
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_u32
ipipsec_output_hdrspace(Ipipsec_param *param);


/*
 *===========================================================================
 *                    ipipsec_3tuple_match
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_bool
ipipsec_3tuple_match(Ipcom_pkt *pkt, Ipipsec_param *param);


/*
 *===========================================================================
 *                   ipipsec_pfkeyv2_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipipsec_pfkeyv2_input(Ipcom_socket *sock, IP_CONST struct Ip_msghdr *msg, Ipcom_pkt *pkt);


/*
 *===========================================================================
 *                    ipipsec_policy_check
 *===========================================================================
 * Description:  Check if pkt passes IPSEc policies
 * Parameters:
 * Returns:      0 for success, else -IP_ERRNO_EACCES
 *
 */
IP_PUBLIC int
ipipsec_policy_check(Ipcom_pkt *pkt, Ipipsec_param *param);


/*
 *===========================================================================
 *                    ipipsec_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:   IPIPSEC_OK
 *            IPIPSEC_TUNNELED
 *            IPIPSEC_KEPT
 *            negative errno
 */
IP_PUBLIC int
ipipsec_input(Ipcom_pkt *pkt, Ipipsec_param *param);


/*
 *===========================================================================
 *                    ipipsec_output
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:   IPIPSEC_OK
 *            IPIPSEC_TUNNELED
 *            IPIPSEC_KEPT
 *            negative errno
 */
IP_PUBLIC int
ipipsec_output(Ipcom_pkt **pktp, Ipipsec_param *param);


/*
 *===========================================================================
 *                    ipipsec_send_daemon_pipe
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:   IPIPSEC_OK
 *            IPIPSEC_KEPT
 *            negative errno
 */
IP_PUBLIC int
ipipsec_send_daemon_pipe(Ipcom_pkt *pkt, Ipipsec_param *param, Ip_u8 input);


/*
 *===========================================================================
 *                    ipipsec_sys_ioctl
 *===========================================================================
 * Description: IO control.
 * Parameters:  fd - A socket descriptor.
 *              request - The IO control request.
 *              argp - Request specific data.
 * Returns:     0 = success, -1 = error (error code from ipcom_errno).
 *
 */
IP_PUBLIC int
ipipsec_sys_ioctl(Ip_fd fd, unsigned long request, void *argp);


#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
