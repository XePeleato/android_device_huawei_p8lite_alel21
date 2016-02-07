/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile:
 *   ipcom_ipd_config.c,v $ $Revision: 1.15.12.1.4.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipcom_ipd_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Lennart Bång <lob@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01m,23nov09,rjq   Added INCLUDE_IPSNTPS_API
01l,29jul09,ten   Added INCLUDE_IPL2TP
01k,16dec08,mry   Added INCLUDE_IPWPS
01j,20feb08,tlu   Added INCLUDE_IPROHC
01i,08nov07,tlu   Added INCLUDE_IPDIAMETERP
01i,10sep07,mry   Add IP8021X_SUPPLICANT support
01h,19sep07,mca   Fix start order of IPCRYPTO and IPSSL
01g,12sep07,spw   Change INCLUDE_DOT11_END to INCLUDE_IPWLAN_ATHEROS
01f,14aug07,tlu   Rename Diameter task name from ipdiametercd to ipdiameterc
		  to be consist with other IP task names
01e,27jul07,spw   Added INCLUDE_IPWLAN_BROADCOM, INCLUDE_DOT11_END
01d,10jul07,tat   Removed INCLUDE_IPCCI
01c,27jun07,mca  Added INCLUDE_IPFREESCALE
01b,10apr07,tlu   Added INCLUDE_DIAMETERC
01a,08jan07,kch   Added ipcom_ipd_products[] configuration here. Also removed
                  unsupported ipproducts from the product array.
*/

/*
 ****************************************************************************
 * 1                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_config.h"
#include "ipcom_ipd.h"
#include "ipcom_os.h"


/*
 ****************************************************************************
 * 2                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#ifdef INCLUDE_IPTELNETS
IP_EXTERN Ip_err iptelnets_start(void);
IP_EXTERN const char *iptelnets_version(void);
#endif

#ifdef INCLUDE_IPNET
IP_EXTERN Ip_err ipnet_create(void);
IP_EXTERN Ip_err ipnet_configure(void);
IP_EXTERN Ip_err ipnet_start(void);
IP_EXTERN const char *ipnet_version(void);
#endif

#ifdef INCLUDE_IPVRRPD
IP_EXTERN Ip_err ipnet_vrrpd_create(void);
IP_EXTERN Ip_err ipnet_vrrpd_configure(void);
IP_EXTERN Ip_err ipnet_vrrpd_start(void);
IP_EXTERN const char *ipnet_vrrpd_version(void);
#endif

#ifdef INCLUDE_IPRADVD
IP_EXTERN Ip_err ipnet_radvd_create(void);
IP_EXTERN Ip_err ipnet_radvd_configure(void);
IP_EXTERN Ip_err ipnet_radvd_start(void);
IP_EXTERN const char *ipnet_radvd_version(void);
#endif

#ifdef INCLUDE_IPSEND
IP_PUBLIC Ip_err ipnet_send_create(void);
IP_PUBLIC Ip_err ipnet_send_configure(void);
IP_PUBLIC Ip_err ipnet_send_start(void);
IP_PUBLIC const char *ipnet_send_version(void);
#endif

#ifdef INCLUDE_IPTCP
IP_EXTERN Ip_err iptcp_create(void);
IP_EXTERN Ip_err iptcp_configure(void);
IP_EXTERN Ip_err iptcp_start(void);
IP_EXTERN const char *iptcp_version(void);
#endif

#ifdef INCLUDE_IPPPP
IP_EXTERN Ip_err ipppp_create(void);
IP_EXTERN Ip_err ipppp_configure(void);
IP_EXTERN Ip_err ipppp_start(void);
IP_EXTERN const char *ipppp_version(void);
#endif

#ifdef INCLUDE_IPL2TP
IP_PUBLIC Ip_err ipl2tp_create(void);
IP_PUBLIC Ip_err ipl2tp_configure(void);
IP_PUBLIC Ip_err ipl2tp_start(void);
IP_PUBLIC const char *ipl2tp_version(void);
#endif

#ifdef INCLUDE_IPMIPMN
IP_EXTERN Ip_err ipmipmn_create(void);
IP_EXTERN Ip_err ipmipmn_configure(void);
IP_EXTERN Ip_err ipmipmn_start(void);
IP_EXTERN const char *ipmipmn_version(void);
#endif

#ifdef INCLUDE_IPMIPHA
IP_EXTERN Ip_err ipmipha_start(void);
IP_EXTERN const char *ipmipha_version(void);
#endif

#ifdef INCLUDE_IPMIPPM
IP_EXTERN Ip_err ipmippm_start(void);
IP_EXTERN const char *ipmippm_version(void);
#endif

#ifdef INCLUDE_IPMIPFA
IP_EXTERN Ip_err ipmipfa_start(void);
IP_EXTERN const char *ipmipfa_version(void);
#endif

#ifdef INCLUDE_IPMIP6MN
IP_EXTERN Ip_err ipmip6mn_start(void);
IP_EXTERN const char *ipmip6mn_version(void);
#endif

#ifdef INCLUDE_IPMIP6HA
IP_EXTERN Ip_err ipmip6ha_start(void);
IP_EXTERN const char *ipmip6ha_version(void);
#endif

#ifdef INCLUDE_IPMIP6PM
IP_EXTERN Ip_err ipmip6pm_start(void);
IP_EXTERN const char *ipmip6pm_version(void);
#endif

#ifdef INCLUDE_IPTFTPS
IP_EXTERN Ip_err iptftps_create(void);
IP_EXTERN Ip_err iptftps_configure(void);
IP_EXTERN Ip_err iptftps_start(void);
IP_EXTERN const char *iptftps_version(void);
#endif

#ifdef INCLUDE_IPFTPS
IP_EXTERN Ip_err ipftps_create(void);
IP_EXTERN Ip_err ipftps_configure(void);
IP_EXTERN Ip_err ipftps_start(void);
IP_EXTERN const char *ipftps_version(void);
#endif

#ifdef INCLUDE_IPRADIUS
IP_EXTERN Ip_err ipradius_create(void);
IP_EXTERN Ip_err ipradius_configure(void);
IP_EXTERN Ip_err ipradius_start(void);
IP_EXTERN const char *ipradius_version(void);
#endif

#ifdef INCLUDE_IPDIAMETERC
IP_EXTERN Ip_err ipdiameterc_create(void);
IP_EXTERN Ip_err ipdiameterc_configure(void);
IP_EXTERN Ip_err ipdiameterc_start(void);
IP_EXTERN const char *ipdiameter_version(void);
#endif

#ifdef INCLUDE_IPDIAMETERP
IP_EXTERN Ip_err ipdiameterp_create(void);
IP_EXTERN Ip_err ipdiameterp_configure(void);
IP_EXTERN Ip_err ipdiameterp_start(void);
IP_EXTERN const char *ipdiameter_version(void);
#endif

#ifdef INCLUDE_IP_SECURITY
IP_EXTERN Ip_err ipipsec_create(void);
IP_EXTERN Ip_err ipipsec_configure(void);
IP_EXTERN Ip_err ipipsec_start(void);
IP_EXTERN const char *ipipsec_version(void);
#endif

#ifdef INCLUDE_INTERNET_KEY_EXCHANGE
IP_EXTERN Ip_err ipike_create(void);
IP_EXTERN Ip_err ipike_configure(void);
IP_EXTERN Ip_err ipike_start(void);
IP_EXTERN const char *ipike_version(void);
#endif

#ifdef INCLUDE_SSH
IP_EXTERN Ip_err ipssh_create(void);
IP_EXTERN Ip_err ipssh_configure(void);
IP_EXTERN Ip_err ipssh_start(void);
IP_EXTERN const char *ipssh_version(void);
#endif

#ifdef INCLUDE_IPFIREWALL
IP_EXTERN Ip_err ipfirewall_create(void);
IP_EXTERN Ip_err ipfirewall_configure(void);
IP_EXTERN Ip_err ipfirewall_start(void);
IP_EXTERN const char *ipfirewall_version(void);
#endif

#ifdef INCLUDE_IPDHCPS
IP_EXTERN Ip_err ipdhcps_create(void);
IP_EXTERN Ip_err ipdhcps_configure(void);
IP_EXTERN Ip_err ipdhcps_start(void);
IP_EXTERN const char *ipdhcps_version(void);
#endif

#ifdef INCLUDE_IPDHCPC6
IP_PUBLIC Ip_err ipdhcpc6_create(void);
IP_PUBLIC Ip_err ipdhcpc6_configure(void);
IP_PUBLIC Ip_err ipdhcpc6_start(void);
IP_PUBLIC const char *ipdhcpc6_version(void);
#endif

#ifdef INCLUDE_IPDHCPS6
IP_EXTERN Ip_err ipdhcps6_create(void);
IP_EXTERN Ip_err ipdhcps6_configure(void);
IP_EXTERN Ip_err ipdhcps6_start(void);
IP_EXTERN const char *ipdhcps6_version(void);
#endif

#ifdef INCLUDE_IPDHCPR
IP_EXTERN Ip_err ipdhcpr_create(void);
IP_EXTERN Ip_err ipdhcpr_configure(void);
IP_EXTERN Ip_err ipdhcpr_start(void);
IP_EXTERN const char *ipdhcpr_version(void);
#endif

#ifdef INCLUDE_IPRIP
IP_EXTERN Ip_err iprip_create(void);
IP_EXTERN Ip_err iprip_configure(void);
IP_EXTERN Ip_err iprip_start(void);
IP_EXTERN const char *iprip_version(void);
#endif

#ifdef INCLUDE_RIPNG
IP_EXTERN Ip_err ipripng_create(void);
IP_EXTERN Ip_err ipripng_configure(void);
IP_EXTERN const char *ipripng_version(void);
#endif

#if defined(INCLUDE_IPSNTPS) || defined(INCLUDE_IPSNTPC) || defined(INCLUDE_IPSNTPC_API) || defined(INCLUDE_IPSNTPS_API)
# if defined(INCLUDE_IPSNTPS) || defined(INCLUDE_IPSNTPC)
#  define IPSNTP_IPD_FLAG IPCOM_IPD_FLAG_IPD_START
# else
#  define IPSNTP_IPD_FLAG 0
# endif
IP_EXTERN Ip_err ipsntp_create(void);
IP_EXTERN Ip_err ipsntp_configure(void);
IP_EXTERN Ip_err ipsntp_start(void);
IP_EXTERN const char *ipsntp_version(void);
#endif

#ifdef INCLUDE_IPDHCPC
IP_EXTERN Ip_err ipdhcpc_create(void);
IP_EXTERN Ip_err ipdhcpc_configure(void);
IP_EXTERN Ip_err ipdhcpc_start(void);
IP_EXTERN const char *ipdhcpc_version(void);
#endif

#ifdef INCLUDE_IPDNSC
IP_EXTERN Ip_err ipdnsc_create(void);
IP_EXTERN Ip_err ipdnsc_configure(void);
IP_EXTERN Ip_err ipdnsc_start(void);
IP_EXTERN const char *ipdnsc_version(void);
#endif

#ifdef INCLUDE_IPTFTPC
IP_EXTERN Ip_err iptftpc_create(void);
IP_EXTERN Ip_err iptftpc_configure(void);
IP_EXTERN Ip_err iptftpc_start(void);
IP_EXTERN const char *iptftpc_version(void);
#endif

#ifdef INCLUDE_IPFTPC
IP_EXTERN Ip_err ipftpc_create(void);
IP_EXTERN Ip_err ipftpc_configure(void);
IP_EXTERN Ip_err ipftpc_start(void);
IP_EXTERN const char *ipftpc_version(void);
#endif

#ifdef INCLUDE_IPMPLS
IP_EXTERN Ip_err ipmpls_create(void);
IP_EXTERN Ip_err ipmpls_configure(void);
IP_EXTERN Ip_err ipmpls_start(void);
IP_EXTERN const char *ipmpls_version(void);
#endif

#ifdef INCLUDE_IPMCP
IP_EXTERN Ip_err ipmcp_create(void);
IP_EXTERN Ip_err ipmcp_configure(void);
IP_EXTERN Ip_err ipmcp_start(void);
IP_EXTERN const char *ipmcp_version(void);
#endif

#ifdef INCLUDE_IP8021X_SUPPLICANT
IP_PUBLIC Ip_err ip8021x_create(void);
IP_PUBLIC Ip_err ip8021x_configure(void);
IP_PUBLIC Ip_err ip8021x_start(void);
IP_PUBLIC const char *ip8021x_version(void);
#endif

#ifdef INCLUDE_IPEAP
IP_EXTERN Ip_err ipeap_create(void);
IP_EXTERN Ip_err ipeap_configure(void);
IP_EXTERN Ip_err ipeap_start(void);
IP_EXTERN const char *ipeap_version(void);
#endif

#ifdef INCLUDE_IPWPS
IP_PUBLIC Ip_err ipwps_create(void);
IP_PUBLIC Ip_err ipwps_configure(void);
IP_PUBLIC Ip_err ipwps_start(void);
IP_PUBLIC const char *ipwps_version(void);
#endif

#ifdef INCLUDE_IPSSL
IP_PUBLIC Ip_err ipssl_create(void);
IP_PUBLIC Ip_err ipssl_configure(void);
IP_PUBLIC Ip_err ipssl_start(void);
IP_PUBLIC const char *ipssl_version(void);
#endif

#ifdef INCLUDE_IPCRYPTO
IP_PUBLIC Ip_err ipcrypto_create(void);
IP_PUBLIC Ip_err ipcrypto_configure(void);
IP_PUBLIC Ip_err ipcrypto_start(void);
IP_PUBLIC const char *ipcrypto_version(void);
#endif


#ifdef INCLUDE_IPHWCRYPTO
IP_EXTERN Ip_err iphwcrypto_create(void);
IP_EXTERN Ip_err iphwcrypto_configure(void);
IP_EXTERN Ip_err iphwcrypto_start(void);
IP_EXTERN const char *iphwcrypto_version(void);
#endif

#ifdef INCLUDE_IPFREESCALE
IP_EXTERN Ip_err ipfreescale_create(void);
IP_EXTERN Ip_err ipfreescale_configure(void);
IP_EXTERN Ip_err ipfreescale_start(void);
IP_EXTERN const char *ipfreescale_version(void);
#endif

#ifdef INCLUDE_IPCAVIUM
IP_PUBLIC Ip_err ipcavium_create(void);
IP_PUBLIC Ip_err ipcavium_configure(void);
IP_PUBLIC Ip_err ipcavium_start(void);
IP_PUBLIC const char *ipcavium_version(void);
#endif

#ifdef IPNETPERF
IP_PUBLIC Ip_err ipnetperf_create(void);
IP_PUBLIC Ip_err ipnetperf_configure(void);
IP_PUBLIC Ip_err ipnetperf_start(void);
IP_PUBLIC const char *ipnetperf_version(void);
#endif

#ifdef INCLUDE_IPWLAN_BROADCOM
IP_EXTERN Ip_err ipwlan_broadcom_create(void);
IP_EXTERN Ip_err ipwlan_broadcom_configure(void);
IP_EXTERN Ip_err ipwlan_broadcom_start(void);
IP_EXTERN const char *ipwlan_broadcom_version(void);
#endif

#ifdef INCLUDE_IPWLAN_ATHEROS
IP_EXTERN Ip_err ipwlan_atheros_create(void);
IP_EXTERN Ip_err ipwlan_atheros_configure(void);
IP_EXTERN Ip_err ipwlan_atheros_start(void);
IP_EXTERN const char *ipwlan_atheros_version(void);
#endif

#ifdef INCLUDE_IPROHC
IP_EXTERN Ip_err iprohc_create(void);
IP_EXTERN Ip_err iprohc_configure(void);
IP_EXTERN Ip_err iprohc_start(void);
IP_EXTERN const char *iprohc_version(void);
#endif

#ifdef INCLUDE_IPSCTP
IP_PUBLIC Ip_err ipsctp_create(void);
IP_PUBLIC Ip_err ipsctp_configure(void);
IP_PUBLIC Ip_err ipsctp_start(void);
IP_PUBLIC const char *ipsctp_version(void);
#endif

/*
 ****************************************************************************
 * 2                    DATA
 ****************************************************************************
 */

IP_GLOBAL const Ipcom_ipd_product ipcom_ipd_products[] =
{
#ifdef INCLUDE_IPNET
    { "ipnet", ipnet_create, ipnet_configure, ipnet_start, ipnet_version, IPCOM_IPD_ID_IPNET, IPCOM_LOG_IPNET, 0},
#endif

#ifdef INCLUDE_IPVRRPD
    { "ipnet_vrrpd", ipnet_vrrpd_create, ipnet_vrrpd_configure, ipnet_vrrpd_start, ipnet_vrrpd_version, IPCOM_IPD_ID_IPNET_VRRPD, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START | IPCOM_IPD_FLAG_HALTED},
#endif

#ifdef INCLUDE_IPRADVD
    { "ipnet_radvd", ipnet_radvd_create, ipnet_radvd_configure, ipnet_radvd_start, ipnet_radvd_version, IPCOM_IPD_ID_IPNET_RADVD, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START | IPCOM_IPD_FLAG_HALTED},
#endif

#ifdef INCLUDE_IPSEND
    { "ipnet_sendd", ipnet_send_create, ipnet_send_configure, ipnet_send_start, ipnet_send_version, IPCOM_IPD_ID_IPNET_SEND, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START | IPCOM_IPD_FLAG_HALTED},
#endif

#ifdef INCLUDE_IPTCP
    { "iptcp", iptcp_create, iptcp_configure, iptcp_start, iptcp_version, IPCOM_IPD_ID_IPTCP, IPCOM_LOG_IPTCP, 0},
#endif

#ifdef INCLUDE_IPSCTP
    { "ipsctp", ipsctp_create, ipsctp_configure, ipsctp_start, ipsctp_version, IPCOM_IPD_ID_IPSCTP, IPCOM_LOG_IPSCTP, 0},
#endif

#ifdef INCLUDE_IPTELNETS
    { "iptelnets", IP_NULL, IP_NULL, iptelnets_start, iptelnets_version, IPCOM_IPD_ID_IPTELNETS, IPCOM_LOG_IPCOM_TELNET, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPCRYPTO
    { "ipcrypto", ipcrypto_create, ipcrypto_configure, ipcrypto_start, ipcrypto_version, IPCOM_IPD_ID_IPCRYPTO, IPCOM_LOG_IPCRYPTO, 0},
#endif

#ifdef INCLUDE_IPSSL
    { "ipssl", ipssl_create, ipssl_configure, ipssl_start, ipssl_version, IPCOM_IPD_ID_IPSSL, IPCOM_LOG_IPSSL, 0},
#endif

#ifdef INCLUDE_IPRADIUS
    { "ipradius", ipradius_create, ipradius_configure, ipradius_start, ipradius_version, IPCOM_IPD_ID_IPRADIUS, IPCOM_LOG_IPRADIUS, 0},
#endif

#ifdef INCLUDE_IPPPP
    { "ipppp", ipppp_create, ipppp_configure, ipppp_start, ipppp_version, IPCOM_IPD_ID_IPPPP, IPCOM_LOG_IPPPP, 0},
#endif

#ifdef INCLUDE_IPL2TP
    { "ipl2tp", ipl2tp_create, ipl2tp_configure, ipl2tp_start, ipl2tp_version, IPCOM_IPD_ID_IPL2TP, IPCOM_LOG_IPL2TP, 0},
#endif

#ifdef INCLUDE_IPMIP6MN
    { "ipmip6mn", IP_NULL, IP_NULL, ipmip6mn_start, ipmip6mn_version, IPCOM_IPD_ID_IPMIP6MN, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPMIP6PM
    { "ipmip6pm", IP_NULL, IP_NULL, ipmip6pm_start, ipmip6pm_version, IPCOM_IPD_ID_IPMIP6PM, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPMIP6HA
    { "ipmip6ha", IP_NULL, IP_NULL, ipmip6ha_start, ipmip6ha_version, IPCOM_IPD_ID_IPMIP6HA, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPMIPMN
    { "ipmipmn", ipmipmn_create, ipmipmn_configure, ipmipmn_start, ipmipmn_version, IPCOM_IPD_ID_IPMIPMN, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPMIPHA
    { "ipmipha", IP_NULL, IP_NULL, ipmipha_start, ipmipha_version, IPCOM_IPD_ID_IPMIPHA, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPMIPFA
    { "ipmipfa", IP_NULL, IP_NULL, ipmipfa_start, ipmipfa_version, IPCOM_IPD_ID_IPMIPFA, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPMIPPM
    { "ipmippm", IP_NULL, IP_NULL, ipmippm_start, ipmippm_version, IPCOM_IPD_ID_IPMIPPM, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPTFTPS
    { "iptftps", iptftps_create, iptftps_configure, iptftps_start, iptftps_version, IPCOM_IPD_ID_IPTFTPS, IPCOM_LOG_IPTFTP, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPFTPS
    { "ipftps", ipftps_create, ipftps_configure, ipftps_start, ipftps_version, IPCOM_IPD_ID_IPFTPS, IPCOM_LOG_IPFTP, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IP_SECURITY
    { "ipipsec", ipipsec_create, ipipsec_configure, ipipsec_start, ipipsec_version, IPCOM_IPD_ID_IPIPSEC, IPCOM_LOG_IPIPSEC, 0},
#endif

#ifdef INCLUDE_SSH
    { "ipssh", ipssh_create, ipssh_configure, ipssh_start, ipssh_version, IPCOM_IPD_ID_IPSSH, IPCOM_LOG_IPSSH, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPFIREWALL
    { "ipfirewall", ipfirewall_create, ipfirewall_configure, ipfirewall_start, ipfirewall_version, IPCOM_IPD_ID_IPFIREWALL, IPCOM_LOG_IPFIREWALL, 0},
#endif

#ifdef INCLUDE_IPDHCPS
    { "ipdhcps", ipdhcps_create, ipdhcps_configure, ipdhcps_start, ipdhcps_version, IPCOM_IPD_ID_IPDHCPS, IPCOM_LOG_IPDHCPS, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPDHCPC6
    { "ipdhcpc6", IP_NULL, IP_NULL, ipdhcpc6_start, ipdhcpc6_version, IPCOM_IPD_ID_IPDHCPC6, IPCOM_LOG_IPDHCPC6, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPDHCPS6
    { "ipdhcps6", ipdhcps6_create, ipdhcps6_configure, ipdhcps6_start, ipdhcps6_version, IPCOM_IPD_ID_IPDHCPS6, IPCOM_LOG_IPDHCPS6, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPDHCPR
    { "ipdhcpr", ipdhcpr_create, ipdhcpr_configure, ipdhcpr_start, ipdhcpr_version, IPCOM_IPD_ID_IPDHCPR, IPCOM_LOG_IPDHCPR, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPRIP
    { "iprip", iprip_create, iprip_configure, iprip_start, iprip_version, IPCOM_IPD_ID_IPRIP, IPCOM_LOG_IPRIP, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_RIPNG
    { "ipripng", ipripng_create, ipripng_configure, 0, ipripng_version, IPCOM_IPD_ID_IPRIPNG, IPCOM_LOG_IPRIP, IPCOM_IPD_FLAG_IPD_START},
#endif

#if defined(INCLUDE_IPSNTPS) || defined(INCLUDE_IPSNTPC) || defined(INCLUDE_IPSNTPC_API) || defined(INCLUDE_IPSNTPS_API)
    { "ipsntp", ipsntp_create, ipsntp_configure, ipsntp_start, ipsntp_version, IPCOM_IPD_ID_IPSNTP, IPCOM_LOG_IPSNTP, IPSNTP_IPD_FLAG},
#endif

#ifdef INCLUDE_IPDHCPC
    { "ipdhcpc", ipdhcpc_create, ipdhcpc_configure, ipdhcpc_start, ipdhcpc_version, IPCOM_IPD_ID_IPDHCPC, IPCOM_LOG_IPDHCPC, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPDNSC
    { "ipdnsc", ipdnsc_create, ipdnsc_configure, ipdnsc_start, ipdnsc_version, IPCOM_IPD_ID_IPDNSC, IPCOM_LOG_NONE, 0},
#endif

#ifdef INCLUDE_IPTFTPC
    { "iptftpc", iptftpc_create, iptftpc_configure, iptftpc_start, iptftpc_version, IPCOM_IPD_ID_IPTFTPC, IPCOM_LOG_IPTFTP, 0},
#endif

#ifdef INCLUDE_IPFTPC
    { "ipftpc", ipftpc_create, ipftpc_configure, ipftpc_start, ipftpc_version, IPCOM_IPD_ID_IPFTPC, IPCOM_LOG_IPFTP, 0},
#endif

#ifdef INCLUDE_IPMPLS
    { "ipmpls", ipmpls_create, ipmpls_configure, ipmpls_start, ipmpls_version, IPCOM_IPD_ID_IPMPLS, IPCOM_LOG_IPMPLS, 0},
#endif

#ifdef INCLUDE_IPMCP
    { "ipmcp", ipmcp_create, ipmcp_configure, ipmcp_start, ipmcp_version, IPCOM_IPD_ID_IPMCP, IPCOM_LOG_IPMCP, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPEAP
    { "ipeap", ipeap_create, ipeap_configure, ipeap_start, ipeap_version, IPCOM_IPD_ID_IPEAP, IPCOM_LOG_IPEAP, 0},
#endif

#ifdef INCLUDE_IPHWCRYPTO
    { "iphwcrypto", iphwcrypto_create, iphwcrypto_configure, iphwcrypto_start, iphwcrypto_version, IPCOM_IPD_ID_IPHWCRYPTO, IPCOM_LOG_IPHWCRYPTO, 0},
#endif

#ifdef INCLUDE_IPFREESCALE
    { "ipfreescale", ipfreescale_create, ipfreescale_configure, ipfreescale_start, ipfreescale_version, IPCOM_IPD_ID_IPFREESCALE, IPCOM_LOG_IPFREESCALE, 0},
#endif

#ifdef INCLUDE_IPCAVIUM
    { "ipcavium", ipcavium_create, ipcavium_configure, ipcavium_start, ipcavium_version, IPCOM_IPD_ID_IPCAVIUM, IPCOM_LOG_IPCAVIUM, 0},
#endif

#ifdef IPNETPERF
    { "ipnetperf", ipnetperf_create, ipnetperf_configure, ipnetperf_start, ipnetperf_version, IPCOM_IPD_ID_IPNETPERF, IPCOM_LOG_NONE, 0},
#endif

#ifdef INCLUDE_IPWLAN_BROADCOM
    { "ipwlan_broadcom", ipwlan_broadcom_create, ipwlan_broadcom_configure, ipwlan_broadcom_start, ipwlan_broadcom_version, IPCOM_IPD_ID_IPWLAN, IPCOM_LOG_NONE, 0},
#endif

#ifdef INCLUDE_IPWLAN_ATHEROS
	{ "ipwlan_atheros", ipwlan_atheros_create, ipwlan_atheros_configure, ipwlan_atheros_start, ipwlan_atheros_version, IPCOM_IPD_ID_IPWLAN, IPCOM_LOG_NONE, 0},
#endif

#ifdef INCLUDE_IP8021X_SUPPLICANT
	/* Has to be after "ipwlan_broadcom" and "ipwlan_atheros" */
	{ "ip8021x", ip8021x_create, ip8021x_configure, ip8021x_start, ip8021x_version, IPCOM_IPD_ID_IP8021X, IPCOM_LOG_IP8021X, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPWPS
	/* Has to be after "ip8021x"  */
	{ "ipwps", ipwps_create, ipwps_configure, ipwps_start, ipwps_version, IPCOM_IPD_ID_IPWPS, IPCOM_LOG_IPWPS, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_INTERNET_KEY_EXCHANGE
    { "ipike", ipike_create, ipike_configure, ipike_start, ipike_version, IPCOM_IPD_ID_IPIKE, IPCOM_LOG_IPIKE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef INCLUDE_IPDIAMETERC
    { "ipdiameterc", ipdiameterc_create, ipdiameterc_configure, ipdiameterc_start, ipdiameter_version, IPCOM_IPD_ID_IPDIAMETERC, IPCOM_LOG_IPDIAMETER, 0},
#endif

#ifdef INCLUDE_IPDIAMETERP
    { "ipdiameterp", ipdiameterp_create, ipdiameterp_configure, ipdiameterp_start, ipdiameter_version, IPCOM_IPD_ID_IPDIAMETERP, IPCOM_LOG_IPDIAMETER, 0},
#endif

#ifdef INCLUDE_IPROHC
    { "iprohc", iprohc_create, iprohc_configure, iprohc_start, iprohc_version, IPCOM_IPD_ID_IPROHC, IPCOM_LOG_IPROHC, 0},
#endif

    { IP_NULL, IP_NULL, IP_NULL, IP_NULL, IP_NULL, -1, IPCOM_LOG_NONE, 0 }  /* end entry. */
};
