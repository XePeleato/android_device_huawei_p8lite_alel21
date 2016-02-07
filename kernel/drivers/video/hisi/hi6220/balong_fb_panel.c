/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */
#include <linux/clk-private.h>
#include <linux/clk.h>
#include <linux/pwm.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>

#include "balong_fb_panel.h"
#include "balong_fb.h"

u32 three_lane_flag = 0;
u32 colortemp_adjust_flag = 0;

void set_value(u32* addr, u32 val, u8 bw, u8 bs)
{
    u32 mask = (1 << bw) - 1;
    u32 tmp = *addr;
    tmp &= ~(mask << bs);
    *addr = tmp | ((val & mask) << bs);
}

void set_reg(u8* addr, u32 val, u8 bw, u8 bs)
{
    u32 mask = (1 << bw) - 1;
    u32 tmp = inp32(addr);
    tmp &= ~(mask << bs);
    outp32(addr, tmp | ((val & mask) << bs));
}

int panel_next_on(struct platform_device *pdev)
{
    int ret = 0;
    struct balong_fb_panel_data *pdata = NULL;
    struct balong_fb_panel_data *next_pdata = NULL;
    struct platform_device *next_pdev = NULL;

    BUG_ON(pdev == NULL);

    pdata = (struct balong_fb_panel_data *)pdev->dev.platform_data;
    if (pdata) {
        next_pdev = pdata->next;
        if (next_pdev) {
            next_pdata = (struct balong_fb_panel_data *)next_pdev->dev.platform_data;
            if(next_pdata){
                if((next_pdata->on)){
                    ret = next_pdata->on(next_pdev);
                }
            }
        }
    }

    return ret;
}

int panel_next_off(struct platform_device *pdev)
{
    int ret = 0;
    struct balong_fb_panel_data *pdata = NULL;
    struct balong_fb_panel_data *next_pdata = NULL;
    struct platform_device *next_pdev = NULL;

    BUG_ON(pdev == NULL);

    pdata = (struct balong_fb_panel_data *)pdev->dev.platform_data;
    if (pdata) {
        next_pdev = pdata->next;
        if (next_pdev) {
            next_pdata = (struct balong_fb_panel_data *)next_pdev->dev.platform_data;
            if (next_pdata){
                if(next_pdata->off){
                    ret = next_pdata->off(next_pdev);
                }
            }
        }
    }

    return ret;
}

int panel_next_remove(struct platform_device *pdev)
{
    int ret = 0;
    struct balong_fb_panel_data *pdata = NULL;
    struct balong_fb_panel_data *next_pdata = NULL;
    struct platform_device *next_pdev = NULL;

    BUG_ON(pdev == NULL);

    pdata = (struct balong_fb_panel_data *)pdev->dev.platform_data;
    if (pdata) {
        next_pdev = pdata->next;
        if (next_pdev) {
            next_pdata = (struct balong_fb_panel_data *)next_pdev->dev.platform_data;
            if (next_pdata){
                if(next_pdata->remove){
                    ret = next_pdata->remove(next_pdev);
                }
            }
        }
    }

    return ret;
}

int panel_next_set_backlight(struct platform_device *pdev)
{
    int ret = 0;
    struct balong_fb_panel_data *pdata = NULL;
    struct balong_fb_panel_data *next_pdata = NULL;
    struct platform_device *next_pdev = NULL;

    BUG_ON(pdev == NULL);

    pdata = (struct balong_fb_panel_data *)pdev->dev.platform_data;
    if (pdata) {
        next_pdev = pdata->next;
        if (next_pdev) {
            next_pdata = (struct balong_fb_panel_data *)next_pdev->dev.platform_data;
            if (next_pdata){
                if(next_pdata->set_backlight){
                    ret = next_pdata->set_backlight(next_pdev);
                }
            }
        }
    }

    return ret;
}

int panel_next_set_timing(struct platform_device *pdev)
{
    int ret = 0;
    struct balong_fb_panel_data *pdata = NULL;
    struct balong_fb_panel_data *next_pdata = NULL;
    struct platform_device *next_pdev = NULL;

    BUG_ON(pdev == NULL);

    pdata = (struct balong_fb_panel_data *)pdev->dev.platform_data;
    if (pdata) {
        next_pdev = pdata->next;
        if (next_pdev) {
            next_pdata = (struct balong_fb_panel_data *)next_pdev->dev.platform_data;
            if (next_pdata){
                if(next_pdata->set_timing){
                    ret = next_pdata->set_timing(next_pdev);
                }
            }
        }
    }

    return ret;
}

int panel_next_set_frc(struct platform_device *pdev, int target_fps)
{
    int ret = 0;
    struct balong_fb_panel_data *pdata = NULL;
    struct balong_fb_panel_data *next_pdata = NULL;
    struct platform_device *next_pdev = NULL;

    BUG_ON(pdev == NULL);

    pdata = (struct balong_fb_panel_data *)pdev->dev.platform_data;
    if (pdata) {
        next_pdev = pdata->next;
        if (next_pdev) {
            next_pdata = (struct balong_fb_panel_data *)next_pdev->dev.platform_data;
            if (next_pdata){
                if(next_pdata->set_frc){
                    ret = next_pdata->set_frc(next_pdev, target_fps);
                }
            }
        }
    }

    return ret;
}

int panel_next_check_esd(struct platform_device *pdev)
{
    int ret = 0;
    struct balong_fb_panel_data *pdata = NULL;
    struct balong_fb_panel_data *next_pdata = NULL;
    struct platform_device *next_pdev = NULL;

    BUG_ON(pdev == NULL);

    pdata = (struct balong_fb_panel_data *)pdev->dev.platform_data;
    if (pdata) {
        next_pdev = pdata->next;
        if (next_pdev) {
            next_pdata = (struct balong_fb_panel_data *)next_pdev->dev.platform_data;
            if (next_pdata){
                if(next_pdata->check_esd){
                    ret = next_pdata->check_esd(next_pdev);
                }
            }
        }
    }

    return ret;
}
#if LCD_CHECK_MIPI_SUPPORT
int panel_next_check_mipi_tr(struct platform_device* pdev)
{
    int ret = 0;
    struct balong_fb_panel_data* pdata = NULL;
    struct balong_fb_panel_data* next_pdata = NULL;
    struct platform_device* next_pdev = NULL;
    BUG_ON(pdev == NULL);
    pdata = (struct balong_fb_panel_data*)pdev->dev.platform_data;

    if (pdata)
    {
        next_pdev = pdata->next;
        if (next_pdev)
        {
            next_pdata = (struct balong_fb_panel_data*)next_pdev->dev.platform_data;
            if ((next_pdata) && (next_pdata->check_mipi_tr))
            { ret = next_pdata->check_mipi_tr(next_pdev); }
        }
    }
    return ret;
}

int panel_next_set_inversion_type(struct platform_device* pdev, unsigned int inversion_mode)
{
    int ret = 0;
    struct balong_fb_panel_data* pdata = NULL;
    struct balong_fb_panel_data* next_pdata = NULL;
    struct platform_device* next_pdev = NULL;

    BUG_ON(pdev == NULL);
    pdata = (struct balong_fb_panel_data*)pdev->dev.platform_data;
    if (pdata)
    {
        next_pdev = pdata->next;
        if (next_pdev)
        {
            next_pdata = (struct balong_fb_panel_data*)next_pdev->dev.platform_data;
            if ((next_pdata) && (next_pdata->set_inversion_type))
            { ret = next_pdata->set_inversion_type(next_pdev, inversion_mode); }
        }
    }

    return ret;
}
#endif

#include "mipi_reg.h"
extern struct semaphore balong_fb_blank_sem;
int mipi_dsi_wait_and_read(struct balong_fb_data_type *balongfd, u32 *val);

/******************************************************************************
** Read lcd reg interface
*/
int mipi_dsi_read_reg(struct balong_fb_data_type *balongfd, u32 reg, u32 *val, u32 rd_type)
{
    u32 tmp = 0;
    int ret = 0;

    balongfb_logi("check esd enter.\n");

    down(&balongfd->mipi_read_sem);
    down(&balong_fb_blank_sem);
    if (!balongfd->ade_ldi_on)
    {
        /*ldi off, discard this read operation*/
        balongfb_loge("ldi off, before send read cmd, discard this read operation.\n");
        up(&balong_fb_blank_sem);
        up(&balongfd->mipi_read_sem);
        return -1;
    }

    /*read status register*/
    tmp = inp32(balongfd->dsi_base + MIPIDSI_CMD_PKT_STATUS_ADDR);
    if ((tmp & 0x00000001) != 1)
    {
        /*dsi fifo is not empty*/
        balongfb_loge("dsi fifo is not empty!\n");
        ret = -1;
        up(&balong_fb_blank_sem);
        up(&balongfd->mipi_read_sem);
        return ret;
    }
    /*send read command*/
    set_MIPIDSI_GEN_HDR(NULL,((reg << 8) | rd_type));
    up(&balong_fb_blank_sem);

    /*wait and read data*/
    ret = mipi_dsi_wait_and_read(balongfd,val);
    if (ret != 0)
    {
        /*read error*/
        balongfb_loge("read error, discard this read operation.\n");
        ret = -1;
    }
    up(&balongfd->mipi_read_sem);
    return ret;
}

int mipi_dsi_wait_and_read(struct balong_fb_data_type *balongfd, u32 *val)
{
    int err = 0;
    int is_timeout = 1;
    unsigned long dw_jiffies = 0;
    MIPIDSI_CMD_PKT_STATUS_UNION    mipidsi_cmd_pkt_status;

    /*wait dsi read data*/
    dw_jiffies = jiffies + HZ;
    do
    {
        if (!balongfd->ade_ldi_on)
        {
            /*ldi off, discard this read operation*/
            balongfb_loge("ldi off, waiting data can read.\n");
            return -1;
        }
        mipidsi_cmd_pkt_status.ul32 = get_MIPIDSI_CMD_PKT_STATUS();
        if (0x1 != mipidsi_cmd_pkt_status.bits.gen_pld_r_empty)
        {
            /*data is ready, we can read data from PLD*/
            is_timeout = 0;
            break;
        }
    } while (time_after(dw_jiffies, jiffies));

    if (is_timeout == 1)
    {
        /*read timeout*/
        balongfb_loge("dsi read timeout.\n");
        err = -1;
        return err;
    }

    //down(&balong_fb_blank_sem);
    if (balongfd->ade_ldi_on)
    {
        /*read data*/
        *val = get_MIPIDSI_GEN_PLD_DATA();
    }
    else
    {
        /*panel off, discard this read operation*/
        balongfb_loge("panel off, when reading data.\n");
        err = -1;
    }
   // up(&balong_fb_blank_sem);
    return err;
}
struct platform_device *balong_fb_device_alloc(struct balong_fb_panel_data *pdata,
        u32 type, u32 index, u32 *graphic_ch)
{
    struct platform_device *this_dev = NULL;
    char dev_name[16] = {0};

    BUG_ON(pdata == NULL);

    switch (type) {
        case PANEL_LDI:
            snprintf(dev_name, sizeof(dev_name), DEV_NAME_LDI);
            *graphic_ch = OVERLAY_PIPE_ADE_DISP;
            break;
        case PANEL_MIPI_VIDEO:
        case PANEL_MIPI_CMD:
            snprintf(dev_name, sizeof(dev_name), DEV_NAME_MIPIDSI);
            *graphic_ch = OVERLAY_PIPE_ADE_DISP;
            break;
        case PANEL_HDMI:
        case PANEL_DP:
            return NULL;
        default:
            balongfb_loge("invalid panel type = %d!\n", type);
            return NULL;
    }

    pdata->next = NULL;

    this_dev = platform_device_alloc(dev_name, index + 1);
    if (this_dev) {
        if (platform_device_add_data(this_dev, pdata, sizeof(struct balong_fb_panel_data))) {
            balongfb_loge("failed to platform_device_add_data!\n");
            platform_device_put(this_dev);
            return NULL;
        }
    }

    return this_dev;
}

void  balong_fb_device_free(struct platform_device *pdev)
{
    BUG_ON(pdev == NULL);
    platform_device_put(pdev);
}


/**************************regulator********************************************/

int vcc_cmds_tx(struct platform_device *pdev, struct vcc_desc *cmds, int cnt)
{
    int ret = 0;
    struct vcc_desc *cm = NULL;
    int i = 0;

    cm = cmds;

    for (i = 0; i < cnt; i++) {
        if ((cm == NULL) || (cm->id == NULL)) {
            balongfb_loge("cm or cm->id is null! index=%d\n", i);
            ret = -1;
            goto error;
        }

        if (cm->dtype == DTYPE_VCC_GET) {
            BUG_ON(pdev == NULL);

            *(cm->regulator) = regulator_get(&pdev->dev, cm->id);
            if (IS_ERR(*(cm->regulator))) {
                balongfb_loge("failed to get %s regulator!\n", cm->id);
                ret = -1;
                goto error;
            }
        } else if (cm->dtype == DTYPE_VCC_PUT) {
            if (!IS_ERR(*(cm->regulator))) {
                regulator_put(*(cm->regulator));
            }
        } else if (cm->dtype == DTYPE_VCC_ENABLE) {
            if (!IS_ERR(*(cm->regulator))) {
                if (regulator_enable(*(cm->regulator)) != 0) {
                    balongfb_loge("failed to enable %s regulator!\n", cm->id);
                    ret = -1;
                    goto error;
                }
            }
        } else if (cm->dtype == DTYPE_VCC_DISABLE) {
            if (!IS_ERR(*(cm->regulator))) {
                if (regulator_disable(*(cm->regulator)) != 0) {
                    balongfb_loge("failed to disable %s regulator!\n", cm->id);
                    ret = -1;
                    goto error;
                }
            }
        } else if (cm->dtype == DTYPE_VCC_SET_VOLTAGE) {
            if (!IS_ERR(*(cm->regulator))) {
                if (regulator_set_voltage(*(cm->regulator), cm->min_uV, cm->max_uV) != 0) {
                    balongfb_loge("failed to set %s regulator voltage!\n", cm->id);
                    ret = -1;
                    goto error;
                }
            }
        } else if (cm->dtype == DTYPE_VCC_SET_CURRENT) {
            if (!IS_ERR(*(cm->regulator))) {
                if (regulator_set_current_limit(*(cm->regulator), cm->min_uA, cm->max_uA) != 0) {
                    balongfb_loge("failed to set %s regulator current!\n", cm->id);
                    ret = -1;
                    goto error;
                }
            }
        } else {
            balongfb_loge("dtype=%x NOT supported\n", cm->dtype);
            ret = -1;
            goto error;
        }

        if (cm->wait) {
            if (cm->waittype == WAIT_TYPE_US)
                udelay(cm->wait);
            else if (cm->waittype == WAIT_TYPE_MS)
                mdelay(cm->wait);
            else
                mdelay(cm->wait * 1000);
        }

        cm++;
    }

    return 0;

error:
    return ret;
}

/**************************GPIO ********************************************/
int gpio_cmds_tx(struct platform_device *pdev, struct gpio_desc *cmds, int cnt)
{
    int ret = 0;
    struct gpio_desc *cm = NULL;
    int i = 0;
    struct device      *dev;
    struct device_node *np;
    enum of_gpio_flags gpio_flags;

    if (pdev) {
        dev = &pdev->dev;
        np  = dev->of_node;
    }

    cm = cmds;
    for (i = 0; i < cnt; i++) {
        if ((cm == NULL) || (cm->label == NULL)) {
            balongfb_loge("cm or cm->label is null! index=%d\n", i);
            ret = -1;
            goto error;
        }
        if ((cm->dtype == DTYPE_GPIO_GET) && (pdev)) {
            *(cm->gpio) = of_get_gpio_by_prop(np, cm->label, 0, 0, &gpio_flags);
            if (!gpio_is_valid(*(cm->gpio))) {
                balongfb_loge("gpio invalid, dtype=%d, lable=%s, gpio=%d!\n",
                        cm->dtype, cm->label, *(cm->gpio));
            }
        } else if (cm->dtype == DTYPE_GPIO_INPUT) {
            if (gpio_direction_input(*(cm->gpio)) != 0) {
                balongfb_loge("failed to gpio_direction_input, lable=%s, gpio=%d!\n",
                        cm->label, *(cm->gpio));
                ret = -1;
                goto error;
            }
        } else if (cm->dtype == DTYPE_GPIO_OUTPUT) {
            if (gpio_direction_output(*(cm->gpio), cm->value) != 0) {
                balongfb_loge("failed to gpio_direction_output, label%s, gpio=%d!\n",
                        cm->label, *(cm->gpio));
                ret = -1;
                goto error;
            }
        } else if (cm->dtype == DTYPE_GPIO_REQUEST) {
            if (gpio_request(*(cm->gpio), cm->label) != 0) {
                balongfb_loge("failed to gpio_request, lable=%s, gpio=%d!\n",
                        cm->label, *(cm->gpio));
                ret = -1;
                goto error;
            }
        } else if (cm->dtype == DTYPE_GPIO_FREE) {
            gpio_free(*(cm->gpio));
        } else {
            balongfb_loge("dtype=%x NOT supported\n", cm->dtype);
            ret = -1;
            goto error;
        }

        if (cm->wait) {
            if (cm->waittype == WAIT_TYPE_US)
                udelay(cm->wait);
            else if (cm->waittype == WAIT_TYPE_MS)
                mdelay(cm->wait);
            else
                mdelay(cm->wait * 1000);
        }

        cm++;
    }

    return 0;

error:
    return ret;
}


int get_resource_from_dts(struct platform_device *pdev, struct balong_panel_info *pinfo)
{
    struct device      *dev;
    struct device_node *np;
    int ret = 0;

    BUG_ON(pdev == NULL);
    BUG_ON(pinfo == NULL);

    dev = &pdev->dev;
    np  = dev->of_node;

    ret = of_property_read_u32(np, "ade_pixel_clk_rate", &pinfo->clk_rate);
    if (ret) {
        balongfb_loge("get ade_pixel_clk_rate is fail \n");
        pinfo->clk_rate = 10000000;
    }

    ret = of_property_read_u32(np, "dsi_bit_clk_rate", &pinfo->mipi.dphy_freq);
    if (ret) {
        balongfb_loge("get dsi_bit_clk is fail \n");
        pinfo->mipi.dphy_freq = 120;
    }

	ret = of_property_read_u32(np, "3lane_flag", &three_lane_flag);
    if (ret) {
        balongfb_loge("get three_lane_flag is fail \n");
        three_lane_flag = 0;
    }

    ret = of_property_read_u32(np, "ct_adjust_flag", &colortemp_adjust_flag);
    if (ret) {
        balongfb_loge("Failed to get the flag of color temperature! \n");
        colortemp_adjust_flag = 0;
    }

    balongfb_logi("pinfo->dsi_bit_clk_rate = %d \n", pinfo->mipi.dphy_freq);

    return 0;
}

/******************pin ctrl*****************/
int pinctrl_cmds_tx(struct platform_device *pdev, struct pinctrl_cmd_desc *cmds, int cnt)
{
    int ret = 0;

    int i = 0;
    struct pinctrl_cmd_desc *cm = NULL;

    cm = cmds;

    for(i = 0; i < cnt; i++) {
        if(cm == NULL) {
            balongfb_loge("command %d is null!\n", i);
            continue;
        }

        if(cm->dtype == DTYPE_PINCTRL_GET) {
            cm->pctrl_data->p = pinctrl_get(&pdev->dev);
        } else if(cm->dtype == DTYPE_PINCTRL_STATE_GET) {
            /* check pinctrl pointer*/
            // TO BE DONE
            if(cm ->mode == DTYPE_PINCTRL_STATE_DEFAULT) {
                cm->pctrl_data->pinctrl_def = pinctrl_lookup_state(cm->pctrl_data->p,PINCTRL_STATE_DEFAULT);
            } else if(cm ->mode == DTYPE_PINCTRL_STATE_IDLE) {
                cm->pctrl_data->pinctrl_idle = pinctrl_lookup_state(cm->pctrl_data->p,PINCTRL_STATE_IDLE);
            } else {
                ret = -1;
                balongfb_loge("unknown pinctrl type to get!\n");
                goto err;
            }
        } else if(cm->dtype == DTYPE_PINCTRL_SET) {
            /* check pinctrl pointer*/
            // TO BE DONE
            if(cm ->mode == DTYPE_PINCTRL_STATE_DEFAULT) {
                /* check pinctrl_def pointer*/
                // TO BE DONE
                ret = pinctrl_select_state(cm->pctrl_data->p, cm->pctrl_data->pinctrl_def);
                if(ret) {
                    balongfb_loge("could not set this pin to default state!\n");
                    ret = -1;
                    goto err;
                }

            } else if(cm ->mode == DTYPE_PINCTRL_STATE_IDLE) {
                /* check pinctrl_def pointer*/
                // TO BE DONE
                ret = pinctrl_select_state(cm->pctrl_data->p, cm->pctrl_data->pinctrl_idle);
                if(ret) {
                    balongfb_loge("could not set this pin to idle state!\n");
                    ret = -1;
                    goto err;
                }
            } else {
                ret = -1;
                balongfb_loge("unknown pinctrl type to set!\n");
                goto err;
            }
        } else if(cm->dtype == DTYPE_PINCTRL_PUT) {
            pinctrl_put(cm->pctrl_data->p);
        } else {
            balongfb_loge("not supported command type!\n");
            ret = -1;
            goto err;
        }

        cm++;

    }

    return ret;

err:
    return ret;
}

int PWM_CLK_GET(struct balong_panel_info *pinfo)
{
    BUG_ON(pinfo == NULL);

    pinfo->pwm_clk = clk_get(NULL, CLK_PWM0_NAME);
    if (IS_ERR(pinfo->pwm_clk)) {
        balongfb_loge("failed to get pwm0 clk!\n");
        return PTR_ERR(pinfo->pwm_clk);
    }

    if (clk_set_rate(pinfo->pwm_clk, DEFAULT_PWM_CLK_RATE) != 0) {
        balongfb_loge("failed to set pwm clk rate!\n");
    }

    return 0;
}

void PWM_CLK_PUT(struct balong_panel_info *pinfo)
{
    BUG_ON(pinfo == NULL);

    if (!IS_ERR(pinfo->pwm_clk)) {
        clk_put(pinfo->pwm_clk);
    }
}

int PWM_RESOUTCE(struct platform_device *pdev, struct balong_panel_info *pinfo,
        struct resource *res)
{
    BUG_ON(pdev == NULL);
    BUG_ON(pinfo == NULL);

    res = platform_get_resource_byname(pdev, IORESOURCE_MEM,  REG_BASE_PWM0_NAME);
    if (!res) {
        balongfb_loge("failed to get pwm0 resource!\n");
        return -ENXIO;
    }
    pinfo->pwm_base = (u8*)ioremap((res)->start, REG_PWM0_IOSIZE);

    /* we never use PWM gpio */

    return 0;
}

void PWM_GPIO_REQUEST(struct balong_panel_info *pinfo)
{
    BUG_ON(pinfo == NULL);

    if (gpio_request(pinfo->gpio_pwm0, GPIO_LCD_PWM0_NAME) != 0) {
        balongfb_loge("failed to request gpio pwm0!\n");
    }

    if (gpio_request(pinfo->gpio_pwm1, GPIO_LCD_PWM1_NAME) != 0) {
        balongfb_loge("failed to request gpio pwm1!\n");
    }
}

void PWM_GPIO_FREE(struct balong_panel_info *pinfo)
{
    BUG_ON(pinfo == NULL);

    if (gpio_is_valid(pinfo->gpio_pwm0)) {
        gpio_free(pinfo->gpio_pwm0);
    }

    if (gpio_is_valid(pinfo->gpio_pwm1)) {
        gpio_free(pinfo->gpio_pwm1);
    }
}
/* peri_pll = 1440, dsp_pll = 1200 */
int balong_fb_set_clock_rate(struct balong_fb_data_type *balongfd, struct clk *clk, u32 clk_rate)
{
    struct clk *parent_clk = NULL;
    u32   dsp_ratio  = 0;
    u32   peri_ratio = 0;
    u32   dsp_div  = 0;
    u32   peri_div = 0;

    if ((balongfd == NULL) || (clk == NULL)) {
        balongfb_loge(" balongfd is null, or clk is null \n");
        return -EINVAL;
    }

    if (balongfd->sys_pll == NULL || balongfd->media_pll == NULL) {

        if (clk_set_rate(clk, clk_rate) != 0) {
            balongfb_loge("set rate fail \n");
            return -EINVAL;
        }

        return 0;
    }

    dsp_div  = balongfd->sys_pll->rate / clk_rate;
    peri_div = balongfd->media_pll->rate / clk_rate;

    if ((dsp_div == 0) && (peri_div == 0)) {
        balongfb_loge(" invalid clk_rate = %d\n", clk_rate);
        return -EINVAL;
    } else if (peri_div == 0) {
        parent_clk = balongfd->sys_pll;
        balongfb_logi_display_debugfs("balong_fb_set_clock_rate: patern clk is sys_pll \n");
    } else if (dsp_div == 0) {
        parent_clk = balongfd->media_pll;
        balongfb_logi_display_debugfs("balong_fb_set_clock_rate: patern clk is media pll \n");
    } else {
        dsp_ratio  = ((balongfd->sys_pll->rate % clk_rate) * 1000) / dsp_div;
        peri_ratio = ((balongfd->media_pll->rate % clk_rate) * 1000) / peri_div;

        if (dsp_ratio <= peri_ratio) {
            parent_clk = balongfd->sys_pll;
        } else {
            parent_clk = balongfd->media_pll;
        }
    }

    /* the parent of pixel clock is pixel gate clock
     * the parent of pixel gate clock is DSP PLL and PERI PLL
     */
    if (clk->parent != NULL) {
        clk_set_parent(clk->parent, parent_clk);
    } else {
        clk_set_parent(clk, parent_clk);
    }

    if (clk_set_rate(clk, clk_rate) != 0) {
        balongfb_loge("set rate fail \n");
        return -EINVAL;
    }

    return 0;
}

#if ADE_DEBUG_LOG_ENABLE
void balongfb_logi_vsync_debugfs (const char* fmt, ...)
{
    va_list args;

    if (g_fb_log_printk_flag.bits.bit_vsync) {
        va_start(args, fmt);
        vprintk(fmt, args);
        va_end(args);
    }
}

void balongfb_logi_display_debugfs(const char* fmt, ...)
{
    va_list args;

    if (g_fb_log_printk_flag.bits.bit_display >= ADE_INFO_PRINT) {
        va_start(args, fmt);
        vprintk(fmt, args);
        va_end(args);
    }
}
void balongfb_logd_display_debugfs(const char * fmt,...)
{
    va_list args;
    if (g_fb_log_printk_flag.bits.bit_display >= ADE_DEBUG_PRINT) {
        va_start(args, fmt);
        vprintk(fmt, args);
        va_end(args);
    }
}
void balongfb_logw_display_debugfs(const char * fmt,...)
{
    va_list args;
    if (g_fb_log_printk_flag.bits.bit_display >= ADE_WARN_PRINT) {
        va_start(args, fmt);
        vprintk(fmt, args);
        va_end(args);
    }
}
void balongfb_loge_display_debugfs(const char * fmt,...)
{
    va_list args;
    if (g_fb_log_printk_flag.bits.bit_display >= ADE_ERR_PRINT) {
        va_start(args, fmt);
        vprintk(fmt, args);
        va_end(args);
    }
}

void balongfb_logi_backlight_debugfs(const char* fmt, ...)
{
    va_list args;

    if (g_fb_log_printk_flag.bits.bit_backlight) {
        va_start(args, fmt);
        vprintk(fmt, args);
        va_end(args);
    }
}

void balongfb_logd_top_info(const char* fmt, ...)
{
    va_list args;
    if (g_fb_log_printk_flag.bits.bit_topinfo >= ADE_DEBUG_PRINT) {
        va_start(args, fmt);
        vprintk(fmt, args);
        va_end(args);
    }
}
void balongfb_logi_top_info(const char* fmt, ...)
{
    va_list args;

    if (g_fb_log_printk_flag.bits.bit_topinfo >= ADE_INFO_PRINT) {
        va_start(args, fmt);
        vprintk(fmt, args);
        va_end(args);
    }
}

void balongfb_logw_top_info(const char* fmt, ...)
{
    va_list args;
    if (g_fb_log_printk_flag.bits.bit_topinfo >= ADE_WARN_PRINT) {
        va_start(args, fmt);
        vprintk(fmt, args);
        va_end(args);
    }
}

void balongfb_loge_top_info(const char* fmt, ...)
{
    va_list args;
    if (g_fb_log_printk_flag.bits.bit_topinfo >= ADE_ERR_PRINT) {
        va_start(args, fmt);
        vprintk(fmt, args);
        va_end(args);
    }
}
#endif

