/*
 **************************************************************************************
 *
 *       Filename:   vpp_inter.c
 *    Description:   source file
 *
 *        Version:  1.0
 *        Created:  2011-07-8 16:20:00
 *         Author:  j00140427
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */
//add for zme coef config by l50053, begin
//#include <mach/hisi_mem.h>
//add for zme coef config by l50053, end
//#include <linux/kernel.h>
/* obviously, for kmalloc */
#include <linux/slab.h>
#include <linux/hisi/hisi_ion.h>
#include "vpp_inter.h"

#define LOG_TAG "VPP_INTER"
#include "vpp_log.h"

#define VPP_RESERVED_BASE_SIZE (5*1024*1024)

#define OFFSET 0x0000000

#define DIE_FRAME_MAX_WIDTH (1920)  //S00198515 add for DIE 20120828
#define DIE_FRAME_MAX_HEIGHT (1088)  //S00198515 add for DIE 20120828
struct k3_vpp_addr k3vpp;
/**************************************************************
全局变量定义
*/
//VPP 通道:0 vsd, 1 vhd
static VPP_VPP_S s_stVPPLayer[HAL_LAYER_BUTT];

//VPP DIE //S00198515 add 20120823
static VPP_DIE_ADDR_S s_stDieAddr;
//static VPP_MMU_ADDR_S       s_MmuAddr;
//VPP DIE addr ctrl
static unsigned int s_u32DieAddrCtrl[10];

/**************************************************************
宏功能单元定义
*/
//获取VPP Layer句柄
#define VPP_GET_HANDLE(enLayer, pstVpp) \
do{                                  \
    pstVpp = &(s_stVPPLayer[(int)enLayer]); \
}while(0)

static int alloc_vpp_reserved_mem(struct k3_vpp_addr *k3vpp, unsigned int buf_size){

    struct ion_client *client = NULL;
    struct ion_handle *handle = NULL;
    size_t len = 0;
    unsigned long phys_addr = 0;
    client = (struct ion_client *)hisi_ion_client_create("k3_vpp_ion");
    if (NULL == client){
        loge("failed to create ion client!\n");
        goto err_ion_client_create;
    }
    handle = ion_alloc(client, buf_size, PAGE_SIZE, ION_HEAP(ION_GRALLOC_HEAP_ID), 0);
    if (IS_ERR_OR_NULL(handle)) {
        loge("failed to ion_alloc!\n");
        goto err_ion_alloc;
    }
    if (NULL == ion_map_kernel(client, handle)) {
        loge("failed to ion_map_kernel!\n");
        goto err_ion_map;
    }
    if (ion_phys(client, handle, &phys_addr, &len) < 0) {
        loge("failed to get ion phys!\n");
        goto err_ion_phys;
    }
    k3vpp->ion_client = client;
    k3vpp->ion_handle = handle;
    return phys_addr;
err_ion_phys:
    ion_unmap_kernel(k3vpp->ion_client, k3vpp->ion_handle);
err_ion_map:
    ion_free(k3vpp->ion_client, k3vpp->ion_handle);
err_ion_alloc:
    ion_client_destroy(k3vpp->ion_client);
err_ion_client_create:
    return 0;
}
static void vpp_free_buffer(struct k3_vpp_addr *k3vpp){
    if (k3vpp->ion_client != NULL && k3vpp->ion_handle != NULL) {
        ion_unmap_kernel(k3vpp->ion_client, k3vpp->ion_handle);
        ion_free(k3vpp->ion_client, k3vpp->ion_handle);
        ion_client_destroy(k3vpp->ion_client);
        k3vpp->ion_client = NULL;
        k3vpp->ion_handle = NULL;
    }
}
/**************************************************************
基本功能函数定义
*/
#if 0
//add for init mmu table
static int inter_create_mmu_table(VPP_MMU_ADDR_S *MmuModule)
{
    int i = 0;
    unsigned long vir_addr = 0;
    unsigned long *MmuPage = NULL;

    memset((VPP_MMU_ADDR_S *)MmuModule, 0, sizeof(VPP_MMU_ADDR_S));

    MmuModule->mmu_BusAddr = VPP_MMU_RESERVED_MEM_ADDR;

    MmuModule->mmu_ch_ptba = (unsigned long)MmuModule->mmu_BusAddr + 80*1024*1024;
    MmuModule->mmu_ch_ptva = (unsigned long)MmuModule->mmu_BusAddr + OFFSET;

    MmuPage
            = (unsigned int*)ioremap(MmuModule->mmu_ch_ptba, 20 * 1024 * 4);
    for(i=0; i<20*1024; i++)
    {
        *(MmuPage+i) = (((unsigned long)MmuModule->mmu_BusAddr) + 4 * 1024 * i)&0xfffff000|0x400;
    }
    for(i=0; i<11; i++)
    {
        hal_set_mmu_ch_ptba(i,(MmuModule->mmu_ch_ptba));
        hal_set_mmu_ch_ptva(i,MmuModule->mmu_ch_ptva);
    }

    return K3_SUCCESS;
}
#endif
// 初始化视频层(当前仅支持视频层1)
static void inter_init_layer(HAL_LAYER_E enLayer)
{
    VPP_VPP_S *pstVpp = NULL;
    int s32Coef[8] = {0,13,-63,306,306,-63,13,0};

    logd();

    VPP_GET_HANDLE(enLayer, pstVpp);

    pstVpp->bopened = false;

    pstVpp->enLayer = enLayer;
    pstVpp->boffline_mode_en = false;

    pstVpp->st_in_rect.s32X = 0;
    pstVpp->st_in_rect.s32Y = 0;
    pstVpp->st_in_rect.s32Width = 320;
    pstVpp->st_in_rect.s32Height = 240;

    pstVpp->st_out_rect.s32X = 0;
    pstVpp->st_out_rect.s32Y = 0;
    pstVpp->st_out_rect.s32Width = 320;
    pstVpp->st_out_rect.s32Height = 240;

    pstVpp->bcsc_enable = false;
    pstVpp->bzoom_enable = false;

    hal_set_rd_bus_id(1);
    //1 as default outstanding value
    if( 0 == pstVpp->axi_ostd_r0 )
    {
        pstVpp->axi_ostd_r0 = 1;
    }

    if( 0 == pstVpp->axi_ostd_r1 )
    {
        pstVpp->axi_ostd_r1 = 1;
    }

    if( 0 == pstVpp->axi_ostd_wr )
    {
        pstVpp->axi_ostd_wr = 1;
    }
    hal_set_rd_out_std(0, pstVpp->axi_ostd_r0);  /* AXI总线读ID0的outstanding。*/
    hal_set_rd_out_std(1, pstVpp->axi_ostd_r1);  /* resident */
    hal_set_wr_out_std(pstVpp->axi_ostd_wr);

    //1 set video layer ifir enable
    hal_set_ifir_mode(HAL_LAYER_VIDEO1, HAL_IFIRMODE_DOUBLE);

    hal_set_ifir_coef(HAL_LAYER_VIDEO1, s32Coef);

    hal_set_die_def_thd(HAL_LAYER_VIDEO1);
    hal_set_pd_defthd (HAL_LAYER_VIDEO1);  /* pull down */

    //1 HAL Clock gate ctrl enable
    hal_set_clk_gate_en(1);


    hal_set_zme_core(HAL_LAYER_VIDEO1, 2);

    hal_set_lnk_lst_node_int_en(1); /*wbc1 int enable */

    hal_set_mmu_enable(HAL_LAYER_VIDEO1,0,0,0,0);
    hal_set_die_enable (HAL_LAYER_VIDEO1, 0, 0);
}

// 去初始化视频层(目前仅支持视频层1)
static void inter_deinit_layer(HAL_LAYER_E enLayer)
{
    //logd();
    hal_set_mmu_core(0);
    hal_set_mmu_core(0);
    hal_set_clk_gate_en(0);
    hal_set_lnk_lst_node_int_en(0); /*wbc1 deint disable */
}

//modified for zme coef config by l50053, begin
static __inline int inter_bitvalue(int s32Value)
{
    return s32Value < 0 ? s32Value : s32Value;
}

//S00198515 add for DIE 20120828 [start]
static int inter_init_dieinfo(VPP_DIE_ADDR_S *pstDieModule)
{
    int i = 0;
    unsigned int phy_addr;
    if(NULL == pstDieModule){
        loge("pstDieModule is null!\n");
        return K3_FAILURE;
    }
    memset((VPP_DIE_ADDR_S *)pstDieModule, 0, sizeof(VPP_DIE_ADDR_S));

    pstDieModule->die_size = ( ((((DIE_FRAME_MAX_WIDTH -1) >> 6) + 1)) << 4)*DIE_FRAME_MAX_HEIGHT*10;

    phy_addr = alloc_vpp_reserved_mem(&k3vpp, VPP_RESERVED_BASE_SIZE);
    pstDieModule->die_BusAddr = &phy_addr;
    logi("in inter_init_dieinfo:phy_addr=%#0x\n",phy_addr);
    for(i=0; i<10; i++)
    {
                s_u32DieAddrCtrl[i] = phy_addr + pstDieModule->die_size/10*i;
    }
    return K3_SUCCESS;
}

/*
**************************************************************************
* FunctionName: inter_acm_mycheck
* Description :
* Input       :
*             :
* Output      : NA;
* ReturnValue : NA;
* Other       : NA;
**************************************************************************
*/
int inter_acm_mycheck(HAL_ACMBLKINFO_S *psgl_a, int ap, HAL_ACMBLKINFO_S *psgl_b, int bp, int reg_a, int reg_b)
{
    int indexUa=0, indexUb=0;
    int indexVa=0, indexVb=0;

    switch(ap)
    {
    case 0:
        {
            indexUa = psgl_a->s8_uoffset_a;
            indexVa = psgl_a->s8_voffset_a;
            break;
        }
    case 1:
        {
            indexUa = psgl_a->s8_uoffset_b;
            indexVa = psgl_a->s8_voffset_b;
            break;
        }
    case 2:
        {
            indexUa = psgl_a->s8_uoffset_c;
            indexVa = psgl_a->s8_voffset_c;
            break;
        }
    case 3:
        {
            indexUa = psgl_a->s8_uoffset_d;
            indexVa = psgl_a->s8_voffset_d;
            break;
        }
    default:
        {
            loge("ap=%d. ap should be integer between 0 and 3.\n", ap);

            return -1;
        }
    }

    switch(bp)
    {
    case 0:
        {
            indexUb = psgl_b->s8_uoffset_a;
            indexVb = psgl_b->s8_voffset_a;
            break;
        }
    case 1:
        {
            indexUb = psgl_b->s8_uoffset_b;
            indexVb = psgl_b->s8_voffset_b;
            break;
        }
    case 2:
        {
            indexUb = psgl_b->s8_uoffset_c;
            indexVb = psgl_b->s8_voffset_c;
            break;
        }
    case 3:
        {
            indexUb = psgl_b->s8_uoffset_d;
            indexVb = psgl_b->s8_voffset_d;
            break;
        }
    default:
        {
            loge("bp=%d. bp should be integer between 0 and 3.\n", bp);
            return -1;
        }
    }

    if(indexUa!=indexUb || indexVa!=indexVb)
    {
        loge("Bad para!Region %d Point %d should be same as Region %d Point %d.\n", reg_a, ap, reg_b, bp);
    }

    return 0;
}
int inter_acm_infocheck(HAL_ACMINFO_S * pacm_info, int num_a, int num_b)
{
    HAL_ACMBLKINFO_S * psgl_a = NULL;
    HAL_ACMBLKINFO_S * psgl_b = NULL;
    int indexUa=0, indexVa=0;
    int indexUb=0, indexVb=0;

    switch(num_a)
    {
    case 0:
        {
            psgl_a = &(pacm_info->stAcmBlk0);
            break;

        }
    case 1:
        {
            psgl_a = &(pacm_info->stAcmBlk1);
            break;
        }
    case 2:
        {
            psgl_a = &(pacm_info->stAcmBlk2);
            break;
        }
    case 3:
        {
            psgl_a = &(pacm_info->stAcmBlk3);
            break;
        }
    default:
        {
            printk("num_a=%d. num_a should be integer between 0 and 3!\n", num_a);
            return -1;
        }
    }

    switch(num_b)
    {
    case 0:
        {
            psgl_b = &(pacm_info->stAcmBlk0);
            break;
        }
    case 1:
        {
            psgl_b = &(pacm_info->stAcmBlk1);
            break;
        }
    case 2:
        {
            psgl_b = &(pacm_info->stAcmBlk2);
            break;
        }
    case 3:
        {
            psgl_b = &(pacm_info->stAcmBlk3);
            break;
        }
    default:
        {
            loge("num_b=%d. num_b should be integer between 0 and 3!\n", num_b);
            return -1;
        }
    }

    indexUa = psgl_a->u8_uIndex;
    indexVa = psgl_a->u8_vIndex;
    indexUb = psgl_b->u8_uIndex;
    indexVb = psgl_b->u8_vIndex;

    if(VPP_ACM_ABS(indexUa-indexUb)<=1 && VPP_ACM_ABS(indexVa-indexVb)<=1) //two regions is connected
    {
        if(indexUb-indexUa==-1 && indexVb-indexVa==-1) //0-b is at the left&below from a
        {
#ifdef OPTM_ALG_ACMCHECKPRINT
            inter_acm_mycheck(psgl_a, 0, psgl_b, 2, num_a, num_b);
#endif

            psgl_b->s8_uoffset_c = psgl_a->s8_uoffset_a;
            psgl_b->s8_voffset_c = psgl_a->s8_voffset_a;
        }
        else if(indexUb-indexUa==-1 && indexVb-indexVa==0) //1-b is at the left from a
        {
#ifdef OPTM_ALG_ACMCHECKPRINT
            inter_acm_mycheck(psgl_a, 1, psgl_b, 2, num_a, num_b);
            inter_acm_mycheck(psgl_a, 0, psgl_b, 3, num_a, num_b);
#endif

            psgl_b->s8_uoffset_c = psgl_a->s8_uoffset_b;
            psgl_b->s8_voffset_c = psgl_a->s8_voffset_b;
            psgl_b->s8_uoffset_d = psgl_a->s8_uoffset_a;
            psgl_b->s8_voffset_d = psgl_a->s8_voffset_a;
        }
        else if(indexUb-indexUa==-1 && indexVb-indexVa==1) //2-b is at the left&up from a
        {
#ifdef OPTM_ALG_ACMCHECKPRINT
            inter_acm_mycheck(psgl_a, 1, psgl_b, 3, num_a, num_b);
#endif

            psgl_b->s8_uoffset_d = psgl_a->s8_uoffset_b;
            psgl_b->s8_voffset_d = psgl_a->s8_voffset_b;
        }
        else if(indexUb-indexUa==0 && indexVb-indexVa==1) //3-b is at the up from a
        {
#ifdef OPTM_ALG_ACMCHECKPRINT
            inter_acm_mycheck(psgl_a, 1, psgl_b, 0, num_a, num_b);
            inter_acm_mycheck(psgl_a, 2, psgl_b, 3, num_a, num_b);
#endif

            psgl_b->s8_uoffset_a = psgl_a->s8_uoffset_b;
            psgl_b->s8_voffset_a = psgl_a->s8_voffset_b;
            psgl_b->s8_uoffset_d = psgl_a->s8_uoffset_c;
            psgl_b->s8_voffset_d = psgl_a->s8_voffset_c;
        }
        else if(indexUb-indexUa==1 && indexVb-indexVa==1) //4-b is at the right&up from a
        {
#ifdef OPTM_ALG_ACMCHECKPRINT
            inter_acm_mycheck(psgl_a, 2, psgl_b, 0, num_a, num_b);
#endif

            psgl_b->s8_uoffset_a = psgl_a->s8_uoffset_c;
            psgl_b->s8_voffset_a = psgl_a->s8_voffset_c;
        }
        else if(indexUb-indexUa==1 && indexVb-indexVa==0) //5-b is at the right from a
        {
#ifdef OPTM_ALG_ACMCHECKPRINT
            inter_acm_mycheck(psgl_a, 3, psgl_b, 0, num_a, num_b);
            inter_acm_mycheck(psgl_a, 2, psgl_b, 1, num_a, num_b);
#endif

            psgl_b->s8_uoffset_a = psgl_a->s8_uoffset_d;
            psgl_b->s8_voffset_a = psgl_a->s8_voffset_d;
            psgl_b->s8_uoffset_b = psgl_a->s8_uoffset_c;
            psgl_b->s8_voffset_b = psgl_a->s8_voffset_c;
        }
        else if(indexUb-indexUa==1 && indexVb-indexVa==-1) //6-b is at the right&below from a
        {
#ifdef OPTM_ALG_ACMCHECKPRINT
            inter_acm_mycheck(psgl_a, 3, psgl_b, 1, num_a, num_b);
#endif

            psgl_b->s8_uoffset_b = psgl_a->s8_uoffset_d;
            psgl_b->s8_voffset_b = psgl_a->s8_voffset_d;
        }
        else if(indexUb-indexUa==0 && indexVb-indexVa==-1) //7-b is at the below from a
        {
#ifdef OPTM_ALG_ACMCHECKPRINT
            inter_acm_mycheck(psgl_a, 0, psgl_b, 1, num_a, num_b);
            inter_acm_mycheck(psgl_a, 3, psgl_b, 2, num_a, num_b);
#endif

            psgl_b->s8_uoffset_b = psgl_a->s8_uoffset_a;
            psgl_b->s8_voffset_b = psgl_a->s8_voffset_a;
            psgl_b->s8_uoffset_c = psgl_a->s8_uoffset_d;
            psgl_b->s8_voffset_c = psgl_a->s8_voffset_d;
        }
        else if(indexUb-indexUa==0 && indexVb-indexVa==0) //8 is at the same position with a
        {
#ifdef OPTM_ALG_ACMCHECKPRINT
            inter_acm_mycheck(psgl_a, 0, psgl_b, 0, num_a, num_b);
            inter_acm_mycheck(psgl_a, 1, psgl_b, 1, num_a, num_b);
            inter_acm_mycheck(psgl_a, 2, psgl_b, 2, num_a, num_b);
            inter_acm_mycheck(psgl_a, 3, psgl_b, 3, num_a, num_b);
#endif

            psgl_b->s8_uoffset_a = psgl_a->s8_uoffset_a;
            psgl_b->s8_voffset_a = psgl_a->s8_voffset_a;
            psgl_b->s8_uoffset_b = psgl_a->s8_uoffset_b;
            psgl_b->s8_voffset_b = psgl_a->s8_voffset_b;
            psgl_b->s8_uoffset_c = psgl_a->s8_uoffset_c;
            psgl_b->s8_voffset_c = psgl_a->s8_voffset_c;
            psgl_b->s8_uoffset_d = psgl_a->s8_uoffset_d;
            psgl_b->s8_voffset_d = psgl_a->s8_voffset_d;
        }
    }
    return 0;
}

void inter_acm_boundcheck(HAL_ACMBLKINFO_S *psgl)
{
    int indexU = psgl->u8_uIndex;
    int indexV = psgl->u8_vIndex;

    if(indexU==0 && indexV==0) //blockU==0 && blockV==0
    {
        psgl->s8_uoffset_a = VPP_ACM_MAX2(psgl->s8_uoffset_a, 0); //shift_a_u>=0
        psgl->s8_voffset_a = VPP_ACM_MAX2(psgl->s8_voffset_a, 0); //shift_a_v>=0
        psgl->s8_uoffset_b = VPP_ACM_MAX2(psgl->s8_uoffset_b, 0); //shift_b_u>=0
        psgl->s8_voffset_d = VPP_ACM_MAX2(psgl->s8_voffset_d, 0); //shift_d_v>=0
    }
    else if(indexU==0 && indexV==15) //blockU==0 && blockV==15
    {
        psgl->s8_uoffset_a = VPP_ACM_MAX2(psgl->s8_uoffset_a, 0); //shift_a_u>=0
        psgl->s8_uoffset_b = VPP_ACM_MAX2(psgl->s8_uoffset_b, 0); //shift_b_u>=0
        psgl->s8_voffset_b = VPP_ACM_MIN2(psgl->s8_voffset_b, 0); //shift_b_v<=0
        psgl->s8_voffset_c = VPP_ACM_MIN2(psgl->s8_voffset_c, 0); //shift_c_v<=0
    }
    else if(indexU==15 && indexV==15) //blockU==15 && blockV==15
    {
        psgl->s8_voffset_b = VPP_ACM_MIN2(psgl->s8_voffset_b, 0); //shift_b_v<=0
        psgl->s8_uoffset_c = VPP_ACM_MIN2(psgl->s8_uoffset_c, 0); //shift_c_u<=0
        psgl->s8_voffset_c = VPP_ACM_MIN2(psgl->s8_voffset_c, 0); //shift_c_v<=0
        psgl->s8_uoffset_d = VPP_ACM_MIN2(psgl->s8_uoffset_d, 0); //shift_d_u<=0
    }
    else if(indexU==15 && indexV==0) //blockU==15 && blockV==0
    {
        psgl->s8_voffset_a = VPP_ACM_MAX2(psgl->s8_voffset_a, 0); //shift_a_v>=0
        psgl->s8_uoffset_c = VPP_ACM_MIN2(psgl->s8_uoffset_c, 0); //shift_c_u<=0
        psgl->s8_uoffset_d = VPP_ACM_MIN2(psgl->s8_uoffset_d, 0); //shift_d_u<=0
        psgl->s8_voffset_d = VPP_ACM_MAX2(psgl->s8_voffset_d, 0); //shift_d_v>=0
    }
    else if(indexU==0) //blockU==0 && 0<blockV<15
    {
        psgl->s8_uoffset_a = VPP_ACM_MAX2(psgl->s8_uoffset_a, 0); //shift_a_u>=0
        psgl->s8_uoffset_b = VPP_ACM_MAX2(psgl->s8_uoffset_b, 0); //shift_b_u>=0
    }
    else if(indexV==15) //0<blockU<15 && blockV==15
    {
        psgl->s8_voffset_b = VPP_ACM_MIN2(psgl->s8_voffset_b, 0); //shift_b_v<=0
        psgl->s8_voffset_c = VPP_ACM_MIN2(psgl->s8_voffset_c, 0); //shift_c_v<=0
    }
    else if(indexU==15) //blockU==15 && 0<blockV<15
    {
        psgl->s8_uoffset_c = VPP_ACM_MIN2(psgl->s8_uoffset_c, 0); //shift_c_u<=0
        psgl->s8_uoffset_d = VPP_ACM_MIN2(psgl->s8_uoffset_d, 0); //shift_d_u<=0
    }
    else if(indexV==0) //0<blockU<15 && blockV==0
    {
        psgl->s8_voffset_a = VPP_ACM_MAX2(psgl->s8_voffset_a, 0); //shift_a_v>=0
        psgl->s8_voffset_d = VPP_ACM_MAX2(psgl->s8_voffset_d, 0); //shift_d_v>=0
    }
}

//ACM_api main function: initial the acm configurations.
void inter_init_acminfo(HAL_ACMINFO_S * pacm_info, unsigned int u32TypeValue)
{
    if(u32TypeValue == 1)
    {
        //initial the acm on-off of the four regions: 0-enable; 1-disable;
        pacm_info->stAcmBlk0.bacm_blk_en = 0;
        pacm_info->stAcmBlk1.bacm_blk_en = 1;
        pacm_info->stAcmBlk2.bacm_blk_en = 1;
        pacm_info->stAcmBlk3.bacm_blk_en = 0;

        //config shift values of the first region
        pacm_info->stAcmBlk0.u8_uIndex   =  6;
        pacm_info->stAcmBlk0.u8_vIndex   = 10;
        pacm_info->stAcmBlk0.s8_uoffset_a =  12;
        pacm_info->stAcmBlk0.s8_voffset_a =  12;
        pacm_info->stAcmBlk0.s8_uoffset_b =  0;
        pacm_info->stAcmBlk0.s8_voffset_b =  0;
        pacm_info->stAcmBlk0.s8_uoffset_c = -4;
        pacm_info->stAcmBlk0.s8_voffset_c = -4;
        pacm_info->stAcmBlk0.s8_uoffset_d =  10;
        pacm_info->stAcmBlk0.s8_voffset_d =  12;

        //config shift values of the second region
        pacm_info->stAcmBlk1.u8_uIndex   =  6;
        pacm_info->stAcmBlk1.u8_vIndex   =  6;
        pacm_info->stAcmBlk1.s8_uoffset_a = -9;
        pacm_info->stAcmBlk1.s8_voffset_a = -12;
        pacm_info->stAcmBlk1.s8_uoffset_b = -15;
        pacm_info->stAcmBlk1.s8_voffset_b = -10;
        pacm_info->stAcmBlk1.s8_uoffset_c =  0;
        pacm_info->stAcmBlk1.s8_voffset_c =  0;
        pacm_info->stAcmBlk1.s8_uoffset_d = -12;
        pacm_info->stAcmBlk1.s8_voffset_d = -5;

        //config shift values of the third region
        pacm_info->stAcmBlk2.u8_uIndex   =  9;
        pacm_info->stAcmBlk2.u8_vIndex   =  6;
        pacm_info->stAcmBlk2.s8_uoffset_a =  12;
        pacm_info->stAcmBlk2.s8_voffset_a = -6;
        pacm_info->stAcmBlk2.s8_uoffset_b =  0;
        pacm_info->stAcmBlk2.s8_voffset_b =  0;
        pacm_info->stAcmBlk2.s8_uoffset_c =  12;
        pacm_info->stAcmBlk2.s8_voffset_c = -12;
        pacm_info->stAcmBlk2.s8_uoffset_d =  12;
        pacm_info->stAcmBlk2.s8_voffset_d =  -9;

        //config shift values of the fourth region
        pacm_info->stAcmBlk3.u8_uIndex   = 0;
        pacm_info->stAcmBlk3.u8_vIndex   = 0;
        pacm_info->stAcmBlk3.s8_uoffset_a = 0;
        pacm_info->stAcmBlk3.s8_voffset_a = 0;
        pacm_info->stAcmBlk3.s8_uoffset_b = 0;
        pacm_info->stAcmBlk3.s8_voffset_b = 0;
        pacm_info->stAcmBlk3.s8_uoffset_c = 0;
        pacm_info->stAcmBlk3.s8_voffset_c = 0;
        pacm_info->stAcmBlk3.s8_uoffset_d = 0;
        pacm_info->stAcmBlk3.s8_voffset_d = 0;
    }

    //if the region is located at the edge of the [0, 255] plane,
    //make sure the shift point won't overflow.
    inter_acm_boundcheck(&(pacm_info->stAcmBlk0));
    inter_acm_boundcheck(&(pacm_info->stAcmBlk1));
    inter_acm_boundcheck(&(pacm_info->stAcmBlk2));
    inter_acm_boundcheck(&(pacm_info->stAcmBlk3));

    //if two regions are connected, make sure their common points have the same shift.
    inter_acm_infocheck(pacm_info, 0, 1);
    inter_acm_infocheck(pacm_info, 0, 2);
    inter_acm_infocheck(pacm_info, 0, 3);
    inter_acm_infocheck(pacm_info, 1, 2);
    inter_acm_infocheck(pacm_info, 1, 3);
    inter_acm_infocheck(pacm_info, 2, 3);


    /* ACM info display */
    logd("AcmBlk0.bacm_blk_en  = %d\n", (unsigned int)pacm_info->stAcmBlk0.bacm_blk_en );
    logd("AcmBlk1.bacm_blk_en  = %d\n", (unsigned int)pacm_info->stAcmBlk1.bacm_blk_en );
    logd("AcmBlk2.bacm_blk_en  = %d\n", (unsigned int)pacm_info->stAcmBlk2.bacm_blk_en );
    logd("AcmBlk3.bacm_blk_en  = %d\n", (unsigned int)pacm_info->stAcmBlk3.bacm_blk_en );

    logd("Zone A\n");
    logd("AcmBlk0.u8_uIndex   = %d\n", (unsigned int)pacm_info->stAcmBlk0.u8_uIndex  );
    logd("AcmBlk0.u8_vIndex   = %d\n", (unsigned int)pacm_info->stAcmBlk0.u8_vIndex  );
    logd("AcmBlk0.s8_uoffset_a = %d\n", (unsigned int)pacm_info->stAcmBlk0.s8_uoffset_a);
    logd("AcmBlk0.s8_voffset_a = %d\n", (unsigned int)pacm_info->stAcmBlk0.s8_voffset_a);
    logd("AcmBlk0.s8_uoffset_b = %d\n", (unsigned int)pacm_info->stAcmBlk0.s8_uoffset_b);
    logd("AcmBlk0.s8_voffset_b = %d\n", (unsigned int)pacm_info->stAcmBlk0.s8_voffset_b);
    logd("AcmBlk0.s8_uoffset_c = %d\n", (unsigned int)pacm_info->stAcmBlk0.s8_uoffset_c);
    logd("AcmBlk0.s8_voffset_c = %d\n", (unsigned int)pacm_info->stAcmBlk0.s8_voffset_c);
    logd("AcmBlk0.s8_uoffset_d = %d\n", (unsigned int)pacm_info->stAcmBlk0.s8_uoffset_d);
    logd("AcmBlk0.s8_voffset_d = %d\n", (unsigned int)pacm_info->stAcmBlk0.s8_voffset_d);

    logd("Zone B\n");
    logd("AcmBlk1.u8_uIndex   = %d\n", (unsigned int)pacm_info->stAcmBlk1.u8_uIndex  );
    logd("AcmBlk1.u8_vIndex   = %d\n", (unsigned int)pacm_info->stAcmBlk1.u8_vIndex  );
    logd("AcmBlk1.s8_uoffset_a = %d\n", (unsigned int)pacm_info->stAcmBlk1.s8_uoffset_a);
    logd("AcmBlk1.s8_voffset_a = %d\n", (unsigned int)pacm_info->stAcmBlk1.s8_voffset_a);
    logd("AcmBlk1.s8_uoffset_b = %d\n", (unsigned int)pacm_info->stAcmBlk1.s8_uoffset_b);
    logd("AcmBlk1.s8_voffset_b = %d\n", (unsigned int)pacm_info->stAcmBlk1.s8_voffset_b);
    logd("AcmBlk1.s8_uoffset_c = %d\n", (unsigned int)pacm_info->stAcmBlk1.s8_uoffset_c);
    logd("AcmBlk1.s8_voffset_c = %d\n", (unsigned int)pacm_info->stAcmBlk1.s8_voffset_c);
    logd("AcmBlk1.s8_uoffset_d = %d\n", (unsigned int)pacm_info->stAcmBlk1.s8_uoffset_d);
    logd("AcmBlk1.s8_voffset_d = %d\n", (unsigned int)pacm_info->stAcmBlk1.s8_voffset_d);

    logd("Zone C\n");
    logd("AcmBlk2.u8_uIndex   = %d\n", (unsigned int)pacm_info->stAcmBlk2.u8_uIndex  );
    logd("AcmBlk2.u8_vIndex   = %d\n", (unsigned int)pacm_info->stAcmBlk2.u8_vIndex  );
    logd("AcmBlk2.s8_uoffset_a = %d\n", (unsigned int)pacm_info->stAcmBlk2.s8_uoffset_a);
    logd("AcmBlk2.s8_voffset_a = %d\n", (unsigned int)pacm_info->stAcmBlk2.s8_voffset_a);
    logd("AcmBlk2.s8_uoffset_b = %d\n", (unsigned int)pacm_info->stAcmBlk2.s8_uoffset_b);
    logd("AcmBlk2.s8_voffset_b = %d\n", (unsigned int)pacm_info->stAcmBlk2.s8_voffset_b);
    logd("AcmBlk2.s8_uoffset_c = %d\n", (unsigned int)pacm_info->stAcmBlk2.s8_uoffset_c);
    logd("AcmBlk2.s8_voffset_c = %d\n", (unsigned int)pacm_info->stAcmBlk2.s8_voffset_c);
    logd("AcmBlk2.s8_uoffset_d = %d\n", (unsigned int)pacm_info->stAcmBlk2.s8_uoffset_d);
    logd("AcmBlk2.s8_voffset_d = %d\n", (unsigned int)pacm_info->stAcmBlk2.s8_voffset_d);

    logd("Zone D\n");
    logd("AcmBlk3.u8_uIndex   = %d\n", (unsigned int)pacm_info->stAcmBlk3.u8_uIndex  );
    logd("AcmBlk3.u8_vIndex   = %d\n", (unsigned int)pacm_info->stAcmBlk3.u8_vIndex  );
    logd("AcmBlk3.s8_uoffset_a = %d\n", (unsigned int)pacm_info->stAcmBlk3.s8_uoffset_a);
    logd("AcmBlk3.s8_voffset_a = %d\n", (unsigned int)pacm_info->stAcmBlk3.s8_voffset_a);
    logd("AcmBlk3.s8_uoffset_b = %d\n", (unsigned int)pacm_info->stAcmBlk3.s8_uoffset_b);
    logd("AcmBlk3.s8_voffset_b = %d\n", (unsigned int)pacm_info->stAcmBlk3.s8_voffset_b);
    logd("AcmBlk3.s8_uoffset_c = %d\n", (unsigned int)pacm_info->stAcmBlk3.s8_uoffset_c);
    logd("AcmBlk3.s8_voffset_c = %d\n", (unsigned int)pacm_info->stAcmBlk3.s8_voffset_c);
    logd("AcmBlk3.s8_uoffset_d = %d\n", (unsigned int)pacm_info->stAcmBlk3.s8_uoffset_d);
    logd("AcmBlk3.s8_voffset_d = %d\n", (unsigned int)pacm_info->stAcmBlk3.s8_voffset_d);
}

void inter_set_pddef (HAL_LAYER_E enLayer){

    unsigned int u32HistThd[4]   = {8,16,32,64};
    unsigned int u32UmThd[3]     = {8,32,64};
    unsigned int u32PccThd[4]    = {16,32,64,128};
    unsigned int u32ItDiffThd[4] = {4,8,16,32};

    switch(enLayer)
    {
    case HAL_LAYER_VIDEO1:
        {
            hal_set_pd_dir_mch    (enLayer, 0);
            hal_set_pd_stl_blk_thd (enLayer, ((720*480)>>11));
            hal_set_pd_diff_thd   (enLayer, 30);
            hal_set_pd_coring_tkr (enLayer, 128);
            hal_set_pd_coring_norm(enLayer, 16);
            hal_set_pd_coring_blk (enLayer, 16);
            hal_set_pd_pcc_hthd   (enLayer, 16);

            hal_set_pd_lasi_thd   (enLayer, 20);
            hal_set_pd_edge_thd   (enLayer, 15);

            hal_set_pd_hist_thd   (enLayer, u32HistThd);
            hal_set_pd_um_thd     (enLayer, u32UmThd);
            hal_set_pd_pcc_vthd   (enLayer, u32PccThd);
            hal_set_pd_it_diff_thd (enLayer, u32ItDiffThd);

            hal_set_pd_smt_enable (enLayer, 0);
            hal_set_pd_smt_thd    (enLayer, 24);

            break;
        }
    default:
        {
            loge("Error! Vou_SetPdDefThd() Select Wrong Layer ID!\n");
            return ;
        }
    }

    return ;
}

static void inter_deinit_dieinfo(VPP_DIE_ADDR_S *pstDieModule)
{
    BUG_ON(NULL == pstDieModule);

    //释放DIE_INFO缓冲区
    if (pstDieModule->die_VirtAddr!= NULL)
    {
        pstDieModule->die_VirtAddr = NULL;
        pstDieModule->die_BusAddr= 0;
    }
    return;
}
//S00198515 add for DIE 20120828 [end]

/*计算分量的地址和stride*/
static void  inter_calc_inputaddrinfo( VPP_INIMAGE_S *pInImg ,VPP_ADDRINFO_S * pVppAddrInfo)
{
    int stride = 0;

    if((NULL == pInImg) || (NULL == pVppAddrInfo)){
        loge("input parameter is null!\n");
        return;
    }

    //stride = pInImg->width;
    stride = pInImg->stride;
    pVppAddrInfo->vhd_lum_str = stride;

    switch(pInImg->pixformat)
    {
        case VPP_INPUTFMT_YCBCR_SEMIPLANAR_400:
        case VPP_INPUTFMT_YCBCR_SEMIPLANAR_420:
        case VPP_INPUTFMT_YCBCR_SEMIPLANAR_422:
        case VPP_INPUTFMT_YCBCR_SEMIPLANAR_444:
        {
            pVppAddrInfo->vhd_chm_str = stride;
            break;
        }
        case VPP_INPUTFMT_CBYCRY_PACKAGE_422:
        case VPP_INPUTFMT_YCBYCR_PACKAGE_422:
        case VPP_INPUTFMT_YCRYCB_PACKAGE_422:
        case VPP_INPUTFMT_CRYCBY_PACKAGE_422:
        {
            if ( 0 == pInImg->stride)
            {
                logi("stride is zero,so we should computer the stride \n");
                //pVppAddrInfo->vhd_lum_str = 2*stride;
                //pVppAddrInfo->vhd_chm_str = 2*stride;
                pVppAddrInfo->vhd_lum_str = pInImg->width*2;
                pVppAddrInfo->vhd_chm_str = pInImg->width*2;

            }
            else
            {
                pVppAddrInfo->vhd_lum_str = pInImg->stride;
                pVppAddrInfo->vhd_chm_str = pInImg->stride;
            }

            break;
        }

        default:
        {
            loge("Input pixformat is not supported, 0x%x\n", pInImg->pixformat);
            break;
        }
    }

    switch(pInImg->pixformat)
    {
        case VPP_INPUTFMT_CBYCRY_PACKAGE_422:
        case VPP_INPUTFMT_YCBYCR_PACKAGE_422:
        case VPP_INPUTFMT_YCRYCB_PACKAGE_422:
        case VPP_INPUTFMT_CRYCBY_PACKAGE_422:
        {
            pVppAddrInfo->vhd_clum_addr = pInImg->buffer_bus_addr;
            break;
        }

        case VPP_INPUTFMT_YCBCR_SEMIPLANAR_400:
        case VPP_INPUTFMT_YCBCR_SEMIPLANAR_420:
        case VPP_INPUTFMT_YCBCR_SEMIPLANAR_422:
        case VPP_INPUTFMT_YCBCR_SEMIPLANAR_444:
        {
            pVppAddrInfo->vhd_clum_addr = pInImg->buffer_bus_addr;
            pVppAddrInfo->vhd_cchm_addr = pInImg->buffer_bus_addr+pVppAddrInfo->vhd_lum_str*
                                    pInImg->height;
            break;
        }

        default:
        {
            loge("Input pixformat is not supported, 0x%x\n", pInImg->pixformat);
            BUG_ON(NULL == NULL);
            break;
        }
    }

    return;
}

/***********************************************************************************
* Function:       inter_set_inputaddr
* Description:   设置输入图像地址
* Data Accessed:
* Data Updated:
* Input:          HAL_LAYER_E enLayer,
*                 bool bEnable,
*                 VPP_OUTIMAGE_S vppOutImage
* Output:
* Return:        K3_SUCCESS/errorcode
* Others:
***********************************************************************************/
static void inter_set_inputaddr(VPP_PIXELFORMAT_E pixformat, VPP_ADDRINFO_S *pVppAddrInfo,HAL_LAYER_E enLayer)
{
    BUG_ON(pixformat >= VPP_PIXELFORMAT_BUTT);
    BUG_ON(NULL == pVppAddrInfo);
    BUG_ON(enLayer != HAL_LAYER_VIDEO1);

    logd();
    switch(pixformat)
    {
        case  VPP_INPUTFMT_YCBCR_SEMIPLANAR_400:
        case  VPP_INPUTFMT_YCBCR_SEMIPLANAR_420:
        case  VPP_INPUTFMT_YCBCR_SEMIPLANAR_422:
        case  VPP_INPUTFMT_YCBCR_SEMIPLANAR_444:
        case  VPP_INPUTFMT_CBYCRY_PACKAGE_422:
        case  VPP_INPUTFMT_YCBYCR_PACKAGE_422:
        case  VPP_INPUTFMT_YCRYCB_PACKAGE_422:
        case  VPP_INPUTFMT_CRYCBY_PACKAGE_422:
        {
            hal_set_layer_addr(enLayer,0,
                                  pVppAddrInfo->vhd_clum_addr+OFFSET,pVppAddrInfo->vhd_cchm_addr+OFFSET,pVppAddrInfo->vhd_lum_str, pVppAddrInfo->vhd_chm_str);
            break;
        }
        default:
        {
            loge("Input format is not supported, 0x%x\n", pixformat);
            BUG_ON(NULL == NULL);
            break;
        }
    }

    return;
}

/***********************************************************************************
* Function:       inter_set_wbcconfig
* Description:   设置输出图像的信息
* Data Accessed:
* Data Updated:
* Input:          HAL_LAYER_E enLayer,
*                 bool bEnable,
*                 VPP_OUTIMAGE_S vppOutImage
* Output:
* Return:        K3_SUCCESS/errorcode
* Others:
***********************************************************************************/
static void inter_set_wbcconfig(HAL_LAYER_E enLayer, bool bEnable, VPP_OUTIMAGE_S *pVppOutImage)
{
    BUG_ON(NULL == pVppOutImage);
    BUG_ON(HAL_LAYER_WBC1 != enLayer);

    logd();

    /*enable wbc1*/
    hal_set_wbc_enable(enLayer, bEnable);

    /*set wbc busaddr*/
    hal_set_wbc_addr(enLayer, pVppOutImage->buffer_bus_addr+OFFSET);

    /*set wbc1 stride*/
    hal_set_wbc_stride(enLayer, pVppOutImage->stride);
    //hal_set_wbc_stride(enLayer, 0x1000);

    /*set wbc1 mode, only surport off line*/
    hal_set_wbc_md(HAL_WBC_RESOSEL_OFL);

    hal_set_wbc_wr_mode(enLayer, 0);

    /*set wbc1 pixformat*/
    hal_set_wbc_out_fmt(enLayer, pVppOutImage->pixformat);

}

static int inter_check_para( VPP_INIMAGE_S * pInImg, VPP_ADDRINFO_S *pVppAddrInfo)
{
    if((NULL == pInImg) || (NULL == pVppAddrInfo)){
        loge("input parameter is null!\n");
        return K3_FAILURE;
    }
    /*数据格式相关检测*/
    switch(pInImg->pixformat)
    {
        case VPP_INPUTFMT_YCBCR_SEMIPLANAR_400:
        case VPP_INPUTFMT_YCBCR_SEMIPLANAR_420:
        case VPP_INPUTFMT_YCBCR_SEMIPLANAR_422:
        case VPP_INPUTFMT_YCBCR_SEMIPLANAR_444:
        {
            if(0 == pVppAddrInfo->vhd_clum_addr )
            {
                loge("SEMIPALNAR dataformat not config Y addr");
                return K3_FAILURE;
            }

            if( 0 == pVppAddrInfo->vhd_cchm_addr )
            {
                loge("SEMIPALNAR dataformat not config C addr");
                return K3_FAILURE;
            }

            if( 0 == pVppAddrInfo->vhd_lum_str )
            {
                loge("SEMIPALNAR dataformat not config Y stride");
                return K3_FAILURE;
            }

            if( 0 == pVppAddrInfo->vhd_chm_str )
            {
                loge("SEMIPALNAR dataformat not config C stride");
                return K3_FAILURE;
            }
            break;
        }
        case VPP_INPUTFMT_CBYCRY_PACKAGE_422:
        case VPP_INPUTFMT_YCBYCR_PACKAGE_422:
        case VPP_INPUTFMT_YCRYCB_PACKAGE_422:
        case VPP_INPUTFMT_CRYCBY_PACKAGE_422:
        {
            if( 0 == pVppAddrInfo->vhd_clum_addr )
            {
                loge("PACKAGE dataformat not config addr");
                return K3_FAILURE;
            }

            if( 0 == pVppAddrInfo->vhd_lum_str )
            {
                loge("PACKAGE dataformat not config stride");
                return K3_FAILURE;
            }
            break;
        }
        default:
        {
            loge("Input format is not supported, 0x%x\n", pInImg->pixformat);
            return K3_FAILURE;
        }
    }

    return K3_SUCCESS;
}

/***********************************************************************************
* Function:       inter_init
* Description:   初始化VPP模块，支持重复打开
* Data Accessed:
* Data Updated:
* Input:          NA
* Output:
* Return:        K3_SUCCESS/errorcode
* Others:
***********************************************************************************/
int inter_init(void)
{
    logd("enter inter Inited!\n");

    //创建公共组件

    if (inter_init_dieinfo(&s_stDieAddr) != K3_SUCCESS)
    {
        loge("VPP malloc die buf failed.\n");
        return K3_FAILURE;
    }

    /*if (inter_create_mmu_table(&s_MmuAddr) != K3_SUCCESS)
    {
        loge("MMU malloc  buf failed.\n");
        return K3_FAILURE;
    }*/
    inter_init_layer(HAL_LAYER_VIDEO1);

    return K3_SUCCESS;
}

/***********************************************************************************
* Function:       inter_deinit
* Description:   去初始化VPP模块，支持重复去初始化
* Data Accessed:
* Data Updated:
* Input:          NA
* Output:
* Return:        K3_SUCCESS/errorcode
* Others:
***********************************************************************************/
void inter_deinit(void)
{
    logd("enter deinit\n");
    //hal_set_mmu_enable(HAL_LAYER_VIDEO1,0,0,0);
    inter_deinit_layer(HAL_LAYER_VIDEO1);

    inter_deinit_dieinfo(&s_stDieAddr);
    vpp_free_buffer(&k3vpp);

    return;
}

/***********************************************************************************
* Function:      inter_open
* Description:   关闭视频层
* Data Accessed:
* Data Updated:
* Input:         enVo --视频层ID
* Output:
* Return:        K3_SUCCESS/errorcode
* Others:
***********************************************************************************/
void inter_open(HAL_LAYER_E enLayer)
{
    VPP_VPP_S  *pstVpp = NULL;

    logd("enter open enLayer = %d!\n",enLayer);

    VPP_GET_HANDLE(enLayer, pstVpp);

    pstVpp->bopened = true;

    return;
}

/***********************************************************************************
* Function:      inter_close
* Description:    关闭视频层
* Data Accessed:
* Data Updated:
* Input:         enVo --视频层ID
* Output:
* Return:        K3_SUCCESS/errorcode
* Others:
**********************************************************************************/
void inter_close(HAL_LAYER_E enLayer)
{
    VPP_VPP_S     *pstVpp = NULL;
    logd("enter close enLayer = %d!\n",enLayer);

    //获取VPP句柄
    VPP_GET_HANDLE(enLayer, pstVpp);

    if (pstVpp->bopened != true)
    {
        logi("the device isn't open\n");
        return;
    }

    /*close video1 layer*/
    hal_enable_layer(enLayer, false);

    pstVpp->bopened = false;

    return;
}

/***********************************************************************************
* Function:      inter_start
* Description:   启动配置VPP
* Data Accessed:
* Data Updated:
* Input:        vppConfig 输入VPP配置参数
* Output:
* Return:        K3_SUCCESS/errorcode
* Others:
***********************************************************************************/
int inter_start(VPP_CONFIG_S* pVppConfig)
{
    VPP_INIMAGE_S  *pInImage = NULL;
    VPP_OUTIMAGE_S *pOutImage = NULL;
    VPP_INCROPPING_S *pInCrop = NULL;
    VPP_ADDRINFO_S addrInfo = {0};
    HAL_RECT_S vhdrect = {0};
    //add for acm
    HAL_ACMINFO_S *pstAcmInfo = NULL;

    logd("enter inter start \n");
    if(NULL == pVppConfig){
        loge("pVppConfig is null!\n");
        return K3_FAILURE;
    }

    pInImage = &(pVppConfig->vpp_in_img);
    pOutImage = &(pVppConfig->vpp_out_img);
    pInCrop = &(pVppConfig->vpp_in_crop);


    vhdrect.s32DXL = pOutImage->width;   /*显示列结束坐标, 以帧高度为参考，以行为单位*/
    vhdrect.s32DYL = pOutImage->height;
    vhdrect.u32InWidth   = pInImage->width;
    vhdrect.u32InHeight  = pInImage->height;
    vhdrect.u32OutWidth  = pOutImage->width;
    vhdrect.u32OutHeight = pOutImage->height;

    inter_calc_inputaddrinfo(pInImage, &addrInfo);
    if (K3_SUCCESS != inter_check_para( pInImage, &addrInfo))
    {
        loge("inter_check_para fail\n");
        return K3_FAILURE;
    }

    hal_enable_layer(HAL_LAYER_VIDEO1, true);
    hal_set_zme_copy(0);
    hal_set_layer_data_fmt( HAL_LAYER_VIDEO1, pInImage->pixformat,pInImage->width,pInImage->height);
    hal_set_layer_rect(HAL_LAYER_VIDEO1,vhdrect );
    if ( 1 == pVppConfig->crop_enable )
    {
        logi("crop enable\n");
        vhdrect.u32InWidth   = pInCrop->width;
        vhdrect.u32InHeight  = pInCrop->height;
    }
    inter_set_inputaddr(pInImage->pixformat, &addrInfo, HAL_LAYER_VIDEO1);
    //S00198515 add for DIE enable 20120828 [start]
    if(0 == pVppConfig->bdie_enbale){
        pVppConfig->vpp_die_addrinfo.cluma_addr = addrInfo.vhd_clum_addr;
        pVppConfig->vpp_die_addrinfo.cchroma_addr= addrInfo.vhd_cchm_addr;
    }
    addrInfo.lluma_addr = pVppConfig->vpp_die_addrinfo.lluma_addr;
    addrInfo.lchroma_addr = pVppConfig->vpp_die_addrinfo.lchroma_addr;
    addrInfo.cluma_addr = pVppConfig->vpp_die_addrinfo.cluma_addr;
    addrInfo.cchroma_addr = pVppConfig->vpp_die_addrinfo.cchroma_addr;
    addrInfo.nluma_addr = pVppConfig->vpp_die_addrinfo.nluma_addr;
    addrInfo.nchroma_addr = pVppConfig->vpp_die_addrinfo.nchroma_addr;
    //S00198515 add for DIE enable 20120828 [end]

    hal_set_zme_reso(HAL_LAYER_VIDEO1, vhdrect );

    hal_set_read_mode(HAL_LAYER_VIDEO1, HAL_PROGRESSIVE, HAL_PROGRESSIVE);

    inter_set_wbcconfig(HAL_LAYER_WBC1, true, pOutImage);
    // must cfg coef update && the coef addr must before it.
    hal_set_layer_para_upd(HAL_LAYER_VIDEO1, HAL_COEFMODE_HOR);
    hal_set_layer_para_upd(HAL_LAYER_VIDEO1, HAL_COEFMODE_VER);

    /***********************acm******start*************************************/
    pstAcmInfo = (HAL_ACMINFO_S*)kmalloc(sizeof(HAL_ACMINFO_S),GFP_KERNEL);
    if(NULL == pstAcmInfo){
        loge("kmalloc pstAcmInfo is fail!\n");
        return K3_FAILURE;
    }
    /*ACM典型模式配置*/
    if(1 == pVppConfig->acm_en)
    {
        /* Config ACM enable and para */
        inter_init_acminfo(pstAcmInfo,1);

        /* Config Zone0 ACM enable */
        hal_set_acm_enable(HAL_ACMBLK_ID0, pstAcmInfo->stAcmBlk0.bacm_blk_en);
        hal_set_acm_coef  (HAL_ACMBLK_ID0, pstAcmInfo->stAcmBlk0);

        /* Config Zone1 ACM enable */
        hal_set_acm_enable(HAL_ACMBLK_ID1, pstAcmInfo->stAcmBlk1.bacm_blk_en);
        hal_set_acm_coef  (HAL_ACMBLK_ID1, pstAcmInfo->stAcmBlk1);

        /* Config Zone2 ACM enable */
        hal_set_acm_enable(HAL_ACMBLK_ID2, pstAcmInfo->stAcmBlk2.bacm_blk_en);
        hal_set_acm_coef  (HAL_ACMBLK_ID2, pstAcmInfo->stAcmBlk2);

        /* Config Zone3 ACM enable */
        hal_set_acm_enable(HAL_ACMBLK_ID3, pstAcmInfo->stAcmBlk3.bacm_blk_en);
        hal_set_acm_coef  (HAL_ACMBLK_ID3, pstAcmInfo->stAcmBlk3);
    }
    /*ACM Debug 模式配置*/

    else if(2 == pVppConfig->acm_en)
    {
        hal_set_acm_test_enable(1);

        /* Config ACM enable and para */
        inter_init_acminfo(pstAcmInfo,1);

        /* Config Zone0 ACM enable */
        hal_set_acm_enable(HAL_ACMBLK_ID0, pstAcmInfo->stAcmBlk0.bacm_blk_en);
        hal_set_acm_coef  (HAL_ACMBLK_ID0, pstAcmInfo->stAcmBlk0);

        /* Config Zone1 ACM enable */
        hal_set_acm_enable(HAL_ACMBLK_ID1, pstAcmInfo->stAcmBlk1.bacm_blk_en);
        hal_set_acm_coef  (HAL_ACMBLK_ID1, pstAcmInfo->stAcmBlk1);

        /* Config Zone2 ACM enable */
        hal_set_acm_enable(HAL_ACMBLK_ID2, pstAcmInfo->stAcmBlk2.bacm_blk_en);
        hal_set_acm_coef  (HAL_ACMBLK_ID2, pstAcmInfo->stAcmBlk2);

        /* Config Zone3 ACM enable */
        hal_set_acm_enable(HAL_ACMBLK_ID3, pstAcmInfo->stAcmBlk3.bacm_blk_en);
        hal_set_acm_coef  (HAL_ACMBLK_ID3, pstAcmInfo->stAcmBlk3);
    }
    else
    {
        /* Config All Zone ACM disble */
        hal_set_acm_enable(HAL_ACMBLK_ID0, 0);
        hal_set_acm_enable(HAL_ACMBLK_ID1, 0);
        hal_set_acm_enable(HAL_ACMBLK_ID2, 0);
        hal_set_acm_enable(HAL_ACMBLK_ID3, 0);
    }
	if(NULL != pstAcmInfo){
        kfree(pstAcmInfo);
        pstAcmInfo = NULL;
	}
    /******************acm end***********************************/

    /**************************mmu start****************************************/

    if(1 == pVppConfig->mmu_enable){

        logd("MMU Type Mode Enable!\n");
        hal_set_mmu_enable(HAL_LAYER_VIDEO1,pVppConfig->edie_mode,pVppConfig->vpp_in_img.pixformat,pVppConfig->bdie_enbale,1);

        hal_set_mmu_core(2);
        hal_set_mmu_cfg(2);
        hal_set_mmu_in_tile_enb(pVppConfig->vpp_in_img.in_mem_mode);
        hal_set_mmu_out_tile_enb(pVppConfig->vpp_out_img.out_mem_mode);
        if(VPP_TILE == pVppConfig->vpp_in_img.in_mem_mode){
            hal_set_mmu_bit7_xor(1,0);
            hal_set_mmu_tile_num(pVppConfig->vpp_in_img.stride,0);
        }
        if(VPP_TILE == pVppConfig->vpp_out_img.out_mem_mode){
            hal_set_mmu_bit7_xor(1,1);
            hal_set_mmu_tile_num(pVppConfig->vpp_out_img.stride,1);
        }
        hal_set_mmu_ch_addr(pVppConfig);
        hal_set_mmu__start(1);
    }
    /**************************mmu end****************************************/

    //S00198515 add for DIE enable 20120828 [start]
    if(1 == pVppConfig->bdie_enbale)
    {
        logd("Die Type Mode Enable!\n");
        hal_set_die_enable      (HAL_LAYER_VIDEO1, 1,1);
        hal_set_die_chm_mode  (HAL_LAYER_VIDEO1, pVppConfig->edie_mode);
        hal_set_die_luma_mode(HAL_LAYER_VIDEO1, pVppConfig->edie_mode);

        hal_set_die_stp_rst      (HAL_LAYER_VIDEO1, 0);
        hal_set_die_outsel      (HAL_LAYER_VIDEO1, 0);
        hal_set_die_stp_update   (HAL_LAYER_VIDEO1, 1);

        hal_set_die_st_mode(HAL_LAYER_VIDEO1, VPP_DIE_STMD_KLM);

        //die stream order, 0:TBTB,1:BTBT
        hal_set_field_order(HAL_LAYER_VIDEO1, pVppConfig->edie_fod);//S00198515 modify 20120904
        /*if(bTop == 1)
        {
            hal_set_field_order(HAL_LAYER_VIDEO1, VPP_T_FIRST);
            bTop = 0;
        }
        else
        {
            hal_set_field_order(HAL_LAYER_VIDEO1, VPP_B_FIRST);
            bTop = 1;
        }*/


        hal_set_layer_addr(HAL_LAYER_VIDEO1, 0, addrInfo.cluma_addr+OFFSET, addrInfo.cchroma_addr+OFFSET, addrInfo.vhd_lum_str, addrInfo.vhd_chm_str);
        hal_set_layer_addr(HAL_LAYER_VIDEO1, 1, addrInfo.lluma_addr+OFFSET, addrInfo.lchroma_addr+OFFSET, addrInfo.vhd_lum_str, addrInfo.vhd_chm_str);
        hal_set_layer_addr(HAL_LAYER_VIDEO1, 2, addrInfo.nluma_addr+OFFSET, addrInfo.nchroma_addr+OFFSET, addrInfo.vhd_lum_str, addrInfo.vhd_chm_str);

        hal_set_die_addr(HAL_LAYER_VIDEO1, VPP_DIE_STMD_KLM, s_u32DieAddrCtrl, 0);

        hal_set_read_mode(HAL_LAYER_VIDEO1, HAL_BOTTOM, HAL_BOTTOM);//S00198515 modify 20120904
        /*if(bTop == 1)
        {
            hal_set_read_mode(HAL_LAYER_VIDEO1, HAL_TOP, HAL_TOP);
            bTop = 0;
        }
        else
        {
            hal_set_read_mode(HAL_LAYER_VIDEO1, HAL_BOTTOM, HAL_BOTTOM);
            bTop = 1;
        }*/
    }
    if(1 == pVppConfig->pd_enable){
        logi("====pd-enable==\n");
        inter_set_pddef (HAL_LAYER_VIDEO1);
    }
    //S00198515 add for DIE enable 20120828 [end]
    hal_set_regup(HAL_LAYER_VIDEO1);

    return K3_SUCCESS;
}


