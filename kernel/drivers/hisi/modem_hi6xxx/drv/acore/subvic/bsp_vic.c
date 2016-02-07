#include <linux/init.h>
#include <asm/io.h>
#include <asm/string.h>
#include <linux/interrupt.h>
#include "BSP.h"
#include "bsp_drv_vic.h"

/*modify by wangjing 2011-4-7*/
UINT32 g_subVicIntCheck=0;
VOIDFUNCPTR subVICIntHookPtr = (VOIDFUNCPTR) NULL;

static char g_subvic_irq_name[11] = "subvic_000";

/*modify by wangjing 2011-4-7 for 可维可测*/
void subVICIntHook(VOIDFUNCPTR p_Func)
{
    subVICIntHookPtr = p_Func;
}

void subVICIntHookDel(void)
{
    subVICIntHookPtr = (VOIDFUNCPTR) NULL;
}


/*****************************************************************************
* 函 数 名  : SubVic_IntHandler
*
* 功能描述  : 二级中断总挂接函数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2009年3月5日   zhanghailun  creat
*****************************************************************************/
irqreturn_t BSP_DRV_SubVicIntHandler(int irq, void *dev_id)
{
    return IRQ_HANDLED;
}

/*****************************************************************************
* 函 数 名  : SubVic_Init
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
static int __init BSP_DRV_SubVicInit(void)
{
    return 0;
}

/*****************************************************************************
* 函 数 名  : BSP_INT_Enable
*
* 功能描述  : 使能某个中断
*
* 输入参数  : BSP_S32 ulLvl 要使能的中断号，取值范围0～40
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   zhanghailun  creat
*****************************************************************************/

BSP_S32 BSP_INT_Enable ( BSP_S32 s32Lvl)
{
    BSP_S32 s32Result = OK;

    if(s32Lvl >= INT_LVL_MAX)
    {
        return ERROR;
    }

    if(s32Lvl < INT_LVL_MAX)
    {
        ;
    }

    return s32Result;
}/*lint !e529*/

int  mdrv_int_enable(int ulLev)
{
    return BSP_INT_Enable(ulLev);
}
/*****************************************************************************
 * 函 数 名  : BSP_INT_Disable
 *
 * 功能描述  : 去使能某个中断
 *
 * 输入参数  : BSP_S32 ulLvl 要使能的中断号，取值范围0～40
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_Disable ( BSP_S32 s32Lvl)
{
    BSP_S32 s32Result = OK;

    if(s32Lvl >= (INT_LVL_MAX))
    {
        return ERROR;
    }

    if(s32Lvl < INT_LVL_MAX)
    {
        ;
    }

    return s32Result;
}
int mdrv_int_disable(int ulLev)
{
    return BSP_INT_Disable(ulLev);
}

/*****************************************************************************
 * 函 数 名  : BSP_INT_Connect
 *
 * 功能描述  : 注册某个中断
 *
 * 输入参数  : VOIDFUNCPTR * vector 中断向量号，取值范围0～40
 *  		 VOIDFUNCPTR routine  中断服务程序
 *           BSP_S32 parameter      中断服务程序参数
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_Connect  (VOIDFUNCPTR * vector,VOIDFUNCPTR routine, BSP_S32 parameter)
{
    BSP_S32 s32Result = OK;
    char *acTmp = g_subvic_irq_name;
    int iTmp;

    if(IVEC_TO_INUM(vector) >= (INT_LVL_MAX))
    {
        return ERROR;
    }

    if(IVEC_TO_INUM(vector) < INT_LVL_MAX)
    {
        iTmp = IVEC_TO_INUM(vector);
        acTmp[9] = (iTmp%10) + '0';
        acTmp[8] = ((iTmp/10)%10) + '0';
        acTmp[7] = (iTmp/100) + '0';

        s32Result = request_irq((unsigned int)vector, (irq_handler_t)routine, 0, acTmp, NULL);
    }

    return s32Result;
}
int mdrv_int_connect(int level, VOIDFUNCPTR routine, unsigned int parameter)
{
    return BSP_INT_Connect((VOIDFUNCPTR*)level, routine, parameter);
}

/*****************************************************************************
 * 函 数 名  : BSP_INT_DisConnect
 *
 * 功能描述  : 取消注册某个中断
 *
 * 输入参数  : VOIDFUNCPTR * vector 中断向量号，取值范围0～40
 *  		 VOIDFUNCPTR routine  中断服务程序
 *           BSP_S32 parameter      中断服务程序参数
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_DisConnect  (VOIDFUNCPTR * vector,VOIDFUNCPTR routine, BSP_S32 parameter)
{
    BSP_S32 s32Result = OK;

    if(IVEC_TO_INUM(vector) >= (INT_LVL_MAX))
    {
        return ERROR;
    }

    if(IVEC_TO_INUM(vector) < INT_LVL_MAX)
    {
        free_irq((unsigned int)vector, NULL);
        s32Result = OK;
    }

    return s32Result;
}

/*****************************************************************************
 * 函 数 名  : BSP_INT_Lock
 *
 * 功能描述  : 屏蔽所有中断
 *
 * 输入参数  : 无
 *
 * 输出参数  : 无
 *
 * 返 回 值  : 中断锁号
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_Lock (BSP_VOID)
{
    unsigned long flag;

    local_irq_save(flag);

    return flag;
}

/*****************************************************************************
 * 函 数 名  : BSP_INT_UnLock
 *
 * 功能描述  : 打开中断
 *
 * 输入参数  : BSP_S32 ulLocKKey  中断锁号，前一次BSP_INT_Lock的返回值
 *
 * 输出参数  : 无
 *
 * 返 回 值  : 中断锁号
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_VOID BSP_INT_UnLock ( BSP_S32 s32LocKKey)
{
    local_irq_restore((unsigned long)s32LocKKey);
}

arch_initcall(BSP_DRV_SubVicInit);

