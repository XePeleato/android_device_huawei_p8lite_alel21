
#include "hisi_noc_bus.h"

char *sysnoc_initflow_array[SYSNOC_INITFLOW_ARRAY_SIZE] = {
	"noc_acpu",
	"noc_cssys",
	"noc_dap",
	"noc_dap_apb",
	"noc_dmac",
	"noc_hifi",
	"noc_ipf",
	"noc_mcu",
	"noc_mmc0",
	"noc_mmc1",
	"noc_mmc2",
	"noc_modem",
	"noc_secend",
	"noc_socp",
	"noc_usb"
};

char *sysnoc_targetflow_array[SYSNOC_TARGETFLOW_ARRAY_SIZE] = {
	"cfg_noc_target",
	"cssys_apb_target",
	"ddrc_target",
	"dmac_s_target",
	"hifi_s_target",
	"ipf_s_target",
	"mcu_s_target",
	"modem_target",
	"secend_s_target",
	"service_target",
	"service_ao_target",
	"service_dma_target",
	"socp_s_target",
	"tzma_target",
	"xg2ram0_target"
};


char *media_initflow_array[MEDIA_INITFLOW_ARRAY_SIZE] = {
	"noc_ade0",
	"noc_ade1",
	"noc_cfg",
	"noc_isp",
	"noc_jpu",
	"noc_vpu",
	"noc_vpu_sec"
};

char *media_targetflow_array[MEDIA_TARGETFLOW_ARRAY_SIZE] = {
	"apb_video_s_jsp_target",
	"apb_video_s_jpu_target",
	"apb_video_s_vpu_target",
	"ahb_isp_s_target",
	"apb_ade_s_target",
	"apb_g3d_s_target",
	"apb_media_sc_target",
	"apb_smmu_s_target",
	"axi_smmu_s_target",
	"axi_xg2ram_harq_target",
	"service_target",
	"service_video_target"
};

struct noc_bus_info hi6220_noc_buses_info[HI6220_MAX_NOC_BUSES_NR] = {
	[0] = {
		.name = "sys_noc_bus",
		.initflow_mask = ((0xf) << 14),
		.targetflow_mask = ((0xf) << 10),
		.targ_subrange_mask = ((0x7) << 7),
		.seq_id_mask = 0,
		.initflow_array = sysnoc_initflow_array,
		.initflow_array_size = SYSNOC_INITFLOW_ARRAY_SIZE,
		.targetflow_array = sysnoc_targetflow_array,
		.targetflow_array_size = SYSNOC_TARGETFLOW_ARRAY_SIZE,
	},
	[1] = {
		.name = "media_noc_bus",
		.initflow_mask = ((0x7) << 14),
		.targetflow_mask = ((0xf) << 10),
		.targ_subrange_mask =  ((0x7) << 7),
		.seq_id_mask = 0 ,
		.initflow_array = media_initflow_array,
		.initflow_array_size = MEDIA_INITFLOW_ARRAY_SIZE,
		.targetflow_array = media_targetflow_array,
		.targetflow_array_size = MEDIA_TARGETFLOW_ARRAY_SIZE,
	}
};




#ifdef CONFIG_HISI_NOC_HI3650_PLATFORM

char *cfg_initflow_array[CFG_INITFLOW_ARRAY_SIZE] = {
	"noc_mmc1bus_sd3",
	"noc_mmc1bus_sdio1",
	"noc_a7tocfg",
	"noc_asp_mst",
	"noc_cci2sysbus",
	"noc_djtag_mst",
	"noc_dmac_mst",
	"noc_emmc0",
	"noc_emmc1",
	"noc_iomcu_ahb_mst",
	"noc_iomcu_axi_mst",
	"noc_iomcu_dma_mst",
	"noc_ivp32_mst",
    "noc_lpmcu_mst",
    "noc_mmc0bus_sdio0",
    "noc_modem_mst",
    "noc_perf_stat",
    "noc_sec_p",
    "noc_sec_s",
    "noc_socp",
    "noc_top_cssys",   
	"noc_usb3",
};

char *cfg_targetflow_array[CFG_TARGETFLOW_ARRAY_SIZE] = {
"aspbus_service_target",
"cfgbus_service_target",
"dbgbus_service_target",
"dmabus_service_target",
"ivp32_peri_bus_service_target",
"mmc0bus_service_target",  
"mmc1bus_service_target",
"modembus_service_target",
"noc_aobus_mmbuf",
"noc_asp_cfg",
"noc_cfg2vivo",
"noc_debug_apb_cfg",
"noc_dmac_cfg",
"noc_emmc0_cfg",
"noc_emmc0bus_apb_cfg",
"noc_gic",
"noc_hkadc_ssi",
"noc_iomcu_ahb_slv",
"noc_iomcu_apb_slv",
"noc_ivp32_cfg",
"noc_lpmcu_slv",
"noc_mmc1bus_apb_cfg",
"noc_modem_cfg",
"noc_socp_cfg",
"noc_sys2ddrc",
"noc_sys2hkmem",
"noc_sys_peri0_cfg",
"noc_sys_peri1_cfg",
"noc_sys_peri2_cfg",
"noc_sys_peri3_cfg",
"noc_top_cssys_slv",
"noc_usb3_cfg",
"sysbus_service_target",
};


char *vivo_initflow_array[VIVO_INITFLOW_ARRAY_SIZE] = {
"noc_a7tovivobus_rd",
"noc_a7tovivobus_wr",
"noc_dss0_rd",
"noc_dss0_wr",
"noc_dss1_rd",
"noc_dss1_wr",
"noc_isp_rd",
"noc_isp_wr",
"noc_vivo_cfg",
};

char *vivo_targetflow_array[VIVO_TARGETFLOW_ARRAY_SIZE] = {
	"dss_service_target",
	"isp_service_target",
	"noc_dss_cfg",
	"noc_isp_cfg",
	"noc_vivobus_ddrc0_rd",
	"noc_vivobus_ddrc0_wr",
	"noc_vivobus_ddrc1_rd",
	"noc_vivobus_ddrc1_wr"
};


char *vcodec_initflow_array[VCODEC_INITFLOW_ARRAY_SIZE] = {
	"noc_vcodec_cfg",
	"noc_vdec",
	"noc_venc",
};

char *vcodec_targetflow_array[VCODEC_TARGETFLOW_ARRAY_SIZE] = {
	"noc_vcodecbus_ddrc0",
	"noc_vcodecbus_ddrc1",
	"noc_vdec_cfg",
	"noc_venc_cfg",
	"vcodec_bus_service_target",
	"vdec_service_target",
	"venc_service_target",
};

struct noc_bus_info hi3xxx_noc_buses_info[HI3XXX_MAX_NOC_BUSES_NR]= {
	[0] = {
		.name = "cfg_sys_noc_bus",
		.initflow_mask = ((0x1f) << 17),
		.targetflow_mask = ((0x3f) << 11),
		.targ_subrange_mask = ((0x3) << 9),
		.seq_id_mask = (0x1FF),
		.initflow_array = cfg_initflow_array,
		.initflow_array_size = CFG_INITFLOW_ARRAY_SIZE,
		.targetflow_array = cfg_targetflow_array,
		.targetflow_array_size = CFG_TARGETFLOW_ARRAY_SIZE,
	},
	[1] = {
		.name = "vcodec_bus",
		.initflow_mask = ((0x3) << 14),
		.targetflow_mask = ((0x7) << 11),
		.targ_subrange_mask = ((0x7) << 8),
		.seq_id_mask = (0xFF) ,
		.initflow_array = vcodec_initflow_array,
		.initflow_array_size = VCODEC_INITFLOW_ARRAY_SIZE,
		.targetflow_array = vcodec_targetflow_array,
		.targetflow_array_size = VCODEC_TARGETFLOW_ARRAY_SIZE,
	},
	[2] = {
		.name = "vivo_bus",
		.initflow_mask = ((0xf) << 14),
		.targetflow_mask = ((0x7) << 11),
		.targ_subrange_mask = ((0x7) << 8),
		.seq_id_mask = (0xFF),
		.initflow_array = vivo_initflow_array,
		.initflow_array_size = VIVO_INITFLOW_ARRAY_SIZE,
		.targetflow_array = vivo_targetflow_array,
		.targetflow_array_size = VIVO_TARGETFLOW_ARRAY_SIZE,
	}
};

#else

char *cfg_initflow_array[CFG_INITFLOW_ARRAY_SIZE] = {
	"noc_hsic",
	"noc_usb2",
	"noc_mmc1bus_sd3",
	"noc_mmc1bus_sdio0",
	"noc_asp_mst",
	"noc_cci2sysbus",
	"noc_djtag_mst",
	"noc_dmac_mst",
	"noc_emmc",
	"noc_iom3_mst0",
	"noc_iom3_mst1",
	"noc_lpm3_mst",
	"noc_modem_mst",
	"noc_sec_p",
	"noc_sec_s",
	"noc_socp",
	"noc_top_cssys"
};

char *cfg_targetflow_array[CFG_TARGETFLOW_ARRAY_SIZE] = {
	"aspbus_service_target",
	"cfgbus_service_target",
	"dbgbus_service_target",
	"dmabus_service_target",
	"mmc0bus_service_target",
	"mmc1bus_service_target",
	"modembus_service_target",
	"noc_asp_cfg",
	"noc_dmac_cfg",
	"noc_emmc_sdio1_cfg",
	"noc_gic",
	"noc_hkadc_ssi",
	"noc_hsic_cfg",
	"noc_iom3_slv",
	"noc_lpm3_slv",
	"noc_modem_cfg",
	"noc_sd3_sdio0_cfg",
	"noc_sec_p_cfg",
	"noc_sec_s_cfg",
	"noc_socp_cfg",
	"noc_sys2ddrc",
	"noc_sys_peri0_cfg",
	"noc_sys_peri1_cfg",
	"noc_sys_peri2_cfg",
	"noc_sys_peri3_cfg",
	"noc_sysbus2cci",
	"noc_top_cssys_slv",
	"sysbus_service_target"
};

char *vivo_initflow_array[VIVO_INITFLOW_ARRAY_SIZE] = {
	"noc_dss0_rd",
	"noc_dss0_wr",
	"noc_dss1_rd",
	"noc_dss1_wr",
	"noc_isp_p1_rd",
	"noc_isp_p1_wr",
	"noc_isp_p2_rd",
	"noc_isp_p2_wr",
	"noc_isp_p3_rd",
	"noc_isp_p3_wr",
	"noc_isp_p4_rd",
	"noc_vivo_cfg"
};

char *vivo_targetflow_array[VIVO_TARGETFLOW_ARRAY_SIZE] = {
	"dss_service_target",
	"isp_service_target",
	"noc_dss_cfg",
	"noc_isp_cfg",
	"noc_vivobus_ddrc0_rd",
	"noc_vivobus_ddrc0_wr",
	"noc_vivobus_ddrc1_rd",
	"noc_vivobus_ddrc1_wr"
};

char *vcodec_initflow_array[VCODEC_INITFLOW_ARRAY_SIZE] = {
	"noc_jpegcodec",
	"noc_vcodec_cfg",
	"noc_vdec0",
	"noc_venc",
	"noc_vpp"
};

char *vcodec_targetflow_array[VCODEC_TARGETFLOW_ARRAY_SIZE] = {
	"noc_vcodecbus_ddrc",
	"noc_vdec_cfg",
	"noc_venc_cfg",
	"noc_vpp_cfg",
	"vcodec_bus_service_targe",
	"vdec_service_target",
	"venc_service_target",
	"vpp_service_target"
};

struct noc_bus_info hi3xxx_noc_buses_info[HI3XXX_MAX_NOC_BUSES_NR] = {
	[0] = {
		.name = "cfg_sys_noc_bus",
		.initflow_mask = ((0x1f) << 17),
		.targetflow_mask = ((0x1f) << 12),
		.targ_subrange_mask =((0x7) << 9),
		.seq_id_mask = 0,
		.initflow_array = cfg_initflow_array,
		.initflow_array_size = CFG_INITFLOW_ARRAY_SIZE,
		.targetflow_array = cfg_targetflow_array,
		.targetflow_array_size = CFG_TARGETFLOW_ARRAY_SIZE,
	},
	[1] = {
		.name = "vcodec_bus",
		.initflow_mask = ((0x7) << 9),
		.targetflow_mask = ((0x7) << 6),
		.targ_subrange_mask = ((0x1) << 5),
		.seq_id_mask = (0x1f) ,
		.initflow_array = vcodec_initflow_array,
		.initflow_array_size = VCODEC_INITFLOW_ARRAY_SIZE,
		.targetflow_array = vcodec_targetflow_array,
		.targetflow_array_size = VCODEC_TARGETFLOW_ARRAY_SIZE,
	},
	[2] = {
		.name = "vivo_bus",
		.initflow_mask = ((0xf) << 10),
		.targetflow_mask = ((0x7) << 7),
		.targ_subrange_mask = 0,
		.seq_id_mask = 0,
		.initflow_array = vivo_initflow_array,
		.initflow_array_size = VIVO_INITFLOW_ARRAY_SIZE,
		.targetflow_array = vivo_targetflow_array,
		.targetflow_array_size = VIVO_TARGETFLOW_ARRAY_SIZE,
	}
};
#endif 
