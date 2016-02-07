/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: iptftpc_cmd_tftp.c,v $ $Revision: 1.4 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/src/iptftpc_cmd_tftp.c,v $
 *   $Author: gzadoyan $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
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

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_getopt.h>

#include "iptftp.h"
#include "iptftp_h.h"


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

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC int iptftp_cmd_tftp(int argc, char **argv);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         iptftp_cmd_tftp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
iptftp_cmd_tftp(int argc, char **argv)
{
    Ip_bool binary = IP_TRUE;
    char *src = IP_NULL, *dst = IP_NULL;
    Ip_s32 i;
    Ip_u16 operation = IPTFTP_OPCODE_RRQ;
    Ip_err ret;
    Ipcom_getopt  opt;

    ipcom_getopt_clear_r(&opt);
    while ((i = ipcom_getopt_r(argc, argv, "a", &opt)) != -1)
    {
        switch (i)
        {
        case 'a':
            binary = IP_FALSE;
            break;
        default:
            ipcom_printf("Unknown switch"IP_LF);
            return -1;
        }
    }

    if (argc - opt.optind < 3)
    {
        /* Interactive mode not supported */
        goto usage;
    }

    /* Get the operation */
    if ((opt.optind+1) < argc)
    {
        if (!ipcom_strcmp(argv[opt.optind+1], "GET") || !ipcom_strcmp(argv[opt.optind+1], "get"))
            operation = IPTFTP_OPCODE_RRQ;
        else if (!ipcom_strcmp(argv[opt.optind+1], "PUT") || !ipcom_strcmp(argv[opt.optind+1], "put"))
            operation = IPTFTP_OPCODE_WRQ;
        else
            goto usage;
    }

    /* Get the source file and optionally the destination file */
    if ((opt.optind+2) < argc)
        src = argv[opt.optind+2];
    if ((opt.optind+3) < argc)
        dst = argv[opt.optind+3];

    /* Call the tftp api */
    if (operation == IPTFTP_OPCODE_RRQ)
        ret = iptftp_client_get(argv[opt.optind], src, dst, binary, IP_TRUE);
    else
        ret = iptftp_client_put(argv[opt.optind], src, dst, binary, IP_TRUE);

    switch(ret)
    {
        case IPTFTP_SUCCESS:
            break;
        case IPTFTP_ERR_SERVER_ADDR:
            ipcom_printf("Unknown host: %s"IP_LF, argv[opt.optind]);
            break;
        case IPTFTP_ERR_MEMORY_FAIL:
            ipcom_printf("Out of memory."IP_LF);
            break;
        case IPTFTP_ERR_FILE_OPEN:
            if (operation == IPTFTP_OPCODE_RRQ)
                ipcom_printf("Could not create file: %s"IP_LF, dst == IP_NULL ? src : dst);
            else
                ipcom_printf("Could not open file: %s"IP_LF, src);
            break;
        case IPTFTP_ERR_FILE_READ:
            ipcom_printf("Could not read from file: %s"IP_LF, src);
            break;
        case IPTFTP_ERR_FILE_WRITE:
            ipcom_printf("Could not write to file: %s"IP_LF, dst == IP_NULL ? src : dst);
            break;
        case IPTFTP_ERR_TRANSFER_TIMEOUT:
            ipcom_printf("Timeout in TFTP transfer."IP_LF);
            break;
        case IPTFTP_ERR_SOCKET_FAIL:
            ipcom_printf("Socket operation failed."IP_LF);
            break;
        case IPTFTP_ERR_ILLEGAL_MESSAGE:
            ipcom_printf("Illegal TFTP message received."IP_LF);
            break;
        case IPTFTP_ERR_ERROR_MESSAGE:
            ipcom_printf("Received error message during transfer."IP_LF);
            break;
        default:
            break;
    }

    return 0;
usage:
    ipcom_printf("usage: tftp [-a] <host[:port]> <get|put> <source> [destination]"IP_LF);
    return 0;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

