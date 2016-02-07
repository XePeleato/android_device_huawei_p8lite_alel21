/*******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 *******************************************************************************
  文 件 名   : _usb_shell.c
  版 本 号   : 初稿
  作    者   : 周明发 00145324
  生成日期   : 2012年11月13日
  最近修改   :
  功能描述   : 用于实现A SHELL / C SHELL虚拟串口模块
  修改历史   :
  1.日    期   : 2012年11月13日
    作    者   : 周明发 00145324
    修改内容   : 创建文件

*******************************************************************************/

/*******************************************************************************
  1 头文件包含
*******************************************************************************/

#include "usb_shell.c"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*******************************************************************************
  2 全局变量定义
*******************************************************************************/
static void test_tty_write_cb(struct console *cons, const char *buf, unsigned count);

struct tty_struct g_tty_ut;
const struct tty_operations g_tty_ops_ut_write_null = {
	.write = NULL,
};
const struct tty_operations g_tty_ops_ut = {
	.write = test_tty_write_cb,
};

/*******************************************************************************
  3 函数声明
*******************************************************************************/

/*******************************************************************************
  4 函数实现
*******************************************************************************/

/*******************************************************************************
 函 数 名: test_tty_write_cb
 功能描述: tty写回调
 输入参数: 无
 输出参数: 无
 返 回 值: -1 失败 0 成功
*******************************************************************************/
static void test_tty_write_cb(struct console *cons, const char *buf, unsigned count)
{
    /* just a stub, do nothing */;
    return 0;
}

/*******************************************************************************
 函 数 名: test_ashell_console_write_fail1
 功能描述: 参数错误导致失败
 输入参数: 无
 输出参数: 无
 返 回 值: -1 失败 0 成功
*******************************************************************************/
int test_ashell_console_write_fail1(void)
{
    ashell_console_write((struct console *)NULL, NULL, 0);
    return -1;
}

/*******************************************************************************
 函 数 名: test_ashell_console_write_fail2
 功能描述: tty为空导致失败
 输入参数: 无
 输出参数: 无
 返 回 值: -1 失败 0 成功
*******************************************************************************/
int test_ashell_console_write_fail2(void)
{
    char *buf = "abcd";

    g_ashell_ctx.tty = NULL;
    ashell_console_write((struct console *)(&(g_ashell_ctx.port_console)), buf, 4);
    return -1;
}

/*******************************************************************************
 函 数 名: test_ashell_console_write_fail3
 功能描述: device为空导致失败
 输入参数: 无
 输出参数: 无
 返 回 值: -1 失败 0 成功
*******************************************************************************/
int test_ashell_console_write_fail3(void)
{

    struct tty_driver *device = NULL;
    char *buf = "abcd";


    device = g_ashell_ctx.port_console.device;
    g_ashell_ctx.port_console.device = NULL;
    ashell_console_write((struct console *)(&(g_ashell_ctx.port_console)), buf, 4);
    g_ashell_ctx.port_console.device = device;
    return -1;
}

/*******************************************************************************
 函 数 名: test_ashell_console_write_fail4
 功能描述: write为空导致失败
 输入参数: 无
 输出参数: 无
 返 回 值: -1 失败 0 成功
*******************************************************************************/
int test_ashell_console_write_fail4(void)
{
    struct tty_struct *tty = NULL;
    struct tty_driver *device = NULL;
    int *write = NULL;
    char *buf = "abcd";

    g_ashell_ctx.tty = &g_tty_ut;
    g_ashell_ctx.tty->ops = &g_tty_ops_ut_write_null;

    ashell_console_write((struct console *)(&(g_ashell_ctx.port_console)), buf, 4);

    return -1;
}

/*******************************************************************************
 函 数 名: test_ashell_console_write_ok
 功能描述: ashell 写成功
 输入参数: 无
 输出参数: 无
 返 回 值: -1 失败 0 成功
*******************************************************************************/
int test_ashell_console_write_ok(void)
{
    struct tty_struct *tty = NULL;
    struct tty_driver *device = NULL;
    char *buf = "abcd";

    g_ashell_ctx.tty = &g_tty_ut;
    g_ashell_ctx.tty->ops = &g_tty_ops_ut;

    ashell_console_write((struct console *)(&(g_ashell_ctx.port_console)), buf, 4);

    return 0;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

