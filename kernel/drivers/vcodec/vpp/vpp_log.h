/*
 **************************************************************************************
 *
 *       Filename:  vpp_log.h
 *    Description:  log debug switch header file
 *
 *        Version:  1.0
 *        Created:  2011-08-13
 *        Author:   j00140427
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */
#include<linux/kernel.h>

#ifndef __VPP_LOG_H_
#define __VPP_LOG_H_


#define DEBUG_LEVEL 1

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif

#ifndef INFO_LEVEL
#define INFO_LEVEL  1
#endif

#ifndef LOG_TAG
#define LOG_TAG ""
#endif

#if DEBUG_LEVEL
#define logd(fmt, ...) pr_debug("[" LOG_TAG "]" " [ DEBUG: %s, line - %d: ]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define logd(fmt, ...)
#endif

#if INFO_LEVEL
#define logi(fmt, ...) pr_info("[" LOG_TAG "]" " [ INFO: %s, line - %d: ]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define logi(fmt, ...)
#endif

#define logw(fmt, ...) pr_warn("[" LOG_TAG "]" " [ WARN: %s, line - %d: ]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define loge(fmt, ...) pr_err("[" LOG_TAG "]" " [ ERROR: %s, line - %d: ]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#endif

