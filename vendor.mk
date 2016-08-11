#################################################################################
#
#				P8 Lite
#			    Propietary Blobs
#
#################################################################################





#Overlay
DEVICE_PACKAGE_OVERLAYS := device/HUAWEI/hi6210sft/overlay

#Audio Config
PRODUCT_COPY_FILES += \
device/HUAWEI/hi6210sft/audio/audio_effects.conf:system/etc/audio_effects.conf \
device/HUAWEI/hi6210sft/audio/audio_policy.conf:system/etc/audio_policy.conf \
vendor/lib/libnvme.so:system/lib/libnvme.so

#Bluetooth
PRODUCT_COPY_FILES += \
device/HUAWEI/hi6210sft/bluetooth/auto_pair_devlist.conf:system/etc/bluetooth/auto_pair_devlist.conf \
device/HUAWEI/hi6210sft/bluetooth/bt_did.conf:system/etc/bluetooth/bt_did.conf \
device/HUAWEI/hi6210sft/bluetooth/bt_stack.conf:system/etc/bluetooth/bt_stack.conf


# Graphics
PRODUCT_COPY_FILES += \
		vendor/lib/egl/libGLES_mali.so:system/lib/egl/libGLES_mali.so \
		vendor/lib/egl/libGLES_android.so:system/lib/egl/libGLES_android.so \
		vendor/lib64/egl/libGLES_mali.so:system/lib64/egl/libGLES_mali.so \
		vendor/lib64/egl/libGLES_android.so:system/lib64/egl/libGLES_android.so \
		vendor/lib64/libhwaps.so:system/lib64/libhwaps.so \
		vendor/lib64/libion.so:system/lib64/libion.so

#RIL
PRODUCT_COPY_FILES += \
		vendor/lib64/libbalong_audio_ril.so:system/lib64/libbalong_audio_ril.so \
		vendor/lib/libbalong_audio_ril.so:system/lib/libbalong_audio_ril.so \
		vendor/lib64/libbalong-ril.so:system/lib64/libbalong-ril.so \
		vendor/lib64/libbalong-ril-1.so:system/lib64/libbalong-ril-1.so \
		vendor/lib64/libreference-ril.so:system/lib64/libreference-ril.so \
		vendor/lib64/libril.so:system/lib64/libril.so \
		vendor/lib64/librilutils.so:system/lib64/librilutils.so \
		vendor/bin/rild:system/bin/rild \
		vendor/lib64/liboeminfo.so:system/lib64/liboeminfo.so \
		vendor/bin/modemlogcat_lte:system/bin/modemlogcat_lte \
		vendor/bin/atcmdserver:system/bin/atcmdserver \
		vendor/lib64/libhsa_common.so:system/lib64/libhsa_common.so \
		vendor/lib64/libhsa_configprocess.so:system/lib64/libhsa_configprocess.so \
		vendor/lib64/libhsa_hdlc.so:system/lib64/libhsa_hdlc.so \
		vendor/lib64/libhsa_msgprocess.so:system/lib64/libhsa_msgprocess.so \
		vendor/lib64/libnvme.so:system/lib64/libnvme.so \
		vendor/lib64/libHiSiUEAgent.so:system/lib64/libHiSiUEAgent.so \
		vendor/lib64/libHisiEncClientDll.so:system/lib64/libHisiEncClientDll.so \
		vendor/lib64/libconn_factory_test.so:system/lib64/libconn_factory_test.so \
		vendor/lib64/lib_atprotocolsw.so:system/lib64/lib_atprotocolsw.so \
		vendor/lib64/libhisi_efuse.so:system/lib64/libhisi_efuse.so \
		vendor/phone.prop:system/phone.prop \
		vendor/isp.bin:system/isp.bin \
		vendor/ons.bin:system/ons.bin
		

#Audio Libs
PRODUCT_COPY_FILES += \
		vendor/lib/libaudioflinger.so:system/lib/libaudioflinger.so \
		vendor/lib/libaudioflinger.huawei.so:system/lib/libaudioflinger.hauwei.so \
		vendor/lib/libaudiopolicyenginedefault.so:system/lib/libaudiopolicyenginedefault.so \
		vendor/lib/libaudiopolicyservice.so:system/lib/libaudiopolicyservice.so \
		vendor/lib/libaudiopolicymanager.so:system/lib/libaudiopolicymanager.so \
		vendor/lib/libaudiopolicymanagerdefault.so:system/lib/libaudiopolicymanagerdefault.so \
		vendor/lib/libaudio_custom.so:system/lib/libaudio_custom.so \
		vendor/lib/libaudioalgoservice_jni.so:system/lib/libaudioalgoservice_jni.so \
		vendor/lib/libaudio-resampler.so:system/lib/libaudio-resampler.so \
		vendor/lib/libswsprocessing.so:system/lib/libswsprocessing.so \
		vendor/lib64/libswsprocessing.so:system/lib64/libswsprocessing.so \
		vendor/lib/libsonic.so:system/lib/libsonic.so \
		vendor/lib64/libsonic.so:system/lib64/libsonic.so \
		vendor/lib/hw/audio.primary.hi6210sft.so:system/lib/hw/audio.primary.hi6210sft.so \
		vendor/lib64/hw/audio.primary.hi6210sft.so:system/lib64/hw/audio.primary.hi6210sft.so \
		vendor/lib/libaudioroute.so:system/lib/libaudioroute.so \
		vendor/lib64/libaudioroute.so:system/lib64/libaudioroute.so \
		vendor/lib/hw/hw_af_extend.default.so:system/lib/hw/hw_af_extend.default.so \
		vendor/lib/hw/audio.r_submix.default.so:system/lib/hw/audio.r_submix.so \
		vendor/lib64/hw/audio.r_submix.default.so:system/lib64/hw/audio.r_submix.so \
		vendor/lib/libtfa9895.so:system/lib/libtfa9895.so \
		vendor/lib64/libtfa9895.so:system/lib64/libtfa9895.so \
		vendor/lib/libsrsprocessing.so:system/lib/libsrsprocessing.so \
		vendor/lib/libswsprocessing.so:system/lib/libswsprocessing.so \
		vendor/lib64/libswsprocessing.so:system/lib64/libswsprocessing.so \
		vendor/lib/libbalong_audio_ril.so:system/lib/libbalong_audio_ril.so \
		vendor/lib64/libbalong_audio_ril.so:system/lib64/libbalong_audio_ril.so \
		vendor/lib/libmax98925.so:system/lib/libmax98925.so \
		vendor/lib64/libmax98925.so:system/lib64/libmax98925.so \
		vendor/lib/libOpenMAXAL.so:system/lib/libOpenMAXAL.so \
		vendor/lib64/libOpenMAXAL.so:system/lib64/libOpenMAXAL.so \
		vendor/lib/libOpenSLES.so:system/lib/libOpenSLES.so \
		vendor/lib64/libOpenSLES.so:system/lib64/libOpenSLES.so \
		vendor/lib/libsoundtrigger.so:system/lib/libsoundtrigger.so \
		vendor/lib64/libsoundtrigger.so:system/lib64/libsoundtrigger.so \
		vendor/lib/libsoundtriggerservice.so:system/lib/libsoundtriggerservice.so \
		vendor/lib64/libsoundtriggerservice.so:system/lib64/libsoundtriggerservice.so \
		vendor/lib/libmedia.so:system/lib/libmedia.so \
vendor/lib/libHwExtendedExtractor.so:system/lib/libHwExtendedExtractor.so


#Camera
PRODUCT_COPY_FILES += \
		vendor/lib/libjpegenchw.so:system/lib/libjpegenchw.so \
		vendor/lib/libjpu.so:system/lib/libjpu.so \
		vendor/lib/hw/camera.hi6210sft.so:system/lib/hw/camera.vendor.hi6210sft.so \
		vendor/lib/libcamera_core.so:system/lib/libcamera_core.so \
		vendor/lib/libcamera_client.so:system/lib/libcamera_client.so \
		vendor/lib/libcamera_post_mediaserver.so:system/lib/libcamera_post_mediaserver.so \
		vendor/lib/libcameraservice.so:system/lib/libcameraservice.so \
		vendor/lib/libCameraHwExtend.so:system/lib/libCameraHwExtend.so \
		vendor/etc/camera_orientation.cfg:system/etc/camera_orientation.cfg \
		vendor/etc/camera_resolutions.cfg:system/etc/camera_resolutions.cfg

#Hardware CFG
PRODUCT_COPY_FILES += \
		vendor/lib64/hw/audio.primary.hi6210sft.so:system/lib64/hw/audio.primary.hi6210sft.so \
		vendor/lib/hw/audio.primary.hi6210sft.so:system/lib/hw/audio.primary.hi6210sft.so \
		vendor/lib64/hw/fm.bcm.hi6210sft.so:system/lib64/hw/fm.bcm.hi6210sft.so \
		vendor/lib64/hw/fm.hi110x.hi6210sft.so:system/lib64/hw/fm.hi110x.hi6210sft.so \
		vendor/lib64/hw/gatekeeper.hi6210sft.so:system/lib64/hw/gatekeeper.hi6210sft.so \
		vendor/lib64/hw/keystore.hi6210sft.so:system/lib64/hw/keystore.hi6210sft.so \
		vendor/lib64/hw/lights.default.so:system/lib64/hw/lights.default.so \
		vendor/lib64/hw/memtrack.hi6210sft.so:system/lib64/hw/memtrack.hi6210sft.so \
		vendor/lib/hw/memtrack.hi6210sft.so:system/lib/hw/memtrack.hi6210sft.so \
		vendor/lib64/libtfa9895.so:system/lib64/libtfa9895.so \
		vendor/lib/hw/hw_af_extend.default.so:system/lib/hw/hw_af_extend.default.so

#Media Libs
PRODUCT_COPY_FILES += \
		vendor/lib/libexpat.so:system/lib/libexpat.so \
		vendor/lib64/libexpat.so:system/lib64/libexpat.so \
		vendor/lib/libstagefrighthw.so:system/lib/libstagefrighthw.so \
		vendor/lib64/libstagefrighthw.so:system/lib64/libstagefrighthw.so \
		vendor/lib/libmediandk.so:system/lib/libmediandk.so \
		vendor/lib/libstagefright_omx.so:system/lib/libstagefright_omx.so \
		vendor/lib64/libstagefright_omx.so:system/lib64/libstagefright_omx.so

#Misc stuff
PRODUCT_COPY_FILES += \
		vendor/lib64/libteec.so:system/lib64/libteec.so \
		vendor/lib64/libaudioroute.so:system/lib64/libaudioroute.so \
		vendor/lib/libaudioroute.so:system/lib/libaudioroute.so \
		vendor/bin/keystore:system/bin/keystore \
		vendor/lib64/libfaclog.so:system/lib64/libfaclog.so \
		vendor/lib64/libchrlog.so:system/lib64/libchrlog.so \
		vendor/lib64/libhisi_ini.so:system/lib64/libhisi_ini.so

#Audio Codecs (Don't worry, these are just text files)
PRODUCT_COPY_FILES += \
		device/HUAWEI/hi6210sft/audio/audio_effects.conf:system/etc/audio_effects.conf \
		device/HUAWEI/hi6210sft/audio/audio_policy.conf:system/etc/audio_policy.conf \
		vendor/etc/media_codecs_google_audio.xml:system/etc/media_codecs_google_audio.xml \
		vendor/etc/media_codecs_google_telephony.xml:system/etc/media_codecs_google_telephony.xml \
		vendor/etc/media_codecs_performance.xml:system/etc/media_codecs_performance.xml \
		vendor/etc/audio/algorithm/algorithm_ALICEPA_normal.xml:system/etc/audio/algorithm/algorithm_ALICEPA_normal.xml \
		vendor/etc/audio/algorithm/ascend_algorithm_default.xml:system/etc/audio/algorithm/ascend_algorithm_default.xml \
		vendor/etc/audio/dts/dts_ALICEPA_normal.xml:system/etc/audio/dts/dts_ALICEPA_normal.xml \
		vendor/etc/audio/dts/dts_default.xml:system/etc/audio/dts/dts_default.xml \
		vendor/etc/audio/hissc/mixer_paths_default.xml:system/etc/audio/hissc/mixer_paths_default.xml \
		vendor/etc/audio/hissc/mixer_volumes_default.xml:system/etc/audio/hissc/mixer_volumes_default.xml \
		vendor/etc/audio/hissc/pop_seq_default.xml:system/etc/audio/hissc/pop_seq_default.xml \
		vendor/etc/audio/hissc/mixer_paths_ALICEPA.xml:system/etc/audio/hissc/mixer_paths_ALICEPA.xml \
		vendor/etc/audio/hissc/mixer_volumes_ALICEPA_ce.xml:system/etc/audio/hissc/mixer_volumes_ALICEPA_ce.xml \
		vendor/etc/audio/hissc/mixer_volumes_ALICEPA_cmcciot.xml:system/etc/audio/hissc/mixer_volumes_ALICEPA_cmcciot.xml \
		vendor/etc/audio/hissc/mixer_volumes_ALICEPA_normal.xml:system/etc/audio/hissc/mixer_volumes_ALICEPA_normal.xml \
		vendor/etc/audio/hissc/mixer_volumes_ALICEPA_telecomiot.xml:system/etc/audio/hissc/mixer_volumes_ALICEPA_telecomiot.xml \
		vendor/etc/audio/hissc/mixer_volumes_ALICEPA_unicomiot.xml:system/etc/audio/hissc/mixer_volumes_ALICEPA_unicomiot.xml \
		vendor/etc/audio/hissc/pop_seq_ALICEPA.xml:system/etc/audio/hissc/pop_seq_ALICEPA.xml \
		vendor/etc/audio/mbdrc/mbdrc_ALICEPA_normal.xml:system/etc/audio/mbdrc/mbdrc_ALICEPA_normal.xml \
		vendor/etc/audio/mbdrc/mbdrc_default.xml:system/etc/audio/mbdrc/mbdrc_default.xml \
		vendor/etc/audio/modem/modem_default.xml:system/etc/audio/modem/modem_default.xml \
		vendor/etc/audio/modem/modem_ALICEPA_cmcciot.xml:system/etc/audio/modem/modem_ALICEPA_cmcciot.xml \
		vendor/etc/audio/modem/modem_ALICEPA_normal.xml:system/etc/audio/modem/modem_ALICEPA_normal.xml \
		vendor/etc/audio/modem/modem_ALICEPA_telecomiot.xml:system/etc/audio/modem/modem_ALICEPA_telecomiot.xml \
		vendor/etc/audio/modem/modem_ALICEPA_unicomiot.xml:system/etc/audio/modem/modem_ALICEPA_unicomiot.xml \
		vendor/etc/audio/nxp/Tfa9895_ALICEPA.cnt:system/etc/audio/nxp/Tfa9895_ALICEPA.cnt \
		vendor/etc/audio/nxp/Tfa9895_ALICEPA_coefficient.config:system/etc/audio/nxp/Tfa9895_ALICEPA_coefficient.config \
		vendor/etc/audio/nxp/Tfa9895_default.cnt:system/etc/audio/nxp/Tfa9895_default.cnt \
		vendor/etc/audio/sws/sws_ALICEPA_normal.xml:system/etc/audio/sws/sws_ALICEPA_normal.xml \
		vendor/etc/audio/sws/sws_default.xml:system/etc/audio/sws/sws_default.xml \
		vendor/etc/sws/sws_config.txt:system/etc/sws/sws_config.txt \
		vendor/etc/srs/srsmodels.lic:system/etc/srs/srsmodels.lic \
		vendor/etc/srs/srs_processing.cfg:system/etc/srs/srs_processing.cfg \
		vendor/etc/srs/models.txt:system/etc/srs/models.txt \
		vendor/etc/hisi_omx.cfg:system/etc/hisi_omx.cfg

#OMX & k3 Codecs
PRODUCT_COPY_FILES += \
		vendor/lib/lib_k3_ffmpeg.so:system/lib/lib_k3_ffmpeg.so \
		vendor/lib64/lib_k3_ffmpeg.so:system/lib64/lib_k3_ffmpeg.so \
		vendor/lib/lib_k3_omx_avc.so:system/lib/lib_k3_omx_avc.so \
		vendor/lib64/lib_k3_omx_avc.so:system/lib64/lib_k3_omx_avc.so \
		vendor/lib/lib_k3_omx_avcenc.so:system/lib/lib_k3_omx_avcenc.so \
		vendor/lib64/lib_k3_omx_avcenc.so:system/lib64/lib_k3_omx_avcenc.so \
		vendor/lib/lib_k3_omx_mpeg2.so:system/lib/lib_k3_omx_mpeg2.so \
		vendor/lib64/lib_k3_omx_mpeg2.so:system/lib64/lib_k3_omx_mpeg2.so \
		vendor/lib/lib_k3_omx_mpeg4.so:system/lib/lib_k3_omx_mpeg4.so \
		vendor/lib64/lib_k3_omx_mpeg4.so:system/lib64/lib_k3_omx_mpeg4.so \
		vendor/lib/lib_k3_omx_rv.so:system/lib/lib_k3_omx_rv.so \
		vendor/lib64/lib_k3_omx_rv.so:system/lib64/lib_k3_omx_rv.so \
		vendor/lib/lib_k3_omx_vc1.so:system/lib/lib_k3_omx_vc1.so \
		vendor/lib64/lib_k3_omx_vc1.so:system/lib64/lib_k3_omx_vc1.so \
		vendor/lib/lib_k3_omx_vp8.so:system/lib/lib_k3_omx_vp8.so \
		vendor/lib64/lib_k3_omx_vp8.so:system/lib64/lib_k3_omx_vp8.so \
		vendor/lib/lib_k3_omxcore.so:system/lib/lib_k3_omxcore.so \
		vendor/lib64/lib_k3_omxcore.so:system/lib64/lib_k3_omxcore.so

#Wifi libs
PRODUCT_COPY_FILES += \
		vendor/lib64/libwifipro.so:system/lib64/libwifipro.so \
		vendor/bin/wpa_supplicant_hisi:system/bin/wpa_supplicant_hisi \
		vendor/bin/wpa_supplicant:system/bin/wpa_supplicant \
		vendor/bin/wpa_cli_hisi:system/bin/wpa_cli_hisi \
		vendor/bin/wpa_cli:system/bin/wpa_cli \
		vendor/bin/hisi_connectivity.sh:system/bin/hisi_connectivity.sh \
		vendor/bin/hostapd_hisi:system/bin/hostapd_hisi \
		vendor/bin/hostapd_cli_hisi:system/bin/hostapd_cli_hisi \
		vendor/bin/dhcpcd:system/bin/dhcpcd

#Wifi Config
PRODUCT_COPY_FILES += \
		vendor/etc/wifi/hostapd_hisi.conf:system/etc/wifi/hostapd_hisi.conf \
		vendor/etc/wifi/p2p_supplicant_overlay.conf:system/etc/wifi/p2p_supplicant:overlay.conf \
		vendor/etc/wifi/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
		vendor/etc/wifi/wpa_supplicant_hisi.conf:system/etc/wifi/wpa_supplicant_hisi.conf \
		vendor/etc/wifi/wpa_supplicant_overlay.conf:system/etc/wifi/wpa_supplicant_overlay.conf

#Firmware
PRODUCT_COPY_FILES += \
		vendor/vendor/firmware/4343A0_001.001.034.0056.0193_ORC.hcd:system/vendor/firmware/4343A0_001.001.034.0056.0193_ORC.hcd\
			vendor/vendor/firmware/BCM4334B0_002.001.013.1554.1613_RC.hcd:system/vendor/firmware/BCM4334B0_002.001.013.1554.1613_RC.hcd\
			vendor/vendor/firmware/fw_bcm4343s_apsta_hw.bin:system/vendor/firmware/fw_bcm4343s_apsta_hw.bin\
			vendor/vendor/firmware/fw_bcm4343s_hw.bin:system/vendor/firmware/fw_bcm4343s_hw.bin\
			vendor/vendor/firmware/fw_bcm4343s_p2p_hw.bin:system/vendor/firmware/fw_bcm4343s_p2p_hw.bin\
			vendor/vendor/firmware/fw_bcm4343s_test_hw_apsta.bin:system/vendor/firmware/fw_bcm4343s_test_hw_apsta.bin\
			vendor/vendor/firmware/GNSS.bin:system/vendor/firmware/GNSS.bin\
			vendor/vendor/firmware/libpn547_fw.so:system/vendor/firmware/libpn547_fw.so\
			vendor/vendor/firmware/nvram4343s_hw.txt:system/vendor/firmware/nvram4343s_hw.txt\
			vendor/vendor/firmware/nvram_CHERRY_PLUS_TL00.txt:system/vendor/firmware/nvram_CHERRY_PLUS_TL00.txt\
			vendor/vendor/firmware/nvram_CHERRY_PLUS_TL20M.txt:system/vendor/firmware/nvram_CHERRY_PLUS_TL20M.txt\
			vendor/vendor/firmware/nvram_CHERRY_PLUS_UL00.txt:system/vendor/firmware/nvram_CHERRY_PLUS_UL00.txt\
			vendor/vendor/firmware/sdio_cfg:system/vendor/firmware/sdio_cfg\
			vendor/vendor/firmware/SDIO_CMP.bin:system/vendor/firmware/SDIO_CMP.bin\
			vendor/vendor/firmware/SDIO_COD.bin:system/vendor/firmware/SDIO_COD.bin\
			vendor/vendor/firmware/SDIO_RW.bin:system/vendor/firmware/SDIO_RW.bin\
			vendor/vendor/firmware/SDIO_RW_ALICE_TL00_FEM.bin:system/vendor/firmware/SDIO_RW_ALICE_TL00_FEM.bin


#Hardware Packages
PRODUCT_PACKAGES += \
audio.primary.default \
audio_policy.stub \
audio.a2dp.default \
audio.usb.default \
audio.r_submix.default \
libaudioutils \
libtinyalsa \
tinyplay \
tinycap \
tinymix \
tinypcminfo \
sound_trigger.primary.hi6210sft \
libion.huawei
