/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: iptftp_common.c,v $ $Revision: 1.23 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/src/iptftp_common.c,v $
 *   $Author: gzadoyan $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Functions used by both TFTP client and TFTP server.
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

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_time.h"
#include "iptftp_h.h"
#include "iptftp.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC Ip_s32 iptftp_write_file(IP_FILE *fp, Ip_u8 *buf, Ip_s32 count, Iptftp *tftp, Ip_s8 *rem);
IP_STATIC Ip_s32 iptftp_read_file(IP_FILE *fp, Ip_u8 *buf, Ip_s32 bufsize, Iptftp *tftp, Ip_s8 *rem);
IP_STATIC Ip_s32 iptftp_create_request(Iptftp *tftp, Ip_u8 *buf, Ip_s32 buflen);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    iptftp_create_request
 *===========================================================================
 * Description: Create a TFTP read or write request.
 * Parameters:  tftp   - TFTP structure
 *              buf    - request buffer
 *              buflen - length of request buffer
 * Returns:     length of request or -1 if failed.
 */
IP_STATIC Ip_s32
iptftp_create_request(Iptftp *tftp, Ip_u8 *buf, Ip_s32 buflen)
{
    Ip_s32 len;

    buf[0] =(Ip_u8)(tftp->opcode / 256);
    buf[1] =(Ip_u8)(tftp->opcode % 256);
    len = 2;
    len += ipcom_strlen(tftp->file) + 1;
    if (len > buflen)
        return -1;
    ipcom_strcpy((char *)&buf[2], tftp->file);

    if (tftp->binary)
    {
        len += ipcom_strlen("octet") + 1;
        if (len > buflen)
            return -1;
        ipcom_strcpy((char *)&buf[len - ipcom_strlen("octet") - 1], "octet");
    }
    else
    {
        len += ipcom_strlen("netascii") + 1;
        if (len > buflen)
            return -1;
        ipcom_strcpy((char *)&buf[len - ipcom_strlen("netascii") - 1], "netascii");
    }

    return len;
}

/*
 *===========================================================================
 *                    iptftp_read_file
 *===========================================================================
 * Description: TFTP read file
 * Parameters:  fp      - stream
 *              buf     - buffer to place read data.
 *              bufsize - length of buffer.
 *              tftp    - TFTP structure.
 *              rem     - remainder
 * Returns:     Number of bytes read or -1 if failed.
 */
IP_STATIC Ip_s32
iptftp_read_file(IP_FILE *fp, Ip_u8 *buf, Ip_s32 bufsize, Iptftp *tftp, Ip_s8 *rem)
{
    Ip_s32 len = 0, i = 0;

    if (tftp->binary)
        len = ipcom_fread(buf, 1, bufsize, fp);
    else
    {
        /* Netascii mode */
        if (*rem >= 0)
        {
            buf[0] = *rem;
            *rem = -1;
            i = 1;
        }
        while (i < bufsize)
        {
            len = ipcom_fread(&buf[i], 1, 1, fp);
            if (!len)
                break;
            if (buf[i] == '\n')
            {
                buf[i] = '\r';   /* Insert the CR LF pair */
                if (i <(bufsize - 1))
                    buf[++i] = '\n';
                else
                    *rem = '\n';
            }
            else if (buf[i] == '\r')
            {
                buf[i] = '\r';   /* Insert the CR NULL pair */
                if (i <(bufsize - 1))
                    buf[++i] = '\0';
                else
                    *rem = '\0';
            }
            i++;
        }
    }

    /* Read error */
    if (!len && !ipcom_feof(fp))
        return -1;
    else
        return tftp->binary ? len : i;
}


/*
 *===========================================================================
 *                    iptftp_write_file
 *===========================================================================
 * Description: TFTP write file
 * Parameters:  fp     - stream
 *              buf    - buffer to write from.
 *              count  - number of bytes to write.
 *              tftp   - TFTP structure.
 *              rem    - remainder
 * Returns:     Number of bytes written or -1 if failed.
 */
IP_STATIC Ip_s32
iptftp_write_file(IP_FILE *fp, Ip_u8 *buf, Ip_s32 count, Iptftp *tftp, Ip_s8 *rem)
{
    Ip_s32 len = 0, i = 0;

    if (tftp->binary)
    {
        len = ipcom_fwrite(buf, 1, count, fp);
    }
    else
    {
        /* Netascii mode */
        while (i < count)
        {
            if (buf[i] == '\r')
            {
                *rem = '\r';
                i++;
                continue;
            }
            if (*rem == '\r' && buf[i] == '\0')
                buf[i] = '\r';
            else if (*rem == '\r' && buf[i] != '\n')
                return -2;
            len = ipcom_fwrite(&buf[i], 1, 1, fp);
            if (len != 1)
                break;
            *rem = -1;
            i++;
        }
    }

    /* Write error */
    if (tftp->binary &&(len != count))
        return -1;
    else if (!tftp->binary &&(len != 1))
        return -1;
    else
        return count;
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    iptftp_strip_path
 *===========================================================================
 * Description: Strip the path from a filename
 * Parameters:  path - the filename
 * Returns:     nothing.
 */
IP_GLOBAL void
iptftp_strip_path(char *path)
{
    char *cp;

    cp = ipcom_strrchr(path, IPTFTP_DIR_SEP);
    if (cp)
        ipcom_strcpy(path, cp + 1);
}


/*
 *===========================================================================
 *                    iptftp_send_loop
 *===========================================================================
 * Description: TFTP send loop
 * Parameters:  sock     - socket descriptor.
 *              fp       - stream
 *              err_msg  - potential error message.
 *              err_code - potential error code.
 *              tftp     - TFTP structure.
 * Returns:     IPTFTP_SUCCESS or an error code.
 */
IP_GLOBAL Ip_err
iptftp_send_loop(Ip_fd sock, IP_FILE *fp, char **err_msg, Ip_u16 *err_code, Iptftp *tftp)
{
    struct Ip_timeval tmo;
    Ip_fd_set *read_set;
    Ip_u8 *sbuf = IP_NULL, *rbuf;
    Ip_u16 opcode, rblock, block = 0;
    Ip_s32 len, num, count = 0, retries;
    Ip_err retval = IPTFTP_SUCCESS;
    Ip_s8 rem = -1;
    Ip_time_t t;
    long timeout;

    *err_msg = IP_NULL;
    *err_code = 0;
    /* Prepare the read set */
    read_set = ipcom_malloc(sizeof(Ip_fd_set));
    if (read_set == IP_NULL)
    {
        IPCOM_LOG0(ERR, "iptftp_send_loop :: ipcom_malloc(read_set) failed");
        retval = IPTFTP_ERR_MEMORY_FAIL;
        goto err;
    }
    ipcom_memset(read_set, 0, sizeof(Ip_fd_set));

    /* Allocate the send buffer */
    sbuf = ipcom_malloc(IPTFTP_MAX_PACKET * 2);
    if (sbuf == IP_NULL)
    {
        IPCOM_LOG0(ERR, "iptftp_send_loop :: ipcom_malloc(sbuf) failed");
        retval = IPTFTP_ERR_MEMORY_FAIL;
        goto err;
    }
    ipcom_memset(sbuf, 0, IPTFTP_MAX_PACKET * 2);
    rbuf = sbuf + IPTFTP_MAX_PACKET;

    while (1)
    {
        if (tftp->opcode == IPTFTP_OPCODE_RRQ || block > 0)
        {
            if (!block)
                block = 1;
            sbuf[0] = IPTFTP_OPCODE_DATA / 256;
            sbuf[1] = IPTFTP_OPCODE_DATA % 256;
            sbuf[2] =(Ip_u8)(block / 256);
            sbuf[3] =(Ip_u8)(block % 256);
            /* Read from file */
            len = iptftp_read_file(fp, &sbuf[4], IPTFTP_MAX_PACKET - 4, tftp, &rem);
            if (len < 0)
            {
                IPCOM_LOG1(ERR, "iptftp_send_loop :: File read error: \"%s\"", tftp->file);
                *err_code = IPTFTP_ERRCODE_ACCESSVIOLATION;
                *err_msg = IPTFTP_ERRMSG_FILEREADERROR;
                retval = IPTFTP_ERR_FILE_READ;
                goto err;
            }
            /* Transfer stats */
            tftp->nbytes += len;
            len += 4;
        }
        else
        {
            len = iptftp_create_request(tftp, sbuf, IPTFTP_MAX_PACKET);
            if (len < 0)
            {
                IPCOM_LOG0(ERR, "iptftp_send_loop :: iptftp_create_request() failed");
                retval = IPTFTP_ERR_MEMORY_FAIL;
                goto err;
            }
        }

        timeout = ipcom_sysvar_get_as_int("iptftp.timeout", 5);
        retries = ipcom_sysvar_get_as_int("iptftp.retries", 2);
resend:
        /* Send the TFTP packet */
        if (iptftp_sendto(sock, tftp, (char *)sbuf, len) != IPCOM_SUCCESS)
        {
            IPCOM_LOG0(ERR, "iptftp_send_loop :: iptftp_sendto() failed");
            retval = IPTFTP_ERR_SOCKET_FAIL;
            goto err;
        }

        /* Assign timeouts */
        tmo.tv_sec = timeout;
        tmo.tv_usec = 0;

wait_for_ack:
        IP_FD_SET(sock, read_set);
        t = ipcom_time(IP_NULL);
        num = ipcom_socketselect(sock + 1, read_set, IP_NULL, IP_NULL, &tmo);
        if (num == IP_SOCKERR)
        {
            IPCOM_LOG1(ERR, "iptftp_send_loop :: ipcom_socketselect() failed, errno = %d", ipcom_errno);
            retval = IPTFTP_ERR_SOCKET_FAIL;
            goto err;
        }

        if (num == 0)
        {
            /* Timeout */
            if (retries--)
            {
                timeout <<= 1;     /* Exponential backoff */
                tmo.tv_sec = timeout;
                IPCOM_LOG1(DEBUG,  "iptftp_send_loop :: timeout -> resending block %d", block);
                goto resend;
            }
            else
            {
                IPCOM_LOG0(DEBUG,  "iptftp_send_loop :: timeout -> no further retries");
                retval = IPTFTP_ERR_TRANSFER_TIMEOUT;
                goto err;
            }
        }
        else if (num > 0 && IP_FD_ISSET(sock, read_set))
        {
            /* Data available */
            count = iptftp_recvfrom(sock, tftp, (char *)rbuf, IPTFTP_MAX_PACKET);
        }

        if (count < 0)
        {
            IPCOM_LOG1(ERR, "iptftp_send_loop :: iptftp_recvfrom() failed, errno = %d", ipcom_errno);
            retval = IPTFTP_ERR_SOCKET_FAIL;
            goto err;
        }
        if (count == 0)
            goto wait_for_ack;     /* Irrelevant message received */

        /* Sanity check */
        if (count < 4)
        {
            *err_code = IPTFTP_ERRCODE_ILLEGALTFTPOP;
            *err_msg = IPTFTP_ERRMSG_INCOMPLETEMSG;
            retval = IPTFTP_ERR_ILLEGAL_MESSAGE;
            goto err;
        }

        /* Check opcode */
        opcode =(Ip_u16)(rbuf[0]*256 + rbuf[1]);
        if (opcode == IPTFTP_OPCODE_ERR)
        {
            if (tftp->verbose)
            {
                Ip_u16 err_val =(Ip_u16)(rbuf[2]*256 + rbuf[3]);
                rbuf[IPTFTP_MAX_PACKET - 1] = '\0';  /* Safety precaution */
                ipcom_printf("ERROR: Code=%d Msg=\"%s\"\n", err_val, &rbuf[4]);
            }
            IPCOM_LOG0(ERR, "iptftp_send_loop :: Received error message");
            retval = IPTFTP_ERR_ERROR_MESSAGE;
            goto err;
        }
        if (opcode != IPTFTP_OPCODE_ACK)
        {
            *err_code = IPTFTP_ERRCODE_ILLEGALTFTPOP;
            *err_msg = IPTFTP_ERRMSG_INVALIDOPCODE;
            retval = IPTFTP_ERR_ILLEGAL_MESSAGE;
            goto err;
        }

        /* Check block number */
        rblock = (Ip_u16)(rbuf[2]*256 + rbuf[3]);
        if (block == rblock)
        {
            /* Expected */
            IPCOM_LOG1(DEBUG2, "iptftp_send_loop :: Received ACK on block %d.", rblock);
        }
        else if (block == (rblock + 1))
        {
            /* Avoid Sorcerer's Apprentice Syndrome - do not resend */
            IPCOM_LOG1(DEBUG, "iptftp_send_loop :: Received duplicate ACK on block %d.", rblock);
            t = ipcom_time(IP_NULL) - t;
            tmo.tv_sec = tmo.tv_sec - t;
            if (tmo.tv_sec < 0)
                tmo.tv_sec = 0;     /* Unlikely, but make sure timeout is not negative */
            goto wait_for_ack;
        }
        else
        {
            IPCOM_LOG1(ERR, "iptftp_send_loop :: Received out of order ACK on block %d.", rblock);
            goto wait_for_ack;
        }


        /* Check for transfer completed */
        if (len < IPTFTP_MAX_PACKET && block > 0)
            break;

        block++;
    }

err:
    if (read_set != IP_NULL)
        ipcom_free(read_set);
    if (sbuf != IP_NULL)
        ipcom_free(sbuf);

    return retval;
}


/*
 *===========================================================================
 *                    iptftp_receive_loop
 *===========================================================================
 * Description: TFTP receive loop.
 * Parameters:  sock     - socket descriptor.
 *              fp       - stream
 *              err_msg  - potential error message.
 *              err_code - potential error code.
 *              tftp     - TFTP structure.
 * Returns:     IPTFTP_SUCCESS or an error code.
 */
IP_GLOBAL Ip_err
iptftp_receive_loop(Ip_fd sock, IP_FILE *fp, char **err_msg, Ip_u16 *err_code, Iptftp *tftp)
{
    struct Ip_timeval tmo;
    Ip_fd_set *read_set;
    Ip_u8 *sbuf = IP_NULL, *rbuf;
    Ip_u16 opcode, rblock, block = 0;
    Ip_s32 len, num, count, retries;
    Ip_s8 rem = -1;
    Ip_err retval = IPTFTP_SUCCESS;
    long timeout;

    *err_msg = IP_NULL;
    *err_code = 0;
    /* Prepare the read set */
    read_set = ipcom_malloc(sizeof(Ip_fd_set));
    if (read_set == IP_NULL)
    {
        IPCOM_LOG0(ERR, "iptftp_receive_loop :: ipcom_malloc(read_set) failed");
        retval = IPTFTP_ERR_MEMORY_FAIL;
        goto err;
    }
    ipcom_memset(read_set, 0, sizeof(Ip_fd_set));

    /* Allocate the send buffer */
    sbuf = ipcom_malloc(IPTFTP_MAX_PACKET * 2);
    if (sbuf == IP_NULL)
    {
        IPCOM_LOG0(ERR, "iptftp_send_loop :: ipcom_malloc(sbuf) failed");
        retval = IPTFTP_ERR_MEMORY_FAIL;
        goto err;
    }
    ipcom_memset(sbuf, 0, IPTFTP_MAX_PACKET * 2);
    rbuf = sbuf + IPTFTP_MAX_PACKET;

    count = IPTFTP_MAX_PACKET;           /* Indicate not completed */
    while (1)
    {
        if (tftp->opcode == IPTFTP_OPCODE_WRQ || block > 0)
        {
            sbuf[0] = IPTFTP_OPCODE_ACK / 256;
            sbuf[1] = IPTFTP_OPCODE_ACK % 256;
            sbuf[2] = (Ip_u8)(block / 256);
            sbuf[3] = (Ip_u8)(block % 256);
            len = 4;
        }
        else
        {
            len = iptftp_create_request(tftp, sbuf, IPTFTP_MAX_PACKET);
            if (len < 0)
            {
                IPCOM_LOG0(ERR, "iptftp_receive_loop :: iptftp_create_request() failed");
                retval = IPTFTP_ERR_MEMORY_FAIL;
                goto err;
            }
        }

        timeout  = ipcom_sysvar_get_as_int("iptftp.timeout", 5);
        retries = ipcom_sysvar_get_as_int("iptftp.retries", 2);
resend:
        /* Send the TFTP packet */
        if (iptftp_sendto(sock, tftp, (char *)sbuf, len) != IPCOM_SUCCESS)
        {
            IPCOM_LOG0(ERR, "iptftp_receive_loop :: iptftp_sendto() failed");
            retval = IPTFTP_ERR_SOCKET_FAIL;
            goto err;
        }

        /* Check for transfer completed */
        if (count < IPTFTP_MAX_PACKET && block > 0)
            break;

        /* Assign timeouts */
        tmo.tv_sec = timeout;
        tmo.tv_usec = 0;

wait_for_data:
        IP_FD_SET(sock, read_set);
        num = ipcom_socketselect(sock + 1, read_set, IP_NULL, IP_NULL, &tmo);
        if (num == IP_SOCKERR)
        {
            IPCOM_LOG1(ERR, "iptftp_receive_loop :: ipcom_socketselect() failed, errno = %d", ipcom_errno);
            retval = IPTFTP_ERR_SOCKET_FAIL;
            goto err;
        }

        if (num == 0)
        {
            /* Timeout */
            if (retries--)
            {
                timeout <<= 1;     /* Exponential backoff */
                tmo.tv_sec = timeout;
                IPCOM_LOG1(DEBUG,  "iptftp_receive_loop :: timeout -> resending ACK on block %d", block);
                goto resend;
            }
            else
            {
                IPCOM_LOG0(DEBUG,  "iptftp_receive_loop :: timeout -> no further retries");
                retval = IPTFTP_ERR_TRANSFER_TIMEOUT;
                goto err;
            }
        }
        else if (num > 0 && IP_FD_ISSET(sock, read_set))
        {
            /* Data available */
            count = iptftp_recvfrom(sock, tftp, (char *)rbuf, IPTFTP_MAX_PACKET);
        }

        if (count < 0)
        {
            IPCOM_LOG1(ERR, "iptftp_receive_loop :: ipcom_recvfrom() failed, errno = %d", ipcom_errno);
            retval = IPTFTP_ERR_SOCKET_FAIL;
            goto err;
        }
        if (count == 0)
            goto wait_for_data;     /* Irrelevant message received */

        /* Sanity check */
        if (count < 4)
        {
            *err_code = IPTFTP_ERRCODE_ILLEGALTFTPOP;
            *err_msg = IPTFTP_ERRMSG_INCOMPLETEMSG;
            retval = IPTFTP_ERR_ILLEGAL_MESSAGE;
            goto err;
        }

        /* Check opcode */
        opcode = (Ip_u16)(rbuf[0]*256 + rbuf[1]);
        if (opcode == IPTFTP_OPCODE_ERR)
        {
            if (tftp->verbose)
            {
                Ip_u16 err_val = (Ip_u16)(rbuf[2]*256 + rbuf[3]);
                rbuf[IPTFTP_MAX_PACKET - 1] = '\0';  /* Safety precaution */
                ipcom_printf("ERROR: Code=%d Msg=\"%s\"\n", err_val, &rbuf[4]);
            }
            IPCOM_LOG0(ERR, "iptftp_receive_loop :: Received error message from");
            retval = IPTFTP_ERR_ERROR_MESSAGE;
            goto err;
        }
        if (opcode != IPTFTP_OPCODE_DATA)
        {
            *err_code = IPTFTP_ERRCODE_ILLEGALTFTPOP;
            *err_msg = IPTFTP_ERRMSG_INVALIDOPCODE;
            retval = IPTFTP_ERR_ILLEGAL_MESSAGE;
            goto err;
        }

        /* Check block number */
        block++;
        rblock = (Ip_u16)(rbuf[2]*256 + rbuf[3]);
        if (block == rblock)
        {
            /* Expected */
            IPCOM_LOG1(DEBUG2, "iptftp_receive_loop :: Received block %d.", rblock);
        }
        else if (block == (rblock + 1))
        {
            block--;
            IPCOM_LOG1(DEBUG, "iptftp_receive_loop :: Received duplicate block %d.", rblock);
            goto resend;
        }
        else
        {
            IPCOM_LOG1(ERR, "iptftp_receive_loop :: Received out of order block %d.", rblock);
            goto wait_for_data;
        }

        /* Check for zero length packet */
        if (count == 4)
            continue;
        /* Transfer stats */
        tftp->nbytes += count - 4;
        /* Write to file */
        len = iptftp_write_file(fp, &rbuf[4], count - 4, tftp, &rem);
        /* Write error */
        if (len == -2)
        {
            IPCOM_LOG0(ERR, "iptftp_receive_loop :: Received invalid character after CR");
            *err_code = IPTFTP_ERRCODE_ILLEGALTFTPOP;
            *err_msg = IPTFTP_ERRMSG_INVALIDLINEDELIMITER;
            retval = IPTFTP_ERR_ILLEGAL_MESSAGE;
            goto err;
        }
        if (len < 0)
        {
            IPCOM_LOG1(ERR, "iptftp_receive_loop :: File write error: \"%s\"", tftp->file);
            *err_code = IPTFTP_ERRCODE_ACCESSVIOLATION;
            *err_msg = IPTFTP_ERRMSG_FILEWRITEERROR;
            retval = IPTFTP_ERR_FILE_WRITE;
            goto err;
        }
    }


err:
    if (read_set != IP_NULL)
        ipcom_free(read_set);
    if (sbuf != IP_NULL)
        ipcom_free(sbuf);

    return retval;
}


/*
 *===========================================================================
 *                    iptftp_send_err
 *===========================================================================
 * Description: Send TFTP error message.
 * Parameters:  sock     - socket descriptor.
 *              tftp     - TFTP structure.
 *              err_code - error code.
 *              err_msg  - error message.
 * Returns:     nothing.
 */
IP_GLOBAL void
iptftp_send_err(Ip_fd sock, Iptftp *tftp, Ip_u16 err_code, char *err_msg)
{
    char *buf;
    Ip_s32 len;

    len = ipcom_strlen(err_msg) + 1 + 4;
    buf = ipcom_malloc(len);
    if (!buf)
        return;

    buf[0] = IPTFTP_OPCODE_ERR / 256;
    buf[1] = IPTFTP_OPCODE_ERR % 256;
    buf[2] =(Ip_u8)(err_code / 256);
    buf[3] =(Ip_u8)(err_code % 256);
    ipcom_strcpy(&buf[4], err_msg);

    /* Send the error message */
    /*lint -e429 */
    (void)iptftp_sendto(sock, tftp, buf, len);

    ipcom_free(buf);
}


/*
 *===========================================================================
 *                    iptftp_recvfrom
 *===========================================================================
 * Description: Wrapper for recvfrom(). Applies ancillary data.
 * Parameters:  sock    - socket descriptor.
 *              tftp    - TFTP structure.
 *              buf     - buffer to receive data into.
 *              bufsize - sizeof buffer.
 * Returns:     number of bytes received or -1 if failed.
 */
IP_GLOBAL Ip_s32
iptftp_recvfrom(Ip_fd sock, Iptftp *tftp, char *buf, Ip_s32 bufsize)
{
#ifdef IPCOM_USE_INET
    struct Ip_sockaddr_in sa;
#endif
#ifdef IPCOM_USE_INET6
    struct Ip_sockaddr_in6 sa6;
#endif
    Ip_s32 count = -1;
    struct Ip_msghdr msghdr;
    struct Ip_iovec  iov;

    tftp->controllen = sizeof(tftp->controlbuf);

    msghdr.msg_controllen = tftp->controllen;
    msghdr.msg_control    = tftp->controlbuf;
    msghdr.msg_flags      = 0;
    msghdr.msg_iov        = &iov;
    msghdr.msg_iovlen     = 1;
    iov.iov_base          = buf;
    iov.iov_len           = bufsize;

    switch (tftp->family)
    {
    #ifdef IPCOM_USE_INET
        case IP_AF_INET:
            msghdr.msg_name = &sa;
            msghdr.msg_namelen = sizeof(sa);
            count = ipcom_recvmsg(sock, &msghdr, 0);
            if (count < 0)
            {
                IPCOM_LOG1(ERR, "iptftp_recvfrom :: ipcom_recvmsg(INET) failed, errno = %d", ipcom_errno);
                return -1;
            }

            /* Store peer's port if request message sent */
            tftp->peer_port = ip_ntohs(sa.sin_port);
            /* Check that source port is correct */
            if (tftp->peer_port != ip_ntohs(sa.sin_port))
            {
                /* Send error message but don't break transfer */
                IPCOM_LOG0(DEBUG,  "iptftp_recvfrom :: received message from incorrect source port");
                iptftp_send_err(sock, tftp, IPTFTP_ERRCODE_UNKNKOWNTRANSFERID, IPTFTP_ERRMSG_INCORRECTSRCPORT);
                return 0;
            }
            break;
    #endif /* IPCOM_USE_INET */

    #ifdef IPCOM_USE_INET6
        case IP_AF_INET6:
            msghdr.msg_name = &sa6;
            msghdr.msg_namelen = sizeof(sa6);
            count = ipcom_recvmsg(sock, &msghdr, 0);
            if (count < 0)
            {
                IPCOM_LOG1(ERR, "iptftp_recvfrom :: ipcom_recvmsg(INET6) failed, errno = %d", ipcom_errno);
                return -1;
            }

            /* Store peer's port if request message sent */
            tftp->peer_port = ip_ntohs(sa6.sin6_port);
            /* Check that source port is correct */
            if (tftp->peer_port != ip_ntohs(sa6.sin6_port))
            {
                /* Send error message but don't break transfer */
                IPCOM_LOG0(DEBUG,  "iptftp_recvfrom :: received message from incorrect source port");
                iptftp_send_err(sock, tftp, IPTFTP_ERRCODE_UNKNKOWNTRANSFERID, IPTFTP_ERRMSG_INCORRECTSRCPORT);
                return 0;
            }
            break;
    #endif /* IPCOM_USE_INET6 */

        default:
            break;
    }

    return count;
}


/*
 *===========================================================================
 *                    iptftp_sendto
 *===========================================================================
 * Description: Wrapper for sendto(). Applies ancillary data.
 * Parameters:  sock - socket descriptor.
 *              tftp - TFTP structure.
 *              buf  - buffer to send from.
 *              len  - number of bytes to send.
 * Returns:     IPCOM_SUCCESS or IPCOM_ERR_FAILED.
 */
IP_GLOBAL Ip_err
iptftp_sendto(Ip_fd sock, Iptftp *tftp, char *buf, Ip_s32 len)
{
    union Ip_sockaddr_union  addr;

    ipcom_memset(&addr, 0, sizeof(addr));
    addr.sa.sa_family =(Ip_u8)tftp->family;

    switch (tftp->family)
    {
    #ifdef IPCOM_USE_INET
        case IP_AF_INET:
            IPCOM_SA_LEN_SET(&addr.sin, sizeof(struct Ip_sockaddr_in));
            addr.sin.sin_port = ip_htons(tftp->peer_port);
            ipcom_memcpy(&addr.sin.sin_addr, &tftp->peer_addr.addr, sizeof(tftp->peer_addr.addr));
            break;
    #endif /* IPCOM_USE_INET */

    #ifdef IPCOM_USE_INET6
        case IP_AF_INET6:
            IPCOM_SA_LEN_SET(&addr.sin6, sizeof(struct Ip_sockaddr_in6));
            addr.sin6.sin6_port = ip_htons(tftp->peer_port);
            addr.sin6.sin6_scope_id = tftp->ifindex;
            ipcom_memcpy(&addr.sin6.sin6_addr, &tftp->peer_addr.addr6, sizeof(tftp->peer_addr.addr6));
            break;
    #endif /* IPCOM_USE_INET6 */

        default:
            return IPCOM_ERR_FAILED;
    }

    if (tftp->controllen > 0)
    {
        struct Ip_iovec  iov;
        struct Ip_msghdr msg;

        iov.iov_base = buf;
        iov.iov_len  = len;

        msg.msg_name       = &addr;
        msg.msg_namelen    = IPCOM_SA_LEN_GET(&addr.sa);
        msg.msg_controllen = tftp->controllen;
        msg.msg_control    = tftp->controlbuf;
        msg.msg_flags      = 0;
        msg.msg_iov        = &iov;
        msg.msg_iovlen     = 1;

        if (ipcom_sendmsg(sock, &msg, 0) < len)
        {
            IPCOM_LOG1(ERR, "iptftp_sendto :: ipcom_sendmsg() failed, errno = %d", ipcom_errno);
            return IPCOM_ERR_FAILED;
        }
    }
    else
    {
        if (ipcom_sendto(sock, buf, (Ip_size_t)len, 0, (IP_CONST struct Ip_sockaddr *)&addr, IPCOM_SA_LEN_GET(&addr.sa)) < len)
        {
            IPCOM_LOG1(ERR, "iptftp_sendto :: ipcom_sendto() failed, errno = %d", ipcom_errno);
            return IPCOM_ERR_FAILED;
        }
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    iptftp_bind_zero
 *===========================================================================
 * Description: Bind TFTP socket to ephemeral port and enable ancillary data.
 * Parameters:  sock   - socket descriptor.
 *              family - Internet address family.
 * Returns:     IPCOM_SUCCESS or IPCOM_ERR_FAILED.
 */
IP_GLOBAL Ip_err
iptftp_bind_zero(Ip_fd sock, Ip_s32 family)
{
#ifdef IPCOM_USE_INET
    struct Ip_sockaddr_in sa;
#endif
#ifdef IPCOM_USE_INET6
    struct Ip_sockaddr_in6 sa6;
#endif
    int on = 1;

#ifdef IPCOM_USE_INET
    if (family == IP_AF_INET)
    {
        /* Enable ancillary data */
        if (ipcom_setsockopt(sock, IP_IPPROTO_IP, IP_IP_PKTINFO, &on, sizeof(on)) < 0)
        {
#ifndef IPLITE
            IPCOM_LOG2(INFO, "iptftp_bind_zero :: ipcom_setsockopt(IP_IP_PKTINFO) failed, errno = %d (%s)",
                ipcom_errno, ipcom_strerror(ipcom_errno));
#endif
        }

        ipcom_memset(&sa, 0, sizeof(sa));
        IPCOM_SA_LEN_SET(&sa, sizeof(struct Ip_sockaddr_in));
        sa.sin_family = IP_AF_INET;
        sa.sin_port = ip_htons(0);
        sa.sin_addr.s_addr = IP_INADDR_ANY;
        if (ipcom_bind(sock, (struct Ip_sockaddr *)&sa, sizeof(sa)) == IP_SOCKERR)
        {
            IPCOM_LOG0(ERR, "iptftp_bind_zero :: ipcom_bind() failed");
            return IPCOM_ERR_FAILED;
        }
    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (family == IP_AF_INET6)
    {
        /* Enable ancillary data */
        if (ipcom_setsockopt(sock, IP_IPPROTO_IPV6, IP_IPV6_RECVPKTINFO, &on, sizeof(on)) < 0)
        {
#ifndef IPLITE
            IPCOM_LOG2(INFO, "iptftps :: ipcom_setsockopt(IP_IPV6_RECVPKTINFO) failed, errno = %d (%s)",
                ipcom_errno, ipcom_strerror(ipcom_errno));
#endif
        }


        ipcom_memset(&sa6, 0, sizeof(sa6));
        IPCOM_SA_LEN_SET(&sa6, sizeof(sa6));
        sa6.sin6_family = IP_AF_INET6;
        sa6.sin6_port = ip_htons(0);
        if (ipcom_bind(sock, (struct Ip_sockaddr *)&sa6, sizeof(sa6)) == IP_SOCKERR)
        {
            IPCOM_LOG0(ERR, "iptftp_bind_zero :: ipcom_bind(INET6) failed");
            return IPCOM_ERR_FAILED;
        }
    }
#endif /* IPCOM_USE_INET6 */

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
