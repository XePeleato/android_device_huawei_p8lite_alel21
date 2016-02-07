#ifndef __HI3620FB_H
#define __HI3620FB_H

#define EDC_ID				0x000

/* Channel 1 */
#define	EDC_GRAPH_CHAN_ADDR		0x004
#define EDC_GRAPH_CHAN_STRIDE		0x00c
#define EDC_GRAPH_CHAN_XY		0x010
#define EDC_GRAPH_CHAN_SIZE		0x014
#define EDC_GRAPH_CHAN_CTRL		0x018
#define EDC_GRAPH_CHAN_CKEY_MIN		0x01c
#define EDC_GRAPH_CHAN_CKEY_MAX		0x020

/* Channel 2 */
#define EDC_VIDEO_CHAN_ADDR		0x024
#define EDC_VIDEO_CHAN_STRIDE		0x02c
#define EDC_VIDEO_CHAN_XY		0x030
#define EDC_VIDEO_CHAN_SIZE		0x034
#define EDC_VIDEO_CHAN_CTRL		0x038
#define EDC_VIDEO_CHAN_CKEY_MIN		0x03c
#define EDC_VIDEO_CHAN_CKEY_MAX		0x040

#define EDC_GRAPH_CHAN_CTRL_EN		(1 << 24)
#define EDC_VIDEO_CHAN_CTRL_EN		(1 << 22)
#define EDC_CHAN_CTRL_BGR		(1 << 19)

#define EDC_DISP_SIZE			0x090
#define EDC_DISP_CTL			0x094
#define EDC_DISP_DPD			0x098
#define	EDC_STS				0x09c
#define	EDC_INTS			0x0a0
#define EDC_INTE			0x0a4

#define LDI_HRZ_CTRL0			0x800
#define LDI_HRZ_CTRL1			0x804
#define LDI_VRT_CTRL0			0x808
#define LDI_VRT_CTRL1			0x80c
#define LDI_PLR_CTRL			0x810
#define LDI_DSP_SIZE			0x814
#define LDI_INT_EN			0x81c
#define LDI_CTRL			0x820
#define LDI_ORG_INT			0x824
#define LDI_MSK_INT			0x828
#define LDI_INT_CLR			0x82c
#define LDI_WORK_MODE			0x830
#define LDI_HDMI_DSI_GT			0x834

#define DSI_CMD_MOD_CTRL		0x83c
#define DSI_TE_CTRL			0x840
#define DSI_TE_HS_NUM			0x844
#define	DSI_TE_HS_WD			0x848
#define DSI_TE_VS_WD			0x84c

#define DSI_PWR_UP			0x904
#define DSI_CLKMGR_CFG			0x908
#define DSI_DPI_CFG			0x90c
#define DSI_PCKHDL_CFG			0x918
#define DSI_VID_MODE_CFG		0x91c
#define DSI_VID_PKT_CFG			0x920
#define DSI_CMD_MODE_CFG		0x924
#define DSI_TMR_LINE_CFG		0x928
#define DSI_VTIMING_CFG			0x92c
#define DSI_PHY_TMR_CFG			0x930
#define DSI_GEN_HDR			0x934
#define DSI_GEN_PLD_DATA		0x938
#define DSI_CMD_PKT_STATUS		0x93c
#define DSI_TO_CNT_CFG			0x940
#define DSI_ERROR_ST0			0x944
#define DSI_ERROR_ST1			0x948
#define DSI_ERROR_MSK0			0x94c
#define DSI_ERROR_MSK1			0x950
#define DSI_PHY_RSTZ			0x954
#define DSI_PHY_IF_CFG			0x958
#define DSI_PHY_IF_CTRL			0x95c
#define DSI_PHY_STATUS			0x960
#define DSI_PHY_TST_CTRL0		0x964
#define DSI_PHY_TST_CTRL1		0x968
#define DSI_EDPI_CFG			0x96c
#define DSI_LPCMD_TIME			0x970

/* bits field of DSI_CMD_PKT_STATUS register */
#define GEN_RD_CMD_BUSY			(1 << 6)
#define GEN_PLD_R_FULL			(1 << 5)
#define GEN_PLD_R_EMPTY			(1 << 4)
#define GEN_PLD_W_FULL			(1 << 3)

/* bits field of EDC_DISP_CTL register */
#define EDC_CTRL_CLK_EN			(1 << 31)
#define EDC_CTRL_FRAME_END_START	(1 << 30)
#define EDC_CTRL_EN			(1 << 10)
#define EDC_CTRL_CFG_OK			(1 << 1)
#define EDC_CTRL_CFG_OK_SEL		(1 << 0)

/* bits field of EDC_INTS/EDC_INTE register */
#define EDC_INT_BAS_END			(1 << 6)

/* bits field of LDI_CTRL register */
#define LDI_CTRL_SHUTDOWN		(1 << 15)
#define LDI_CTRL_COLOR_MODE		(1 << 14)
#define LDI_CTRL_BGR			(1 << 13)
#define LDI_CTRL_DATA_GATE_EN		(1 << 2)
#define LDI_CTRL_DISP_MODE		(1 << 1)
#define LDI_CTRL_EN			(1 << 0)

/* bits field of LDI_WORK_MODE register */
#define LDI_WORK_MODE_EN		(1 << 0)

/* bits field of LDI_PLT_CTRL register */
#define LDI_POLARITY_MASK		0xf
#define LDI_DATAEN_POLARITY		(1 << 3)
#define LDI_PIXELCLK_POLARITY		(1 << 2)
#define LDI_HSYNC_POLARITY		(1 << 1)
#define LDI_VSYNC_POLARITY		(1 << 0)

/* bits field of DSI_DPI_CFG register */
#define DSI_DPI_POLARITY_MASK		(0x1f << 5)
#define DSI_DPI_COLORM_POLARITY		(1 << 9)
#define DSI_DPI_SHUTD_POLARITY		(1 << 8)
#define DSI_DPI_HSYNC_POLARITY		(1 << 7)
#define DSI_DPI_VSYNC_POLARITY		(1 << 6)
#define DSI_DPI_DATAEN_POLARITY		(1 << 5)

/* bits field of DSI_PHY_STATUS register */
#define DSI_PHY_STOP_STATE3_LANE	(1 << 12)
#define DSI_PHY_STOP_STATE2_LANE	(1 << 9)
#define DSI_PHY_STOP_STATE1_LANE	(1 << 7)
#define DSI_PHY_STOP_STATE0_LANE	(1 << 4)
#define DSI_PHY_LOCK			(1 << 0)

enum {
	IMG_PIXEL_FORMAT_ARGB1555 = 0,
	IMG_PIXEL_FORMAT_RGB555,
	IMG_PIXEL_FORMAT_RGB565,
	IMG_PIXEL_FORMAT_RGB888,
	IMG_PIXEL_FORMAT_ARGB8888,
};

struct hi3620fb_info {
	struct fb_info		*fb;
	struct device		*dev;
	void __iomem		*reg_base;
	struct clk		*clk_ldi;
	struct clk		*clk_edc;
	struct clk		*clk_dsi;
	int			irq_edc;
	int			irq_ldi;
	int			irq_dsi;
	struct regulator	*vedc;
	dma_addr_t		fb_start_dma;
	int			pix_fmt;
	int			dsi_rate;	/* dsi bit clock rate */
	const char		*mipi_mode_name;
	int			lane_cnt;
	int			color_mode;
	wait_queue_head_t	wait_vsync;
	int			vsync_cnt;
	struct mutex		dsi_mutex;
	unsigned int		graph_chan_addr;
	unsigned int		graph_chan_stride;
	unsigned int		graph_chan_xy;
	unsigned int		graph_chan_size;
	unsigned int		graph_chan_ctrl;
	unsigned int		video_chan_addr;
	unsigned int		video_chan_stride;
	unsigned int		video_chan_xy;
	unsigned int		video_chan_size;
	unsigned int		video_chan_ctrl;
	unsigned int		edc_inte;
	unsigned int		edc_disp_size;
	unsigned int		edc_disp_dpd;
	unsigned int		edc_disp_ctrl;
	unsigned int		ldi_hrz_ctrl0;
	unsigned int		ldi_hrz_ctrl1;
	unsigned int		ldi_vrt_ctrl0;
	unsigned int		ldi_vrt_ctrl1;
	unsigned int		ldi_plr_ctrl;
	unsigned int		ldi_dsp_size;
	unsigned int		ldi_inte;
	unsigned int		ldi_ctrl;
	unsigned int		ldi_work_mode;
	unsigned int		ldi_hdmi_dsi_gt;
	unsigned int		dsi_cmd_mod_ctrl;
	unsigned int		dsi_te_ctrl;
	unsigned int		dsi_te_hs_num;
	unsigned int		dsi_te_hs_wd;
	unsigned int		dsi_te_vs_wd;
	unsigned int		clkmgr_cfg;
	unsigned int		dpi_cfg;
	unsigned int		pckhdl_cfg;
	unsigned int		vid_mode_cfg;
	unsigned int		vid_pkt_cfg;
	unsigned int		cmd_mode_cfg;
	unsigned int		tmr_line_cfg;
	unsigned int		vtiming_cfg;
	unsigned int		to_cnt_cfg;
	unsigned int		phy_rstz;
	unsigned int		phy_if_cfg;
	unsigned int		phy_if_ctrl;
	unsigned int		edpi_cfg;
	unsigned int		lpcmd_time;
};

extern int hi3620_mipi_init(struct device *dev);
extern int hi3620_mipi_enable(struct device *dev);
extern int hi3620_mipi_disable(struct device *dev);
extern int send_generic_packet(u8 *cmd, int len);
#endif	/* __HI3620FB_H */
