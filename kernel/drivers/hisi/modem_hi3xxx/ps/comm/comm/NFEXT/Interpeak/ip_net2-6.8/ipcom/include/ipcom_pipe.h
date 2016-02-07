/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_pipe.h,v $ $Revision: 1.11 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_pipe.h,v $
 *   $Author: kenneth $ $Date: 2009-07-02 11:16:47 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */
#ifndef IPCOM_PIPE_H
#define IPCOM_PIPE_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/*
  The IPCOM pipe is a inter-task communication channel where the
  sender and receive must share the same address space. The receiver
  must also know how to free the resources held by the passed message.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_cstyle.h>
#include <ipcom_time.h>
#include <ipcom_type.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_PIPE_INVALID  IP_NULL

/* Pipe sizes */
#define IPCOM_PIPE_MSG_LIMIT_UNLIMITED  0
#define IPCOM_PIPE_MSG_LIMIT_ONE        2
#define IPCOM_PIPE_MSG_LIMIT_FEW        10
#define IPCOM_PIPE_MSG_LIMIT_MANY       25

/* Pipe types */
#define IPCOM_PIPE_GENERIC              0
#define IPCOM_PIPE_LOCKLESS             1
#define IPCOM_PIPE_JOB_QUEUE            2
#define IPCOM_PIPE_IPNET                3
#define IPCOM_PIPE_MAX                  15
#define IPCOM_PIPE_NEXT                 (IPCOM_PIPE_MAX+1)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipcom_pipe_attr
 *===========================================================================
 */
typedef struct Ipcom_pipe_attr_struct
{
    int msg_limit;         /* maximum number of messages that can be
                              queued on this pipe */
    void (*handler)(void); /* optional: function that knows how to
                              process the messages on this pipe */
}
Ipcom_pipe_attr;

struct Ipcom_pipe_struct;

typedef void (*Ipcom_pipe_dtor)(struct Ipcom_pipe_struct *pipe);
typedef void (*Ipcom_pipe_send)(struct Ipcom_pipe_struct *pipe, void *msg);
typedef Ip_bool (*Ipcom_pipe_try_send)(struct Ipcom_pipe_struct *pipe, void *msg);
typedef int (*Ipcom_pipe_recv)(struct Ipcom_pipe_struct *pipe, void **pmsg);
typedef int (*Ipcom_pipe_recv_with_tmo)(struct Ipcom_pipe_struct *pipe, void **pmsg, struct Ip_timeval *tv);


/*
 *===========================================================================
 *                         Ipcom_pipe_ops
 *===========================================================================
 */
typedef struct Ipcom_pipe_ops_struct
{
    Ipcom_pipe_dtor          dtor;
    Ipcom_pipe_send          send;
    Ipcom_pipe_try_send      try_send;
    Ipcom_pipe_recv          recv;
    Ipcom_pipe_recv_with_tmo recv_with_tmo;
}
Ipcom_pipe_ops;


/*
 *===========================================================================
 *                         Ipcom_pipe
 *===========================================================================
 */
typedef struct Ipcom_pipe_struct
{
    void            *p;
    Ipcom_pipe_ops  *ops;
    int              type;
    Ip_pid_t         owner;
    Ipcom_pipe_attr  attr;
}
Ipcom_pipe;


/*
 *===========================================================================
 *                         Ipcom_pipe_create
 *===========================================================================
 */
typedef Ip_err (* Ipcom_pipe_create) (Ipcom_pipe *, Ipcom_pipe_attr *);


IP_EXTERN struct Ip_timeval ipcom_pipe_tv_zero;

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_PUBLIC void        ipcom_pipe_attr_init(Ipcom_pipe_attr *attr);
IP_PUBLIC Ipcom_pipe *ipcom_pipe_new(int type, Ipcom_pipe_attr *attr);
IP_PUBLIC void        ipcom_pipe_free(Ipcom_pipe *pipe);
IP_PUBLIC Ip_err      ipcom_pipe_register_type(int *type, Ipcom_pipe_create create);

/*
 *===========================================================================
 *                   ipcom_pipe_send
 *===========================================================================
 * Description: Sends a message through the pipe, will block if the
 *                   pipe is full.
 * Parameters:  pipe - the pipe to send to.
 *              msg - the message to send
 * Returns:
 *
 */
#define ipcom_pipe_send(pipe, msg) \
    (pipe)->ops->send(pipe, msg)


/*
 *===========================================================================
 *                   ipcom_pipe_try_send
 *===========================================================================
 * Description: Sends a message through the pipe, will fail if the
 *                   pipe is full.
 * Parameters:  pipe - the pipe to send to.
 *              msg - the message to send
 * Returns:     IP_TRUE - message was sent,
 *              IP_FALSE - message was not sent, pipe full.
 *
 */
#define ipcom_pipe_try_send(pipe, msg) \
    (pipe)->ops->try_send(pipe, msg)


/*
 *===========================================================================
 *                   ipcom_pipe_recv
 *===========================================================================
 * Description: Receives a message from a pipe, will block until a
 *                   message is available.
 * Parameters:  pipe - the pipe to send to.
 *              pmsg - pointer to where the message pointer should be
 *                   stored.
 * Returns:
 *
 */
#define ipcom_pipe_recv(pipe, pmsg) \
    (pipe)->ops->recv(pipe, pmsg)


/*
 *===========================================================================
 *                   ipcom_pipe_recv_with_tmo
 *===========================================================================
 * Description: Receives a message from a pipe, will block until a
 *                   message is available or until the timeout experies.
 * Parameters:  pipe - the pipe to send to.
 *              pmsg - pointer to where the message pointer should be
 *                   stored.
 *              tv - maximum time this call can block. IP_NULL means
 *                   block forever, i.e. same as ipcom_pipe_recv()
 * Returns:     IP_TRUE - message was received.
 *              IP_FALSE - timeout expired, no message recieved.
 *
 */
#define ipcom_pipe_recv_with_tmo(pipe, pmsg, tv) \
    (pipe)->ops->recv_with_tmo(pipe, pmsg, tv)


/*
 *===========================================================================
 *                   ipcom_pipe_try_recv
 *===========================================================================
 * Description: Tries to receive a message from the pipe. This call
 *              will never block, it returns whether a message was
 *              successfully received or not.
 * Parameters:  pipe - the pipe to send to.
 *              pmsg - pointer to where the message pointer should be
 *                   stored.
 * Returns:     IP_TRUE - message was received.
 *              IP_FALSE - no message recieved.
 *
 */
#define ipcom_pipe_try_recv(pipe, pmsg) \
    (pipe)->ops->recv_with_tmo(pipe, pmsg, &ipcom_pipe_tv_zero)

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
