/* ipftpc_api_proto.h - API of FTP client */
/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipftpc_api_proto.h,v $ $Revision: 1.12 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/include/ipftpc_api_proto.h,v $
 *   $Author: n $ $Date: 2008-04-02 13:19:09 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Ulf Olofsson <ulf@interpeak.se>
 ******************************************************************************
 */
#ifndef IPFTPC_API_PROTO_H
#define IPFTPC_API_PROTO_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
This library contains the API for Wind River FTP.
*/

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif

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
 * 6                    LOCAL FUNCTIONS
 ****************************************************************************
 */

#ifdef IPFTPC_API_C

/*
 *===========================================================================
 *      ipftpc_ntop
 *===========================================================================
 * Description:
 *      Print IP address using ntop.
 *
 * Parameters:
 *      addr    IP address.
 *      dest    Result buffer.
 *      len     Size of result buffer.
 *
 * Returns:
 *      Pointer to buffer if success, 0 otherwise.
 */
IP_STATIC const char *
ipftpc_ntop(Ipftpc_addr *addr, char *dest, int len);


/*
 *===========================================================================
 *      ipftpc_flush
 *===========================================================================
 * Description:
 *      Flush control socket input queue.
 *
 * Parameters:
 *      session Session descriptor.
 *      fdrd    Read file descriptor.
 *      fdwr    Write file descriptor.
 *      timeout Timeout in seconds.
 *
 * Returns:
 *      -1 if select error.
 *      0 if timeout.
 *      1 if read descriptor ready.
 *      2 if write descriptor ready.
 *      4 if control channel is ready for read,
 *        and no read descriptor is supplied.
 */
IP_STATIC int
ipftpc_flush(Ipftpc_session *session);


/*
 *===========================================================================
 *      ipftpc_sockwait
 *===========================================================================
 * Description:
 *      Wait until socket ready for I/O.
 *
 * Parameters:
 *      session Session descriptor.
 *      fdrd    Read file descriptor.
 *      fdwr    Write file descriptor.
 *      timeout Timeout in seconds.
 *
 * Returns:
 *      -1 if select error.
 *      0 if timeout.
 *      1 if read descriptor ready.
 *      2 if write descriptor ready.
 *      4 if control channel is ready for read,
 *        and no read descriptor is supplied.
 */
IP_STATIC int
ipftpc_sockwait(Ipftpc_session *session, Ip_fd fdrd, Ip_fd fdwr, int timeout);


/*
 *===========================================================================
 *      ipftpc_dsock_active
 *===========================================================================
 * Description:
 *      Prepare socket for active connection (by server).
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_dsock_active(Ipftpc_session *session);


/*
 *===========================================================================
 *      ipftpc_dsock_passive
 *===========================================================================
 * Description:
 *      Prepare socket for passive connection (by client).
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_dsock_passive(Ipftpc_session *session);


/*
 *===========================================================================
 *      ipftpc_dsock_vopen
 *===========================================================================
 * Description:
 *      Open data socket and send FTP command to control socket.
 *
 * Parameters:
 *      session Session descriptor.
 *      fmt     Format string for FTP command.
 *      argptr  Additional parms for FTP command.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_dsock_vopen(Ipftpc_session *session, const char *fmt, va_list argptr);


/*
 *===========================================================================
 *      ipftpc_dsock_open
 *===========================================================================
 * Description:
 *      Open data socket and send FTP command to control socket.
 *
 * Parameters:
 *      session Session descriptor.
 *      fmt     Format string for FTP command.
 *      ...     Additional parms for FTP command.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_dsock_open(Ipftpc_session *session, const char *fmt, ...);


/*
 *===========================================================================
 *      ipftpc_sock_close
 *===========================================================================
 * Description:
 *      Close socket.
 *
 * Parameters:
 *      session Session descriptor.
 *      sock    Socket descriptor.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_sock_close(Ipftpc_session *session, Ipftpc_sockcb *sock);


/*
 *===========================================================================
 *      ipftpc_telnet
 *===========================================================================
 * Description:
 *      Handle telnet stuff.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftpc_telnet(Ipftpc_session *session);


/*
 *===========================================================================
 *      ipftpc_sread
 *===========================================================================
 * Description:
 *      Get input line from control socket.
 *
 * Parameters:
 *      session Session descriptor.
 *      timeout Timeout in seconds.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_sread(Ipftpc_session *session, int timeout);


/*
 *===========================================================================
 *      ipftpc_sin
 *===========================================================================
 * Description:
 *      Get terminating input line from control socket.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_sin(Ipftpc_session *session);


/*
 *===========================================================================
 *      ipftpc_sinpoll
 *===========================================================================
 * Description:
 *      Poll for terminating input line from control socket.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_sinpoll(Ipftpc_session *session);


/*
 *===========================================================================
 *      ipftpc_swrite
 *===========================================================================
 * Description:
 *      Write to control socket.
 *
 * Parameters:
 *      session Session descriptor.
 *      buf     Buffer.
 *      len     Length of buffer.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_swrite(Ipftpc_session *session, void *buf, int len);


/*
 *===========================================================================
 *      ipftpc_vsout
 *===========================================================================
 * Description:
 *      Write formatted string to control socket,
 *      interpret FTP command and set local flags accordingly.
 *
 * Parameters:
 *      session Session descriptor.
 *      cl      Append CR,LF to output.
 *      fmt     Format string.
 *      argptr  Additional arguments.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_vsout(Ipftpc_session *session, int cl, const char *fmt, va_list argptr);


/*
 *===========================================================================
 *      ipftpc_sout
 *===========================================================================
 * Description:
 *      Write formatted string to control socket,
 *
 * Parameters:
 *      session Session descriptor.
 *      cl      Append CR,LF to output.
 *      fmt     Format string.
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_STATIC int
ipftpc_sout(Ipftpc_session *session, int cl, const char *fmt, ...);


/*
 *===========================================================================
 *      ipftpc_cout
 *===========================================================================
 * Description:
 *      Send buffer to console.
 *
 * Parameters:
 *      session Session descriptor.
 *      buf     Buffer.
 *      bufsize Buffer size.
 *      orphant True if last buffer was terminated with '\r'.
 *
 * Returns:
 */
IP_STATIC int
ipftpc_cout(Ipftpc_session *session, char *buf, int bufsize, Ip_bool *orphant);


/*
 *===========================================================================
 *      ipftpc_filewrite_ascii
 *===========================================================================
 * Description:
 *      Write ASCII buffer to file.
 *
 * Parameters:
 *      fd      File descriptor.
 *      buf     Buffer.
 *      nbuf    New buffer.
 *      bufsize Buffer size.
 *      orphant True if last buffer was terminated with '\r'.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftpc_filewrite_ascii(Ip_fd fd, char *buf, char *nbuf, int bufsize,
                       Ip_bool *orphant);


/*
 *===========================================================================
 *      ipftpc_socketwrite_ascii
 *===========================================================================
 * Description:
 *      Write ASCII buffer to socket.
 *
 * Parameters:
 *      fd      File descriptor.
 *      buf     Buffer.
 *      nbuf    New buffer.
 *      bufsize Buffer size.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftpc_socketwrite_ascii(Ip_fd fd, char *buf, char *nbuf, int bufsize);


#endif /* IPFTPC_API_C */


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *      ipftpc_create
 *===========================================================================
 * Description:
 *      Initialize FTP service.
 *
 * Parameters:
 *
 * Returns:
 *      IPCOM_SUCCESS, or error number.
 */
IP_PUBLIC Ip_err
ipftpc_create(void);


/*
 *===========================================================================
 *      ipftpc_exit
 *===========================================================================
 * Description:
 *      Deinitialize FTP service and free resources. This
 *      requires that all FTP sessions are closed.
 *
 * Parameters:
 *
 * Returns:
 *      IPFTPC_ESUCCESS, or error number.
 */
IP_PUBLIC int
ipftpc_exit(void);


/*******************************************************************************
*
* ipftpc_open - open a connection to an FTP server
*
* This routine opens a connection to an FTP server.
*
* Parameters:
* \is
* \i <idp>
* A pointer to the location for storing the session ID.
* \i <addr>
* A pointer to an 'ipftpc_addr' union containing the IP address of the FTP
* server. 'ipftpc_addr' is defined as follows:
* \cs
* typedef union
* {
*     struct Ip_sockaddr sa;              /@ Generic address @/
*     struct Ip_sockaddr_in sin;          /@ IPv4 address @/
* #ifdef IPCOM_USE_INET6
*     struct Ip_sockaddr_in6 sin6;        /@ IPv6 address @/
* #endif
* }
* Ipftpc_addr;
* \ce
*
* \i <attr>
* A pointer to an 'ipftpc_attr' structure containing the session attributes to
* use when communicating with the server. The 'ipftpc_attr' structure is defined
* as follows:
* \cs
* typedef struct Ipftpc_attr
* {
* 	Ip_bool extended;             /@ Use extended addressing @/
* 	Ip_bool passive;              /@ Passive mode @/
* 	Ip_bool ipcheck;              /@ Disable proxy FTP @/
* 	Ip_bool verbose;              /@ Enable verbose printouts @/
* 	Ip_bool fb_extended;          /@ Allow fallback extended->normal @/
* 	Ip_bool fb_passive;           /@ Allow fallback passive->active @/
* 	Ip_u16 pportbase;             /@ Reject peer ports <= pportbase @/
* 	Ip_u16 lportbase;             /@ Use local ports from lportbase+1 @/
* 	void *data;                   /@ User data @/
*
* 	/@ Console output @/
* 	int (*conout)(struct Ipftpc_attr *attr, const char *buf, int len);
*
* 	/@ Control input (for debug) @/
* 	void (*ctrlin)(struct Ipftpc_attr *attr, const char *buf, int len);
* 	/@ Control output (for debug) @/
* 	void (*ctrlout)(struct Ipftpc_attr *attr, const char *buf, int len);
* }
* Ipftpc_attr;
* \ce
*
*
* The following describe, at somewhat greater length, the fields in the
* 'ipftpc_attr' structure that correspond to communication attributes.:
*
* \ml
* \m -
* <extended>
*
* Use extended addressing for IPv4 connections.
* \m -
* <passive>
*
* Use passive mode when connecting a server.
* \m -
* <ipcheck>
*
* Check that the IP address of the control and data connection correspond. This
* effectively disables proxy FTP.
* \m -
* <verbose>
*
* Enable printouts of client/server communication to the 'ctrlin' and 'ctrlout'
* attribute routines defined below. Bit 0 enables 'ctrlin', and bit 1 enables
* 'ctrlout'.
* \m -
* <fb_extended>
*
* Allow fallback from extended mode to normal.
* \m -
* <fb_passive>
*
* Allow fallback from passive mode to active.
* \m -
* <pportbase>
*
* Reject data connection to server ports that have a number that is less than or
* equal to this value.
* \m -
* <lportbase>
*
* Use local port numbers that are above this value. A value of 0 will let the
* client operating system select a port number.
* \me
* \i <data>
* User-supplied data that can be used in the following I/O routines:''
*
* 'int (*conout)(struct Ipftpc_attr *attr, char *buf, int len)'
*
* Console output routine called when there is output from the ipftpc_list()
* routine. The buffer is terminated with a “\\0”, which allows standard
* C-library output routines to be used. If this value is 'IP_NULL', there is no
* output.
*
* 'void (*ctrlin)(struct Ipftpc_attr *attr, char *buf, int len)'
*
* Control input routine, which is called when the FTP client reads a valid input
* line from the server. The buffer is terminated with a “'\\0'”, which allows
* standard C-library output routines to be used. The routine is called only if
* the verbose attribute in the 'ipftpc_attr' structure has bit 0 set.
*
* 'void (*ctrlout)(struct Ipftpc_attr *attr, char *buf, int len)'
*
* Control output routine called when the FTP client sends a command to the
* server. The buffer is terminated with a “'\\0'”, which allows standard
* C-library output routines to be used. This routine is called only if the
* 'verbose' attribute in the 'ipftpc_attr' structure has bit 1 set.
* \ie
*
* EXAMPLE
* \cs
* void example(void)
* {
*     Ipftpc_addr addr;
*     Ipftpc_attr attr;
*     Ipftpc_id id;
*
*     /@ Clear @/
*     ipcom_memset(&addr, 0, sizeof(addr));
*     ipcom_memset(&attr, 0, sizeof(attr));
*
*     /@ Use IPv4 loopback, port 21 @/
*     addr.sin.sin_family = IPAF_INET;
*     addr.sin.sin_port = ip_htons(21);
*     if (ipcom_inet_pton(IPAF_INET, “127.0.0.1”,
*                         &addr.sin.sin_addr) != 1)
*     {
*         printf("Bad IPv4 address\n");
*         return;
*     }
*
*     /@ Set attributes @/
*     attr.passive = 1;
*     attr.ipcheck = 1;
*     attr.fb_passive = 1;
*
*     /@ Open connection @/
*     ipftpc_open(&id, &addr, &attr);
* }
* \ce
*
* RETURNS: Either 'IPFTPC_ESUCCESS' or an error code containing an error number.
* An FTP-client error code is a combination of an error class and an error
* number. For information on FTP client error codes, see FTP in the Wind River
* network stack documentation [* replace w/ specific volume and chapter when
* available] .
*
* ERRNO:
*/
   IP_PUBLIC int ipftpc_open
   (
    Ipftpc_id *idp,
    Ipftpc_addr *addr,
    Ipftpc_attr *attr
   );


/*******************************************************************************
*
* ipftpc_close - close a connection to an FTP server
*
* This routine closes a connection to an FTP server.
*
* Parameter:
* \is
* \i <id>
* The session ID, obtained from the ipftpc_open() call.
* \ie
*
* EXAMPLE
* \cs
* void example(Ipftpc_id id)
* {
*     /@ Close connection @/
*     ipftpc_close(id);
* }
* \ce
*
* RETURNS: Either 'IPFTPC_ESUCCESS' or an error code containing an error number.
* An FTP-client error code is a combination of an error class and an error
* number. For information on FTP client error codes, see FTP in the Wind River
* network stack documentation [* replace w/ specific volume and chapter when
* available] .
*
* ERRNO:
*/
   IP_PUBLIC int ipftpc_close
   (
    Ipftpc_id id
   );


/*******************************************************************************
*
* ipftpc_getattr - get the current set of session attributes
*
* This routine gets the current set of session attributes.
*
* Parameters:
* \is
* \i <id>
* The session ID, obtained from the ipftpc_open() call.
* \i <attr>
* A pointer to an 'ipftpc_attr' structure containing the current session
* attributes. For the definition of the 'ipftpc_attr' structure, see
* ipftpc_open().
* \ie
*
* EXAMPLE
* \cs
* void example(Ipftpc_id id)
* {
*     Ipftpc_attr attr;
*
*     /@ Set passive mode @/
*     ipftpc_getattr(id, &attr);
*     attr.passive = 1;
*     ipftpc_setattr(id, &attr);
* }
* \ce
*
* RETURNS: Either 'IPFTPC_ESUCCESS' or an error code containing an error number.
* An FTP-client error code is a combination of an error class and an error
* number. For information on FTP client error codes, see FTP in the Wind River
* network stack documentation [* replace w/ specific volume and chapter when
* available] .
*
* ERRNO:
*/
   IP_PUBLIC int ipftpc_getattr
   (
    Ipftpc_id id,
    Ipftpc_attr *attr
   );


/*******************************************************************************
*
* ipftpc_setattr - set new session attributes
*
* This routine sets new session attributes.
*
* Parameters:
* \is
* \i <id>
* Session ID, obtained from the ipftpc_open() call.
* \i <attr>
* A pointer to an 'ipftpc_attr' structure containing the attributes available
* for communication with the server. For the definition of the 'ipftpc_attr'
* structure, see ipftpc_open().
* \i <data>
* User-supplied data that can be used in the following I/O routines:''
*
* 'int (*conout)(struct Ipftpc_attr *attr, char *buf, int len)'
*
* Console output routine called when there is output from the ipftpc_list()
* routine. The buffer is terminated with a “\\0”, which allows standard
* C-library output routines to be used. If this value is 'IPNULL', there is no
* output.
*
* 'void (*ctrlin)(struct Ipftpc_attr *attr, char *buf, int len)'
*
* Control input routine, which is called when the FTP client reads a valid input
* line from the server. The buffer is terminated with a “'\\0'”, which allows
* standard C-library output routines to be used. The routine is called only if
* the verbose attribute in the 'ipftpc_attr' structure has bit 0 set.
*
* 'void (*ctrlout)(struct Ipftpc_attr *attr, char *buf, int len)'
*
* Control output routine called when the FTP client sends a command to the
* server. The buffer is terminated with a “'\\0'”, which allows standard
* C-library output routines to be used. This routine is called only if the
* 'verbose' attribute in the 'ipftpc_attr' structure has bit 1 set.
* \ie
*
* EXAMPLE
* \cs
* void example(Ipftpc_id id)
* {
*     Ipftpc_attr attr;
*
*     /@ Set passive mode @/
*     ipftpc_getattr(id, &attr);
*     attr.passive = 1;
*     ipftpc_setattr(id, &attr);
* }
* \ce
*
* RETURNS: Either 'IPFTPC_ESUCCESS' or an error code containing an error number.
* An FTP-client error code is a combination of an error class and an error
* number. For information on FTP client error codes, see FTP in the Wind River
* network stack documentation [* replace w/ specific volume and chapter when
* available] .
*
* ERRNO:
*/
   IP_PUBLIC int ipftpc_setattr
   (
    Ipftpc_id id,
    Ipftpc_attr *attr
   );


/*******************************************************************************
*
* ipftpc_cmd - send a command to the FTP server and wait for a reply
*
* This routine sends a command to the FTP server and waits for a reply. The
* command can not be used with a data connection.
*
* Parameters:
* \is
* \i <id>
* The session ID, obtained from an ipftpc_open() call.
* \i <fmt>
* Printf-style format string, used to create the FTP command along with possible
* additional parameters.
* \i <…>
* Additional parameters that are used to create the FTP command.
* \ie
*
* EXAMPLE
* \cs
* void example(Ipftpc_id id, char ttype)
* {
*     /@ Set new transfer type @/
*     ipftpc_cmd(id, “TYPE %c”, ttype);
* }
* \ce
*
* RETURNS: Either 'IPFTPC_ESUCCESS' or an error code containing an error number.
* An FTP-client error code is a combination of an error class and an error
* number. For information on FTP client error codes, see FTP in the Wind River
* network stack documentation [* replace w/ specific volume and chapter when
* available] .
*
* ERRNO:
*/
   IP_PUBLIC int ipftpc_cmd
   (
    Ipftpc_id id,
    const char *fmt,
    ...
   );


/*******************************************************************************
*
* ipftpc_login - log in to an FTP server
*
* This routine logs in to an FTP server with username and password.
*
* Parameters:
* \is
* \i <id>
* Session ID, obtained from the ipftpc_open() call.
* \i <username>
* User name.
* \i <password>
* Password.
* \ie
*
* EXAMPLE
* \cs
* void example(Ipftpc_id id, char *mailaddress)
* {
*     /@ Anonymous login @/
*     ipftpc_login(id, “anonymous”, mailaddress);
* }
* \ce
*
* RETURNS: Either 'IPFTPC_ESUCCESS' or an error code containing an error number.
* An FTP-client error code is a combination of an error class and an error
* number. For information on FTP client error codes, see FTP in the Wind River
* network stack documentation [* replace w/ specific volume and chapter when
* available] .
*
* ERRNO:
*/
   IP_PUBLIC int ipftpc_login
   (
    Ipftpc_id id,
    const char *username,
    const char *password
   );


/*******************************************************************************
*
* ipftpc_list - send a list command to the FTP server and wait for a reply
*
* Send a list command to the FTP server and wait for a reply. The command opens
* a data connection, and all data output from the FTP server is sent to the
* routine in the <conout> attribute.
*
* Parameters:
* \is
* \i <id>
* Session ID, obtained from the pftpc_open() call.
* \i <fmt>
* Printf-style format string, used to create the FTP list command along with
* possible additional parameters.
* \i <…>
* Additional parameters that are used to create the FTP list command.
* \ie
*
* EXAMPLE
* \cs
* void example(Ipftpc_id id, char *name)
* {
*     /@ List file(s) @/
*     ipftpc_list(id, “LIST %s”, name);
* }
* \ce
*
* RETURNS: Either 'IPFTPC_ESUCCESS' or an error code containing an error number.
* An FTP-client error code is a combination of an error class and an error
* number. For information on FTP client error codes, see FTP in the Wind River
* network stack documentation [* replace w/ specific volume and chapter when
* available] .
*
* ERRNO:
*/
   IP_PUBLIC int ipftpc_list
   (
    Ipftpc_id id,
    const char *fmt,
    ...
   );


/*******************************************************************************
*
* ipftpc_get - get a file from the FTP server and wait for a reply
*
* This routine gets a file from the FTP server and waits for a reply. A data
* connection is opened to transfer the file.
*
* Parameters:
* \is
* \i <id>
* The session ID, obtained from an ipftpc_open() call.
* \i <lpath>
* The local filename.
* \i <rpath>
* The remote filename.
* \ie
*
* EXAMPLE
* \cs
* void example(Ipftpc_id id, char *name)
* {
*     /@ Get file @/
*     ipftpc_get(id, name, name);
* }
* \ce
*
* RETURNS: Either 'IPFTPC_ESUCCESS' or an error code containing an error number.
* An FTP-client error code is a combination of an error class and an error
* number. For information on FTP client error codes, see FTP in the Wind River
* network stack documentation [* replace w/ specific volume and chapter when
* available] .
*
* ERRNO:
*/
   IP_PUBLIC int ipftpc_get
   (
    Ipftpc_id id,
    const char *lpath,
    const char *rpath
   );


/*******************************************************************************
*
* ipftpc_put - send a file to the FTP server and wait for a reply
*
* This routine sends a file to the FTP server and waits for a reply. A data
* connection is opened to transfer the file.
*
* Parameters:
* \is
* \i <id>
* The session ID, obtained from the ipftpc_open() call.
* \i <lpath>
* The local filename.
* \i <rpath>
* The remote filename.
* \i <append>
* An integer; if zero, replace the remote file with the
* local file; otherwise, append the local file to the remote file.
* \ie
*
* \h Example
* \cs
* void example(Ipftpc_id id, char *name)
* {
*     /@ Put file @/
*     ipftpc_put(id, name, name, 0);
* }
* \ce
*
* RETURNS: Either 'IPFTPC_ESUCCESS' or an error code containing an error number.
* An FTP-client error code is a combination of an error class and an error
* number. For information on FTP client error codes, see FTP in the Wind River
* network stack documentation [* replace w/ specific volume and chapter when
* available] .
*
* ERRNO:
*/
   IP_PUBLIC int ipftpc_put
   (
    Ipftpc_id id,
    const char *lpath,
    const char *rpath,
    int append
   );


/*******************************************************************************
*
* ipftpc_lastreply - get the last server output processed by the FTP client
*
* This routine gets the last server output that was processed by the FTP client.
*
* Parameter:
* \is
* \i <id>
* Session ID, obtained from the ipftpc_open() call.
* \ie
*
* EXAMPLE
* \cs
* void example(Ipftpc_id id)
* {
*     /@ Check system @/
*     ipftpc_cmd(id, “SYST”);
*     printf(“Server system: %s\n”, ipftpc_lastreply(id));
* }
* \ce
*
* RETURNS: Last server output string.
*
* ERRNO:
*/
   IP_PUBLIC char *ipftpc_lastreply
   (
    Ipftpc_id id
   );


/*******************************************************************************
*
* ipftpc_strclass - get a text description of an error class
*
* This routine gets a text description of an error class. An FTP-client error
* code is a combination of an error class and an error number. For information
* on FTP client error codes, see FTP in the Wind River network stack
* documentation [* replace w/ specific volume and chapter when available] .
*
* Parameter:
* \is
* \i <err>
* Error code from one of the routines in the client API.
* \ie
*
* EXAMPLE
* \cs
* void example(Ipftpc_id id)
* {
*     int err;
*
*     /@ Check system @/
*     if ((err = ipftpc_cmd(id, “SYST”)) != 215)
*     {
*         printf(“Error class: %s\n”, ipftpc_strclass(err));
*     }
* }
* \ce
*
* RETURNS: A string describing the error class.
*
* ERRNO:
*/
   IP_PUBLIC char *ipftpc_strclass
   (
    int err
   );


/*******************************************************************************
*
* ipftpc_strerror - get a textual description of an error
*
* This routine gets a text description of an error number. An FTP-client error
* code is a combination of an error class and an error number. For information
* on FTP client error codes, see FTP in the Wind River network stack
* documentation [* replace w/ specific volume and chapter when available] .
*
* Parameter:
* \is
* \i <err>
* Error code from one of the routines in the client API.
* \ie
*
* EXAMPLE
* \cs
* void example(Ipftpc_id id)
* {
*     int err;
*
*     /@ Check system @/
*     if ((err = ipftpc_cmd(id, “SYST”)) != 215)
*     {
*         printf(“Error description: %s\n”,
*                ipftpc_strerror(err));
*     }
* }
* \ce
*
* RETURNS: A string describing the error number.
*
* ERRNO:
*/
   IP_PUBLIC char *ipftpc_strerror
   (
    int err
   );


#ifdef __cplusplus
}
#endif

#endif
/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
