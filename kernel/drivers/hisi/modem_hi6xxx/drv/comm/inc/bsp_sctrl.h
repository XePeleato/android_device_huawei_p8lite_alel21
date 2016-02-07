#ifndef __BSP_SCTRL_H_
#define  __BSP_SCTRL_H_

#include "soc_baseaddr_interface.h"
#include "soc_modem_sctrl_interface.h"
#if defined(BOARD_FPGA)
#elif defined(CHIP_BB_HI6758)
#else
#include <soc_ao_sctrl_interface.h>
#endif


#endif
