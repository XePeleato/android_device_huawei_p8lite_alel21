#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#include "mhl_phy_i2c.h"

#define LOG_TAG "mhl-phy-i2c"
#include "k3_hdmi_log.h"
#include "k3_hdmi.h"

static int phy_read_reg32(u32 reg, u32* val, u32 len);
static int phy_write_reg32(u32 reg, u32 val);
static int hisik3_mhl_phy_remove(struct i2c_client *client);
static int hisik3_mhl_phy_probe(struct i2c_client *client, const struct i2c_device_id *idp);
void mhl_phy_reset(void);
void config_mhl_phy_register(void);

static const struct i2c_device_id hisik3_mhl_phy_id[] = {
    { "hisik3_mhl_phy_i2c", 0 },
    { }
};
static const struct of_device_id mhl_dt_ids[] = {
    { .compatible = "hisilicon,mhl_phy", },
    { }
};

struct i2c_client *mhl_phy_client = NULL;

MODULE_DEVICE_TABLE(of, mhl_dt_ids);

static struct i2c_driver mhl_phy_i2c_driver = {
    .driver =   {
        .owner = THIS_MODULE,
        .name = "hisik3_mhl_phy_i2c",
        .of_match_table = of_match_ptr(mhl_dt_ids),
    },
    .id_table = hisik3_mhl_phy_id,
    .probe = hisik3_mhl_phy_probe,
    .remove = hisik3_mhl_phy_remove,
};

static int phy_read_reg32(u32 reg, u32* val, u32 len)
{
    struct i2c_msg msg[2];
    int ret = -1;

    IN_FUNCTION;

    msg[0].addr = MHL_PHY_I2C_ADDRESS;
    msg[0].len = 1;
    msg[0].buf = (u8*)&reg;
    msg[0].flags = 0x2;

    msg[1].addr = MHL_PHY_I2C_ADDRESS;
    msg[1].len = len;
    msg[1].buf = (u8*)val;
    msg[1].flags = 0;
    msg[1].flags = I2C_M_RD;

    if (mhl_phy_client->adapter == NULL) {
        loge("touch adapter error\n");
        OUT_FUNCTION;
        return ret;
    }

    ret = i2c_transfer(mhl_phy_client->adapter, msg, 2);
    if (ret < 0) {
        loge("read phy error\n");
        OUT_FUNCTION;
        return ret;
    }

    logd("phy read reg=0x%02x, val:0x%02x\n", reg, val[0]);
    msleep(10);

    OUT_FUNCTION;
    return 0;
}

static int phy_write_reg32(u32 reg, u32 val)
{
    struct i2c_msg msg[1];
    int ret = -1;
    u8  data[2];

    IN_FUNCTION;

    data[0] = reg;
    data[1] = val;

    msg[0].addr = MHL_PHY_I2C_ADDRESS;
    msg[0].len = 2;
    msg[0].buf = data;
    msg[0].flags = 0;

    if (mhl_phy_client->adapter == NULL) {
        loge("touch adapter error\n");
        OUT_FUNCTION;
        return ret;
    }

    ret = i2c_transfer(mhl_phy_client->adapter, msg, 1);
    if(ret < 0){
        loge("write phy error\n");
        OUT_FUNCTION;
        return ret;
    }

    logd("phy write reg=0x%x,val=0x%x\n", reg, val);
    msleep(10);

    OUT_FUNCTION;
    return 0;
}

static int hisik3_mhl_phy_probe(struct i2c_client *client,
                             const struct i2c_device_id *idp)
{
 #if !HDMI_CHIP_VER
    IN_FUNCTION;

    mhl_phy_client = client;
    mhl_phy_reset();

    msleep(200);
    config_mhl_phy_register();

    OUT_FUNCTION;
#endif
    return 0;
}

static int hisik3_mhl_phy_remove(struct i2c_client *client)
{
    return 0;
}

void config_mhl_phy_register(void)
{
#ifdef MHL_SUPPORT
    u32 val = 0;

    IN_FUNCTION;

    phy_write_reg32(0x00,0x30);
    phy_read_reg32(0x00,&val,1);

    phy_write_reg32(0x01,0x6c);
    phy_read_reg32(0x01,&val,1);


    phy_write_reg32(0x02,0x4d);
    //phy_write_reg32(0x02, 0x7d);
    phy_read_reg32(0x02,&val,1);


    phy_write_reg32(0x03,0x94);
    //phy_write_reg32(0x03, 0x97);
    phy_read_reg32(0x03,&val,1);


    phy_write_reg32(0x04,0x40);
    phy_read_reg32(0x04,&val,1);


    phy_write_reg32(0x05,0x20);
    phy_read_reg32(0x05,&val,1);


    phy_write_reg32(0x06,0x30);
    phy_read_reg32(0x06,&val,1);


    //phy_write_reg32(0x07,0x30);
    phy_write_reg32(0x07,0xF8);
    //phy_write_reg32(0x07,0x80);//test by zhang
    phy_read_reg32(0x07,&val,1);


    phy_write_reg32(0x08,0x64);
    phy_read_reg32(0x08,&val,1);


    phy_write_reg32(0x09,0x8b);
    phy_read_reg32(0x09,&val,1);

    OUT_FUNCTION;
#endif
}

void gobal_phy_low_register(void)
{



}
EXPORT_SYMBOL(gobal_phy_low_register);

void gobal_phy_hi_register(void)
{
#if !HDMI_CHIP_VER
    u32 val = 0;
    IN_FUNCTION;

    phy_write_reg32(0x06,0x36);
    phy_read_reg32(0x06,&val,1);

    IN_FUNCTION;
#endif
}
EXPORT_SYMBOL(gobal_phy_hi_register);


void mhl_phy_reset(void)
{

    int ret = 0;
#if 0
    struct device_node *node = mhl_phy_client->dev.of_node;

    struct mhl_phy_platform_data *pdata=NULL;

    pdata = devm_kzalloc(&mhl_phy_client->dev, sizeof(struct mhl_phy_platform_data),
                         GFP_KERNEL);
    of_property_read_u32_array(node,"mhl_phy,gpio-reset", &pdata->gpio_reset, 1);
    of_property_read_u32_array(node,"mhl_phy,gpio-pakedmode", &pdata->gpio_paked_mode, 1);
    of_property_read_u32_array(node,"mhl_phy,gpio-modesel", &pdata->gpio_mode_sel, 1);
    of_property_read_u32_array(node,"mhl_phy,gpio-pdb", &pdata->gpio_pdb, 1);
    of_property_read_u32_array(node,"mhl_phy,gpio-1080pmode", &pdata->gpio_1080p_mode, 1);

    ret=gpio_request(pdata->gpio_reset, "mhl_phy_reset");
    if (ret < 0)
        printk("l00221410 gpio request err!\n");
    else
        printk("l00221410 gpio request right!\n");
    gpio_request(pdata->gpio_paked_mode, "mhl_phy_pakedmode");
    gpio_request(pdata->gpio_mode_sel, "mhl_mode_sel");
    gpio_request(pdata->gpio_pdb, "mhl_phy_pdb");
    gpio_request(pdata->gpio_1080p_mode, "mhl_phy_1080p_mode");
    ret=gpio_direction_output(pdata->gpio_reset, 0);

    msleep(100);
    gpio_direction_output(pdata->gpio_pdb, 1);
    udelay(1);
    ret=gpio_direction_output(pdata->gpio_reset, 1);
    if (ret < 0)
        printk("l00221410 gpio output err!\n");

    gpio_direction_output(pdata->gpio_paked_mode, 0);
    gpio_direction_output(pdata->gpio_mode_sel, 0);
    gpio_direction_output(pdata->gpio_1080p_mode, 0);
#else

    ret = gpio_request(48, "mhl_phy_reset");
    if (ret < 0)
        loge("gpio request mhl_phy_reset error!\n");
    ret = gpio_request(47, "mhl_phy_pakedmode");
    if (ret < 0)
        loge("gpio request mhl_phy_pakedmode error!\n");
    ret = gpio_request(46, "mhl_mode_sel");
    if (ret < 0)
        loge("gpio request mhl_mode_sel error!\n");
    ret = gpio_request(45, "mhl_phy_pdb");
    if (ret < 0)
        loge("gpio request mhl_phy_pdb error!\n");
    ret = gpio_request(44, "mhl_phy_1080p_mode");
    if (ret < 0)
        loge("gpio request mhl_phy_1080p_mode error!\n");
    //gpio_request(45, "mhl_logic_switch");

    gpio_direction_output(48, 0);
    msleep(100);
    gpio_direction_output(45, 1);
    udelay(1);
    gpio_direction_output(48, 1);
    gpio_direction_output(47, 0);
    gpio_direction_output(46, 0);
    gpio_direction_output(44, 0);
    //gpio_direction_output(45, 0);//1---74m,0---27m

#endif

}
static int __init mhl_phy_init(void)
{
    int ret;
    ret = i2c_add_driver(&mhl_phy_i2c_driver);

    if(0 != ret){
        loge("i2c add driver error\n");
        goto err2;
    }
    return 0;
err2:
    i2c_del_driver(&mhl_phy_i2c_driver);

    return -EINVAL;
}

static void __exit mhl_phy_exit(void)
{
    i2c_del_driver(&mhl_phy_i2c_driver);
}

MODULE_LICENSE("GPL");
module_init(mhl_phy_init);
module_exit(mhl_phy_exit);
