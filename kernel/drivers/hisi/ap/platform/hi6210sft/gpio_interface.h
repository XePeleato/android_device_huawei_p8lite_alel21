#ifndef __GPIOINTERFACE_H_
#define __GPIOINTERFACE_H_




#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* --------------------------GPIO寄存器组列表<begin>------------------------- */


#define GPIO_SHINE_DB4                  ( DB4 )                                 /* GPIO亮灯BIT */

/* GPIO寄存器组偏移地址列表 */
#define GPIODATA                        ( 0x00 )                                /* 数据寄存器 */
#define GPIODIR                         ( 0x04 )                                /* 方向控制寄存器 */
/* -------------------------- GPIO寄存器组列表<end> ------------------------- */


/* --------------------------UART寄存器组列表<begin>------------------------- */

#define UART_RBR_THR_DLL                ( UART_BASE_ADDR_LOW + 0x000 )          /* UART接收发送数据和波特率低8位寄存器寄存器 */
#define UART_DLH_IER                    ( UART_BASE_ADDR_LOW + 0x004 )          /* UART中断使能和波特率高8位寄存器 */
#define UART_IIR_FCR                    ( UART_BASE_ADDR_LOW + 0x008 )          /* UART中断ID和fifo控制寄存器 */
#define UART_LCR                        ( UART_BASE_ADDR_LOW + 0x00C )          /* UART线性控制寄存器 */
#define UART_MCR                        ( UART_BASE_ADDR_LOW + 0x010 )          /* UART Modem控制寄存器 */
#define UART_LSR                        ( UART_BASE_ADDR_LOW + 0x014 )          /* UART线性状态寄存器 */
#define UART_MSR                        ( UART_BASE_ADDR_LOW + 0x018 )          /* Modem状态寄存器 */
#define UART_SCR                        ( UART_BASE_ADDR_LOW + 0x01C )          /* 中间结果暂存寄存器 */
#define UART_FAR                        ( UART_BASE_ADDR_LOW + 0x070 )          /* FIFO访问控制寄存器 */
#define UART_TFR                        ( UART_BASE_ADDR_LOW + 0x074 )          /* 发送FIFO读寄存器 */
#define UART_RFW                        ( UART_BASE_ADDR_LOW + 0x078 )          /* 接收FIFO写寄存器 */
#define UART_USR                        ( UART_BASE_ADDR_LOW + 0x07C )          /* UART状态寄存器 */
#define UART_TFL                        ( UART_BASE_ADDR_LOW + 0x080 )          /* 发送FIFO数据个数寄存器 */
#define UART_RFL                        ( UART_BASE_ADDR_LOW + 0x084 )          /* 接收FIFO数据个数寄存器 */
#define UART_HTX                        ( UART_BASE_ADDR_LOW + 0x0A4 )          /* 暂停发送寄存器 */
#define UART_DMASA                      ( UART_BASE_ADDR_LOW + 0x0A8 )          /* DMA软件响应寄存器 */
#define UART_UCV                        ( UART_BASE_ADDR_LOW + 0x0F8 )          /* UART版本寄存器 */
#define UART_CTR                        ( UART_BASE_ADDR_LOW + 0x0FC )          /* 外设ID寄存器 */
/* --------------------------UART寄存器组列表<end>------------------------- */



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif