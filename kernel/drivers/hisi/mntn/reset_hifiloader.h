/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : reset_hifiloader.h
  版 本 号   : 初稿
  作    者   : 刘慈红 lKF71598
  生成日期   : 2012年8月29日
  最近修改   :
  功能描述   : reset_hifiloader.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年8月29日
    作    者   : 刘慈红 lKF71598
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include <linux/hisi/reset.h>


#ifndef __RESET_HIFILOADER_H__
#define __RESET_HIFILOADER_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define HIFI_IMAGE_NAME          ("hifi")

#define HIFI_PHY2VIRT_BEGIN_ADDR (0xC0000000)
#define HIFI_PHY2VIRT_END_ADDR   (0xEFFFFFFF)
#define HIFI_PHY2VIRT_OFFSET     (0xC0000000)

/* 安全加载 0，非安全加载 1 */
#define HIFI_SAFE_LOAD           (0)
#define HIFI_UNSAFE_LOAD         (1)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 实 体 名  : DRV_HIFI_IMAGE_SEC_TYPE_ENUM
 功能描述  : 镜像段类型
*****************************************************************************/
enum DRV_HIFI_IMAGE_SEC_TYPE_ENUM {
    DRV_HIFI_IMAGE_SEC_TYPE_CODE = 0,        /* 代码 */
    DRV_HIFI_IMAGE_SEC_TYPE_DATA,            /* 数据 */
    DRV_HIFI_IMAGE_SEC_TYPE_BUTT,
};
typedef unsigned char DRV_HIFI_IMAGE_SEC_TYPE_ENUM_UINT8;

/*****************************************************************************
 实 体 名  : DRV_HIFI_IMAGE_SEC_LOAD_ENUM
 功能描述  : 镜像段加载属性
*****************************************************************************/
enum DRV_HIFI_IMAGE_SEC_LOAD_ENUM {
    DRV_HIFI_IMAGE_SEC_LOAD_STATIC = 0,      /* 单次加载, 即解复位前加载 */
    DRV_HIFI_IMAGE_SEC_LOAD_DYNAMIC,         /* 多次加载, 由其他机制加载 */
    DRV_HIFI_IMAGE_SEC_UNLOAD,               /* 不需要底软加载 */
    DRV_HIFI_IMAGE_SEC_LOAD_BUTT,
};
typedef unsigned char DRV_HIFI_IMAGE_SEC_LOAD_ENUM_UINT8;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/*****************************************************************************
 实 体 名  : DRV_HIFI_IMAGE_SEC_STRU
 功能描述  : 镜像段定义
*****************************************************************************/
struct drv_hifi_image_sec
{
    unsigned short                      sn;              /* 编号 */
    DRV_HIFI_IMAGE_SEC_TYPE_ENUM_UINT8  type;            /* 类型 */
    DRV_HIFI_IMAGE_SEC_LOAD_ENUM_UINT8  load_attib;      /* 加载属性 */
    unsigned int                       src_offset;      /* 在文件中的偏移, bytes */
    unsigned int                       des_addr;        /* 加载目的地址, bytes */
    unsigned int                       size;            /* 段长度, bytes */

};

/*****************************************************************************
 实 体 名  : drv_hifi_image_head
 功能描述  : 镜像文件头定义
*****************************************************************************/
struct drv_hifi_image_head
{
    char                                time_stamp[24]; /* 镜像编译时间 */
    unsigned int                       image_size;     /* 镜像文件大小, bytes */
    unsigned int                        sections_num;   /* 文件包含段数目 */
    struct drv_hifi_image_sec           sections[HIFI_SEC_MAX_NUM];    /* 段信息, 共sections_num个 */

};

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

/*****************************************************************************
 函 数 名  : drv_hifi_read_image
 功能描述  : 读取Hifi镜像
 输入参数  : void
 输出参数  : img_head hifi镜像头信息
             img_buf  hifi镜像信息
 返 回 值  : int
             成功返回 0，失败返回 -1
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月29日
    作    者   : 刘慈红 lKF71598
    修改内容   : 新生成函数

*****************************************************************************/
extern int drv_hifi_read_image(void *img_head, void **img_buf);

/*****************************************************************************
 函 数 名  : drv_hifi_load_sec
 功能描述  : Hifi按每段加载到内存
 输入参数  : img_head 镜像头指针
             img_buf 镜像读到内存的指针
             share_mem 存放非单次加载段信息的内存
 输出参数  : 无
 返 回 值  : int
             成功返回 0，失败返回 -1
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月30日
    作    者   : 刘慈红 lKF71598
    修改内容   : 新生成函数

*****************************************************************************/
extern int drv_hifi_load_sec(void *img_buf, unsigned int* share_mem);

/*****************************************************************************
 函 数 名  : image_verification
 功能描述  : hifi镜像校验桩函数
 输入参数  : void
 输出参数  : 无
 返 回 值  : int
             成功返回 0，失败返回 -1
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月29日
    作    者   : 刘慈红 lKF71598
    修改内容   : 新生成函数

*****************************************************************************/
extern int image_verification(void **img_buf);
extern void drv_hifi_power_up(void);
extern void drv_hifi_phy2virt(unsigned int *sec_addr);
extern void drv_hifi_fill_mb_info(unsigned long addr);
extern void drv_hifi_init_mem(unsigned int* share_addr_base);
extern int drv_hifi_check_img_head(struct drv_hifi_image_head *img_head, unsigned length);
extern int drv_hifi_check_sections(struct drv_hifi_image_head *img_head,
                                 struct drv_hifi_image_sec *img_sec);
extern int  execute_load_hifi(int safe_load, unsigned long share_mem);
extern int check_secure_mode(void);







#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hifi_loader.h */

