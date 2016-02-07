/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : TafAppRabm.h
******************************************************************************/

#ifndef __TAF_APP_RABM_H__
#define __TAF_APP_RABM_H__


#include "TafTypeDef.h"


#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#pragma pack(4)

/*数传接口宏定义*/
#define TAF_DATA_CS_DOMAIN                   0    /*CS域*/
#define TAF_DATA_PS_DOMAIN                   1    /*PS域*/

#define TAF_RABM_STOP_DATA                   0    /*指示APP停止发送数据*/
#define TAF_RABM_START_DATA                  1    /*指示APP开始发送数据*/
#define TAF_DATA_STOP                        2    /*无法进行数传，故停止数传*/
#define TAF_DATA_START                       3    /*启动数传(未用)*/

/*指示APP停止发送数据的原因值*/
#define TAF_RABM_STOP_CAUSE_NORMAL                        0xff      /*停止数传*/

/*指示APP开始发送数据的原因值*/
#define TAF_RABM_START_CAUSE_REEST_FAILURE                0         /*因重建RAB失败开始发送数据*/
#define TAF_RABM_START_CAUSE_REEST_SUCCESS                1         /*因重建RAB成功开始发送数据*/
#define TAF_RABM_START_CAUSE_RRC_INDICATION               2         /*因RRC指示开始发送数据*/
#define TAF_RABM_START_CAUSE_2G_TO_3G_CUR_2G              3         /*2G模式切至3G模式，当前网络为2G(V200R001版本新增)*/
#define TAF_RABM_START_CAUSE_2G_TO_3G_CUR_3G              4         /*2G模式切至3G模式，当前网络为3G(V200R001版本新增)*/
#define TAR_RABM_START_CAUSE_3G_TO_2G_CUR_3G              5         /*3G模式切至2G模式，当前网络为3G(V200R001版本新增)*/
#define TAF_RABM_START_CAUSE_3G_TO_2G_RA_UPDATE_FINISH    6         /*3G向2G切换中的路由区更新流程结束(V200R001版本新增)*/

/*无法进行数传的原因值*/
#define TAF_DATA_CAUSE_REEST_FAILURE         0    /*用户面承载重建失败*/
#define TAF_DATA_CAUSE_REEST_SUCCESS         1    /*用户面承载重建成功*/
#define TAF_DATA_CAUSE_LOWLAYER_INDICATION   2    /*用户面底层原因*/
#define TAF_DATA_CAUSE_NO_TRANSMODE          3    /*此时该NSAPI未设置TRANSMODE(V200R001版本新增)*/
#define TAF_DATA_CAUSE_3G_TO_2G_RA_UPDATE    4    /*正在进行3G向2G切换中的路由区更新(V200R001版本新增)*/

/*其他宏定义:*/
#define   TAF_RABM_MAX_ENT_NUM               11





#if ((TAF_OS_VER == TAF_WIN32) || (TAF_OS_VER == TAF_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of TafAppRabm.h */
