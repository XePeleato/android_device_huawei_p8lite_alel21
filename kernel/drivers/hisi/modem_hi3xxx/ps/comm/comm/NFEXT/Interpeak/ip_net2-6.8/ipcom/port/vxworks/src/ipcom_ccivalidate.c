/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_ccivalidate.c,v $ $Revision: 1.7 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_ccivalidate.c,v $
 *     $Author: braghava $ $Date: 2007-10-04 06:33:42 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING 
 *     Design and implementation by Roger Boden <roger@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 *
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
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_os.h"
#include "ipcom_ccivalidate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
****************************************************************************
* 5                    DEFINES
****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    TYPES
 ****************************************************************************
 */


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

/*
 ****************************************************************************
 * 9                    FUNCTIONS
 ****************************************************************************
 */

/*
*****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_cmd_wvnet
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
#ifdef IPCOM_CCIVALIDATE_TEST

#include "cci.h"

IP_GLOBAL int
ipcom_cmd_ccivalidate(int argc, char **argv)
{
    int              provider_num = 0, ret = 0;
    /* Validate arguments */
    if (argc != 2)
    {
    	ipcom_printf("Usage: ccivalidate <provider>. 0 is the default provider. \n");
        return -1;
    }
    provider_num=atoi(argv[1]);
    ret=cciProviderValidate(provider_num);

    /* Check return values of cciProviderValidate*/
    if(ret == CCI_SUCCESS)
    	return 0;
    else
	return -1;
}
#endif /* IPCOM_CCIVALIDATE_TEST */


#ifdef IPCOM_CCIPERFORMANCE_TEST

#include "cci.h"

IP_GLOBAL int
ipcom_cmd_cciperformance(int argc, char **argv)
{
    int provider_num = 0, ret = 0;
    /* Validate arguments */
    if (argc != 2)
    {
    	ipcom_printf("Usage: cciperf <provider>. 0 is the default provider. \n");
        return -1;
    }
    provider_num=atoi(argv[1]);

    ret=cciLibUnInit();
    
    if(ret != CCI_SUCCESS)
	return -1;

    cciSharedRegionSet(8192000);

    ret=cciLibInit();

    if(ret != CCI_SUCCESS)
	return -1;

    ret=cciProviderBenchmark(provider_num);

    /* Check return values of cciProviderValidate*/
    if(ret == CCI_SUCCESS)
    	return 0;
    else
	return -1;
}


#endif /* IPCOM_CCIPERFORMANCE_TEST */

/*
*****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/* None as of now 
 *
 */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
