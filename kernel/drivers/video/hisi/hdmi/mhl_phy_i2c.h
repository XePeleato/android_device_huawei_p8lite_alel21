#ifndef _LINUX_MHL_PHY_I2C_H
#define _LINUX_MHL_PHY_I2C_H
#include <linux/gpio.h>

#define MHL_PHY_I2C_ADDRESS     0x30
#define MHL_PHY_I2C_NAME "hisik3_mhl_phy_i2c"
#define I2C_ID 0x00
#define MHL_PHY_GPIO_RESETN         (44)
#define MHL_PHY_GPIO_PAKED_MODE         (43)
#define MHL_PHY_GPIO_MODE_SEL           (42)
#define MHL_PHY_GPIO_PDB            (41)
#define MHL_PHY_GPIO_1080P_MODE         (40)

struct mhl_phy_platform_data
{
    int gpio_reset;
    int gpio_pdb;
    int gpio_mode_sel;
    int gpio_paked_mode;
    int gpio_1080p_mode;

};

#endif
