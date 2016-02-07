
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#include "drv_global.h"
#endif

typedef enum{
    BOARDTYPE_BBIT = 0,
    BOARDTYPE_SFT = 1,
    BOARDTYPE_ASIC = 2,
    BOARDTYPE_MAX
}boardtype;

enum{
    CLK_32K_TYPE_TCXO = 0,
    CLK_32K_TYPE_DCXO
};

#define         BOARDID_UDP     (3)
#define         BOARDID_SFT     (23)

#define         BOARDID_SFT_MALI400     (1000)
#define         BOARDID_SFT_MALI450     (1001)

#define         IS_HI6210      (1)
#define         FPGA_B03X     (0x3)

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
/*性能优化针对此函数实现在MODEM侧使用静态内嵌且不直接读取AXI*/
extern boardtype g_ulBoardType;
static INLINE_ALWAYS boardtype get_type_of_board(void)
{
	return g_ulBoardType;
}
void set_type_of_board(void);
#else
boardtype get_type_of_board(void);
#endif

unsigned int get_arm_midr(void);
int is_hi6210(void);
int is_hi6210_before_io_init(void);
unsigned int get_id_of_board(void);
unsigned int get_product_id(void);
unsigned int get_b0xx_of_sft(void);
unsigned int get_32k_clk_type(void);
int is_udp(void);
