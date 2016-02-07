dtb-y += hi6220_cs_udp_dsds_ddr3_config.dtb
dtb-y += hi6220_cs_udp_ddr2_config.dtb
dtb-y += hi6220_cs_udp_fhd_ddr3_config.dtb
dtb-y += hi6220_cs_udp_ddr3_config.dtb
dtb-y += hi6220_sft_mali450_config.dtb
dtb-y += hi6220_sft_mali400_config.dtb
dtb-y += hi6220_cs_udp_dsds_ddr2_config.dtb
dtb-y += hi6220_cs_udp_dsds_fhd_ddr3_config.dtb

targets += dtbs
dtbs: $(addprefix $(obj)/, $(dtb-y))
	$(Q)rm -f $(obj)/../*.dtb

clean-files := *.dtb
