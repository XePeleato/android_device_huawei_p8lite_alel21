/* ipcom_ifproxy_proto.h - header defining protocol used with ifproxy driver */

/*
 * Copyright (c) 2010 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

#ifndef __INCifproxy_protoh
#define __INCifproxy_protoh

#ifdef __cplusplus
extern "C" {
#endif

#define IFPROXY_CMD_PACKET_OUT    0x04

typedef struct ifproxy_cmd_hdr_s
{
    uint16_t PortIndex;
    uint8_t  Flags;
    uint8_t  Command;
    uint16_t EndOffset;
    uint16_t StartOffset;

} ifproxy_cmd_hdr_t;


/*
 * ifproxy_initial_t is the format of the first message sent
 * by the control NAE on the control socket.
 */
typedef struct ifproxy_initial_s
{
    uint16_t dataport;    /* NAE data socket MIPC port number for this port */
    uint16_t running;     /* Is port running (media active)? */
    uint8_t  hw_addr[6];  /* MAC station address */

} ifproxy_initial_t;

/*
 * After receiving the ifproxy_initial_t message from the
 * control NAE, the ifproxy driver connects its data socket and
 * responds with an ifproxy_initresp_t message on the control
 * connection.
 */
typedef struct ifproxy_initresp_s
{
    uint16_t dataport; /* MIPC port number for ifproxy driver data port */
    uint16_t if_index; /* Linux interface index for proxy port.
                          TODO: should be 32 bits like dev->ifindex */
    uint32_t rsvd1;
    uint64_t polling_cores;  /* what forwarder cores should poll this port */

} ifproxy_initresp_t;

/*
 * The ifproxy driver sends various control requests to the NAE
 * whose format is described by one of the ifproxy_ctrl_msg variants.
 */
#define IFPROXY_CTRL_DOWN      0x00 /* Tell the data-plane that the
                                       interface is bought down. No reply. */
#define IFPROXY_CTRL_UP        0x01 /* Tell the data-plane that the
                                       interface is bought up. No reply. */
#define IFPROXY_CTRL_GET_MAC   0x02 /* Get the MAC address of the real device
                                     * Reply contains only 6-byte MAC address.
                                     */
#define IFPROXY_CTRL_SET_MAC   0x03 /* Set the MAC address of the real device
                                     * to the value in the eth_addr member.
                                     * No reply expected.
                                     */
#define IFPROXY_CTRL_SET_MCLIST 0x04 /* Set the L2 multicast group list for
                                      * the real port.  No reply expected.
                                      */

#define IFPROXY_CTRL_RESULT    0xff /* Result of set operation */

/* ctrl 0x00, 01, 02 */
typedef struct ifproxy_ctrl_msg_s
{
    uint8_t cmd;
    uint8_t flags;
#define IFPROXY_CTRL_FLAGS_ALLMULTI  1 /* for IFPROXY_CTRL_SET_MCLIST */
    uint16_t rsvd16;

} ifproxy_ctrl_msg_t;

/* ctrl 0x03 */
typedef struct ifproxy_ctrl_set_mac_s
{
    ifproxy_ctrl_msg_t hdr;
    unsigned char eth_addr[6];

} ifproxy_ctrl_set_mac_t;

/* ctrl 0x04 */
typedef struct ifproxy_ctrl_set_mclist_s {
    ifproxy_ctrl_msg_t hdr;
    uint16_t num_addrs;      /* 0xffff -> ALLMULTI */
    uint8_t  eth_addrs [1];  /* variable length (6*num_addrs) */
} ifproxy_ctrl_set_mclist_t;

/*
 * How to separate unsolicited messages sent by the control NAE
 * on the control connection (e.g. link state changes) from responses
 * to requests on the control connection?  Maybe send such unsolicited
 * messages on the data socket, distinguished from incoming packets?
 *
 * For now, we send link state change notifications as express data
 * on the control connection.
 */

#define IFPROXY_EXPRESS_DATA(cmd, data) \
    (((uint64_t)(cmd)) | (((uint64_t)(data)) << 8))
#define IFPROXY_EXPRESS_CMD(expressdata) \
    ((unsigned int)((expressdata) & 0xffULL))

#define IFPROXY_CTRL_EXPRESS_LSC        0U

#define IFPROXY_CTRL_EXPRESS_LSC_DATA_ACTIVE    0x100


/*
 * Data connection messages from Control NAE to ifproxy driver.
 * For the most part, these are received packets, but
 * there may also be others such as link-state change notifications.
 */

#define IFPROXY_DATA_PACKET_IN     0x00  /* Deliver a received packet */

/* data 0x00 */
typedef struct ifproxy_data_msg_s
{
    uint8_t cmd;
    uint8_t flags;
} ifproxy_data_msg_t;


#ifdef __cplusplus
}
#endif

#endif /* __INCifproxy_protoh */
