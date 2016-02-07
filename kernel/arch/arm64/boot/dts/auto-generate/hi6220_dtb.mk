#Copyright Huawei Technologies Co., Ltd. 1998-2011. All rights reserved.
#This file is Auto Generated 

dtb-y += hi6220/SHINE_UL10_VA_config.dtb
dtb-y += hi6220/SHINE_UL13_VA_config.dtb
dtb-y += hi6220/SHINE_UL03_VA_config.dtb
dtb-y += hi6220/SHINE_TL10_VA_config.dtb
dtb-y += hi6220/SHINE_UL01_VA_config.dtb
dtb-y += hi6220/CHERRY_PRO_TL10_VB_config.dtb
dtb-y += hi6220/CHERRY_PRO_TL10_VA_config.dtb
dtb-y += hi6220/CHERRY_PRO_TL00_VB_config.dtb
dtb-y += hi6220/CHERRY_PRO_TL10_VC_config.dtb
dtb-y += hi6220/CHERRY_PRO_TL00_VA_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_TL00_VC_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_TL10M_VB_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_LS00_VB_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_ED00_VB_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_ID00_VB_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_TL00M_VB_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_TL10_VB_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_UL00_VC_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_UL10_VB_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_TL00M_VC_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_AS00_VB_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_TL10M_VC_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_LD00_VB_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_TL00_VB_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_TL10_VC_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_UL10_VC_config.dtb
dtb-y += hi6220/CHERRY_PLUS_R2_UL00_VB_config.dtb
dtb-y += hi6220/ALICE_TL02_VA_config.dtb
dtb-y += hi6220/ALICE_UL00_VA_config.dtb
dtb-y += hi6220/ALICE_TL02_VB_config.dtb
dtb-y += hi6220/ALICE_TL21_VA_config.dtb
dtb-y += hi6220/ALICE_TL23_VB_config.dtb
dtb-y += hi6220/ALICE_TL21_VB_config.dtb
dtb-y += hi6220/ALICE_TL21_VC_config.dtb
dtb-y += hi6220/ALICE_TL23_VC_config.dtb
dtb-y += hi6220/ALICE_TL02_VC_config.dtb
dtb-y += hi6220/ALICE_TL00_VB_config.dtb
dtb-y += hi6220/ALICE_UL00_VB_config.dtb
dtb-y += hi6220/ALICE_TL00_VA_config.dtb
dtb-y += hi6220/ALICE_TL23_VA_config.dtb
dtb-y += hi6220/hi6220_cs_udp_dsds_ddr3_config.dtb
dtb-y += hi6220/hi6220_cs_udp_ddr2_config.dtb
dtb-y += hi6220/hi6220_cs_udp_fhd_ddr3_config.dtb
dtb-y += hi6220/hi6220_cs_udp_ddr3_config.dtb
dtb-y += hi6220/hi6220_sft_mali450_config.dtb
dtb-y += hi6220/hi6220_sft_mali400_config.dtb
dtb-y += hi6220/hi6220_cs_udp_dsds_ddr2_config.dtb
dtb-y += hi6220/hi6220_cs_udp_dsds_fhd_ddr3_config.dtb
dtb-y += hi6220/hisi_pilot_TL10_VB_config.dtb

targets += hi6220_dtb
targets += $(dtb-y)

# *.dtb used to be generated in the directory above. Clean out the
# old build results so people don't accidentally use them.
hi6220_dtb: $(addprefix $(obj)/, $(dtb-y))
	$(Q)rm -f $(obj)/../*.dtb

clean-files := *.dtb

#end of file
