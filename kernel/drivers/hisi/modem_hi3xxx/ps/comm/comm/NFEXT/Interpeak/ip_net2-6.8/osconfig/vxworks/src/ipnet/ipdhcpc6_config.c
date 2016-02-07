/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   INTERPEAK_VERSION_STRING
 *   Copyright 2000-2006 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01d,28aug07,ps  add support for dhcpv6 authentication
01c,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01b,01dec06,kch  Added support for additional configuration parameters.
01a,28aug06,tkf  Adopted from ipdhcpc6/config/ipdhcpc6_config.c.
*/

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    INCLUDE FILES
 ****************************************************************************
 */


#include <ipcom_type.h>
#include <ipcom_err.h>
#include <ipcom_syslog.h>
#include <ipcom_sysvar.h>
#include <ipcom_shell.h>

/*
 ****************************************************************************
 * 3                    DEFINES
 ****************************************************************************
 */

#ifndef IPCOM_USE_INET6
#error "Support for IPv6 is required by IPDHCPC6"
#endif

/*
 ****************************************************************************
 * 4                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

/* Sysvar split into conf_sysvar and external for Workbench Configurator  */

#if defined(WRS_IPNET) || defined(PRJ_BUILD)

/*
 *===========================================================================
 *                         ipdhcpc6_conf_sysvar
 *===========================================================================
 * System wide DHCPC6 Client Configuration Variables
 */
IP_CONST Ipcom_sysvar ipdhcpc6_conf_sysvar[] =
{
    /* LL, LLT or EN. Default LL */
    { "ipdhcpc6.duid.type", DHCPC6_DUID_TYPE        },

    /* For LL/LLT; if not specified first usable interface will be used */
    { "ipdhcpc6.duid.if", DHCPC6_DUID_IF      },
    /* For EN; must be specified */
    { "ipdhcpc6.duid.en.number", DHCPC6_DUID_EN_NUM      },
    { "ipdhcpc6.duid.en.value", DHCPC6_DUID_EN_VAL  },
    { "ipdhcpc6.authkey", DHCPC6_HMAC_MD5_SECRET },
    { "ipdhcpc6.authrealm", DHCPC6_AUTHENTICATION_REALM },

    { IP_NULL, IP_NULL }
};

/* Interface specific list configurations */
IP_CONST Ipcom_sysvar_ext ipdhcpc6_conf_sysvar_ext[] =
{
    /* Table of interfaces; and their status */
    /* Modes are; automatic = 3, enable = 2, disable = 1 */
    /*
     * DHCPC6_IF_ENUM_LIST is a list of interface/Status pairs, each separated
     * from the other by a semi-colon as follows:
     * "eth0=enable;vlan21=automatic"
     */
    { "ipdhcpc6.if.enum", IP_NULL, DHCPC6_IF_ENUM_LIST   },

    /*
     * DHCPC6_IF_RAPID_COMMIT_LIST is a list of interface/Rapid Commit Status
     * pairs, each separated from the other by a semi-colon as follows:
     * "eth0=enable;vlan21=automatic"
     */
    { "ipdhcpc6.if", "rapid_commit", DHCPC6_IF_RAPID_COMMIT_LIST    },

    /*
     * DHCPC6_IF_INFORMATION_ONLY_LIST is a list of interface/Information Only
     * Status pairs, each separated from the other by a semi-colon as follows:
     * "eth0=enable;vlan21=automatic"
     */
    { "ipdhcpc6.if","information_only", DHCPC6_IF_INFORMATION_ONLY_LIST  },

    /*
     * DHCPC6_IF_DNS_LIST is a list of interface/DNS Status pairs,
     * each separated from the other by a semi-colon as follows:
     * "eth0=enable;vlan21=automatic"
     */
    { "ipdhcpc6.if","dns", DHCPC6_IF_DNS_LIST   },

    /* DHCPC6_IF_SNTP_LIST is a list of interface/SNTP status pair
     * each separated from the other by a semi-colon as follow:
     * "eth0=enable;vlan21=enable"
     */
    { "ipdhcpc6.if","sntp", DHCPC6_IF_SNTP_LIST },

    /*
     * DHCPC6_IF_INFO_REFRESH_LIST is a lis of interface/Information Refresh
     * Status pair, each separated from the other by a semi-colon as follows:
     * "eth0=enable;vlan21=enable". 
     */
    { "ipdhcpc6.if", "information_refresh", DHCPC6_IF_INFO_REFRESH_LIST },

    /*
     * DHCPC6_IF_INFO_REFRESH_MIN_LIST is a list of interface/Information 
     * Refresh Minimum Value pair, each separated from the other by a 
     * semi-colon as follows:
     * "eth0=600;vlan21=600".
     */
    { "ipdhcpc6.if", "information_refresh.minimum", 
       DHCPC6_IF_INFO_REFRESH_MIN_LIST },

    /*
     * DHCPC6_IF_INFO_REFRESH_DEFAULT_LIST is a list of interface/Information
     * Refresh Default Value pair, each separated from the other by a 
     * semi-colon as follows:
     * "eth0=86400;vlan21=86400".
     */ 
    { "ipdhcpc6.if", "information_refresh.default",  
       DHCPC6_IF_INFO_REFRESH_DEFAULT_LIST },

    /*
     * DHCPC6_IF_INFO_REFRESH_MAX_LIST is a list of interface/Information 
     * Refresh Maximum Value pair, each separated from the other by a 
     * semi-colon as follows:
     * "eth0=0;vlan21=0".
     */
    { "ipdhcpc6.if", "information_refresh.maximum", 
       DHCPC6_IF_INFO_REFRESH_MAX_LIST },

    /*
     * DHCPC6_IF_IA_NA_DEFAULT_LIST a list of interface/IA_NA Default Status
     * pairs, each separated from the other by a semi-colon as follows:
     * "eth0=enable;vlan21=automatic"
     */
    /* Table definitions specify what IA_?? are available */
    { "ipdhcpc6.if","ia_na.enum.default", DHCPC6_IF_IA_NA_DEFAULT_LIST  },

    /*
     * DHCPC6_IF_IA_NA_DEFAULT_IAID_LIST is list of interface/IA_NA Default
     * IAID pairs, each separated from the other by a semi-colon as follows:
     * "eth0=1;vlan21=2"
     */
    /* The IAID; if not specified, will be automatically generated */
    { "ipdhcpc6.if","ia_na.default.iaid", DHCPC6_IF_IA_NA_DEFAULT_IAID_LIST },

    /*
     * DHCPC6_IF_HINTS_DEFAULT_ENUM_LIST is a list of interface/Default Hints
     * Status pairs, each separated from the other by a semi-colon as follows:
     * "eth0=enable;vlan21=automatic"
     */
    /* Hints for this IA */  
    { "ipdhcpc6.if","ia_na.default.hints.enum.default", DHCPC6_IF_HINTS_DEFAULT_ENUM_LIST },

    /*
     * DHCPC6_IF_HINTS_DEFAULT_PREFIX_LIST is a list of interface/Default
     * Prefix Hints pairs, each separated from the other by a semi-colon as
     * follows:
     * "eth0=3FFE:01;vlan21=3FFE:02"
     */
    { "ipdhcpc6.if","ia_na.default.hints.default.prefix", DHCPC6_IF_HINTS_DEFAULT_PREFIX_LIST  },

    /*
     * DHCPC6_IF_HINTS_DEFAULT_VALID_LIST is a list of interface/Default Valid
     * Hints pairs, each separated from the other by a semi-colon as follows:
     * "eth0=8000;vlan21=7000"
     */
    { "ipdhcpc6.if","ia_na.default.hints.default.valid", DHCPC6_IF_HINTS_DEFAULT_VALID_LIST    },

    /*
     * DHCPC6_IF_HINTS_DEFAULT_VALID_LIST is a list of interface/Default
     * Preferred Hints pairs, each separated from the other by a semi-colon as
     * follows:
     * "eth0=8000;vlan21=7000"
     */
    { "ipdhcpc6.if","ia_na.default.hints.default.preferred", DHCPC6_IF_HINTS_DEFAULT_PREFERRED_LIST },

    /*
     * DHCPC6_IF_HINTS_DEFAULT_RENEW_LIST is a list of interface/Default Renew
     * Hints pairs, each separated from the other by a semi-colon as follows:
     * "eth0=8000;vlan21=4000"
     */
    { "ipdhcpc6.if","ia_na.default.hints.renew", DHCPC6_IF_HINTS_DEFAULT_RENEW_LIST },

    /*
     * DHCPC6_IF_HINTS_DEFAULT_REBIND_LIST is a list of interface/Default
     * Rebind Hints pairs, each separated from the other by a semi-colon as
     * follows:
     * "eth0=8000;vlan21=4000"
     */
    { "ipdhcpc6.if","ia_na.default.hints.rebind", DHCPC6_IF_HINTS_DEFAULT_REBIND_LIST },

    { IP_NULL, IP_NULL, IP_NULL }
};

#else  /* !WRS_IPNET */
/* Old style ipeak sysvar definitions for other ports to work */

/*
 *===========================================================================
 *                         ipdhcpc6_conf_sysvar
 *===========================================================================
 *
 */
IP_CONST Ipcom_sysvar ipdhcpc6_conf_sysvar[] =
{
#ifdef IPDHCPC6_USE_DEFAULT_CONFIG
    /* LL, LLT or EN. Default LL */
    { "ipdhcpc6.duid.type",                                     "ll"        },

    /* For LL/LLT; if not specified first usable interface will be used */
    { "ipdhcpc6.duid.if",                                       "eth1"      },
    /* For EN; must be specified */
    { "ipdhcpc6.duid.en.number",                                "1000"      },
    { "ipdhcpc6.duid.en.value",                                 "0xabcdef"  },
    { "ipdhcpc6.authkey", DHCPC6_HMAC_MD5_SECRET },
    { "ipdhcpc6.authrealm", DHCPC6_AUTHENTICATION_REALM },



    /* Table of interfaces; and their status */
    /* Modes are; automatic = 3, enable = 2, disable = 1 */
    { "ipdhcpc6.if.enum.eth1",                                  "enable"    },

    /* Rapid commit     */
    { "ipdhcpc6.if.eth1.rapid_commit",                          "enable"    },
    /* Information only */
    { "ipdhcpc6.if.eth1.information_only",                      "disable"   },
    /* DNS */
    { "ipdhcpc6.if.eth1.dns",                                   "enable"    },
    /* SNTP */
    { "ipdhcpc6.if.eth1.sntp",                                  "enable"    },

    { "ipdhcpc6.if.eth1.information_refresh",                   "enable"    },
    { "ipdhcpc6.if.eth1.information_refresh.minimum",           "600"       },
    { "ipdhcpc6.if.eth1.information_refresh.default",           "86400"     },
    /* Infinity */
    { "ipdhcpc6.if.eth1.information_refresh.maximum",           "0"         },


    /* Table definitions specify what IA_?? are available */
    { "ipdhcpc6.if.eth1.ia_na.enum.default",                    "enable"    },

    /* The IAID; if not specified, will be automatically generated */
    { "ipdhcpc6.if.eth1.ia_na.default.iaid",                    "1"    },

    /* Hints for this IA */  
    { "ipdhcpc6.if.eth1.ia_na.default.hints.enum.default",      "enable"  },
    { "ipdhcpc6.if.eth1.ia_na.default.hints.default.prefix",    "3FFE:302:111:200::1/64"  },
    { "ipdhcpc6.if.eth1.ia_na.default.hints.default.valid",     "8000"    },
    { "ipdhcpc6.if.eth1.ia_na.default.hints.default.preferred", "6000"    },


    { "ipdhcpc6.if.enum.vlan21",                                "enable"    },

    /* Rapid commit     */
    { "ipdhcpc6.if.vlan21.rapid_commit",                          "disable"   },

    /* Information only */
    { "ipdhcpc6.if.vlan21.information_only",                      "disable"   },

    /* Table definitions specify what IA_?? are available */
    { "ipdhcpc6.if.vlan21.ia_na.enum.default",                    "enable"    },

    /* The IAID; if not specified, will be automatically generated */
    { "ipdhcpc6.if.vlan21.ia_na.default.iaid",                    "2"    },

    { "ipdhcpc6.if.vlan21.ia_na.default.hints.renew",             "2000"  },
    { "ipdhcpc6.if.vlan21.ia_na.default.hints.rebind",            "4000"  },


    { "ipdhcpc6.if.enum.vlan22",                                "enable"    },

    /* Rapid commit     */
    { "ipdhcpc6.if.vlan22.rapid_commit",                          "disable"   },
    /* Information only */
    { "ipdhcpc6.if.vlan22.information_only",                      "disable"   },

    /* Table definitions specify what IA_?? are available */
    { "ipdhcpc6.if.vlan22.ia_na.enum.default",                    "enable"    },

    /* The IAID; if not specified, will be automatically generated */
    { "ipdhcpc6.if.vlan22.ia_na.default.iaid",                    "3"    },
#endif /* IPDHCPC6_USE_DEFAULT_CONFIG */

    { IP_NULL, IP_NULL }
};


#endif 

/*
 ****************************************************************************
 * 8                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

extern void ipdhcpc6_authdb_init (void);
extern void ipdhcpc6_block_on_user_authdb_config (void);
extern Ip_err ipdhcpc6_get_auth_realm (int *, Ip_u8 **);

/*
 *===========================================================================
 *                    ipdhcpc6_configure
 *===========================================================================
 * Description: Configure DHCPv6 client.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipdhcpc6_configure(void)
{

    if (ipcom_sysvar_setv(ipdhcpc6_conf_sysvar, 0) == IPCOM_SUCCESS)
    {   Ip_u8 *auth_realm;
        int    auth_realm_len;

        ipdhcpc6_authdb_init ();

        ipdhcpc6_get_auth_realm (&auth_realm_len, &auth_realm);
        if (auth_realm_len)
            ipdhcpc6_block_on_user_authdb_config ();

        /* now configure the interface-specific sysvars */

#if defined(WRS_IPNET) || defined(PRJ_BUILD)
        return ipcom_sysvar_ext_setv (ipdhcpc6_conf_sysvar_ext, 0);
#else 
	return IPCOM_SUCCESS;
#endif 
    }

    return IPCOM_ERR_INVALID_CONFIG;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */




