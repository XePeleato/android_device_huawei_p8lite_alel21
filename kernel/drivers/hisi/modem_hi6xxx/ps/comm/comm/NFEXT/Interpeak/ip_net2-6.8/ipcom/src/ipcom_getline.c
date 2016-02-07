/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_getline.c,v $ $Revision: 1.42 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_getline.c,v $
 *   $Author: rboden $
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
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_clib2.h"
#include "ipcom_file.h"
#include "ipcom_os.h"
#include "ipcom_sock.h"
#include "ipcom_err.h"
#include "ipcom_shell.h"
#include "ipcom_syslog.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM

/* Escape character */
#define ESC 0x1b
/* Telnet IAC */
#define IAC 0xff


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipcom_getline_history
 *===========================================================================
 */
typedef struct Ipcom_getline_history_struct
{
    Ipcom_list list;
    char *cmd;
}
Ipcom_getline_history;


/*
 *===========================================================================
 *                         Ipcom_getline_cursor
 *===========================================================================
 */
typedef struct Ipcom_getline_cursor_struct
{
    int  prompt_len;
    int  row_len;
    int  buf_len;
    int  position;
    int  count;
    Ip_bool echo;
}
Ipcom_getline_cursor;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#if IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE && (defined(IP_PORT_OSE) || defined(IP_PORT_OSE5) || defined(IP_PORT_VXWORKS))
extern IP_GLOBAL void ipcom_stdio_set_echo (int fd, Ip_bool echo);
#endif



/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

const char up_arrow[3]      =  { ESC, '[', 'A' };
const char down_arrow[3]    =  { ESC, '[', 'B' };
const char right_arrow[3]   =  { ESC, '[', 'C' };
const char left_arrow[3]    =  { ESC, '[', 'D' };
const char space            =  ' ';
const char cr[2]            =  { '\r', '\0' };
const char lf[2]            =  { '\r', '\n' };


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_getline_getone
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_getline_getone(Ip_fd fd)
{
    if (fd == IP_INVALID_SOCKET)
    {
        int c;

        c = ipcom_getchar();
        if (c == IP_EOF)
            return -1;
        else
            return c;
    }
    else
    {
        int len;
        Ip_u8 byte;

        len = ipcom_socketread(fd, &byte, sizeof(byte));
        if (len < 0)
            return -2;  /* failed */
        else if (len == 0)
            return -1;  /* closed */
        else
            return (int)byte;
    }
}


/*
 *===========================================================================
 *                    ipcom_getline_putraw
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_getline_putraw(Ip_fd fd, const char *buf, int buflen, Ipcom_getline_cursor *cursor)
{
    int ret;

    if (cursor->echo == IP_TRUE)
    {
        if (fd == IP_INVALID_SOCKET)
        {
#if (!defined(IP_PORT_OSE) && !defined(IP_PORT_OSE5)) || IPCOM_USE_SHELL != IPCOM_SHELL_NATIVE
            int i;
            for(i = 0; i < buflen; i++)
            {
                ret = ipcom_printf("%c", buf[i]);
                if (ret == IP_EOF)
                    return -1;
            }
#endif
        }
        else
        {
            ret = ipcom_socketwrite(fd, (IP_CONST void *)buf, buflen);
            if (ret == IP_SOCKERR)
                return -1;
        }
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_getline_putone
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_getline_putone(Ip_fd fd, char c, Ipcom_getline_cursor *cursor)
{
    if (ipcom_getline_putraw(fd, &c, 1, cursor) < 0)
        return -1;

    if ((cursor->position + 1) % cursor->row_len == 0)
    {
        if (ipcom_getline_putraw(fd, lf, sizeof(lf), cursor) < 0)
            return -1;
        if (ipcom_getline_putraw(fd, cr, sizeof(cr), cursor) < 0)
            return -1;
    }

    cursor->position++;
    cursor->count++;
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_getline_putbuf
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_getline_putbuf(Ip_fd fd, const char *buf, int buflen, Ipcom_getline_cursor *cursor)
{
    int i;

    for(i = 0; i < buflen; i++)
        if (ipcom_getline_putone(fd, buf[i], cursor) < 0)
            return -1;

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_getline_left_arrow
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_getline_left_arrow(Ip_fd fd, Ipcom_getline_cursor *cursor)
{
    int i;

    if (cursor->position <= cursor->prompt_len)
        return IPCOM_SUCCESS;

    if (cursor->position % cursor->row_len == 0)
    {
        if (ipcom_getline_putraw(fd, up_arrow, sizeof(up_arrow), cursor) < 0)
            return IPCOM_ERR_FAILED;

        for(i=0; i<(cursor->row_len-1); i++)
            if (ipcom_getline_putraw(fd, right_arrow, sizeof(right_arrow), cursor) < 0)
                return IPCOM_ERR_FAILED;
    }
    else
    {
        if (ipcom_getline_putraw(fd, left_arrow, sizeof(left_arrow), cursor) < 0)
            return IPCOM_ERR_FAILED;
    }
    cursor->position--;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_getline_right_arrow
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_getline_right_arrow(Ip_fd fd, Ipcom_getline_cursor *cursor)
{
    if (cursor->position >= cursor->count + cursor->prompt_len)
        return IPCOM_SUCCESS;

    if ((cursor->position+1) % cursor->row_len == 0)
    {
        if (ipcom_getline_putraw(fd, lf, sizeof(lf), cursor) < 0)
            return IPCOM_ERR_FAILED;

        if (ipcom_getline_putraw(fd, cr, sizeof(cr), cursor) < 0)
            return IPCOM_ERR_FAILED;
    }
    else
    {
        if (ipcom_getline_putraw(fd, right_arrow, sizeof(right_arrow), cursor) < 0)
            return IPCOM_ERR_FAILED;
    }
    cursor->position++;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_getline_cursor_home
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_getline_cursor_home(Ip_fd fd, Ipcom_getline_cursor *cursor)
{
    while(cursor->position > cursor->prompt_len)
        if (ipcom_getline_left_arrow(fd, cursor) != IPCOM_SUCCESS)
            return IPCOM_ERR_FAILED;
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_getline_cursor_end
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_getline_cursor_end(Ip_fd fd, Ipcom_getline_cursor *cursor)
{
    while((cursor->position - cursor->prompt_len) < cursor->count)
        if (ipcom_getline_right_arrow(fd, cursor) != IPCOM_SUCCESS)
            return IPCOM_ERR_FAILED;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_getline_backspace
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_getline_backspace(Ip_fd fd, char *buf, Ipcom_getline_cursor *cursor)
{
    if (cursor->position > cursor->prompt_len)
    {
        int tmp = cursor->count - (cursor->position - cursor->prompt_len);

        ipcom_memmove(&buf[cursor->position - cursor->prompt_len - 1],
                        &buf[cursor->position - cursor->prompt_len], tmp);

        if (ipcom_getline_left_arrow(fd, cursor) != IPCOM_SUCCESS)
            return IPCOM_ERR_FAILED;
        if (ipcom_getline_putbuf(fd, &buf[cursor->position - cursor->prompt_len], tmp, cursor) < 0)
            return IPCOM_ERR_FAILED;
        cursor->count -= tmp;
        if (ipcom_getline_putbuf(fd, &space, sizeof(space), cursor) < 0)
            return IPCOM_ERR_FAILED;
        cursor->count -= 1;

        tmp++;
        while(tmp-- > 0)
            if (ipcom_getline_left_arrow(fd, cursor) != IPCOM_SUCCESS)
                return IPCOM_ERR_FAILED;
        cursor->count -= 1;
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_getline_ctrl_d
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_getline_ctrl_d(Ip_fd fd, char *buf, Ipcom_getline_cursor *cursor)
{
    if ((cursor->position - cursor->prompt_len) < cursor->count)
    {
        if (ipcom_getline_right_arrow(fd, cursor) != IPCOM_SUCCESS)
            return IPCOM_ERR_FAILED;

        if (ipcom_getline_backspace(fd, buf, cursor) != IPCOM_SUCCESS)
            return IPCOM_ERR_FAILED;
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_getline_paste_clipboard
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_getline_paste_clipboard(Ip_fd fd, char *buf, Ipcom_getline_cursor *cursor,
                              char *clipboard)
{
    int bytes, move;

    if (clipboard == IP_NULL)
        return IPCOM_SUCCESS;

    bytes = (int)ipcom_strlen(clipboard);
    if (bytes > (cursor->buf_len - cursor->count - 1))
        bytes = (cursor->buf_len - cursor->count - 1);
    if (bytes <= 0)
        return IPCOM_SUCCESS;
    move = cursor->count - (cursor->position - cursor->prompt_len);
    ip_assert(move >= 0);
    ipcom_memmove(&buf[cursor->position - cursor->prompt_len + bytes],
                  &buf[cursor->position - cursor->prompt_len], move); /*lint !e671 */
    ipcom_memcpy(&buf[cursor->position - cursor->prompt_len], clipboard, bytes);

    if (ipcom_getline_putbuf(fd, &buf[cursor->position - cursor->prompt_len], bytes, cursor) < 0)
        return -2;
    if (ipcom_getline_putbuf(fd, &buf[cursor->position - cursor->prompt_len], move, cursor) < 0)
        return -2;

    cursor->count -= move;
    while(move-- > 0)
    {
        if (ipcom_getline_left_arrow(fd, cursor) != IPCOM_SUCCESS)
            return -2;
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_getline_right_kill
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_getline_right_kill(Ip_fd fd, char *buf, Ipcom_getline_cursor *cursor, char **clipboard)
{
    int i;
    int size = cursor->count - (cursor->position - cursor->prompt_len);

    if (clipboard != IP_NULL && size > 0)
    {
        if (*clipboard != IP_NULL)
            ipcom_free(*clipboard);
        *clipboard = ipcom_malloc(size+1);
        if (*clipboard != IP_NULL)
        {
            ipcom_memcpy(*clipboard, &buf[cursor->position - cursor->prompt_len], size);
            (*clipboard)[size] = '\0';
        }
    }

    for(i=0; i<size; i++)
        if (ipcom_getline_putbuf(fd, &space, sizeof(space), cursor) < 0)
            return IPCOM_ERR_FAILED;

    for(i=0; i<size; i++)
        if (ipcom_getline_left_arrow(fd, cursor) < 0)
            return IPCOM_ERR_FAILED;

    cursor->count = cursor->position - cursor->prompt_len;
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_getline_window_resize
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_getline_window_resize(Ip_fd fd, char *buf, Ipcom_getline_cursor *cursor, int old_row_len)
{
    int i, j, row, col, pos, count;

    /* Save position */
    pos = cursor->count - (cursor->position - cursor->prompt_len);

    /* Go home with cursor */
    row = cursor->position / old_row_len;
    col = IP_MIN(cursor->position % old_row_len, cursor->row_len);
    for(i=0; i<row; i++)
        if (ipcom_getline_putraw(fd, up_arrow, sizeof(up_arrow), cursor) < 0)
            return IPCOM_ERR_FAILED;
    for(i=0; i<col; i++)
        if (ipcom_getline_putraw(fd, left_arrow, sizeof(left_arrow), cursor) < 0)
            return IPCOM_ERR_FAILED;

    /* Clean old characters */
    row = (cursor->count + cursor->prompt_len) / old_row_len;
    col = IP_MIN(old_row_len, cursor->row_len);
    for(i=0; i<row; i++)
    {
        if (ipcom_getline_putraw(fd, down_arrow, sizeof(down_arrow), cursor) < 0)
            return IPCOM_ERR_FAILED;
        for(j=0; j<col; j++)
            if (ipcom_getline_putraw(fd, &space, sizeof(space), cursor) < 0)
                return IPCOM_ERR_FAILED;
        for(j=0; j<col; j++)
            if (ipcom_getline_putraw(fd, left_arrow, sizeof(left_arrow), cursor) < 0)
                return IPCOM_ERR_FAILED;
    }
    for(i=0; i<row; i++)
        if (ipcom_getline_putraw(fd, up_arrow, sizeof(up_arrow), cursor) < 0)
            return IPCOM_ERR_FAILED;

    /* Output command */
    for(i=0; i<cursor->prompt_len; i++)
        if (ipcom_getline_putraw(fd, right_arrow, sizeof(right_arrow), cursor) < 0)
            return IPCOM_ERR_FAILED;
    count = cursor->count;
    cursor->position = cursor->prompt_len;
    cursor->count = 0;

    if (ipcom_getline_putbuf(fd, buf, count, cursor) < 0)
        return IPCOM_ERR_FAILED;

    for(i=0; i<pos; i++)
        if (ipcom_getline_left_arrow(fd, cursor) < 0)
            return IPCOM_ERR_FAILED;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_getline_up_arrow
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_getline_up_arrow(Ip_fd fd, char *buf, Ipcom_getline_cursor *cursor,
                       Ipcom_list *history_list1, Ipcom_getline_history **history_entry)
{
    if (history_list1 != IP_NULL)
    {
        /* Clean old command */
        if (ipcom_getline_cursor_home(fd, cursor) != IPCOM_SUCCESS)
            return IPCOM_ERR_FAILED;
        if (ipcom_getline_right_kill(fd, buf, cursor, IP_NULL) != IPCOM_SUCCESS)
            return IPCOM_ERR_FAILED;

        /* Get next command */
        if (*history_entry == IP_NULL)
        {
            *history_entry = IPCOM_LIST_FIRST(history_list1);
        }
        else
        {
            *history_entry = IPCOM_LIST_NEXT(&(*history_entry)->list);
            if (*history_entry == IP_NULL)
                *history_entry = IPCOM_LIST_LAST(history_list1);
        }

        /* Print next command */
        if (*history_entry != IP_NULL)
        {
            ipcom_strcpy(buf, (*history_entry)->cmd);
            if (ipcom_getline_putbuf(fd, (*history_entry)->cmd,
                ipcom_strlen((*history_entry)->cmd), cursor) < 0)
                return IPCOM_ERR_FAILED;
        }
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_getline_down_arrow
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_getline_down_arrow(Ip_fd fd, char *buf, Ipcom_getline_cursor *cursor,
                         Ipcom_list *history_list2, Ipcom_getline_history **history_entry)
{
    if (history_list2 != IP_NULL)
    {
        /* Clean old command */
        if (ipcom_getline_cursor_home(fd, cursor) != IPCOM_SUCCESS)
            return IPCOM_ERR_FAILED;
        if (ipcom_getline_right_kill(fd, buf, cursor, IP_NULL) != IPCOM_SUCCESS)
            return IPCOM_ERR_FAILED;

        /* Get previous command */
        if (*history_entry != IP_NULL)
            *history_entry = IPCOM_LIST_PREV(&(*history_entry)->list);

        /* Print previous command */
        if (*history_entry != IP_NULL)
        {
            ipcom_strcpy(buf, (*history_entry)->cmd);
            if (ipcom_getline_putbuf(fd, (*history_entry)->cmd,
                ipcom_strlen((*history_entry)->cmd), cursor) < 0)
                return IPCOM_ERR_FAILED;
        }
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_getline_add_history
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_getline_add_history(Ipcom_list *history_list3, int history_length, char *cmd)
{
    Ipcom_getline_history *hist;

    if (history_list3 != IP_NULL && history_length > 0)
    {
        /* Skip empty commands */
        if (*cmd == '\0')
            return;

        /* Skip if previous command is the same */
        hist = IPCOM_LIST_FIRST(history_list3);
        if (hist != IP_NULL && ipcom_strcmp(hist->cmd, cmd) == 0)
            return;

        if ((int)history_list3->size >= history_length)
        {
            hist = IPCOM_LIST_LAST(history_list3);
            if (hist != IP_NULL)
            {
                ipcom_list_remove(&hist->list);
                ipcom_free(hist->cmd);
                ipcom_free(hist);
            }
        }
        hist = ipcom_malloc(sizeof(*hist));
        if (hist != IP_NULL)
        {
            hist->cmd = ipcom_malloc(ipcom_strlen(cmd)+1);
            if (hist->cmd != IP_NULL)
            {
                ipcom_strcpy(hist->cmd, cmd);
                ipcom_list_insert_first(history_list3, &hist->list);
            }
            else
            {
                ipcom_free(hist);
            }
        }
    }
}


#if IPCOM_USE_FILE != IPCOM_FILE_NONE
/*
 *===========================================================================
 *                    ipcom_getline_restore_history
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_getline_restore_history(Ipcom_getline *gl)
{
    IP_FILE *f;
    char     line[1000];
    int      tkn;
    unsigned i;

    f = ipcom_fopen(IPCOM_FILE_ROOT IPCOM_GETLINE_HISTORY_FILENAME, "r");
    if (f != IP_NULL)
    {
        i = 0;
        while ((tkn = ipcom_fgetc(f)) != IP_EOF)
        {
            if (tkn == '\n')
            {
                line[i] = '\0';
                i = 0;
                ipcom_getline_add_history(&gl->history_list, gl->history_len, line);
            }
            else if (i < sizeof(line) - 1)
                line[i++] = (char) tkn;
        }

        (void)ipcom_fclose(f);
    }
}


/*
 *===========================================================================
 *                    ipcom_getline_save_history
 *==========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_getline_save_history(Ipcom_getline *gl)
{
    IP_FILE *f;

    f = ipcom_fopen(IPCOM_FILE_ROOT IPCOM_GETLINE_HISTORY_FILENAME, "w");
    if (f != IP_NULL)
    {
        Ipcom_getline_history *history_entry;

        for (history_entry = IPCOM_LIST_LAST(&gl->history_list);
             history_entry != IP_NULL;
             history_entry = IPCOM_LIST_PREV(&history_entry->list))
        {
            if (ipcom_fprintf(f, "%s\n", history_entry->cmd) < 0)
                break;
        }
        (void)ipcom_fclose(f);
    }
}
#endif /* IPCOM_USE_FILE != IPCOM_FILE_NONE */


/*
 ****************************************************************************
 * 10                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_getline_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ipcom_getline *
ipcom_getline_init(int history_len)
{
    Ipcom_getline *gl;

    gl = ipcom_malloc(sizeof(*gl));
    if (gl != IP_NULL)
    {
        ipcom_memset(gl, 0, sizeof(*gl));
        ipcom_list_init(&gl->history_list);
        gl->history_len = history_len;
        gl->row_len = 80;    /* Terminal defaults to 80 characters width */
        gl->clipboard = IP_NULL;

#if IPCOM_USE_FILE != IPCOM_FILE_NONE
        ipcom_getline_restore_history(gl);
#endif
    }
    return gl;
}


/*
 *===========================================================================
 *                    ipcom_getline_exit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_getline_exit(Ipcom_getline *gl)
{
    Ipcom_getline_history *history_entry;

    if (gl != IP_NULL)
    {
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
        ipcom_getline_save_history(gl);
#endif

        for (history_entry = IPCOM_LIST_FIRST(&gl->history_list);
             history_entry;
             history_entry = IPCOM_LIST_FIRST(&gl->history_list))
        {
            ipcom_list_remove(&history_entry->list);
            ipcom_free(history_entry->cmd);
            ipcom_free(history_entry);
        }
        if (gl->clipboard != IP_NULL)
            ipcom_free(gl->clipboard);
        ipcom_free(gl);
    }
}


/*
 *===========================================================================
 *                    ipcom_getline
 *==========================================================================
 * Description:     Get a line from an input stream
 * Parameters:      fd - file descriptor
 *                  buf - user supplied buffer
 *                  buflen - length of buffer
 *                  echo - controls echo
 *                  gl - getline handle (returned from ipcom_getline_init() call)
 *                  promptlen - length of the prompt;
 *
 * Returns:          0 = Complete command stored in buffer.
 *                  -1 = Descriptor closed.
 *                  -2 = Descriptor error.
 */
IP_PUBLIC int
ipcom_getline(Ip_fd fd, char *buf, int buflen, Ip_bool echo, Ipcom_getline *gl, int promptlen)
{
    int c;
    char cb[2] = { '\0', '\0' };
    Ipcom_getline_cursor cursor;
    Ip_bool in_esc = IP_FALSE, esc_bracket = IP_FALSE;
    Ip_bool in_iac = IP_FALSE, msb = IP_FALSE;
    Ipcom_getline_history *history_entry = IP_NULL;
    int   ret;

    ip_assert(gl != IP_NULL);

    ipcom_memset(&cursor, 0, sizeof(cursor));
    cursor.prompt_len = promptlen;
    cursor.position = promptlen;
    cursor.buf_len = buflen;
    cursor.row_len = gl->row_len;
    /* Echo only is explicitely requested and not already done by driver */
    cursor.echo = echo;

#if IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE && (defined(IP_PORT_OSE) || defined(IP_PORT_OSE5) || defined(IP_PORT_VXWORKS))
    if (!echo)
        ipcom_stdio_set_echo(fd, IP_FALSE);
#endif

    for(;;)
    {
        ip_assert(cursor.count >= cursor.position - cursor.prompt_len && cursor.position >= cursor.prompt_len);
        c = ipcom_getline_getone(fd);
        if (c < 0)
        {
            ret = c;
            goto outdone;
        }

        if (in_esc == IP_TRUE)
        {
            if (esc_bracket == IP_TRUE)
            {
                switch (c)
                {
                    case 'A':
                        if (ipcom_getline_up_arrow(fd, buf, &cursor, &gl->history_list, &history_entry) != IPCOM_SUCCESS)
                        {
                            ret = -2;
                            goto outdone;
                        }
                        break;
                    case 'B':
                        if (ipcom_getline_down_arrow(fd, buf, &cursor, &gl->history_list, &history_entry) != IPCOM_SUCCESS)
                        {
                            ret = -2;
                            goto outdone;
                        }
                        break;
                    case 'C':
                        if (ipcom_getline_right_arrow(fd, &cursor) != IPCOM_SUCCESS)
                        {
                            ret = -2;
                            goto outdone;
                        }
                        break;
                    case 'D':
                        if (ipcom_getline_left_arrow(fd, &cursor) != IPCOM_SUCCESS)
                        {
                            ret = -2;
                            goto outdone;
                        }
                        break;
                    default:
                        /* Drop character */
                        IPCOM_LOG1(DEBUG, "ipcom_getline :: unhandled escape sequence: ESC[%c", (char)c);
                        break;
                }
                in_esc = IP_FALSE;
                esc_bracket = IP_FALSE;
            }
            else if (c != '[')
            {
                /* Drop character */
                IPCOM_LOG1(DEBUG, "ipcom_getline :: unhandled escape sequence: ESC%c", (char)c);
                in_esc = IP_FALSE;
            }
            else
            {
                esc_bracket = IP_TRUE;
            }
        }
        else if (in_iac == IP_TRUE)
        {
            if (msb == IP_TRUE)
            {
                cursor.row_len += (Ip_u8)c;
                if (cursor.count > 0 && cursor.row_len != gl->row_len)
                    if (ipcom_getline_window_resize(fd, buf, &cursor, gl->row_len) != IPCOM_SUCCESS)
                    {
                        ret = IPCOM_ERR_FAILED;
                        goto outdone;
                    }
                gl->row_len = cursor.row_len;
                in_iac = IP_FALSE;
                msb = IP_FALSE;
                /*IPCOM_LOG1(DEBUG, "ipcom_getline :: rowlength: %d", cursor.row_len);*/
            }
            else
            {
                cursor.row_len = (Ip_u8)c;
                cursor.row_len <<= 8;
                msb = IP_TRUE;
            }
        }
        else
        {
            switch (c)
            {
                case 0x01:  /* CTRL-A */
                    if (ipcom_getline_cursor_home(fd, &cursor) != IPCOM_SUCCESS)
                    {
                        ret = -2;
                        goto outdone;
                    }
                    break;

                case 0x02:  /* CTRL-B */
                    if (ipcom_getline_left_arrow(fd, &cursor) != IPCOM_SUCCESS)
                    {
                        ret = -2;
                        goto outdone;
                    }
                    break;

                case 0x04:  /* CTRL-D */
                    if (ipcom_getline_ctrl_d(fd, buf, &cursor) != IPCOM_SUCCESS)
                    {
                        ret = -2;
                        goto outdone;
                    }
                    break;

                case 0x05:  /* CTRL-E */
                    if (ipcom_getline_cursor_end(fd, &cursor) != IPCOM_SUCCESS)
                    {
                        ret = -2;
                        goto outdone;
                    }
                    break;

                case 0x06:  /* CTRL-F */
                    if (ipcom_getline_right_arrow(fd, &cursor) != IPCOM_SUCCESS)
                    {
                        ret = -2;
                        goto outdone;
                    }
                    break;

                case 0x0b:  /* CTRL-K */
                    if (ipcom_getline_right_kill(fd, buf, &cursor, &gl->clipboard) != IPCOM_SUCCESS)
                    {
                        ret = -2;
                        goto outdone;
                    }
                    break;

                case 0x0e:  /* CTRL-N */
                    if (ipcom_getline_down_arrow(fd, buf, &cursor, &gl->history_list, &history_entry) != IPCOM_SUCCESS)
                    {
                        ret = -2;
                        goto outdone;
                    }
                    break;

                case 0x10:  /* CTRL-P */
                    if (ipcom_getline_up_arrow(fd, buf, &cursor, &gl->history_list, &history_entry) != IPCOM_SUCCESS)
                    {
                        ret = -2;
                        goto outdone;
                    }
                    break;

                case 0x19:  /* CTRL-Y */
                    if (ipcom_getline_paste_clipboard(fd, buf, &cursor, gl->clipboard) != IPCOM_SUCCESS)
                    {
                        ret = -2;
                        goto outdone;
                    }
                    break;

                case ESC:   /* Escape sequence start */
                    in_esc = IP_TRUE;
                    continue;

                case '\b':  /* BS */
                case 127:   /* DEL */
                    if (ipcom_getline_backspace(fd, buf, &cursor) != IPCOM_SUCCESS)
                    {
                        ret = -2;
                        goto outdone;
                    }
                    break;

                case '\r':  /* CR */
                case '\n':  /* LF */
                    buf[cursor.count] = '\0';
                    if (ipcom_getline_putbuf(fd, "\r\n", 2, &cursor) < 0)
                    {
                        ret = -2;
                        goto outdone;
                    }
                    if (echo)
                        ipcom_getline_add_history(&gl->history_list, gl->history_len, buf);
                    ip_assert(cursor.count >= cursor.position - cursor.prompt_len && cursor.position >= cursor.prompt_len);
                    ret = 0;
                    goto outdone;

                case IAC:   /* Telnet command */
                    in_iac = IP_TRUE;
                    continue;

                default:
                    if (c > 31 && c < 127)
                    {
                        cb[0] = (char)c;
                        if (ipcom_getline_paste_clipboard(fd, buf, &cursor, cb) != IPCOM_SUCCESS)
                        {
                            ret = -2;
                            goto outdone;
                        }
                    }
                    else
                    {
                        /* Drop character */
                    }
                    break;
             }
        }
    }

 outdone:
#if IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE && (defined(IP_PORT_OSE) || defined(IP_PORT_OSE5) || defined(IP_PORT_VXWORKS))
    if (!echo)
        ipcom_stdio_set_echo(fd, IP_TRUE);
#endif

    return ret;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

