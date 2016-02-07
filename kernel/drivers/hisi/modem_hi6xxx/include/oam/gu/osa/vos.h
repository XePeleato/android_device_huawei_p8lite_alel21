/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: vos.h                                                           */
/*                                                                           */
/* Author: Yang Xiangqian                                                    */    
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2006-10                                                             */
/*                                                                           */
/* Description:                                                              */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*                                                                           */
/* 2. Date: 2006-10                                                          */
/*    Author: Xu Cheng                                                       */
/*    Modification: Standardize code                                         */
/*                                                                           */
/*****************************************************************************/


#ifndef  _VOS_H
#define  _VOS_H

#include "dopra_def.h"

#if ((VOS_VXWORKS == VOS_OS_VER) || (VOS_RTOSCK == VOS_OS_VER) || (VOS_WIN32 == VOS_OS_VER))
#include "stdio.h"
#endif

#include "vos_config.h"
#include "v_typdef.h"
#include "v_iddef.h"
#include "v_id.h"
#include "v_queue.h"
#include "v_msg.h"
#include "vos_Id.h"
#include "v_root.h"
#include "v_task.h"
#include "v_sem.h"
#include "v_timer.h"
#include "v_blkMem.h"
#include "v_IO.h"
#include "v_int.h"
#include "v_lib.h"
#include "WatchDog.h"
#include "VosTaskPrioDef.h"

#endif /* _VOS_H */

