/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipl2tp_config.c,v $ $Revision: 1.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipl2tp_config.c,v $
 *   $Author: tneale $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *
 ******************************************************************************
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
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_shell.h>
#include <ipcom_sysvar.h>

#include <ipl2tp.h>
#include <ipl2tp_cmd.h>

/*
 ****************************************************************************
 * 3                    DEFINES
 ****************************************************************************
 */

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

IP_PUBLIC Ip_err ipl2tp_configure(void);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */
IP_STATIC IP_CONST Ipcom_sysvar ipl2tp_conf_sysvar[] =
{
    { "ipl2tp.v3.control.any.attributes", "secret=interop local_router_id=0" },
    { "ipl2tp.v3.circuit.0.name",         "mycircuit" },
    { "ipl2tp.v3.circuit.0.interface",    "eth0"      },
    { "ipl2tp.v3.circuit.0.vlan",         "21"        },
    { "ipl2tp.v3.circuit.0.attributes",   "pseudowire_type=ethernet_vlan data_sequencing=0" },

    /* End of configuration - DO NOT REMOVE! */
	{ IP_NULL, IP_NULL }
};

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

/*
 *===========================================================================
 *                    ipl2tp_configure
 *===========================================================================
 * Description: Configure FTP server.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipl2tp_configure(void)
{
    /* Add the sysvars */
    ipcom_sysvar_setv(ipl2tp_conf_sysvar, 0);

    /* Init shell commands */
    (void)ipcom_shell_add_cmd("l2tp", "l2tp <command> <parameters>",
                              "L2TP client control",
                              ipl2tp_cmd_main,
                              IPCOM_SHELL_PRIO_SAME,
                              IPCOM_PROC_STACK_DEFAULT);

    return IPCOM_SUCCESS;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
