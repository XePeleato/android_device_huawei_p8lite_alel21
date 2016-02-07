/*-
 * Copyright (c) 1999 Brian Somers <brian@Awfulhak.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/usr.sbin/ppp/proto.c,v 1.6 2000/07/19 02:10:35 brian Exp $
 */
 /*
  1.日    期   : 2006年10月21日
    作    者   : L47619
    修改内容   : 问题单A32D06605
 */

#if 0/*fanzhibin f49086 delete it*/
#include <sys/types.h>

#include <stdio.h>
#include <termios.h>

#include "layer.h"
#include "acf.h"
#include "defs.h"
#include "log.h"
#include "timer.h"
#include "ppp_fsm.h"
#include "ppp_mbuf.h"
#include "proto.h"
#include "throughput.h"
#include "lqr.h"
#include "hdlc.h"
#include "lcp.h"
#include "ccp.h"
#include "link.h"
#endif
/*fanzhibin f49086 add it begin*/
#include "ppp_public.h"
#include "layer.h"
#include "acf.h"
#include "ppp_fsm.h"
#include "ppp_mbuf.h"
#include "proto.h"
#include "throughput.h"
#include "hdlc.h"
#include "lcp.h"
#include "async.h"
#include "auth.h"
#include "ipcp.h"
#include "pppid.h"
#include "link.h"
/*fanzhibin f49086 add it end*/
/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_PROTO_C


#if(FEATURE_ON == FEATURE_PPP)


VOS_INT32
proto_WrapperOctets(struct lcp *lcp, VOS_UINT16 proto)
{
  return (lcp->his_protocomp && !(proto & 0xff00)) ? 1 : 2;
}

struct ppp_mbuf *
proto_Prepend(struct ppp_mbuf *bp, VOS_UINT16 proto, unsigned comp, VOS_INT32 extra)
{
  VOS_UCHAR cp[2];

  cp[0] = proto >> 8;
  cp[1] = proto & 0xff;

  if (comp && cp[0] == 0)
    bp = ppp_m_prepend(bp, cp + 1, 1, extra);
  else
    bp = ppp_m_prepend(bp, cp, 2, extra);

  return bp;
}

struct ppp_mbuf *
proto_LayerPush(/*struct bundle *b, */struct link *l, struct ppp_mbuf *bp,
                VOS_INT32 pri, VOS_UINT16 *proto)
{
  bp = proto_Prepend(bp, *proto, l->lcp.his_protocomp,
                     acf_WrapperOctets(&l->lcp, *proto));
  return bp;
}

PPP_ZC_STRU *proto_LayerPull(/*struct bundle *b, */struct link *l, PPP_ZC_STRU *pstMem,
                VOS_UINT16 *pusProto)
{
    VOS_UCHAR       cp[2];
    VOS_UINT32      ulRst;


    ulRst   = PPP_MemGet(pstMem, 0, cp, 1);

    if (PS_SUCC != ulRst)
    {
        PPP_MemFree(pstMem);

        return VOS_NULL_PTR;
    }

    *pusProto = (VOS_UINT16)cp[0];

    /* 如果协议字段占据2两个字节 */
    if (!(cp[0] & 1))
    {
        ulRst = PPP_MemCutHeadData(&pstMem, cp, 2);

        if (PS_SUCC != ulRst)
        {
            PPP_MemFree(pstMem);

            return VOS_NULL_PTR;
        }

        *pusProto = (((VOS_UINT16)cp[0]) << 8) | ((VOS_UINT16)cp[1]);
    }
    else
    {
        PPP_MemCutHeadData(&pstMem, cp, 1);
    }

    return pstMem;
}

struct layer protolayer =
  { LAYER_PROTO, "proto", proto_LayerPush, proto_LayerPull };

#endif /* #if(FEATURE_ON == FEATURE_PPP) */

