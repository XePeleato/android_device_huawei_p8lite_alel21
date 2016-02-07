#ifndef __MDRV_ACORE_EFUSE_H__
#define __MDRV_ACORE_EFUSE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "mdrv_efuse_common.h"

    /*************************************************
     *  函 数 名   : mdrv_efuse_get_chipid
     *  功能描述   : mdrv_efuse_get_chipid
     *  输入参数   : buf: data buffer
     *               len: length of the group
     *  输出参数   :
     *  返 回 值   : OK                (0)
     *               BUF_ERROR         (-55)
     *               LEN_ERROR         (-56)
     *               READ_EFUSE_ERROR  (-57)
     *  修改历史   :
     *  日    期 : 2013年8月12日
     *  作    者 : l00225826
     *  修改内容 : 新生成函数
     *************************************************/
    int mdrv_efuse_get_chipid(unsigned char* buf,int length);

    /*************************************************
     *  函 数 名   : mdrv_efuse_get_dieid
     *  功能描述   : mdrv_efuse_get_dieid
     *  输入参数   : buf: data buffer
     *               len: length of the group
     *  输出参数   :
     *  返 回 值   : OK                (0)
     *               BUF_ERROR         (-55)
     *               LEN_ERROR         (-56)
     *               READ_EFUSE_ERROR  (-57)

     *  修改历史   :
     *  日    期 : 2013年8月12日
     *  作    者 : l00225826
     *  修改内容 : 新生成函数
     *************************************************/
    int mdrv_efuse_get_dieid(unsigned char* buf, int length);


#ifdef __cplusplus
}
#endif
#endif
