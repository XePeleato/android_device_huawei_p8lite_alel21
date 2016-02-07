#ifndef _HI_SOCKET_H
#define _HI_SOCKET_H
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/time.h>

#ifdef __KERNEL__

typedef struct {
    unsigned int fds_bits;
} hi_fd_set;

#define __HI_NFDBITS	(8 * sizeof(unsigned int))
#define	__HI_FDMASK(d)	(1UL << ((d) % __HI_NFDBITS))

#define HI_FD_ISSET(d, set) ((set)->fds_bits & __HI_FDMASK(d))
#define HI_FD_SET(d, set) ((set)->fds_bits |= __HI_FDMASK(d))
#define HI_FD_CLR(d, set)  ((set)->fds_bits &= ~ __HI_FDMASK(d))
#define HI_FD_ZERO(set)   (memset(set, 0, sizeof(*set)))

#define DISABLE_SOCKET_DEBUG 0
#define ENABLE_SOCKET_DEBUG 1



int hi_socket(int family, int type, int protocol);
int hi_bind(int fd, struct sockaddr *addr, int addrlen);
int hi_listen(int fd, int backlog);
int hi_accept(int fd, struct sockaddr *upeer_sockaddr, int *upeer_addrlen);
int hi_connect(int fd, struct sockaddr *addr, int addrlen);
int hi_select(int n, hi_fd_set *inp, hi_fd_set *outp, hi_fd_set *exp, struct timeval *tvp);
int hi_close(int fd);
int hi_send(int fd, void *buff, size_t len, int flags);
int hi_recv(int fd, void *ubuf, size_t size, int flags);
int hi_sendto(int fd, void *buff, size_t len, unsigned flags,
                    struct sockaddr *addr, int addr_len);
int hi_recvfrom(int fd, void *ubuf, size_t size, unsigned flags,
                      struct sockaddr *addr, int *addr_len);
int hi_getsockname(int fd, struct sockaddr *addr, int *addrlen);
int hi_getpeername(int fd, struct sockaddr *addr, int *addrlen);
int hi_getsockopt(int fd, int level, int optname, char *optval, int *optlen);
int hi_setsockopt(int fd, int level, int optname, char *optval, unsigned int optlen);
int hi_ioctl(int fd, int cmd, unsigned int arg);
int hi_shutdown(int fd, enum sock_shutdown_cmd how);
unsigned int hi_inet_addr(const char * str);

#endif


#endif
