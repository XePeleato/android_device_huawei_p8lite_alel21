#################################################################################
#
#				P8 Lite
#			    Propietary Blobs
#
#################################################################################





#Overlay
#DEVICE_PACKAGE_OVERLAYS := device/HUAWEI/hi6210sft/overlay

#Ramdisk
PRODUCT_COPY_FILES += \
		device/linaro/hi6210sft/rootdir/fstab.hi6210sft:root/fstab.hi6210sft \
		device/linaro/hi6210sft/rootdir/init.connectivity.rc:root/init.connectivity.rc \
		device/linaro/hi6210sft/rootdir/init.hi6210sft.rc:root/init.hi6210sft.rc \
		device/linaro/hi6210sft/rootdir/init.hwconfig.rc:root/init.hwconfig.rc \
		device/linaro/hi6210sft/rootdir/ueventd.hi6210sft.rc:root/ueventd.hi6210sft.rc \
		device/linaro/hi6210sft/rootdir/sbin/oeminfo_nvm_server:root/sbin/oeminfo_nvm_server \
		device/linaro/hi6210sft/rootdir/sbin/volisnotd:root/sbin/volisnotd \
		device/linaro/hi6210sft/rootdir/sbin/teecd:root/sbin/teecd \

#Audio Config
PRODUCT_COPY_FILES += \
vendor/etc/audio_effects.conf:system/etc/audio_effects.conf \
vendor/etc/audio_policy.conf:system/etc/audio_policy.conf \
device/linaro/hi6210sft/kernel:kernel

#Media codecs
PRODUCT_COPY_FILES += \
		vendor/hi6210sft/etc/media_codecs.xml:system/etc/media_codecs.xml \
		vendor/hi6210sft/etc/media_profiles.xml:system/etc/media_codecs.xml

#Bluetooth
PRODUCT_COPY_FILES += \
		vendor/hi6210sft/lib/hw/bluetooth.default.so:system/lib/hw/bluetooth.default.so \
		vendor/hi6210sft/vendor/lib/libbt-vendor-hi110x.so:system/vendor/lib/libbt-vendor-hi110x.so \
		vendor/hi6210sft/vendor/lib/libbt-vendor.so:system/vendor/lib/libbt-vendor.so \
		vendor/hi6210sft/vendor/lib/libbt-vendor-hi110x.so:system/vendor/lib/libbt-vendor-hi110x.so \
		vendor/hi6210sft/etc/bluetooth/auto_pair_devlist.conf:system/etc/bluetooth/auto_pair_devlist.conf \
		vendor/hi6210sft/etc/bluetooth/bt_did.conf:system/etc/bluetooth/bt_did.conf \
		vendor/hi6210sft/etc/bluetooth/bt_stack.conf:system/etc/bluetooth/bt_stack.conf \
		vendor/hi6210sft/etc/bluetooth/bt_stack_beta.conf:system/etc/bluetooth/bt_stack_beta.conf \
		vendor/hi6210sft/etc/bluetooth/bt_stack_log.conf:system/etc/bluetooth/bt_stack_log.conf \
		vendor/hi6210sft/etc/bluetooth/bt_vendor.conf:system/etc/bluetooth/bt_vendor.conf \
		vendor/hi6210sft/lib/libbluetoothex_jni.so:system/lib/libbluetoothex_jni.so \
		vendor/hi6210sft/lib64/libbluetoothex_jni.so:system/lib64/libbluetoothex_jni.so

# Graphics
PRODUCT_COPY_FILES += \
		vendor/hi6210sft/lib/egl/libGLES_mali.so:system/lib/egl/libGLES_mali.so \
		vendor/hi6210sft/lib/egl/libGLES_android.so:system/lib/egl/libGLES_android.so \
		vendor/hi6210sft/lib64/egl/libGLES_mali.so:system/lib64/egl/libGLES_mali.so \
		vendor/hi6210sft/lib64/egl/libGLES_android.so:system/lib64/egl/libGLES_android.so \
		vendor/hi6210sft/lib64/hw/gralloc.hi6210sft.so:system/lib64/hw/gralloc.hi6210sft.so \
                vendor/hi6210sft/lib/hw/gralloc.hi6210sft.so:system/lib/hw/gralloc.hi6210sft.so \
		vendor/hi6210sft/lib/libion.so:system/lib/libion.so \
		vendor/hi6210sft/lib64/libion.so:system/lib64/libion.so

#Sensors
PRODUCT_COPY_FILES += \
		vendor/hi6210sft/lib/hw/sensors.default.so:system/lib/hw/sensors.default.so \
		vendor/hi6210sft/lib64/hw/sensors.default.so:system/lib64/hw/sensors.default.so \
		vendor/hi6210sft/lib/hw/sensorcaps.default.so:system/lib/hw/sensorcaps.default.so \
		vendor/hi6210sft/lib64/hw/sensorcaps.default.so:system/lib64/hw/sensorcaps.default.so \
		vendor/hi6210sft/lib64/hw/lights.default.so:system/lib64/hw/lights.hi6210sft.so \
		vendor/hi6210sft/vendor/lib/libcalmodule_akm.so:system/vendor/lib/libcalmodule_akm.so \
		vendor/hi6210sft/vendor/lib64/libcalmodule_akm.so:system/vendor/lib64/libcalmodule_akm.so

#RIL
PRODUCT_COPY_FILES += \
		vendor/hi6210sft/lib64/libbalong_audio_ril.so:system/lib64/libbalong_audio_ril.so \
		vendor/hi6210sft/lib/libbalong_audio_ril.so:system/lib/libbalong_audio_ril.so \
		vendor/hi6210sft/lib64/libbalong-ril.so:system/lib64/libbalong-ril.so \
		vendor/hi6210sft/lib64/libbalong-ril-1.so:system/lib64/libbalong-ril-1.so \
		vendor/hi6210sft/lib64/libreference-ril.so:system/lib64/libreference-ril.so \
		vendor/hi6210sft/lib64/libril.so:system/lib64/libril.so \
		vendor/hi6210sft/lib64/librilutils.so:system/lib64/librilutils.so \
		vendor/hi6210sft/bin/rild:system/bin/rild \
		vendor/hi6210sft/lib64/liboeminfo.so:system/lib64/liboeminfo.so \
		vendor/hi6210sft/bin/modemlogcat_lte:system/bin/modemlogcat_lte \
		vendor/hi6210sft/bin/atcmdserver:system/bin/atcmdserver \
		vendor/hi6210sft/lib64/libhsa_common.so:system/lib64/libhsa_common.so \
		vendor/hi6210sft/lib64/libhsa_configprocess.so:system/lib64/libhsa_configprocess.so \
		vendor/hi6210sft/lib64/libhsa_hdlc.so:system/lib64/libhsa_hdlc.so \
		vendor/hi6210sft/lib64/libhsa_msgprocess.so:system/lib64/libhsa_msgprocess.so \
		vendor/hi6210sft/lib64/libnvme.so:system/lib64/libnvme.so \
		vendor/hi6210sft/lib/libnvme.so:system/lib/libnvme.so \
		vendor/hi6210sft/lib64/libHiSiUEAgent.so:system/lib64/libHiSiUEAgent.so \
		vendor/hi6210sft/lib64/libHisiEncClientDll.so:system/lib64/libHisiEncClientDll.so \
		vendor/hi6210sft/lib64/libconn_factory_test.so:system/lib64/libconn_factory_test.so \
		vendor/hi6210sft/lib64/lib_atprotocolsw.so:system/lib64/lib_atprotocolsw.so \
		vendor/hi6210sft/lib64/libhisi_efuse.so:system/lib64/libhisi_efuse.so \
		vendor/hi6210sft/phone.prop:system/phone.prop \
		vendor/hi6210sft/isp.bin:system/isp.bin \
		vendor/hi6210sft/ons.bin:system/ons.bin
		

#RIL config
PRODUCT_COPY_FILES += \
		vendor/hi6210sft/etc/modemConfig/balong/FieldTest.cfg:system/etc/modemConfig/balong/FieldTest.cfg \
		vendor/hi6210sft/etc/modemConfig/balong/basic_name:system/etc/modemConfig/balong/basic_name \
		vendor/hi6210sft/etc/modemConfig/hids_ue_msg.cfg:system/etc/modemConfig/hids_ue_msg.cfg \
		vendor/hi6210sft/etc/modemConfig/hua.cfg:system/etc/modemConfig/hua.cfg \
		device/linaro/hi6210sft/etc/rild.rc:system/etc/init/rild.rc

#Audio Libs
PRODUCT_COPY_FILES += \
		vendor/hi6210sft/lib/libaudioflinger.huawei.so:system/lib/libaudioflinger.hauwei.so \
		vendor/hi6210sft/lib/libaudio_custom.so:system/lib/libaudio_custom.so \
		vendor/hi6210sft/lib/libaudioalgoservice_jni.so:system/lib/libaudioalgoservice_jni.so \
		vendor/hi6210sft/lib/libswsprocessing.so:system/lib/libswsprocessing.so \
		vendor/hi6210sft/lib64/libswsprocessing.so:system/lib64/libswsprocessing.so \
		vendor/hi6210sft/lib/hw/audio.primary.hi6210sft.so:system/lib/hw/audio.primary.hi6210sft.so \
		vendor/hi6210sft/lib64/hw/audio.primary.hi6210sft.so:system/lib64/hw/audio.primary.hi6210sft.so \
		vendor/hi6210sft/lib/hw/hw_af_extend.default.so:system/lib/hw/hw_af_extend.default.so \
		vendor/hi6210sft/lib/hw/audio.r_submix.default.so:system/lib/hw/audio.r_submix.so \
		vendor/hi6210sft/lib64/hw/audio.r_submix.default.so:system/lib64/hw/audio.r_submix.so \
		vendor/hi6210sft/lib/libtfa9895.so:system/lib/libtfa9895.so \
		vendor/hi6210sft/lib64/libtfa9895.so:system/lib64/libtfa9895.so \
		vendor/hi6210sft/lib/libsrsprocessing.so:system/lib/libsrsprocessing.so \
		vendor/hi6210sft/lib/libswsprocessing.so:system/lib/libswsprocessing.so \
		vendor/hi6210sft/lib64/libswsprocessing.so:system/lib64/libswsprocessing.so \
		vendor/hi6210sft/lib/libbalong_audio_ril.so:system/lib/libbalong_audio_ril.so \
		vendor/hi6210sft/lib64/libbalong_audio_ril.so:system/lib64/libbalong_audio_ril.so \
		vendor/hi6210sft/lib/libmax98925.so:system/lib/libmax98925.so \
		vendor/hi6210sft/lib64/libmax98925.so:system/lib64/libmax98925.so \
		vendor/hi6210sft/lib/libOpenMAXAL.so:system/lib/libOpenMAXAL.so \
		vendor/hi6210sft/lib64/libOpenMAXAL.so:system/lib64/libOpenMAXAL.so \
		vendor/hi6210sft/lib/libOpenSLES.so:system/lib/libOpenSLES.so \
		vendor/hi6210sft/lib64/libOpenSLES.so:system/lib64/libOpenSLES.so \
		vendor/hi6210sft/lib/libsoundtrigger.so:system/lib/libsoundtrigger.so \
		vendor/hi6210sft/lib64/libsoundtrigger.so:system/lib64/libsoundtrigger.so \
		vendor/hi6210sft/lib/libsoundtriggerservice.so:system/lib/libsoundtriggerservice.so \
		vendor/hi6210sft/lib64/libsoundtriggerservice.so:system/lib64/libsoundtriggerservice.so \

#Camera
PRODUCT_COPY_FILES += \
		vendor/hi6210sft/lib/libjpegenchw.so:system/lib/libjpegenchw.so \
		vendor/hi6210sft/lib/libjpu.so:system/lib/libjpu.so \
		vendor/hi6210sft/lib/hw/camera.hi6210sft.so:system/lib/hw/camera.hi6210sft.so \
		vendor/hi6210sft/lib64/hw/camera.hi6210sft.so:system/lib64/hw/camera.hi6210sft.so \
		vendor/hi6210sft/lib/libCameraHwExtend.so:system/lib/libCameraHwExtend.so \
		vendor/hi6210sft/lib/libcamera_core.so:system/lib/libcamera_core.so \
		vendor/hi6210sft/etc/camera_orientation.cfg:system/etc/camera_orientation.cfg \
		vendor/hi6210sft/etc/camera_resolutions.cfg:system/etc/camera_resolutions.cfg

#Hardware CFG
#PRODUCT_COPY_FILES += \
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
		vendor/lib/hw/hw_af_extend.default.so:system/lib/hw/hw_af_extend.default.so \
		vendor/lib/hw/sensors.default.so:system/lib/hw/sensors.default.so \
		vendor/lib64/hw/sensors.default.so:system/lib64/hw/sensors.default.so
#Media Libs
#PRODUCT_COPY_FILES += \
		vendor/lib/libexpat.so:system/lib/libexpat.so \
		vendor/lib64/libexpat.so:system/lib64/libexpat.so \
		vendor/lib/libstagefrighthw.so:system/lib/libstagefrighthw.so \
		vendor/lib64/libstagefrighthw.so:system/lib64/libstagefrighthw.so \
		vendor/lib/libmediandk.so:system/lib/libmediandk.so \
		vendor/lib/libstagefright_omx.so:system/lib/libstagefright_omx.so \
		vendor/lib64/libstagefright_omx.so:system/lib64/libstagefright_omx.so

#Misc stuff
PRODUCT_COPY_FILES += \
		vendor/hi6210sft/lib64/libteec.so:system/lib64/libteec.so \
		vendor/hi6210sft/lib64/libaudioroute.so:system/lib64/libaudioroute.so \
		vendor/hi6210sft/lib/libaudioroute.so:system/lib/libaudioroute.so \
		vendor/hi6210sft/lib64/libfaclog.so:system/lib64/libfaclog.so \
		vendor/hi6210sft/lib64/libchrlog.so:system/lib64/libchrlog.so \
		vendor/hi6210sft/lib64/libhisi_ini.so:system/lib64/libhisi_ini.so

#Audio Codecs (Don't worry, these are just text files)
PRODUCT_COPY_FILES += \
		device/HUAWEI/hi6210sft/audio/audio_effects.conf:system/etc/audio_effects.conf \
		device/HUAWEI/hi6210sft/audio/audio_policy.conf:system/etc/audio_policy.conf \
		vendor/hi6210sft/etc/media_codecs_google_audio.xml:system/etc/media_codecs_google_audio.xml \
		vendor/hi6210sft/etc/media_codecs_google_telephony.xml:system/etc/media_codecs_google_telephony.xml \
		vendor/hi6210sft/etc/media_codecs_performance.xml:system/etc/media_codecs_performance.xml \
		vendor/hi6210sft/etc/audio/algorithm/algorithm_ALICEPA_normal.xml:system/etc/audio/algorithm/algorithm_ALICEPA_normal.xml \
		vendor/hi6210sft/etc/audio/algorithm/ascend_algorithm_default.xml:system/etc/audio/algorithm/ascend_algorithm_default.xml \
		vendor/hi6210sft/etc/audio/dts/dts_ALICEPA_normal.xml:system/etc/audio/dts/dts_ALICEPA_normal.xml \
		vendor/hi6210sft/etc/audio/dts/dts_default.xml:system/etc/audio/dts/dts_default.xml \
		vendor/hi6210sft/etc/audio/hissc/mixer_paths_default.xml:system/etc/audio/hissc/mixer_paths_default.xml \
		vendor/hi6210sft/etc/audio/hissc/mixer_volumes_default.xml:system/etc/audio/hissc/mixer_volumes_default.xml \
		vendor/hi6210sft/etc/audio/hissc/pop_seq_default.xml:system/etc/audio/hissc/pop_seq_default.xml \
		vendor/hi6210sft/etc/audio/hissc/mixer_paths_ALICEPA.xml:system/etc/audio/hissc/mixer_paths_ALICEPA.xml \
		vendor/hi6210sft/etc/audio/hissc/mixer_volumes_ALICEPA_ce.xml:system/etc/audio/hissc/mixer_volumes_ALICEPA_ce.xml \
		vendor/hi6210sft/etc/audio/hissc/mixer_volumes_ALICEPA_cmcciot.xml:system/etc/audio/hissc/mixer_volumes_ALICEPA_cmcciot.xml \
		vendor/hi6210sft/etc/audio/hissc/mixer_volumes_ALICEPA_normal.xml:system/etc/audio/hissc/mixer_volumes_ALICEPA_normal.xml \
		vendor/hi6210sft/etc/audio/hissc/mixer_volumes_ALICEPA_telecomiot.xml:system/etc/audio/hissc/mixer_volumes_ALICEPA_telecomiot.xml \
		vendor/hi6210sft/etc/audio/hissc/mixer_volumes_ALICEPA_unicomiot.xml:system/etc/audio/hissc/mixer_volumes_ALICEPA_unicomiot.xml \
		vendor/hi6210sft/etc/audio/hissc/pop_seq_ALICEPA.xml:system/etc/audio/hissc/pop_seq_ALICEPA.xml \
		vendor/hi6210sft/etc/audio/mbdrc/mbdrc_ALICEPA_normal.xml:system/etc/audio/mbdrc/mbdrc_ALICEPA_normal.xml \
		vendor/hi6210sft/etc/audio/mbdrc/mbdrc_default.xml:system/etc/audio/mbdrc/mbdrc_default.xml \
		vendor/hi6210sft/etc/audio/modem/modem_default.xml:system/etc/audio/modem/modem_default.xml \
		vendor/hi6210sft/etc/audio/modem/modem_ALICEPA_cmcciot.xml:system/etc/audio/modem/modem_ALICEPA_cmcciot.xml \
		vendor/hi6210sft/etc/audio/modem/modem_ALICEPA_normal.xml:system/etc/audio/modem/modem_ALICEPA_normal.xml \
		vendor/hi6210sft/etc/audio/modem/modem_ALICEPA_telecomiot.xml:system/etc/audio/modem/modem_ALICEPA_telecomiot.xml \
		vendor/hi6210sft/etc/audio/modem/modem_ALICEPA_unicomiot.xml:system/etc/audio/modem/modem_ALICEPA_unicomiot.xml \
		vendor/hi6210sft/etc/audio/nxp/Tfa9895_ALICEPA.cnt:system/etc/audio/nxp/Tfa9895_ALICEPA.cnt \
		vendor/hi6210sft/etc/audio/nxp/Tfa9895_ALICEPA_coefficient.config:system/etc/audio/nxp/Tfa9895_ALICEPA_coefficient.config \
		vendor/hi6210sft/etc/audio/nxp/Tfa9895_default.cnt:system/etc/audio/nxp/Tfa9895_default.cnt \
		vendor/hi6210sft/etc/audio/sws/sws_ALICEPA_normal.xml:system/etc/audio/sws/sws_ALICEPA_normal.xml \
		vendor/hi6210sft/etc/audio/sws/sws_default.xml:system/etc/audio/sws/sws_default.xml \
		vendor/hi6210sft/etc/sws/sws_config.txt:system/etc/sws/sws_config.txt \
		vendor/hi6210sft/etc/srs/srsmodels.lic:system/etc/srs/srsmodels.lic \
		vendor/hi6210sft/etc/srs/srs_processing.cfg:system/etc/srs/srs_processing.cfg \
		vendor/hi6210sft/etc/srs/models.txt:system/etc/srs/models.txt \
		vendor/hi6210sft/etc/hisi_omx.cfg:system/etc/hisi_omx.cfg

#OMX & k3 Codecs
#PRODUCT_COPY_FILES += \
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
		vendor/hi6210sft/lib64/libwifipro.so:system/lib64/libwifipro.so \
		vendor/hi6210sft/lib64/libwpa_client_hisi.so:system/lib64/libwpa_client_hisi.so \
		vendor/hi6210sft/lib/libwpa_client_hisi.so:system/lib/libwpa_client_hisi.so

#Wifi Config
PRODUCT_COPY_FILES += \
		vendor/hi6210sft/etc/wifi/hostapd_hisi.conf:system/etc/wifi/hostapd_hisi.conf \
		vendor/hi6210sft/etc/wifi/p2p_supplicant_overlay.conf:system/etc/wifi/p2p_supplicant_overlay.conf \
		vendor/hi6210sft/etc/wifi/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
		vendor/hi6210sft/etc/wifi/wpa_supplicant_hisi.conf:system/etc/wifi/wpa_supplicant_hisi.conf \
		vendor/hi6210sft/etc/wifi/wpa_supplicant_overlay.conf:system/etc/wifi/wpa_supplicant_overlay.conf

#Firmware
PRODUCT_COPY_FILES += \
		vendor/hi6210sft/vendor/firmware/4343A0_001.001.034.0056.0193_ORC.hcd:system/vendor/firmware/4343A0_001.001.034.0056.0193_ORC.hcd\
			vendor/hi6210sft/vendor/firmware/BCM4334B0_002.001.013.1554.1613_RC.hcd:system/vendor/firmware/BCM4334B0_002.001.013.1554.1613_RC.hcd\
			vendor/hi6210sft/vendor/firmware/fw_bcm4343s_apsta_hw.bin:system/vendor/firmware/fw_bcm4343s_apsta_hw.bin\
			vendor/hi6210sft/vendor/firmware/fw_bcm4343s_hw.bin:system/vendor/firmware/fw_bcm4343s_hw.bin\
			vendor/hi6210sft/vendor/firmware/fw_bcm4343s_p2p_hw.bin:system/vendor/firmware/fw_bcm4343s_p2p_hw.bin\
			vendor/hi6210sft/vendor/firmware/fw_bcm4343s_test_hw_apsta.bin:system/vendor/firmware/fw_bcm4343s_test_hw_apsta.bin\
			vendor/hi6210sft/vendor/firmware/GNSS.bin:system/vendor/firmware/GNSS.bin\
			vendor/hi6210sft/vendor/firmware/libpn547_fw.so:system/vendor/firmware/libpn547_fw.so\
			vendor/hi6210sft/vendor/firmware/nvram4343s_hw.txt:system/vendor/firmware/nvram4343s_hw.txt\
			vendor/hi6210sft/vendor/firmware/nvram_CHERRY_PLUS_TL00.txt:system/vendor/firmware/nvram_CHERRY_PLUS_TL00.txt\
			vendor/hi6210sft/vendor/firmware/nvram_CHERRY_PLUS_TL20M.txt:system/vendor/firmware/nvram_CHERRY_PLUS_TL20M.txt\
			vendor/hi6210sft/vendor/firmware/nvram_CHERRY_PLUS_UL00.txt:system/vendor/firmware/nvram_CHERRY_PLUS_UL00.txt\
			vendor/hi6210sft/vendor/firmware/RECOVER_DATA.bin:system/vendor/firmware/RECOVER_DATA.bin \
			vendor/hi6210sft/vendor/firmware/sdio_cfg:system/vendor/firmware/sdio_cfg\
			vendor/hi6210sft/vendor/firmware/SDIO_CMP.bin:system/vendor/firmware/SDIO_CMP.bin\
			vendor/hi6210sft/vendor/firmware/SDIO_COD.bin:system/vendor/firmware/SDIO_COD.bin\
			vendor/hi6210sft/vendor/firmware/sdio_recover_cfg:system/vendor/firmware/sdio_recover_cfg \
			vendor/hi6210sft/vendor/firmware/SDIO_RW.bin:system/vendor/firmware/SDIO_RW.bin\
			vendor/hi6210sft/vendor/firmware/SDIO_RW_ALICE_TL00_FEM.bin:system/vendor/firmware/SDIO_RW_ALICE_TL00_FEM.bin \
			vendor/hi6210sft/vendor/firmware/SDIO_RW_ALICE_TL21_FEM.bin:system/vendor/firmware/SDIO_RW_ALICE_TL21_FEM.bin \
			vendor/hi6210sft/vendor/firmware/SDIO_TBL.bin:system/vendor/firmware/SDIO_TBL.bin \
			vendor/hi6210sft/vendor/firmware/uart_cfg:system/vendor/firmware/uart_cfg \
			vendor/hi6210sft/vendor/firmware/UART_CMP.bin:system/vendor/firmware/UART_CMP.bin \
			vendor/hi6210sft/vendor/firmware/UART_COD.bin:system/vendor/firmware/UART_COD.bin \
			vendor/hi6210sft/vendor/firmware/UART_RW.bin:system/vendor/firmware/UART_RW.bin \
			vendor/hi6210sft/vendor/firmware/UART_TBL.bin:system/vendor/firmware/UART_TBL.bin \
			vendor/hi6210sft/etc/hisi_cfg.ini:system/etc/hisi_cfg.ini \
			vendor/hi6210sft/etc/hisi_cfg_alice.ini:system/etc/hisi_cfg_alice.ini


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
