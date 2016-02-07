

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "mdrv_udi.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 函数声明
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
* 函 数 名  : mdrv_udi_open
*
* 功能描述  : 打开设备(数据通道)
*
* 输入参数  : pParam: 设备的打开配置参数
* 输出参数  : 无
* 返 回 值  : -1:失败 / 其他:成功
*****************************************************************************/
extern UDI_HANDLE udi_open(UDI_OPEN_PARAM_S *pParam);
UDI_HANDLE mdrv_udi_open(UDI_OPEN_PARAM_S *pParam)
{
    return udi_open(pParam);
}


/*****************************************************************************
* 函 数 名  : mdrv_udi_close
*
* 功能描述  : 关闭设备(数据通道)
*
* 输入参数  : handle: 设备的handle
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
extern int udi_close(UDI_HANDLE handle);
int mdrv_udi_close(UDI_HANDLE handle)
{
    return udi_close(handle);
}

/*****************************************************************************
* 函 数 名  : mdrv_udi_write
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
extern int udi_write(UDI_HANDLE handle, void* pMemObj, unsigned int u32Size);
int mdrv_udi_write(UDI_HANDLE handle, void* pMemObj, unsigned int u32Size)
{
    return udi_write(handle, pMemObj, u32Size);
}


/*****************************************************************************
* 函 数 名  : mdrv_udi_read
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
extern int udi_read(UDI_HANDLE handle, void* pMemObj, unsigned int u32Size);
int mdrv_udi_read(UDI_HANDLE handle, void* pMemObj, unsigned int u32Size)
{
    return udi_read( handle,  pMemObj,  u32Size);
}


/*****************************************************************************
* 函 数 名  : mdrv_udi_ioctl
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
extern int udi_ioctl(UDI_HANDLE handle, unsigned int u32Cmd, void* pParam);
int mdrv_udi_ioctl(UDI_HANDLE handle, unsigned int u32Cmd, void* pParam)
{
    return udi_ioctl( handle,  u32Cmd, pParam);
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

