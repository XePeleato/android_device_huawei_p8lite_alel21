/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_DRV_VIC.h
*
*   作    者 :  zhanghailun
*
*   描    述 :  中断模块用户接口文件
*
*   修改记录 :  2009年3月5日  v1.00  zhanghailun  创建
*************************************************************************/

#ifndef _BSP_DRVVIC_H_
#define _BSP_DRVVIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#define INT_LVL_MAX 160

/* 宏定义 */
#ifndef IVEC_TO_INUM 
#define IVEC_TO_INUM(intVec)    ((int) (intVec))
#endif
#define INUM_TO_IVEC(intNum)    ((VOIDFUNCPTR *) (intNum))

/*****************************************************************************
* 函 数 名  : BSP_DRV_SubVicInit
*
* 功能描述  : 二级中断初始化
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2009年3月5日   zhanghailun  creat
*****************************************************************************/
static int __init BSP_DRV_SubVicInit(void);


#ifdef __cplusplus
}
#endif

#endif /* end #define _BSP_VIC_H_*/

