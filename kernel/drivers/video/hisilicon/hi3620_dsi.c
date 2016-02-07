
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <linux/platform_data/hi3620-dsi.h>

#include "hi3620_fb.h"

/* DSI PHY internal register */
#define DSI_PHY_CP_CURRENT		0x11
#define DSI_PHY_LPF_CTRL		0x12
#define DSI_PHY_PLL_UNLOCK_FILTER	0x16
#define DSI_PHY_N_PLL			0x17
#define DSI_PHY_M_PLL			0x18
#define DSI_PHY_FACTOR			0x19
#define DSI_PHY_HS_FREQ_RANGE		0x44

#define PHY_ADDR			(1 << 16)

#define DTYPE_GEN_WRITE_HEAD		0x03
#define DTYPE_GEN_WRITE_PAYLOAD		0x29

struct phy_timing {
	int	dsi2x;		/* MHz */
	int	lp2hs;
	int	hs2lp;
	int	hsfreq;
};

static struct device *dsi_dev = NULL;

static void reset(void __iomem *reg_base)
{
	writel_relaxed(0, reg_base + DSI_PWR_UP);
}

static void unreset(void __iomem *reg_base)
{
	writel_relaxed(1, reg_base + DSI_PWR_UP);
}

static void set_highspeed(void __iomem *reg_base)
{
	unsigned int data;

	/* enable high speed clock for PHY */
	data = readl_relaxed(reg_base + DSI_PHY_IF_CTRL);
	data |= (1 << 0);
	writel_relaxed(data, reg_base + DSI_PHY_IF_CTRL);
}

static void unset_highspeed(void __iomem *reg_base)
{
	unsigned int data;

	/* disable high speed clock for PHY */
	data = readl_relaxed(reg_base + DSI_PHY_IF_CTRL);
	data &= ~(1 << 0);
	writel_relaxed(data, reg_base + DSI_PHY_IF_CTRL);
}

/* bit definition of register DSI_VID_MODE_CFG */
#define VIDEO_MODE_EN		(1 << 0)

/* bit definition of register DSI_CMD_MODE_CFG */
#define CMD_MODE_EN		(1 << 0)
#define CMD_MODE_LP		(0x1ffe)

static int is_video_mode(void __iomem *base)
{
	unsigned int data;

	data = readl_relaxed(base + DSI_VID_MODE_CFG);
	if (data & VIDEO_MODE_EN)
		return 1;
	return 0;
}

static int is_cmd_mode(void __iomem *base)
{
	unsigned int data;

	data = readl_relaxed(base + DSI_CMD_MODE_CFG);
	if (data & CMD_MODE_EN)
		return 1;
	return 0;
}

/* Switch to CMD mode with LP transmission */
static void set_cmd_mode(void __iomem *base)
{
	unsigned int data;

	/* disable VIDEO mode */
	data = readl_relaxed(base + DSI_VID_MODE_CFG);
	data &= ~VIDEO_MODE_EN;
	writel_relaxed(data, base + DSI_VID_MODE_CFG);
	/* enable COMMAND mode with LP transmission */
	data = readl_relaxed(base + DSI_CMD_MODE_CFG);
	data |= CMD_MODE_LP | CMD_MODE_EN;
	writel_relaxed(data, base + DSI_CMD_MODE_CFG);
}

static void init_video_mode(void __iomem *base, struct hi3620fb_info *info)
{
	struct fb_var_screeninfo *var = &info->fb->var;
	unsigned int data;

	data = readl_relaxed(base + DSI_VID_MODE_CFG);
	data |= 0x1f8;		/* enable Low Power control */
	data &= ~(3 << 1);	/* mask video transfer mode */
	data |= (2 << 1);	/* Burst with Sync Pulses */
	data &= ~0x800;		/* Disable frame end ACK */
	writel_relaxed(data, base + DSI_VID_MODE_CFG);
	data = readl_relaxed(base + DSI_VID_PKT_CFG);
	data &= ~0x7ff;		/* mask single video packet size */
	data |= var->xres;
	writel_relaxed(data, base + DSI_VID_PKT_CFG);
	data = readl_relaxed(base + DSI_PCKHDL_CFG);
	data |= (1 << 2);	/* enable Bus Turn-Around */
	writel_relaxed(data, base + DSI_PCKHDL_CFG);
}

/* Switch to VIDEO mode */
static void set_video_mode(void __iomem *base)
{
	unsigned int data;

	/* disable CMD mode */
	data = readl_relaxed(base + DSI_CMD_MODE_CFG);
	data |= CMD_MODE_LP;
	data &= ~CMD_MODE_EN;
	writel_relaxed(data, base + DSI_CMD_MODE_CFG);
	/* enable VIDEO mode */
	data = readl_relaxed(base + DSI_VID_MODE_CFG);
	data |= VIDEO_MODE_EN;
	writel_relaxed(data, base + DSI_VID_MODE_CFG);
}

static void set_dpi_timing(struct hi3620fb_info *info)
{
	void __iomem *base = info->reg_base;
	struct fb_videomode *fb_vm = info->fb->mode;
	unsigned int dsi_rate, lane_rate, pixclock, data, timing;
	unsigned long long int tmp;

	dsi_rate = clk_get_rate(info->clk_dsi);	/* dsi1x, not dsi2x */
	lane_rate = dsi_rate / 4;		/* dsi2x / 8 */

	pixclock = PICOS2KHZ(fb_vm->pixclock) * 1000;
	tmp = (unsigned long long int)(fb_vm->hsync_len) * lane_rate;
	do_div(tmp, pixclock);
	data = (unsigned int)tmp;
	timing = data & 0x1ff;
	tmp = (unsigned long long int)(fb_vm->left_margin) * lane_rate;
	do_div(tmp, pixclock);
	data = (unsigned int)tmp;
	timing |= (data & 0x1ff) << 9;
	tmp = (unsigned long long int)(fb_vm->left_margin + fb_vm->xres +
				       fb_vm->right_margin + fb_vm->hsync_len);
	tmp *= lane_rate;
	do_div(tmp, pixclock);
	data = (unsigned int)tmp;
	timing |= data << 18;
	writel_relaxed(timing, info->reg_base + DSI_TMR_LINE_CFG);

	/* setup frame timing */
	timing = (fb_vm->vsync_len) & 0xf;
	timing |= ((fb_vm->upper_margin) & 0x3f) << 4;
	timing |= ((fb_vm->lower_margin) & 0x3f) << 10;
	timing |= ((fb_vm->yres) & 0x7ff) << 16;
	writel_relaxed(timing, info->reg_base + DSI_VTIMING_CFG);
}

#define PHY_TX_OUT_LP_DATA		(1 << 4)
#define PHY_TX_IN_LP_DATA		(1 << 3)
#define PHY_TX_OUT_LP_CLK		(1 << 2)
#define PHY_TX_IN_LP_CLK		(1 << 1)
#define PHY_TX_REQ_HS_CLK		(1 << 0)
#define PHY_TX_MASK			0x1f

static int is_phy_hs(void __iomem *base)
{
	unsigned int data, expected;

	data = readl_relaxed(base + DSI_PHY_IF_CTRL);
	expected = PHY_TX_OUT_LP_DATA | PHY_TX_OUT_LP_CLK | PHY_TX_REQ_HS_CLK;
	if ((data & PHY_TX_MASK) == expected)
		return 1;
	return 0;
}

static int is_phy_lp(void __iomem *base)
{
	unsigned int data, expected;

	data = readl_relaxed(base + DSI_PHY_IF_CTRL);
	expected = PHY_TX_IN_LP_DATA | PHY_TX_IN_LP_CLK;
	if ((data & PHY_TX_MASK) == expected)
		return 1;
	return 0;
}

static int set_phy_hs(void __iomem *base)
{
	unsigned int data;

	data = readl_relaxed(base + DSI_PHY_IF_CTRL);
	data &= ~PHY_TX_MASK;
	data |= PHY_TX_OUT_LP_DATA | PHY_TX_OUT_LP_CLK;
	writel_relaxed(data, base + DSI_PHY_IF_CTRL);
	data = readl_relaxed(base + DSI_PHY_IF_CTRL);
	return 0;
}

static int set_phy_lp(void __iomem *base)
{
	unsigned int data;

	data = readl_relaxed(base + DSI_PHY_IF_CTRL);
	data &= ~PHY_TX_MASK;
	data |= PHY_TX_OUT_LP_DATA | PHY_TX_OUT_LP_CLK;
	writel_relaxed(data, base + DSI_PHY_IF_CTRL);
	data = readl_relaxed(base + DSI_PHY_IF_CTRL);
	return 0;
}

static void enable_phy(void __iomem *base)
{
	unsigned int status, mask;

	writel_relaxed(0x7, base + DSI_PHY_RSTZ);
	mask = DSI_PHY_STOP_STATE0_LANE | DSI_PHY_LOCK;
	do {
		cpu_relax();
		status = readl_relaxed(base + DSI_PHY_STATUS);
	} while ((status & mask) != mask);
}

static void disable_phy(void __iomem *reg_base)
{
	writel_relaxed(0, reg_base + DSI_PHY_RSTZ);
}

void hi3620_mipi_dsi_set_video_packet_size(void __iomem *reg_base,
					   int null_pkt_size, int num_vid_pkt,
					   int vid_pkt_size)
{
	unsigned int data;
	data = (null_pkt_size & 0x3ff) << 21;	/* byte size */
	/* number of video packets for Each Multiple Packets */
	data |= (num_vid_pkt & 0x3ff) << 11;
	/* pixels of each video packet */
	data |= vid_pkt_size & 0x7ff;
	writel_relaxed(data, reg_base + DSI_VID_PKT_CFG);
}

static void hi3620_mipi_setup_dpi(struct hi3620fb_info *info)
{
	unsigned int data;

	/* set lane numbers */
	/*
	data = readl_relaxed(info->reg_base + DSI_PHY_IF_CFG) & ~0x3;
	data |= (info->lane_cnt - 1) & 0x3;
	writel_relaxed(data, info->reg_base + DSI_PHY_IF_CFG);
	*/
	data = 0;
	//data = readl_relaxed(info->reg_base + DSI_DPI_CFG);
	/* set virtual channel ID as 0 */
	data &= ~(3 << 0);
	/* set color mode */
	data &= ~(7 << 2);
	data |= info->color_mode << 2;
	/* always set color mode & shutdown high active */
	writel_relaxed(data, info->reg_base + DSI_DPI_CFG);
}

#define MAX_TX_ESC_CLK		(10 * 1000 * 1000)

static void setup_phy(struct hi3620fb_info *info)
{
	struct clk *parent;
	unsigned char hs2lp = 0, lp2hs = 0, hsfreq = 0;
	unsigned int data;
	void __iomem *base = info->reg_base;
	int rate, rate_mhz, dsi_rate, lane_rate, i;
	struct phy_timing timing[] = {
				{90, 24, 14, 0}, {100, 25, 14, 0x20},
				{110, 25, 14, 0x40}, {125, 25, 14, 0x02},
				{140, 25, 14, 0x22}, {150, 25, 14, 0x42},
				{160, 25, 14, 0x04}, {180, 28, 16, 0x24},
				{200, 32, 16, 0x44}, {210, 31, 16, 0x06},
				{240, 35, 17, 0x26}, {250, 37, 18, 0x46},
				{270, 37, 18, 0x08}, {300, 39, 19, 0x28},
				{330, 44, 20, 0x08}, {360, 47, 21, 0x2a},
				{400, 48, 21, 0x4a}, {450, 54, 23, 0x0c},
				{500, 58, 25, 0x2c}, {550, 62, 26, 0x0e},
				{600, 67, 28, 0x2e}, {650, 72, 30, 0x10},
				{700, 76, 31, 0x30}, {750, 81, 32, 0x12},
				{800, 86, 34, 0x32}, {850, 89, 35, 0x14},
				{900, 95, 37, 0x34}, {950, 99, 38, 0x54},
				{1000, 104, 40, 0x74}, };
	parent = clk_get_parent(info->clk_dsi);
	rate = clk_get_rate(parent);
	rate_mhz = rate / 1000000;
	for (i = 0; i < ARRAY_SIZE(timing); i++) {
		if (rate_mhz <= timing[i].dsi2x) {
			hs2lp = timing[i].hs2lp;
			lp2hs = timing[i].lp2hs;
			hsfreq = timing[i].hsfreq;
			break;
		}
	}

	/* setup PHY timing */
	data = 4095;	/* bta time */
	data |= (lp2hs & 0xff) << 16;
	data |= (hs2lp & 0xff) << 24;
	writel_relaxed(data, base + DSI_PHY_TMR_CFG);
	/* set hsfreqrange */
	hi3620_dsi_phy_write(base, 0x44, hsfreq);
	writel_relaxed(0x7, base + DSI_PHY_RSTZ);

#if 0
	/* set tx esc clk division */
	dsi_rate = clk_get_rate(info->clk_dsi);
	lane_rate = dsi_rate / 4;
	data = (lane_rate + (MAX_TX_ESC_CLK / 2) - 1) / MAX_TX_ESC_CLK;
	writel_relaxed(data, base + DSI_CLKMGR_CFG);
#endif
}

void hi3620_mipi_dsi_set_lane(void __iomem *reg_base, int id, int count)
{
	unsigned int data, cnt;

	cnt = count - 1;
	if (cnt < 0 || id > cnt)
		return;
	data = readl_relaxed(reg_base + DSI_PHY_IF_CFG) & ~0x3;
	data |= cnt & 0x3;
	writel_relaxed(data, reg_base + DSI_PHY_IF_CFG);
	data = readl_relaxed(reg_base + DSI_DPI_CFG) & ~0x3;
	data |= id & 0x3;
	writel_relaxed(data, reg_base + DSI_DPI_CFG);
}

/*
 * PHY_TST_CTRL0 & PHY_TST_CTRL1 registers are the interfaces of accessing
 * PHY internal registers.
 * PHY_TST_CTRL0 is used to produce clock, as I2C SCLK.
 * PHY_TST_CTRL1 is used to store address or data, as I2C SDA.
 */
static void set_phy_testclk(void __iomem *reg_base, int level)
{
	unsigned int data;

	if (level)
		data = 0x2;
	else
		data = 0;
	writel_relaxed(data, reg_base + DSI_PHY_TST_CTRL0);
}

/* write 8-bit data into 8-bit phy register */
int hi3620_dsi_phy_write(void __iomem *reg_base, unsigned char addr,
			 unsigned char data)
{
	unsigned int value;

	set_phy_testclk(reg_base, 0);
	value = (unsigned int)addr | PHY_ADDR;
	writel_relaxed(value, reg_base + DSI_PHY_TST_CTRL1);
	set_phy_testclk(reg_base, 1);

	set_phy_testclk(reg_base, 0);
	value = (unsigned int)data;
	writel_relaxed(value, reg_base + DSI_PHY_TST_CTRL1);
	set_phy_testclk(reg_base, 1);
	set_phy_testclk(reg_base, 0);
	return 0;
}
EXPORT_SYMBOL(hi3620_dsi_phy_write);

/* read 8-bit data from 8-bit phy register */
unsigned char hi3620_dsi_phy_read(void __iomem *reg_base, unsigned char addr)
{
	unsigned int value;

	set_phy_testclk(reg_base, 0);
	value = (unsigned int)addr | PHY_ADDR;
	writel_relaxed(value, reg_base + DSI_PHY_TST_CTRL1);
	set_phy_testclk(reg_base, 1);
	set_phy_testclk(reg_base, 0);
	value = readl_relaxed(reg_base + DSI_PHY_TST_CTRL1);
	return (unsigned char)(value >> 8);
}

static bool is_wfifo_full(void __iomem *base)
{
	unsigned int status;

	status = readl_relaxed(base + DSI_CMD_PKT_STATUS);
	if (status & GEN_PLD_W_FULL)
		return true;
	return false;
}

static int recv_generic_pkt(void __iomem *base, u8 *val, int len)
{
	int i, cnt;
	unsigned int status, data;

	cnt = (len + 3) / 4;
	for (i = 0; i < cnt; i++) {
		do {
			cpu_relax();
			status = readl_relaxed(base + DSI_CMD_PKT_STATUS);
			if (status & GEN_PLD_R_EMPTY) {
				pr_err("#%s, %d, found empty read fifo\n",
					__func__, __LINE__);
				return -EINVAL;
			}
		} while (status & GEN_RD_CMD_BUSY);

		data = readl_relaxed(base + DSI_GEN_PLD_DATA);
		val[i * 4] = data & 0xff;
		val[i * 4 + 1] = (data >> 8) & 0xff;
		val[i * 4 + 2] = (data >> 16) & 0xff;
		val[i * 4 + 3] = (data >> 24) & 0xff;
	}
	do {
		cpu_relax();
		status = readl_relaxed(base + DSI_CMD_PKT_STATUS);
		/* read data until FIFO empty */
		readl_relaxed(base + DSI_GEN_PLD_DATA);
	} while (!(status & GEN_PLD_R_EMPTY));
	return 0;
}

static void wait_phy_ready(void __iomem *base)
{
#if 0
	unsigned int status;

	do {
		cpu_relax();
		status = readl_relaxed(base + DSI_PHY_STATUS);
	} while (!(status & DSI_PHY_STOP_STATE0_LANE));
#else
	/*
	 * In some board, it always loop as dead.
	 * So use delay as workaround.
	 */
	msleep(5);
#endif
}

int set_packet(struct device *dev, u8 *cmd, int nr_payload)
{
	struct hi3620fb_info *info = dev_get_drvdata(dev);
	void __iomem *base = info->reg_base;
	unsigned int header, type, data = 0;
	int ret = 0, video_mode = 0, i = 0, j = 0;

	mutex_lock(&info->dsi_mutex);
	wait_phy_ready(base);
	if (is_video_mode(base)) {
		video_mode = 1;
		set_cmd_mode(base);
	}

	/* set DT type */
	type = cmd[0] & 0xff;
	header = type;

	switch (type) {
	case DCS_SHORT_WR_PARAM0:
		/* set DSI command */
		header |= (cmd[1] & 0xff) << 8;
		writel_relaxed(header, base + DSI_GEN_HDR);
		break;
	case DCS_SHORT_WR_PARAM1:
		header |= (cmd[1] & 0xff) << 8;
		header |= (cmd[2] & 0xff) << 16;
		writel_relaxed(header, base + DSI_GEN_HDR);
		break;
	case GEN_LONG_WR:
		for (i = 0; i < nr_payload;) {
			data = 0;
			for (j = 0; i < nr_payload && j < 4; i++, j++)
				data |= cmd[i + 1] << (j * 8);
			writel_relaxed(data, base + DSI_GEN_PLD_DATA);
			if (is_wfifo_full(base))
				pr_err("wfifo is full\n");
		}
		header |= nr_payload << 8;
		writel_relaxed(header, base + DSI_GEN_HDR);
		break;
	case GEN_RD_PARAM1:
		header |= (cmd[1] & 0xff) << 8;
		writel_relaxed(header, base + DSI_GEN_HDR);
		udelay(20);	/* FIXME */
		ret = recv_generic_pkt(base, &cmd[2], nr_payload);
		break;
	case DCS_RD_PARAM0:
		header |= (cmd[1] & 0xff) << 8;
		writel_relaxed(header, base + DSI_GEN_HDR);
		udelay(20);	/* FIXME */
		ret = recv_generic_pkt(base, &cmd[2], 1);
		break;
	default:
		pr_warn("###%s: not set packet type\n", __func__);
		break;
	}
	wait_phy_ready(base);
	/* restore video mode if necessary */
	if (video_mode)
		set_video_mode(base);
	mutex_unlock(&info->dsi_mutex);
	return ret;
}

static int write_packet(struct device *dev, enum DATA_TYPE type, u8 *cmd,
			int len)
{
	struct hi3620fb_info *info = dev_get_drvdata(dev);
	void __iomem *base = info->reg_base;
	unsigned int header;
	int ret, video_mode = 0;

	if (is_video_mode(base)) {
		video_mode = 1;
		set_cmd_mode(base);
	}

	header = type & 0xff;
	header |= (cmd[0] & 0xff) << 8;

	switch (type) {
	case DCS_SHORT_WR_PARAM0:
		writel_relaxed(header, base + DSI_GEN_HDR);
		break;
	case DCS_SHORT_WR_PARAM1:
		header |= (cmd[1] & 0xff) << 16;
		writel_relaxed(header, base + DSI_GEN_HDR);
		break;
	default:
		pr_err("#%s, %d, wrong type:%d\n", __func__, __LINE__, type);
		break;
	}
	/* restore video mode if necessary */
	if (video_mode)
		set_video_mode(base);
	return ret;
}

int dsi_set_packet(u8 *cmd, int nr_payload)
{
	if (dsi_dev)
		return set_packet(dsi_dev, cmd, nr_payload);
	else
		return -EINVAL;
}

int hi3620_mipi_init(struct device *dev)
{
	struct hi3620fb_info *info = dev_get_drvdata(dev);

	dsi_dev = dev;
	mutex_init(&info->dsi_mutex);
	return 0;
}
EXPORT_SYMBOL(hi3620_mipi_init);

int hi3620_mipi_enable(struct device *dev)
{
	struct hi3620fb_info *info = dev_get_drvdata(dev);

	unreset(info->reg_base);
	init_video_mode(info->reg_base, info);

	clk_set_rate(info->clk_dsi, info->dsi_rate);	/* huawei logo is shifted to right & color may be changed??? */
	set_dpi_timing(info);
	clk_prepare_enable(info->clk_dsi);

	setup_phy(info);
	enable_phy(info->reg_base);

	set_cmd_mode(info->reg_base);
	unset_highspeed(info->reg_base);

	/* set panel on */

	if (!strncmp(info->mipi_mode_name, "video", 5))
		set_video_mode(info->reg_base);
	else if (!strncmp(info->mipi_mode_name, "command", 7))
		set_cmd_mode(info->reg_base);
	else
		return -EINVAL;
	set_highspeed(info->reg_base);
	return 0;
}
EXPORT_SYMBOL(hi3620_mipi_enable);

int hi3620_mipi_disable(struct device *dev)
{
	struct hi3620fb_info *info = dev_get_drvdata(dev);

	/* set panel off */

	set_cmd_mode(info->reg_base);
	unset_highspeed(info->reg_base);
	unreset(info->reg_base);
	disable_phy(info->reg_base);
	reset(info->reg_base);
	clk_disable_unprepare(info->clk_dsi);
	return 0;
}
EXPORT_SYMBOL(hi3620_mipi_disable);
