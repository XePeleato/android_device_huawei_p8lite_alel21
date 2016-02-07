/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_windview.c,v $ $Revision: 1.1 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_windview.c,v $
 *     $Author: markus $ $Date: 2007-05-31 08:42:32 $
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
#include "ipcom_windview.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <wvLib.h>
#include <wvNetDLib.h>
#include <rBuffLib.h>
#include <private/wvUploadPathP.h>
#include <private/wvFileUploadPathLibP.h>


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
#ifdef IPCOM_WV_INSTRUMENTATION_TEST
IP_GLOBAL int
ipcom_cmd_wvnet(int argc, char **argv)
{
    char             cmd, *filename = IP_NULL;
    int              sample_time = 30, ret = 0;
    BUFFER_ID   	 rBuff;
    UPLOAD_ID		 wvUpPathId;
	WV_UPLOADTASK_ID wvUpTaskId;
    extern rBuffCreateParamsType wvDefaultRBuffParams;

    if (argc < 2)
    {
        ipcom_printf("Usage: wvnet <start | stop <file> | auto <file> [sampletime]>\n");
        return -1;
    }

    if (ipcom_strcmp(argv[1], "start") == 0)
    {
        cmd = 'B';
    }
    else if (ipcom_strcmp(argv[1], "stop") == 0)
    {
        cmd = 'E';
        if (argv[2] == IP_NULL)
        {
            ipcom_printf("Must supply a filename!\n");
            return -1;
        }
        filename = argv[2];
    }
    else if (ipcom_strcmp(argv[1], "auto") == 0)
    {
        cmd = 'A';
        if (argv[2] == IP_NULL)
        {
            ipcom_printf("Must supply a filename!\n");
            return -1;
        }
        filename = argv[2];
        if (argv[3] != IP_NULL)
        {
            sample_time = ipcom_atoi(argv[3]);
            if (sample_time < 0)
                sample_time = 30;
        }
    }
    else
    {
        ipcom_printf("Unknown command: %s\n", argv[1]);
        return -1;
    }

    if (cmd == 'B' || cmd == 'A')
    {
        /* create log list if required */
        if (wvCurrentLogListGet() == IP_NULL)
        {
            if (wvLogListCreate() == IP_NULL)
            {
                ipcom_printf("Could not create log list.\n");
                return -1;
            }
        }

        /* change rBuff params if required, then create the buffer */
        wvDefaultRBuffParams.minimum = 8;
        wvDefaultRBuffParams.maximum = 8;
        wvDefaultRBuffParams.buffSize = (1024 * 1024);
        wvDefaultRBuffParams.threshold = (512 * 1024);

        rBuff = rBuffCreate (&wvDefaultRBuffParams);
        if (rBuff == IP_NULL)
        {
            ipcom_printf ("Failed to create rBuff.\n");
            return -1;
        }

        /* create wv log using the rBuff just created */
        if (wvLogCreate (rBuff) == IP_NULL)
        {
            ipcom_printf ("Error creating log.\n");
            rBuffDestroy (rBuff);
            return -1;
        }

        /* level 3 logging (additional instrumentation */
        wvEvtClassSet (WV_CLASS_3);

        /* Enable network events */
        wvNetDConfigure (1, WV_NETD_VERBOSE, 0);

        /* Start logging */
        wvEvtLogStart();

        if (cmd == 'B')
        {
            ipcom_printf("Logging network events..."IP_LF);
            return ret;
        }
        else if (cmd == 'A')
        {
            ipcom_printf("Logging network events for %d seconds..."IP_LF, sample_time);
            ipcom_sleep(sample_time);
        }
    }

    /* Stop logging */
    wvEvtLogStop ();
    ipcom_printf("Stopped logging network events."IP_LF);

    /* Upload the log to file */
    wvUpPathId = wvFileUploadPathCreate (filename, O_WRONLY | O_TRUNC | O_CREAT);
    if (wvUpPathId == IP_NULL)
    {
        ipcom_printf ("Error creating file upload path.\n");
        ret = -1;
        goto exit;
    }
    wvUpTaskId = wvUploadStart (wvCurrentLogGet(), wvUpPathId, IP_FALSE);
    if (wvUpTaskId == IP_NULL)
    {
        ipcom_printf ("Error launching upload.\n");
        ret = -1;
        goto exit;
    }

    /* Finish uploading */
    wvUploadStop (wvUpTaskId);

exit:
    /* Close the upload path and destroy the event buffer */
    if (wvUpPathId != IP_NULL)
        wvFileUploadPathClose (wvUpPathId);
    wvLogDelete (wvCurrentLogListGet(), wvCurrentLogGet());
    return ret;
}
#endif /* #ifdef IPCOM_WV_INSTRUMENTATION_TEST */


/*
*****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_wv_marker_1
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC void
ipcom_wv_marker_1(Ip_u32 classId, Ip_u32 eventLevel, Ip_u32 eventOffset, Ip_u32 eventTag,
                  Ip_u32 eventId, void *arg1, Ip_u8 module, Ip_u8 entity)
{
#ifdef IPCOM_WV_INSTRUMENTATION
    Ip_u8 wvNetDModuleId    = module;
    Ip_u8 wvNetDLocalFilter = WV_NETD_NONE;
    Ip_u8 wvNetDEntityId    = entity;
    (void)wvNetDEventBlob; /* Avoid compiler warning */
#endif

    WV_NET_MARKER_1(classId, eventLevel, eventOffset, eventTag, eventId, arg1)
}


/*
 *===========================================================================
 *                    ipcom_wv_marker_2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC void
ipcom_wv_marker_2(Ip_u32 classId, Ip_u32 eventLevel, Ip_u32 eventOffset, Ip_u32 eventTag,
                  Ip_u32 eventId, void *arg1, int arg2, Ip_u8 module, Ip_u8 entity)
{
#ifdef IPCOM_WV_INSTRUMENTATION
    Ip_u8 wvNetDModuleId    = module;
    Ip_u8 wvNetDLocalFilter = WV_NETD_NONE;
    Ip_u8 wvNetDEntityId    = entity;
#endif

    WV_NET_MARKER_2(classId, eventLevel, eventOffset, eventTag, eventId, arg1, arg2)
}


/*
 *===========================================================================
 *                    ipcom_wv_event_2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC void
ipcom_wv_event_2(Ip_u32 classId, Ip_u32 eventLevel, Ip_u32 eventOffset, Ip_u32 eventTag,
                 Ip_u32 eventId, Ip_u32 status, void *arg1, int arg2, Ip_u8 module, Ip_u8 entity)
{
#ifdef IPCOM_WV_INSTRUMENTATION
    Ip_u8 wvNetDModuleId    = module;
    Ip_u8 wvNetDLocalFilter = WV_NETD_NONE;
    Ip_u8 wvNetDEntityId    = entity;
#endif

    WV_NET_EVENT_2(classId, eventLevel, eventOffset, eventTag, eventId, status, arg1, arg2)
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
