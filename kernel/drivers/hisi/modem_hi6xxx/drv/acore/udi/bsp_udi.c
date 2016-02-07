/*
#include <vxWorks.h>
#include <logLib.h>
#include <usrLib.h>
#include <string.h>
#include <stdio.h>
#include "BSP_UDI.h"
#include "BSP_UDI_DRV.h"
*/

#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>

#include <BSP.h>
#include "bsp_udi_drv.h"

#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */

#define UDI_MAX_MAIN_DEV_NUM UDI_DEV_MAX
#define UDI_MAX_DEV_TYPE_NUM 32
#define UDI_MAX_OPEN_NODE_NUM 64
//#define UDI_MAX_INIT_FUNC_NUM UDI_TYPE_MAX
#define UDI_OPEN_NODE_HEADER  0x5A0000

/* Mutex 行为封装 */
#if 0
#define UDI_MUTEX_T SEM_ID
#define UDI_MTX_CREATE(mtx) \
do {\
    /* 使用可重入Mutex */\
    mtx = semMCreate(SEM_Q_FIFO | SEM_DELETE_SAFE);\
}while((0))
#define UDI_MTX_DELETE(mtx) {semDelete(mtx); mtx = NULL;}
#define UDI_MTX_ENTER(mtx) semTake (mtx, WAIT_FOREVER)
#define UDI_MTX_LEAVE(mtx) semGive (mtx)
#endif

#define UDI_DEBUG

/**************************************************************************
  类型定义
**************************************************************************/
/* 设备打开节点 */
typedef struct tagUDI_OPEN_NODE
{
	BSP_BOOL bOpen; 
	VOID* pPrivate;
	UDI_DRV_INTEFACE_TABLE *pstDrvTable;
}UDI_OPEN_NODE;

/* 设备实例属性 */
typedef struct tagUDI_DEV_INSTANCE
{
	//BSP_U32 u32MagicNum;				/* 检查用 */
	//BSP_U32 u32DevId;					/* 设备ID */
	BSP_U32 u32Capability;					/* 设备特性 */
	//BSP_U8* strDevName;					/* 设备名,用于适配系统标准设备 */
	UDI_DRV_INTEFACE_TABLE *pDrvInterface;  /* 接口回调列表 */
	BSP_VOID* pPrivate;					/* 每个驱动私有全局 */
}UDI_DEV_INSTANCE;


/**************************************************************************
  全局变量
**************************************************************************/
UDI_OPEN_NODE			g_openNodeTable[UDI_MAX_OPEN_NODE_NUM] = {{0}};/*lint661,modify by z00212992,2012.4.21*/
BSP_U32					g_openNodeCurPos = 0;
UDI_DEV_INSTANCE			g_deviceTable[UDI_MAX_MAIN_DEV_NUM][UDI_MAX_DEV_TYPE_NUM];
extern UDI_ADP_INIT_CB_T	g_udiInitFuncTable[UDI_DEV_MAX+1];
///UDI_MUTEX_T g_udiMtxOpen;
struct semaphore			g_udiMtxOpen;

/**************************************************************************
  宏实现
**************************************************************************/
#define UDI_IDX_TO_HANDLE(idx) ((UDI_HANDLE)(UDI_OPEN_NODE_HEADER | (idx)))
#define UDI_HANDLE_TO_IDX(hdl) ((BSP_U32)((hdl) & 0xFFFF))

#define UDI_IS_INVALID_TABLE(pstDrvTable) ((NULL == (pstDrvTable)) || ((void *)(-1) == (void *)(pstDrvTable)))/*lint24/35,modify by z00212992,2012.4.17*/
/*lint661.modify by z00212992,2012.4.20*/
#define UDI_PARSE_DEV_ID(devId, mainId, devType) \
do{\
	mainId = UDI_GET_MAIN_DEV_ID(devId);\
	devType = UDI_GET_DEV_TYPE(devId);\
	\
	BSP_ASSERT(mainId < UDI_MAX_MAIN_DEV_NUM);\
	BSP_ASSERT(devType < UDI_MAX_DEV_TYPE_NUM);\
}while(0)

#define UDI_CALL_OPT_FUNC_SAFELY(handle, param1, param2, functionCB, ret) \
do{\
	BSP_U32 u32Idx;\
	UDI_DRV_INTEFACE_TABLE *pstDrvTable;\
	VOID* pPrivate;\
	\
	UDI_CHECK_HANDLE(handle);\
	u32Idx = UDI_HANDLE_TO_IDX(handle);\
	\
	if (u32Idx >= UDI_MAX_OPEN_NODE_NUM)\
	{\
		printk(KERN_ERR "BSP_MODU_UDI invalid_handle:0x%x, line:%d\n", \
			handle, __LINE__);\
		return ERROR;\
	}\
	/* 取出函数指针及参数 */\
	pstDrvTable = g_openNodeTable[u32Idx].pstDrvTable;\
	if (UDI_IS_INVALID_TABLE(pstDrvTable))\
    {\
        printk(KERN_ERR "pstDrvTable is Invalid, line:%d\n", \
			__LINE__);\
        return ERROR;\
    }\
	if (NULL == (void*)(pstDrvTable->functionCB))\
	{\
		printk(KERN_ERR "BSP_MODU_UDI %s callback is NULL, line:%d\n", \
			#functionCB, __LINE__);\
		return ERROR;\
	}\
	pPrivate = g_openNodeTable[u32Idx].pPrivate;\
	\
	/* 调用用户的回调函数 */\
	ret = pstDrvTable->functionCB(pPrivate, param1, param2);\
}while(0)

#ifdef UDI_DEBUG
#define UDI_CHECK_HANDLE(handle) \
do{\
	if ((handle & 0xFF0000) != UDI_OPEN_NODE_HEADER || \
	UDI_HANDLE_TO_IDX(handle) >= UDI_MAX_OPEN_NODE_NUM)\
	{\
		printk(KERN_ERR "BSP_MODU_UDI invalid_handle:0x%x, line:%d\n", \
			handle, __LINE__);\
		return ERROR;\
	}\
}while(0)
#else
#define UDI_CHECK_HANDLE(handle)
#endif

/**************************************************************************
  内部函数
**************************************************************************/
static UDI_HANDLE udiGetOutOpenNode(VOID)
{
	BSP_U32 u32Cnt;
	UDI_HANDLE handle = UDI_INVALID_HANDLE;

	down(&g_udiMtxOpen);
	/* 优先从当前位置找 */
	for (u32Cnt = g_openNodeCurPos; u32Cnt < UDI_MAX_OPEN_NODE_NUM; u32Cnt++)
	{
		if (FALSE == g_openNodeTable[u32Cnt].bOpen)
		{
			handle = UDI_IDX_TO_HANDLE(u32Cnt);
			break;  
		}
	}

	/* 否则, 再从头找 */
	if(UDI_INVALID_HANDLE == handle)
	{
		for (u32Cnt = 0; u32Cnt < g_openNodeCurPos; u32Cnt++)
		{
			if (FALSE == g_openNodeTable[u32Cnt].bOpen)
			{
				handle = UDI_IDX_TO_HANDLE(u32Cnt);
				break;  
			}
		}
	}

	/* 找到一个可用的handle */
	if (UDI_INVALID_HANDLE != handle)
	{
		g_openNodeCurPos = (u32Cnt+1) % UDI_MAX_OPEN_NODE_NUM;
		g_openNodeTable[u32Cnt].bOpen = TRUE;
	}
	up(&g_udiMtxOpen);

	return handle;
}

static BSP_S32 udiReturnOpenNode(BSP_U32 u32Idx)
{
	BSP_ASSERT(u32Idx < UDI_MAX_OPEN_NODE_NUM);/*lint661.modify by z00212992,2012.4.20*/

	down(&g_udiMtxOpen);
	g_openNodeTable[u32Idx].bOpen = FALSE;
	g_openNodeTable[u32Idx].pstDrvTable = (UDI_DRV_INTEFACE_TABLE*)(-1);
	up(&g_udiMtxOpen);

	return OK;
}

/**************************************************************************
  接口实现
**************************************************************************/

/*****************************************************************************
* 函 数 名  : BSP_UDI_SetPrivate
*
* 功能描述  : 设置驱动内部私有数据
*
* 输入参数  : devId: 设备ID
*             pPrivate: 私有数据
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_UDI_SetPrivate(UDI_DEVICE_ID_E devId, VOID* pPrivate)
{
	BSP_U32 u32MainId;
	BSP_U32 u32DevType;

	UDI_PARSE_DEV_ID(devId, u32MainId, u32DevType);
	g_deviceTable[u32MainId][u32DevType].pPrivate = pPrivate;

	return OK;
}

/*****************************************************************************
* 函 数 名  : BSP_UDI_SetCapability
*
* 功能描述  : 设置设备特性值
*
* 输入参数  : devId: 设备ID
*             u32Capability: 设备特性值
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_UDI_SetCapability(UDI_DEVICE_ID_E devId, BSP_U32 u32Capability)
{
	BSP_U32 u32MainId;
	BSP_U32 u32DevType;

	UDI_PARSE_DEV_ID(devId, u32MainId, u32DevType);
	g_deviceTable[u32MainId][u32DevType].u32Capability = u32Capability;

	//printk("BSP_UDI_SetCapability **********************  u32MainId=%u u32DevType=%u\n", u32MainId, u32DevType);


	return OK;
}

/*****************************************************************************
* 函 数 名  : BSP_UDI_SetInterfaceTable
*
* 功能描述  : 设置设备回调函数列表(由适配层调用)
*
* 输入参数  : devId: 设备ID
*             pDrvInterface: 驱动层的回调函数列表
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_UDI_SetInterfaceTable(UDI_DEVICE_ID_E devId, UDI_DRV_INTEFACE_TABLE *pDrvInterface)
{
	BSP_U32 u32MainId;
	BSP_U32 u32DevType;

	UDI_PARSE_DEV_ID(devId, u32MainId, u32DevType);
	g_deviceTable[u32MainId][u32DevType].pDrvInterface = pDrvInterface;

	//printk("BSP_UDI_SetInterfaceTable  **********************  u32MainId=%u u32DevType=%u\n", u32MainId, u32DevType);

	return OK;
}

/*****************************************************************************
* 函 数 名  : BSP_UDI_Init
*
* 功能描述  : UDI 模块初始化
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_UDI_Init(void)
{
	UDI_ADP_INIT_CB_T initCB;
	BSP_U32 u32Cnt;

	memset(g_deviceTable, 0, sizeof(g_deviceTable));
	memset(g_openNodeTable, 0, sizeof(g_openNodeTable));

	sema_init(&g_udiMtxOpen, SEM_FULL);

	/* 调用初始化函数 */
	for (u32Cnt = 0; u32Cnt < (BSP_U32)UDI_DEV_MAX; u32Cnt++)
	{
		initCB = g_udiInitFuncTable[u32Cnt];
		if (initCB)
		{
			if (initCB() != OK)
			{
				printk(KERN_ERR "BSP_MODU_UDI usr initCB fail, line:%d\n", __LINE__);
				return ERROR;
			}
		}
	}
	return OK;
}

/*****************************************************************************
* 函 数 名  : udi_get_capability
*
* 功能描述  : 根据设备ID获取当前设备支持的特性
*
* 输入参数  : devId: 设备ID
* 输出参数  : 无
* 返 回 值  : 支持的特性值
*****************************************************************************/
BSP_S32 udi_get_capability(UDI_DEVICE_ID_E devId)
{
	BSP_U32 u32MainId;
	BSP_U32 u32DevType;

	UDI_PARSE_DEV_ID(devId, u32MainId, u32DevType);
	return (BSP_S32)g_deviceTable[u32MainId][u32DevType].u32Capability;
}

/*****************************************************************************
* 函 数 名  : BSP_UDI_GetPrivate
*
* 功能描述  : 得到设备 驱动内部私有数据
*
* 输入参数  : handle: 设备的handle
* 输出参数  : 无
* 返 回 值  : 设备内部私有数据
*****************************************************************************/
BSP_S32 BSP_UDI_GetPrivate(UDI_HANDLE handle)
{
	BSP_U32 u32Idx = 0;

	if (UDI_INVALID_HANDLE == handle)
	{
		printk(KERN_ERR "BSP_MODU_UDI can't find open node, line:%d\n", __LINE__);
		return (BSP_S32)NULL;
	}

	u32Idx = UDI_HANDLE_TO_IDX(handle);

	return (BSP_S32)g_openNodeTable[u32Idx].pPrivate;
}

/*****************************************************************************
* 函 数 名  : udi_open
*
* 功能描述  : 打开设备(数据通道)
*
* 输入参数  : pParam: 设备的打开配置参数
* 输出参数  : 无
* 返 回 值  : -1:失败 / 其他:成功
*****************************************************************************/
UDI_HANDLE udi_open(UDI_OPEN_PARAM_S *pParam)
{
	BSP_U32 u32MainId = 0;
	BSP_U32 u32DevType = 0;
	BSP_U32 u32Idx = 0;
	UDI_HANDLE handle;
	UDI_DRV_INTEFACE_TABLE *pstDrvTable;

	if (NULL == pParam)
	{
		goto UDI_OPEN_ERR;
	}

	UDI_PARSE_DEV_ID(pParam->devid, u32MainId, u32DevType);

	/* 查找一个可用的节点 */
	handle = udiGetOutOpenNode();
	if (UDI_INVALID_HANDLE == handle)
	{
		printk(KERN_ERR "BSP_MODU_UDI can't find open node, line:%d\n", __LINE__);
		goto UDI_OPEN_ERR;
	}
	u32Idx = UDI_HANDLE_TO_IDX(handle);

	/* 调用用户回调函数 */
	pstDrvTable = g_deviceTable[u32MainId][u32DevType].pDrvInterface;
	if (NULL == pstDrvTable || NULL == pstDrvTable->udi_open_cb)
	{
		printk(KERN_ERR "BSP_MODU_UDI usr open cb is NULL, line:%d\n", __LINE__);
		goto UDI_OPEN_ERR_RET_NODE;
	}
	if (pstDrvTable->udi_open_cb(pParam, handle))
	{
		printk(KERN_ERR "BSP_MODU_UDI usr open fail, line:%d\n", __LINE__);
		goto UDI_OPEN_ERR_RET_NODE;
	}

	/* 保存驱动私有数据 */
	g_openNodeTable[u32Idx].pstDrvTable = 
	g_deviceTable[u32MainId][u32DevType].pDrvInterface;
	g_openNodeTable[u32Idx].pPrivate = 
	g_deviceTable[u32MainId][u32DevType].pPrivate;

	return handle;

UDI_OPEN_ERR_RET_NODE:
	(BSP_VOID)udiReturnOpenNode(u32Idx);
UDI_OPEN_ERR:
	return UDI_INVALID_HANDLE;
}

/*****************************************************************************
* 函 数 名  : udi_close
*
* 功能描述  : 关闭设备(数据通道)
*
* 输入参数  : handle: 设备的handle
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
BSP_S32 udi_close(UDI_HANDLE handle)
{
	BSP_S32 s32Ret = -1;
	BSP_U32 u32Idx;
	UDI_DRV_INTEFACE_TABLE *pstDrvTable;

	UDI_CHECK_HANDLE(handle);

	u32Idx = UDI_HANDLE_TO_IDX(handle);

	if (u32Idx >= UDI_MAX_OPEN_NODE_NUM)
	{
		printk(KERN_ERR "BSP_MODU_UDI invalid handle:0x%x, line:%d\n", handle, __LINE__);
		return ERROR;
	}
	/* 调用用户的回调函数 */
	pstDrvTable = g_openNodeTable[u32Idx].pstDrvTable;
	if (UDI_IS_INVALID_TABLE(pstDrvTable))/*lint58,modify by z00212992,2012.4.17*/
	{
	    printk(KERN_ERR "pstDrvTable is Invalid, line:%d\n", __LINE__);
	    return ERROR;
	}   	/*lint525,modify by z00212992,2012.4.18*/
	if (NULL == pstDrvTable->udi_close_cb)
	{
		printk(KERN_ERR "BSP_MODU_UDI udi_close_cb is NULL, line:%d\n", __LINE__);
		return ERROR;
	}
	s32Ret = pstDrvTable->udi_close_cb(g_openNodeTable[u32Idx].pPrivate);

	/* 释放 Open Node */
	(BSP_VOID)udiReturnOpenNode(u32Idx);

	return s32Ret;
}

/*****************************************************************************
* 函 数 名  : udi_write
*
* 功能描述  : 数据写
*
* 输入参数  : handle:  设备的handle
*             pMemObj: buffer内存 或 内存链表对象
*             u32Size: 数据写尺寸 或 内存链表对象可不设置
* 输出参数  : 
*
* 返 回 值  : 完成字节数 或 成功/失败
*****************************************************************************/
BSP_S32 udi_write(UDI_HANDLE handle, void* pMemObj, BSP_U32 u32Size)
{
	BSP_S32 s32Ret = -1;

	UDI_CALL_OPT_FUNC_SAFELY(handle, pMemObj, u32Size, udi_write_cb, s32Ret);
	return s32Ret;
}

/*****************************************************************************
* 函 数 名  : udi_read
*
* 功能描述  : 数据读
*
* 输入参数  : handle:  设备的handle
*             pMemObj: buffer内存 或 内存链表对象
*             u32Size: 数据读尺寸 或 内存链表对象可不设置
* 输出参数  : 
*
* 返 回 值  : 完成字节数 或 成功/失败
*****************************************************************************/
BSP_S32 udi_read(UDI_HANDLE handle, void* pMemObj, BSP_U32 u32Size)
{
	BSP_S32 s32Ret = -1;

	UDI_CALL_OPT_FUNC_SAFELY(handle, pMemObj, u32Size, udi_read_cb, s32Ret);
	return s32Ret;
}

/*****************************************************************************
* 函 数 名  : udi_ioctl
*
* 功能描述  : 数据通道属性配置
*
* 输入参数  : handle: 设备的handle
*             u32Cmd: IOCTL命令码
*             pParam: 操作参数
* 输出参数  : 
*
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 udi_ioctl(UDI_HANDLE handle, BSP_U32 u32Cmd, VOID* pParam)
{
	BSP_S32 s32Ret = -1;

	UDI_CALL_OPT_FUNC_SAFELY(handle, u32Cmd, pParam, udi_ioctl_cb, s32Ret);
	return s32Ret;
}

#if 0
/**************************************************************************
  调试信息实现
**************************************************************************/
BSP_S32 BSP_UDI_DumpOpenNode(BSP_BOOL bDumpAll)
{
	BSP_U32 cnt;

	printk("+- BSP UDI OpenNode Info Dump:\n");
	printk("|-- g_openNodeCurPos:%d\n", g_openNodeCurPos);
	for (cnt = 0; cnt < UDI_MAX_OPEN_NODE_NUM; cnt++)
	{
		if (bDumpAll || g_openNodeTable[cnt].bOpen)
		{
			/*
			printk("|-+-- OpenNode idx:           %d:\n", cnt);
			printk("  |-- OpenNode private:       0x%x\n", g_openNodeTable[cnt].pPrivate);
			printk("  |-- OpenNode close cb:      0x%x\n", g_openNodeTable[cnt].pstDrvTable->udi_close_cb);
			printk("  |-- OpenNode open  cb:      0x%x\n", g_openNodeTable[cnt].pstDrvTable->udi_open_cb);
			printk("  |-- OpenNode write cb:      0x%x\n", g_openNodeTable[cnt].pstDrvTable->udi_write_cb);
			printk("  |-- OpenNode read  cb:      0x%x\n", g_openNodeTable[cnt].pstDrvTable->udi_read_cb);
			printk("  |-- OpenNode ioctl cb:      0x%x\n", g_openNodeTable[cnt].pstDrvTable->udi_ioctl_cb);
			*/
		}
	}
	return 0;
}

#define BSP_UDI_PRINT_DEVID_BEG(devid) {switch(devid){
#define BSP_UDI_PRINT_DEVID_ITEM(name) case(name):return (#name);
#define BSP_UDI_PRINT_DEVID_END() default:return ("unknown"); }}

static BSP_U8* udiGetDevID(UDI_DEVICE_ID devid)
{
	BSP_UDI_PRINT_DEVID_BEG(devid);

	BSP_UDI_PRINT_DEVID_ITEM(UDI_ACM_CTRL_ID);
	BSP_UDI_PRINT_DEVID_ITEM(UDI_ACM_AT_ID);
	BSP_UDI_PRINT_DEVID_ITEM(UDI_ACM_SHELL_ID);
#if defined(CONFIG_ARCH_HI3635) || defined(CONFIG_ARCH_HI3635_FPGA)
	BSP_UDI_PRINT_DEVID_ITEM(UDI_ACM_3G_DIAG_ID);
#else
	BSP_UDI_PRINT_DEVID_ITEM(UDI_ACM_OM_ID);
#endif
	BSP_UDI_PRINT_DEVID_ITEM(UDI_ACM_MODEM_ID);
	BSP_UDI_PRINT_DEVID_ITEM(UDI_NCM_NDIS_ID);
	BSP_UDI_PRINT_DEVID_ITEM(UDI_NCM_CTRL_ID);

	BSP_UDI_PRINT_DEVID_END();
}

static BSP_U8* udiGetDevMainID(UDI_DEVICE_MAIN_ID mainId)
{
	BSP_UDI_PRINT_DEVID_BEG(mainId);

	BSP_UDI_PRINT_DEVID_ITEM(UDI_DEV_USB_ACM);
	BSP_UDI_PRINT_DEVID_ITEM(UDI_DEV_USB_NCM);
	BSP_UDI_PRINT_DEVID_ITEM(UDI_DEV_ICC);

	BSP_UDI_PRINT_DEVID_END();
}

static BSP_U8* udiGetDevTypeID(BSP_U32 mainId, BSP_U32 type)
{
	switch(mainId)
	{
		case UDI_DEV_USB_ACM:
			BSP_UDI_PRINT_DEVID_BEG(type);
			BSP_UDI_PRINT_DEVID_ITEM(UDI_ACM_CTRL_ID);
			BSP_UDI_PRINT_DEVID_ITEM(UDI_USB_ACM_AT);
			BSP_UDI_PRINT_DEVID_ITEM(UDI_USB_ACM_SHELL);
			BSP_UDI_PRINT_DEVID_ITEM(UDI_USB_ACM_LTE_DIAG);
			BSP_UDI_PRINT_DEVID_ITEM(UDI_USB_ACM_3G_DIAG);
			BSP_UDI_PRINT_DEVID_ITEM(UDI_USB_ACM_MODEM);
			BSP_UDI_PRINT_DEVID_END();
			break;
		case UDI_DEV_USB_NCM:
			BSP_UDI_PRINT_DEVID_BEG(type);
			BSP_UDI_PRINT_DEVID_ITEM(UDI_USB_NCM_NDIS);
			BSP_UDI_PRINT_DEVID_ITEM(UDI_USB_NCM_CTRL);
			BSP_UDI_PRINT_DEVID_END();
			break;
		default:
			return "unknown";
	}
}

BSP_S32 BSP_UDI_DumpDev(BSP_BOOL bAll)
{
	BSP_U32 mainId, typeId;

	printk("+- BSP UDI OpenNode Info Dump:\n");
	for (mainId = 0; mainId < UDI_MAX_MAIN_DEV_NUM; mainId++)
	{
		for (typeId = 0; typeId < UDI_MAX_DEV_TYPE_NUM; typeId++)
		{
			if (bAll || NULL != g_deviceTable[mainId][typeId].pDrvInterface)
			{
				/*
				printk("|-Dev Name:%s\n", udiGetDevID(UDI_BUILD_DEV_ID(mainId, typeId)));
				printk("|-+-- mainId:%s, typeId:%s\n", udiGetDevMainID(mainId), udiGetDevTypeID(mainId, typeId));
				printk("  |-- dev private:       0x%x\n", g_deviceTable[mainId][typeId].pPrivate);
				printk("  |-- dev close cb:      0x%x\n", g_deviceTable[mainId][typeId].pDrvInterface->udi_close_cb);
				printk("  |-- dev open  cb:      0x%x\n", g_deviceTable[mainId][typeId].pDrvInterface->udi_open_cb);
				printk("  |-- dev write cb:      0x%x\n", g_deviceTable[mainId][typeId].pDrvInterface->udi_write_cb);
				printk("  |-- dev read  cb:      0x%x\n", g_deviceTable[mainId][typeId].pDrvInterface->udi_read_cb);
				printk("  |-- dev ioctl cb:      0x%x\n", g_deviceTable[mainId][typeId].pDrvInterface->udi_ioctl_cb);
				printk("  |  \n");
				*/
			}
		}
	}
	return 0;
}
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */



