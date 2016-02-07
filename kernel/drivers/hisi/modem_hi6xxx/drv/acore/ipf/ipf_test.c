#ifdef CONFIG_ARM64
#include <soc_ao_sctrl_interface.h>
#endif
#include "ipf_test.h"
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#include "bsp_mem_drv.h"
#include <bsp_mem.h>
#include "ipf_driver.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


static BSP_U8 *pu8ShortIpData = BSP_NULL;
static BSP_U8 *pu8LongIpData = BSP_NULL;
static IPF_CONFIG_ULPARAM_S *pstShortUlPara = BSP_NULL;
static IPF_CONFIG_ULPARAM_S *pstLongUlPara = BSP_NULL;
static BSP_U8 pu8TestData[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static IpfStResultCheck_S stResultCheck = {0};
static int g_IpfDlAdqNullTaskId = 0;
static struct semaphore g_IpfDlTaskSemId;
static struct semaphore g_IpfDlIntResultSemId;
static struct semaphore g_IpfDlIntAdqNullSemId;
static struct semaphore g_IpfDlIntAdq0NullSemId;
static struct semaphore g_IpfDlIntAdq1NullSemId;
static struct task_struct * g_IpfDlTaskId;

#define ipfTestDelay(a)     msleep((a)*10)
#define SHORT_IP_LEN        (400)
#define LONG_IP_LEN         (1000)
#define ADQ0_LEN            (448)
#define ADQ1_LEN            (1536)
#define IPF_TEST_ENTRY()  \
    do{ \
        pu8ShortIpData = (BSP_U8*)BSP_SMalloc(SHORT_IP_LEN, MEM_ICC_DDR_POOL); \
        if(BSP_NULL == pu8ShortIpData) \
        { \
            IPF_PRINT_ERROR("TESTERROR line=%d   FAILED\n", __LINE__); \
            goto err; \
        } \
        pstShortUlPara = (IPF_CONFIG_ULPARAM_S*)kmalloc(IPF_ULBD_DESC_SIZE * sizeof(IPF_CONFIG_ULPARAM_S), GFP_KERNEL); \
        if(BSP_NULL == pstShortUlPara) \
        { \
            IPF_PRINT_ERROR("TESTERROR line=%d   FAILED\n", __LINE__); \
            goto err; \
        } \
        pu8LongIpData = (BSP_U8*)BSP_SMalloc(LONG_IP_LEN, MEM_ICC_DDR_POOL); \
        if(BSP_NULL == pu8LongIpData) \
        { \
            IPF_PRINT_ERROR("TESTERROR line=%d   FAILED\n", __LINE__); \
            goto err; \
        } \
        pstLongUlPara = (IPF_CONFIG_ULPARAM_S*)kmalloc(IPF_ULBD_DESC_SIZE * sizeof(IPF_CONFIG_ULPARAM_S), GFP_KERNEL); \
        if(BSP_NULL == pstLongUlPara) \
        { \
            IPF_PRINT_ERROR("TESTERROR line=%d   FAILED\n", __LINE__); \
            goto err; \
        } \
    }while(0)
#define IPF_TEST_LEAVE() \
    do{ \
        if(BSP_NULL != pu8ShortIpData) \
        { \
            BSP_SFREE(pu8ShortIpData); \
            pu8ShortIpData = BSP_NULL; \
        } \
        if(BSP_NULL != pu8LongIpData) \
        { \
            BSP_SFREE(pu8LongIpData); \
            pu8LongIpData = BSP_NULL; \
        } \
        if(BSP_NULL != pstShortUlPara) \
        { \
            kfree(pstShortUlPara); \
            pstShortUlPara = BSP_NULL; \
        } \
        if(BSP_NULL != pstLongUlPara) \
        { \
            kfree(pstLongUlPara); \
            pstLongUlPara = BSP_NULL; \
        } \
    }while(0)
extern BSP_CHAR *g_pSyncState;
/********************************************************************
* 函数声明
********************************************************************/
static BSP_S32 IPF_ST_ConfigDlADQ0(BSP_U32 u32AdNum);
static BSP_S32 IPF_ST_ConfigDlADQ1(BSP_U32 u32AdNum);

/********************************************************************
* 函数实现
********************************************************************/
static void msDelay(unsigned int ms)
{
    unsigned int i =0;
    unsigned int Base=25165;
    unsigned int count;

    count = ms*Base;
    while(count--)
    {
        i++;
    }
}
static BSP_S32 IpfDlIntResultCb(BSP_VOID)
{
    up(&g_IpfDlIntResultSemId);
    return OK;
}
static BSP_S32 IpfDlIntAdq0NullCb(IPF_ADQ_EMPTY_E eAdqEmpty)
{
    if(IPF_EMPTY_ADQ0 == eAdqEmpty)
    {
        up(&g_IpfDlIntAdq0NullSemId);
        return OK;
    }
    IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
    return ERROR;
}
static BSP_S32 IpfDlIntAdq1NullCb(IPF_ADQ_EMPTY_E eAdqEmpty)
{
    if(IPF_EMPTY_ADQ1 == eAdqEmpty)
    {
        up(&g_IpfDlIntAdq1NullSemId);
        return OK;
    }
    IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
    return ERROR;
}
static BSP_S32 IpfDlIntAdqNullCb(BSP_VOID)
{
    up(&g_IpfDlIntAdqNullSemId);

    return OK;
}
static void IPF_DL_AdqNull_Task(void)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;

    while(1)
    {
        down(&g_IpfDlIntAdqNullSemId);

        s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
        if(IPF_SUCCESS != s32Ret)
        {
            IPF_PRINT_ERROR("TESTERROR line=%d s32Ret = %d\n", __LINE__, s32Ret);
            return;
        }
        else
        {
            IPF_ST_ConfigDlADQ0(u32Adq0Num);
            IPF_ST_ConfigDlADQ1(u32Adq1Num);
        }
    }
}
static IPF_RD_DESC_S g_stIpfRd[IPF_DLRD_DESC_SIZE];
static BSP_VOID IPF_DL_Task(void *param)
{
    IpfStResultCheck_S *pstResultCheck = (IpfStResultCheck_S*)param;
    BSP_U32 u32Num = 0;
    IPF_RD_DESC_S *stRd = &g_stIpfRd[0];
    TTF_MEM_ST* pstTtf = BSP_NULL;
    BSP_U32 i = 0;
    while(1)
    {
        hisi_io_memset((void*)stRd, 0x0, (sizeof(IPF_RD_DESC_S) * IPF_DLRD_DESC_SIZE));
        u32Num = IPF_DLRD_DESC_SIZE;

        down(&g_IpfDlIntResultSemId);
        BSP_IPF_GetDlRd(&u32Num, stRd);
        pstResultCheck->u32RdNum += u32Num;
        IPF_PRINT_ERROR("u32Num = %d\n", u32Num);

        for(i = 0; i < u32Num; i++)
        {
            if(0 != (stRd[i].u32OutPtr))
            {
                pstTtf = (TTF_MEM_ST*)(HISI_VA_ADDRESS(stRd[i].u32OutPtr));
                BSP_SFREE(pstTtf->pData);
                BSP_SFREE(pstTtf);
            }
            else
            {
                IPF_PRINT_ERROR("TESTERROR line=%d, i = %d\n", __LINE__, i);
                return;
            }

            if(stRd[i].u16Result != pstResultCheck->u16Result)
            {
                IPF_PRINT_ERROR("TESTERROR line=%d, u16Result = 0x%x(expect) 0x%x(fact), i = %d \n",__LINE__,
                            pstResultCheck->u16Result, stRd[i].u16Result, i);
            }
            if(stRd[i].u16UsrField1 != pstResultCheck->u16UsrField1)
            {
                IPF_PRINT_ERROR("TESTERROR line=%d, u16UsrField1 = 0x%x(expect) 0x%x(fact), i = %d \n",__LINE__,
                            pstResultCheck->u16UsrField1, stRd[i].u16UsrField1, i);
            }
            if(stRd[i].u32UsrField2 != pstResultCheck->u32UsrField2)
            {
                IPF_PRINT_ERROR("TESTERROR line=%d, u16UsrField2 = 0x%x(expect) 0x%x(fact), i = %d \n",__LINE__,
                            pstResultCheck->u32UsrField2, stRd[i].u32UsrField2, i);
            }
            if(stRd[i].u32UsrField3 != pstResultCheck->u32UsrField3)
            {
                IPF_PRINT_ERROR("TESTERROR line=%d, u32UsrField3 = 0x%x(expect) 0x%x(fact), i = %d \n",__LINE__,
                            pstResultCheck->u32UsrField3, stRd[i].u32UsrField3, i);
            }
        }

        up(&g_IpfDlTaskSemId);
    }
}

static BSP_VOID IPF_DL_Task_Fn(void *param)
{
    BSP_U32 i = 0;
    IpfStResultCheck_S *pstResultCheck = (IpfStResultCheck_S*)param;
    BSP_U32 u32Num = 0;
    IPF_RD_DESC_S *stRd = &g_stIpfRd[0];
    TTF_MEM_ST* pstTtf = BSP_NULL;
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;

    while(1)
    {
        hisi_io_memset((void*)stRd, 0x0, (sizeof(IPF_RD_DESC_S) * IPF_DLRD_DESC_SIZE));
        u32Num = IPF_DLRD_DESC_SIZE;

        down(&g_IpfDlIntResultSemId);
        BSP_IPF_GetDlRd(&u32Num, stRd);
        pstResultCheck->u32RdNum += u32Num;
        IPF_PRINT_ERROR("u32Num = %d\n", u32Num);

        for(i = 0; i < u32Num; i++)
        {
            if(0 != (stRd[i].u32OutPtr))
            {
                pstTtf = (TTF_MEM_ST*)(HISI_VA_ADDRESS(stRd[i].u32OutPtr));
                BSP_SFREE(pstTtf->pData);
                BSP_SFREE(pstTtf);
            }
            else
            {
                IPF_PRINT_ERROR("TESTERROR line=%d, i = %d\n", __LINE__, i);
                return;
            }

            if(stRd[i].u16Result != pstResultCheck->u16Result)
            {
                IPF_PRINT_ERROR("TESTERROR line=%d, u16Result = 0x%x(expect) 0x%x(fact), i = %d \n",__LINE__,
                            pstResultCheck->u16Result, stRd[i].u16Result, i);
            }
            if(stRd[i].u16UsrField1 != pstResultCheck->u16UsrField1)
            {
                IPF_PRINT_ERROR("TESTERROR line=%d, u16UsrField1 = 0x%x(expect) 0x%x(fact), i = %d \n",__LINE__,
                            pstResultCheck->u16UsrField1, stRd[i].u16UsrField1, i);
            }
            if(stRd[i].u32UsrField2 != pstResultCheck->u32UsrField2)
            {
                IPF_PRINT_ERROR("TESTERROR line=%d, u16UsrField2 = 0x%x(expect) 0x%x(fact), i = %d \n",__LINE__,
                            pstResultCheck->u32UsrField2, stRd[i].u32UsrField2, i);
            }
            if(stRd[i].u32UsrField3 != pstResultCheck->u32UsrField3)
            {
                IPF_PRINT_ERROR("TESTERROR line=%d, u32UsrField3 = 0x%x(expect) 0x%x(fact), i = %d \n",__LINE__,
                            pstResultCheck->u32UsrField3, stRd[i].u32UsrField3, i);
            }
        }

        s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
        if(IPF_SUCCESS != s32Ret)
        {
            IPF_PRINT_ERROR("TESTERROR line=%d, s32Ret = %d\n", __LINE__, s32Ret);
        }
        else
        {
            IPF_ST_ConfigDlADQ0(u32Adq0Num);
            IPF_ST_ConfigDlADQ1(u32Adq1Num);
        }

        up(&g_IpfDlTaskSemId);
    }
}

static BSP_VOID IPF_DL_Task_Speed(void *param)
{
    IpfStResultCheck_S *pstResultCheck = (IpfStResultCheck_S*)param;
    BSP_U32 u32Num = 0;
    IPF_RD_DESC_S *stRd = &g_stIpfRd[0];
    TTF_MEM_ST* pstTtf = BSP_NULL;
    BSP_U32 i = 0;
    while(1)
    {
        hisi_io_memset((void*)stRd, 0x0, (sizeof(IPF_RD_DESC_S) * IPF_DLRD_DESC_SIZE));
        u32Num = IPF_DLRD_DESC_SIZE;

        down(&g_IpfDlIntResultSemId);

        BSP_IPF_GetDlRd(&u32Num, stRd);
        pstResultCheck->u32RdNum += u32Num;

        for(i = 0; i < u32Num; i++)
        {
            if(0 != (stRd[i].u32OutPtr))
            {
                pstTtf = (TTF_MEM_ST*)(HISI_VA_ADDRESS(stRd[i].u32OutPtr));
                BSP_SFREE(pstTtf->pData);
                BSP_SFREE(pstTtf);
            }
            else
            {
                IPF_PRINT_ERROR("TESTERROR line=%d, i = %d\n", __LINE__, i);
                return;
            }
        }

        up(&g_IpfDlTaskSemId);
    }
}
/* test BSP_IPF_ConfigTimeout */
static BSP_S32 IPF_ST_ConfigTimeout(BSP_U32 u32Timeout)
{
    BSP_S32 s32ret = 0;
    s32ret = BSP_IPF_ConfigTimeout(u32Timeout);
    if(s32ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d, u32Timeout :%d\n",__LINE__, u32Timeout);
        return BSP_ERROR;
    }
    if(*(BSP_U32*)(SOC_IPF_TIME_OUT_ADDR(IPF_REGBASE_ADR)) != (0x10000 | u32Timeout))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d, u32Timeout :%d\n",__LINE__, u32Timeout);
        return BSP_ERROR;
    }
    return BSP_OK;
}
static BSP_S32 IPF_ST_TimeoutCheck(BSP_U32 u32Timeout)
{
    BSP_S32 s32ret = 0;
    s32ret = BSP_IPF_ConfigTimeout(u32Timeout);
    if(s32ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d, u32Timeout :%d\n",__LINE__, u32Timeout);
        return BSP_ERROR;
    }
    return BSP_OK;
}
static BSP_S32 IPF_ST_ConfigUlShortPacket(IpfStModemId_E eModemId, BSP_U32 u32BdNum, BSP_BOOL bReportFlag)
{
    BSP_U32 i = 0;
    StreamIPv4Header stIPHeader = {0};
    StreamUDP stUDP = {0};
    BSP_S32 s32Ret = 0;

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = SHORT_IP_LEN;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;

    hisi_io_memset(pu8ShortIpData, 0x0, SHORT_IP_LEN);
    hisi_io_memcpy(pu8ShortIpData, &stIPHeader, sizeof(StreamIPv4Header));
    hisi_io_memcpy(pu8ShortIpData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
    hisi_io_memcpy(pu8ShortIpData+sizeof(StreamIPv4Header)+sizeof(StreamUDP), pu8TestData, (sizeof(pu8TestData)-1));

    pstShortUlPara[0].u16Len = SHORT_IP_LEN;
    pstShortUlPara[0].u16Attribute = (IPF_MODEM0_ULFC <<4) | 0x0;
    pstShortUlPara[0].u32Data = HISI_PA_ADDRESS(pu8ShortIpData);
    pstShortUlPara[0].u16Reserved = 0;
    pstShortUlPara[0].u16UsrField1 = eModemId;
    pstShortUlPara[0].u32UsrField2 = 0x5A5A5A5A;
    pstShortUlPara[0].u32UsrField3 = 0x55AA55AA;
    for(i = 1; i < u32BdNum; i++)
    {
        hisi_io_memcpy(&pstShortUlPara[i], &pstShortUlPara[0], sizeof(IPF_CONFIG_ULPARAM_S));
    }

    if(BSP_TRUE == bReportFlag)
    {
        pstShortUlPara[--i].u16Attribute = (IPF_MODEM0_ULFC <<4) | 0x1;;
    }
    s32Ret = BSP_IPF_ConfigUpFilter(u32BdNum, pstShortUlPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    return OK;
}
static BSP_S32 IPF_ST_ConfigUlLongPacket(IpfStModemId_E eModemId, BSP_U32 u32BdNum, BSP_BOOL bReportFlag)
{
    BSP_U32 i = 0;
    StreamIPv4Header stIPHeader = {0};
    StreamUDP stUDP = {0};
    BSP_S32 s32Ret = 0;

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = LONG_IP_LEN;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;

    hisi_io_memset(pu8LongIpData, 0x0, LONG_IP_LEN);
    hisi_io_memcpy(pu8LongIpData, &stIPHeader, sizeof(StreamIPv4Header));
    hisi_io_memcpy(pu8LongIpData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
    hisi_io_memcpy(pu8LongIpData+sizeof(StreamIPv4Header)+sizeof(StreamUDP), pu8TestData, (sizeof(pu8TestData)-1));

    pstLongUlPara[0].u16Len = LONG_IP_LEN;
    pstLongUlPara[0].u16Attribute = (IPF_MODEM0_ULFC <<4) | 0x0;
    pstLongUlPara[0].u32Data = HISI_PA_ADDRESS(pu8LongIpData);
    pstLongUlPara[0].u16Reserved = 0;
    pstLongUlPara[0].u16UsrField1 = eModemId;
    pstLongUlPara[0].u32UsrField2 = 0x5A5A5A5A;
    pstLongUlPara[0].u32UsrField3 = 0x55AA55AA;
    for(i = 1; i < u32BdNum; i++)
    {
        hisi_io_memcpy(&pstLongUlPara[i], &pstLongUlPara[0], sizeof(IPF_CONFIG_ULPARAM_S));
    }

    if(BSP_TRUE == bReportFlag)
    {
        pstLongUlPara[--i].u16Attribute = (IPF_MODEM0_ULFC <<4) | 0x1;
    }
    s32Ret = BSP_IPF_ConfigUpFilter(u32BdNum, pstLongUlPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    return OK;
}
static BSP_S32 IPF_ST_ConfigDlADQ0(BSP_U32 u32AdNum)
{
    BSP_U32 i = 0;
    BSP_S32 s32Ret = 0;
    IPF_AD_DESC_S *pstAdBuffer;
    TTF_MEM_ST* pstDstTtf;

    pstAdBuffer = (IPF_AD_DESC_S*)kmalloc(u32AdNum * sizeof(IPF_AD_DESC_S), GFP_KERNEL);
    for(i = 0; i < u32AdNum; i++)
    {
        pstDstTtf = BSP_SMalloc(sizeof(TTF_MEM_ST), MEM_ICC_DDR_POOL);
        if(pstDstTtf == BSP_NULL)
        {
            IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
            return ERROR;
        }
        pstDstTtf->pData = BSP_SMalloc(ADQ0_LEN, MEM_ICC_DDR_POOL);
        if(pstDstTtf->pData == BSP_NULL)
        {
            IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
            return ERROR;
        }
        pstAdBuffer[i].u32OutPtr0 = HISI_PA_ADDRESS(pstDstTtf->pData);
        pstAdBuffer[i].u32OutPtr1 = HISI_PA_ADDRESS(pstDstTtf);
    }

    s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_0, u32AdNum, pstAdBuffer);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR s32Ret=%d\n", s32Ret);
        return ERROR;
    }
    return OK;
}

static BSP_S32 IPF_ST_ConfigDlADQ1(BSP_U32 u32AdNum)
{
    BSP_U32 i = 0;
    BSP_S32 s32Ret = 0;
    IPF_AD_DESC_S *pstAdBuffer;
    TTF_MEM_ST* pstDstTtf;

    pstAdBuffer = (IPF_AD_DESC_S*)kmalloc(u32AdNum * sizeof(IPF_AD_DESC_S), GFP_KERNEL);
    for(i = 0; i < u32AdNum; i++)
    {
        pstDstTtf = BSP_SMalloc(sizeof(TTF_MEM_ST), MEM_ICC_DDR_POOL);
        if(pstDstTtf == BSP_NULL)
        {
            IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
            return ERROR;
        }
        pstDstTtf->pData = BSP_SMalloc(ADQ1_LEN, MEM_ICC_DDR_POOL);
        if(pstDstTtf->pData == BSP_NULL)
        {
            IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
            return ERROR;
        }
        pstAdBuffer[i].u32OutPtr0 = HISI_PA_ADDRESS(pstDstTtf->pData);
        pstAdBuffer[i].u32OutPtr1 = HISI_PA_ADDRESS(pstDstTtf);
    }

    s32Ret= BSP_IPF_ConfigDlAd(IPF_AD_1, u32AdNum, pstAdBuffer);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR s32Ret=%d\n", s32Ret);
        return ERROR;
    }
    return OK;
}
/****************************************
* 0   两核各自都用的测试用例
* 1   先执行C核后执行A核用例
* 2   C核的测试用例
* 3   先执行A核后执行C核用例
* 4   A/C核同时并发执行的用例
****************************************/
/*验证超时配置*/
BSP_S32 BSP_IPF_ST_001(BSP_VOID)
{
    BSP_U32 u32Timeout = 0;
    BSP_U32 u32OldTime = 0;
    BSP_S32 s32ret = 0;

    IPF_REG_READ(SOC_IPF_TIME_OUT_ADDR(IPF_REGBASE_ADR), u32OldTime);

    s32ret = IPF_ST_TimeoutCheck(u32Timeout);

    u32Timeout = 1;
    s32ret += IPF_ST_ConfigTimeout(u32Timeout);

    u32Timeout = 0xffff;
    s32ret += IPF_ST_ConfigTimeout(u32Timeout);

    u32Timeout = 0x10000;
    s32ret += IPF_ST_TimeoutCheck(u32Timeout);

    IPF_REG_WRITE(SOC_IPF_TIME_OUT_ADDR(IPF_REGBASE_ADR), u32OldTime);

    if(BSP_OK == s32ret)
    {
        IPF_PRINT_ERROR("SUCCESS!\n");
        return BSP_OK;
    }
    else
    {
        return BSP_ERROR;
    }
}

/*验证超时配置*/
BSP_S32 BSP_IPF_ST_002(BSP_VOID)
{
    BSP_S32 s32Ret;
    BSP_U32 u32MaxLen = 0;
    BSP_U32 u32MinLen = 0;

    s32Ret = BSP_IPF_SetPktLen(u32MaxLen,u32MinLen);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",__LINE__);
        return ERROR;
    }

    u32MaxLen = 0x5;
    u32MinLen = 0x6;
    s32Ret = BSP_IPF_SetPktLen(u32MaxLen,u32MinLen);
    if(s32Ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",__LINE__);
        return ERROR;
    }

    u32MaxLen = 0x345;
    u32MinLen = 0x123;
    s32Ret = BSP_IPF_SetPktLen(u32MaxLen,u32MinLen);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",__LINE__);
        return ERROR;
    }
    if(*(BSP_U32*)(SOC_IPF_PKT_LEN_ADDR(IPF_REGBASE_ADR)) != 0x3450123)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",__LINE__);
        return ERROR;
    }

    u32MaxLen = 1500;
    u32MinLen = 40;
    s32Ret = BSP_IPF_SetPktLen(u32MaxLen,u32MinLen);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",__LINE__);
        return ERROR;
    }

    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/*上行ADQ读写计数测试*/
BSP_S32 BSP_IPF_ST_003(BSP_U32 u32Count)
{
    BSP_S32 s32Ret;
    BSP_U32 u32Adq0Num_1 = 0;
    BSP_U32 u32Adq1Num_1 = 0;
    BSP_U32 u32Adq0Num_2 = 0;
    BSP_U32 u32Adq1Num_2 = 0;
    BSP_U32 u32Adq0Num_3 = 0;
    BSP_U32 u32Adq1Num_3 = 0;

    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num_1, &u32Adq1Num_1);
    IPF_PRINT_ERROR("u32Adq0Num_1 = %d, u32Adq1Num_1 = %d\n", u32Adq0Num_1, u32Adq1Num_1);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    s32Ret = IPF_ST_ConfigDlADQ0(u32Count);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num_2, &u32Adq1Num_2);
    IPF_PRINT_ERROR("u32Adq0Num_2 = %d, u32Adq1Num_2 = %d\n", u32Adq0Num_2, u32Adq1Num_2);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    if((u32Adq0Num_2 != (u32Adq0Num_1-u32Count)) || (u32Adq1Num_2 != u32Adq1Num_1))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        IPF_PRINT_ERROR("u32Adq0Num_1 = %d, u32Adq0Num_2 = %d\n", u32Adq0Num_1, u32Adq0Num_2);
        IPF_PRINT_ERROR("u32Adq1Num_1 = %d, u32Adq1Num_2 = %d\n", u32Adq1Num_1, u32Adq1Num_2);
        return ERROR;
    }

    s32Ret = IPF_ST_ConfigDlADQ1(u32Count);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num_3, &u32Adq1Num_3);
    IPF_PRINT_ERROR("u32Adq0Num_3 = %d, u32Adq1Num_3 = %d\n", u32Adq0Num_3, u32Adq1Num_3);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    if((u32Adq0Num_3 != u32Adq0Num_2) || (u32Adq1Num_3 != (u32Adq1Num_2-u32Count)))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        IPF_PRINT_ERROR("u32Adq0Num_2 = %d, u32Adq0Num_3 = %d\n", u32Adq0Num_2, u32Adq0Num_3);
        IPF_PRINT_ERROR("u32Adq1Num_2 = %d, u32Adq1Num_3 = %d\n", u32Adq1Num_2, u32Adq1Num_3);
        return ERROR;
    }

    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/*验证上行ADQ配置*/
BSP_S32 BSP_IPF_ST_004(IPF_AD_TYPE_E eAdType)
{
    BSP_U32 i = 0;
    BSP_S32 s32Ret = 0;
    IPF_AD_DESC_S *pstAdBuffer;
    TTF_MEM_ST* pstDstTtf;

    pstAdBuffer = (IPF_AD_DESC_S*)kmalloc(30 * sizeof(IPF_AD_DESC_S), GFP_KERNEL);
    for(i = 0; i < 30; i++)
    {
        pstDstTtf = BSP_SMalloc(sizeof(TTF_MEM_ST), MEM_ICC_DDR_POOL);
        if(pstDstTtf == BSP_NULL)
        {
            IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
            return ERROR;
        }
        pstDstTtf->pData = BSP_SMalloc(ADQ0_LEN, MEM_ICC_DDR_POOL);
        if(pstDstTtf->pData == BSP_NULL)
        {
            IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
            return ERROR;
        }
        pstAdBuffer[i].u32OutPtr0 = (BSP_U32)HISI_PA_ADDRESS(pstDstTtf->pData);
        pstAdBuffer[i].u32OutPtr1 = (BSP_U32)HISI_PA_ADDRESS(pstDstTtf);
    }
    s32Ret = BSP_IPF_ConfigDlAd(eAdType, 30, pstAdBuffer);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d s32Ret=%d\n", __LINE__, s32Ret);
        return ERROR;
    }
    for(i = 0; i < 30; i++)
    {
	    BSP_SFREE((BSP_VOID*)HISI_VA_ADDRESS(pstAdBuffer[i].u32OutPtr0));
		BSP_SFREE((BSP_VOID*)HISI_VA_ADDRESS(pstAdBuffer[i].u32OutPtr1));
    }
    kfree(pstAdBuffer);

    ipfTestDelay(2);
    pstAdBuffer = (IPF_AD_DESC_S*)kmalloc(30 * sizeof(IPF_AD_DESC_S), GFP_KERNEL);
    for(i = 0; i < 30; i++)
    {
        pstDstTtf = BSP_SMalloc(sizeof(TTF_MEM_ST), MEM_ICC_DDR_POOL);
        if(pstDstTtf == BSP_NULL)
        {
            IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
            return ERROR;
        }
        pstDstTtf->pData = BSP_SMalloc(ADQ0_LEN, MEM_ICC_DDR_POOL);
        if(pstDstTtf->pData == BSP_NULL)
        {
            IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
            return ERROR;
        }
        pstAdBuffer[i].u32OutPtr0 = (BSP_U32)HISI_PA_ADDRESS(pstDstTtf->pData);
        pstAdBuffer[i].u32OutPtr1 = (BSP_U32)HISI_PA_ADDRESS(pstDstTtf);
    }
    s32Ret = BSP_IPF_ConfigDlAd(eAdType, 30, pstAdBuffer);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d s32Ret=%d\n", __LINE__, s32Ret);
        return ERROR;
    }
    for(i = 0; i < 30; i++)
    {
	    BSP_SFREE((BSP_VOID*)HISI_VA_ADDRESS(pstAdBuffer[i].u32OutPtr0));
		BSP_SFREE((BSP_VOID*)HISI_VA_ADDRESS(pstAdBuffer[i].u32OutPtr1));
    }
    kfree(pstAdBuffer);

    ipfTestDelay(2);
    pstAdBuffer = (IPF_AD_DESC_S*)kmalloc(20 * sizeof(IPF_AD_DESC_S), GFP_KERNEL);
    for(i = 0; i < 20; i++)
    {
        pstDstTtf = BSP_SMalloc(sizeof(TTF_MEM_ST), MEM_ICC_DDR_POOL);
        if(pstDstTtf == BSP_NULL)
        {
            IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
            return ERROR;
        }
        pstDstTtf->pData = BSP_SMalloc(ADQ0_LEN, MEM_ICC_DDR_POOL);
        if(pstDstTtf->pData == BSP_NULL)
        {
            IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
            return ERROR;
        }
	    pstAdBuffer[i].u32OutPtr0 = (BSP_U32)HISI_PA_ADDRESS(pstDstTtf->pData);
        pstAdBuffer[i].u32OutPtr1 = (BSP_U32)HISI_PA_ADDRESS(pstDstTtf);
    }
    s32Ret = BSP_IPF_ConfigDlAd(eAdType, 20, pstAdBuffer);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d s32Ret=%d\n", __LINE__, s32Ret);
        return ERROR;
    }
    for(i = 0; i < 20; i++)
    {
	    BSP_SFREE((BSP_VOID*)HISI_VA_ADDRESS(pstAdBuffer[i].u32OutPtr0));
		BSP_SFREE((BSP_VOID*)HISI_VA_ADDRESS(pstAdBuffer[i].u32OutPtr1));
    }
    kfree(pstAdBuffer);

    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/*验证上行通道透传域正常*/
BSP_S32 BSP_IPF_ST_101(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;

    IPF_TEST_ENTRY();
    if(u32PacketNum > (IPF_ULAD0_DESC_SIZE - 1))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n", __LINE__);
        goto err;
    }

    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    if(OK != IPF_ST_ConfigUlShortPacket(IPF_MODEM_0, u32PacketNum, BSP_TRUE))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    ipfTestDelay(200);
    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
/*验证上行通道处理结果中断上报场景*/
BSP_S32 BSP_IPF_ST_102(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;

    IPF_TEST_ENTRY();
    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    if(OK != IPF_ST_ConfigUlShortPacket(IPF_MODEM_0, 1, BSP_TRUE))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    ipfTestDelay(200);
    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
/*验证上行通道处理结果超时中断上报场景*/
BSP_S32 BSP_IPF_ST_103(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;

    IPF_TEST_ENTRY();
    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    if(OK != IPF_ST_ConfigUlShortPacket(IPF_MODEM_0, 1, BSP_FALSE))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    ipfTestDelay(200);
    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
/*验证上行通道ADQ0队列由非空到空的场景*/
BSP_S32 BSP_IPF_ST_104(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;

    IPF_TEST_ENTRY();
    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    if(OK != IPF_ST_ConfigUlShortPacket(IPF_MODEM_0, IPF_ULBD_DESC_SIZE, BSP_TRUE))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    ipfTestDelay(200);
    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
/*验证上行通道ADQ1队列由非空到空的场景*/
BSP_S32 BSP_IPF_ST_105(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;

    IPF_TEST_ENTRY();
    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    if(OK != IPF_ST_ConfigUlLongPacket(IPF_MODEM_0, IPF_ULBD_DESC_SIZE, BSP_TRUE))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    ipfTestDelay(200);
    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
/*模拟上行配置BD，数据流向为modem0的无业务搬移验证*/
BSP_S32 BSP_IPF_ST_108(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;

    IPF_TEST_ENTRY();
    if(u32PacketNum > (IPF_ULAD0_DESC_SIZE - 1))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n", __LINE__);
        goto err;
    }

    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    if(OK != IPF_ST_ConfigUlShortPacket(IPF_MODEM_0, u32PacketNum, BSP_TRUE))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    ipfTestDelay(200);
    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
/*模拟上行配置BD，数据流向为modem0的无业务过滤搬移验证*/
BSP_S32 BSP_IPF_ST_109(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;

    IPF_TEST_ENTRY();
    if(u32PacketNum > (IPF_ULAD0_DESC_SIZE - 1))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n", __LINE__);
        goto err;
    }

    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    if(OK != IPF_ST_ConfigUlShortPacket(IPF_MODEM_0, u32PacketNum, BSP_TRUE))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    ipfTestDelay(200);
    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
/*模拟上行配置BD，数据流向为modem1的无业务过滤搬移验证*/
BSP_S32 BSP_IPF_ST_110(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;

    IPF_TEST_ENTRY();
    if(u32PacketNum > (IPF_ULAD0_DESC_SIZE - 1))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n", __LINE__);
        goto err;
    }

    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    if(OK != IPF_ST_ConfigUlShortPacket(IPF_MODEM_1, u32PacketNum, BSP_TRUE))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    ipfTestDelay(200);
    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
/*验证下行通道透传域正常*/
BSP_S32 BSP_IPF_ST_301(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;

    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_ST_ConfigDlADQ0(u32Adq0Num);


    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task, &stResultCheck, "IpfDlTask");
    IPF_PRINT_ERROR("CCPU 可以输入测试用例\n");

    down(&g_IpfDlTaskSemId);
    if(u32PacketNum != stResultCheck.u32RdNum)
    {
        IPF_PRINT_ERROR("TESTERROR u32RDNum = %d, u32PacketNum = %d, line=%d\n", stResultCheck.u32RdNum, u32PacketNum, __LINE__);
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/*验证下行通道处理结果中断上报场景*/
BSP_S32 BSP_IPF_ST_302(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;

    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    IPF_ST_ConfigDlADQ0(u32Adq0Num);

    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task, &stResultCheck, "IpfDlTask");

    IPF_PRINT_ERROR("CCPU 可以输入测试用例\n");

    down(&g_IpfDlTaskSemId);
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/*验证下行通道处理结果超时中断上报场景*/
BSP_S32 BSP_IPF_ST_303(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;

    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    IPF_ST_ConfigDlADQ0(u32Adq0Num);

    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task, &stResultCheck, "IpfDlTask");

    IPF_PRINT_ERROR("CCPU 可以输入测试用例\n");

    down(&g_IpfDlTaskSemId);
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/*验证下行通道ADQ0队列由非空到空的场景*/
BSP_S32 BSP_IPF_ST_304(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;

    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    IPF_ST_ConfigDlADQ0(u32Adq0Num);

    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task, &stResultCheck, "IpfDlTask");
    sema_init(&g_IpfDlIntAdq0NullSemId, 0);
    s32Ret = BSP_IPF_RegisterAdqEmptyDlCb(IpfDlIntAdq0NullCb);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_PRINT_ERROR("CCPU 可以输入测试用例\n");

    down(&g_IpfDlIntAdq0NullSemId);
    IPF_ST_ConfigDlADQ0(IPF_DLAD0_DESC_SIZE - 1);

    while(IPF_DLBD_DESC_SIZE != stResultCheck.u32RdNum)
    {
        ipfTestDelay(2);
    }
    ipfTestDelay(200);

    IPF_PRINT_ERROR("u32RDNum = %d\n", stResultCheck.u32RdNum);

    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}

/*验证下行通道ADQ1队列由非空到空的场景*/
BSP_S32 BSP_IPF_ST_305(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;

    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    IPF_ST_ConfigDlADQ1(u32Adq1Num);

    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task, &stResultCheck, "IpfDlTask");
    sema_init(&g_IpfDlIntAdq1NullSemId, 0);
    s32Ret = BSP_IPF_RegisterAdqEmptyDlCb(IpfDlIntAdq1NullCb);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_PRINT_ERROR("CCPU 可以输入测试用例\n");

    down(&g_IpfDlIntAdq1NullSemId);
    IPF_ST_ConfigDlADQ1(IPF_DLAD1_DESC_SIZE - 1);

    while(IPF_DLBD_DESC_SIZE != stResultCheck.u32RdNum)
    {
        ipfTestDelay(2);
    }
    ipfTestDelay(200);

    IPF_PRINT_ERROR("u32RDNum = %d\n", stResultCheck.u32RdNum);

    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/*模拟下行配置BD，数据流向为modem0的无业务搬移验证*/
BSP_S32 BSP_IPF_ST_308(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;

    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_ST_ConfigDlADQ0(u32Adq0Num);

    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 0x3F;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task, &stResultCheck, "IpfDlTask");
    IPF_PRINT_ERROR("CCPU 可以输入测试用例\n");

    down(&g_IpfDlTaskSemId);
    if(u32PacketNum != stResultCheck.u32RdNum)
    {
        IPF_PRINT_ERROR("TESTERROR u32RDNum = %d, u32PacketNum = %d, line=%d\n", stResultCheck.u32RdNum, u32PacketNum, __LINE__);
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/*模拟下行配置BD，数据流向为modem0的无业务过滤搬移验证*/
BSP_S32 BSP_IPF_ST_309(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;

    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_ST_ConfigDlADQ0(u32Adq0Num);

    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task, &stResultCheck, "IpfDlTask");
    IPF_PRINT_ERROR("CCPU 可以输入测试用例\n");

    down(&g_IpfDlTaskSemId);
    if(u32PacketNum != stResultCheck.u32RdNum)
    {
        IPF_PRINT_ERROR("TESTERROR u32RDNum = %d, u32PacketNum = %d, line=%d\n", stResultCheck.u32RdNum, u32PacketNum, __LINE__);
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/*模拟下行配置BD，数据流向为modem1的无业务过滤搬移验证*/
BSP_S32 BSP_IPF_ST_310(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;

    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_ST_ConfigDlADQ0(u32Adq0Num);

    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_1;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task, &stResultCheck, "IpfDlTask");
    IPF_PRINT_ERROR("CCPU 可以输入测试用例\n");

    down(&g_IpfDlTaskSemId);
    if(u32PacketNum != stResultCheck.u32RdNum)
    {
        IPF_PRINT_ERROR("TESTERROR u32RDNum = %d, u32PacketNum = %d, line=%d\n", stResultCheck.u32RdNum, u32PacketNum, __LINE__);
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/*创建任务，用于模拟上、下行长时间并发进行大包数据过滤搬移操作的无业务验证*/
BSP_S32 BSP_IPF_ST_401(BSP_U32 u32Count, BSP_U32 u32Tick)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;
    BSP_U32 u32CountTmp = u32Count;

    IPF_TEST_ENTRY();
    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    else
    {
        IPF_ST_ConfigDlADQ0(u32Adq0Num);
        IPF_ST_ConfigDlADQ1(u32Adq1Num);
    }
    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task_Fn, &stResultCheck, "IpfDlTaskFn");

    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    ipfTestDelay(500);
    while(u32Count--)
    {
        u32BDNum = BSP_IPF_GetUlBDNum();
        if(u32BDNum > 0)
        {
            if(OK != IPF_ST_ConfigUlLongPacket(IPF_MODEM_0, 32, BSP_TRUE))
            {
                IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
                goto err;
            }
        }
        ipfTestDelay(u32Tick);
    }

    while(stResultCheck.u32RdNum != (u32CountTmp * 32))
    {
        IPF_PRINT_ERROR("u32RDNum = %d\n", stResultCheck.u32RdNum);
        ipfTestDelay(2);
    }

    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
/*创建任务，用于模拟上、下行长时间并发进行大包数据过滤搬移操作的无业务压力验证*/
BSP_S32 BSP_IPF_ST_401_Ex(BSP_U32 u32Count, BSP_U32 u32Tick)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;
    BSP_U32 u32CountTmp = u32Count;
    BSP_U32 u32SendBdNum = 0;

    /*数传前准备工作*/
    IPF_TEST_ENTRY();
    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    else
    {
        IPF_ST_ConfigDlADQ0(u32Adq0Num);
        IPF_ST_ConfigDlADQ1(u32Adq1Num);
    }
    sema_init(&g_IpfDlIntResultSemId, 0);
    sema_init(&g_IpfDlIntAdqNullSemId, 0);
    sema_init(&g_IpfDlTaskSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    (BSP_VOID)BSP_IPF_RegisterAdqEmptyDlCb((BSP_IPF_AdqEmptyDlCb)IpfDlIntAdqNullCb);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task_Fn, &stResultCheck, "IpfDlTaskFn");
    g_IpfDlAdqNullTaskId = (int)kthread_run((int (*)(void *))IPF_DL_AdqNull_Task, (void *)0, "IpfDLAdqNullTask");

    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    /*延时等待两个核均配置完成*/
    ipfTestDelay(500);
    /*开始进行数传*/
    while(u32SendBdNum != (u32CountTmp*64))
    {
        for(;;)
        {
            u32BDNum = BSP_IPF_GetUlBDNum();
            if(0 != u32BDNum)
            break;
        }
        if((u32SendBdNum + u32BDNum) > (u32CountTmp * IPF_ULBD_DESC_SIZE)){
            u32BDNum = (u32CountTmp * IPF_ULBD_DESC_SIZE)-u32SendBdNum;
        }
        if(OK != IPF_ST_ConfigUlLongPacket(IPF_MODEM_0, u32BDNum, BSP_TRUE))
        {
            IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
            goto err;
        }
        u32SendBdNum += u32BDNum;
        msDelay(u32Tick);
    }

    while(stResultCheck.u32RdNum != (u32CountTmp * IPF_ULBD_DESC_SIZE))
    {
        IPF_PRINT_ERROR("u32RDNum = %d\n", stResultCheck.u32RdNum);
        ipfTestDelay(2);
    }

    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
/*创建任务，用于模拟上、下行长时间并发进行小包数据过滤搬移操作的无业务验证*/
BSP_S32 BSP_IPF_ST_402(BSP_U32 u32Count, BSP_U32 u32Tick)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;
    BSP_U32 u32CountTmp = u32Count;

    IPF_TEST_ENTRY();
    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    else
    {
        IPF_ST_ConfigDlADQ0(u32Adq0Num);
        IPF_ST_ConfigDlADQ1(u32Adq1Num);
    }
    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task_Fn, &stResultCheck, "IpfDlTaskFn");

    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    ipfTestDelay(500);
    while(u32Count--)
    {
        u32BDNum = BSP_IPF_GetUlBDNum();
        if(u32BDNum > 0)
        {
            if(OK != IPF_ST_ConfigUlShortPacket(IPF_MODEM_0, 32, BSP_TRUE))
            {
                IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
                goto err;
            }
        }
        ipfTestDelay(u32Tick);
    }

    while(stResultCheck.u32RdNum != (u32CountTmp * 32))
    {
        IPF_PRINT_ERROR("u32RDNum = %d\n", stResultCheck.u32RdNum);
        ipfTestDelay(2);
    }

    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
/*创建任务，用于模拟上、下行长时间并发进行小包数据过滤搬移操作的无业务压力验证*/
BSP_S32 BSP_IPF_ST_402_Ex(BSP_U32 u32Count, BSP_U32 u32Tick)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;
    BSP_U32 u32CountTmp = u32Count;
    BSP_U32 u32SendBdNum = 0;

    /*数传前准备工作*/
    IPF_TEST_ENTRY();
    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    else
    {
        IPF_ST_ConfigDlADQ0(u32Adq0Num);
        IPF_ST_ConfigDlADQ1(u32Adq1Num);
    }
    sema_init(&g_IpfDlIntResultSemId, 0);
    sema_init(&g_IpfDlIntAdqNullSemId, 0);
    sema_init(&g_IpfDlTaskSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    (BSP_VOID)BSP_IPF_RegisterAdqEmptyDlCb((BSP_IPF_AdqEmptyDlCb)IpfDlIntAdqNullCb);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task_Fn, &stResultCheck, "IpfDlTaskFn");
    g_IpfDlAdqNullTaskId = (int)kthread_run((int (*)(void *))IPF_DL_AdqNull_Task, (void *)0, "IpfDLAdqNullTask");

    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    /*延时等待两个核均配置完成*/
    ipfTestDelay(500);
    /*开始进行数传*/
    while(u32SendBdNum != (u32CountTmp*64))
    {
        for(;;)
        {
            u32BDNum = BSP_IPF_GetUlBDNum();
            if(0 != u32BDNum)
            break;
        }
        if((u32SendBdNum + u32BDNum) > (u32CountTmp * IPF_ULBD_DESC_SIZE)){
            u32BDNum = (u32CountTmp * IPF_ULBD_DESC_SIZE)-u32SendBdNum;
        }
        if(OK != IPF_ST_ConfigUlShortPacket(IPF_MODEM_0, u32BDNum, BSP_TRUE))
        {
            IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
            goto err;
        }
        u32SendBdNum += u32BDNum;
        msDelay(u32Tick);
    }

    while(stResultCheck.u32RdNum != (u32CountTmp * IPF_ULBD_DESC_SIZE))
    {
        IPF_PRINT_ERROR("u32RDNum = %d\n", stResultCheck.u32RdNum);
        ipfTestDelay(2);
    }

    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
BSP_S32 BSP_IPF_ST_403_Init(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;

    IPF_TEST_ENTRY();
    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    else
    {
        IPF_ST_ConfigDlADQ0(u32Adq0Num);
        IPF_ST_ConfigDlADQ1(u32Adq1Num);
    }
    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task_Fn, &stResultCheck, "IpfDlTaskFn");

    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }


    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
BSP_S32 BSP_IPF_ST_403_Ul(BSP_U32 u32Count, BSP_U32 u32Num, BSP_U32 u32Tick)
{
    BSP_U32 u32BDNum = 0;

    while(u32Count--)
    {
        u32BDNum = BSP_IPF_GetUlBDNum();
        if(u32BDNum > 0)
        {
            if(OK != IPF_ST_ConfigUlLongPacket(IPF_MODEM_0, u32Num, BSP_TRUE))
            {
                IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
                goto err;
            }
        }
        ipfTestDelay(u32Tick);
    }

    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
BSP_S32 BSP_IPF_ST_403_Uninit(BSP_VOID)
{
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/*测试上行大包数据过滤搬移数据的速率*/
BSP_S32 BSP_IPF_ST_501(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;
    BSP_U32 u32SendTime = 0;

    IPF_TEST_ENTRY();
    if(u32PacketNum > (IPF_ULAD0_DESC_SIZE - 1))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    u32SendTime = mdrv_timer_get_normal_timestamp();
    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    if(OK != IPF_ST_ConfigUlLongPacket(IPF_MODEM_0, u32PacketNum, BSP_TRUE))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    ipfTestDelay(200);
    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("u32SendTime = 0x%x\n", u32SendTime);
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
/*测试上行小包数据过滤搬移数据的速率*/
BSP_S32 BSP_IPF_ST_502(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;
    BSP_U32 u32SendTime = 0;

    IPF_TEST_ENTRY();
    if(u32PacketNum > (IPF_ULAD0_DESC_SIZE - 1))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    u32SendTime = mdrv_timer_get_normal_timestamp();
    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    if(OK != IPF_ST_ConfigUlShortPacket(IPF_MODEM_0, u32PacketNum, BSP_TRUE))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    ipfTestDelay(200);
    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("u32SendTime = 0x%x\n", u32SendTime);
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
/*测试下行大包数据过滤搬移数据的速率*/
BSP_S32 BSP_IPF_ST_503(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;
    BSP_U32 u32RecvTime = 0;

    if(u32PacketNum > (IPF_DLAD0_DESC_SIZE - 1))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    IPF_ST_ConfigDlADQ1(u32Adq1Num);

    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task_Speed, &stResultCheck, "IpfDlTask");
    sema_init(&g_IpfDlIntAdq0NullSemId, 0);
    s32Ret = BSP_IPF_RegisterAdqEmptyDlCb(IpfDlIntAdq0NullCb);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_PRINT_ERROR("CCPU 可以输入测试用例\n");

    down(&g_IpfDlTaskSemId);
    u32RecvTime = (BSP_S32)mdrv_timer_get_normal_timestamp();
    if(u32PacketNum != stResultCheck.u32RdNum)
    {
        IPF_PRINT_ERROR("TESTERROR u32RDNum = %d, u32PacketNum = %d, line=%d\n", stResultCheck.u32RdNum, u32PacketNum, __LINE__);
        IPF_PRINT_ERROR("TESTERROR line=%d\n", __LINE__);
        return ERROR;
    }

    IPF_PRINT_ERROR("s32RecvTime = 0x%x\n", u32RecvTime);
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/*测试下行小包数据过滤搬移数据的速率*/
BSP_S32 BSP_IPF_ST_504(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;
    BSP_U32 u32RecvTime = 0;

    if(u32PacketNum > (IPF_DLAD0_DESC_SIZE - 1))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    IPF_ST_ConfigDlADQ0(u32Adq0Num);

    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task_Speed, &stResultCheck, "IpfDlTask");
    sema_init(&g_IpfDlIntAdq0NullSemId, 0);
    s32Ret = BSP_IPF_RegisterAdqEmptyDlCb(IpfDlIntAdq0NullCb);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_PRINT_ERROR("CCPU 可以输入测试用例\n");

    down(&g_IpfDlTaskSemId);
    u32RecvTime = mdrv_timer_get_normal_timestamp();
    if(u32PacketNum != stResultCheck.u32RdNum)
    {
        IPF_PRINT_ERROR("TESTERROR u32RDNum = %d, u32PacketNum = %d, line=%d\n", stResultCheck.u32RdNum, u32PacketNum, __LINE__);
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_PRINT_ERROR("u32RecvTime = 0x%x\n", u32RecvTime);
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/*测试上、下行大包数据过滤搬移数据的速率*/
BSP_S32 BSP_IPF_ST_505_Step1(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;

    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    else
    {
        IPF_ST_ConfigDlADQ0(u32Adq0Num);
        IPF_ST_ConfigDlADQ1(u32Adq1Num);
    }
    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task_Fn, &stResultCheck, "IpfDlTaskFn");

    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
BSP_S32 BSP_IPF_ST_505_Step2(BSP_U32 u32PacketNum)
{
    BSP_U32 i = 0;
    StreamIPv4Header stIPHeader = {0};
    StreamUDP stUDP = {0};
    BSP_U8* pu8IpData = BSP_NULL;
    IPF_CONFIG_ULPARAM_S *stUlPara = BSP_NULL;
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;
    BSP_U32 u32UlSendTime = 0;
    BSP_U32 u32DlRecvTime = 0;

    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_PRINT_ERROR("CCPU 可以输入测试用例\n");

    /* 等待另外一个core同步完成 */
    g_pSyncState[SYNC_MODULE_IPF] = 0;
    s32Ret = mdrv_sync_wait(SYNC_MODULE_IPF, 10000);
    if(s32Ret != BSP_OK)
    {
        IPF_PRINT_ERROR("\r IPF BSP_SYNC_Wait error! \n");
        return ERROR;
    }
    u32UlSendTime = mdrv_timer_get_normal_timestamp();

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = LONG_IP_LEN;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;

    pu8IpData = (BSP_U8*)BSP_SMalloc(LONG_IP_LEN, MEM_ICC_DDR_POOL);
    if(pu8IpData == BSP_NULL)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    hisi_io_memset(pu8IpData, 0x0, LONG_IP_LEN);
    hisi_io_memcpy(pu8IpData, &stIPHeader, sizeof(StreamIPv4Header));
    hisi_io_memcpy(pu8IpData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
    hisi_io_memcpy(pu8IpData+sizeof(StreamIPv4Header)+sizeof(StreamUDP), pu8TestData, (sizeof(pu8TestData)-1));

    stUlPara = (IPF_CONFIG_ULPARAM_S*)kmalloc(u32PacketNum * sizeof(IPF_CONFIG_ULPARAM_S), GFP_KERNEL);
    if (!stUlPara)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d  FAILED\n", __LINE__);
        return ERROR;
    }
    stUlPara[0].u16Len = LONG_IP_LEN;
    stUlPara[0].u16Attribute = (IPF_MODEM0_ULFC <<4) | 0x0;
    stUlPara[0].u32Data = HISI_PA_ADDRESS(pu8IpData);
    stUlPara[0].u16Reserved = 0;
    stUlPara[0].u16UsrField1 = IPF_MODEM_0;
    stUlPara[0].u32UsrField2 = 0x5A5A5A5A;
    stUlPara[0].u32UsrField3 = 0x55AA55AA;
    for(i = 1; i < u32PacketNum; i++)
    {
        hisi_io_memcpy(&stUlPara[i], &stUlPara[0], sizeof(IPF_CONFIG_ULPARAM_S));
    }
    stUlPara[--i].u16Attribute = (IPF_MODEM0_ULFC <<4) | 0x1;

    s32Ret = BSP_IPF_ConfigUpFilter(u32PacketNum, stUlPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        BSP_SFREE(pu8IpData);
        return ERROR;
    }

    down(&g_IpfDlTaskSemId);
    u32DlRecvTime = (BSP_S32)mdrv_timer_get_normal_timestamp();
    if(u32PacketNum != stResultCheck.u32RdNum)
    {
        IPF_PRINT_ERROR("TESTERROR u32RDNum = %d, u32PacketNum = %d, line=%d\n", stResultCheck.u32RdNum, u32PacketNum, __LINE__);
        IPF_PRINT_ERROR("TESTERROR line=%d\n", __LINE__);
        return ERROR;
    }
    BSP_SFREE(pu8IpData);
    pu8IpData = BSP_NULL;
    kfree(stUlPara);
    stUlPara = BSP_NULL;
    IPF_PRINT_ERROR("s32UlSendTime = 0x%x, s32DlRecvTime = 0x%x\n", u32UlSendTime, u32DlRecvTime);

    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/*测试上、下行小包数据过滤搬移数据的速率*/
BSP_S32 BSP_IPF_ST_506_Step1(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;

    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    else
    {
        IPF_ST_ConfigDlADQ0(u32Adq0Num);
        IPF_ST_ConfigDlADQ1(u32Adq1Num);
    }
    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task_Fn, &stResultCheck, "IpfDlTaskFn");

    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
BSP_S32 BSP_IPF_ST_506_Step2(BSP_U32 u32PacketNum)
{
    BSP_U32 i = 0;
    StreamIPv4Header stIPHeader = {0};
    StreamUDP stUDP = {0};
    BSP_U8* pu8IpData = BSP_NULL;
    IPF_CONFIG_ULPARAM_S *stUlPara = BSP_NULL;
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;
    BSP_U32 u32UlSendTime = 0;
    BSP_U32 u32DlRecvTime = 0;

    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_PRINT_ERROR("CCPU 可以输入测试用例\n");

    /* 等待另外一个core同步完成 */
    g_pSyncState[SYNC_MODULE_IPF] = 0;
    s32Ret = mdrv_sync_wait(SYNC_MODULE_IPF, 10000);
    if(s32Ret != BSP_OK)
    {
        IPF_PRINT_ERROR("\r IPF BSP_SYNC_Wait error! \n");
        return ERROR;
    }
    u32UlSendTime = mdrv_timer_get_normal_timestamp();

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = SHORT_IP_LEN;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;

    pu8IpData = (BSP_U8*)BSP_SMalloc(SHORT_IP_LEN, MEM_ICC_DDR_POOL);
    if(pu8IpData == BSP_NULL)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }
    hisi_io_memset(pu8IpData, 0x0, SHORT_IP_LEN);
    hisi_io_memcpy(pu8IpData, &stIPHeader, sizeof(StreamIPv4Header));
    hisi_io_memcpy(pu8IpData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
    hisi_io_memcpy(pu8IpData+sizeof(StreamIPv4Header)+sizeof(StreamUDP), pu8TestData, (sizeof(pu8TestData)-1));

    stUlPara = (IPF_CONFIG_ULPARAM_S*)kmalloc(u32PacketNum * sizeof(IPF_CONFIG_ULPARAM_S), GFP_KERNEL);
    if (!stUlPara)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d  FAILED\n", __LINE__);
        return ERROR;
    }
    stUlPara[0].u16Len = SHORT_IP_LEN;
    stUlPara[0].u16Attribute = (IPF_MODEM0_ULFC <<4) | 0x0;
    stUlPara[0].u32Data = HISI_PA_ADDRESS(pu8IpData);
    stUlPara[0].u16Reserved = 0;
    stUlPara[0].u16UsrField1 = IPF_MODEM_0;
    stUlPara[0].u32UsrField2 = 0x5A5A5A5A;
    stUlPara[0].u32UsrField3 = 0x55AA55AA;
    for(i = 1; i < u32PacketNum; i++)
    {
        hisi_io_memcpy(&stUlPara[i], &stUlPara[0], sizeof(IPF_CONFIG_ULPARAM_S));
    }
    stUlPara[--i].u16Attribute = (IPF_MODEM0_ULFC <<4) | 0x1;

    s32Ret = BSP_IPF_ConfigUpFilter(u32PacketNum, stUlPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        BSP_SFREE(pu8IpData);
        return ERROR;
    }

    down(&g_IpfDlTaskSemId);
    u32DlRecvTime = (BSP_S32)mdrv_timer_get_normal_timestamp();
    if(u32PacketNum != stResultCheck.u32RdNum)
    {
        IPF_PRINT_ERROR("TESTERROR u32RDNum = %d, u32PacketNum = %d, line=%d\n", stResultCheck.u32RdNum, u32PacketNum, __LINE__);
        IPF_PRINT_ERROR("BSP_IPF_ST_503, %d\n", __LINE__);
        return ERROR;
    }
    BSP_SFREE(pu8IpData);
    pu8IpData = BSP_NULL;
    kfree(stUlPara);
    stUlPara = BSP_NULL;
    IPF_PRINT_ERROR("s32UlSendTime = 0x%x, s32DlRecvTime = 0x%x\n", u32UlSendTime, u32DlRecvTime);

    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/* 上下电测试一: 寄存器备份恢复测试 */
BSP_S32 BSP_IPF_ST_601(BSP_VOID)
{
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

}

/* 上下电测试二: 过滤规则备份恢复测试 */
BSP_S32 BSP_IPF_ST_602(BSP_VOID)
{
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

}

/* 上下电测试三: 数传测试 */
/* 上行测试 */
BSP_S32 BSP_IPF_ST_109_603(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;

    IPF_TEST_ENTRY();
    if(u32PacketNum > (IPF_ULAD0_DESC_SIZE - 1))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n", __LINE__);
        goto err;
    }

    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    if(OK != IPF_ST_ConfigUlShortPacket(IPF_MODEM_0, u32PacketNum, BSP_TRUE))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    ipfTestDelay(200);
    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}
/* 下行测试 */
BSP_S32 BSP_IPF_ST_309_604(BSP_U32 u32PacketNum)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;

    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_ST_ConfigDlADQ0(u32Adq0Num);

    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task, &stResultCheck, "IpfDlTask");
    IPF_PRINT_ERROR("CCPU 可以输入测试用例\n");

    down(&g_IpfDlTaskSemId);
    if(u32PacketNum != stResultCheck.u32RdNum)
    {
        IPF_PRINT_ERROR("TESTERROR u32RDNum = %d, u32PacketNum = %d, line=%d\n", stResultCheck.u32RdNum, u32PacketNum, __LINE__);
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        return ERROR;
    }

    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;
}
/* 上下行并发测试 */
BSP_S32 BSP_IPF_ST_401_605(BSP_U32 u32Count, BSP_U32 u32Tick)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32BDNum = 0;
    BSP_U32 u32Adq0Num = 0;
    BSP_U32 u32Adq1Num = 0;
    BSP_U32 u32CountTmp = u32Count;

    IPF_TEST_ENTRY();
    s32Ret = BSP_IPF_GetDlAdNum(&u32Adq0Num, &u32Adq1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    else
    {
        IPF_ST_ConfigDlADQ0(u32Adq0Num);
        IPF_ST_ConfigDlADQ1(u32Adq1Num);
    }
    sema_init(&g_IpfDlIntResultSemId, 0);
    (BSP_VOID)BSP_IPF_RegisterWakeupDlCb(IpfDlIntResultCb);
    sema_init(&g_IpfDlTaskSemId, 0);

    stResultCheck.u16Result = 5;
    stResultCheck.u16UsrField1 = IPF_MODEM_0;
    stResultCheck.u32UsrField2 = 0x5A5A5A5A;
    stResultCheck.u32UsrField3 = 0x55AA55AA;
    stResultCheck.u32RdNum = 0;
    g_IpfDlTaskId = kthread_run((int (*)(void *))IPF_DL_Task_Fn, &stResultCheck, "IpfDlTaskFn");

    u32BDNum = BSP_IPF_GetUlBDNum();
    if(u32BDNum != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }

    ipfTestDelay(500);
    while(u32Count--)
    {
        u32BDNum = BSP_IPF_GetUlBDNum();
        if(u32BDNum > 0)
        {
            if(OK != IPF_ST_ConfigUlLongPacket(IPF_MODEM_0, 32, BSP_TRUE))
            {
                IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
                goto err;
            }
        }
        ipfTestDelay(u32Tick);
    }

    while(stResultCheck.u32RdNum != (u32CountTmp * 32))
    {
        IPF_PRINT_ERROR("TESTERROR line=%d, u32RDNum = %d\n", __LINE__, stResultCheck.u32RdNum);
        ipfTestDelay(2);
    }

    s32Ret = BSP_IPF_UlStateIdle();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT_ERROR("TESTERROR line=%d\n",  __LINE__);
        goto err;
    }
    IPF_TEST_LEAVE();
    IPF_PRINT_ERROR("SUCCESS!\n");
    return OK;

    err:
        IPF_TEST_LEAVE();
        return ERROR;
}

EXPORT_SYMBOL(BSP_IPF_ST_001);
EXPORT_SYMBOL(BSP_IPF_ST_002);
EXPORT_SYMBOL(BSP_IPF_ST_003);
EXPORT_SYMBOL(BSP_IPF_ST_004);
EXPORT_SYMBOL(BSP_IPF_ST_101);
EXPORT_SYMBOL(BSP_IPF_ST_102);
EXPORT_SYMBOL(BSP_IPF_ST_103);
EXPORT_SYMBOL(BSP_IPF_ST_104);
EXPORT_SYMBOL(BSP_IPF_ST_105);
EXPORT_SYMBOL(BSP_IPF_ST_108);
EXPORT_SYMBOL(BSP_IPF_ST_109);
EXPORT_SYMBOL(BSP_IPF_ST_110);
EXPORT_SYMBOL(BSP_IPF_ST_301);
EXPORT_SYMBOL(BSP_IPF_ST_302);
EXPORT_SYMBOL(BSP_IPF_ST_303);
EXPORT_SYMBOL(BSP_IPF_ST_304);
EXPORT_SYMBOL(BSP_IPF_ST_305);
EXPORT_SYMBOL(BSP_IPF_ST_308);
EXPORT_SYMBOL(BSP_IPF_ST_309);
EXPORT_SYMBOL(BSP_IPF_ST_310);
EXPORT_SYMBOL(BSP_IPF_ST_401);
EXPORT_SYMBOL(BSP_IPF_ST_402);
EXPORT_SYMBOL(BSP_IPF_ST_501);
EXPORT_SYMBOL(BSP_IPF_ST_502);
EXPORT_SYMBOL(BSP_IPF_ST_503);
EXPORT_SYMBOL(BSP_IPF_ST_504);
EXPORT_SYMBOL(BSP_IPF_ST_505_Step1);
EXPORT_SYMBOL(BSP_IPF_ST_505_Step2);
EXPORT_SYMBOL(BSP_IPF_ST_506_Step1);
EXPORT_SYMBOL(BSP_IPF_ST_506_Step2);
EXPORT_SYMBOL(BSP_IPF_ST_601);
MODULE_AUTHOR("zhoushan@212992");
MODULE_DESCRIPTION("IPF TEST MODULE");
MODULE_LICENSE("GPL");
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
