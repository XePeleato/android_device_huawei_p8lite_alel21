
#include <linux/slab.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/core.h>
#include <linux/mmc/sd.h>
#include <linux/kernel.h>
#include <linux/scatterlist.h>
#include <linux/hisi/drv_sd_if.h>
/********************************SD start***********************************/
extern struct semaphore g_sem_sdcard_wr_test;

/*保存结构体*/
struct mmc_host *g_host = NULL;

/*数据传输使用结构体*/
struct scatterlist *g_sd_sg;
int                 g_sgcnt = 0;

 int sd_check_result(struct mmc_card *card,struct mmc_request *mrq)
 {
     int ret;

     if(!mrq || !mrq->cmd || !mrq->data){

        (void)printk(KERN_DEBUG"sd_check_result para is error!\n");
        return -1;

     }

     ret = 0;

     if (!ret && mrq->cmd->error)
         ret = mrq->cmd->error;
     if (!ret && mrq->data->error)
         ret = mrq->data->error;
     if (!ret && mrq->stop && mrq->stop->error)
         ret = mrq->stop->error;
     if (!ret && mrq->data->bytes_xfered !=
         mrq->data->blocks * mrq->data->blksz)
         ret = RESULT_FAIL;

     if (ret == -EINVAL)
         ret = RESULT_UNSUP_HOST;

     (void)printk(KERN_DEBUG"sd_transfer is over!\n");

     return ret;

 }


 int sd_wait_busy(struct mmc_card *card)
 {
     int ret, busy;
     struct mmc_command cmd;

     busy = 0;
     do {
         (void)memset(&cmd, 0, sizeof(struct mmc_command));

         cmd.opcode = MMC_SEND_STATUS;
         cmd.arg = card->rca << 16;
         cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;

         ret = mmc_wait_for_cmd(card->host, &cmd, 0);
         if (ret)
             break;

         if (!busy && !(cmd.resp[0] & R1_READY_FOR_DATA)) {
             busy = 1;
             (void)printk(KERN_INFO "%s: Warning: Host did not "
                 "wait for busy state to end.\n",
                 mmc_hostname(card->host));
         }
     } while (!(cmd.resp[0] & R1_READY_FOR_DATA));

     return ret;
 }


 void sd_prepare_mrq(struct mmc_card *card,
                     struct mmc_request *mrq,
                     struct scatterlist *sg,
                     unsigned sg_len,
                     unsigned dev_addr,
                     unsigned blocks,
                     unsigned blksz,
                     int write)
 {
     if(!mrq || !mrq->cmd || !mrq->data || !mrq->stop){

        (void)printk(KERN_DEBUG"sd_prepare_mrq para is error!\n");
        return;

     }

     if (blocks > 1) {
         mrq->cmd->opcode = write ?
             MMC_WRITE_MULTIPLE_BLOCK : MMC_READ_MULTIPLE_BLOCK;
     } else {
         mrq->cmd->opcode = write ?
             MMC_WRITE_BLOCK : MMC_READ_SINGLE_BLOCK;
     }

     mrq->cmd->arg = dev_addr;
     if (!mmc_card_blockaddr(card))
         mrq->cmd->arg <<= 9;

     mrq->cmd->flags = MMC_RSP_R1 | MMC_CMD_ADTC;

     if (blocks == 1)
         mrq->stop = NULL;
     else {
         mrq->stop->opcode = MMC_STOP_TRANSMISSION;
         mrq->stop->arg = 0;
         mrq->stop->flags = MMC_RSP_R1B | MMC_CMD_AC;
     }

     mrq->data->blksz = blksz;
     mrq->data->blocks = blocks;
     mrq->data->flags = write ? MMC_DATA_WRITE : MMC_DATA_READ;
     mrq->data->sg = sg;
     mrq->data->sg_len = sg_len;

     mmc_set_data_timeout(mrq->data, card);
 }


/*****************************************************************************
* 函 数 名  : sd_sg_init_table
*
* 功能描述  : SD多块数据传输sg list初始化
*
* 输入参数  :  const void *buf        待操作的buffer地址
               unsigned int buflen    待操作的buffer大小,小于32K，为512B的整数倍;
                                      大于32K，为32KB的整数倍，最大128K

* 输出参数  : NA
*
* 返 回 值  : 0 : 成功；其它: 失败
*
* 其它说明  : NA
*
*****************************************************************************/
int sd_sg_init_table(const void *buf,unsigned int buflen)
{
    int cnt = 0;
    int sgcnt = 0;
    u8 *dataBuf = (u8*)buf;
    struct scatterlist *sg;
    struct scatterlist *sgNode;


    g_sd_sg = NULL;
    g_sgcnt = 0;

    if ((0 == buflen)||(NULL == buf)|| (0 != (buflen % 512)))
    {
        (void)printk("sd_sg_init_table para is err!\n");
        return -1;
    }
    if (buflen < SD_TRACE_CLUSTER_SIZE)
    {
        sgcnt = 1;
    }
    else if ( 0 == buflen % SD_TRACE_CLUSTER_SIZE )
    {
        sgcnt = ( buflen/SD_TRACE_CLUSTER_SIZE );
    }
    else
    {
        (void)printk("sd_sg_init_table buf isn't n*32k!\n");
        return -1;
    }

    /*每一个block一个 scatterlist*/
    sg = (struct scatterlist *)kzalloc(sizeof(struct scatterlist)*sgcnt, GFP_KERNEL);
    if (NULL == sg)
    {
        (void)printk("sg kmalloc fail!\n");
        return -1;
    }

    sg_init_table(sg, sgcnt);

    if (1 == sgcnt)
    {
        sg_set_buf(sg, (const void *)dataBuf, buflen);
    }
    else
    {
        for_each_sg(sg, sgNode, sgcnt, cnt)
        {
            sg_set_buf(sgNode, (const void *)dataBuf, SD_TRACE_CLUSTER_SIZE);
            dataBuf += SD_TRACE_CLUSTER_SIZE;
        }
    }

    g_sd_sg = sg;
    g_sgcnt = sgcnt;


    return 0;

}

EXPORT_SYMBOL(sd_sg_init_table);

/*****************************************************************************
* 函 数 名  : sd_get_capacity
*
* 功能描述  : 卡容量查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 失败；>0: 卡容量
*
* 其它说明  : NA
*
*****************************************************************************/
int sd_get_capacity(void)
{
    if (g_host)
    {
        if(g_host->ops->get_cd(g_host)){

            return (g_host->card->csd.capacity << (g_host->card->csd.read_blkbits - 9));
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }

}

EXPORT_SYMBOL(sd_get_capacity);

/*****************************************************************************
* 函 数 名  : sd_get_status
*
* 功能描述  : 卡在位查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 在位；-1: 不在位
*
* 其它说明  : NA
*
*****************************************************************************/
int sd_get_status(void)
{
    if (NULL == g_host)
    {
        return -1;
    }
    else
    {
        if(g_host->ops->get_cd(g_host)){

            return 0;
        }
        else
        {
            return -1;
        }
    }
}

EXPORT_SYMBOL(sd_get_status);

/*****************************************************************************
* 函 数 名  : sd_multi_transfer
*
* 功能描述  : SD多块数据传输。
*
* 输入参数  : unsigned dev_addr   待写入的SD block 地址
              unsigned blocks     待写入的block个数
              unsigned blksz      每个block的大小，单位字节
              int write           读写标志位，写:1 ; 读:0
* 输出参数  : NA
*
* 返 回 值  : 0 : 成功；其它: 失败
*
* 其它说明  : NA
*
*****************************************************************************/
int sd_multi_transfer(unsigned dev_addr, unsigned blocks, unsigned blksz, int write)
{
    struct mmc_request mrq;
    struct mmc_command cmd;
    struct mmc_command stop;
    struct mmc_data data;
    int ret;



    static unsigned int ploop = 0;

    if (NULL == g_sd_sg)
    {
        (void)printk("sd_multi_transfer para g_sd_sg=NULL!\n");
        return -1;
    }

    if ((NULL == g_host)||(0 == g_sgcnt))
    {
        kzfree(g_sd_sg);
        (void)printk("sd_multi_transfer para is err,g_host =%d,g_sgcnt =%d!\n", (int)g_host,g_sgcnt);
        return -1;
    }

    mmc_claim_host(g_host);

    (void)printk(KERN_DEBUG"[%x:%.8x:%x]\n",write,dev_addr,(blksz * blocks));

    if (++ploop%8 == 0)
        (void)printk(KERN_DEBUG"\n");

    memset(&mrq, 0, sizeof(struct mmc_request));
    memset(&cmd, 0, sizeof(struct mmc_command));
    memset(&data, 0, sizeof(struct mmc_data));
    memset(&stop, 0, sizeof(struct mmc_command));

    mrq.cmd = &cmd;
    mrq.data = &data;
    mrq.stop = &stop;

    sd_prepare_mrq(g_host->card,&mrq, g_sd_sg, g_sgcnt, dev_addr,
        blocks, blksz, write);

    mmc_wait_for_req(g_host, &mrq);

    (void)sd_wait_busy(g_host->card);

    mmc_release_host(g_host);

    ret = sd_check_result(g_host->card, &mrq);

    /*传输完成，需要删除sg链表*/
    kzfree(g_sd_sg);
    g_sd_sg = NULL;


    return ret;
}

EXPORT_SYMBOL(sd_multi_transfer);


/********************************SD end*************************************/
