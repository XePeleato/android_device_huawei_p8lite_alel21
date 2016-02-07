/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: ../TARGET/config/v_MemCfg.inc                                   */
/*                                                                           */
/* Author:                                                                   */
/*                                                                           */
/* Version:  1.0                                                             */
/*                                                                           */
/* Date:                                                                     */
/*                                                                           */
/* Description: create this file                                             */
/*                                                                           */
/*****************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#ifndef _V_MEMCFG_INC
#define _V_MEMCFG_INC

#if (RAT_MODE != RAT_GU)

#if (OSA_CPU_CCPU == VOS_OSA_CPU)

/* modify this to config memmory */
/* defination of message buffer */
VOS_MEM_BLOCK_INFO MsgBlockInfo[VOS_MEM_CTRL_BLOCK_NUMBER] =
{
    { 32,     40  },
    { 96,     500 },
    { 224,    240 },
    { 480,    300 },
    { 992,    20  },
    { 2016,   70  },
    { 4160,   10  },
    { 8160,   2   },
    { 16352,  2   },
    { 24992,  2   },
    { 80000,  1   },
    { 100000, 1   },
    { 64,     VOS_TIMER_MESSAGE_NUM },/* only for Timer */
};


/* defination of memory buffer */
VOS_MEM_BLOCK_INFO MemBlockInfo[VOS_SIMPLE_MEM_CTRL_BLOCK_NUMBER] =
{
    { 32,     1000},
    { 92,     4000},
    { 116,    500 },
    { 256,    300 },
    { 532,    200 },
    { 1600,   200 },
    { 2080,   100 },
    { 4480,   30  },
    { 8212,   6   },
    { 16404,  6   },
    { 35848,  16  },
    { 64000,  4  },
    { 65536,  7   },
    { 80000,  3   },
    { 640000, 3   },
    { 800000, 1   },
    {1600000, 1   },
};
#endif

#if (OSA_CPU_ACPU == VOS_OSA_CPU)

VOS_MEM_BLOCK_INFO MsgBlockInfo[VOS_MEM_CTRL_BLOCK_NUMBER] =
{
    { 32,     40  },
    { 100,    40  },
    { 224,    100 },
    { 480,    40  },
    { 800,    40  },
    { 2416,   40  },
    { 4160,   40  },
    { 8160,   2   },
    { 16352,  2   },
    { 24992,  2   },
    { 80000,  1   },
    { 64,     VOS_TIMER_MESSAGE_NUM },/* only for Timer */
};


/* defination of memory buffer */
VOS_MEM_BLOCK_INFO MemBlockInfo[VOS_SIMPLE_MEM_CTRL_BLOCK_NUMBER] =
{
    { 40,     40},
    { 92,     40},
    { 256,    100 },
    { 532,    100 },
    { 1600,   100 },
    { 2080,   100 },
    { 4480,   16  },
    { 16404,  6   },
    { 35848,  4   },
	{ 65536,  2   },
};
#endif

#endif

#if (RAT_MODE == RAT_GU)

#if (OSA_CPU_CCPU == VOS_OSA_CPU)

/* modify this to config memmory */
/* defination of message buffer */
VOS_MEM_BLOCK_INFO MsgBlockInfo[VOS_MEM_CTRL_BLOCK_NUMBER] =
{
    { 36,     40  },
    { 104,    60  },
    { 228,    60  },
    { 480,    20  },
    { 992,    20  },
    { 2084,   20  },
    { 4968,   10  },
    { 8160,   2   },
    { 16352,  2   },
    { 27168,  2   },
    { 80000,  1   },
    { 100000, 1   },
    { 64,     VOS_TIMER_MESSAGE_NUM },/* only for Timer */
};


/* defination of memory buffer */
VOS_MEM_BLOCK_INFO MemBlockInfo[VOS_SIMPLE_MEM_CTRL_BLOCK_NUMBER] =
{
    { 36,     500 },
    { 92,     1000},
    { 116,    250 },
    { 256,    150 },
    { 560,    100 },
    { 1776,   80  },
    { 2080,   50  },
    { 4556,   30  },
    { 8212,   5   },
    { 16404,  5   },
    { 35848,  14  },
    { 64000,  4   },
    { 65536,  4   },
    { 67960,  1   },
};

#endif


#if (OSA_CPU_ACPU == VOS_OSA_CPU)

/* modify this to config memmory */
/* defination of message buffer */
VOS_MEM_BLOCK_INFO MsgBlockInfo[VOS_MEM_CTRL_BLOCK_NUMBER] =
{
    { 36,     20  },
    { 100,    70  },
    { 224,    20  },
    { 480,    20  },
    { 800,    20  },
    { 2416,   20  },
    { 4160,   20  },
    { 8532,   2   },
    { 64,     VOS_TIMER_MESSAGE_NUM },/* only for Timer */
};


/* defination of memory buffer */
VOS_MEM_BLOCK_INFO MemBlockInfo[VOS_SIMPLE_MEM_CTRL_BLOCK_NUMBER] =
{
    { 40,     40 },
    { 92,     40 },
    { 256,    40  },
    { 532,    20   },
    { 1600,   20   },
    { 2080,   20   },
    { 4480,   10   },
    { 16404,  4    },
    { 35848,  2    },
};

#endif

#endif

#endif /* _V_MEMCFG_INC */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


