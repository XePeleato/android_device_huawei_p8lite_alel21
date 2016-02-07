/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_ipd.c,v $ $Revision: 1.94.20.3 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_ipd.c,v $
 *   $Author: dbush $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bång <lob@interpeak.se>
 ******************************************************************************
 */


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
#ifdef IPNET
#include <ipnet_config.h>
#endif

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */


#define IPCOM_USE_CLIB_PROTO
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_time.h"
#include "ipcom_os.h"
#include "ipcom_err.h"
#include "ipcom_sock.h"
#include "ipcom_ipd.h"
#include "ipcom_syslog.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_IPD_TIMEOUT   12

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM


#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */
#if !defined(WRS_IPNET) /* Not included here for new VxWorks build */

#ifdef IPTELNETS
IP_PUBLIC Ip_err iptelnets_start(void);
IP_PUBLIC const char *iptelnets_version(void);
#endif

#ifdef IPLITE
IP_PUBLIC Ip_err iplite_create(void);
IP_PUBLIC Ip_err iplite_configure(void);
IP_PUBLIC Ip_err iplite_start(void);
IP_PUBLIC const char *iplite_version(void);
#endif

#ifdef IPNET
#include <ipnet_config.h>
IP_PUBLIC Ip_err ipnet_create(void);
IP_PUBLIC Ip_err ipnet_configure(void);
IP_PUBLIC Ip_err ipnet_start(void);
IP_PUBLIC const char *ipnet_version(void);

#if defined(IPNET_USE_VRRP) && defined(IPCOM_USE_INET)
IP_PUBLIC Ip_err ipnet_vrrpd_create(void);
IP_PUBLIC Ip_err ipnet_vrrpd_configure(void);
IP_PUBLIC Ip_err ipnet_vrrpd_start(void);
IP_PUBLIC const char *ipnet_vrrpd_version(void);
#endif

#ifdef IPCOM_USE_INET6
IP_PUBLIC Ip_err ipnet_radvd_create(void);
IP_PUBLIC Ip_err ipnet_radvd_configure(void);
IP_PUBLIC Ip_err ipnet_radvd_start(void);
IP_PUBLIC const char *ipnet_radvd_version(void);
#endif /* IPCOM_USE_INET6 */

#ifdef IPNET_USE_RFC3971
IP_PUBLIC Ip_err ipnet_send_create(void);
IP_PUBLIC Ip_err ipnet_send_configure(void);
IP_PUBLIC Ip_err ipnet_send_start(void);
IP_PUBLIC const char *ipnet_send_version(void);
#endif /* IPNET_USE_RFC3971 */
#endif /* IPNET */

#ifdef IPTCP
IP_PUBLIC Ip_err iptcp_create(void);
IP_PUBLIC Ip_err iptcp_configure(void);
IP_PUBLIC Ip_err iptcp_start(void);
IP_PUBLIC const char *iptcp_version(void);
#endif

#ifdef IPPPP
IP_PUBLIC Ip_err ipppp_create(void);
IP_PUBLIC Ip_err ipppp_configure(void);
IP_PUBLIC Ip_err ipppp_start(void);
IP_PUBLIC const char *ipppp_version(void);
#endif

#ifdef IPPPPOE
IP_PUBLIC Ip_err ippppoe_create(void);
IP_PUBLIC Ip_err ippppoe_configure(void);
IP_PUBLIC Ip_err ippppoe_start(void);
IP_PUBLIC const char *ippppoe_version(void);
#endif

#ifdef IPMIPMN
IP_PUBLIC Ip_err ipmipmn_create(void);
IP_PUBLIC Ip_err ipmipmn_configure(void);
IP_PUBLIC Ip_err ipmipmn_start(void);
IP_PUBLIC const char *ipmipmn_version(void);
#endif

#ifdef IPMIPHA
IP_PUBLIC Ip_err ipmipha_start(void);
IP_PUBLIC const char *ipmipha_version(void);
#endif

#ifdef IPMIPFA
IP_PUBLIC Ip_err ipmipfa_start(void);
IP_PUBLIC const char *ipmipfa_version(void);
#endif

#ifdef IPMIP6MN
IP_PUBLIC Ip_err ipmip6mn_start(void);
IP_PUBLIC const char *ipmip6mn_version(void);
#endif

#ifdef IPMIP6HA
IP_PUBLIC Ip_err ipmip6ha_start(void);
IP_PUBLIC const char *ipmip6ha_version(void);
#endif

#ifdef IPMIP6PM
IP_PUBLIC Ip_err ipmip6pm_start(void);
IP_PUBLIC const char *ipmip6pm_version(void);
#endif

#ifdef IPMIP6CN
IP_PUBLIC Ip_err ipmip6cn_start(void);
IP_PUBLIC const char *ipmip6cn_version(void);
#endif

#ifdef IPMLDS
IP_PUBLIC Ip_err ipmlds_create(void);
IP_PUBLIC Ip_err ipmlds_configure(void);
IP_PUBLIC Ip_err ipmlds_start(void);
IP_PUBLIC const char *ipmlds_version(void);
#endif

#ifdef IPTFTPS
IP_PUBLIC Ip_err iptftps_create(void);
IP_PUBLIC Ip_err iptftps_configure(void);
IP_PUBLIC Ip_err iptftps_start(void);
IP_PUBLIC const char *iptftps_version(void);
#endif

#ifdef IPFTPS
IP_PUBLIC Ip_err ipftps_create(void);
IP_PUBLIC Ip_err ipftps_configure(void);
IP_PUBLIC Ip_err ipftps_start(void);
IP_PUBLIC const char *ipftps_version(void);
#endif

#ifdef IPRADIUS
IP_PUBLIC Ip_err ipradius_create(void);
IP_PUBLIC Ip_err ipradius_configure(void);
IP_PUBLIC Ip_err ipradius_start(void);
IP_PUBLIC const char *ipradius_version(void);
#endif

#ifdef IPL2TP
IP_PUBLIC Ip_err ipl2tp_create(void);
IP_PUBLIC Ip_err ipl2tp_configure(void);
IP_PUBLIC Ip_err ipl2tp_start(void);
IP_PUBLIC const char *ipl2tp_version(void);
#endif

#ifdef IPNAT
IP_PUBLIC Ip_err ipnat_create(void);
IP_PUBLIC Ip_err ipnat_configure(void);
IP_PUBLIC Ip_err ipnat_start(void);
IP_PUBLIC const char *ipnat_version(void);
#endif

#if defined(IPIPSEC) || defined(IPIPSEC2)
IP_PUBLIC Ip_err ipipsec_create(void);
IP_PUBLIC Ip_err ipipsec_configure(void);
IP_PUBLIC Ip_err ipipsec_start(void);
IP_PUBLIC const char *ipipsec_version(void);
#endif

#ifdef IPCCI
IP_PUBLIC Ip_err ipcci_create(void);
IP_PUBLIC Ip_err ipcci_configure(void);
IP_PUBLIC Ip_err ipcci_start(void);
IP_PUBLIC const char *ipcci_version(void);
#endif

#ifdef IPLDAPC
IP_PUBLIC Ip_err ipldapc_create(void);
IP_PUBLIC Ip_err ipldapc_configure(void);
IP_PUBLIC Ip_err ipldapc_start(void);
IP_PUBLIC const char *ipldapc_version(void);
#endif

#ifdef IPIKE
IP_PUBLIC Ip_err ipike_create(void);
IP_PUBLIC Ip_err ipike_configure(void);
IP_PUBLIC Ip_err ipike_start(void);
IP_PUBLIC const char *ipike_version(void);
#endif

#ifdef IPCRYPTO
IP_PUBLIC Ip_err ipcrypto_create(void);
IP_PUBLIC Ip_err ipcrypto_configure(void);
IP_PUBLIC Ip_err ipcrypto_start(void);
IP_PUBLIC const char *ipcrypto_version(void);
#endif

#ifdef IPSSL
IP_PUBLIC Ip_err ipssl_create(void);
IP_PUBLIC Ip_err ipssl_configure(void);
IP_PUBLIC Ip_err ipssl_start(void);
IP_PUBLIC const char *ipssl_version(void);
#endif

#ifdef IPSSLPROXY
IP_PUBLIC Ip_err ipsslproxy_create(void);
IP_PUBLIC Ip_err ipsslproxy_configure(void);
IP_PUBLIC Ip_err ipsslproxy_start(void);
IP_PUBLIC const char *ipsslproxy_version(void);
#endif

#ifdef IPSSH
IP_PUBLIC Ip_err ipssh_create(void);
IP_PUBLIC Ip_err ipssh_configure(void);
IP_PUBLIC Ip_err ipssh_start(void);
IP_PUBLIC const char *ipssh_version(void);
#endif

#ifdef IPFIREWALL
IP_PUBLIC Ip_err ipfirewall_create(void);
IP_PUBLIC Ip_err ipfirewall_configure(void);
IP_PUBLIC Ip_err ipfirewall_start(void);
IP_PUBLIC const char *ipfirewall_version(void);
#endif

#ifdef IPDHCPS
IP_PUBLIC Ip_err ipdhcps_create(void);
IP_PUBLIC Ip_err ipdhcps_configure(void);
IP_PUBLIC Ip_err ipdhcps_start(void);
IP_PUBLIC const char *ipdhcps_version(void);
#endif

#ifdef IPDHCPC6
IP_PUBLIC Ip_err ipdhcpc6_create(void);
IP_PUBLIC Ip_err ipdhcpc6_configure(void);
IP_PUBLIC Ip_err ipdhcpc6_start(void);
IP_PUBLIC const char *ipdhcpc6_version(void);
#endif

#ifdef IPDHCPS6
IP_PUBLIC Ip_err ipdhcps6_create(void);
IP_PUBLIC Ip_err ipdhcps6_configure(void);
IP_PUBLIC Ip_err ipdhcps6_start(void);
IP_PUBLIC const char *ipdhcps6_version(void);
#endif

#ifdef IPDHCPR
IP_PUBLIC Ip_err ipdhcpr_create(void);
IP_PUBLIC Ip_err ipdhcpr_configure(void);
IP_PUBLIC Ip_err ipdhcpr_start(void);
IP_PUBLIC const char *ipdhcpr_version(void);
#endif

#ifdef IPWEBS
IP_PUBLIC Ip_err ipwebs_create(void);
IP_PUBLIC Ip_err ipwebs_configure(void);
IP_PUBLIC Ip_err ipwebs_start(void);
IP_PUBLIC const char *ipwebs_version(void);
#endif

#ifdef IPRIP
IP_PUBLIC Ip_err iprip_create(void);
IP_PUBLIC Ip_err iprip_configure(void);
IP_PUBLIC Ip_err iprip_start(void);
IP_PUBLIC const char *iprip_version(void);
#endif

#ifdef IPSNTP
#include <ipsntp_config.h>
# ifdef IPSNTP_USE_DAEMON
#  define IPSNTP_IPD_FLAG IPCOM_IPD_FLAG_IPD_START
# else
#  define IPSNTP_IPD_FLAG 0
# endif
IP_PUBLIC Ip_err ipsntp_create(void);
IP_PUBLIC Ip_err ipsntp_configure(void);
IP_PUBLIC Ip_err ipsntp_start(void);
IP_PUBLIC const char *ipsntp_version(void);
#endif

#ifdef IPSNMP
IP_PUBLIC Ip_err ipsnmp_create(void);
IP_PUBLIC Ip_err ipsnmp_configure(void);
IP_PUBLIC Ip_err ipsnmp_start(void);
IP_PUBLIC const char *ipsnmp_version(void);
#endif

#ifdef IPEMANATE
IP_PUBLIC Ip_err ipemanate_create(void);
IP_PUBLIC Ip_err ipemanate_configure(void);
IP_PUBLIC Ip_err ipemanate_start(void);
IP_PUBLIC const char *ipemanate_version(void);
#endif

#ifdef IPFREESCALE
IP_PUBLIC Ip_err ipfreescale_create(void);
IP_PUBLIC Ip_err ipfreescale_configure(void);
IP_PUBLIC Ip_err ipfreescale_start(void);
IP_PUBLIC const char *ipfreescale_version(void);
#endif

#ifdef IPCAVIUM
IP_PUBLIC Ip_err ipcavium_create(void);
IP_PUBLIC Ip_err ipcavium_configure(void);
IP_PUBLIC Ip_err ipcavium_start(void);
IP_PUBLIC const char *ipcavium_version(void);
#endif

#ifdef ZEBOS
IP_PUBLIC Ip_err ipzebos_create(void);
IP_PUBLIC Ip_err ipzebos_configure(void);
IP_PUBLIC Ip_err ipzebos_start(void);
IP_PUBLIC const char *ipzebos_version(void);
#endif

#ifdef IPDC
IP_PUBLIC Ip_err ipdc_create(void);
IP_PUBLIC Ip_err ipdc_configure(void);
IP_PUBLIC Ip_err ipdc_start(void);
IP_PUBLIC const char *ipdc_version(void);
#endif

#ifdef IPNGC
IP_PUBLIC Ip_err ipngc_create(void);
IP_PUBLIC Ip_err ipngc_configure(void);
IP_PUBLIC Ip_err ipngc_start(void);
IP_PUBLIC const char *ipngc_version(void);
#endif

#ifdef IPQUAGGA
IP_PUBLIC Ip_err ipquagga_create(void);
IP_PUBLIC Ip_err ipquagga_configure(void);
IP_PUBLIC Ip_err ipquagga_start(void);
IP_PUBLIC const char *ipquagga_version(void);
#endif

#if defined(IPDHCPC) && defined(IPCOM_SOCK)
IP_PUBLIC Ip_err ipdhcpc_create(void);
IP_PUBLIC Ip_err ipdhcpc_configure(void);
IP_PUBLIC Ip_err ipdhcpc_start(void);
IP_PUBLIC const char *ipdhcpc_version(void);
#endif

#ifdef IPDNSC
IP_PUBLIC Ip_err ipdnsc_create(void);
IP_PUBLIC Ip_err ipdnsc_configure(void);
IP_PUBLIC Ip_err ipdnsc_start(void);
IP_PUBLIC const char *ipdnsc_version(void);
#endif

#ifdef IPTFTPC
IP_PUBLIC Ip_err iptftpc_create(void);
IP_PUBLIC Ip_err iptftpc_configure(void);
IP_PUBLIC Ip_err iptftpc_start(void);
IP_PUBLIC const char *iptftpc_version(void);
#endif

#ifdef IPFTPC
IP_PUBLIC Ip_err ipftpc_create(void);
IP_PUBLIC Ip_err ipftpc_configure(void);
IP_PUBLIC Ip_err ipftpc_start(void);
IP_PUBLIC const char *ipftpc_version(void);
#endif

#ifdef IPMPLS
IP_PUBLIC Ip_err ipmpls_create(void);
IP_PUBLIC Ip_err ipmpls_configure(void);
IP_PUBLIC Ip_err ipmpls_start(void);
IP_PUBLIC const char *ipmpls_version(void);
#endif

#ifdef IPMCP
IP_PUBLIC Ip_err ipmcp_create(void);
IP_PUBLIC Ip_err ipmcp_configure(void);
IP_PUBLIC Ip_err ipmcp_start(void);
IP_PUBLIC const char *ipmcp_version(void);
#endif

#ifdef IPPRISM
IP_PUBLIC Ip_err ipprism_create(void);
IP_PUBLIC Ip_err ipprism_configure(void);
IP_PUBLIC Ip_err ipprism_start(void);
IP_PUBLIC const char *ipprism_version(void);
#endif

#ifdef IP8021X
IP_PUBLIC Ip_err ip8021x_create(void);
IP_PUBLIC Ip_err ip8021x_configure(void);
IP_PUBLIC Ip_err ip8021x_start(void);
IP_PUBLIC const char *ip8021x_version(void);
#endif

#ifdef IPEAP
IP_PUBLIC Ip_err ipeap_create(void);
IP_PUBLIC Ip_err ipeap_configure(void);
IP_PUBLIC Ip_err ipeap_start(void);
IP_PUBLIC const char *ipeap_version(void);
#endif

#ifdef IPWPS
IP_PUBLIC Ip_err ipwps_create(void);
IP_PUBLIC Ip_err ipwps_configure(void);
IP_PUBLIC Ip_err ipwps_start(void);
IP_PUBLIC const char *ipwps_version(void);
#endif

#ifdef IPSAFENET
IP_PUBLIC Ip_err ipsafenet_create(void);
IP_PUBLIC Ip_err ipsafenet_configure(void);
IP_PUBLIC Ip_err ipsafenet_start(void);
IP_PUBLIC const char *ipsafenet_version(void);
#endif

#ifdef IPHWCRYPTO
IP_PUBLIC Ip_err iphwcrypto_create(void);
IP_PUBLIC Ip_err iphwcrypto_configure(void);
IP_PUBLIC Ip_err iphwcrypto_start(void);
IP_PUBLIC const char *iphwcrypto_version(void);
#endif

#ifdef IPNETPERF
IP_PUBLIC Ip_err ipnetperf_create(void);
IP_PUBLIC Ip_err ipnetperf_configure(void);
IP_PUBLIC Ip_err ipnetperf_start(void);
IP_PUBLIC const char *ipnetperf_version(void);
#endif

#ifdef IPNETSNMP
IP_PUBLIC Ip_err ipnetsnmp_create(void);
IP_PUBLIC Ip_err ipnetsnmp_configure(void);
IP_PUBLIC Ip_err ipnetsnmp_start(void);
IP_PUBLIC const char *ipnetsnmp_version(void);
#endif

#ifdef IPMARVELL
IP_PUBLIC Ip_err ipmarvell_create(void);
IP_PUBLIC Ip_err ipmarvell_configure(void);
IP_PUBLIC Ip_err ipmarvell_start(void);
IP_PUBLIC const char *ipmarvell_version(void);
#endif

#ifdef IPANVL
IP_PUBLIC Ip_err ipanvl_create(void);
IP_PUBLIC Ip_err ipanvl_configure(void);
IP_PUBLIC Ip_err ipanvl_start(void);
IP_PUBLIC const char *ipanvl_version(void);
#endif

#ifdef IP_PORT_OSE5
IP_PUBLIC Ip_err ipcom_ipd_isinstalled_ose5(const char *name);
IP_PUBLIC Ip_err ipcom_ipd_start_ose5(const char *name);
#endif

#ifdef IPDIAMETER
IP_EXTERN Ip_err ipdiameterc_create(void);
IP_EXTERN Ip_err ipdiameterc_configure(void);
IP_EXTERN Ip_err ipdiameterc_start(void);
IP_EXTERN const char *ipdiameter_version(void);
#endif

#ifdef IPMIPPM
IP_PUBLIC Ip_err ipmippm_start(void);
IP_PUBLIC const char *ipmippm_version(void);
#endif

#ifdef IPSCTP
IP_PUBLIC Ip_err ipsctp_create(void);
IP_PUBLIC Ip_err ipsctp_configure(void);
IP_PUBLIC Ip_err ipsctp_start(void);
IP_PUBLIC const char *ipsctp_version(void);
#endif

#ifdef IPIFPROXY
IP_PUBLIC Ip_err ipifproxy_create(void);
IP_PUBLIC Ip_err ipifproxy_configure(void);
IP_PUBLIC Ip_err ipifproxy_start(void);
IP_PUBLIC const char *ipifproxy_version(void);
#endif

#ifdef IPWRNAD
IP_PUBLIC Ip_err wrnad_create(void);
IP_PUBLIC Ip_err wrnad_configure(void);
IP_PUBLIC Ip_err wrnad_start(void);
IP_PUBLIC const char *wrnad_version(void);
#endif
/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_GLOBAL const Ipcom_ipd_product ipcom_ipd_products[] =
{
#ifdef IPLITE
    { "iplite", iplite_create, iplite_configure, iplite_start, iplite_version, IPCOM_IPD_ID_IPLITE, IPCOM_LOG_IPLITE, 0},
#elif defined(IP_PORT_OSE5)
    { "iplite", 0, 0, 0, 0, IPCOM_IPD_ID_IPLITE, IPCOM_LOG_IPLITE, 0},
#endif

#ifdef IPNET
    { "ipnet", ipnet_create, ipnet_configure, ipnet_start, ipnet_version, IPCOM_IPD_ID_IPNET, IPCOM_LOG_IPNET, 0},
#if defined(IPNET_USE_VRRP)
    { "ipnet_vrrpd", ipnet_vrrpd_create, ipnet_vrrpd_configure, ipnet_vrrpd_start, ipnet_vrrpd_version, IPCOM_IPD_ID_IPNET_VRRPD, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START | IPCOM_IPD_FLAG_HALTED},
#endif
#ifdef IPCOM_USE_INET6
    { "ipnet_radvd", ipnet_radvd_create, ipnet_radvd_configure, ipnet_radvd_start, ipnet_radvd_version, IPCOM_IPD_ID_IPNET_RADVD, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START | IPCOM_IPD_FLAG_HALTED},
#endif
#ifdef IPNET_USE_RFC3971
    { "ipnet_sendd", ipnet_send_create, ipnet_send_configure, ipnet_send_start, ipnet_send_version, IPCOM_IPD_ID_IPNET_SEND, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START | IPCOM_IPD_FLAG_HALTED },
#endif /* IPNET_USE_RFC3971 */
#elif defined(IP_PORT_OSE5)  /* IPNET */
    { "ipnet", 0, 0, 0, 0, IPCOM_IPD_ID_IPNET, IPCOM_LOG_IPNET, 0},
#if defined(IPNET_USE_VRRP)
    { "ipnet_vrrpd", 0, 0, 0, 0, IPCOM_IPD_ID_IPNET_VRRPD, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START | IPCOM_IPD_FLAG_HALTED},
#endif
#ifdef IPCOM_USE_INET6
    { "ipnet_radvd", IP_NULL, IP_NULL, IP_NULL, IP_NULL, IPCOM_IPD_ID_IPNET_RADVD, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_HALTED},
#endif
#endif /* IPNET */

#ifdef IPTCP
    { "iptcp", iptcp_create, iptcp_configure, iptcp_start, iptcp_version, IPCOM_IPD_ID_IPTCP, IPCOM_LOG_IPTCP, 0},
#elif defined(IP_PORT_OSE5)
    { "iptcp", 0, 0, 0, 0, IPCOM_IPD_ID_IPTCP, IPCOM_LOG_IPTCP, 0},
#endif

#ifdef IPSCTP
    { "ipsctp", ipsctp_create, ipsctp_configure, ipsctp_start, ipsctp_version, IPCOM_IPD_ID_IPSCTP, IPCOM_LOG_IPSCTP, 0},
#endif

#ifdef IPTELNETS
    { "iptelnets", IP_NULL, IP_NULL, iptelnets_start, iptelnets_version, IPCOM_IPD_ID_IPTELNETS, IPCOM_LOG_IPCOM_TELNET, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)
    { "iptelnets", IP_NULL, IP_NULL, IP_NULL, IP_NULL, IPCOM_IPD_ID_IPTELNETS, IPCOM_LOG_IPCOM_TELNET, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPCRYPTO
    { "ipcrypto", ipcrypto_create, ipcrypto_configure, ipcrypto_start, ipcrypto_version, IPCOM_IPD_ID_IPCRYPTO, IPCOM_LOG_IPCRYPTO, 0},
#elif defined(IP_PORT_OSE5)
    { "ipcrypto", 0, 0, 0, 0, IPCOM_IPD_ID_IPCRYPTO, IPCOM_LOG_IPCRYPTO, 0},
#endif

#ifdef IPRADIUS
    { "ipradius", ipradius_create, ipradius_configure, ipradius_start, ipradius_version, IPCOM_IPD_ID_IPRADIUS, IPCOM_LOG_IPRADIUS, 0},
#elif defined(IP_PORT_OSE5)
    { "ipradius", 0, 0, 0, 0, IPCOM_IPD_ID_IPRADIUS, IPCOM_LOG_IPRADIUS, 0},
#endif

#ifdef IPPPP
    { "ipppp", ipppp_create, ipppp_configure, ipppp_start, ipppp_version, IPCOM_IPD_ID_IPPPP, IPCOM_LOG_IPPPP, 0},
#endif

#ifdef IPPPPOE
    { "ippppoe", ippppoe_create, ippppoe_configure, ippppoe_start, ippppoe_version, IPCOM_IPD_ID_IPPPPOE, IPCOM_LOG_IPPPPOE, 0},
#endif

#ifdef IPMIP6MN
    { "ipmip6mn", IP_NULL, IP_NULL, ipmip6mn_start, ipmip6mn_version, IPCOM_IPD_ID_IPMIP6MN, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPMIP6HA
    { "ipmip6ha", IP_NULL, IP_NULL, ipmip6ha_start, ipmip6ha_version, IPCOM_IPD_ID_IPMIP6HA, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPMIP6CN
    { "ipmip6cn", IP_NULL, IP_NULL, ipmip6cn_start, ipmip6cn_version, IPCOM_IPD_ID_IPMIP6CN, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPMIP6PM
    { "ipmip6pm", IP_NULL, IP_NULL, ipmip6pm_start, ipmip6pm_version, IPCOM_IPD_ID_IPMIP6PM, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPMIPMN
    { "ipmipmn", ipmipmn_create, ipmipmn_configure, ipmipmn_start, ipmipmn_version, IPCOM_IPD_ID_IPMIPMN, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)
    { "ipmipmn", 0, 0, 0, 0, IPCOM_IPD_ID_IPMIPMN, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPMIPHA
    { "ipmipha", IP_NULL, IP_NULL, ipmipha_start, ipmipha_version, IPCOM_IPD_ID_IPMIPHA, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)
    { "ipmipha", 0, 0, 0, 0, IPCOM_IPD_ID_IPMIPHA, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPMIPFA
    { "ipmipfa", IP_NULL, IP_NULL, ipmipfa_start, ipmipfa_version, IPCOM_IPD_ID_IPMIPFA, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)
    { "ipmipfa", 0, 0, 0, 0, IPCOM_IPD_ID_IPMIPFA, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPMLDS
    { "ipmlds", ipmlds_create, ipmlds_configure, ipmlds_start, ipmlds_version, IPCOM_IPD_ID_IPMLDS, IPCOM_LOG_IPMLDS, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)
    { "ipmlds", 0, 0, 0, 0, IPCOM_IPD_ID_IPMLDS, IPCOM_LOG_IPMLDS, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPTFTPS
    { "iptftps", iptftps_create, iptftps_configure, iptftps_start, iptftps_version, IPCOM_IPD_ID_IPTFTPS, IPCOM_LOG_IPTFTP, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)
    { "iptftps", 0, 0, 0, 0, IPCOM_IPD_ID_IPTFTPS, IPCOM_LOG_IPTFTP, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPFTPS
    { "ipftps", ipftps_create, ipftps_configure, ipftps_start, ipftps_version, IPCOM_IPD_ID_IPFTPS, IPCOM_LOG_IPFTP, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)
    { "ipftps", 0, 0, 0, 0, IPCOM_IPD_ID_IPFTPS, IPCOM_LOG_IPFTP, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPL2TP
    { "ipl2tp", ipl2tp_create, ipl2tp_configure, ipl2tp_start, ipl2tp_version, IPCOM_IPD_ID_IPL2TP, IPCOM_LOG_IPL2TP, 0},
#elif defined(IP_PORT_OSE5)
    { "ipl2tp", 0, 0, 0, 0, IPCOM_IPD_ID_IPL2TP, IPCOM_LOG_IPL2TP, 0},
#endif

#ifdef IPNAT
    { "ipnat", ipnat_create, ipnat_configure, ipnat_start, ipnat_version, IPCOM_IPD_ID_IPNAT, IPCOM_LOG_IPNAT, 0},
#endif

#ifdef IPSSL
    { "ipssl", ipssl_create, ipssl_configure, ipssl_start, ipssl_version, IPCOM_IPD_ID_IPSSL, IPCOM_LOG_IPSSL, 0},
#elif defined(IP_PORT_OSE5)
    { "ipssl", 0, 0, 0, 0, IPCOM_IPD_ID_IPSSL, IPCOM_LOG_IPSSL, 0},
#endif

#ifdef IPLDAPC
    { "ipldapc", ipldapc_create, ipldapc_configure, ipldapc_start, ipldapc_version, IPCOM_IPD_ID_IPLDAPC, IPCOM_LOG_IPLDAPC, 0},
#elif defined(IP_PORT_OSE5)
    { "ipldapc", 0, 0, 0, 0, IPCOM_IPD_ID_IPLDAPC, IPCOM_LOG_IPLDAPC, 0},
#endif

#ifdef IPSSH
    { "ipssh", ipssh_create, ipssh_configure, ipssh_start, ipssh_version, IPCOM_IPD_ID_IPSSH, IPCOM_LOG_IPSSH, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)
    { "ipssh", 0, 0, 0, 0, IPCOM_IPD_ID_IPSSH, IPCOM_LOG_IPSSH, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPFIREWALL
    { "ipfirewall", ipfirewall_create, ipfirewall_configure, ipfirewall_start, ipfirewall_version, IPCOM_IPD_ID_IPFIREWALL, IPCOM_LOG_IPFIREWALL, 0},
#elif defined(IP_PORT_OSE5)
    { "ipfirewall", 0, 0, 0, 0, IPCOM_IPD_ID_IPFIREWALL, IPCOM_LOG_IPFIREWALL, 0},
#endif

#ifdef IPDHCPS
    { "ipdhcps", ipdhcps_create, ipdhcps_configure, ipdhcps_start, ipdhcps_version, IPCOM_IPD_ID_IPDHCPS, IPCOM_LOG_IPDHCPS, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)
    { "ipdhcps", 0, 0, 0, 0, IPCOM_IPD_ID_IPDHCPS, IPCOM_LOG_IPDHCPS, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPDHCPC6
    { "ipdhcpc6", IP_NULL, IP_NULL, ipdhcpc6_start, ipdhcpc6_version, IPCOM_IPD_ID_IPDHCPC6, IPCOM_LOG_IPDHCPC6, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)
    { "ipdhcpc6", 0, 0, 0, 0, IPCOM_IPD_ID_IPDHCPC6, IPCOM_LOG_IPDHCPC6, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPDHCPS6
    { "ipdhcps6", ipdhcps6_create, ipdhcps6_configure, ipdhcps6_start, ipdhcps6_version, IPCOM_IPD_ID_IPDHCPS6, IPCOM_LOG_IPDHCPS6, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)
    { "ipdhcps6", 0, 0, 0, 0, IPCOM_IPD_ID_IPDHCPS6, IPCOM_LOG_IPDHCPS6, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPDHCPR
    { "ipdhcpr", ipdhcpr_create, ipdhcpr_configure, ipdhcpr_start, ipdhcpr_version, IPCOM_IPD_ID_IPDHCPR, IPCOM_LOG_IPDHCPR, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)
    { "ipdhcpr", 0, 0, 0, 0, IPCOM_IPD_ID_IPDHCPR, IPCOM_LOG_IPDHCPR, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPWEBS
    { "ipwebs", ipwebs_create, ipwebs_configure, ipwebs_start, ipwebs_version, IPCOM_IPD_ID_IPWEBS, IPCOM_LOG_IPWEBS, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)

    { "ipwebs", 0, 0, 0, 0, IPCOM_IPD_ID_IPWEBS, IPCOM_LOG_IPWEBS, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPRIP
    { "iprip", iprip_create, iprip_configure, iprip_start, iprip_version, IPCOM_IPD_ID_IPRIP, IPCOM_LOG_IPRIP, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)

    { "iprip", 0, 0, 0, 0, IPCOM_IPD_ID_IPRIP, IPCOM_LOG_IPRIP, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPSNTP
    { "ipsntp", ipsntp_create, ipsntp_configure, ipsntp_start, ipsntp_version, IPCOM_IPD_ID_IPSNTP, IPCOM_LOG_IPSNTP, IPSNTP_IPD_FLAG},
#elif defined(IP_PORT_OSE5)
    { "ipsntp", 0, 0, 0, 0, IPCOM_IPD_ID_IPSNTP, IPCOM_LOG_IPSNTP, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPSNMP
    { "ipsnmp", ipsnmp_create, ipsnmp_configure, ipsnmp_start, ipsnmp_version, IPCOM_IPD_ID_IPSNMP, IPCOM_LOG_IPSNMP, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)

    { "ipsnmp", 0, 0, 0, 0, IPCOM_IPD_ID_IPSNMP, IPCOM_LOG_IPSNMP, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPEMANATE
    { "ipemanate", ipemanate_create, ipemanate_configure, ipemanate_start, ipemanate_version, IPCOM_IPD_ID_IPEMANATE, IPCOM_LOG_IPEMANATE, 0},
#elif defined(IP_PORT_OSE5)
    { "ipemanate", 0, 0, 0, 0, IPCOM_IPD_ID_IPEMANATE, IPCOM_LOG_IPEMANATE, 0},
#endif

#ifdef ZEBOS
    { "ipzebos", ipzebos_create, ipzebos_configure, ipzebos_start, ipzebos_version, IPCOM_IPD_ID_ZEBOS, IPCOM_LOG_NONE, 0},
#elif defined(IP_PORT_OSE5)

    { "ipzebos", 0, 0, 0, 0, IPCOM_IPD_ID_ZEBOS, IPCOM_LOG_NONE, 0},
#endif

#ifdef IPDC
    { "ipdc", ipdc_create, ipdc_configure, ipdc_start, ipdc_version, IPCOM_IPD_ID_IPDC, IPCOM_LOG_NONE, 0},
#elif defined(IP_PORT_OSE5)

    { "ipdc", 0, 0, 0, 0, IPCOM_IPD_ID_IPDC, IPCOM_LOG_NONE, 0},
#endif

#ifdef IPNGC
    { "ipngc", ipngc_create, ipngc_configure, ipngc_start, ipngc_version, IPCOM_IPD_ID_IPNGC, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START /* | IPCOM_IPD_FLAG_HALTED*/},
#elif defined(IP_PORT_OSE5)

    { "ipngc", 0, 0, 0, 0, IPCOM_IPD_ID_IPNGC, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPQUAGGA
    { "ipquagga", ipquagga_create, ipquagga_configure, ipquagga_start, ipquagga_version, IPCOM_IPD_ID_IPQUAGGA, IPCOM_LOG_IPQUAGGA, IPCOM_IPD_FLAG_IPD_START /* | IPCOM_IPD_FLAG_HALTED*/},
#elif defined(IP_PORT_OSE5)

    { "ipquagga", 0, 0, 0, 0, IPCOM_IPD_ID_IPQUAGGA, IPCOM_LOG_IPQUAGGA, IPCOM_IPD_FLAG_IPD_START},
#endif

#if defined(IPDHCPC) && defined(IPCOM_SOCK)
    { "ipdhcpc", ipdhcpc_create, ipdhcpc_configure, ipdhcpc_start, ipdhcpc_version, IPCOM_IPD_ID_IPDHCPC, IPCOM_LOG_IPDHCPC, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)

    { "ipdhcpc", 0, 0, 0, 0, IPCOM_IPD_ID_IPDHCPC, IPCOM_LOG_IPDHCPC, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPDNSC
    { "ipdnsc", ipdnsc_create, ipdnsc_configure, ipdnsc_start, ipdnsc_version, IPCOM_IPD_ID_IPDNSC, IPCOM_LOG_NONE, 0},
#elif defined(IP_PORT_OSE5)

    { "ipdnsc", 0, 0, 0, 0, IPCOM_IPD_ID_IPDNSC, IPCOM_LOG_NONE, 0},
#endif

#ifdef IPTFTPC
    { "iptftpc", iptftpc_create, iptftpc_configure, iptftpc_start, iptftpc_version, IPCOM_IPD_ID_IPTFTPC, IPCOM_LOG_IPTFTP, 0},
#elif defined(IP_PORT_OSE5)

    { "iptftpc", 0, 0, 0, 0, IPCOM_IPD_ID_IPTFTPC, IPCOM_LOG_IPTFTP, 0},
#endif

#ifdef IPFTPC
    { "ipftpc", ipftpc_create, ipftpc_configure, ipftpc_start, ipftpc_version, IPCOM_IPD_ID_IPFTPC, IPCOM_LOG_IPFTP, 0},
#elif defined(IP_PORT_OSE5)

    { "ipftpc", 0, 0, 0, 0, IPCOM_IPD_ID_IPFTPC, IPCOM_LOG_IPFTP, 0},
#endif

#ifdef IPMPLS
    { "ipmpls", ipmpls_create, ipmpls_configure, ipmpls_start, ipmpls_version, IPCOM_IPD_ID_IPMPLS, IPCOM_LOG_IPMPLS, 0},
#elif defined(IP_PORT_OSE5)

    { "ipmpls", 0, 0, 0, 0, IPCOM_IPD_ID_IPMPLS, IPCOM_LOG_IPMPLS, 0},
#endif

#ifdef IPMCP
    { "ipmcp", ipmcp_create, ipmcp_configure, ipmcp_start, ipmcp_version, IPCOM_IPD_ID_IPMCP, IPCOM_LOG_IPMCP, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)

    { "ipmcp", 0, 0, 0, 0, IPCOM_IPD_ID_IPMCP, IPCOM_LOG_IPMCP, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPPRISM
    { "ipprism", ipprism_create, ipprism_configure, ipprism_start, ipprism_version, IPCOM_IPD_ID_IPPRISM, IPCOM_LOG_IPPRISM, 0},
#elif defined(IP_PORT_OSE5)

    { "ipprism", 0, 0, 0, 0, IPCOM_IPD_ID_IPPRISM, IPCOM_LOG_IPPRISM, 0},
#endif

#ifdef IP8021X
    { "ip8021x", ip8021x_create, ip8021x_configure, ip8021x_start, ip8021x_version, IPCOM_IPD_ID_IP8021X, IPCOM_LOG_IP8021X, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)

    { "ip8021x", 0, 0, 0, 0, IPCOM_IPD_ID_IP8021X, IPCOM_LOG_IP8021X, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPEAP
    { "ipeap", ipeap_create, ipeap_configure, ipeap_start, ipeap_version, IPCOM_IPD_ID_IPEAP, IPCOM_LOG_IPEAP, 0},
#elif defined(IP_PORT_OSE5)

    { "ipeap", 0, 0, 0, 0, IPCOM_IPD_ID_IPEAP, IPCOM_LOG_IPEAP, 0},
#endif

#ifdef IPWPS
    { "ipwps", ipwps_create, ipwps_configure, ipwps_start, ipwps_version, IPCOM_IPD_ID_IPWPS, IPCOM_LOG_IPWPS, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)

    { "ipwps", 0, 0, 0, 0, IPCOM_IPD_ID_IPWPS, IPCOM_LOG_IPWPS, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPHWCRYPTO
    { "iphwcrypto", iphwcrypto_create, iphwcrypto_configure, iphwcrypto_start, iphwcrypto_version, IPCOM_IPD_ID_IPHWCRYPTO, IPCOM_LOG_IPHWCRYPTO, 0},
#elif defined(IP_PORT_OSE5)

    { "iphwcrypto", 0, 0, 0, 0, IPCOM_IPD_ID_IPHWCRYPTO, IPCOM_LOG_IPHWCRYPTO, 0},
#endif

#ifdef IPSAFENET
    { "ipsafenet", ipsafenet_create, ipsafenet_configure, ipsafenet_start, ipsafenet_version, IPCOM_IPD_ID_IPSAFENET, IPCOM_LOG_IPSAFENET, 0},
#elif defined(IP_PORT_OSE5)

    { "ipsafenet", 0, 0, 0, 0, IPCOM_IPD_ID_IPSAFENET, IPCOM_LOG_IPSAFENET, 0},
#endif

#ifdef IPFREESCALE
    { "ipfreescale", ipfreescale_create, ipfreescale_configure, ipfreescale_start, ipfreescale_version, IPCOM_IPD_ID_IPFREESCALE, IPCOM_LOG_IPFREESCALE, 0},
#elif defined(IP_PORT_OSE5)

    { "ipfreescale", 0, 0, 0, 0, IPCOM_IPD_ID_IPFREESCALE, IPCOM_LOG_IPFREESCALE, 0},
#endif

#ifdef IPCAVIUM
    { "ipcavium", ipcavium_create, ipcavium_configure, ipcavium_start, ipcavium_version, IPCOM_IPD_ID_IPCAVIUM, IPCOM_LOG_IPCAVIUM, 0},
#elif defined(IP_PORT_OSE5)

    { "ipcavium", 0, 0, 0, 0, IPCOM_IPD_ID_IPCAVIUM, IPCOM_LOG_IPCAVIUM, 0},
#endif

#ifdef IPCCI
    { "ipcci", ipcci_create, ipcci_configure, ipcci_start, ipcci_version, IPCOM_IPD_ID_IPCCI, IPCOM_LOG_IPCCI, 0},
#elif defined(IP_PORT_OSE5)

    { "ipcci", 0, 0, 0, 0, IPCOM_IPD_ID_IPCCI, IPCOM_LOG_IPCCI, 0},
#endif

#ifdef IPNETPERF
    { "ipnetperf", ipnetperf_create, ipnetperf_configure, ipnetperf_start, ipnetperf_version, IPCOM_IPD_ID_IPNETPERF, IPCOM_LOG_NONE, 0},
#elif defined(IP_PORT_OSE5)

    { "ipnetperf", 0, 0, 0, 0, IPCOM_IPD_ID_IPNETPERF, IPCOM_LOG_NONE, 0},
#endif

#ifdef IPNETSNMP
    { "ipnetsnmp", ipnetsnmp_create, ipnetsnmp_configure, ipnetsnmp_start, ipnetsnmp_version, IPCOM_IPD_ID_IPNETSNMP, IPCOM_LOG_IPNETSNMP, 0},
#elif defined(IP_PORT_OSE5)

    { "ipnetsnmp", 0, 0, 0, 0, IPCOM_IPD_ID_IPNETSNMP, IPCOM_LOG_IPNETSNMP, 0},
#endif

#if defined(IPIPSEC) || defined(IPIPSEC2)
    { "ipipsec", ipipsec_create, ipipsec_configure, ipipsec_start, ipipsec_version, IPCOM_IPD_ID_IPIPSEC, IPCOM_LOG_IPIPSEC, 0},
#elif defined(IP_PORT_OSE5)

    { "ipipsec", 0, 0, 0, 0, IPCOM_IPD_ID_IPIPSEC, IPCOM_LOG_IPIPSEC, 0},
#endif

#ifdef IPIKE
    { "ipike", ipike_create, ipike_configure, ipike_start, ipike_version, IPCOM_IPD_ID_IPIKE, IPCOM_LOG_IPIKE, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)

    { "ipike", 0, 0, 0, 0, IPCOM_IPD_ID_IPIKE, IPCOM_LOG_IPIKE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPMARVELL
    { "ipmarvell", ipmarvell_create, ipmarvell_configure, ipmarvell_start, ipmarvell_version, IPCOM_IPD_ID_IPMARVELL, IPCOM_LOG_NONE, 0},
#elif defined(IP_PORT_OSE5)

    { "ipmarvell", 0, 0, 0, 0, IPCOM_IPD_ID_IPMARVELL, IPCOM_LOG_NONE, 0},
#endif

#ifdef IPANVL
    { "ipanvl", ipanvl_create, ipanvl_configure, ipanvl_start, ipanvl_version, IPCOM_IPD_ID_IPANVL, IPCOM_LOG_NONE, 0},
#elif defined(IP_PORT_OSE5)

    { "ipanvl", 0, 0, 0, 0, IPCOM_IPD_ID_IPANVL, IPCOM_LOG_NONE, 0},
#endif

#ifdef IPDIAMETER
    { "ipdiameterc", ipdiameterc_create, ipdiameterc_configure, ipdiameterc_start, ipdiameter_version, IPCOM_IPD_ID_IPDIAMETERC, IPCOM_LOG_IPDIAMETER, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)

    { "ipdiameterc", 0, 0, 0, 0, IPCOM_IPD_ID_IPDIAMETERC, IPCOM_LOG_IPDIAMETER, 0},
#endif

#ifdef IPMIPPM
    { "ipmippm", IP_NULL, IP_NULL, ipmippm_start, ipmippm_version, IPCOM_IPD_ID_IPMIPPM, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#elif defined(IP_PORT_OSE5)

    { "ipmippm", 0, 0, 0, 0, IPCOM_IPD_ID_IPMIPPM, IPCOM_LOG_NONE, IPCOM_IPD_FLAG_IPD_START},
#endif

#ifdef IPIFPROXY
    { "ipifproxy", ipifproxy_create, ipifproxy_configure, ipifproxy_start, ipifproxy_version, IPCOM_IPD_ID_IPIFPROXY, IPCOM_LOG_IPIFPROXY, 0},
#endif

#ifdef IPWRNAD
    { "wrnad", wrnad_create, wrnad_configure, wrnad_start, wrnad_version, IPCOM_IPD_ID_WRNAD, IPCOM_LOG_WRNAD, 0},
#endif

    { IP_NULL, IP_NULL, IP_NULL, IP_NULL, IP_NULL, -1, IPCOM_LOG_NONE, 0 }  /* end entry. */
};

#endif /* WRS_IPNET */


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

#if IPCOM_USE_SOCK != IPCOM_SOCK_NONE

/*
 *===========================================================================
 *                    ipcom_ipd_socket_clear
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_ipd_socket_clear(Ip_fd fd)
{
    Ip_socklen_t   errlen;
    int so_errno;

    errlen = sizeof(int);
    (void)ipcom_getsockopt(fd, IP_SOL_SOCKET, IP_SO_ERROR, &so_errno, &errlen);
}


/*
 *===========================================================================
 *                    ipcom_ipd_socket
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_fd
ipcom_ipd_socket(int indx, Ip_bool is_application)
{
    Ip_fd  fd;
    union Ip_sockaddr_union   addr;
    Ip_u16   port;
    Ip_socklen_t    salen;

    /* Get socket. */
    for (;;)
    {
#ifdef IPCOM_USE_INET
        fd = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, IP_IPPROTO_UDP);
#else
        fd = ipcom_socket(IP_AF_INET6, IP_SOCK_DGRAM, IP_IPPROTO_UDP);
#endif

#if defined(IP_PORT_OSE) && !defined(IPCOM_SOCK)
        /* Workaround due to problem with fd == 0 and OSE INET /lob 050822 */
        if (fd == 0)
        {
            Ip_fd fd2;
#ifdef IPCOM_USE_INET
            fd2 = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, IP_IPPROTO_UDP);
#else
            fd2 = ipcom_socket(IP_AF_INET6, IP_SOCK_DGRAM, IP_IPPROTO_UDP);
#endif
            (void) ipcom_socketclose(fd);
            fd = fd2;
        }
#endif /* defined(IP_PORT_OSE) && !defined(IPCOM_SOCK) */

        if (fd != IP_INVALID_SOCKET)
            break;

        (void)ipcom_sleep(1);
    }

    port = (Ip_u16)(IPCOM_IPD_PORT_BASE + (ipcom_ipd_products[indx].id << 1));
    if (is_application)
        port++;

    /* Bind socket. */
    ipcom_memset(&addr, 0, sizeof(union Ip_sockaddr_union));
#ifdef IPCOM_USE_INET
    salen = sizeof(struct Ip_sockaddr_in);
    addr.sin.sin_family = IP_AF_INET;
    addr.sin.sin_addr.s_addr = ip_htonl(IP_INADDR_LOOPBACK);
    addr.sin.sin_port = ip_htons(port);
#else
    salen = sizeof(struct Ip_sockaddr_in6);
    addr.sin6.sin6_family = IP_AF_INET6;
    addr.sin6.sin6_addr.in6.addr8[15] = 1;
    addr.sin6.sin6_port = ip_htons(port);
#endif
    IPCOM_SA_LEN_SET(&addr, salen);

    for (;;)
    {
        if (ipcom_bind(fd, (struct Ip_sockaddr *)&addr, salen) != IP_SOCKERR)
            break;

        if (ipcom_errno == IP_ERRNO_EADDRNOTAVAIL)
            return IP_INVALID_SOCKET;

        /* Clear socket error and retry in one second. */
        IPCOM_LOG1(WARNING, "Failed to bind the IPD socket, will retry in one second. Error code: %s",
                   ipcom_strerror(ipcom_errno));
        ipcom_ipd_socket_clear(fd);
        (void)ipcom_sleep(1);
    }

    /* Connect socket. */
    if (is_application)
        port--;
    else
        port++;
#ifdef IPCOM_USE_INET
    addr.sin.sin_port = ip_htons(port);
#else
    addr.sin6.sin6_port = ip_htons(port);
#endif
    for (;;)
    {
        if (ipcom_connect(fd, (struct Ip_sockaddr *)&addr, salen) != IP_SOCKERR)
            break;
        /* Clear socket error and retry in one second. */
        ipcom_ipd_socket_clear(fd);
        (void)ipcom_sleep(1);
    }

    return fd;
}


/*
 *===========================================================================
 *                    ipcom_ipd_isstarted
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipcom_ipd_isstarted(const char *name, Ip_fd fd)
{
    struct Ip_timeval tv;
    Ip_fd_set  read_set;
    int num;
    Ipcom_ipd_msg  msg;
    Ip_ssize_t   len;

    (void)name;

    tv.tv_usec = 0;
    tv.tv_sec  = 1;

    for (;;)
    {
        /* Send ping. */
        msg.msgtype = IPCOM_IPD_MSGTYPE_PING;
        msg.msgerr  = 0;
        len = ipcom_socketwrite(fd, &msg, sizeof(msg));
        if (len == -1)
        {
            IPCOM_LOG4(DEBUG2, "ipcom_ipd_isstarted() :: %s - write(len=%d) failed : %s[%d]",
                       name, (int)len, ipcom_strerror(ipcom_errno), ipcom_errno);
            goto false;
        }
        ip_assert(len == sizeof(msg));

        /* Wait for ping. */
        IP_FD_ZERO(&read_set);
        IP_FD_SET(fd, &read_set);
        num = ipcom_socketselect(fd + 1, &read_set, IP_NULL, IP_NULL, &tv);
        if (num >= 1 && IP_FD_ISSET(fd, &read_set))
        {
            len = ipcom_socketread(fd, &msg, sizeof(msg));
            if (len == sizeof(msg) && msg.msgtype == IPCOM_IPD_MSGTYPE_PING)
                return IP_TRUE;
            IPCOM_LOG4(DEBUG2, "ipcom_ipd_isstarted() :: %s - read(len=%d) failed : %s[%d]",
                       name, (int)len, ipcom_strerror(ipcom_errno), ipcom_errno);
            goto false;
        }
        else
        {
            ip_assert(num != 1);
        }

#ifdef IP_PORT_ATI
        /* Nucleus native stack seems not to trigger select() for incoming ICMP port unreachables */
        if (num == 0 && tv.tv_sec > 1)
            goto false;
#endif

        /* Double timeout, wait max 4 seconds. */
        ip_timeval_double(&tv);
        tv.tv_sec = IP_MIN(tv.tv_sec, 4);

        IPCOM_LOG1(DEBUG2, "ipcom_ipd_isstarted() :: %s - timeout", name);
    }

 false:
    /* Clear socket error. */
    ipcom_ipd_socket_clear(fd);
    return IP_FALSE;
}


/*
 ****************************************************************************
 * 10                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_ipd_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_ipd_init(const char *name, Ip_err err, Ip_fd *fdp)
{
    Ip_fd   fd;
    int     i, indx, num;
    Ipcom_ipd_msg  msg;
    Ip_ssize_t   len;
    struct Ip_timeval tv;
    Ip_fd_set  read_set;

    IPCOM_LOG2(DEBUG2, "ipcom_ipd_init() :: %s, err = %d", name, (int)err);

    for (indx = 0; ipcom_ipd_products[indx].name != IP_NULL; indx++)
        if (ipcom_strcmp(ipcom_ipd_products[indx].name, name) == 0)
            break;

    if (ipcom_ipd_products[indx].name == IP_NULL)
        return IPCOM_ERR_NOT_FOUND;

    fd = ipcom_ipd_socket(indx, IP_TRUE);
    if (fd == IP_INVALID_SOCKET)
    {
        IPCOM_LOG0(DEBUG2, "ipcom_ipd_init() :: attempting to open socket in invalid VR");
        return IPCOM_ERR_FAILED;
    }

    tv.tv_usec = 0;
    tv.tv_sec  = 1;

    for (i = 0; i < IPCOM_IPD_TIMEOUT; i++)
    {
        /* Send start ack. */
        IPCOM_LOG2(DEBUG2, "ipcom_ipd_init() :: %s - sending ACK (num = %d) ", name, i);
        msg.msgtype = IPCOM_IPD_MSGTYPE_START;
        msg.msgerr  = err;
        len = ipcom_socketwrite(fd, &msg, sizeof(msg));
        if (len == -1)
        {
            /* Remote side closed socket which means that they have seen our message. */
            IPCOM_LOG4(DEBUG2, "ipcom_ipd_init() :: %s - remote side closed socket (num = %d) : %s[%d]",
                       name, i, ipcom_strerror(ipcom_errno), ipcom_errno);
            goto leave;
        }

        /* Wait for ack. */
        IP_FD_ZERO(&read_set);
        IP_FD_SET(fd, &read_set);
        num = ipcom_socketselect(fd + 1, &read_set, IP_NULL, IP_NULL, &tv);
        if (num >= 1 && IP_FD_ISSET(fd, &read_set))
        {
            len = ipcom_socketread(fd, &msg, sizeof(msg));
            if (len == sizeof(msg) && msg.msgtype == IPCOM_IPD_MSGTYPE_START)
            {
                /* Remote side succesfully ACK'd our message. */
                IPCOM_LOG2(DEBUG2, "ipcom_ipd_init() :: %s - remote side ACK (num = %d) ", name, i);
                goto leave;
            }
            if (len == -1)
            {
                /* Remote side closed socket which means that they have seen our message. */
                IPCOM_LOG4(DEBUG2, "ipcom_ipd_init() :: %s - remote side closed socket#2 (num = %d) : %s[%d]",
                       name, i, ipcom_strerror(ipcom_errno), ipcom_errno);
                goto leave;
            }
        }

        /* Double timeout. */
        ip_timeval_double(&tv);
        tv.tv_sec = IP_MIN(tv.tv_sec, 4);
    }

 leave:
    ipcom_ipd_socket_clear(fd);
    *fdp = fd;
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_ipd_exit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_ipd_exit(const char *name, Ip_fd fd)
{
    int    indx;

    if (fd != IP_INVALID_SOCKET)
        (void)ipcom_socketclose(fd);

    for (indx = 0; ipcom_ipd_products[indx].name != IP_NULL; indx++)
        if (ipcom_strcmp(ipcom_ipd_products[indx].name, name) == 0)
            break;

    if (ipcom_ipd_products[indx].name == IP_NULL)
        return IPCOM_ERR_NOT_FOUND;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_ipd_input_process
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:   Event
 *
 */
IP_PUBLIC int
ipcom_ipd_input_process(Ip_fd fd, Ipcom_ipd_msg *msg)
{
    /* Send ACK. */
    (void)ipcom_socketwrite(fd, msg, sizeof(*msg));

    /* Clear error if remote side was closed and we got dest-unreach. */
    ipcom_ipd_socket_clear(fd);

    switch(msg->msgtype)
    {
    case IPCOM_IPD_MSGTYPE_START:
        /* A trailing START ACK can be received if the ipcom_ipd_init() function had to retransmit. */
        break;

    case IPCOM_IPD_MSGTYPE_PING:
        break;

    case IPCOM_IPD_MSGTYPE_KILL:
        return IPCOM_IPD_EVENT_KILL;

    case IPCOM_IPD_MSGTYPE_RECONFIGURE:
        return IPCOM_IPD_EVENT_RECONFIGURE;

    default:
        /* return unknown messages. */
        return msg->msgtype;
    }

    return IPCOM_IPD_EVENT_NONE;
}


/*
 *===========================================================================
 *                    ipcom_ipd_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:   Event
 *
 */
IP_PUBLIC int
ipcom_ipd_input(Ip_fd fd)
{
    Ip_ssize_t   len;
    Ipcom_ipd_msg  msg;

    len = ipcom_socketread(fd, &msg, sizeof(msg));
    if (len != sizeof(msg))
    {
        /* Clear socket error. */
        ipcom_ipd_socket_clear(fd);
        return IPCOM_IPD_EVENT_NONE;
    }

    return ipcom_ipd_input_process(fd, &msg);
}


/*
 *===========================================================================
 *                    ipcom_ipd_start
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_ipd_start(const char *name)
{
    int     indx, num;
    Ip_fd   fd;
    Ip_err  err;
    Ipcom_ipd_msg  msg;
    Ip_ssize_t   len;
    Ip_fd_set  read_set;
    struct Ip_timeval tv;

    for (indx = 0; ipcom_ipd_products[indx].name != IP_NULL; indx++)
        if (ipcom_strcmp(ipcom_ipd_products[indx].name, name) == 0)
            break;

    if (ipcom_ipd_products[indx].name == IP_NULL)
        return IPCOM_ERR_NOT_FOUND;

#ifdef IP_PORT_OSE5
    if (ipcom_ipd_products[indx].start == IP_NULL
        && ipcom_ipd_isinstalled_ose5(name) != IPCOM_SUCCESS)
        return IPCOM_ERR_NOT_FOUND;
#else
    if (ipcom_ipd_products[indx].start == IP_NULL)
        return IPCOM_ERR_NOT_SUPPORTED;
#endif /* IP_PORT_OSE5 */

   /* Get IPD socket. */
    fd = ipcom_ipd_socket(indx, IP_FALSE);
    if (fd == IP_INVALID_SOCKET)
    {
        IPCOM_LOG0(DEBUG2, "ipcom_ipd_init() :: attempting to open socket in invalid VR");
        return IPCOM_ERR_FAILED;
    }

    /* Test if started. */
    if (ipcom_ipd_isstarted(name, fd))
    {
        err = IPCOM_ERR_ALREADY_STARTED;
        goto leave;
    }

    /* Start product. */
#ifdef IP_PORT_OSE5
    if (ipcom_ipd_products[indx].start != IP_NULL)
        err = ipcom_ipd_products[indx].start();
    else
        err = ipcom_ipd_start_ose5(name);
#else
    err = ipcom_ipd_products[indx].start();
#endif /* IP_PORT_OSE5 */
    if (err != IPCOM_SUCCESS)
    {
        IP_PANIC2();
        goto leave;
    }

    /* Wait for start ack. */
    tv.tv_usec = 0;
    tv.tv_sec  = IPCOM_IPD_TIMEOUT;

    IP_FD_ZERO(&read_set);
    IP_FD_SET(fd, &read_set);

    num = ipcom_socketselect(fd + 1, &read_set, IP_NULL, IP_NULL, &tv);
    if (num >= 1 && IP_FD_ISSET(fd, &read_set))
    {
        len = ipcom_socketread(fd, &msg, sizeof(msg));
        if (len != sizeof(msg) || msg.msgtype != IPCOM_IPD_MSGTYPE_START)
        {
            IP_PANIC2();
            err = IPCOM_ERR_FAILED;
            goto leave;
        }
    }
    else
    {
        /* Timeout */
        err = IPCOM_ERR_FAILED;
        goto leave;
    }

    /* Send ACK. */
    err = msg.msgerr;
    len = ipcom_socketwrite(fd, &msg, sizeof(msg));
    if (len != sizeof(msg))
    {
        IP_PANIC();
    }

 leave:
    (void)ipcom_socketclose(fd);
    return err;
}


/*
 *===========================================================================
 *                    ipcom_ipd_read
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:   Event
 *
 */
IP_PUBLIC int
ipcom_ipd_read(Ip_fd fd, char *buf, Ip_size_t buflen, int *event)
{
    int len;

    len = ipcom_socketread(fd, buf, buflen);

    if (len < (int) sizeof(Ipcom_ipd_msg))
        return -1;

    *event = ipcom_ipd_input_process(fd, (Ipcom_ipd_msg *)buf);

    /* Return the length of the read message */
    return len;
}


/*
 *===========================================================================
 *                    ipcom_ipd_sendmsg
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_ipd_sendmsg(const char *name, void *msgbuf, Ip_size_t msglen)
{
    int     i, indx, num;
    Ip_fd   fd;
    Ip_ssize_t   len;
    Ip_fd_set  read_set;
    struct Ip_timeval tv;
    Ip_err  err = 0;
    Ipcom_ipd_msg *msg = msgbuf;

    if (msg->msgtype == IPCOM_IPD_MSGTYPE_START)
        return ipcom_ipd_start(name);

    for (indx = 0; ipcom_ipd_products[indx].name != IP_NULL; indx++)
        if (ipcom_strcmp(ipcom_ipd_products[indx].name, name) == 0)
            break;

    if (ipcom_ipd_products[indx].name == IP_NULL)
        return IPCOM_ERR_NOT_FOUND;

    if (IP_BIT_ISFALSE(ipcom_ipd_products[indx].flags, IPCOM_IPD_FLAG_IPD_START))
        return IPCOM_ERR_NOT_SUPPORTED;

    /* Get IPD socket. */
    fd = ipcom_ipd_socket(indx, IP_FALSE);
    if (fd == IP_INVALID_SOCKET)
    {
        IPCOM_LOG0(DEBUG2, "ipcom_ipd_init() :: attempting to open socket in invalid VR");
        return IPCOM_ERR_FAILED;
    }

    /* Test if started. */
    if (ipcom_ipd_isstarted(name, fd) == IP_FALSE)
    {
        (void)ipcom_socketclose(fd);
        return IPCOM_ERR_NOT_STARTED;
    }

    tv.tv_usec = 0;
    tv.tv_sec  = 1;

    for (i = 0; i < IPCOM_IPD_TIMEOUT; i++)
    {
        /* Send message. */
        len = ipcom_socketwrite(fd, msg, msglen);
        if (len == -1)
            break;

        /* Wait for message ack (message header size with same msgtype) */
    again:
        IP_FD_ZERO(&read_set);
        IP_FD_SET(fd, &read_set);
        num = ipcom_socketselect(fd + 1, &read_set, IP_NULL, IP_NULL, &tv);
        if (num >= 1 && IP_FD_ISSET(fd, &read_set))
        {
            Ipcom_ipd_msg msgack;

            len = ipcom_socketread(fd, &msgack, sizeof(msgack));
            if (len >= (Ip_ssize_t) sizeof(Ipcom_ipd_msg) && msg->msgtype != msgack.msgtype)
                goto again;
            if (len == sizeof(msgack))
                err = IPCOM_SUCCESS;
            else
                err = IPCOM_ERR_FAILED;
            break;
        }

        /* Double timeout. */
        ip_timeval_double(&tv);
        tv.tv_sec = IP_MIN(tv.tv_sec, 4);
    }

    (void)ipcom_socketclose(fd);
    return err;
}

/*
 *===========================================================================
 *                    ipcom_ipd_datamsg
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_ipd_datamsg(const char *name, void *outbuf, Ip_size_t outlen,
                  void *inbuf, Ip_size_t inlen)
{
    int               i, indx, num;
    Ip_fd             fd;
    Ip_ssize_t        len;
    Ip_fd_set         read_set;
    struct Ip_timeval tv;
    Ip_err            err = 0;

    for (indx = 0; ipcom_ipd_products[indx].name != IP_NULL; indx++)
        if (ipcom_strcmp(ipcom_ipd_products[indx].name, name) == 0)
            break;

    if (ipcom_ipd_products[indx].name == IP_NULL)
        return IPCOM_ERR_NOT_FOUND;

    if (IP_BIT_ISFALSE(ipcom_ipd_products[indx].flags, IPCOM_IPD_FLAG_IPD_START))
        return IPCOM_ERR_NOT_SUPPORTED;

    /* Get IPD socket. */
    fd = ipcom_ipd_socket(indx, IP_FALSE);
    if (fd == IP_INVALID_SOCKET)
    {
        IPCOM_LOG0(DEBUG2, "ipcom_ipd_init() :: attempting to open socket in invalid VR");
        return IPCOM_ERR_FAILED;
    }

    /* Test if started. */
    if (ipcom_ipd_isstarted(name, fd) == IP_FALSE)
    {
        (void)ipcom_socketclose(fd);
        return IPCOM_ERR_NOT_STARTED;
    }

    tv.tv_usec = 0;
    tv.tv_sec  = 2;

    IPCOM_LOG0(DEBUG2, "ipcom_ipd_datamsg() :: Before read loop!");
    for (i = 0; i < IPCOM_IPD_TIMEOUT; i++)
    {
        /* Send message. */
        len = ipcom_socketwrite(fd, outbuf, outlen);
        if (len == -1)
            break;

        /* Wait for message ack (message header size with same msgtype) */
    again:
        IP_FD_ZERO(&read_set);
        IP_FD_SET(fd, &read_set);
        IPCOM_LOG0(DEBUG2, "ipcom_ipd_datamsg() :: Before read");
        num = ipcom_socketselect(fd + 1, &read_set, IP_NULL, IP_NULL, &tv);
        IPCOM_LOG1(DEBUG2, "ipcom_ipd_datamsg() :: After read: %d",num);
        if (num >= 1 && IP_FD_ISSET(fd, &read_set))
        {
            Ipcom_ipd_msg *outmsg;
            Ipcom_ipd_msg *inmsg;

            len = ipcom_socketread(fd, inbuf, inlen);
            outmsg = (Ipcom_ipd_msg*) outbuf;
            inmsg = (Ipcom_ipd_msg*) inbuf;
            IPCOM_LOG2(DEBUG2, "ipcom_ipd_datamsg() :: len: %d, inlen: %d",len,inlen);
            if (len >= (Ip_ssize_t) sizeof(Ipcom_ipd_msg) &&
                    outlen >= sizeof(Ipcom_ipd_msg) &&
                    inmsg->msgtype != outmsg->msgtype)
                goto again;
            if ((Ip_size_t) len == inlen)
                err = IPCOM_SUCCESS;
            else
                err = IPCOM_ERR_FAILED;
            break;
        }

        /* Double timeout. */
        ip_timeval_double(&tv);
        tv.tv_sec = IP_MIN(tv.tv_sec, 6);
    }

    (void)ipcom_socketclose(fd);
    return err;
}

/*
 *===========================================================================
 *                    ipcom_ipd_send
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_ipd_send(const char *name, int msgtype)
{
    Ipcom_ipd_msg   msg;

    msg.msgtype = msgtype;
    msg.msgerr  = 0;

    return ipcom_ipd_sendmsg(name, &msg, sizeof(msg));
}


/*
 *===========================================================================
 *                    ipcom_ipd_kill
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_ipd_kill(const char *name)
{
    return ipcom_ipd_send(name, IPCOM_IPD_MSGTYPE_KILL);
}


/*
 *===========================================================================
 *                    ipcom_ipd_reconfigure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_ipd_reconfigure(const char *name)
{
    return ipcom_ipd_send(name, IPCOM_IPD_MSGTYPE_RECONFIGURE);
}

#endif /* IPCOM_USE_SOCK != IPCOM_SOCK_NONE */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
