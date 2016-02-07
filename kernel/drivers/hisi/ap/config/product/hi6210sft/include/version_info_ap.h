/* Huawei Hisi, Copyright (c) 2010 hisi LTE Ltd. */ 
#ifndef __VERSION_INFO_AP_H__
#define __VERSION_INFO_AP_H__

/*Move the version info to  "projectname = hisi/ap/config/product" in order to separate the  version info from different branches, which share the same "modem code branch"*/
#if defined(CHIP_BB_HI6210)
	#ifdef BSP_VERSION
	#undef  BSP_VERSION
	#endif

	#define BSP_SET_VERSION                 "V800R100C52B126SP12"
	#define BSP_VERSION                     "V800R100C52B126SP12"
	#define DLOAD_ID                        "V8R2C52"
	#define PRODUCT_ID                      "V8R2C52"
	#define HARD_VERSION                    "V8R2C52"
	#define PRODUCT_VERSION                 "22.126.12.00.00"

#elif defined (BOARD_FPGA)
	#define BSP_SET_VERSION                 "V800R100C00B170"
	#define BSP_VERSION                     "V800R100C00B170"
	#define DLOAD_ID                        "H69DUSAVA"
	#define PRODUCT_ID                      "H69DUSAVA"
	#define HARD_VERSION                    "H69DUSAVA"
	#define PRODUCT_VERSION                 "21.100.00.00.00"

#else
	#error "no version define"
#endif

#endif /*__VERSION_INFO_AP_H__*/
