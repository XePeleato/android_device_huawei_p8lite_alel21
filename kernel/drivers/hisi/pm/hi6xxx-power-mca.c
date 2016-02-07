#include <linux/hisi/pm/hi6xxx-power-mca-prot.h>
#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>
#include <linux/slab.h>
#include <linux/hisi/ipc_msg.h>

#ifdef GENERIC_PROJECT
#define MCA_INST_IPC_ID                 1   //fIXME:定义在typedef enum tagIPC_INT_LEV_E中
#endif

#define MCA_DEBUG printk
#if 0 //w00176398
/*stub *************************************/

#define MAILBOX_OK   (0)
#define BSP_MAILBOX_CHANNEL_MCU_TO_ACPU_MCA_CH 3
#define BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_MCA_CH 1
unsigned long mailbox_send_msg(
                unsigned long           MailCode,
                void                   *pData,
                unsigned long           Lengt)
{ return MAILBOX_OK; }


unsigned long mailbox_reg_msg_cb(
                unsigned long           MailCode,
                void *  pFun,
                void                   *UserHandle)
{ return MAILBOX_OK; }


unsigned long mailbox_read_msg_data(
                void                   *MailHandle,
                unsigned char          *pData,
                unsigned long          *pSize)
{ return MAILBOX_OK; }

/*stub end*/


struct mca_sblock* slist[MCA_MAX_SESSION_NUM];

MCA_LOCAL int mca_create_session()
{
    int idx;

    for(idx=0; idx<MCA_MAX_SESSION_NUM; idx++)
    {
        if(!slist[idx])
        {
            slist[idx] = (struct mca_sblock*)mca_malloc(sizeof(struct mca_sblock));

            mca_sem_init(&slist[idx]->sem, 0);

            return idx;
        }
    }

    return -1;
}

MCA_LOCAL int mca_remove_session(int sid)
{
    if(!mca_valid_sid(sid))
        return -1;

    if(!slist[sid])
        return -1;

    mca_sem_up(&slist[sid]->sem);

    mca_sem_del(&slist[sid]->sem);

    mca_free(slist[sid]);

    slist[sid] = NULL;

    return 0;
}

MCA_LOCAL int mca_save_session(int sid, struct mca_sdata sdata)
{
    if(!mca_valid_sid(sid))
        return -1;

    if(!slist[sid])
        return -1;

    slist[sid]->sdata = sdata;

    return 0;
}

MCA_LOCAL int mca_read_session(int sid, struct mca_sdata* sbuf)
{
    if(!mca_valid_sid(sid))
        return -1;

    if(!slist[sid])
        return -1;

    if(!sbuf)
        return -1;

    *sbuf = slist[sid]->sdata;

    return 0;
}

MCA_LOCAL int mca_lock_session(int sid, int time_out)
{
    if(!mca_valid_sid(sid))
        return -1;

    if(!slist[sid])
        return -1;

    if(!time_out)
        mca_sem_down(&slist[sid]->sem);
    else
        if(mca_sem_down_timeout(&slist[sid]->sem, time_out))
            return -1;

    return 0;
}

MCA_LOCAL int mca_unlock_session(int sid)
{
    if(!mca_valid_sid(sid))
        return -1;

    if(!slist[sid])
        return -1;

    mca_sem_up(&slist[sid]->sem);

    return 0;
}

MCA_LOCAL int mca_is_session_locked(int sid)
{
    return mca_sem_count(&slist[sid]->sem) == 0;
}

int mca_send_no_use(int cmd_id, union mca_udata_req req_data, union mca_udata_rsp* p_rsp_data, int time_out)
{
    int sid;
    struct mca_sdata sdata = {0};
    struct mca_frame frame = {0};

    // If user need response data, p_rsp_data != NULL
    if(p_rsp_data)
    {
        sid = mca_create_session();
        if(sid < 0)
            return -1;

        sdata.p_rsp = p_rsp_data;

        if(mca_save_session(sid, sdata))
            goto ERR_DESTROY_EXIT;

        memset(&frame, 0, sizeof(struct mca_frame));
        frame.sid    = sid;
        frame.stype  = MCA_SESSION_SYNC;
        frame.cmd_id = cmd_id;
        frame.udata  = req_data;
        if(MAILBOX_OK != mailbox_send_msg(
                BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_MCA_CH,
                &frame,
                sizeof(struct mca_frame)))
            goto ERR_DESTROY_EXIT;

        // wait for response.
        if(mca_lock_session(sid, time_out) != 0)
        {
            // may response hare after time_out.
            mca_lock_irq();

            // check if responsed after time_out.
            if(mca_is_session_locked(sid))
                mca_unlock_session(sid); /* session shoud be kept untill rsp. */
            else
                mca_remove_session(sid); /* destroy if responsed. */

            mca_unlock_irq();

            return -1;
        }

        // if all ok.
        mca_remove_session(sid);
    }
    else /* If p_rsp_data == NULL, we'll think user needn't response.  */
    {
        memset(&frame, 0, sizeof(struct mca_frame));
        frame.stype  = MCA_SESSION_NRSP;
        frame.cmd_id = cmd_id;
        frame.udata  = req_data;
        if(MAILBOX_OK != mailbox_send_msg(
                BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_MCA_CH,
                &frame,
                sizeof(struct mca_frame)))
            return -1;
    }

    return 0;

ERR_DESTROY_EXIT:

    mca_remove_session(sid);

    return -1;
}

int mca_post(int cmd_id, union mca_udata_req req_data, mca_post_cb post_cb, void* private_data, int time_out)
{
    int sid;
    struct mca_sdata sdata = {0};
    struct mca_frame frame = {0};

    if(post_cb)
    {
        sid = mca_create_session();
        if(sid < 0)
            return -1;

        sdata.post_cb      = post_cb;
        sdata.private_data = private_data;

        if(mca_save_session(sid, sdata))
            goto ERR_DESTROY_EXIT;

        memset(&frame, 0, sizeof(struct mca_frame));
        frame.sid    = sid;
        frame.stype  = MCA_SESSION_ASYN;
        frame.cmd_id = cmd_id;
        frame.udata  = req_data;
        if(MAILBOX_OK != mailbox_send_msg(
                BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_MCA_CH,
                &frame,
                sizeof(struct mca_frame)))
            goto ERR_DESTROY_EXIT;
    }
    else
    {
        memset(&frame, 0, sizeof(struct mca_frame));
        frame.stype  = MCA_SESSION_NRSP;
        frame.cmd_id = cmd_id;
        frame.udata  = req_data;
        if(MAILBOX_OK != mailbox_send_msg(
                BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_MCA_CH,
                &frame,
                sizeof(struct mca_frame)))
            return -1;
    }

    //FIXME: time_out not use. session may never destroy.
    return 0;

ERR_DESTROY_EXIT:

    mca_remove_session(sid);

    return -1;
}

int mca_inst(int cmd_id, union mca_udata_req req_data)
{
#if 0
    struct mca_frame frame = {0};

    if(*(int*)MCA_INST_DATA_MUTEX_ADDR)
        return -1;

    // FIXME: DANGEROUS! USE RES LOCK!
    *(int*)MCA_INST_DATA_MUTEX_ADDR = 1;

    memset(&frame, 0, sizeof(struct mca_frame));
    frame.stype  = MCA_SESSION_NRSP;
    frame.cmd_id = cmd_id;
    frame.udata  = req_data;

    memcpy(MCA_INST_DATA_ADDR, &frame, sizeof(struct mca_frame));

    BSP_IPC_IntSend(IPC_TARGET_CPU_MCU, MCA_INST_IPC_ID);
#endif
    return 0;
}

MCA_LOCAL void mca_rsp_isr(void *UserHandle, void *MailHandle, unsigned long MailLen)
{
#if 0
    struct mca_frame frame = {0};
    struct mca_sdata sdata = {0};
    unsigned long frame_size;

    if(mailbox_read_msg_data(MailHandle, (unsigned char*)&frame, &frame_size) != MAILBOX_OK)
        return;

    if(frame.cmd_id != MCA_CMD_RSP)
        return;

    if(mca_read_session(frame.sid, &sdata))
        return;

    if(frame.stype == MCA_SESSION_SYNC)
    {
        // check if session expired.
        if(mca_is_session_locked(frame.sid))
        {
            *sdata.p_rsp = frame.udata;
            mca_unlock_session(frame.sid);
        }
        else
            mca_remove_session(frame.sid);
    }
    else if(frame.stype == MCA_SESSION_ASYN)
    {
        if(sdata.post_cb)
            sdata.post_cb(frame.udata, sdata.private_data);

        mca_remove_session(frame.sid);
    }
#endif
}

void mca_init()
{
    mailbox_reg_msg_cb(BSP_MAILBOX_CHANNEL_MCU_TO_ACPU_MCA_CH, mca_rsp_isr, 0);
}
#endif

int mca_send(int cmd_id, union mca_udata_req req_data, union mca_udata_rsp* p_rsp_data, int time_out)
{
    struct ipc_msg msg;
    struct mca_frame frame = {0};
    unsigned int data_len = 0;
    int ret = 0;

    if(!p_rsp_data)
        return -1;
        
    (msg.cmd_mix).cmd_type = TYPE_PWC;
    (msg.cmd_mix).cmd = CMD_SETTING;
    (msg.cmd_mix).cmd_obj = OBJ_MCA;
    (msg.cmd_mix).cmd_src = OBJ_AP;
    msg.mode = time_out ? SYNC_CMD : ASYNC_CMD;    
    memset(&frame, 0, sizeof(struct mca_frame));
    frame.cmd_id = cmd_id;
    memcpy((void*)&frame.udata, (void*)&req_data, sizeof(union mca_udata_req));       
    data_len = (sizeof(msg.data) - sizeof(msg.data[0])) < sizeof(frame) \
                ? (sizeof(msg.data) - sizeof(msg.data[0])) : sizeof(frame);  
                
    memcpy((void*)&msg.data[1], (void*)&frame, data_len);

    if(WARN_ON((sizeof(msg.data) - sizeof(msg.data[0])) < sizeof(frame))) 
    {
        MCA_DEBUG("mca_send frame size is out of range!!!\n");
    } 

    ret = ipc_msg_send(OBJ_LPM3, &msg, SYNC_MODE);
    if(0 != ret)
    {
        MCA_DEBUG("mca send cmd:%d ret:0x%x failed!!!\n",cmd_id, ret);
        return -1;
    }

    if (time_out) {
        data_len= ((sizeof(msg.data) - sizeof(msg.data[0])) < sizeof(union mca_udata_rsp)) ?
                        (sizeof(msg.data) - sizeof(msg.data[0])) : sizeof(union mca_udata_rsp);
        memcpy(p_rsp_data, &(msg.data[1]), data_len);
        if(-1 == p_rsp_data->dfs_default_rsp.ret)
        {
            MCA_DEBUG("mca send cmd:%d rsp.ret:-1 failed!!!\n",cmd_id);
            return -1;
        }
    }
    return 0;

}

int test_mca_cmd(MCA_CMD_ID cmd_id,s32_t qos_id, u32_t req_value,int time_out)
{
    union mca_udata_req req;
    union mca_udata_rsp rsp;
    s32_t buf[32] = {0};
    s32_t* req_id = &buf[0];

    if(!req_id)
        return RET_ERR_PARAM_NULL;

    req.dfs_rqqos_req.qos_id    = qos_id;
    req.dfs_rqqos_req.req_value = req_value;
    req.dfs_rqqos_req.core_id   = 2;

    if(-1 == mca_send(cmd_id , req, &rsp, time_out))
    {
        printk("mca_send failed!!!\n");
        return RET_ERR;
    }
    *req_id = rsp.dfs_rqqos_rsp.req_id;

    printk("req_id:%d\n",*req_id);
    
    return rsp.dfs_rqqos_rsp.ret;
}
