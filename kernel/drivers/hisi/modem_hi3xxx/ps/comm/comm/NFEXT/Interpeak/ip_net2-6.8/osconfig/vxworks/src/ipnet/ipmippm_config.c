/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipmippm_config.c,v $ $Revision: 1.4 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipmippm_config.c,v $
 *   $Author: jhorteli $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
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

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_sysvar.h>

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

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
IP_PUBLIC int ipmippm_cmd_pm(int argc, char **argv);

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipmippm_configure(void);
IP_PUBLIC Ip_err ipmippm_init(void);
IP_PUBLIC Ip_err ipmippm_exit(void);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *             Mobile Node sysvar Configuration
 *===========================================================================
 */
#if defined(INCLUDE_IPMIPPM)

IP_STATIC IP_CONST Ipcom_sysvar ipmippm_conf_sysvar[] =
{
    { "ipmippm.interfaces", MIPPM_INTERFACES},

#if 0
    /* Registration lifetime in seconds. 0 = infinite. */
    { "ipmippm.reg_lifetime", MIPPM_REG_LIFETIME},

    /* Receive broadcast packets from home network. */
    { "ipmippm.receivebroadcasts",MIPPM_RECV_BROADCASTS},

    /* Enable(1)/disable(0) reverse tunneling. */
    { "ipmippm.reversetunneling", MIPPM_REVERSE_TUNNELING},

    /* DHAA */
    { "ipmippm.dhaa",    MIPPM_DYNAMIC_HOME_AGENT_ASSIGNMENT },
#endif
    /* Configuration delimiter - DO NOT EDIT! */
	{ IP_NULL, IP_NULL }
};

IP_STATIC IP_CONST Ipcom_sysvar_ext ipmippm_conf_sysvar_ext[] =
{
#if 0
    /* Extended security association selection and creation */
    { "ipmippm.fa.spi", "secret", MIPPM_FA_SPI_SECRET_LIST},
    { "ipmippm.fa.spi", "method", MIPPM_FA_SPI_METHOD_LIST},
    { "ipmippm.ha.spi", "secret", MIPPM_HA_SPI_SECRET_LIST },
    { "ipmippm.ha.spi", "method", MIPPM_HA_SPI_METHOD_LIST},

    { "ipmippm.ha.sa.address", IP_NULL, MIPPM_HA_SA_ADDRESS_LIST},
    { "ipmippm.fa.sa.address", IP_NULL, MIPPM_FA_SA_ADDRESS_LIST},
#endif

    /* "ipip" = IP over IP (gif), "gre" = GRE, "min" = minimal encapsulation. */
    { "ipmippm.if", "tunneltype", MIPPM_TUNNEL_TYPE_ENUM},
    { "ipmippm.if", "homeagent",  MIPPM_HOME_AGENT_ENUM},


#if defined(INCLUDE_IPMIPPM_PMIPV4_DHCP)
    { "ipmippm.if", "pmipv4.dhcp.enable",   "enabled"},
    { "ipmippm.if", "pmipv4.dhcp.port",     MIPPM_PMIPV4_DHCP_CLIENT_PORT_ENUM},
#else
    { "ipmippm.if.%s.enable", IP_NULL, "disabled"},
#endif

    /* Configuration delimiter - DO NOT EDIT! */
	{ IP_NULL, IP_NULL, IP_NULL }
};

#else

IP_STATIC IP_CONST Ipcom_sysvar ipmippm_conf_sysvar[] =
{
    /* Configuration delimiter - DO NOT EDIT! */
	{ IP_NULL, IP_NULL }
};

#define INCLUDE_IPMIPPM_CMD             1
#define INCLUDE_IPMIPPM_PMIPV4_DHCP     1

#endif

#if defined(INCLUDE_IPMIPPM_FEATURE_EXTENSION)
extern void MIPPM_FEATURE_INITIALIZATION_HOOK(void);
#endif

#if defined(INCLUDE_IPMIPPM_PMIPV4_DHCP)
IP_GLOBAL void ipmippm_pmipv4_install(void);
#endif

typedef void (*Ipmippm_config_func_t) (void);
Ipmippm_config_func_t      ipmippm_function_registrations[] =
{
#if defined(INCLUDE_IPMIPPM_PMIPV4_DHCP)
    ipmippm_pmipv4_install,
#endif

#if defined(INCLUDE_IPMIPPM_FEATURE_EXTENSION)
    MIPPM_FEATURE_INITIALIZATION_HOOK,
#endif
    IP_NULL
};

/*
 ****************************************************************************
 * 9                    PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipmipmn_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipmippm_configure(void)
{
    Ip_err err;
    /* Initialize IPMIPMN system variables. */
    if ((err = ipcom_sysvar_setv(ipmippm_conf_sysvar, 0)) != IPCOM_SUCCESS)
        return err;

#if defined(INCLUDE_IPMIPPM)
    if ((err = ipcom_sysvar_ext_setv(ipmippm_conf_sysvar_ext, 0)) != IPCOM_SUCCESS)
        return err;
#endif

    return IPCOM_SUCCESS;
}

/*
 *===========================================================================
 *                    ipmippm_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipmippm_init(void)
{
#if defined(INCLUDE_IPMIPPM_CMD) && !defined(IP_PORT_LAS)
    (void)ipcom_shell_add_cmd("pm", "pm <command> [modifiers]", "Mobile IP Foreign Agent",
                        ipmippm_cmd_pm,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_SMALL);
#endif
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipmipfa_exit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipmippm_exit(void)
{
#if defined(INCLUDE_IPMIPPM_CMD) && !defined(IP_PORT_LAS)
    (void)ipcom_shell_remove_cmd("pm", ipmippm_cmd_pm);
#endif
    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

