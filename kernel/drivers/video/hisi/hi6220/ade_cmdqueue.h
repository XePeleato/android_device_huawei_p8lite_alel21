/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *	 * Redistributions of source code must retain the above copyright
 *	   notice, this list of conditions and the following disclaimer.
 *	 * Redistributions in binary form must reproduce the above
 *	   copyright notice, this list of conditions and the following
 *	   disclaimer in the documentation and/or other materials provided
 *	   with the distribution.
 *	 * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *	   contributors may be used to endorse or promote products derived
 *	   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __ADE_CMDQUEUE_H__
#define __ADE_CMDQUEUE_H__

#include <linux/hisi/hisi_ion.h>
#include <linux/hisi/reg_ops.h>
#include "soc_ao_sctrl_interface.h"
#include "soc_baseaddr_interface.h"
#include "soc_modem_sctrl_interface.h"
#include "balong_ade.h"
#include "ade_overlay.h"

/**************************macro*************************/
#define ADE_CMD_WITE_REG_MAX            (128)
#define ADE_CMD_BUFF_SIZE_MAX           (ION_8K_ALIGN(SZ_1M))
#define ADE_SCL_COEF_MAX                (195)

#define HWC_BLENDING_PREMULT    (0x0105)
#define HWC_BLENDING_COVERAGE   (0x0405)

#define ADE_CMD_FILE_BUFF_MAX      (2)

#define ADE_CMD_ALIGN_BYTE_LEN           (64)   /* 16 * 4, 16 words */
/*************************enum***************************/
enum {
    ADE_CMD_TYPE_NULL  = 0x0,
    ADE_CMD_TYPE_WRITE = 0x1,
    ADE_CMD_TYPE_READ  = 0x2,
    ADE_CMD_TYPE_WAIT  = 0x3,
    ADE_CMD_TYPE_EOF   = 0x4,
    ADE_CMD_TYPE_INVAL
};

enum {
    ADE_CMDQ_TYPE_ONLINE = 0,
    ADE_CMDQ_TYPE_OFFLINE,
    ADE_CMDQ_TYPE_MAX
};

enum {
    ADE_CMD_UNIT_DMA = 0,
    ADE_CMD_UNIT_CLIP,
    ADE_CMD_UNIT_SCL,
    ADE_CMD_UNIT_CTRAN,
    ADE_CMD_UNIT_OVLY,
    ADE_CMD_UNIT_ROT,
    ADE_CMD_UNIT_MAX
};

enum {
    TOP_ROT_BUFF_ROT_NULL = 0,
    TOP_ROT_BUFF_ROT_BLOCK,
    TOP_ROT_BUFF_ROT_720p,
    TOP_ROT_BUFF_ROT_1080p
};


/*************************struct***************************/

typedef union {
    struct {
        u32     first_reg_offset : 16;
        u32     arg_len  : 8;
        u32     cmd_type : 8;
    }bits;
    u32     ul32;
}WRITE_CMD_HEAD;


typedef union {
    struct {
        u32     reserved : 24;
        u32     cmd_type : 8;
    }bits;
    u32     ul32;
}NULL_EOF_CMD;

typedef union {
    struct {
        u32     reg_offset : 16;
        u32     comm_reg   : 8;
        u32     cmd_type   : 8;
    }bits;
    u32     ul32;
}READ_CMD;

typedef union {
    struct {
        u32     event_no   : 8;
        u32     reserved   : 16;
        u32     cmd_type   : 8;
    }bits;
    u32     ul32;
}WAIT_CMD;

struct write_cmd_head {
    WRITE_CMD_HEAD   cmd_head;
    u32              reg_val[ADE_CMD_WITE_REG_MAX];
};

struct cmdfile_buffer{
    void    *vaddr;
    u32     paddr;
    u32     buff_size;
    u32     cmd_len;
    u32     is_busy;
    struct cmdfile_buffer *next;
    struct ion_handle *cmd_ion_handle;
};

struct cmdqueue_buff {
    u32     current_idx;
    u32     reserved;
    struct  cmdfile_buffer   cf_list[ADE_CMD_FILE_BUFF_MAX];
};


/*****************************function*******************************/
extern int ade_overlay_set_rot_cmd(u8* ade_base, struct ade_overlay_rot* rot_info, struct cmdfile_buffer* cf_buff);
extern int ade_cmdq_cmdfile_buff_init(struct ion_client *ade_ion_client, struct  cmdqueue_buff   *cmdq_list);
extern void ade_cmdq_wr_cmd(u32 reg_addr, u32 val);
extern void ade_cmdq_wr_rdma_pe_cmd(u32 reg_addr, u32 ch_type, u32 rotation);
extern void ade_cmdq_wr_rdma_ctrl_cmd(u32 reg_addr, struct overlay_region_info* pipe_info);
extern void ade_cmdq_wr_rdma_ctrl_rot_cmd(u32 reg_addr, u32 format, ADE_ROT_UINT32 rotation);
extern void ade_cmdq_wr_wdma_ctrl_cmd(u32 reg_addr, struct overlay_compose_info* pcomp_info);
extern void ade_cmdq_wr_wdma3_ctrl_cmd(u32 reg_addr, struct overlay_compose_info *pcomp_info, u32 has_scl3);
extern void ade_cmdq_wr_blk_rot_wdma3_ctrl_cmd(u32 reg_addr, u32 format);
extern void ade_cmdq_wr_blk_rot_wdma1_ctrl_cmd(u32 reg_addr, u32 format);
extern void ade_cmdq_wr_scl_ctrl_cmd(u32 reg_addr, struct overlay_region_info* pipe_info, struct ade_rect *src_rect, struct ade_rect *dst_rect);
extern void ade_cmdq_wr_scl2_ctrl_cmd(u32 reg_addr, struct ade_overlay_scl2* scl2_info,struct ade_rect *src_rect, struct ade_rect *dst_rect);
extern void ade_cmdq_wr_ovly_scl_ctrl_cmd(u32 reg_addr, struct overlay_compose_info* pcomp_info);
extern void ade_cmdq_wr_blk_rot_scl_ctrl_cmd(u32 reg_addr, u32 input_format);
extern void ade_cmdq_wr_scl_hsp_cmd(u32 reg_addr, struct ade_rect* src_rect, struct ade_rect* dst_rect);
extern void ade_cmdq_wr_scl_vsp_cmd(u32 reg_addr, struct ade_rect* src_rect, struct ade_rect* dst_rect);
extern void ade_cmdq_wr_scl_hcoef_cmd(void *cmdbuff_vaddr, u32* cmd_len, u32 reg_addr, struct ade_rect* src_rect, struct ade_rect* dst_rect);
extern void ade_cmdq_wr_scl_vcoef_cmd(void *cmdbuff_vaddr, u32* cmd_len, u32 reg_addr, struct ade_rect* src_rect, struct ade_rect* dst_rect);
extern void ade_cmdq_wr_ctran_chdc_cmd_with_yuv_info(u32 reg_addr, u32 val, u32 yuv_info);
extern void ade_cmdq_wr_ctran_csc_cmd_with_yuv_info(u32 reg_addr, u32 val, u32 yuv_info);
extern void ade_cmdq_wr_ctran_chdc_cmd(u32 reg_addr, u32 val);
extern void ade_cmdq_wr_ctran_csc_cmd(u32 reg_addr, u32 val);
extern void ade_cmdq_wr_ovly_ch_ctrl_cmd(u32 reg_addr, struct ovly_ctrl_info ctrl_info);
extern void ade_cmdq_wr_rot_cfg_cmd(u32 reg_addr, struct ade_overlay_rot* rot_info);
extern void ade_cmdq_wr_eof_cmd(void *cmdbuff_vaddr, u32 *cmd_len);
extern void ade_cmdq_wr_wait_cmd(void *cmdbuff_vaddr, u32 *cmd_len, u32 val);
extern void ade_cmdq_wr_cmd2buff(void *cmdbuff_vaddr, u32 *cmd_len);

#if ADE_DEBUG_LOG_ENABLE
extern int ade_cmdq_wr_cmdbuff2file(struct cmdfile_buffer  *cf_buff, bool is_online, u32 pattern_num);
extern int ade_cmdq_wr_topinfo2file(char* buff, int len);
#endif
extern  u32 g_601_csc_value[];
extern bool g_is_set_color_temperature;
extern struct semaphore g_ct_sem;
#endif  /*  ADE_CMDQUEUE_H  */
