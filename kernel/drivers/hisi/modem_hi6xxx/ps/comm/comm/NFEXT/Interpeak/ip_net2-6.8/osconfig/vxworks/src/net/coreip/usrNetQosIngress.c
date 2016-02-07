/* usrNetQosIngress.c - Ingress QoS initialization */

/*
 * Copyright (c) 2005 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement. No license to Wind River intellectual property rights
 * is granted herein. All rights not licensed by Wind River are reserved
 * by Wind River.
 */

/*
modification history
--------------------
01a,14aug05,niq  written
*/

/*
DESCRIPTION

*/

/* extern */

IMPORT QOS_CONFIG_PARAMS        qosDefaultConfigParams;
IMPORT JOB_QUEUE_ID             QOS_JOBQ;

STATUS usrNetQosIngressLibInit (void)
    {
    /* Initialize parameters for the qosIngressLib */

    qosDefaultConfigParams.cfg_pJobQueue = QOS_JOBQ;
    qosDefaultConfigParams.cfg_jobQPri = QOS_JOBQ_PRI;
    qosDefaultConfigParams.cfg_qosDefaultPri = QOS_DEFAULT_PRI;

    return (qosIngressLibInit(&qosDefaultConfigParams));
    }

