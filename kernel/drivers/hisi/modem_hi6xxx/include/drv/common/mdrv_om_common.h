/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  om.h
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件命名为"mdrv.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/
#ifndef _MDRV_OM_COMMON_H_
#define _MDRV_OM_COMMON_H_

#ifdef _cplusplus
extern "C"
{
#endif

/* field id定义只允许添加，不允许删除，枚举定义删除之后，id值需要保留，新添加id需要跳过删除id */
/* 添加AP新的field，放到OM_AP_FIELD_END之前，添加CP新的field，放到OM_CP_FIELD_END之前 */
enum OM_FIELD_ID_ENUM
{
    OM_AP_FIELD_BEGIN   = (0x01200000),
    OM_AP_OSA           = (OM_AP_FIELD_BEGIN),
    OM_AP_FIELD_END,

    OM_CP_FIELD_BEGIN   = (0x02200000),
    OM_CP_OSA           = (OM_CP_FIELD_BEGIN),
    OM_CP_MSP_SLEEP     = (0x02200001),
    OM_CP_TLPS          = (0x02200002),
    OM_CP_FTM_MNTN      = (0x02200003),
    OM_CP_FIELD_END,
};

typedef int dump_handle;
typedef void (*dump_hook)(void);

/*****************************************************************************
 * 函 数 名  : mdrv_om_system_error
 * 功能描述  : 软件异常记录接口，完成异常文件记录之后主动复位单板。
 * 输入参数  : modId:   复位原因
 *             arg1:    用户自定义参数1
 *             arg2:    用户自定义参数2
 *             arg3:    用户自定义参数3，
 *                       buf指针，dump程序会自动拷贝到dump内存，
 *                       在dump文件中的field以用户数据显示。
 *             arg3Length:  arg所指buf的长度最大长度max_len(v7为4k，v8为1k)
 *                       超过max_len只拷贝起始max_len数据
 * 输出参数  : 无。
 * 返 回 值  : 无。
 *****************************************************************************/
void mdrv_om_system_error(int modId, int arg1, int arg2, char * arg3, int arg3Length);

/*****************************************************************************
 * 函 数 名  : mdrv_om_register_field
 * 功能描述  : 记录异常复位信息的地址申请函数，该接口申请出来的内存中存入的数据，
 *             在system_error的时候会保存到dump文件对应的field区域中。
 * 输入参数  : field_id :   申请buffer对应的field id
 *             field_name:  field名称，用于在解析工具中显示。
 *             virt_addr:   自带地址注册的虚拟地址，如果为空，表示从dump分配内存。
 *             phy_addr:    自带地址注册的物理地址
 *             length:      申请buffer的长度
 *             version:     申请buffer的版本号
 *
 * 输出参数  : NULL:  申请buf失败
 *
 * 返 回 值  : 无。
 *****************************************************************************/
unsigned char* mdrv_om_register_field(unsigned int field_id, char * field_name, void * virt_addr,
                            void * phy_addr, unsigned int length, unsigned short version);

/*****************************************************************************
 * 函 数 名  : mdrv_om_get_field_addr
 * 功能描述  : 获取field_id对应的dump内存的虚拟地址，可跨核使用
 *
 * 输入参数  : field_id: 通过mdrv_om_register_field接口向dump申请内存的ID
 *
 * 输出参数  : 无
 *
 * 返 回 值  : >0   成功，返回field虚拟地址
 *             0    失败
 *
 *****************************************************************************/
unsigned char * mdrv_om_get_field_addr(unsigned int field_id);

/*****************************************************************************
 * 函 数 名  : mdrv_om_get_field_phy_addr
 * 功能描述  : 获取field_id对应的dump内存的物理地址，可跨核使用
 *
 * 输入参数  : field_id: 通过mdrv_om_register_field接口向dump申请内存的ID
 *
 * 输出参数  : 无
 *
 * 返 回 值  : >0   成功，返回field物理地址
 *             0    失败
 *
 *****************************************************************************/
unsigned char * mdrv_om_get_field_phy_addr(unsigned int field_id);

/*****************************************************************************
 * 函 数 名  : mdrv_om_register_callback
 * 功能描述  : DUMP处理钩子注册函数，dump模块提供注册接口，其他模块注册处理函数，
 * system_error的时候会依次调用执行注册进来的处理函数。
 *
 * 输入参数  : name :   异常注册的模块名称
 *             pFunc:   系统异常时的回调函数
 *
 * 输出参数  : 无
 *
 * 返 回 值     : >0  成功，返回注册结点句柄
 *        -1, DUMP_HANDLE_INVALID  失败
 *
 *****************************************************************************/
dump_handle mdrv_om_register_callback(char * name, dump_hook pFunc);

/*****************************************************************************
  函 数 名  : mdrv_om_unregister_callback
  功能描述  : 删除DUMP钩子函数。

  输入参数  : handle :   mdrv_om_register_callback注册获取到的句柄

  输出参数  : 无

  返 回 值     : 0  成功
  -1   失败

 *****************************************************************************/
 int mdrv_om_unregister_callback(dump_handle handle);


/*****************************************************************************
  函 数 名  : mdrv_om_set_hsoflag
  功能描述  : DIAG通知底软HSO连接状态接口。

  输入参数  : flag :  1表示HSO连接，0表示HSO断开

  输出参数  : 无

  返 回 值     : 无

 *****************************************************************************/
void mdrv_om_set_hsoflag(unsigned int flag);

/*****************************************************************************
 * 函 数 名  : mdrv_om_set_sysview_swt
 * 功能描述  : HSO配置sysview开关。
 *
 * 输入参数  : set_type :  上报类型
 *             set_swt: 开关，1表示开，0 表示关
 *             period:  上报周期，单位是秒
 * 输出参数  : 无
 *
 * 返 回 值  :  0  成功
 *        -1   失败
 *****************************************************************************/
unsigned int mdrv_om_set_sysview_swt(unsigned int set_type,unsigned int set_swt,unsigned int period);

/*****************************************************************************
 * 函 数 名  : mdrv_om_get_alltaskinfo
 * 功能描述  : 获取当前核中所有任务的任务ID和任务名称。
 *
 * 输入参数  : p_task_stru:  内存指针，由使用者申请
 *             param_len:  内存长度
 *
 * 输出参数  : 无
 *
 * 返 回 值     :  0    成功
 *                 -1   失败
 *
 *****************************************************************************/
unsigned int mdrv_om_get_alltaskinfo(void *p_task_stru,unsigned int param_len);

 /**************************************************************************
  * 函 数 名  : mdrv_om_get_rootfs
  * 功能描述  : 查询根目录的分区情况
  * 输入参数  : data：用于存放根目录字符串的缓冲区的首地址
  *             len: 缓冲区data的长度
  * 输出参数  :
  *             返 回 值  : 0:  操作成功；
  *                        -1： 操作失败。
  ****************************************************************************/
int mdrv_om_get_rootfs(char *data, unsigned int len);

#ifdef _cplusplus
}
#endif
#endif


