
#include <linux/printk.h>
#include <linux/spinlock.h>
#include <linux/hisi/hi6xxx_ipc_enum.h>
//#include "mdrv_temp_cfg.h"
#include "hi6xxx_hkadc_value.h"
/*#include "NvIdList.h"*/

#ifdef _DRV_LLT_
//#include "stdio.h"
#endif

extern unsigned int omTimerGet(void);
extern void BSP_IPC_SpinLock (unsigned int u32SignalNum);
extern void BSP_IPC_SpinUnLock (unsigned int u32SignalNum);

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

int g_hkadc_map_fill_flag = HKADC_FALSE;

struct drv_hkadc_map g_hkadc_chan_map[] = {
    {HKADC_TEMP_BATTERY,    (unsigned int)HKADC_CHAN_INVALID,   0, 28, 0, "battery"},
    {HKADC_TEMP_PA0,        (unsigned int)HKADC_CHAN_INVALID,   0, 32, 0, "pa0"},
    {HKADC_TEMP_PA1,        (unsigned int)HKADC_CHAN_INVALID,   0, 32, 1, "pa1"},
    {HKADC_TEMP_DCXO0,      (unsigned int)HKADC_CHAN_INVALID,   0, 32, 0, "dcxo_0"},
//    {HKADC_TEMP_DCXO0_LOW,  HKADC_CHAN_INVALID,   en_NV_Item_XO_LOW_TEMP_SENSOR_TABLE,  32, 0, "dcxo_0_low"},
//    {HKADC_TEMP_DCXO0_LOW,	HKADC_CHAN_INVALID,   en_NV_Item_XO_TEMP_SENSOR_TABLE,  32, 0, "dcxo_0_low"},
    {HKADC_TEMP_DCXO1,      (unsigned int)HKADC_CHAN_INVALID,   0,  32, 1, "dcxo_1"},
//    {HKADC_TEMP_DCXO1_LOW,  HKADC_CHAN_INVALID,   en_NV_Item_XO_TEMP_SENSOR_TABLE,  32, 1, "dcxo_1_low"},
    {HKADC_TEMP_LCD,			(unsigned int)HKADC_CHAN_INVALID,   0, 0, 0, "lcd"},
    {HKADC_TEMP_SIM_CARD,	(unsigned int)HKADC_CHAN_INVALID,   0, 0, 0, "sim_card1"},
    {HKADC_TEMP_SYSTEMH,		(unsigned int)HKADC_CHAN_INVALID,   0, 0, 0, "systemh"},
    {HKADC_TEMP_SYSTEML,		(unsigned int)HKADC_CHAN_INVALID,   0, 0, 0, "systeml"},
    {HKADC_TEMP_BATT_ID,		(unsigned int)HKADC_CHAN_INVALID,   0, 0, 0, "batt_id"},
    {HKADC_TEMP_CAMERA0_ID,	(unsigned int)HKADC_CHAN_INVALID,   0, 0, 0, "camera0_id"},
    {HKADC_TEMP_CAMERA1_ID,	(unsigned int)HKADC_CHAN_INVALID,   0, 0, 0, "camera1_id"},
    {HKADC_TEMP_CODEC,	    (unsigned int)HKADC_CHAN_INVALID,   0, 0, 0, "codec"},
    {HKADC_TEMP_CHARGER_ID,	(unsigned int)HKADC_CHAN_INVALID,   0, 0, 0, "charger"},
    {HKADC_TEMP_BUTT,		(unsigned int)HKADC_CHAN_INVALID,   0, 0, 0, "buttom"}
};

/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*****************************************************************************
 函 数 名  : hkadc_fill_chan_map
 功能描述  : 填充g_hkadc_chan_map表中的物理通道ID部分
 输入参数  :
 输出参数  : 无
 返 回 值  : BSP_S32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月13日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
int hkadc_fill_chan_map(unsigned short *p2l_tbl)
{
    int logic_id = 0;
    int phy_id = 0;
    struct drv_hkadc_map * chan_map = NULL;
    int ret = HKADC_OK;

    for (phy_id=HKADC_CHAN_0; phy_id<HKADC_CHAN_MAX; phy_id++) {
        logic_id = p2l_tbl[phy_id];
        chan_map =  bsp_hkadc_get_chan_map(logic_id);
        if (HKADC_NULL != chan_map) {
            HKADC_DEBUG(("hkadc_fill_chan_map logic_id->phyID: %d -> %d \n",
                            logic_id, phy_id));
            chan_map->phy_chan = phy_id;
            ret = HKADC_ERROR;
        } else {
            HKADC_DEBUG(( "hkadc_fill_chan_map get map ERROR: logic_id %d \n",
                            logic_id));
            ret = HKADC_ERROR;
        }
    }

    return ret;
}

int hkadc_chan_map_renew(unsigned short *l2p_tbl)
{
    int logic_id = 0;
    int phy_id = 0;
    struct drv_hkadc_map * chan_map = NULL;
    int ret = HKADC_OK;

    for (logic_id=HKADC_TEMP_TOP; logic_id<HKADC_TEMP_BUTT; logic_id++) {
        phy_id = l2p_tbl[logic_id];
        if (INVALID_HKADC_PHYVALUE != phy_id) {
            chan_map=  bsp_hkadc_get_chan_map(logic_id);
            if (HKADC_NULL != chan_map) {
                HKADC_DEBUG(("hkadc_fill_chan_map logic_id->phyID: %d -> %d \n",
                                logic_id, phy_id));
                chan_map->phy_chan = phy_id;
            } else {
                HKADC_DEBUG(( "hkadc_fill_chan_map get map ERROR: logic_id %d \n",
                                logic_id));
                ret = HKADC_ERROR;
            }
        }
    }

    return ret;
}

/*****************************************************************************
 函 数 名  : bsp_hkadc_get_chan_map
 功能描述  : 获取某个逻辑通道的数据资料映射表
 输入参数  :
            enMode : logic_chan 设备逻辑通道号
 输出参数  :
 返 回 值  :
            struct drv_hkadc_map * 指向某个hkadc映射表的指针
*****************************************************************************/
struct drv_hkadc_map *bsp_hkadc_get_chan_map(int logic_chan)
{
    struct drv_hkadc_map *map = &g_hkadc_chan_map[0];

    /*遍历表数组，如果发现对应逻辑通道的映射表，返回其指针*/
    while (HKADC_TEMP_BUTT != map->logic_chan) {
        if (map->logic_chan == logic_chan) {
            return map;
        }
        map++;
     }

    return HKADC_NULL;
}

void hkadc_chann_map_show(void)
{
    struct drv_hkadc_map *map = &g_hkadc_chan_map[0];

    HKADC_DEBUG(("\nlogic_chan  phy_chan  nv_id  nv_len  modem_id name \n"));
    while (HKADC_TEMP_BUTT != map->logic_chan) {
       HKADC_DEBUG(("%8d %8d %8d %8d %8d %8s\n",map->logic_chan,map->phy_chan,
       map->nv_id, map->nv_len,map->modem_id,map->name));

	map++;
    }
}

/*****************************************************************************
函 数 名  : hkadc_volttemp_trans
功能描述  :
输入参数  :
                      ucChan       :通道值
                      usVoltage    :电压
                      hkAdcTable   :电压与其余数据的换算表
                      tempTableLen :hkadc电压温度转换表的长度
输出参数  :
           N/A
返 回 值  :
           换算完的数据
*****************************************************************************/
short hkadc_volttemp_trans(unsigned char ucChan, unsigned short usVoltage, unsigned short *hkAdcTable, unsigned short tempTableLen)
{
    unsigned short usTemNum = 0x00;          /*记录温度个数*/
    short RetVal = 0x00;

    struct hkadc_tem_value *pstTemVoltTAble = (struct hkadc_tem_value *)hkAdcTable;
    struct hkadc_tem_value *pstTemp = (struct hkadc_tem_value *)hkAdcTable;

    /*need convert*/
    if (!hkAdcTable) {
        /*直接返回电压*/
        return TEMP_VALUE_INVALID;
    }

    for (usTemNum=0; usTemNum<tempTableLen; usTemNum++) {
        if((0x00 == pstTemp->volt) && (0x00 == pstTemp->temp)) {
            break;
        }
        pstTemp++;
    }

    /*NV项无效，返回无效值*/
    if ((0x00 == pstTemVoltTAble->temp) && (0x00 == pstTemVoltTAble->volt)) {
        HKADC_DEBUG(("HKADC ch %d NV invalid !\n", ucChan));
        return TEMP_VALUE_INVALID;
    }
    /*NV项数据合法*/

    /*判断极端值*/
    if (usVoltage >= pstTemVoltTAble->volt) {
        RetVal =  pstTemVoltTAble->temp;
        return RetVal;
    }

    pstTemp =(struct hkadc_tem_value *)((pstTemVoltTAble + usTemNum) -1);
    if (usVoltage <= pstTemp->volt) {
        RetVal = pstTemp->temp;
        return RetVal;
    }

    /*根据电压进行插值计算*/
    while (usVoltage < pstTemVoltTAble->volt) {
        pstTemVoltTAble++;
    }

    pstTemp = (struct hkadc_tem_value *)(pstTemVoltTAble -1); /*指向前一个元素*/

    RetVal = (short)pstTemp->temp + (short)((((pstTemp->volt -usVoltage) * (pstTemVoltTAble->temp - pstTemp->temp))) \
                    /(pstTemp->volt -pstTemVoltTAble->volt));

    return RetVal;
}


/*****************************************************************************
 函 数 名  : bsp_hkadc_get_shared_value
 功能描述  : 获取某个通道的温度及电压数值
 输入参数  :
            struct drv_hkadc_map * 指向某个hkadc映射表的指针
 输出参数  :
 返 回 值  :
*****************************************************************************/
static int bsp_hkadc_get_shared_value(struct drv_hkadc_map *map,
                    unsigned short* volt,
                    short* temp)
{
    struct hkadc_chan_out * chan_out = g_thermal_share_area->chan_out;
    struct hkadc_chan_config * chan_cfg = g_thermal_share_area->chan_cfg;
    short  volt_l;
    short  volt_r;
    short  temp_l;
    short  temp_r;
    unsigned int phy_chan;
    unsigned int count;
    int ret = HKADC_OK;
    unsigned short voltvalue = 0;
    short tempvalue = 0;
    OUT_PROP_CONFIG outConfig;

    phy_chan = map->phy_chan;

    /*如果通道没有配置，直接返回HKADC_ERROR*/
    if (chan_cfg[phy_chan].have_config == 0)
        return HKADC_ERROR;

    outConfig = chan_cfg[phy_chan].outcfg.bit_out_config;


    /*比较电压的原始值和取反值，如果相符合则返回原始值*/
    count = 5;
    do {
        volt_l = (short)chan_out[phy_chan].volt_l;
        volt_r = (short)chan_out[phy_chan].volt_r;
    } while ((count--) && (~volt_l != volt_r));

    if (~volt_l == volt_r) {
        voltvalue = volt_l;
    } else {
        return HKADC_ERROR;
    }

    if (volt) {
        *volt = voltvalue;
    }

    /*比较温度的原始值和取反值，如果相符合则返回原始值*/
    if (temp) {
        /*如果配置输出温度，直接从共享内存获取温度并返回*/
        if (outConfig.out_vol_temp == 1) {
            count = 5;
            do {
                temp_l = chan_out[phy_chan].temp_l;
                temp_r = chan_out[phy_chan].temp_r;
            } while ((count--) && (~temp_l != temp_r));

            if(~temp_l == temp_r) {
                *temp = temp_l;
            } else {
                ret = HKADC_ERROR;
            }
        }
        /*如果没有配置输出温度，但是上层需要温度，可以通过电压温度转换表直接转换*/
        else {
            unsigned short temptablelen = g_thermal_share_area->chan_cfg[phy_chan].temp_data_len;
            tempvalue = hkadc_volttemp_trans(phy_chan, voltvalue,
                (unsigned short *)g_thermal_share_area->chan_cfg[phy_chan].temp_table, temptablelen);

	     if (tempvalue != TEMP_VALUE_INVALID) {
                *temp = tempvalue;
            } else {
                ret = HKADC_ERROR;
            }
        }
    }

    return ret;
}

int get_voltage_by_hkadc(unsigned char ucChan, unsigned short *voltage)
{
    unsigned int ulTmpVal = 0x00;
    unsigned int count = 0x0;
    unsigned short rd_data0 = 0;
    unsigned short rd_data1 = 0;
    unsigned int num = 0;
    unsigned int slice_diff = 0;
    unsigned long flag;

    spin_lock_irqsave(&g_hkadc_lock,flag);

    BSP_IPC_SpinLock(IPC_SEM_HKADC);

    do
    {
        slice_diff = (omTimerGet() - g_thermal_share_area->last_slice);
    }while(slice_diff < SLICE_ONE_MS);


    /*设置采样通道、转换速率,开启PMU_HKADC*/
    ulTmpVal = HKADC_WR01_VALUE | ucChan;

    /*将要进行的读写操作写入HKADC_IP的寄存器*/
    HKADC_REG_WRITE(HKADC_WR01_DATA_ADDR, ulTmpVal);
    HKADC_REG_WRITE(HKADC_WR23_DATA_ADDR, HKADC_WR23_VALUE);
    HKADC_REG_WRITE(HKADC_WR45_DATA_ADDR, HKADC_WR45_VALUE);
    /*配置总共要进行的读写操作个数*/
    HKADC_REG_WRITE(HKADC_WR_NUM_ADDR, HKADC_WR_NUM_VALUE);
    /*设置5个读写操作之间配置延迟*/
    HKADC_REG_WRITE(HKADC_DELAY01_ADDR, HKADC_DELAY01_VALUE);
    HKADC_REG_WRITE(HKADC_DELAY23_ADDR, HKADC_DELAY23_VALUE);

    /*设置HKADC_IP启动位*/
    HKADC_REG_WRITE(HKADC_START_ADDR, 0x1);

    /*查询HKADC启动位是否清零*/
    num = 0;
    do
    {
        count++;
        /* 超时时间建议为250us */
        if(count > START_DELAY_TIMEOUT)
        {
            /* 配0后退出 */
            HKADC_REG_WRITE(HKADC_START_ADDR, 0);
            printk(KERN_ERR"Read hkadc timeout!\n");
            g_thermal_share_area->last_slice = omTimerGet();
            BSP_IPC_SpinUnLock(IPC_SEM_HKADC);
            spin_unlock_irqrestore(&g_hkadc_lock,flag);
            return HKADC_ERROR;
        }
        HKADC_REG_READ(HKADC_START_ADDR,ulTmpVal);
        if(0 == (ulTmpVal & 0x1))
        {
            if (num < 5)
            {
                num++;
                continue;
            }
            else
            {
                break;
            }
        }
        else
        {
            num = 0;
            continue;
        }
    }while(1);

    HKADC_REG_READ(HKADC_RD2_DATA_ADDR,rd_data0);
    HKADC_REG_READ(HKADC_RD3_DATA_ADDR,rd_data1);

    /*rd_data1为回读数据[11:4]，rd_data0[7:4]为回读数据[3:0]*/
    ulTmpVal = (((rd_data1 << 4) & HKADC_VALUE_HIGH)|((rd_data0 >> 4) & HKADC_VALUE_LOW));

    /*将AD转换为实际电压mv*/
    *voltage = (unsigned short)(((ulTmpVal & HKADC_VALID_VALUE)* HKADC_VREF_1V8)>>12);

    g_thermal_share_area->last_slice = omTimerGet();
    BSP_IPC_SpinUnLock(IPC_SEM_HKADC);
    spin_unlock_irqrestore(&g_hkadc_lock,flag);

    return HKADC_OK;
}

/*****************************************************************************
 函 数 名  : DRV_HKADC_GET_TEMP
 功能描述  : 如果上层输入的enMode无效，或者V3R3不支持，则输出0xFFFF函数直接返回。
             如果pusVolt!=NULL，则输出温度
             如果psTemp!=NULL,则输出电压
 输入参数  :
            enMode : hkadc设备
 输出参数  :
            pusVolt : 通道电压
            psTemp: 通道温度
 返 回 值  :
            HKADC_OK/HKADC_ERROR
*****************************************************************************/
int DRV_HKADC_GET_TEMP(HKADC_TEMP_PROTECT_E log_chan,
                unsigned short* volt,
                short* temp, HKADC_CONV_MODE_E ulMode)
{
    struct drv_hkadc_map *map = NULL;

    /*获取通道映射表*/
    map = bsp_hkadc_get_chan_map(log_chan);

    /*赋初值*/
    if (volt)
        *volt = HKADC_VALUE_INVALID;

    if (temp)
        *temp = HKADC_VALUE_INVALID;

    /*入参判断*/
    if (((0 == volt) && (0 == temp)) || ( HKADC_NULL == map )
        || (map->phy_chan >= HKADC_CHAN_MAX))
    {
        HKADC_DEBUG(("DRV_HKADC_GET_TEMP ERROR param %d, 0x%x, 0x%x \n",
            (int)log_chan, (int)volt, (int)temp));

	 return HKADC_ERROR;
    }

    if( HKADC_TEMP_CODEC == log_chan )
    {
        return get_voltage_by_hkadc(map->phy_chan , volt);
    }

    /*获取温度值*/
	return bsp_hkadc_get_shared_value(map, volt, temp);
}

void hkadc_tsensor_debug_show(void)
{
    unsigned short voltage = 0;
    short temp_battery,temp_systemh,temp_systeml,temp_pa1,temp_pa0;
    int ret;
    ret = DRV_HKADC_GET_TEMP(HKADC_TEMP_BATTERY, &voltage, &temp_battery, 0);	
    ret = DRV_HKADC_GET_TEMP(HKADC_TEMP_SYSTEMH, &voltage, &temp_systemh, 0);
    ret = DRV_HKADC_GET_TEMP(HKADC_TEMP_SYSTEML, &voltage, &temp_systeml, 0);
    ret = DRV_HKADC_GET_TEMP(HKADC_TEMP_PA0, &voltage, &temp_pa0, 0);
    ret = DRV_HKADC_GET_TEMP(HKADC_TEMP_PA1, &voltage, &temp_pa1, 0);

    printk("hw tsensor debug info battery temp [%d]\n", temp_battery);
    printk("hw tsensor debug info systemh temp [%d]\n", temp_systemh);
    printk("hw tsensor debug info systeml temp [%d]\n", temp_systeml);
    printk("hw tsensor debug info PA0 temp [%d]\n", temp_pa0);
    printk("hw tsensor debug info PA1 temp [%d]\n", temp_pa1);   

}


void hkadc_read_test(int channel)
{
    unsigned short voltage = 0;
    short temperature = 0;
    int ret = 0;

    ret = DRV_HKADC_GET_TEMP(channel, &voltage, &temperature, 0);
    if(HKADC_ERROR == ret)
    {
        printk(KERN_ERR"Read HKADC Error.\n");
    }

    printk(KERN_ERR"Voltage=%d, Temperature=%d.\n",voltage,temperature);
}
