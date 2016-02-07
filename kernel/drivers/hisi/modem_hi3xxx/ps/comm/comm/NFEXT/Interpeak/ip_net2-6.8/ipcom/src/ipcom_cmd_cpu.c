/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_cpu.c,v $ $Revision: 1.4 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_cpu.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus.carlstedt@windriver.com>
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


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */


#define IPCOM_USE_CLIB_PROTO
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_os.h"
#include "ipcom_err.h"


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

IP_PUBLIC int ipcom_cmd_cpu(int argc, char **argv);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_cmd_cpu_getpid
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC Ip_pid_t
ipcom_cmd_cpu_getpid(char *pidstr)
{
    if (pidstr[0] == '0' && pidstr[1] == 'x')
        return (Ip_pid_t)ipcom_strtoul(pidstr, IP_NULL, 16);
    else
        return (Ip_pid_t)ipcom_strtoul(pidstr, IP_NULL, 10);
}


/*
 ****************************************************************************
 * 10                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_cmd_cpu
 *===========================================================================
 * Description:     Shell command: set/get cpu affinity
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0
 *
 */
IP_PUBLIC int
ipcom_cmd_cpu(int argc, char **argv)
{
    Ip_pid_t pid;
    Ip_u32   mask;
    Ip_cpu_set_t cpuset;
    Ip_err ret;
    Ip_bool all = IP_FALSE;

    if (argc < 3)
    {
        if (argc == 2 && ipcom_strcmp(argv[1], "num") == 0)
            ipcom_printf("number of CPUs: configured %d online %d\n",
                         ipcom_num_configured_cpus(),
                         ipcom_num_online_cpus());
        else
            goto usage;
    }
    else
    {
        IP_CPU_ZERO(&cpuset);
        if (ipcom_strcmp(argv[1], "get") == 0)
        {
            pid = ipcom_cmd_cpu_getpid(argv[2]);
            ret = ipcom_proc_cpu_affinity_get(pid, &cpuset);
            if (ret != IPCOM_SUCCESS)
                ipcom_printf("failed to get cpu affinity for pid %x\n", pid);
            else
                ipcom_printf("cpu affinity is 0x%x for pid %x\n", cpuset.set[0], pid);
        }
        else if (ipcom_strcmp(argv[1], "set") == 0)
        {
            if (argc < 4)
                goto usage;
            if (ipcom_strcmp(argv[2], "all") == 0)
                all = IP_TRUE;
            else
                pid = ipcom_cmd_cpu_getpid(argv[2]);
            mask = (Ip_u32)ipcom_strtol(argv[3], IP_NULL, 16);
            cpuset.set[0] = mask;
            if (all == IP_TRUE)
            {
                ret = ipcom_proc_cpu_affinity_set_all(&cpuset);
                if (ret != IPCOM_SUCCESS)
                    ipcom_printf("failed to set cpu affinity to 0x%x for all pids\n",  mask);
            }
            else
            {
                ret = ipcom_proc_cpu_affinity_set(pid, &cpuset);
                if (ret != IPCOM_SUCCESS)
                    ipcom_printf("failed to set cpu affinity to 0x%x for pid %x\n",  mask, pid);
            }
        }
        else
            goto usage;
    }
    return 0;

usage:
    ipcom_printf("usage: 'cpu <num | get <pid> | set <pid> <mask (in hex)>>'\n");
    return 0;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

