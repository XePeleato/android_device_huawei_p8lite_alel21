

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_MEDIA_NOC__INTERFACE_H__
#define __SOC_MEDIA_NOC__INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/2) SERVICE
 ****************************************************************************/
/* 寄存器说明：IP 模块标识寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_COREID_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_COREID_ADDR(base) ((base) + (0x0000))

/* 寄存器说明：IP 版本标识寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_REVISIONID_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_REVISIONID_ADDR(base) ((base) + (0x0004))

/* 寄存器说明：总线错误中断上报使能寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_FAULTEN_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_FAULTEN_ADDR(base) ((base) + (0x0008))

/* 寄存器说明：总线错误状态指示寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRVLD_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRVLD_ADDR(base) ((base) + (0x000C))

/* 寄存器说明：总线错误中断清除寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRCLR_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRCLR_ADDR(base) ((base) + (0x0010))

/* 寄存器说明：总线错误信息寄存器0
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG0_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG0_ADDR(base) ((base) + (0x0014))

/* 寄存器说明：总线错误信息寄存器1
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG1_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG1_ADDR(base) ((base) + (0x0018))

/* 寄存器说明：总线错误信息寄存器3
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG3_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG3_ADDR(base) ((base) + (0x0020))

/* 寄存器说明：总线错误信息寄存器5
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG5_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG5_ADDR(base) ((base) + (0x0028))

/* 寄存器说明：总线错误信息寄存器7
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG7_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG7_ADDR(base) ((base) + (0x0030))

/* 寄存器说明：ADE0 QoS Generator 的域ID
   位域定义UNION结构:  SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_COREID_UNION */
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_COREID_ADDR(base) ((base) + (0x0100))

/* 寄存器说明：ADE0 QoS Generator 总线代码版本寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_REVISIONID_UNION */
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_REVISIONID_ADDR(base) ((base) + (0x0104))

/* 寄存器说明：ADE0 QoS Generator 优先级寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ADE0_QOSGENERATOR_PRIORITY_UNION */
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_PRIORITY_ADDR(base) ((base) + (0x0108))

/* 寄存器说明：ADE0 QoS Generator 模式寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ADE0_QOSGENERATOR_MODE_UNION */
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_MODE_ADDR(base) ((base) + (0x010C))

/* 寄存器说明：ADE0 QoS Generator 带宽阈值寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ADE0_QOSGENERATOR_BANDWIDTH_UNION */
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_BANDWIDTH_ADDR(base) ((base) + (0x0110))

/* 寄存器说明：ADE0 QoS Generator 饱和量寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ADE0_QOSGENERATOR_SATURATION_UNION */
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_SATURATION_ADDR(base) ((base) + (0x0114))

/* 寄存器说明：ADE0 QoS Generator 外部控制寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ADE0_QOSGENERATOR_EXTCONTROL_UNION */
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_EXTCONTROL_ADDR(base) ((base) + (0x0118))

/* 寄存器说明：ADE1 QoS Generator 的域ID
   位域定义UNION结构:  SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_COREID_UNION */
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_COREID_ADDR(base) ((base) + (0x0200))

/* 寄存器说明：ADE1 QoS Generator总线代码版本寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_REVISIONID_UNION */
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_REVISIONID_ADDR(base) ((base) + (0x0204))

/* 寄存器说明：ADE1 QoS Generator优先级寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ADE1_QOSGENERATOR_PRIORITY_UNION */
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_PRIORITY_ADDR(base) ((base) + (0x0208))

/* 寄存器说明：ADE1 QoS Generator 模式寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ADE1_QOSGENERATOR_MODE_UNION */
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_MODE_ADDR(base) ((base) + (0x020C))

/* 寄存器说明：ADE1 QoS Generator 带宽阈值寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ADE1_QOSGENERATOR_BANDWIDTH_UNION */
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_BANDWIDTH_ADDR(base) ((base) + (0x0210))

/* 寄存器说明：ADE1 QoS Generator 饱和量寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ADE1_QOSGENERATOR_SATURATION_UNION */
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_SATURATION_ADDR(base) ((base) + (0x0214))

/* 寄存器说明：ADE1 QoS Generator外部控制寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ADE1_QOSGENERATOR_EXTCONTROL_UNION */
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_EXTCONTROL_ADDR(base) ((base) + (0x0218))

/* 寄存器说明：SMMU adapter域ID
   位域定义UNION结构:  SOC_MEDIA_NOC_SMMU_ADAPTER_ID_COREID_UNION */
#define SOC_MEDIA_NOC_SMMU_ADAPTER_ID_COREID_ADDR(base) ((base) + (0x1000))

/* 寄存器说明：总线代码版本寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_SMMU_ADAPTER_ID_REVISIONID_UNION */
#define SOC_MEDIA_NOC_SMMU_ADAPTER_ID_REVISIONID_ADDR(base) ((base) + (0x1004))

/* 寄存器说明：SMMU Rate Adapter配置寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_SMMU_ADAPTER_RATE_UNION */
#define SOC_MEDIA_NOC_SMMU_ADAPTER_RATE_ADDR(base)   ((base) + (0x1008))

/* 寄存器说明：bypass SMMU Rate Adapter
   位域定义UNION结构:  SOC_MEDIA_NOC_SMMU_ADAPTER_BYPASS_UNION */
#define SOC_MEDIA_NOC_SMMU_ADAPTER_BYPASS_ADDR(base) ((base) + (0x100C))

/* 寄存器说明：MEDIA_BUS Packet probe域ID
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_COREID_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_COREID_ADDR(base) ((base) + (0x2000))

/* 寄存器说明：总线代码版本寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_REVISIONID_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_REVISIONID_ADDR(base) ((base) + (0x2004))

/* 寄存器说明：Packet Probe全局控制寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_ADDR(base) ((base) + (0x2008))

/* 寄存器说明：Packet Probe使能寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_CFGCTL_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_CFGCTL_ADDR(base) ((base) + (0x200C))

/* 寄存器说明：监控端口选择寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEPORTSEL_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEPORTSEL_ADDR(base) ((base) + (0x2010))

/* 寄存器说明：过滤条件查找表
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_FILTERLUT_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_FILTERLUT_ADDR(base) ((base) + (0x2014))

/* 寄存器说明：trace 使能寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMEN_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMEN_ADDR(base) ((base) + (0x2018))

/* 寄存器说明：trace 状态寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMSTATUS_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMSTATUS_ADDR(base) ((base) + (0x201C))

/* 寄存器说明：trace 清除寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMCLR_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMCLR_ADDR(base) ((base) + (0x2020))

/* 寄存器说明：统计周期配置寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATPERIOD_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATPERIOD_ADDR(base) ((base) + (0x2024))

/* 寄存器说明：手动带宽统计计时开始使能
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATGO_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATGO_ADDR(base) ((base) + (0x2028))

/* 寄存器说明：统计计数最小告警阈值
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMMIN_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMMIN_ADDR(base) ((base) + (0x202C))

/* 寄存器说明：统计计数器最大告警阈值
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMMAX_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMMAX_ADDR(base) ((base) + (0x2030))

/* 寄存器说明：告警状态寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMSTATUS_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMSTATUS_ADDR(base) ((base) + (0x2034))

/* 寄存器说明：告警状态清除寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMCLR_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMCLR_ADDR(base) ((base) + (0x2038))

/* 寄存器说明：告警状态使能寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMEN_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMEN_ADDR(base) ((base) + (0x203C))

/* 寄存器说明：F0的RouteId过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ROUTEIDBASE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ROUTEIDBASE_ADDR(base) ((base) + (0x2044))

/* 寄存器说明：F0的RouteId过滤条件Mask配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ROUTEIDMASK_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ROUTEIDMASK_ADDR(base) ((base) + (0x2048))

/* 寄存器说明：F0的地址过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ADDRBASE_LOW_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ADDRBASE_LOW_ADDR(base) ((base) + (0x204C))

/* 寄存器说明：F0的地址过滤的Mask地址大小配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_WINDOWSIZE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_WINDOWSIZE_ADDR(base) ((base) + (0x1054))

/* 寄存器说明：F0的安全过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_SECURITYBASE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_SECURITYBASE_ADDR(base) ((base) + (0x2058))

/* 寄存器说明：F0的安全过滤条件的Mask
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_SECURITYMASK_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_SECURITYMASK_ADDR(base) ((base) + (0x205C))

/* 寄存器说明：F0的Opcode过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_OPCODE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_OPCODE_ADDR(base) ((base) + (0x2060))

/* 寄存器说明：F0的packet状态过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_STATUS_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_STATUS_ADDR(base) ((base) + (0x2064))

/* 寄存器说明：F0的包长过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_LENGTH_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_LENGTH_ADDR(base) ((base) + (0x2068))

/* 寄存器说明：F0的urgency过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_URGENCY_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_URGENCY_ADDR(base) ((base) + (0x206C))

/* 寄存器说明：F0的User过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_USERBASE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_USERBASE_ADDR(base) ((base) + (0x2070))

/* 寄存器说明：F0的User过滤条件Mask
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_USERMASK_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_USERMASK_ADDR(base) ((base) + (0x2074))

/* 寄存器说明：F1的RouteId过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ROUTEIDBASE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ROUTEIDBASE_ADDR(base) ((base) + (0x2080))

/* 寄存器说明：F1的RouteId过滤条件Mask配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ROUTEIDMASK_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ROUTEIDMASK_ADDR(base) ((base) + (0x2084))

/* 寄存器说明：F1的地址过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ADDRBASE_LOW_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ADDRBASE_LOW_ADDR(base) ((base) + (0x2088))

/* 寄存器说明：F1的地址过滤的Mask地址大小配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_WINDOWSIZE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_WINDOWSIZE_ADDR(base) ((base) + (0x2090))

/* 寄存器说明：F1的安全过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_SECURITYBASE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_SECURITYBASE_ADDR(base) ((base) + (0x2094))

/* 寄存器说明：F1的安全过滤条件的Mask
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_SECURITYMASK_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_SECURITYMASK_ADDR(base) ((base) + (0x2098))

/* 寄存器说明：F1的Opcode过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_OPCODE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_OPCODE_ADDR(base) ((base) + (0x209C))

/* 寄存器说明：F1的packet状态过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_STATUS_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_STATUS_ADDR(base) ((base) + (0x20A0))

/* 寄存器说明：F1的包长过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_LENGTH_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_LENGTH_ADDR(base) ((base) + (0x20A4))

/* 寄存器说明：F1的urgency过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_URGENCY_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_URGENCY_ADDR(base) ((base) + (0x20A8))

/* 寄存器说明：F1的User过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_USERBASE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_USERBASE_ADDR(base) ((base) + (0x20AC))

/* 寄存器说明：F1的User过滤条件Mask
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_USERMASK_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_USERMASK_ADDR(base) ((base) + (0x20B0))

/* 寄存器说明：F2的RouteId过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ROUTEIDBASE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ROUTEIDBASE_ADDR(base) ((base) + (0x20BC))

/* 寄存器说明：F2的RouteId过滤条件Mask配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ROUTEIDMASK_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ROUTEIDMASK_ADDR(base) ((base) + (0x20C0))

/* 寄存器说明：F2的地址过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ADDRBASE_LOW_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ADDRBASE_LOW_ADDR(base) ((base) + (0x20C4))

/* 寄存器说明：F2的地址过滤的Mask地址大小配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_WINDOWSIZE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_WINDOWSIZE_ADDR(base) ((base) + (0x20CC))

/* 寄存器说明：F2的安全过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_SECURITYBASE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_SECURITYBASE_ADDR(base) ((base) + (0x20D0))

/* 寄存器说明：F2的安全过滤条件的Mask
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_SECURITYMASK_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_SECURITYMASK_ADDR(base) ((base) + (0x20D4))

/* 寄存器说明：F2的Opcode过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_OPCODE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_OPCODE_ADDR(base) ((base) + (0x20D8))

/* 寄存器说明：F2的packet状态过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_STATUS_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_STATUS_ADDR(base) ((base) + (0x20DC))

/* 寄存器说明：F2的包长过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_LENGTH_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_LENGTH_ADDR(base) ((base) + (0x20E0))

/* 寄存器说明：F2的urgency过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_URGENCY_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_URGENCY_ADDR(base) ((base) + (0x20E4))

/* 寄存器说明：F2的User过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_USERBASE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_USERBASE_ADDR(base) ((base) + (0x20E8))

/* 寄存器说明：F2的User过滤条件Mask
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_USERMASK_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_USERMASK_ADDR(base) ((base) + (0x20EC))

/* 寄存器说明：F3的RouteId过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ROUTEIDBASE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ROUTEIDBASE_ADDR(base) ((base) + (0x20F8))

/* 寄存器说明：F3的RouteId过滤条件Mask配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ROUTEIDMASK_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ROUTEIDMASK_ADDR(base) ((base) + (0x20FC))

/* 寄存器说明：F3的地址过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ADDRBASE_LOW_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ADDRBASE_LOW_ADDR(base) ((base) + (0x2100))

/* 寄存器说明：F3的地址过滤的Mask地址大小配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_WINDOWSIZE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_WINDOWSIZE_ADDR(base) ((base) + (0x2108))

/* 寄存器说明：F3的安全过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_SECURITYBASE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_SECURITYBASE_ADDR(base) ((base) + (0x210C))

/* 寄存器说明：F3的安全过滤条件的Mask
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_SECURITYMASK_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_SECURITYMASK_ADDR(base) ((base) + (0x2110))

/* 寄存器说明：F3的Opcode过滤条件配置
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_OPCODE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_OPCODE_ADDR(base) ((base) + (0x2114))

/* 寄存器说明：F3的packet状态过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_STATUS_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_STATUS_ADDR(base) ((base) + (0x2118))

/* 寄存器说明：F3的包长过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_LENGTH_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_LENGTH_ADDR(base) ((base) + (0x211C))

/* 寄存器说明：F3的urgency过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_URGENCY_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_URGENCY_ADDR(base) ((base) + (0x2120))

/* 寄存器说明：F3的User过滤条件
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_USERBASE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_USERBASE_ADDR(base) ((base) + (0x2124))

/* 寄存器说明：F3的User过滤条件Mask
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_USERMASK_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_USERMASK_ADDR(base) ((base) + (0x2128))

/* 寄存器说明：Counter的统计类型控制寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_SRC_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_SRC_ADDR(base) ((base) + (0x2138))

/* 寄存器说明：告警中断产生配置寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_ALARMMODE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_ALARMMODE_ADDR(base) ((base) + (0x213C))

/* 寄存器说明：统计计数器的统计值
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_VAL_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_VAL_ADDR(base) ((base) + (0x2140))

/* 寄存器说明：Counter的统计类型控制寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_SRC_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_SRC_ADDR(base) ((base) + (0x214C))

/* 寄存器说明：告警中断产生配置寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_ALARMMODE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_ALARMMODE_ADDR(base) ((base) + (0x2150))

/* 寄存器说明：统计计数器的统计值
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_VAL_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_VAL_ADDR(base) ((base) + (0x2154))

/* 寄存器说明：Counter的统计类型控制寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_SRC_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_SRC_ADDR(base) ((base) + (0x2160))

/* 寄存器说明：告警中断产生配置寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_ALARMMODE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_ALARMMODE_ADDR(base) ((base) + (0x2164))

/* 寄存器说明：统计计数器的统计值
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_VAL_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_VAL_ADDR(base) ((base) + (0x2168))

/* 寄存器说明：Counter的统计类型控制寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_SRC_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_SRC_ADDR(base) ((base) + (0x2174))

/* 寄存器说明：告警中断产生配置寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_ALARMMODE_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_ALARMMODE_ADDR(base) ((base) + (0x2178))

/* 寄存器说明：统计计数器的统计值
   位域定义UNION结构:  SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_VAL_UNION */
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_VAL_ADDR(base) ((base) + (0x217C))



/****************************************************************************
                     (2/2) SERVICE_VIDEO
 ****************************************************************************/
/* 寄存器说明：ISP QoS Generator 的域ID
   位域定义UNION结构:  SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_COREID_UNION */
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_COREID_ADDR(base) ((base) + (0x0100))

/* 寄存器说明：ISP QoS Generator总线代码版本寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_REVISIONID_UNION */
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_REVISIONID_ADDR(base) ((base) + (0x0104))

/* 寄存器说明：ISP QoS Generator优先级寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ISP_QOSGENERATOR_PRIORITY_UNION */
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_PRIORITY_ADDR(base) ((base) + (0x0108))

/* 寄存器说明：ISP QoS Generator 模式寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ISP_QOSGENERATOR_MODE_UNION */
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_MODE_ADDR(base) ((base) + (0x010C))

/* 寄存器说明：ISP QoS Generator 带宽阈值寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ISP_QOSGENERATOR_BANDWIDTH_UNION */
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_BANDWIDTH_ADDR(base) ((base) + (0x0110))

/* 寄存器说明：ISP QoS Generator 饱和量寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ISP_QOSGENERATOR_SATURATION_UNION */
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_SATURATION_ADDR(base) ((base) + (0x0114))

/* 寄存器说明：ISP QoS Generator外部控制寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_ISP_QOSGENERATOR_EXTCONTROL_UNION */
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_EXTCONTROL_ADDR(base) ((base) + (0x0118))

/* 寄存器说明：JPU QoS Generator 的域ID
   位域定义UNION结构:  SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_COREID_UNION */
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_COREID_ADDR(base) ((base) + (0x0200))

/* 寄存器说明：JPU QoS Generator总线代码版本寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_REVISIONID_UNION */
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_REVISIONID_ADDR(base) ((base) + (0x0204))

/* 寄存器说明：JPU QoS Generator优先级寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_JPU_QOSGENERATOR_PRIORITY_UNION */
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_PRIORITY_ADDR(base) ((base) + (0x0208))

/* 寄存器说明：JPU QoS Generator 模式寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_JPU_QOSGENERATOR_MODE_UNION */
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_MODE_ADDR(base) ((base) + (0x020C))

/* 寄存器说明：JPU QoS Generator 带宽阈值寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_JPU_QOSGENERATOR_BANDWIDTH_UNION */
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_BANDWIDTH_ADDR(base) ((base) + (0x0210))

/* 寄存器说明：JPU QoS Generator 饱和量寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_JPU_QOSGENERATOR_SATURATION_UNION */
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_SATURATION_ADDR(base) ((base) + (0x0214))

/* 寄存器说明：JPU QoS Generator外部控制寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_JPU_QOSGENERATOR_EXTCONTROL_UNION */
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_EXTCONTROL_ADDR(base) ((base) + (0x0218))

/* 寄存器说明：VPU QoS Generator 的域ID
   位域定义UNION结构:  SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_COREID_UNION */
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_COREID_ADDR(base) ((base) + (0x0300))

/* 寄存器说明：VPU QoS Generator总线代码版本寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_REVISIONID_UNION */
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_REVISIONID_ADDR(base) ((base) + (0x0304))

/* 寄存器说明：VPU QoS Generator优先级寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_VPU_QOSGENERATOR_PRIORITY_UNION */
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_PRIORITY_ADDR(base) ((base) + (0x0308))

/* 寄存器说明：VPU QoS Generator 模式寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_VPU_QOSGENERATOR_MODE_UNION */
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_MODE_ADDR(base) ((base) + (0x030C))

/* 寄存器说明：VPU QoS Generator 带宽阈值寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_VPU_QOSGENERATOR_BANDWIDTH_UNION */
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_BANDWIDTH_ADDR(base) ((base) + (0x0310))

/* 寄存器说明：VPU QoS Generator 饱和量寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_VPU_QOSGENERATOR_SATURATION_UNION */
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_SATURATION_ADDR(base) ((base) + (0x0314))

/* 寄存器说明：VPU QoS Generator外部控制寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_VPU_QOSGENERATOR_EXTCONTROL_UNION */
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_EXTCONTROL_ADDR(base) ((base) + (0x0318))

/* 寄存器说明：VPU_SEC QoS Generator 的域ID
   位域定义UNION结构:  SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_COREID_UNION */
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_COREID_ADDR(base) ((base) + (0x0400))

/* 寄存器说明：VPU_SEC QoS Generator总线代码版本寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_REVISIONID_UNION */
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_REVISIONID_ADDR(base) ((base) + (0x0404))

/* 寄存器说明：VPU_SEC QoS Generator优先级寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_PRIORITY_UNION */
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_PRIORITY_ADDR(base) ((base) + (0x0408))

/* 寄存器说明：VPU_SEC QoS Generator 模式寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_MODE_UNION */
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_MODE_ADDR(base) ((base) + (0x040C))

/* 寄存器说明：VPU_SEC QoS Generator 带宽阈值寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_BANDWIDTH_UNION */
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_BANDWIDTH_ADDR(base) ((base) + (0x0410))

/* 寄存器说明：VPU_SEC QoS Generator 饱和量寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_SATURATION_UNION */
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_SATURATION_ADDR(base) ((base) + (0x0414))

/* 寄存器说明：VPU_SEC QoS Generator外部控制寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_EXTCONTROL_UNION */
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_EXTCONTROL_ADDR(base) ((base) + (0x0418))

/* 寄存器说明：VIDEO adapter域ID
   位域定义UNION结构:  SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_COREID_UNION */
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_COREID_ADDR(base) ((base) + (0x1000))

/* 寄存器说明：总线代码版本寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_REVISIONID_UNION */
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_REVISIONID_ADDR(base) ((base) + (0x1004))

/* 寄存器说明：VIDEO Rate Adapter配置寄存器
   位域定义UNION结构:  SOC_MEDIA_NOC_VIDEO_ADAPTER_RATE_UNION */
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_RATE_ADDR(base)  ((base) + (0x1008))

/* 寄存器说明：bypass VIDEO Rate Adapter
   位域定义UNION结构:  SOC_MEDIA_NOC_VIDEO_ADAPTER_BYPASS_UNION */
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_BYPASS_ADDR(base) ((base) + (0x100C))





/*****************************************************************************
  3 枚举定义
*****************************************************************************/



/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/



/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/



/*****************************************************************************
  7 UNION定义
*****************************************************************************/

/****************************************************************************
                     (1/2) SERVICE
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_COREID_UNION
 结构说明  : MEDIA_BUS_ERR_LOGGER_ID_COREID 寄存器结构定义。地址偏移量:0x0000，初值:0x74B07D0D，宽度:32
 寄存器说明: IP 模块标识寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core_type_id   : 8;  /* bit[0-7] : MEDIA_BUS 的Error Logger的域ID */
        unsigned int  core_check_sum : 24; /* bit[8-31]: MEDIA_BUS Error Logger 的参数检验和 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_COREID_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_COREID_core_type_id_START    (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_COREID_core_type_id_END      (7)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_COREID_core_check_sum_START  (8)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_COREID_core_check_sum_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_REVISIONID_UNION
 结构说明  : MEDIA_BUS_ERR_LOGGER_ID_REVISIONID 寄存器结构定义。地址偏移量:0x0004，初值:0x012D5300，宽度:32
 寄存器说明: IP 版本标识寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  user_id    : 8;  /* bit[0-7] : 用户自定义值，不在 IP 内使用 */
        unsigned int  flexnoc_id : 24; /* bit[8-31]: 总线代码版本寄存器 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_REVISIONID_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_REVISIONID_user_id_START     (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_REVISIONID_user_id_END       (7)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_REVISIONID_flexnoc_id_START  (8)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ID_REVISIONID_flexnoc_id_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_FAULTEN_UNION
 结构说明  : MEDIA_BUS_ERR_LOGGER_FAULTEN 寄存器结构定义。地址偏移量:0x0008，初值:0x00000000，宽度:32
 寄存器说明: 总线错误中断上报使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  fault_en : 1;  /* bit[0]   : 监控到总线读写错误（寄存器ErrVld为1）时的中断上报使能。
                                                   0：禁止；
                                                   1：使能。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_FAULTEN_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_FAULTEN_fault_en_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_FAULTEN_fault_en_END    (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRVLD_UNION
 结构说明  : MEDIA_BUS_ERR_LOGGER_ERRVLD 寄存器结构定义。地址偏移量:0x000C，初值:0x00000000，宽度:32
 寄存器说明: 总线错误状态指示寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  err_vld  : 1;  /* bit[0]   : 总线错误状态指示。
                                                   0：无效；
                                                   1：有效错误。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRVLD_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRVLD_err_vld_START   (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRVLD_err_vld_END     (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRCLR_UNION
 结构说明  : MEDIA_BUS_ERR_LOGGER_ERRCLR 寄存器结构定义。地址偏移量:0x0010，初值:0x00000000，宽度:32
 寄存器说明: 总线错误中断清除寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  err_clr  : 1;  /* bit[0]   : 1.写1'b1清零ErrVld寄存器（如果中断使能，同时也清除中断）；
                                                   2.写1'b1并不能存储在ErrClr寄存器中，读该寄存器，返回总是1'b0；
                                                   3.写1'b0无效。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRCLR_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRCLR_err_clr_START   (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRCLR_err_clr_END     (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG0_UNION
 结构说明  : MEDIA_BUS_ERR_LOGGER_ERRLOG0 寄存器结构定义。地址偏移量:0x0014，初值:0x80000000，宽度:32
 寄存器说明: 总线错误信息寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  lock     : 1;  /* bit[0]    : 指示Transaction是否为Lock操作，高有效； */
        unsigned int  opc      : 4;  /* bit[1-4]  : Transaction操作类型：
                                                    0&#45;->RD：INCR的读操作；
                                                    1&#45;->RDW：WRAP的读操作；
                                                    2&#45;->RDL：Exclusive 读操作；
                                                    3&#45;->RDX：Lock read；
                                                    4&#45;->WR：INCR写操作；
                                                    5&#45;->WRW：WRAP写操作；
                                                    6&#45;->WRC：Exclusive写操作；
                                                    8&#45;->PRE：FIXED BURST；
                                                    9&#45;->URG：urgency packet，Error Probe不会检测到此类packet；
                                                    其它&#45;->Reserveed：非法操作； */
        unsigned int  reserved_0: 3;  /* bit[5-7]  : 保留 */
        unsigned int  err_code : 3;  /* bit[8-10] : 错误类型：
                                                    0&#45;->被访问slave返回Error Response；
                                                    1&#45;->master访问了总线的Reserve地址空间；
                                                    2&#45;->master发送了slave不支持的Burst类型（当前总线不会出现这种场景）；
                                                    3&#45;->master访问了掉电区域；
                                                    4&#45;->访问权限报错；
                                                    5&#45;->master访问时，收到Firewall的Hide Error Response；
                                                    6&#45;->被访问slave TimeOut，并返回Error Response；
                                                    7&#45;->none。 */
        unsigned int  reserved_1: 5;  /* bit[11-15]: 保留 */
        unsigned int  len1     : 8;  /* bit[16-23]: 总线错误操作的包长，操作的数据长度为2len1 bytes。 */
        unsigned int  reserved_2: 7;  /* bit[24-30]: 保留 */
        unsigned int  format   : 1;  /* bit[31]   : 总线错误操作的包格式版本。
                                                    0：NTTP-3；
                                                    1：NTTP-3.5。 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG0_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG0_lock_START      (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG0_lock_END        (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG0_opc_START       (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG0_opc_END         (4)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG0_err_code_START  (8)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG0_err_code_END    (10)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG0_len1_START      (16)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG0_len1_END        (23)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG0_format_START    (31)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG0_format_END      (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG1_UNION
 结构说明  : MEDIA_BUS_ERR_LOGGER_ERRLOG1 寄存器结构定义。地址偏移量:0x0018，初值:0x00000000，宽度:32
 寄存器说明: 总线错误信息寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  errlog1  : 17; /* bit[0-16] : RouteId：路由信息，各bit含义如下
                                                    [16:14]&#45;->Initiator Flow ID，用于表示发起访问的master；
                                                    0: ADE0, 1: ADE1, 2: CFG, 3: ISP, 4: JPU, 5: VPU, 6: VPU_SEC, 7: 保留。
                                                    
                                                    [13:10]&#45;->Target Flow ID，用于表示被访问的slave；
                                                    0: APB_ISP, 1: APB_JPU, 2: APB_VPU, 3: AHB_ISP, 4: APB_ADE, 5: APB_G3D, 6: APB_MEDIA_SC, 7: APB_SMMU，8: AXI_SMMU, 9: XG2RAM_HARQ, 10: MEDIA_BUS_SERVICE, 11: MEDIA_BUS_SERVICE_VIDEO, Others: 保留。
                                                    
                                                    [9:7]&#45;->Target SubRange，表示Target NIU的不同区间地址段；
                                                    [6:0]&#45;->Seq ID，Transaction标记； */
        unsigned int  reserved : 15; /* bit[17-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG1_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG1_errlog1_START   (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG1_errlog1_END     (16)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG3_UNION
 结构说明  : MEDIA_BUS_ERR_LOGGER_ERRLOG3 寄存器结构定义。地址偏移量:0x0020，初值:0x00000000，宽度:32
 寄存器说明: 总线错误信息寄存器3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  errlog3 : 32; /* bit[0-31]: 总线错误操作的地址 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG3_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG3_errlog3_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG3_errlog3_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG5_UNION
 结构说明  : MEDIA_BUS_ERR_LOGGER_ERRLOG5 寄存器结构定义。地址偏移量:0x0028，初值:0x00000000，宽度:32
 寄存器说明: 总线错误信息寄存器5
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  errlog5  : 3;  /* bit[0-2] : 总线错误操作的User bit信息 */
        unsigned int  reserved : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG5_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG5_errlog5_START   (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG5_errlog5_END     (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG7_UNION
 结构说明  : MEDIA_BUS_ERR_LOGGER_ERRLOG7 寄存器结构定义。地址偏移量:0x0030，初值:0x00000000，宽度:32
 寄存器说明: 总线错误信息寄存器7
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  errlog7  : 1;  /* bit[0]   : 总线错误操作的Security bit信息 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG7_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG7_errlog7_START   (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_ERRLOG7_errlog7_END     (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_COREID_UNION
 结构说明  : ADE0_QOSGENERATOR_ID_COREID 寄存器结构定义。地址偏移量:0x0100，初值:0xE6BE3104，宽度:32
 寄存器说明: ADE0 QoS Generator 的域ID
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core_type_id   : 8;  /* bit[0-7] : ADE0 QoS Generator 的域ID */
        unsigned int  core_check_sum : 24; /* bit[8-31]: ADE0 QoS Generator 的参数检验和 */
    } reg;
} SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_COREID_UNION;
#endif
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_COREID_core_type_id_START    (0)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_COREID_core_type_id_END      (7)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_COREID_core_check_sum_START  (8)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_COREID_core_check_sum_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_REVISIONID_UNION
 结构说明  : ADE0_QOSGENERATOR_ID_REVISIONID 寄存器结构定义。地址偏移量:0x0104，初值:0x012D5300，宽度:32
 寄存器说明: ADE0 QoS Generator 总线代码版本寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  user_id    : 8;  /* bit[0-7] : 用户自定义值，不在 IP 内使用 */
        unsigned int  flexnoc_id : 24; /* bit[8-31]: 总线代码版本寄存器 */
    } reg;
} SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_REVISIONID_UNION;
#endif
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_REVISIONID_user_id_START     (0)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_REVISIONID_user_id_END       (7)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_REVISIONID_flexnoc_id_START  (8)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_ID_REVISIONID_flexnoc_id_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ADE0_QOSGENERATOR_PRIORITY_UNION
 结构说明  : ADE0_QOSGENERATOR_PRIORITY 寄存器结构定义。地址偏移量:0x0108，初值:0x80000000，宽度:32
 寄存器说明: ADE0 QoS Generator 优先级寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0       : 3;  /* bit[0-2]  : 在 Regulator 模式，定义了低 hurry 值；在 Fixed/limiter 模式，定义了写 urgency 值 */
        unsigned int  reserved_0: 5;  /* bit[3-7]  : 保留 */
        unsigned int  p1       : 3;  /* bit[8-10] : 在 Regulator 模式，定义了高 hurry 值；在 Fixed/limiter 模式，定义了读 urgency 值 */
        unsigned int  reserved_1: 20; /* bit[11-30]: 保留 */
        unsigned int  mark     : 1;  /* bit[31-31]: 向后兼容标志（为0时） */
    } reg;
} SOC_MEDIA_NOC_ADE0_QOSGENERATOR_PRIORITY_UNION;
#endif
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_PRIORITY_p0_START        (0)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_PRIORITY_p0_END          (2)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_PRIORITY_p1_START        (8)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_PRIORITY_p1_END          (10)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_PRIORITY_mark_START      (31)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_PRIORITY_mark_END        (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ADE0_QOSGENERATOR_MODE_UNION
 结构说明  : ADE0_QOSGENERATOR_MODE 寄存器结构定义。地址偏移量:0x010C，初值:0x00000000，宽度:32
 寄存器说明: ADE0 QoS Generator 模式寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mode     : 2;  /* bit[0-1] : 模式选择：0—Fixed；1—Limiter；2—Bypass；3—Regulator */
        unsigned int  reserved : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_ADE0_QOSGENERATOR_MODE_UNION;
#endif
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_MODE_mode_START      (0)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_MODE_mode_END        (1)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ADE0_QOSGENERATOR_BANDWIDTH_UNION
 结构说明  : ADE0_QOSGENERATOR_BANDWIDTH 寄存器结构定义。地址偏移量:0x0110，初值:0x0000058E，宽度:32
 寄存器说明: ADE0 QoS Generator 带宽阈值寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bandwidth : 13; /* bit[0-12] : 带宽阈值 */
        unsigned int  reserved  : 19; /* bit[13-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_ADE0_QOSGENERATOR_BANDWIDTH_UNION;
#endif
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_BANDWIDTH_bandwidth_START  (0)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_BANDWIDTH_bandwidth_END    (12)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ADE0_QOSGENERATOR_SATURATION_UNION
 结构说明  : ADE0_QOSGENERATOR_SATURATION 寄存器结构定义。地址偏移量:0x0114，初值:0x00000100，宽度:32
 寄存器说明: ADE0 QoS Generator 饱和量寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  saturation : 10; /* bit[0-9]  : 带宽计数器大小 */
        unsigned int  reserved   : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_ADE0_QOSGENERATOR_SATURATION_UNION;
#endif
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_SATURATION_saturation_START  (0)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_SATURATION_saturation_END    (9)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ADE0_QOSGENERATOR_EXTCONTROL_UNION
 结构说明  : ADE0_QOSGENERATOR_EXTCONTROL 寄存器结构定义。地址偏移量:0x0118，初值:0x00000000，宽度:32
 寄存器说明: ADE0 QoS Generator 外部控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  socket_qos_en : 1;  /* bit[0]   : 决定采用 socket 的 QoS 值还是 QoS Generator 的 QoS 值 */
        unsigned int  ext_thr_en    : 1;  /* bit[1]   : 外部阈值代替带宽寄存器设置的阈值控制高低优先级 */
        unsigned int  int_clk_en    : 1;  /* bit[2]   : 本地时钟（NIU时钟）替代参考时钟进行带宽计算 */
        unsigned int  reserved      : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_ADE0_QOSGENERATOR_EXTCONTROL_UNION;
#endif
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_EXTCONTROL_socket_qos_en_START  (0)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_EXTCONTROL_socket_qos_en_END    (0)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_EXTCONTROL_ext_thr_en_START     (1)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_EXTCONTROL_ext_thr_en_END       (1)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_EXTCONTROL_int_clk_en_START     (2)
#define SOC_MEDIA_NOC_ADE0_QOSGENERATOR_EXTCONTROL_int_clk_en_END       (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_COREID_UNION
 结构说明  : ADE1_QOSGENERATOR_ID_COREID 寄存器结构定义。地址偏移量:0x0200，初值:0x7B96DE04，宽度:32
 寄存器说明: ADE1 QoS Generator 的域ID
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core_type_id   : 8;  /* bit[0-7] : ADE1 QoS Generator 的域ID */
        unsigned int  core_check_sum : 24; /* bit[8-31]: ADE1 QoS Generator 的参数检验和 */
    } reg;
} SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_COREID_UNION;
#endif
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_COREID_core_type_id_START    (0)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_COREID_core_type_id_END      (7)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_COREID_core_check_sum_START  (8)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_COREID_core_check_sum_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_REVISIONID_UNION
 结构说明  : ADE1_QOSGENERATOR_ID_REVISIONID 寄存器结构定义。地址偏移量:0x0204，初值:0x012D5300，宽度:32
 寄存器说明: ADE1 QoS Generator总线代码版本寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  user_id    : 8;  /* bit[0-7] : 用户自定义值，不在 IP 内使用 */
        unsigned int  flexnoc_id : 24; /* bit[8-31]: 总线代码版本寄存器 */
    } reg;
} SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_REVISIONID_UNION;
#endif
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_REVISIONID_user_id_START     (0)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_REVISIONID_user_id_END       (7)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_REVISIONID_flexnoc_id_START  (8)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_ID_REVISIONID_flexnoc_id_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ADE1_QOSGENERATOR_PRIORITY_UNION
 结构说明  : ADE1_QOSGENERATOR_PRIORITY 寄存器结构定义。地址偏移量:0x0208，初值:0x80000000，宽度:32
 寄存器说明: ADE1 QoS Generator优先级寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0       : 3;  /* bit[0-2]  : 在 Regulator 模式，定义了低 hurry 值；在 Fixed/limiter 模式，定义了写 urgency 值 */
        unsigned int  reserved_0: 5;  /* bit[3-7]  : 保留 */
        unsigned int  p1       : 3;  /* bit[8-10] : 在 Regulator 模式，定义了高 hurry 值；在 Fixed/limiter 模式，定义了读 urgency 值 */
        unsigned int  reserved_1: 20; /* bit[11-30]: 保留 */
        unsigned int  mark     : 1;  /* bit[31-31]: 向后兼容标志（为0时） */
    } reg;
} SOC_MEDIA_NOC_ADE1_QOSGENERATOR_PRIORITY_UNION;
#endif
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_PRIORITY_p0_START        (0)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_PRIORITY_p0_END          (2)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_PRIORITY_p1_START        (8)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_PRIORITY_p1_END          (10)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_PRIORITY_mark_START      (31)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_PRIORITY_mark_END        (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ADE1_QOSGENERATOR_MODE_UNION
 结构说明  : ADE1_QOSGENERATOR_MODE 寄存器结构定义。地址偏移量:0x020C，初值:0x00000000，宽度:32
 寄存器说明: ADE1 QoS Generator 模式寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mode     : 2;  /* bit[0-1] : 模式选择：0—Fixed；1—Limiter；2—Bypass；3—Regulator */
        unsigned int  reserved : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_ADE1_QOSGENERATOR_MODE_UNION;
#endif
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_MODE_mode_START      (0)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_MODE_mode_END        (1)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ADE1_QOSGENERATOR_BANDWIDTH_UNION
 结构说明  : ADE1_QOSGENERATOR_BANDWIDTH 寄存器结构定义。地址偏移量:0x0210，初值:0x0000058E，宽度:32
 寄存器说明: ADE1 QoS Generator 带宽阈值寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bandwidth : 13; /* bit[0-12] : 带宽阈值 */
        unsigned int  reserved  : 19; /* bit[13-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_ADE1_QOSGENERATOR_BANDWIDTH_UNION;
#endif
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_BANDWIDTH_bandwidth_START  (0)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_BANDWIDTH_bandwidth_END    (12)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ADE1_QOSGENERATOR_SATURATION_UNION
 结构说明  : ADE1_QOSGENERATOR_SATURATION 寄存器结构定义。地址偏移量:0x0214，初值:0x00000100，宽度:32
 寄存器说明: ADE1 QoS Generator 饱和量寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  saturation : 10; /* bit[0-9]  : 带宽计数器大小 */
        unsigned int  reserved   : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_ADE1_QOSGENERATOR_SATURATION_UNION;
#endif
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_SATURATION_saturation_START  (0)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_SATURATION_saturation_END    (9)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ADE1_QOSGENERATOR_EXTCONTROL_UNION
 结构说明  : ADE1_QOSGENERATOR_EXTCONTROL 寄存器结构定义。地址偏移量:0x0218，初值:0x00000000，宽度:32
 寄存器说明: ADE1 QoS Generator外部控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  socket_qos_en : 1;  /* bit[0-0] : 决定采用 socket 的 QoS 值还是 QoS Generator 的 QoS 值 */
        unsigned int  ext_thr_en    : 1;  /* bit[1-1] : 外部阈值代替带宽寄存器设置的阈值控制高低优先级 */
        unsigned int  int_clk_en    : 1;  /* bit[2-2] : 本地时钟（NIU时钟）替代参考时钟进行带宽计算 */
        unsigned int  reserved      : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_ADE1_QOSGENERATOR_EXTCONTROL_UNION;
#endif
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_EXTCONTROL_socket_qos_en_START  (0)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_EXTCONTROL_socket_qos_en_END    (0)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_EXTCONTROL_ext_thr_en_START     (1)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_EXTCONTROL_ext_thr_en_END       (1)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_EXTCONTROL_int_clk_en_START     (2)
#define SOC_MEDIA_NOC_ADE1_QOSGENERATOR_EXTCONTROL_int_clk_en_END       (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_SMMU_ADAPTER_ID_COREID_UNION
 结构说明  : SMMU_ADAPTER_ID_COREID 寄存器结构定义。地址偏移量:0x1000，初值:0xF8859C01，宽度:32
 寄存器说明: SMMU adapter域ID
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core_type_id   : 8;  /* bit[0-7] : 访问SMMU通路的RateAdapter的域ID */
        unsigned int  core_check_sum : 24; /* bit[8-31]: 访问SMMU通路的RateAdapter的参数检验和 */
    } reg;
} SOC_MEDIA_NOC_SMMU_ADAPTER_ID_COREID_UNION;
#endif
#define SOC_MEDIA_NOC_SMMU_ADAPTER_ID_COREID_core_type_id_START    (0)
#define SOC_MEDIA_NOC_SMMU_ADAPTER_ID_COREID_core_type_id_END      (7)
#define SOC_MEDIA_NOC_SMMU_ADAPTER_ID_COREID_core_check_sum_START  (8)
#define SOC_MEDIA_NOC_SMMU_ADAPTER_ID_COREID_core_check_sum_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_SMMU_ADAPTER_ID_REVISIONID_UNION
 结构说明  : SMMU_ADAPTER_ID_REVISIONID 寄存器结构定义。地址偏移量:0x1004，初值:0x012D5300，宽度:32
 寄存器说明: 总线代码版本寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  user_id    : 8;  /* bit[0-7] : 用户自定义值，不在 IP 内使用 */
        unsigned int  flexnoc_id : 24; /* bit[8-31]: 总线代码版本寄存器 */
    } reg;
} SOC_MEDIA_NOC_SMMU_ADAPTER_ID_REVISIONID_UNION;
#endif
#define SOC_MEDIA_NOC_SMMU_ADAPTER_ID_REVISIONID_user_id_START     (0)
#define SOC_MEDIA_NOC_SMMU_ADAPTER_ID_REVISIONID_user_id_END       (7)
#define SOC_MEDIA_NOC_SMMU_ADAPTER_ID_REVISIONID_flexnoc_id_START  (8)
#define SOC_MEDIA_NOC_SMMU_ADAPTER_ID_REVISIONID_flexnoc_id_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_SMMU_ADAPTER_RATE_UNION
 结构说明  : SMMU_ADAPTER_RATE 寄存器结构定义。地址偏移量:0x1008，初值:0x00000000，宽度:32
 寄存器说明: SMMU Rate Adapter配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rate     : 10; /* bit[0-9]  : 带宽适配调解：
                                                    [7：0]：小数部分；
                                                    [9：8]：整数部分；
                                                    计算公式：Rate=1/（ratio-1），ratio=outgoing_throughput/incoming_throughput；
                                                    例如：ratio=2，那么Rate=1=0x1.00；ratio=11/3，Rate=3/8=0x0.60； */
        unsigned int  reserved : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_SMMU_ADAPTER_RATE_UNION;
#endif
#define SOC_MEDIA_NOC_SMMU_ADAPTER_RATE_rate_START      (0)
#define SOC_MEDIA_NOC_SMMU_ADAPTER_RATE_rate_END        (9)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_SMMU_ADAPTER_BYPASS_UNION
 结构说明  : SMMU_ADAPTER_BYPASS 寄存器结构定义。地址偏移量:0x100C，初值:0x00000000，宽度:32
 寄存器说明: bypass SMMU Rate Adapter
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bypass   : 1;  /* bit[0]   : bypass SMMU Rate Adapter */
        unsigned int  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_SMMU_ADAPTER_BYPASS_UNION;
#endif
#define SOC_MEDIA_NOC_SMMU_ADAPTER_BYPASS_bypass_START    (0)
#define SOC_MEDIA_NOC_SMMU_ADAPTER_BYPASS_bypass_END      (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_COREID_UNION
 结构说明  : MEDIA_BUS_PACKET_ID_COREID 寄存器结构定义。地址偏移量:0x2000，初值:0x5B100906，宽度:32
 寄存器说明: MEDIA_BUS Packet probe域ID
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  id_core_id     : 8;  /* bit[0-7] : MEDIA_BUS的Packet Probe的域ID */
        unsigned int  core_check_sum : 24; /* bit[8-31]: MEDIA_BUS的Packet Probe的参数检验和 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_COREID_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_COREID_id_core_id_START      (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_COREID_id_core_id_END        (7)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_COREID_core_check_sum_START  (8)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_COREID_core_check_sum_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_REVISIONID_UNION
 结构说明  : MEDIA_BUS_PACKET_ID_REVISIONID 寄存器结构定义。地址偏移量:0x2004，初值:0x012D5300，宽度:32
 寄存器说明: 总线代码版本寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  user_id        : 8;  /* bit[0-7] :  */
        unsigned int  id_revision_id : 24; /* bit[8-31]: 低8bit自定义，高24bit是NOC的配置HUSH值。 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_REVISIONID_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_REVISIONID_user_id_START         (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_REVISIONID_user_id_END           (7)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_REVISIONID_id_revision_id_START  (8)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_ID_REVISIONID_id_revision_id_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_UNION
 结构说明  : MEDIA_BUS_PACKET_MAINCTL 寄存器结构定义。地址偏移量:0x2008，初值:0x00000000，宽度:32
 寄存器说明: Packet Probe全局控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  err_en                        : 1;  /* bit[0]   : ErrEn：使能Packet Probe将检测到的任何错误状态的Packet上报给Observer。由于本总线的Packet Probe不支持trace功能，故此位无意义(没有定义Observer_Packet）。 */
        unsigned int  trace_en                      : 1;  /* bit[1]   : TraceEn：使能Packet Probe将检测到的Packet上报给Observer。该总线不支持Trace功能，故此位无意义。 */
        unsigned int  payload_en                    : 1;  /* bit[2]   : PayloadEn：设置为1时，上报的Packet既包含header，又包含Payload，设置为0时，上报的Packet只包含Header。该总线不支持Trace功能，故此位无意义。 */
        unsigned int  stat_en                       : 1;  /* bit[3]   : StatEn：统计模型使能寄存器，当StatEn由0配置为1时，统计计数器被清零；当StatEn配置为0时，统计计数器被disabled。 */
        unsigned int  alarm_en                      : 1;  /* bit[4]   : AlarmEn：中断使能配置。
                                                                        0&#45;->屏蔽中断，输出信号MEDIA_BUS_Packet_Probe_StatAlarm为0；
                                                                        1&#45;->使能中断输出，输出信号MEDIA_BUS_Packet_Probe_StatAlarm。 */
        unsigned int  stat_cond_dump                : 1;  /* bit[5]   : 0&#45;->表示无条件dump统计数据，寄存器counters_m_val的值，实时反映统计计数器的值；
                                                                        1&#45;->表示有条件dump数据，在给定的统计周期内发生out of bounds 事件，才会将统计计数器的值dump至寄存器counters_m_val中。 */
        unsigned int  intrusive_mode                : 1;  /* bit[6]   : IntrusiveMode：总线配置mode为Spy，故此位hard_code为0. */
        unsigned int  filt_byte_always_chainable_en : 1;  /* bit[7]   : FiltByteAlwaysChainableEn：奇数和偶数计数器串行使能的全局配置：
                                                                        0&#45;->不使能，此时，所有侧statistic counters都可以被Map；
                                                                        1&#45;->使能，只有偶数statistic counters才可以被Map。 */
        unsigned int  reserved                      : 24; /* bit[8-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_err_en_START                         (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_err_en_END                           (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_trace_en_START                       (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_trace_en_END                         (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_payload_en_START                     (2)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_payload_en_END                       (2)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_stat_en_START                        (3)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_stat_en_END                          (3)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_alarm_en_START                       (4)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_alarm_en_END                         (4)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_stat_cond_dump_START                 (5)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_stat_cond_dump_END                   (5)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_intrusive_mode_START                 (6)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_intrusive_mode_END                   (6)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_filt_byte_always_chainable_en_START  (7)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_MAINCTL_filt_byte_always_chainable_en_END    (7)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_CFGCTL_UNION
 结构说明  : MEDIA_BUS_PACKET_CFGCTL 寄存器结构定义。地址偏移量:0x200C，初值:0x00000000，宽度:32
 寄存器说明: Packet Probe使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  global_en : 1;  /* bit[0]   : GlobalEn：Packet Probe使能信号，高有效。 */
        unsigned int  active    : 1;  /* bit[1]   : Active：Packet Probe的工作状态指示信号。
                                                    0&#45;->当GlobalEn配置为0的几个cycle后，Active被拉低；
                                                    1&#45;->当GlobalEn配置为1时，Active被拉高，此时不允许对Packet Probe进行配置。 */
        unsigned int  reserved  : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_CFGCTL_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_CFGCTL_global_en_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_CFGCTL_global_en_END    (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_CFGCTL_active_START     (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_CFGCTL_active_END       (1)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEPORTSEL_UNION
 结构说明  : MEDIA_BUS_PACKET_TRACEPORTSEL 寄存器结构定义。地址偏移量:0x2010，初值:0x00000000，宽度:32
 寄存器说明: 监控端口选择寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trace_port_sel : 1;  /* bit[0]   :  */
        unsigned int  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEPORTSEL_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEPORTSEL_trace_port_sel_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEPORTSEL_trace_port_sel_END    (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_FILTERLUT_UNION
 结构说明  : MEDIA_BUS_PACKET_FILTERLUT 寄存器结构定义。地址偏移量:0x2014，初值:0x00000000，宽度:32
 寄存器说明: 过滤条件查找表
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filter_lut : 16; /* bit[0-15] : 选择过滤条件：
                                                      0x5&#45;->~F0 过滤条件为F0取反；
                                                      0x6&#45;->F1 ^ F0 过滤条件为F1和F0异或；
                                                      0x8&#45;->F1 &amp; F0 过滤条件为F1和F0相与；
                                                      0xA&#45;->F0 过滤条件为F0；
                                                      0xC&#45;->F1 过滤条件为F1；
                                                      0xE&#45;->F1 | F0 过滤条件为F1或F0。 */
        unsigned int  reserved   : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_FILTERLUT_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_FILTERLUT_filter_lut_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_FILTERLUT_filter_lut_END    (15)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMEN_UNION
 结构说明  : MEDIA_BUS_PACKET_TRACEALARMEN 寄存器结构定义。地址偏移量:0x2018，初值:0x00000000，宽度:32
 寄存器说明: trace 使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trace_alarm_en : 5;  /* bit[0-4] : 不支持 trace 功能 */
        unsigned int  reserved       : 27; /* bit[5-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMEN_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMEN_trace_alarm_en_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMEN_trace_alarm_en_END    (4)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMSTATUS_UNION
 结构说明  : MEDIA_BUS_PACKET_TRACEALARMSTATUS 寄存器结构定义。地址偏移量:0x201C，初值:0x00000000，宽度:32
 寄存器说明: trace 状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trace_alarm_status : 5;  /* bit[0-4] : 不支持 trace 功能 */
        unsigned int  reserved           : 27; /* bit[5-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMSTATUS_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMSTATUS_trace_alarm_status_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMSTATUS_trace_alarm_status_END    (4)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMCLR_UNION
 结构说明  : MEDIA_BUS_PACKET_TRACEALARMCLR 寄存器结构定义。地址偏移量:0x2020，初值:0x00000000，宽度:32
 寄存器说明: trace 清除寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trace_alarm_clr : 5;  /* bit[0-4] : 不支持 trace 功能 */
        unsigned int  reserved        : 27; /* bit[5-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMCLR_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMCLR_trace_alarm_clr_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_TRACEALARMCLR_trace_alarm_clr_END    (4)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATPERIOD_UNION
 结构说明  : MEDIA_BUS_PACKET_STATPERIOD 寄存器结构定义。地址偏移量:0x2024，初值:0x00000000，宽度:32
 寄存器说明: 统计周期配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  stat_period : 5;  /* bit[0-4] : 自动统计模式下的统计周期，统计周期= N_Cycle = 2^min(23,StatPeriod)，最大为8 mega cycles，最小为2 cycles。当配置为0时，自动统计模式被关闭，需要采用手动模式，也就是配置StatGo寄存器来实现带宽统计。 */
        unsigned int  reserved    : 27; /* bit[5-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATPERIOD_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATPERIOD_stat_period_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATPERIOD_stat_period_END    (4)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATGO_UNION
 结构说明  : MEDIA_BUS_PACKET_STATGO 寄存器结构定义。地址偏移量:0x2028，初值:0x00000000，宽度:32
 寄存器说明: 手动带宽统计计时开始使能
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  stat_go  : 1;  /* bit[0]   : 当StatPeriod=0时，当globalen为1，向寄存器StatGo写1，则会将counter_m_val的值与statalarmmax或statalarmmin的值进行比较；读该寄存器始终返回0。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATGO_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATGO_stat_go_START   (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATGO_stat_go_END     (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMMIN_UNION
 结构说明  : MEDIA_BUS_PACKET_STATALARMMIN 寄存器结构定义。地址偏移量:0x202C，初值:0x00000000，宽度:32
 寄存器说明: 统计计数最小告警阈值
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  stat_alarm_min : 32; /* bit[0-31]: 触发告警的低阈值，当任意一个统计计数器的值小于StatAlarmMin的值，就触发告警寄存器，MEDIA_BUS_PACKET_STATALARMSTATU 状态寄存器置1。 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMMIN_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMMIN_stat_alarm_min_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMMIN_stat_alarm_min_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMMAX_UNION
 结构说明  : MEDIA_BUS_PACKET_STATALARMMAX 寄存器结构定义。地址偏移量:0x2030，初值:0x00000000，宽度:32
 寄存器说明: 统计计数器最大告警阈值
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  stat_alarm_max : 32; /* bit[0-31]: 触发告警的高阈值，当任意一个统计计数器的值大于StatAlarmMax的值，就触发告警寄存器，MEDIA_BUS_PACKET_STATALARMSTATU 状态寄存器置1。 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMMAX_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMMAX_stat_alarm_max_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMMAX_stat_alarm_max_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMSTATUS_UNION
 结构说明  : MEDIA_BUS_PACKET_STATALARMSTATUS 寄存器结构定义。地址偏移量:0x2034，初值:0x00000000，宽度:32
 寄存器说明: 告警状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  stat_alarm_status : 1;  /* bit[0]   : 当任意一个统计计数器的值大于StatAlarmMax或小于StatAlarmMin，则该寄存器置位为1； */
        unsigned int  reserved          : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMSTATUS_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMSTATUS_stat_alarm_status_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMSTATUS_stat_alarm_status_END    (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMCLR_UNION
 结构说明  : MEDIA_BUS_PACKET_STATALARMCLR 寄存器结构定义。地址偏移量:0x2038，初值:0x00000000，宽度:32
 寄存器说明: 告警状态清除寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  stat_alarm_clr : 1;  /* bit[0]   : 写1会清零StatAlarmStatus寄存器，读此寄存器返回总是0； */
        unsigned int  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMCLR_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMCLR_stat_alarm_clr_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMCLR_stat_alarm_clr_END    (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMEN_UNION
 结构说明  : MEDIA_BUS_PACKET_STATALARMEN 寄存器结构定义。地址偏移量:0x203C，初值:0x00000001，宽度:32
 寄存器说明: 告警状态使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  stat_alarm_en : 1;  /* bit[0]   : 置0会屏蔽告警和中断 */
        unsigned int  reserved      : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMEN_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMEN_stat_alarm_en_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_STATALARMEN_stat_alarm_en_END    (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ROUTEIDBASE_UNION
 结构说明  : MEDIA_BUS_PACKET_F0_ROUTEIDBASE 寄存器结构定义。地址偏移量:0x2044，初值:0x00000000，宽度:32
 寄存器说明: F0的RouteId过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_0_routeid_base : 17; /* bit[0-16] : 通过配置routeId可以过滤master到slave的访问通路上的Packet：
                                                                  [16:14]&#45;->Initiator Flow ID，用于表示Master ID；
                                                                  0: ADE0, 1: ADE1, 2: CFG, 3: ISP, 4: JPU, 5: VPU, 6: VPU_SEC, 7: 保留。
                                                                  
                                                                  [13:10]&#45;->Target Flow ID，用于表示Slave ID；
                                                                  0: APB_ISP, 1: APB_JPU, 2: APB_VPU, 3: AHB_ISP, 4: APB_ADE, 5: APB_G3D, 6: APB_MEDIA_SC, 7: APB_SMMU，8: AX_SMMU, 9: XG2RAM_HARQ, 10: MEDIA_BUS_SERVICE, 11: MEDIA_BUS_SERVICE_VIDEO, Others: 保留。
                                                                  
                                                                  [9:7]&#45;->Target SubRange，表示Target NIU的不同区间地址段；
                                                                  [6:0]&#45;->SeqIds：用于表示同一条访问通路的不同transaction的ID，建议不把这7bit作为过滤条件； */
        unsigned int  reserved               : 15; /* bit[17-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ROUTEIDBASE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ROUTEIDBASE_filters_0_routeid_base_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ROUTEIDBASE_filters_0_routeid_base_END    (16)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ROUTEIDMASK_UNION
 结构说明  : MEDIA_BUS_PACKET_F0_ROUTEIDMASK 寄存器结构定义。地址偏移量:0x2048，初值:0x00000000，宽度:32
 寄存器说明: F0的RouteId过滤条件Mask配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_0_routeid_mask : 17; /* bit[0-16] : 对RouteIdBase进行Mask。
                                                                  例如：过滤条件为initiator flow + target flow，那么配置此寄存器的值为0x1FC00。 */
        unsigned int  reserved               : 15; /* bit[17-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ROUTEIDMASK_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ROUTEIDMASK_filters_0_routeid_mask_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ROUTEIDMASK_filters_0_routeid_mask_END    (16)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ADDRBASE_LOW_UNION
 结构说明  : MEDIA_BUS_PACKET_F0_ADDRBASE_LOW 寄存器结构定义。地址偏移量:0x204C，初值:0x00000000，宽度:32
 寄存器说明: F0的地址过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_0_addrbase_low : 32; /* bit[0-31]: 地址过滤条件的基地址。 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ADDRBASE_LOW_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ADDRBASE_LOW_filters_0_addrbase_low_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_ADDRBASE_LOW_filters_0_addrbase_low_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_WINDOWSIZE_UNION
 结构说明  : MEDIA_BUS_PACKET_F0_WINDOWSIZE 寄存器结构定义。地址偏移量:0x1054，初值:0x00000000，宽度:32
 寄存器说明: F0的地址过滤的Mask地址大小配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_0_window_size : 6;  /* bit[0-5] : 地址过滤的Mask窗口大小。
                                                                例如：地址过滤的Mask窗口大小为64MB，WidowSize=log2(64MB)=26，此时，就配置filter_0_windowsize=0x1A；
                                                                Address Mask=~（2^max(WindowSize,packet.len)-1）=0xFC000000（packet.len为NOC内部表示一个packet包长变量的位宽，其值为6~7）；
                                                                过滤条件为：PacketAddress &amp; AddressMask = AddressBase &amp; AddressMask。 */
        unsigned int  reserved              : 26; /* bit[6-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_WINDOWSIZE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_WINDOWSIZE_filters_0_window_size_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_WINDOWSIZE_filters_0_window_size_END    (5)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_SECURITYBASE_UNION
 结构说明  : MEDIA_BUS_PACKET_F0_SECURITYBASE 寄存器结构定义。地址偏移量:0x2058，初值:0x00000000，宽度:32
 寄存器说明: F0的安全过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_0_security_base : 1;  /* bit[0]   : 0&#45;->表示安全过滤条件；
                                                                  1&#45;->表示非安全过滤条件。 */
        unsigned int  reserved                : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_SECURITYBASE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_SECURITYBASE_filters_0_security_base_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_SECURITYBASE_filters_0_security_base_END    (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_SECURITYMASK_UNION
 结构说明  : MEDIA_BUS_PACKET_F0_SECURITYMASK 寄存器结构定义。地址偏移量:0x205C，初值:0x00000000，宽度:32
 寄存器说明: F0的安全过滤条件的Mask
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_0_security_mask : 1;  /* bit[0]   : 安全过滤条件：packet.Security &amp; SecurityMask = SecurityBase &amp; SecurityMasks；
                                                                  也就是说当Filters_0_SecurityMask=1时安全过滤条件Filters_0_SecurityBase有效，可以过滤安全或非安全packet；否则，不起过滤作用； */
        unsigned int  reserved                : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_SECURITYMASK_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_SECURITYMASK_filters_0_security_mask_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_SECURITYMASK_filters_0_security_mask_END    (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_OPCODE_UNION
 结构说明  : MEDIA_BUS_PACKET_F0_OPCODE 寄存器结构定义。地址偏移量:0x2060，初值:0x00000000，宽度:32
 寄存器说明: F0的Opcode过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rd_en    : 1;  /* bit[0]   : 0&#45;->不选择统计读操作packet；
                                                   1&#45;->选择统计读操作packet。 */
        unsigned int  wr_en    : 1;  /* bit[1]   : 0&#45;->不选择统计写操作packet；
                                                   1&#45;->选择统计写操作packet。 */
        unsigned int  lock_en  : 1;  /* bit[2]   : 0&#45;->不选择统计exclusive transaction；
                                                   1&#45;->选择统计exclusive transaction。 */
        unsigned int  urg_en   : 1;  /* bit[3]   : 0&#45;->不选择统计带有urgency的packet；
                                                   1&#45;->选择统计带有urgency的packet。 */
        unsigned int  reserved : 28; /* bit[4-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_OPCODE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_OPCODE_rd_en_START     (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_OPCODE_rd_en_END       (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_OPCODE_wr_en_START     (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_OPCODE_wr_en_END       (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_OPCODE_lock_en_START   (2)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_OPCODE_lock_en_END     (2)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_OPCODE_urg_en_START    (3)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_OPCODE_urg_en_END      (3)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_STATUS_UNION
 结构说明  : MEDIA_BUS_PACKET_F0_STATUS 寄存器结构定义。地址偏移量:0x2064，初值:0x00000000，宽度:32
 寄存器说明: F0的packet状态过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  req_en   : 1;  /* bit[0]   : 0&#45;->不选择统计request packet；
                                                   1&#45;->选择统计request packet。 */
        unsigned int  rsp_en   : 1;  /* bit[1]   : 0&#45;->不选择统计response packet；
                                                   1&#45;->选择统计response packet。 */
        unsigned int  reserved : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_STATUS_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_STATUS_req_en_START    (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_STATUS_req_en_END      (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_STATUS_rsp_en_START    (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_STATUS_rsp_en_END      (1)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_LENGTH_UNION
 结构说明  : MEDIA_BUS_PACKET_F0_LENGTH 寄存器结构定义。地址偏移量:0x2068，初值:0x00000000，宽度:32
 寄存器说明: F0的包长过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_0_length : 4;  /* bit[0-3] : 选择统计包长小于等于2^Filters_0_Length的packet（byte为单位） */
        unsigned int  reserved         : 28; /* bit[4-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_LENGTH_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_LENGTH_filters_0_length_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_LENGTH_filters_0_length_END    (3)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_URGENCY_UNION
 结构说明  : MEDIA_BUS_PACKET_F0_URGENCY 寄存器结构定义。地址偏移量:0x206C，初值:0x00000000，宽度:32
 寄存器说明: F0的urgency过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_0_urgency : 3;  /* bit[0-2] : 选择统计urgency 大于等于Filters_0_Urgency的packet。Eg.配置为0，则过滤所有urgency等级的packet。 */
        unsigned int  reserved          : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_URGENCY_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_URGENCY_filters_0_urgency_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_URGENCY_filters_0_urgency_END    (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_USERBASE_UNION
 结构说明  : MEDIA_BUS_PACKET_F0_USERBASE 寄存器结构定义。地址偏移量:0x2070，初值:0x00000000，宽度:32
 寄存器说明: F0的User过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_0_user_base : 3;  /* bit[0-2] : User信号过滤条件 */
        unsigned int  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_USERBASE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_USERBASE_filters_0_user_base_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_USERBASE_filters_0_user_base_END    (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_USERMASK_UNION
 结构说明  : MEDIA_BUS_PACKET_F0_USERMASK 寄存器结构定义。地址偏移量:0x2074，初值:0x00000000，宽度:32
 寄存器说明: F0的User过滤条件Mask
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_0_user_mask : 3;  /* bit[0-2] : 对UserBase进行Mask，选择UserBase中的任意几bit User信号作为过滤条件。
                                                              过滤条件：packet.User &amp; UserMask = UserBase &amp; UserMask。 */
        unsigned int  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_USERMASK_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_USERMASK_filters_0_user_mask_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F0_USERMASK_filters_0_user_mask_END    (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ROUTEIDBASE_UNION
 结构说明  : MEDIA_BUS_PACKET_F1_ROUTEIDBASE 寄存器结构定义。地址偏移量:0x2080，初值:0x00000000，宽度:32
 寄存器说明: F1的RouteId过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_1_routeid_base : 17; /* bit[0-16] : 通过配置routeId可以过滤master到slave的访问通路上的Packet：
                                                                  [16:14]&#45;->Initiator Flow ID，用于表示Master ID；
                                                                  0: ADE0, 1: ADE1, 2: CFG, 3: ISP, 4: JPU, 5: VPU, 6: VPU_SEC, 7: 保留。
                                                                  
                                                                  [13:10]&#45;->Target Flow ID，用于表示Slave ID；
                                                                  0: APB_ISP, 1: APB_JPU, 2: APB_VPU, 3: AHB_ISP, 4: APB_ADE, 5: APB_G3D, 6: APB_MEDIA_SC, 7: APB_SMMU，8: AX_SMMU, 9: XG2RAM_HARQ, 10: MEDIA_BUS_SERVICE, 11: MEDIA_BUS_SERVICE_VIDEO, Others: 保留。
                                                                  
                                                                  [9:7]&#45;->Target SubRange，表示Target NIU的不同区间地址段；
                                                                  [6:0]&#45;->SeqIds：用于表示同一条访问通路的不同transaction的ID，建议不把这7bit作为过滤条件； */
        unsigned int  reserved               : 15; /* bit[17-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ROUTEIDBASE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ROUTEIDBASE_filters_1_routeid_base_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ROUTEIDBASE_filters_1_routeid_base_END    (16)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ROUTEIDMASK_UNION
 结构说明  : MEDIA_BUS_PACKET_F1_ROUTEIDMASK 寄存器结构定义。地址偏移量:0x2084，初值:0x00000000，宽度:32
 寄存器说明: F1的RouteId过滤条件Mask配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_1_routeid_mask : 17; /* bit[0-16] : 对RouteIdBase进行Mask。
                                                                  例如：过滤条件为initiator flow + target flow，那么配置此寄存器的值为0x1FC00。 */
        unsigned int  reserved               : 15; /* bit[17-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ROUTEIDMASK_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ROUTEIDMASK_filters_1_routeid_mask_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ROUTEIDMASK_filters_1_routeid_mask_END    (16)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ADDRBASE_LOW_UNION
 结构说明  : MEDIA_BUS_PACKET_F1_ADDRBASE_LOW 寄存器结构定义。地址偏移量:0x2088，初值:0x00000000，宽度:32
 寄存器说明: F1的地址过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_1_addrbase_low : 32; /* bit[0-31]: 地址过滤条件的其实地址。 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ADDRBASE_LOW_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ADDRBASE_LOW_filters_1_addrbase_low_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_ADDRBASE_LOW_filters_1_addrbase_low_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_WINDOWSIZE_UNION
 结构说明  : MEDIA_BUS_PACKET_F1_WINDOWSIZE 寄存器结构定义。地址偏移量:0x2090，初值:0x00000000，宽度:32
 寄存器说明: F1的地址过滤的Mask地址大小配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_1_window_size : 6;  /* bit[0-5] : 地址过滤的Mask窗口大小。
                                                                例如：地址过滤的Mask窗口大小为64MB，WidowSize=log2(64MB)=26，此时，就配置filter_0_windowsize=0x1A；
                                                                Address Mask=~（2^max(WindowSize,packet.len)-1）=0xFC000000（packet.len为NOC内部表示一个packet包长变量的位宽，其值为6~7）；
                                                                过滤条件为：PacketAddress &amp; AddressMask = AddressBase &amp; AddressMask。 */
        unsigned int  reserved              : 26; /* bit[6-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_WINDOWSIZE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_WINDOWSIZE_filters_1_window_size_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_WINDOWSIZE_filters_1_window_size_END    (5)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_SECURITYBASE_UNION
 结构说明  : MEDIA_BUS_PACKET_F1_SECURITYBASE 寄存器结构定义。地址偏移量:0x2094，初值:0x00000000，宽度:32
 寄存器说明: F1的安全过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_1_security_base : 1;  /* bit[0]   : 0&#45;->表示安全过滤条件；
                                                                  1&#45;->表示非安全过滤条件。 */
        unsigned int  reserved                : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_SECURITYBASE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_SECURITYBASE_filters_1_security_base_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_SECURITYBASE_filters_1_security_base_END    (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_SECURITYMASK_UNION
 结构说明  : MEDIA_BUS_PACKET_F1_SECURITYMASK 寄存器结构定义。地址偏移量:0x2098，初值:0x00000000，宽度:32
 寄存器说明: F1的安全过滤条件的Mask
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_1_security_mask : 1;  /* bit[0]   : 安全过滤条件：packet.Security &amp; SecurityMask = SecurityBase &amp; SecurityMasks；
                                                                  也就是说当Filters_1_SecurityMask=1时安全过滤条件Filters_1_SecurityBase有效，可以过滤安全或非安全packet；否则，不起过滤作用； */
        unsigned int  reserved                : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_SECURITYMASK_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_SECURITYMASK_filters_1_security_mask_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_SECURITYMASK_filters_1_security_mask_END    (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_OPCODE_UNION
 结构说明  : MEDIA_BUS_PACKET_F1_OPCODE 寄存器结构定义。地址偏移量:0x209C，初值:0x00000000，宽度:32
 寄存器说明: F1的Opcode过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rd_en    : 1;  /* bit[0]   : 0&#45;->不选择统计读操作packet；
                                                   1&#45;->选择统计读操作packet。 */
        unsigned int  wr_en    : 1;  /* bit[1]   : 0&#45;->不选择统计写操作packet；
                                                   1&#45;->选择统计写操作packet。 */
        unsigned int  lock_en  : 1;  /* bit[2]   : 0&#45;->不选择统计exclusive transaction；
                                                   1&#45;->选择统计exclusive transaction。 */
        unsigned int  urg_en   : 1;  /* bit[3]   : 0&#45;->不选择统计带有urgency的packet；
                                                   1&#45;->选择统计带有urgency的packet。 */
        unsigned int  reserved : 28; /* bit[4-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_OPCODE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_OPCODE_rd_en_START     (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_OPCODE_rd_en_END       (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_OPCODE_wr_en_START     (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_OPCODE_wr_en_END       (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_OPCODE_lock_en_START   (2)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_OPCODE_lock_en_END     (2)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_OPCODE_urg_en_START    (3)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_OPCODE_urg_en_END      (3)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_STATUS_UNION
 结构说明  : MEDIA_BUS_PACKET_F1_STATUS 寄存器结构定义。地址偏移量:0x20A0，初值:0x00000000，宽度:32
 寄存器说明: F1的packet状态过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  req_en   : 1;  /* bit[0]   : 0&#45;->不选择统计request packet；
                                                   1&#45;->选择统计request packet。 */
        unsigned int  rsp_en   : 1;  /* bit[1]   : 0&#45;->不选择统计response packet；
                                                   1&#45;->选择统计response packet。 */
        unsigned int  reserved : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_STATUS_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_STATUS_req_en_START    (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_STATUS_req_en_END      (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_STATUS_rsp_en_START    (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_STATUS_rsp_en_END      (1)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_LENGTH_UNION
 结构说明  : MEDIA_BUS_PACKET_F1_LENGTH 寄存器结构定义。地址偏移量:0x20A4，初值:0x00000000，宽度:32
 寄存器说明: F1的包长过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_1_length : 4;  /* bit[0-3] : 选择统计包长小于等于2^Filters_0_Length的packet */
        unsigned int  reserved         : 28; /* bit[4-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_LENGTH_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_LENGTH_filters_1_length_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_LENGTH_filters_1_length_END    (3)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_URGENCY_UNION
 结构说明  : MEDIA_BUS_PACKET_F1_URGENCY 寄存器结构定义。地址偏移量:0x20A8，初值:0x00000000，宽度:32
 寄存器说明: F1的urgency过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_1_urgency : 3;  /* bit[0-2] : 选择统计urgency 大于等于Filters_0_Urgency的packet。Eg.配置为0，则过滤所有urgency等级的packet。 */
        unsigned int  reserved          : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_URGENCY_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_URGENCY_filters_1_urgency_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_URGENCY_filters_1_urgency_END    (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_USERBASE_UNION
 结构说明  : MEDIA_BUS_PACKET_F1_USERBASE 寄存器结构定义。地址偏移量:0x20AC，初值:0x00000000，宽度:32
 寄存器说明: F1的User过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_1_userbase : 3;  /* bit[0-2] : User信号过滤条件 */
        unsigned int  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_USERBASE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_USERBASE_filters_1_userbase_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_USERBASE_filters_1_userbase_END    (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_USERMASK_UNION
 结构说明  : MEDIA_BUS_PACKET_F1_USERMASK 寄存器结构定义。地址偏移量:0x20B0，初值:0x00000000，宽度:32
 寄存器说明: F1的User过滤条件Mask
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_1_usermask : 3;  /* bit[0-2] : 对UserBase进行Mask，选择UserBase中的任意几bit User信号作为过滤条件。
                                                             过滤条件：packet.User &amp; UserMask = UserBase &amp; UserMask。 */
        unsigned int  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_USERMASK_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_USERMASK_filters_1_usermask_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F1_USERMASK_filters_1_usermask_END    (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ROUTEIDBASE_UNION
 结构说明  : MEDIA_BUS_PACKET_F2_ROUTEIDBASE 寄存器结构定义。地址偏移量:0x20BC，初值:0x00000000，宽度:32
 寄存器说明: F2的RouteId过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_2_routeid_base : 17; /* bit[0-16] : 通过配置routeId可以过滤master到slave的访问通路上的Packet：
                                                                  [16:14]&#45;->Initiator Flow ID，用于表示Master ID；
                                                                  0: ADE0, 1: ADE1, 2: CFG, 3: ISP, 4: JPU, 5: VPU, 6: VPU_SEC, 7: 保留。
                                                                  
                                                                  [13:10]&#45;->Target Flow ID，用于表示Slave ID；
                                                                  0: APB_ISP, 1: APB_JPU, 2: APB_VPU, 3: AHB_ISP, 4: APB_ADE, 5: APB_G3D, 6: APB_MEDIA_SC, 7: APB_SMMU，8: AX_SMMU, 9: XG2RAM_HARQ, 10: MEDIA_BUS_SERVICE, 11: MEDIA_BUS_SERVICE_VIDEO, Others: 保留。
                                                                  
                                                                  [9:7]&#45;->Target SubRange，表示Target NIU的不同区间地址段；
                                                                  [6:0]&#45;->SeqIds：用于表示同一条访问通路的不同transaction的ID，建议不把这7bit作为过滤条件； */
        unsigned int  reserved               : 15; /* bit[17-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ROUTEIDBASE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ROUTEIDBASE_filters_2_routeid_base_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ROUTEIDBASE_filters_2_routeid_base_END    (16)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ROUTEIDMASK_UNION
 结构说明  : MEDIA_BUS_PACKET_F2_ROUTEIDMASK 寄存器结构定义。地址偏移量:0x20C0，初值:0x00000000，宽度:32
 寄存器说明: F2的RouteId过滤条件Mask配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_2_routeid_mask : 17; /* bit[0-16] : 对RouteIdBase进行Mask。
                                                                  例如：过滤条件为initiator flow + target flow，那么配置此寄存器的值为0x1FC00。 */
        unsigned int  reserved               : 15; /* bit[17-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ROUTEIDMASK_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ROUTEIDMASK_filters_2_routeid_mask_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ROUTEIDMASK_filters_2_routeid_mask_END    (16)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ADDRBASE_LOW_UNION
 结构说明  : MEDIA_BUS_PACKET_F2_ADDRBASE_LOW 寄存器结构定义。地址偏移量:0x20C4，初值:0x00000000，宽度:32
 寄存器说明: F2的地址过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_2_addrbase_low : 32; /* bit[0-31]: 地址过滤条件的其实地址。 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ADDRBASE_LOW_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ADDRBASE_LOW_filters_2_addrbase_low_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_ADDRBASE_LOW_filters_2_addrbase_low_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_WINDOWSIZE_UNION
 结构说明  : MEDIA_BUS_PACKET_F2_WINDOWSIZE 寄存器结构定义。地址偏移量:0x20CC，初值:0x00000000，宽度:32
 寄存器说明: F2的地址过滤的Mask地址大小配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_2_window_size : 6;  /* bit[0-5] : 地址过滤的Mask窗口大小。
                                                                例如：地址过滤的Mask窗口大小为64MB，WidowSize=log2(64MB)=26，此时，就配置filter_0_windowsize=0x1A；
                                                                Address Mask=~（2^max(WindowSize,packet.len)-1）=0xFC000000（packet.len为NOC内部表示一个packet包长变量的位宽，其值为6~7）；
                                                                过滤条件为：PacketAddress &amp; AddressMask = AddressBase &amp; AddressMask。 */
        unsigned int  reserved              : 26; /* bit[6-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_WINDOWSIZE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_WINDOWSIZE_filters_2_window_size_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_WINDOWSIZE_filters_2_window_size_END    (5)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_SECURITYBASE_UNION
 结构说明  : MEDIA_BUS_PACKET_F2_SECURITYBASE 寄存器结构定义。地址偏移量:0x20D0，初值:0x00000000，宽度:32
 寄存器说明: F2的安全过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_2_security_base : 1;  /* bit[0]   : 0&#45;->表示安全过滤条件；
                                                                  1&#45;->表示非安全过滤条件。 */
        unsigned int  reserved                : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_SECURITYBASE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_SECURITYBASE_filters_2_security_base_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_SECURITYBASE_filters_2_security_base_END    (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_SECURITYMASK_UNION
 结构说明  : MEDIA_BUS_PACKET_F2_SECURITYMASK 寄存器结构定义。地址偏移量:0x20D4，初值:0x00000000，宽度:32
 寄存器说明: F2的安全过滤条件的Mask
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_2_security_mask : 1;  /* bit[0]   : 安全过滤条件：packet.Security &amp; SecurityMask = SecurityBase &amp; SecurityMasks；
                                                                  也就是说当Filters_1_SecurityMask=1时安全过滤条件Filters_1_SecurityBase有效，可以过滤安全或非安全packet；否则，不起过滤作用； */
        unsigned int  reserved                : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_SECURITYMASK_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_SECURITYMASK_filters_2_security_mask_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_SECURITYMASK_filters_2_security_mask_END    (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_OPCODE_UNION
 结构说明  : MEDIA_BUS_PACKET_F2_OPCODE 寄存器结构定义。地址偏移量:0x20D8，初值:0x00000000，宽度:32
 寄存器说明: F2的Opcode过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rd_en    : 1;  /* bit[0]   : 0&#45;->不选择统计读操作packet；
                                                   1&#45;->选择统计读操作packet。 */
        unsigned int  wr_en    : 1;  /* bit[1]   : 0&#45;->不选择统计写操作packet；
                                                   1&#45;->选择统计写操作packet。 */
        unsigned int  lock_en  : 1;  /* bit[2]   : 0&#45;->不选择统计exclusive transaction；
                                                   1&#45;->选择统计exclusive transaction。 */
        unsigned int  urg_en   : 1;  /* bit[3]   : 0&#45;->不选择统计带有urgency的packet；
                                                   1&#45;->选择统计带有urgency的packet。 */
        unsigned int  reserved : 28; /* bit[4-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_OPCODE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_OPCODE_rd_en_START     (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_OPCODE_rd_en_END       (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_OPCODE_wr_en_START     (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_OPCODE_wr_en_END       (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_OPCODE_lock_en_START   (2)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_OPCODE_lock_en_END     (2)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_OPCODE_urg_en_START    (3)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_OPCODE_urg_en_END      (3)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_STATUS_UNION
 结构说明  : MEDIA_BUS_PACKET_F2_STATUS 寄存器结构定义。地址偏移量:0x20DC，初值:0x00000000，宽度:32
 寄存器说明: F2的packet状态过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  req_en   : 1;  /* bit[0]   : 0&#45;->不选择统计request packet；
                                                   1&#45;->选择统计request packet。 */
        unsigned int  rsp_en   : 1;  /* bit[1]   : 0&#45;->不选择统计response packet；
                                                   1&#45;->选择统计response packet。 */
        unsigned int  reserved : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_STATUS_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_STATUS_req_en_START    (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_STATUS_req_en_END      (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_STATUS_rsp_en_START    (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_STATUS_rsp_en_END      (1)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_LENGTH_UNION
 结构说明  : MEDIA_BUS_PACKET_F2_LENGTH 寄存器结构定义。地址偏移量:0x20E0，初值:0x00000000，宽度:32
 寄存器说明: F2的包长过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_2_length : 4;  /* bit[0-3] : 选择统计包长小于等于2^Filters_0_Length的packet */
        unsigned int  reserved         : 28; /* bit[4-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_LENGTH_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_LENGTH_filters_2_length_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_LENGTH_filters_2_length_END    (3)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_URGENCY_UNION
 结构说明  : MEDIA_BUS_PACKET_F2_URGENCY 寄存器结构定义。地址偏移量:0x20E4，初值:0x00000000，宽度:32
 寄存器说明: F2的urgency过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_2_urgency : 3;  /* bit[0-2] : 选择统计urgency 大于等于Filters_0_Urgency的packet。Eg.配置为0，则过滤所有urgency等级的packet。 */
        unsigned int  reserved          : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_URGENCY_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_URGENCY_filters_2_urgency_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_URGENCY_filters_2_urgency_END    (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_USERBASE_UNION
 结构说明  : MEDIA_BUS_PACKET_F2_USERBASE 寄存器结构定义。地址偏移量:0x20E8，初值:0x00000000，宽度:32
 寄存器说明: F2的User过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_2_userbase : 3;  /* bit[0-2] : User信号过滤条件 */
        unsigned int  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_USERBASE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_USERBASE_filters_2_userbase_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_USERBASE_filters_2_userbase_END    (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_USERMASK_UNION
 结构说明  : MEDIA_BUS_PACKET_F2_USERMASK 寄存器结构定义。地址偏移量:0x20EC，初值:0x00000000，宽度:32
 寄存器说明: F2的User过滤条件Mask
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_2_usermask : 3;  /* bit[0-2] : 对UserBase进行Mask，选择UserBase中的任意几bit User信号作为过滤条件。
                                                             过滤条件：packet.User &amp; UserMask = UserBase &amp; UserMask。 */
        unsigned int  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_USERMASK_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_USERMASK_filters_2_usermask_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F2_USERMASK_filters_2_usermask_END    (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ROUTEIDBASE_UNION
 结构说明  : MEDIA_BUS_PACKET_F3_ROUTEIDBASE 寄存器结构定义。地址偏移量:0x20F8，初值:0x00000000，宽度:32
 寄存器说明: F3的RouteId过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_3_routeid_base : 17; /* bit[0-16] : 通过配置routeId可以过滤master到slave的访问通路上的Packet：
                                                                  [16:14]&#45;->Initiator Flow ID，用于表示Master ID；
                                                                  0: ADE0, 1: ADE1, 2: CFG, 3: ISP, 4: JPU, 5: VPU, 6: VPU_SEC, 7: 保留。
                                                                  
                                                                  [13:10]&#45;->Target Flow ID，用于表示Slave ID；
                                                                  0: APB_ISP, 1: APB_JPU, 2: APB_VPU, 3: AHB_ISP, 4: APB_ADE, 5: APB_G3D, 6: APB_MEDIA_SC, 7: APB_SMMU，8: AX_SMMU, 9: XG2RAM_HARQ, 10: MEDIA_BUS_SERVICE, 11: MEDIA_BUS_SERVICE_VIDEO, Others: 保留。
                                                                  
                                                                  [9:7]&#45;->Target SubRange，表示Target NIU的不同区间地址段；
                                                                  [6:0]&#45;->SeqIds：用于表示同一条访问通路的不同transaction的ID，建议不把这7bit作为过滤条件； */
        unsigned int  reserved               : 15; /* bit[17-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ROUTEIDBASE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ROUTEIDBASE_filters_3_routeid_base_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ROUTEIDBASE_filters_3_routeid_base_END    (16)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ROUTEIDMASK_UNION
 结构说明  : MEDIA_BUS_PACKET_F3_ROUTEIDMASK 寄存器结构定义。地址偏移量:0x20FC，初值:0x00000000，宽度:32
 寄存器说明: F3的RouteId过滤条件Mask配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_3_routeid_mask : 17; /* bit[0-16] : 对RouteIdBase进行Mask。
                                                                  例如：过滤条件为initiator flow + target flow，那么配置此寄存器的值为0x1FC00。 */
        unsigned int  reserved               : 15; /* bit[17-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ROUTEIDMASK_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ROUTEIDMASK_filters_3_routeid_mask_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ROUTEIDMASK_filters_3_routeid_mask_END    (16)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ADDRBASE_LOW_UNION
 结构说明  : MEDIA_BUS_PACKET_F3_ADDRBASE_LOW 寄存器结构定义。地址偏移量:0x2100，初值:0x00000000，宽度:32
 寄存器说明: F3的地址过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_3_addrbase_low : 32; /* bit[0-31]: 地址过滤条件的基地址。 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ADDRBASE_LOW_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ADDRBASE_LOW_filters_3_addrbase_low_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_ADDRBASE_LOW_filters_3_addrbase_low_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_WINDOWSIZE_UNION
 结构说明  : MEDIA_BUS_PACKET_F3_WINDOWSIZE 寄存器结构定义。地址偏移量:0x2108，初值:0x00000000，宽度:32
 寄存器说明: F3的地址过滤的Mask地址大小配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_3_window_size : 6;  /* bit[0-5] : 地址过滤的Mask窗口大小。
                                                                例如：地址过滤的Mask窗口大小为64MB，WidowSize=log2(64MB)=26，此时，就配置filter_0_windowsize=0x1A；
                                                                Address Mask=~（2^max(WindowSize,packet.len)-1）=0xFC000000（packet.len为NOC内部表示一个packet包长变量的位宽，其值为6~7）；
                                                                过滤条件为：PacketAddress &amp; AddressMask = AddressBase &amp; AddressMask。 */
        unsigned int  reserved              : 26; /* bit[6-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_WINDOWSIZE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_WINDOWSIZE_filters_3_window_size_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_WINDOWSIZE_filters_3_window_size_END    (5)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_SECURITYBASE_UNION
 结构说明  : MEDIA_BUS_PACKET_F3_SECURITYBASE 寄存器结构定义。地址偏移量:0x210C，初值:0x00000000，宽度:32
 寄存器说明: F3的安全过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_3_security_base : 1;  /* bit[0]   : 0&#45;->表示安全过滤条件；
                                                                  1&#45;->表示非安全过滤条件。 */
        unsigned int  reserved                : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_SECURITYBASE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_SECURITYBASE_filters_3_security_base_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_SECURITYBASE_filters_3_security_base_END    (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_SECURITYMASK_UNION
 结构说明  : MEDIA_BUS_PACKET_F3_SECURITYMASK 寄存器结构定义。地址偏移量:0x2110，初值:0x00000000，宽度:32
 寄存器说明: F3的安全过滤条件的Mask
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_3_security_mask : 1;  /* bit[0]   : 安全过滤条件：packet.Security &amp; SecurityMask = SecurityBase &amp; SecurityMasks；
                                                                  也就是说当Filters_0_SecurityMask=1时安全过滤条件Filters_0_SecurityBase有效，可以过滤安全或非安全packet；否则，不起过滤作用； */
        unsigned int  reserved                : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_SECURITYMASK_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_SECURITYMASK_filters_3_security_mask_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_SECURITYMASK_filters_3_security_mask_END    (0)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_OPCODE_UNION
 结构说明  : MEDIA_BUS_PACKET_F3_OPCODE 寄存器结构定义。地址偏移量:0x2114，初值:0x00000000，宽度:32
 寄存器说明: F3的Opcode过滤条件配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rd_en    : 1;  /* bit[0]   : 0&#45;->不选择统计读操作packet；
                                                   1&#45;->选择统计读操作packet。 */
        unsigned int  wr_en    : 1;  /* bit[1]   : 0&#45;->不选择统计写操作packet；
                                                   1&#45;->选择统计写操作packet。 */
        unsigned int  lock_en  : 1;  /* bit[2]   : 0&#45;->不选择统计exclusive transaction；
                                                   1&#45;->选择统计exclusive transaction。 */
        unsigned int  urg_en   : 1;  /* bit[3]   : 0&#45;->不选择统计带有urgency的packet；
                                                   1&#45;->选择统计带有urgency的packet。 */
        unsigned int  reserved : 28; /* bit[4-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_OPCODE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_OPCODE_rd_en_START     (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_OPCODE_rd_en_END       (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_OPCODE_wr_en_START     (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_OPCODE_wr_en_END       (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_OPCODE_lock_en_START   (2)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_OPCODE_lock_en_END     (2)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_OPCODE_urg_en_START    (3)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_OPCODE_urg_en_END      (3)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_STATUS_UNION
 结构说明  : MEDIA_BUS_PACKET_F3_STATUS 寄存器结构定义。地址偏移量:0x2118，初值:0x00000000，宽度:32
 寄存器说明: F3的packet状态过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  req_en   : 1;  /* bit[0]   : 0&#45;->不选择统计request packet；
                                                   1&#45;->选择统计request packet。 */
        unsigned int  rsp_en   : 1;  /* bit[1]   : 0&#45;->不选择统计response packet；
                                                   1&#45;->选择统计response packet。 */
        unsigned int  reserved : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_STATUS_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_STATUS_req_en_START    (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_STATUS_req_en_END      (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_STATUS_rsp_en_START    (1)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_STATUS_rsp_en_END      (1)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_LENGTH_UNION
 结构说明  : MEDIA_BUS_PACKET_F3_LENGTH 寄存器结构定义。地址偏移量:0x211C，初值:0x00000000，宽度:32
 寄存器说明: F3的包长过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_3_length : 4;  /* bit[0-3] : 选择统计包长小于等于2^Filters_0_Length的packet（byte为单位） */
        unsigned int  reserved         : 28; /* bit[4-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_LENGTH_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_LENGTH_filters_3_length_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_LENGTH_filters_3_length_END    (3)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_URGENCY_UNION
 结构说明  : MEDIA_BUS_PACKET_F3_URGENCY 寄存器结构定义。地址偏移量:0x2120，初值:0x00000000，宽度:32
 寄存器说明: F3的urgency过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_3_urgency : 3;  /* bit[0-2] : 选择统计urgency 大于等于Filters_0_Urgency的packet。Eg.配置为0，则过滤所有urgency等级的packet。 */
        unsigned int  reserved          : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_URGENCY_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_URGENCY_filters_3_urgency_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_URGENCY_filters_3_urgency_END    (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_USERBASE_UNION
 结构说明  : MEDIA_BUS_PACKET_F3_USERBASE 寄存器结构定义。地址偏移量:0x2124，初值:0x00000000，宽度:32
 寄存器说明: F3的User过滤条件
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_3_user_base : 3;  /* bit[0-2] : User信号过滤条件 */
        unsigned int  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_USERBASE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_USERBASE_filters_3_user_base_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_USERBASE_filters_3_user_base_END    (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_USERMASK_UNION
 结构说明  : MEDIA_BUS_PACKET_F3_USERMASK 寄存器结构定义。地址偏移量:0x2128，初值:0x00000000，宽度:32
 寄存器说明: F3的User过滤条件Mask
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  filters_3_user_mask : 3;  /* bit[0-2] : 对UserBase进行Mask，选择UserBase中的任意几bit User信号作为过滤条件。
                                                              过滤条件：packet.User &amp; UserMask = UserBase &amp; UserMask。 */
        unsigned int  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_USERMASK_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_USERMASK_filters_3_user_mask_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_F3_USERMASK_filters_3_user_mask_END    (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_SRC_UNION
 结构说明  : MEDIA_BUS_PACKET_COUNTERS_0_SRC 寄存器结构定义。地址偏移量:0x2138，初值:0x00000000，宽度:32
 寄存器说明: Counter的统计类型控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_event : 5;  /* bit[0-4] : 当CfgCtl寄存器中的Active=0时，该寄存器可配置，其它时间均不可配置：
                                                    0x00&#45;->disabled计数器；
                                                    0x01&#45;->Clock cycles；
                                                    0x02&#45;->IDLE cycles；
                                                    0x03&#45;->Transfer cycles；
                                                    0x04&#45;->Busy cycles；
                                                    0x05&#45;->Wait cycles；
                                                    0x06&#45;->Packets个数统计；
                                                    0x07&#45;->被LUT过滤选中的Packet个数统计；
                                                    0x08&#45;->统计总的bytes数量；
                                                    0x09&#45;->统计Pressure level>0的时钟周期个数；
                                                    0x0A&#45;->统计Pressure level>1的时钟周期个数；
                                                    0x0B&#45;->统计Pressure level>2（Press[2]=1）的时钟周期个数；
                                                    0x0C&#45;->被F0过滤条件选中的Packet数量；
                                                    0x0D&#45;->被F1过滤条件选中的Packet数量；
                                                    0x0E~0x0F&#45;->保留；
                                                    0x10&#45;->将counter_2N和counter_2N+1两个统计计数器进行串联，只有标号为奇数统计计数器可被串联，且只能将两个统计计数器进行串联；
                                                    0x11&#45;->通过LUT过滤统计Packet中的有效Byte数量；
                                                    0x12&#45;->通过LUT过滤统计Packet中的Byte数量（含无效Byte，作用于含byteenable的写操作）；
                                                    0x13&#45;->通过filtersx（当counters_m_src中的m是奇数时，filterx表示filter0；当counters_m_src中的m是偶数时，filterx表示filter0）过滤统计Packet中的有效Byte数量；
                                                    0x14&#45;->通过filtersx（当counters_m_src中的m是奇数时，filterx表示filter0；当counters_m_src中的m是偶数时，filterx表示filter0）过滤统计Packet中的Byte数量（含无效Byte）；
                                                    0x15~0x1F&#45;->保留；
                                                    0x20~0x3F&#45;->保留； */
        unsigned int  reserved  : 27; /* bit[5-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_SRC_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_SRC_int_event_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_SRC_int_event_END    (4)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_ALARMMODE_UNION
 结构说明  : MEDIA_BUS_PACKET_COUNTERS_0_ALARMMODE 寄存器结构定义。地址偏移量:0x213C，初值:0x00000000，宽度:32
 寄存器说明: 告警中断产生配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  counters_0_alarmmode : 2;  /* bit[0-1] : 当CfgCtl寄存器中的Active=0时，该寄存器可配置，其它时间均不可配置：
                                                               0x0&#45;->OFF，告警模式配置disabled；
                                                               0x1&#45;->MIN，在statperiod配置的统计周期结束后，counter的值小于StatAlarmMin寄存器的值，则StatAlarmStatus为1；
                                                               0x2&#45;->MAX，在statperiod配置的统计周期结束后，counter的值大于StatAlarmMax寄存器的值，则StatAlarmStatus为1；
                                                               0x3&#45;->MIN_MAX，在statperiod配置的统计周期结束后，counter的值小于StatAlarmMin寄存器的值或大于StatAlarmMax寄存器的值，则StatAlarmStatus为1； */
        unsigned int  reserved             : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_ALARMMODE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_ALARMMODE_counters_0_alarmmode_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_ALARMMODE_counters_0_alarmmode_END    (1)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_VAL_UNION
 结构说明  : MEDIA_BUS_PACKET_COUNTERS_0_VAL 寄存器结构定义。地址偏移量:0x2140，初值:0x00000000，宽度:32
 寄存器说明: 统计计数器的统计值
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  counters_0_val : 16; /* bit[0-15] : 当Counters_0_Val达到最大之后，会保持不变直到被clear（串联计数器除外）； */
        unsigned int  reserved       : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_VAL_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_VAL_counters_0_val_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_0_VAL_counters_0_val_END    (15)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_SRC_UNION
 结构说明  : MEDIA_BUS_PACKET_COUNTERS_1_SRC 寄存器结构定义。地址偏移量:0x214C，初值:0x00000000，宽度:32
 寄存器说明: Counter的统计类型控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_event : 5;  /* bit[0-4] : 当CfgCtl寄存器中的Active=0时，该寄存器可配置，其它时间均不可配置：
                                                    0x00&#45;->disabled计数器；
                                                    0x01&#45;->Clock cycles；
                                                    0x02&#45;->IDLE cycles；
                                                    0x03&#45;->Transfer cycles；
                                                    0x04&#45;->Busy cycles；
                                                    0x05&#45;->Wait cycles；
                                                    0x06&#45;->Packets个数统计；
                                                    0x07&#45;->被LUT过滤选中的Packet个数统计；
                                                    0x08&#45;->统计总的bytes数量；
                                                    0x09&#45;->统计Pressure level>0的时钟周期个数；
                                                    0x0A&#45;->统计Pressure level>1的时钟周期个数；
                                                    0x0B&#45;->统计Pressure level>2（Press[2]=1）的时钟周期个数；
                                                    0x0C&#45;->被F0过滤条件选中的Packet数量；
                                                    0x0D&#45;->被F1过滤条件选中的Packet数量；
                                                    0x0E~0x0F&#45;->保留；
                                                    0x10&#45;->将counter_2N和counter_2N+1两个统计计数器进行串联，只有标号为奇数统计计数器可被串联，且只能将两个统计计数器进行串联；
                                                    0x11&#45;->通过LUT过滤统计Packet中的有效Byte数量；
                                                    0x12&#45;->通过LUT过滤统计Packet中的Byte数量（含无效Byte，作用于含byteenable的写操作）；
                                                    0x13&#45;->通过filtersx（当counters_m_src中的m是奇数时，filterx表示filter0；当counters_m_src中的m是偶数时，filterx表示filter0）过滤统计Packet中的有效Byte数量；
                                                    0x14&#45;->通过filtersx（当counters_m_src中的m是奇数时，filterx表示filter0；当counters_m_src中的m是偶数时，filterx表示filter0）过滤统计Packet中的Byte数量（含无效Byte）；
                                                    0x15~0x1F&#45;->保留；
                                                    0x20~0x3F&#45;->保留； */
        unsigned int  reserved  : 27; /* bit[5-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_SRC_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_SRC_int_event_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_SRC_int_event_END    (4)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_ALARMMODE_UNION
 结构说明  : MEDIA_BUS_PACKET_COUNTERS_1_ALARMMODE 寄存器结构定义。地址偏移量:0x2150，初值:0x00000000，宽度:32
 寄存器说明: 告警中断产生配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  counters_1_alarmmode : 2;  /* bit[0-1] : 当CfgCtl寄存器中的Active=0时，该寄存器可配置，其它时间均不可配置：
                                                               0x0&#45;->OFF，告警模式配置disabled；
                                                               0x1&#45;->MIN，在statperiod配置的统计周期结束后，counter的值小于StatAlarmMin寄存器的值，则StatAlarmStatus为1；
                                                               0x2&#45;->MAX，在statperiod配置的统计周期结束后，counter的值大于StatAlarmMax寄存器的值，则StatAlarmStatus为1；
                                                               0x3&#45;->MIN_MAX，在statperiod配置的统计周期结束后，counter的值小于StatAlarmMin寄存器的值或大于StatAlarmMax寄存器的值，则StatAlarmStatus为1； */
        unsigned int  reserved             : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_ALARMMODE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_ALARMMODE_counters_1_alarmmode_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_ALARMMODE_counters_1_alarmmode_END    (1)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_VAL_UNION
 结构说明  : MEDIA_BUS_PACKET_COUNTERS_1_VAL 寄存器结构定义。地址偏移量:0x2154，初值:0x00000000，宽度:32
 寄存器说明: 统计计数器的统计值
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  counters_1_val : 16; /* bit[0-15] : 当Counters_1_Val达到最大之后，会保持不变直到被clear（串联计数器除外）； */
        unsigned int  reserved       : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_VAL_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_VAL_counters_1_val_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_1_VAL_counters_1_val_END    (15)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_SRC_UNION
 结构说明  : MEDIA_BUS_PACKET_COUNTERS_2_SRC 寄存器结构定义。地址偏移量:0x2160，初值:0x00000000，宽度:32
 寄存器说明: Counter的统计类型控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_event : 5;  /* bit[0-4] : 当CfgCtl寄存器中的Active=0时，该寄存器可配置，其它时间均不可配置：
                                                    0x00&#45;->disabled计数器；
                                                    0x01&#45;->Clock cycles；
                                                    0x02&#45;->IDLE cycles；
                                                    0x03&#45;->Transfer cycles；
                                                    0x04&#45;->Busy cycles；
                                                    0x05&#45;->Wait cycles；
                                                    0x06&#45;->Packets个数统计；
                                                    0x07&#45;->被LUT过滤选中的Packet个数统计；
                                                    0x08&#45;->统计总的bytes数量；
                                                    0x09&#45;->统计Pressure level>0的时钟周期个数；
                                                    0x0A&#45;->统计Pressure level>1的时钟周期个数；
                                                    0x0B&#45;->统计Pressure level>2（Press[2]=1）的时钟周期个数；
                                                    0x0C&#45;->被F0过滤条件选中的Packet数量；
                                                    0x0D&#45;->被F1过滤条件选中的Packet数量；
                                                    0x0E~0x0F&#45;->保留；
                                                    0x10&#45;->将counter_2N和counter_2N+1两个统计计数器进行串联，只有标号为奇数统计计数器可被串联，且只能将两个统计计数器进行串联；
                                                    0x11&#45;->通过LUT过滤统计Packet中的有效Byte数量；
                                                    0x12&#45;->通过LUT过滤统计Packet中的Byte数量（含无效Byte，作用于含byteenable的写操作）；
                                                    0x13&#45;->通过filtersx（当counters_m_src中的m是奇数时，filterx表示filter0；当counters_m_src中的m是偶数时，filterx表示filter0）过滤统计Packet中的有效Byte数量；
                                                    0x14&#45;->通过filtersx（当counters_m_src中的m是奇数时，filterx表示filter0；当counters_m_src中的m是偶数时，filterx表示filter0）过滤统计Packet中的Byte数量（含无效Byte）；
                                                    0x15~0x1F&#45;->保留；
                                                    0x20~0x3F&#45;->保留； */
        unsigned int  reserved  : 27; /* bit[5-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_SRC_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_SRC_int_event_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_SRC_int_event_END    (4)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_ALARMMODE_UNION
 结构说明  : MEDIA_BUS_PACKET_COUNTERS_2_ALARMMODE 寄存器结构定义。地址偏移量:0x2164，初值:0x00000000，宽度:32
 寄存器说明: 告警中断产生配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  counters_2_alarmmode : 2;  /* bit[0-1] : 当CfgCtl寄存器中的Active=0时，该寄存器可配置，其它时间均不可配置：
                                                               0x0&#45;->OFF，告警模式配置disabled；
                                                               0x1&#45;->MIN，在statperiod配置的统计周期结束后，counter的值小于StatAlarmMin寄存器的值，则StatAlarmStatus为1；
                                                               0x2&#45;->MAX，在statperiod配置的统计周期结束后，counter的值大于StatAlarmMax寄存器的值，则StatAlarmStatus为1；
                                                               0x3&#45;->MIN_MAX，在statperiod配置的统计周期结束后，counter的值小于StatAlarmMin寄存器的值或大于StatAlarmMax寄存器的值，则StatAlarmStatus为1； */
        unsigned int  reserved             : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_ALARMMODE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_ALARMMODE_counters_2_alarmmode_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_ALARMMODE_counters_2_alarmmode_END    (1)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_VAL_UNION
 结构说明  : MEDIA_BUS_PACKET_COUNTERS_2_VAL 寄存器结构定义。地址偏移量:0x2168，初值:0x00000000，宽度:32
 寄存器说明: 统计计数器的统计值
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  counters_2_val : 16; /* bit[0-15] : 当Counters_2_Val达到最大之后，会保持不变直到被clear（串联计数器除外）； */
        unsigned int  reserved       : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_VAL_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_VAL_counters_2_val_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_2_VAL_counters_2_val_END    (15)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_SRC_UNION
 结构说明  : MEDIA_BUS_PACKET_COUNTERS_3_SRC 寄存器结构定义。地址偏移量:0x2174，初值:0x00000000，宽度:32
 寄存器说明: Counter的统计类型控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_event : 5;  /* bit[0-4] : 当CfgCtl寄存器中的Active=0时，该寄存器可配置，其它时间均不可配置：
                                                    0x00&#45;->disabled计数器；
                                                    0x01&#45;->Clock cycles；
                                                    0x02&#45;->IDLE cycles；
                                                    0x03&#45;->Transfer cycles；
                                                    0x04&#45;->Busy cycles；
                                                    0x05&#45;->Wait cycles；
                                                    0x06&#45;->Packets个数统计；
                                                    0x07&#45;->被LUT过滤选中的Packet个数统计；
                                                    0x08&#45;->统计总的bytes数量；
                                                    0x09&#45;->统计Pressure level>0的时钟周期个数；
                                                    0x0A&#45;->统计Pressure level>1的时钟周期个数；
                                                    0x0B&#45;->统计Pressure level>2（Press[2]=1）的时钟周期个数；
                                                    0x0C&#45;->被F0过滤条件选中的Packet数量；
                                                    0x0D&#45;->被F1过滤条件选中的Packet数量；
                                                    0x0E~0x0F&#45;->保留；
                                                    0x10&#45;->将counter_2N和counter_2N+1两个统计计数器进行串联，只有标号为奇数统计计数器可被串联，且只能将两个统计计数器进行串联；
                                                    0x11&#45;->通过LUT过滤统计Packet中的有效Byte数量；
                                                    0x12&#45;->通过LUT过滤统计Packet中的Byte数量（含无效Byte，作用于含byteenable的写操作）；
                                                    0x13&#45;->通过filtersx（当counters_m_src中的m是奇数时，filterx表示filter0；当counters_m_src中的m是偶数时，filterx表示filter0）过滤统计Packet中的有效Byte数量；
                                                    0x14&#45;->通过filtersx（当counters_m_src中的m是奇数时，filterx表示filter0；当counters_m_src中的m是偶数时，filterx表示filter0）过滤统计Packet中的Byte数量（含无效Byte）；
                                                    0x15~0x1F&#45;->保留；
                                                    0x20~0x3F&#45;->保留； */
        unsigned int  reserved  : 27; /* bit[5-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_SRC_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_SRC_int_event_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_SRC_int_event_END    (4)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_ALARMMODE_UNION
 结构说明  : MEDIA_BUS_PACKET_COUNTERS_3_ALARMMODE 寄存器结构定义。地址偏移量:0x2178，初值:0x00000000，宽度:32
 寄存器说明: 告警中断产生配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  counters_3_alarmmode : 2;  /* bit[0-1] : 当CfgCtl寄存器中的Active=0时，该寄存器可配置，其它时间均不可配置：
                                                               0x0&#45;->OFF，告警模式配置disabled；
                                                               0x1&#45;->MIN，在statperiod配置的统计周期结束后，counter的值小于StatAlarmMin寄存器的值，则StatAlarmStatus为1；
                                                               0x2&#45;->MAX，在statperiod配置的统计周期结束后，counter的值大于StatAlarmMax寄存器的值，则StatAlarmStatus为1；
                                                               0x3&#45;->MIN_MAX，在statperiod配置的统计周期结束后，counter的值小于StatAlarmMin寄存器的值或大于StatAlarmMax寄存器的值，则StatAlarmStatus为1； */
        unsigned int  reserved             : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_ALARMMODE_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_ALARMMODE_counters_3_alarmmode_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_ALARMMODE_counters_3_alarmmode_END    (1)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_VAL_UNION
 结构说明  : MEDIA_BUS_PACKET_COUNTERS_3_VAL 寄存器结构定义。地址偏移量:0x217C，初值:0x00000000，宽度:32
 寄存器说明: 统计计数器的统计值
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  counters_3_val : 16; /* bit[0-15] : 当Counters_3_Val达到最大之后，会保持不变直到被clear（串联计数器除外）； */
        unsigned int  reserved       : 16; /* bit[16-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_VAL_UNION;
#endif
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_VAL_counters_3_val_START  (0)
#define SOC_MEDIA_NOC_MEDIA_BUS_PACKET_COUNTERS_3_VAL_counters_3_val_END    (15)




/****************************************************************************
                     (2/2) SERVICE_VIDEO
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_COREID_UNION
 结构说明  : ISP_QOSGENERATOR_ID_COREID 寄存器结构定义。地址偏移量:0x0100，初值:0xA1726704，宽度:32
 寄存器说明: ISP QoS Generator 的域ID
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core_type_id   : 8;  /* bit[0-7] : ISP QoS Generator 的域ID */
        unsigned int  core_check_sum : 24; /* bit[8-31]: ISP QoS Generator 的参数检验和 */
    } reg;
} SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_COREID_UNION;
#endif
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_COREID_core_type_id_START    (0)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_COREID_core_type_id_END      (7)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_COREID_core_check_sum_START  (8)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_COREID_core_check_sum_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_REVISIONID_UNION
 结构说明  : ISP_QOSGENERATOR_ID_REVISIONID 寄存器结构定义。地址偏移量:0x0104，初值:0x012D5300，宽度:32
 寄存器说明: ISP QoS Generator总线代码版本寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  user_id    : 8;  /* bit[0-7] : 用户自定义值，不在 IP 内使用 */
        unsigned int  flexnoc_id : 24; /* bit[8-31]: 总线代码版本寄存器 */
    } reg;
} SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_REVISIONID_UNION;
#endif
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_REVISIONID_user_id_START     (0)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_REVISIONID_user_id_END       (7)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_REVISIONID_flexnoc_id_START  (8)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_ID_REVISIONID_flexnoc_id_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ISP_QOSGENERATOR_PRIORITY_UNION
 结构说明  : ISP_QOSGENERATOR_PRIORITY 寄存器结构定义。地址偏移量:0x0108，初值:0x80000000，宽度:32
 寄存器说明: ISP QoS Generator优先级寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0       : 3;  /* bit[0-2]  : 在 Regulator 模式，定义了低 hurry 值；在 Fixed/limiter 模式，定义了写 urgency 值 */
        unsigned int  reserved_0: 5;  /* bit[3-7]  : 保留 */
        unsigned int  p1       : 3;  /* bit[8-10] : 在 Regulator 模式，定义了高 hurry 值；在 Fixed/limiter 模式，定义了读 urgency 值 */
        unsigned int  reserved_1: 20; /* bit[11-30]: 保留 */
        unsigned int  mark     : 1;  /* bit[31-31]: 向后兼容标志（为0时） */
    } reg;
} SOC_MEDIA_NOC_ISP_QOSGENERATOR_PRIORITY_UNION;
#endif
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_PRIORITY_p0_START        (0)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_PRIORITY_p0_END          (2)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_PRIORITY_p1_START        (8)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_PRIORITY_p1_END          (10)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_PRIORITY_mark_START      (31)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_PRIORITY_mark_END        (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ISP_QOSGENERATOR_MODE_UNION
 结构说明  : ISP_QOSGENERATOR_MODE 寄存器结构定义。地址偏移量:0x010C，初值:0x00000000，宽度:32
 寄存器说明: ISP QoS Generator 模式寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mode     : 2;  /* bit[0-1] : 模式选择：0—Fixed；1—Limiter；2—Bypass；3—Regulator */
        unsigned int  reserved : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_ISP_QOSGENERATOR_MODE_UNION;
#endif
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_MODE_mode_START      (0)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_MODE_mode_END        (1)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ISP_QOSGENERATOR_BANDWIDTH_UNION
 结构说明  : ISP_QOSGENERATOR_BANDWIDTH 寄存器结构定义。地址偏移量:0x0110，初值:0x00000378，宽度:32
 寄存器说明: ISP QoS Generator 带宽阈值寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bandwidth : 13; /* bit[0-12] : 带宽阈值 */
        unsigned int  reserved  : 19; /* bit[13-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_ISP_QOSGENERATOR_BANDWIDTH_UNION;
#endif
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_BANDWIDTH_bandwidth_START  (0)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_BANDWIDTH_bandwidth_END    (12)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ISP_QOSGENERATOR_SATURATION_UNION
 结构说明  : ISP_QOSGENERATOR_SATURATION 寄存器结构定义。地址偏移量:0x0114，初值:0x00000100，宽度:32
 寄存器说明: ISP QoS Generator 饱和量寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  saturation : 10; /* bit[0-9]  : 带宽计数器大小 */
        unsigned int  reserved   : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_ISP_QOSGENERATOR_SATURATION_UNION;
#endif
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_SATURATION_saturation_START  (0)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_SATURATION_saturation_END    (9)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_ISP_QOSGENERATOR_EXTCONTROL_UNION
 结构说明  : ISP_QOSGENERATOR_EXTCONTROL 寄存器结构定义。地址偏移量:0x0118，初值:0x00000000，宽度:32
 寄存器说明: ISP QoS Generator外部控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  socket_qos_en : 1;  /* bit[0]   : 决定采用 socket 的 QoS 值还是 QoS Generator 的 QoS 值 */
        unsigned int  ext_thr_en    : 1;  /* bit[1-1] : 外部阈值代替带宽寄存器设置的阈值控制高低优先级 */
        unsigned int  int_clk_en    : 1;  /* bit[2-2] : 本地时钟（NIU时钟）替代参考时钟进行带宽计算 */
        unsigned int  reserved      : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_ISP_QOSGENERATOR_EXTCONTROL_UNION;
#endif
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_EXTCONTROL_socket_qos_en_START  (0)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_EXTCONTROL_socket_qos_en_END    (0)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_EXTCONTROL_ext_thr_en_START     (1)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_EXTCONTROL_ext_thr_en_END       (1)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_EXTCONTROL_int_clk_en_START     (2)
#define SOC_MEDIA_NOC_ISP_QOSGENERATOR_EXTCONTROL_int_clk_en_END       (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_COREID_UNION
 结构说明  : JPU_QOSGENERATOR_ID_COREID 寄存器结构定义。地址偏移量:0x0200，初值:0xBF0A3D04，宽度:32
 寄存器说明: JPU QoS Generator 的域ID
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core_type_id   : 8;  /* bit[0-7] : JPU QoS Generator 的域ID */
        unsigned int  core_check_sum : 24; /* bit[8-31]: JPU QoS Generator 的参数检验和 */
    } reg;
} SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_COREID_UNION;
#endif
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_COREID_core_type_id_START    (0)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_COREID_core_type_id_END      (7)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_COREID_core_check_sum_START  (8)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_COREID_core_check_sum_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_REVISIONID_UNION
 结构说明  : JPU_QOSGENERATOR_ID_REVISIONID 寄存器结构定义。地址偏移量:0x0204，初值:0x012D5300，宽度:32
 寄存器说明: JPU QoS Generator总线代码版本寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  user_id    : 8;  /* bit[0-7] : 用户自定义值，不在 IP 内使用 */
        unsigned int  flexnoc_id : 24; /* bit[8-31]: 总线代码版本寄存器 */
    } reg;
} SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_REVISIONID_UNION;
#endif
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_REVISIONID_user_id_START     (0)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_REVISIONID_user_id_END       (7)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_REVISIONID_flexnoc_id_START  (8)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_ID_REVISIONID_flexnoc_id_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_JPU_QOSGENERATOR_PRIORITY_UNION
 结构说明  : JPU_QOSGENERATOR_PRIORITY 寄存器结构定义。地址偏移量:0x0208，初值:0x80000000，宽度:32
 寄存器说明: JPU QoS Generator优先级寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0       : 3;  /* bit[0-2]  : 在 Regulator 模式，定义了低 hurry 值；在 Fixed/limiter 模式，定义了写 urgency 值 */
        unsigned int  reserved_0: 5;  /* bit[3-7]  : 保留 */
        unsigned int  p1       : 3;  /* bit[8-10] : 在 Regulator 模式，定义了高 hurry 值；在 Fixed/limiter 模式，定义了读 urgency 值 */
        unsigned int  reserved_1: 20; /* bit[11-30]: 保留 */
        unsigned int  mark     : 1;  /* bit[31-31]: 向后兼容标志（为0时） */
    } reg;
} SOC_MEDIA_NOC_JPU_QOSGENERATOR_PRIORITY_UNION;
#endif
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_PRIORITY_p0_START        (0)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_PRIORITY_p0_END          (2)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_PRIORITY_p1_START        (8)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_PRIORITY_p1_END          (10)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_PRIORITY_mark_START      (31)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_PRIORITY_mark_END        (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_JPU_QOSGENERATOR_MODE_UNION
 结构说明  : JPU_QOSGENERATOR_MODE 寄存器结构定义。地址偏移量:0x020C，初值:0x00000000，宽度:32
 寄存器说明: JPU QoS Generator 模式寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mode     : 2;  /* bit[0-1] : 模式选择：0—Fixed；1—Limiter；2—Bypass；3—Regulator */
        unsigned int  reserved : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_JPU_QOSGENERATOR_MODE_UNION;
#endif
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_MODE_mode_START      (0)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_MODE_mode_END        (1)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_JPU_QOSGENERATOR_BANDWIDTH_UNION
 结构说明  : JPU_QOSGENERATOR_BANDWIDTH 寄存器结构定义。地址偏移量:0x0210，初值:0x000001C7，宽度:32
 寄存器说明: JPU QoS Generator 带宽阈值寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bandwidth : 12; /* bit[0-11] : 带宽阈值 */
        unsigned int  reserved  : 20; /* bit[12-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_JPU_QOSGENERATOR_BANDWIDTH_UNION;
#endif
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_BANDWIDTH_bandwidth_START  (0)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_BANDWIDTH_bandwidth_END    (11)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_JPU_QOSGENERATOR_SATURATION_UNION
 结构说明  : JPU_QOSGENERATOR_SATURATION 寄存器结构定义。地址偏移量:0x0214，初值:0x00000080，宽度:32
 寄存器说明: JPU QoS Generator 饱和量寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  saturation : 10; /* bit[0-9]  : 带宽计数器大小 */
        unsigned int  reserved   : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_JPU_QOSGENERATOR_SATURATION_UNION;
#endif
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_SATURATION_saturation_START  (0)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_SATURATION_saturation_END    (9)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_JPU_QOSGENERATOR_EXTCONTROL_UNION
 结构说明  : JPU_QOSGENERATOR_EXTCONTROL 寄存器结构定义。地址偏移量:0x0218，初值:0x00000000，宽度:32
 寄存器说明: JPU QoS Generator外部控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  socket_qos_en : 1;  /* bit[0-0] : 决定采用 socket 的 QoS 值还是 QoS Generator 的 QoS 值 */
        unsigned int  ext_thr_en    : 1;  /* bit[1-1] : 外部阈值代替带宽寄存器设置的阈值控制高低优先级 */
        unsigned int  int_clk_en    : 1;  /* bit[2-2] : 本地时钟（NIU时钟）替代参考时钟进行带宽计算 */
        unsigned int  reserved      : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_JPU_QOSGENERATOR_EXTCONTROL_UNION;
#endif
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_EXTCONTROL_socket_qos_en_START  (0)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_EXTCONTROL_socket_qos_en_END    (0)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_EXTCONTROL_ext_thr_en_START     (1)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_EXTCONTROL_ext_thr_en_END       (1)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_EXTCONTROL_int_clk_en_START     (2)
#define SOC_MEDIA_NOC_JPU_QOSGENERATOR_EXTCONTROL_int_clk_en_END       (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_COREID_UNION
 结构说明  : VPU_QOSGENERATOR_ID_COREID 寄存器结构定义。地址偏移量:0x0300，初值:0xE71E5C04，宽度:32
 寄存器说明: VPU QoS Generator 的域ID
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core_type_id   : 8;  /* bit[0-7] : VPU QoS Generator 的域ID */
        unsigned int  core_check_sum : 24; /* bit[8-31]: VPU QoS Generator 的参数检验和 */
    } reg;
} SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_COREID_UNION;
#endif
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_COREID_core_type_id_START    (0)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_COREID_core_type_id_END      (7)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_COREID_core_check_sum_START  (8)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_COREID_core_check_sum_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_REVISIONID_UNION
 结构说明  : VPU_QOSGENERATOR_ID_REVISIONID 寄存器结构定义。地址偏移量:0x0304，初值:0x012D5300，宽度:32
 寄存器说明: VPU QoS Generator总线代码版本寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  user_id    : 8;  /* bit[0-7] : 用户自定义值，不在 IP 内使用 */
        unsigned int  flexnoc_id : 24; /* bit[8-31]: 总线代码版本寄存器 */
    } reg;
} SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_REVISIONID_UNION;
#endif
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_REVISIONID_user_id_START     (0)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_REVISIONID_user_id_END       (7)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_REVISIONID_flexnoc_id_START  (8)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_ID_REVISIONID_flexnoc_id_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VPU_QOSGENERATOR_PRIORITY_UNION
 结构说明  : VPU_QOSGENERATOR_PRIORITY 寄存器结构定义。地址偏移量:0x0308，初值:0x80000000，宽度:32
 寄存器说明: VPU QoS Generator优先级寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0       : 3;  /* bit[0-2]  : 在 Regulator 模式，定义了低 hurry 值；在 Fixed/limiter 模式，定义了写 urgency 值 */
        unsigned int  reserved_0: 5;  /* bit[3-7]  : 保留 */
        unsigned int  p1       : 3;  /* bit[8-10] : 在 Regulator 模式，定义了高 hurry 值；在 Fixed/limiter 模式，定义了读 urgency 值 */
        unsigned int  reserved_1: 20; /* bit[11-30]: 保留 */
        unsigned int  mark     : 1;  /* bit[31-31]: 向后兼容标志（为0时） */
    } reg;
} SOC_MEDIA_NOC_VPU_QOSGENERATOR_PRIORITY_UNION;
#endif
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_PRIORITY_p0_START        (0)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_PRIORITY_p0_END          (2)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_PRIORITY_p1_START        (8)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_PRIORITY_p1_END          (10)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_PRIORITY_mark_START      (31)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_PRIORITY_mark_END        (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VPU_QOSGENERATOR_MODE_UNION
 结构说明  : VPU_QOSGENERATOR_MODE 寄存器结构定义。地址偏移量:0x030C，初值:0x00000000，宽度:32
 寄存器说明: VPU QoS Generator 模式寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mode     : 2;  /* bit[0-1] : 模式选择：0—Fixed；1—Limiter；2—Bypass；3—Regulator */
        unsigned int  reserved : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_VPU_QOSGENERATOR_MODE_UNION;
#endif
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_MODE_mode_START      (0)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_MODE_mode_END        (1)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VPU_QOSGENERATOR_BANDWIDTH_UNION
 结构说明  : VPU_QOSGENERATOR_BANDWIDTH 寄存器结构定义。地址偏移量:0x0310，初值:0x0000016C，宽度:32
 寄存器说明: VPU QoS Generator 带宽阈值寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bandwidth : 12; /* bit[0-11] : 带宽阈值 */
        unsigned int  reserved  : 20; /* bit[12-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_VPU_QOSGENERATOR_BANDWIDTH_UNION;
#endif
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_BANDWIDTH_bandwidth_START  (0)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_BANDWIDTH_bandwidth_END    (11)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VPU_QOSGENERATOR_SATURATION_UNION
 结构说明  : VPU_QOSGENERATOR_SATURATION 寄存器结构定义。地址偏移量:0x0314，初值:0x00000080，宽度:32
 寄存器说明: VPU QoS Generator 饱和量寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  saturation : 10; /* bit[0-9]  : 带宽计数器大小 */
        unsigned int  reserved   : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_VPU_QOSGENERATOR_SATURATION_UNION;
#endif
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_SATURATION_saturation_START  (0)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_SATURATION_saturation_END    (9)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VPU_QOSGENERATOR_EXTCONTROL_UNION
 结构说明  : VPU_QOSGENERATOR_EXTCONTROL 寄存器结构定义。地址偏移量:0x0318，初值:0x00000000，宽度:32
 寄存器说明: VPU QoS Generator外部控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  socket_qos_en : 1;  /* bit[0]   : 决定采用 socket 的 QoS 值还是 QoS Generator 的 QoS 值 */
        unsigned int  ext_thr_en    : 1;  /* bit[1-1] : 外部阈值代替带宽寄存器设置的阈值控制高低优先级 */
        unsigned int  int_clk_en    : 1;  /* bit[2-2] : 本地时钟（NIU时钟）替代参考时钟进行带宽计算 */
        unsigned int  reserved      : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_VPU_QOSGENERATOR_EXTCONTROL_UNION;
#endif
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_EXTCONTROL_socket_qos_en_START  (0)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_EXTCONTROL_socket_qos_en_END    (0)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_EXTCONTROL_ext_thr_en_START     (1)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_EXTCONTROL_ext_thr_en_END       (1)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_EXTCONTROL_int_clk_en_START     (2)
#define SOC_MEDIA_NOC_VPU_QOSGENERATOR_EXTCONTROL_int_clk_en_END       (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_COREID_UNION
 结构说明  : VPU_SEC_QOSGENERATOR_ID_COREID 寄存器结构定义。地址偏移量:0x0400，初值:0x3F31B604，宽度:32
 寄存器说明: VPU_SEC QoS Generator 的域ID
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core_type_id   : 8;  /* bit[0-7] : VPU_SEC QoS Generator 的域ID */
        unsigned int  core_check_sum : 24; /* bit[8-31]: VPU_SEC QoS Generator 的参数检验和 */
    } reg;
} SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_COREID_UNION;
#endif
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_COREID_core_type_id_START    (0)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_COREID_core_type_id_END      (7)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_COREID_core_check_sum_START  (8)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_COREID_core_check_sum_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_REVISIONID_UNION
 结构说明  : VPU_SEC_QOSGENERATOR_ID_REVISIONID 寄存器结构定义。地址偏移量:0x0404，初值:0x012D5300，宽度:32
 寄存器说明: VPU_SEC QoS Generator总线代码版本寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  user_id    : 8;  /* bit[0-7] : 用户自定义值，不在 IP 内使用 */
        unsigned int  flexnoc_id : 24; /* bit[8-31]: 总线代码版本寄存器 */
    } reg;
} SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_REVISIONID_UNION;
#endif
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_REVISIONID_user_id_START     (0)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_REVISIONID_user_id_END       (7)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_REVISIONID_flexnoc_id_START  (8)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_ID_REVISIONID_flexnoc_id_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_PRIORITY_UNION
 结构说明  : VPU_SEC_QOSGENERATOR_PRIORITY 寄存器结构定义。地址偏移量:0x0408，初值:0x80000000，宽度:32
 寄存器说明: VPU_SEC QoS Generator优先级寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0       : 3;  /* bit[0-2]  : 在 Regulator 模式，定义了低 hurry 值；在 Fixed/limiter 模式，定义了写 urgency 值 */
        unsigned int  reserved_0: 5;  /* bit[3-7]  : 保留 */
        unsigned int  p1       : 3;  /* bit[8-10] : 在 Regulator 模式，定义了高 hurry 值；在 Fixed/limiter 模式，定义了读 urgency 值 */
        unsigned int  reserved_1: 20; /* bit[11-30]: 保留 */
        unsigned int  mark     : 1;  /* bit[31-31]: 向后兼容标志（为0时） */
    } reg;
} SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_PRIORITY_UNION;
#endif
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_PRIORITY_p0_START        (0)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_PRIORITY_p0_END          (2)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_PRIORITY_p1_START        (8)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_PRIORITY_p1_END          (10)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_PRIORITY_mark_START      (31)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_PRIORITY_mark_END        (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_MODE_UNION
 结构说明  : VPU_SEC_QOSGENERATOR_MODE 寄存器结构定义。地址偏移量:0x040C，初值:0x00000000，宽度:32
 寄存器说明: VPU_SEC QoS Generator 模式寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mode     : 2;  /* bit[0-1] : 模式选择：0—Fixed；1—Limiter；2—Bypass；3—Regulator */
        unsigned int  reserved : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_MODE_UNION;
#endif
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_MODE_mode_START      (0)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_MODE_mode_END        (1)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_BANDWIDTH_UNION
 结构说明  : VPU_SEC_QOSGENERATOR_BANDWIDTH 寄存器结构定义。地址偏移量:0x0410，初值:0x0000016C，宽度:32
 寄存器说明: VPU_SEC QoS Generator 带宽阈值寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bandwidth : 12; /* bit[0-11] : 带宽阈值 */
        unsigned int  reserved  : 20; /* bit[12-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_BANDWIDTH_UNION;
#endif
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_BANDWIDTH_bandwidth_START  (0)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_BANDWIDTH_bandwidth_END    (11)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_SATURATION_UNION
 结构说明  : VPU_SEC_QOSGENERATOR_SATURATION 寄存器结构定义。地址偏移量:0x0414，初值:0x00000080，宽度:32
 寄存器说明: VPU_SEC QoS Generator 饱和量寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  saturation : 10; /* bit[0-9]  : 带宽计数器大小 */
        unsigned int  reserved   : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_SATURATION_UNION;
#endif
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_SATURATION_saturation_START  (0)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_SATURATION_saturation_END    (9)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_EXTCONTROL_UNION
 结构说明  : VPU_SEC_QOSGENERATOR_EXTCONTROL 寄存器结构定义。地址偏移量:0x0418，初值:0x00000000，宽度:32
 寄存器说明: VPU_SEC QoS Generator外部控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  socket_qos_en : 1;  /* bit[0]   : 决定采用 socket 的 QoS 值还是 QoS Generator 的 QoS 值 */
        unsigned int  ext_thr_en    : 1;  /* bit[1-1] : 外部阈值代替带宽寄存器设置的阈值控制高低优先级 */
        unsigned int  int_clk_en    : 1;  /* bit[2-2] : 本地时钟（NIU时钟）替代参考时钟进行带宽计算 */
        unsigned int  reserved      : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_EXTCONTROL_UNION;
#endif
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_EXTCONTROL_socket_qos_en_START  (0)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_EXTCONTROL_socket_qos_en_END    (0)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_EXTCONTROL_ext_thr_en_START     (1)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_EXTCONTROL_ext_thr_en_END       (1)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_EXTCONTROL_int_clk_en_START     (2)
#define SOC_MEDIA_NOC_VPU_SEC_QOSGENERATOR_EXTCONTROL_int_clk_en_END       (2)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_COREID_UNION
 结构说明  : VIDEO_ADAPTER_ID_COREID 寄存器结构定义。地址偏移量:0x1000，初值:0x7714BE01，宽度:32
 寄存器说明: VIDEO adapter域ID
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core_type_id   : 8;  /* bit[0-7] : 访问SMMU通路的RateAdapter的域ID */
        unsigned int  core_check_sum : 24; /* bit[8-31]: 访问SMMU通路的RateAdapter的参数检验和 */
    } reg;
} SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_COREID_UNION;
#endif
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_COREID_core_type_id_START    (0)
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_COREID_core_type_id_END      (7)
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_COREID_core_check_sum_START  (8)
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_COREID_core_check_sum_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_REVISIONID_UNION
 结构说明  : VIDEO_ADAPTER_ID_REVISIONID 寄存器结构定义。地址偏移量:0x1004，初值:0x012D5300，宽度:32
 寄存器说明: 总线代码版本寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  user_id    : 8;  /* bit[0-7] : 用户自定义值，不在 IP 内使用 */
        unsigned int  flexnoc_id : 24; /* bit[8-31]: 总线代码版本寄存器 */
    } reg;
} SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_REVISIONID_UNION;
#endif
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_REVISIONID_user_id_START     (0)
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_REVISIONID_user_id_END       (7)
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_REVISIONID_flexnoc_id_START  (8)
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_ID_REVISIONID_flexnoc_id_END    (31)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VIDEO_ADAPTER_RATE_UNION
 结构说明  : VIDEO_ADAPTER_RATE 寄存器结构定义。地址偏移量:0x1008，初值:0x00000000，宽度:32
 寄存器说明: VIDEO Rate Adapter配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rate     : 10; /* bit[0-9]  : 带宽适配调解：
                                                    [7：0]：小数部分；
                                                    [9：8]：整数部分；
                                                    计算公式：Rate=1/（ratio-1），ratio=outgoing_throughput/incoming_throughput；
                                                    例如：ratio=2，那么Rate=1=0x1.00；ratio=11/3，Rate=3/8=0x0.60； */
        unsigned int  reserved : 22; /* bit[10-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_VIDEO_ADAPTER_RATE_UNION;
#endif
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_RATE_rate_START      (0)
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_RATE_rate_END        (9)


/*****************************************************************************
 结构名    : SOC_MEDIA_NOC_VIDEO_ADAPTER_BYPASS_UNION
 结构说明  : VIDEO_ADAPTER_BYPASS 寄存器结构定义。地址偏移量:0x100C，初值:0x00000000，宽度:32
 寄存器说明: bypass VIDEO Rate Adapter
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bypass   : 1;  /* bit[0]   : bypass VIDEO Rate Adapter */
        unsigned int  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MEDIA_NOC_VIDEO_ADAPTER_BYPASS_UNION;
#endif
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_BYPASS_bypass_START    (0)
#define SOC_MEDIA_NOC_VIDEO_ADAPTER_BYPASS_bypass_END      (0)






/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/



/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of soc_media_noC_interface.h */
