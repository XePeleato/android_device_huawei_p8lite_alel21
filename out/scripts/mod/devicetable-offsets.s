	.cpu generic
	.file	"devicetable-offsets.c"
// GNU C (GCC) version 4.8 (aarch64-linux-android)
//	compiled by GNU C version 4.6.x-google 20120106 (prerelease), GMP version 5.0.5, MPFR version 3.1.1, MPC version 1.0.1
// GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
// options passed:  -nostdinc
// -I /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include
// -I arch/arm64/include/generated
// -I /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include -I include
// -I /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi
// -I arch/arm64/include/generated/uapi
// -I /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi
// -I include/generated/uapi
// -I /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/drivers
// -I /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/mm
// -I /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/drivers/hisi/ap/config/product/hi6210sft/include
// -I /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/drivers/hisi/ap/platform/hi6210sft
// -I /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod
// -I scripts/mod
// -iprefix /home/edu/AOSP/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.8/bin/../lib/gcc/aarch64-linux-android/4.8/
// -D __KERNEL__ -D FEATURE_ON=1 -D FEATURE_OFF=0
// -D FEATURE_UE_MODE_CDMA=FEATURE_OFF -D CHIP_BB_HI6210 -D BSP_CORE_APP
// -D BSP_COMPILE_ALLY -D BSP_CORE_APP -D BSP_COMPILE_ALLY
// -D KBUILD_STR(s)=#s -D KBUILD_BASENAME=KBUILD_STR(devicetable_offsets)
// -D KBUILD_MODNAME=KBUILD_STR(devicetable_offsets)
// -isystem /home/edu/AOSP/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.8/bin/../lib/gcc/aarch64-linux-android/4.8/include
// -include /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/kconfig.h
// -MD scripts/mod/.devicetable-offsets.s.d
// /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c
// -mbionic -fpic -mlittle-endian -mgeneral-regs-only
// -auxbase-strip scripts/mod/devicetable-offsets.s -g -g -O2 -Wall -Wundef
// -Wstrict-prototypes -Wno-trigraphs -Wimplicit-function-declaration
// -Wno-format-security -Wframe-larger-than=2048
// -Wno-unused-but-set-variable -Wdeclaration-after-statement
// -Wno-pointer-sign -fno-strict-aliasing -fno-common
// -fno-delete-null-pointer-checks -fno-reorder-blocks -fno-ipa-cp-clone
// -fno-partial-inlining -fno-stack-protector -fno-omit-frame-pointer
// -fno-optimize-sibling-calls -fno-var-tracking-assignments
// -fno-strict-overflow -fconserve-stack -fverbose-asm
// options enabled:  -faggressive-loop-optimizations -fauto-inc-dec
// -fbranch-count-reg -fcaller-saves -fcombine-stack-adjustments
// -fcompare-elim -fcprop-registers -fcrossjumping -fcse-follow-jumps
// -fdefer-pop -fdevirtualize -fdwarf2-cfi-asm -fearly-inlining
// -feliminate-unused-debug-types -fexpensive-optimizations
// -fforward-propagate -ffunction-cse -fgcse -fgcse-lm -fgnu-runtime
// -fguess-branch-probability -fhoist-adjacent-loads -fident
// -fif-conversion -fif-conversion2 -findirect-inlining -finline
// -finline-atomics -finline-functions-called-once -finline-small-functions
// -fipa-cp -fipa-profile -fipa-pure-const -fipa-reference -fipa-sra
// -fira-hoist-pressure -fira-share-save-slots -fira-share-spill-slots
// -fivopts -fkeep-static-consts -fleading-underscore -fmath-errno
// -fmerge-constants -fmerge-debug-strings -fmove-loop-invariants
// -fomit-frame-pointer -foptimize-register-move -foptimize-strlen
// -fpeephole -fpeephole2 -fpic -fprefetch-loop-arrays -freg-struct-return
// -fregmove -freorder-functions -frerun-cse-after-loop
// -fsched-critical-path-heuristic -fsched-dep-count-heuristic
// -fsched-group-heuristic -fsched-interblock -fsched-last-insn-heuristic
// -fsched-rank-heuristic -fsched-spec -fsched-spec-insn-heuristic
// -fsched-stalled-insns-dep -fschedule-insns -fschedule-insns2
// -fsection-anchors -fshow-column -fshrink-wrap -fsigned-zeros
// -fsplit-ivs-in-unroller -fsplit-wide-types -fstrict-volatile-bitfields
// -fsync-libcalls -fthread-jumps -ftoplevel-reorder -ftrapping-math
// -ftree-bit-ccp -ftree-builtin-call-dce -ftree-ccp -ftree-ch
// -ftree-coalesce-vars -ftree-copy-prop -ftree-copyrename -ftree-cselim
// -ftree-dce -ftree-dominator-opts -ftree-dse -ftree-forwprop -ftree-fre
// -ftree-loop-if-convert -ftree-loop-im -ftree-loop-ivcanon
// -ftree-loop-optimize -ftree-parallelize-loops= -ftree-phiprop -ftree-pre
// -ftree-pta -ftree-reassoc -ftree-scev-cprop -ftree-sink
// -ftree-slp-vectorize -ftree-slsr -ftree-sra -ftree-switch-conversion
// -ftree-tail-merge -ftree-ter -ftree-vect-loop-version -ftree-vrp
// -funit-at-a-time -fvar-tracking -fverbose-asm -fzero-initialized-in-bss
// -mandroid -mbionic -mgeneral-regs-only -mlittle-endian
// -momit-leaf-frame-pointer

	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.section	.text.startup,"ax",%progbits
	.align	2
	.global	main
	.type	main, %function
main:
.LFB5:
.LM1:
	.cfi_startproc
.LM2:
#APP
// 10 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_usb_device_id 32 sizeof(struct usb_device_id)	//
// 0 "" 2
.LM3:
// 11 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_usb_device_id_match_flags 0 offsetof(struct usb_device_id, match_flags)	//
// 0 "" 2
.LM4:
// 12 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_usb_device_id_idVendor 2 offsetof(struct usb_device_id, idVendor)	//
// 0 "" 2
.LM5:
// 13 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_usb_device_id_idProduct 4 offsetof(struct usb_device_id, idProduct)	//
// 0 "" 2
.LM6:
// 14 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_usb_device_id_bcdDevice_lo 6 offsetof(struct usb_device_id, bcdDevice_lo)	//
// 0 "" 2
.LM7:
// 15 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_usb_device_id_bcdDevice_hi 8 offsetof(struct usb_device_id, bcdDevice_hi)	//
// 0 "" 2
.LM8:
// 16 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_usb_device_id_bDeviceClass 10 offsetof(struct usb_device_id, bDeviceClass)	//
// 0 "" 2
.LM9:
// 17 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_usb_device_id_bDeviceSubClass 11 offsetof(struct usb_device_id, bDeviceSubClass)	//
// 0 "" 2
.LM10:
// 18 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_usb_device_id_bDeviceProtocol 12 offsetof(struct usb_device_id, bDeviceProtocol)	//
// 0 "" 2
.LM11:
// 19 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_usb_device_id_bInterfaceClass 13 offsetof(struct usb_device_id, bInterfaceClass)	//
// 0 "" 2
.LM12:
// 20 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_usb_device_id_bInterfaceSubClass 14 offsetof(struct usb_device_id, bInterfaceSubClass)	//
// 0 "" 2
.LM13:
// 21 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_usb_device_id_bInterfaceProtocol 15 offsetof(struct usb_device_id, bInterfaceProtocol)	//
// 0 "" 2
.LM14:
// 22 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_usb_device_id_bInterfaceNumber 16 offsetof(struct usb_device_id, bInterfaceNumber)	//
// 0 "" 2
.LM15:
// 24 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_hid_device_id 24 sizeof(struct hid_device_id)	//
// 0 "" 2
.LM16:
// 25 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_hid_device_id_bus 0 offsetof(struct hid_device_id, bus)	//
// 0 "" 2
.LM17:
// 26 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_hid_device_id_group 2 offsetof(struct hid_device_id, group)	//
// 0 "" 2
.LM18:
// 27 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_hid_device_id_vendor 4 offsetof(struct hid_device_id, vendor)	//
// 0 "" 2
.LM19:
// 28 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_hid_device_id_product 8 offsetof(struct hid_device_id, product)	//
// 0 "" 2
.LM20:
// 30 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_ieee1394_device_id 32 sizeof(struct ieee1394_device_id)	//
// 0 "" 2
.LM21:
// 31 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ieee1394_device_id_match_flags 0 offsetof(struct ieee1394_device_id, match_flags)	//
// 0 "" 2
.LM22:
// 32 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ieee1394_device_id_vendor_id 4 offsetof(struct ieee1394_device_id, vendor_id)	//
// 0 "" 2
.LM23:
// 33 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ieee1394_device_id_model_id 8 offsetof(struct ieee1394_device_id, model_id)	//
// 0 "" 2
.LM24:
// 34 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ieee1394_device_id_specifier_id 12 offsetof(struct ieee1394_device_id, specifier_id)	//
// 0 "" 2
.LM25:
// 35 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ieee1394_device_id_version 16 offsetof(struct ieee1394_device_id, version)	//
// 0 "" 2
.LM26:
// 37 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_pci_device_id 32 sizeof(struct pci_device_id)	//
// 0 "" 2
.LM27:
// 38 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_pci_device_id_vendor 0 offsetof(struct pci_device_id, vendor)	//
// 0 "" 2
.LM28:
// 39 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_pci_device_id_device 4 offsetof(struct pci_device_id, device)	//
// 0 "" 2
.LM29:
// 40 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_pci_device_id_subvendor 8 offsetof(struct pci_device_id, subvendor)	//
// 0 "" 2
.LM30:
// 41 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_pci_device_id_subdevice 12 offsetof(struct pci_device_id, subdevice)	//
// 0 "" 2
.LM31:
// 42 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_pci_device_id_class 16 offsetof(struct pci_device_id, class)	//
// 0 "" 2
.LM32:
// 43 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_pci_device_id_class_mask 20 offsetof(struct pci_device_id, class_mask)	//
// 0 "" 2
.LM33:
// 45 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_ccw_device_id 16 sizeof(struct ccw_device_id)	//
// 0 "" 2
.LM34:
// 46 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ccw_device_id_match_flags 0 offsetof(struct ccw_device_id, match_flags)	//
// 0 "" 2
.LM35:
// 47 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ccw_device_id_cu_type 2 offsetof(struct ccw_device_id, cu_type)	//
// 0 "" 2
.LM36:
// 48 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ccw_device_id_cu_model 6 offsetof(struct ccw_device_id, cu_model)	//
// 0 "" 2
.LM37:
// 49 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ccw_device_id_dev_type 4 offsetof(struct ccw_device_id, dev_type)	//
// 0 "" 2
.LM38:
// 50 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ccw_device_id_dev_model 7 offsetof(struct ccw_device_id, dev_model)	//
// 0 "" 2
.LM39:
// 52 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_ap_device_id 16 sizeof(struct ap_device_id)	//
// 0 "" 2
.LM40:
// 53 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ap_device_id_dev_type 2 offsetof(struct ap_device_id, dev_type)	//
// 0 "" 2
.LM41:
// 55 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_css_device_id 16 sizeof(struct css_device_id)	//
// 0 "" 2
.LM42:
// 56 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_css_device_id_type 1 offsetof(struct css_device_id, type)	//
// 0 "" 2
.LM43:
// 58 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_serio_device_id 4 sizeof(struct serio_device_id)	//
// 0 "" 2
.LM44:
// 59 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_serio_device_id_type 0 offsetof(struct serio_device_id, type)	//
// 0 "" 2
.LM45:
// 60 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_serio_device_id_proto 3 offsetof(struct serio_device_id, proto)	//
// 0 "" 2
.LM46:
// 61 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_serio_device_id_id 2 offsetof(struct serio_device_id, id)	//
// 0 "" 2
.LM47:
// 62 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_serio_device_id_extra 1 offsetof(struct serio_device_id, extra)	//
// 0 "" 2
.LM48:
// 64 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_acpi_device_id 24 sizeof(struct acpi_device_id)	//
// 0 "" 2
.LM49:
// 65 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_acpi_device_id_id 0 offsetof(struct acpi_device_id, id)	//
// 0 "" 2
.LM50:
// 67 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_pnp_device_id 16 sizeof(struct pnp_device_id)	//
// 0 "" 2
.LM51:
// 68 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_pnp_device_id_id 0 offsetof(struct pnp_device_id, id)	//
// 0 "" 2
.LM52:
// 70 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_pnp_card_device_id 80 sizeof(struct pnp_card_device_id)	//
// 0 "" 2
.LM53:
// 71 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_pnp_card_device_id_devs 16 offsetof(struct pnp_card_device_id, devs)	//
// 0 "" 2
.LM54:
// 73 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_pcmcia_device_id 80 sizeof(struct pcmcia_device_id)	//
// 0 "" 2
.LM55:
// 74 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_pcmcia_device_id_match_flags 0 offsetof(struct pcmcia_device_id, match_flags)	//
// 0 "" 2
.LM56:
// 75 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_pcmcia_device_id_manf_id 2 offsetof(struct pcmcia_device_id, manf_id)	//
// 0 "" 2
.LM57:
// 76 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_pcmcia_device_id_card_id 4 offsetof(struct pcmcia_device_id, card_id)	//
// 0 "" 2
.LM58:
// 77 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_pcmcia_device_id_func_id 6 offsetof(struct pcmcia_device_id, func_id)	//
// 0 "" 2
.LM59:
// 78 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_pcmcia_device_id_function 7 offsetof(struct pcmcia_device_id, function)	//
// 0 "" 2
.LM60:
// 79 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_pcmcia_device_id_device_no 8 offsetof(struct pcmcia_device_id, device_no)	//
// 0 "" 2
.LM61:
// 80 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_pcmcia_device_id_prod_id_hash 12 offsetof(struct pcmcia_device_id, prod_id_hash)	//
// 0 "" 2
.LM62:
// 82 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_of_device_id 200 sizeof(struct of_device_id)	//
// 0 "" 2
.LM63:
// 83 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_of_device_id_name 0 offsetof(struct of_device_id, name)	//
// 0 "" 2
.LM64:
// 84 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_of_device_id_type 32 offsetof(struct of_device_id, type)	//
// 0 "" 2
.LM65:
// 85 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_of_device_id_compatible 64 offsetof(struct of_device_id, compatible)	//
// 0 "" 2
.LM66:
// 87 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_vio_device_id 64 sizeof(struct vio_device_id)	//
// 0 "" 2
.LM67:
// 88 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_vio_device_id_type 0 offsetof(struct vio_device_id, type)	//
// 0 "" 2
.LM68:
// 89 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_vio_device_id_compat 32 offsetof(struct vio_device_id, compat)	//
// 0 "" 2
.LM69:
// 91 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_input_device_id 192 sizeof(struct input_device_id)	//
// 0 "" 2
.LM70:
// 92 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_input_device_id_flags 0 offsetof(struct input_device_id, flags)	//
// 0 "" 2
.LM71:
// 93 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_input_device_id_bustype 8 offsetof(struct input_device_id, bustype)	//
// 0 "" 2
.LM72:
// 94 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_input_device_id_vendor 10 offsetof(struct input_device_id, vendor)	//
// 0 "" 2
.LM73:
// 95 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_input_device_id_product 12 offsetof(struct input_device_id, product)	//
// 0 "" 2
.LM74:
// 96 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_input_device_id_version 14 offsetof(struct input_device_id, version)	//
// 0 "" 2
.LM75:
// 97 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_input_device_id_evbit 16 offsetof(struct input_device_id, evbit)	//
// 0 "" 2
.LM76:
// 98 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_input_device_id_keybit 24 offsetof(struct input_device_id, keybit)	//
// 0 "" 2
.LM77:
// 99 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_input_device_id_relbit 120 offsetof(struct input_device_id, relbit)	//
// 0 "" 2
.LM78:
// 100 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_input_device_id_absbit 128 offsetof(struct input_device_id, absbit)	//
// 0 "" 2
.LM79:
// 101 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_input_device_id_mscbit 136 offsetof(struct input_device_id, mscbit)	//
// 0 "" 2
.LM80:
// 102 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_input_device_id_ledbit 144 offsetof(struct input_device_id, ledbit)	//
// 0 "" 2
.LM81:
// 103 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_input_device_id_sndbit 152 offsetof(struct input_device_id, sndbit)	//
// 0 "" 2
.LM82:
// 104 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_input_device_id_ffbit 160 offsetof(struct input_device_id, ffbit)	//
// 0 "" 2
.LM83:
// 105 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_input_device_id_swbit 176 offsetof(struct input_device_id, swbit)	//
// 0 "" 2
.LM84:
// 107 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_eisa_device_id 16 sizeof(struct eisa_device_id)	//
// 0 "" 2
.LM85:
// 108 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_eisa_device_id_sig 0 offsetof(struct eisa_device_id, sig)	//
// 0 "" 2
.LM86:
// 110 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_parisc_device_id 8 sizeof(struct parisc_device_id)	//
// 0 "" 2
.LM87:
// 111 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_parisc_device_id_hw_type 0 offsetof(struct parisc_device_id, hw_type)	//
// 0 "" 2
.LM88:
// 112 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_parisc_device_id_hversion 2 offsetof(struct parisc_device_id, hversion)	//
// 0 "" 2
.LM89:
// 113 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_parisc_device_id_hversion_rev 1 offsetof(struct parisc_device_id, hversion_rev)	//
// 0 "" 2
.LM90:
// 114 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_parisc_device_id_sversion 4 offsetof(struct parisc_device_id, sversion)	//
// 0 "" 2
.LM91:
// 116 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_sdio_device_id 16 sizeof(struct sdio_device_id)	//
// 0 "" 2
.LM92:
// 117 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_sdio_device_id_class 0 offsetof(struct sdio_device_id, class)	//
// 0 "" 2
.LM93:
// 118 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_sdio_device_id_vendor 2 offsetof(struct sdio_device_id, vendor)	//
// 0 "" 2
.LM94:
// 119 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_sdio_device_id_device 4 offsetof(struct sdio_device_id, device)	//
// 0 "" 2
.LM95:
// 121 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_ssb_device_id 6 sizeof(struct ssb_device_id)	//
// 0 "" 2
.LM96:
// 122 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ssb_device_id_vendor 0 offsetof(struct ssb_device_id, vendor)	//
// 0 "" 2
.LM97:
// 123 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ssb_device_id_coreid 2 offsetof(struct ssb_device_id, coreid)	//
// 0 "" 2
.LM98:
// 124 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ssb_device_id_revision 4 offsetof(struct ssb_device_id, revision)	//
// 0 "" 2
.LM99:
// 126 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_bcma_device_id 6 sizeof(struct bcma_device_id)	//
// 0 "" 2
.LM100:
// 127 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_bcma_device_id_manuf 0 offsetof(struct bcma_device_id, manuf)	//
// 0 "" 2
.LM101:
// 128 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_bcma_device_id_id 2 offsetof(struct bcma_device_id, id)	//
// 0 "" 2
.LM102:
// 129 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_bcma_device_id_rev 4 offsetof(struct bcma_device_id, rev)	//
// 0 "" 2
.LM103:
// 130 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_bcma_device_id_class 5 offsetof(struct bcma_device_id, class)	//
// 0 "" 2
.LM104:
// 132 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_virtio_device_id 8 sizeof(struct virtio_device_id)	//
// 0 "" 2
.LM105:
// 133 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_virtio_device_id_device 0 offsetof(struct virtio_device_id, device)	//
// 0 "" 2
.LM106:
// 134 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_virtio_device_id_vendor 4 offsetof(struct virtio_device_id, vendor)	//
// 0 "" 2
.LM107:
// 136 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_hv_vmbus_device_id 24 sizeof(struct hv_vmbus_device_id)	//
// 0 "" 2
.LM108:
// 137 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_hv_vmbus_device_id_guid 0 offsetof(struct hv_vmbus_device_id, guid)	//
// 0 "" 2
.LM109:
// 139 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_i2c_device_id 32 sizeof(struct i2c_device_id)	//
// 0 "" 2
.LM110:
// 140 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_i2c_device_id_name 0 offsetof(struct i2c_device_id, name)	//
// 0 "" 2
.LM111:
// 142 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_spi_device_id 40 sizeof(struct spi_device_id)	//
// 0 "" 2
.LM112:
// 143 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_spi_device_id_name 0 offsetof(struct spi_device_id, name)	//
// 0 "" 2
.LM113:
// 145 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_dmi_system_id 344 sizeof(struct dmi_system_id)	//
// 0 "" 2
.LM114:
// 146 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_dmi_system_id_matches 16 offsetof(struct dmi_system_id, matches)	//
// 0 "" 2
.LM115:
// 148 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_platform_device_id 32 sizeof(struct platform_device_id)	//
// 0 "" 2
.LM116:
// 149 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_platform_device_id_name 0 offsetof(struct platform_device_id, name)	//
// 0 "" 2
.LM117:
// 151 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_mdio_device_id 8 sizeof(struct mdio_device_id)	//
// 0 "" 2
.LM118:
// 152 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_mdio_device_id_phy_id 0 offsetof(struct mdio_device_id, phy_id)	//
// 0 "" 2
.LM119:
// 153 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_mdio_device_id_phy_id_mask 4 offsetof(struct mdio_device_id, phy_id_mask)	//
// 0 "" 2
.LM120:
// 155 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_zorro_device_id 16 sizeof(struct zorro_device_id)	//
// 0 "" 2
.LM121:
// 156 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_zorro_device_id_id 0 offsetof(struct zorro_device_id, id)	//
// 0 "" 2
.LM122:
// 158 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_isapnp_device_id 16 sizeof(struct isapnp_device_id)	//
// 0 "" 2
.LM123:
// 159 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_isapnp_device_id_vendor 4 offsetof(struct isapnp_device_id, vendor)	//
// 0 "" 2
.LM124:
// 160 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_isapnp_device_id_function 6 offsetof(struct isapnp_device_id, function)	//
// 0 "" 2
.LM125:
// 162 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_ipack_device_id 12 sizeof(struct ipack_device_id)	//
// 0 "" 2
.LM126:
// 163 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ipack_device_id_format 0 offsetof(struct ipack_device_id, format)	//
// 0 "" 2
.LM127:
// 164 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ipack_device_id_vendor 4 offsetof(struct ipack_device_id, vendor)	//
// 0 "" 2
.LM128:
// 165 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_ipack_device_id_device 8 offsetof(struct ipack_device_id, device)	//
// 0 "" 2
.LM129:
// 167 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_amba_id 16 sizeof(struct amba_id)	//
// 0 "" 2
.LM130:
// 168 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_amba_id_id 0 offsetof(struct amba_id, id)	//
// 0 "" 2
.LM131:
// 169 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_amba_id_mask 4 offsetof(struct amba_id, mask)	//
// 0 "" 2
.LM132:
// 171 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_x86_cpu_id 16 sizeof(struct x86_cpu_id)	//
// 0 "" 2
.LM133:
// 172 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_x86_cpu_id_feature 6 offsetof(struct x86_cpu_id, feature)	//
// 0 "" 2
.LM134:
// 173 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_x86_cpu_id_family 2 offsetof(struct x86_cpu_id, family)	//
// 0 "" 2
.LM135:
// 174 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_x86_cpu_id_model 4 offsetof(struct x86_cpu_id, model)	//
// 0 "" 2
.LM136:
// 175 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_x86_cpu_id_vendor 0 offsetof(struct x86_cpu_id, vendor)	//
// 0 "" 2
.LM137:
// 177 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_cpu_feature 2 sizeof(struct cpu_feature)	//
// 0 "" 2
.LM138:
// 178 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_cpu_feature_feature 0 offsetof(struct cpu_feature, feature)	//
// 0 "" 2
.LM139:
// 180 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->SIZE_mei_cl_device_id 40 sizeof(struct mei_cl_device_id)	//
// 0 "" 2
.LM140:
// 181 "/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c" 1
	
->OFF_mei_cl_device_id_name 0 offsetof(struct mei_cl_device_id, name)	//
// 0 "" 2
.LM141:
#NO_APP
	mov	w0, 0	//,
	ret
	.cfi_endproc
.LFE5:
	.size	main, .-main
	.text
.Letext0:
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.4byte	0x9e
	.2byte	0x4
	.4byte	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.4byte	.LASF12
	.byte	0x1
	.4byte	.LASF13
	.4byte	.LASF14
	.4byte	.Ldebug_ranges0+0
	.8byte	0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF0
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF1
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.4byte	.LASF2
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.4byte	.LASF3
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF4
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF5
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF6
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF7
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF8
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF9
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF10
	.uleb128 0x2
	.byte	0x1
	.byte	0x2
	.4byte	.LASF11
	.uleb128 0x4
	.4byte	.LASF15
	.byte	0x1
	.byte	0x8
	.4byte	0x45
	.8byte	.LFB5
	.8byte	.LFE5-.LFB5
	.uleb128 0x1
	.byte	0x9c
	.byte	0
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",%progbits
	.4byte	0x2c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x8
	.byte	0
	.2byte	0
	.2byte	0
	.8byte	.LFB5
	.8byte	.LFE5-.LFB5
	.8byte	0
	.8byte	0
	.section	.debug_ranges,"",%progbits
.Ldebug_ranges0:
	.8byte	.LFB5
	.8byte	.LFE5
	.8byte	0
	.8byte	0
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.4byte	.LELT0-.LSLT0
.LSLT0:
	.2byte	0x4
	.4byte	.LELTP0-.LASLTP0
.LASLTP0:
	.byte	0x1
	.byte	0x1
	.byte	0x1
	.byte	0xf6
	.byte	0xf2
	.byte	0xd
	.byte	0
	.byte	0x1
	.byte	0x1
	.byte	0x1
	.byte	0x1
	.byte	0
	.byte	0
	.byte	0
	.byte	0x1
	.byte	0
	.byte	0
	.byte	0x1
	.ascii	"/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod"
	.byte	0
	.byte	0
	.string	"devicetable-offsets.c"
	.uleb128 0x1
	.uleb128 0
	.uleb128 0
	.byte	0
.LELTP0:
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM1
	.byte	0x1f
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM2
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM3
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM4
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM5
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM6
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM7
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM8
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM9
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM10
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM11
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM12
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM13
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM14
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM15
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM16
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM17
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM18
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM19
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM20
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM21
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM22
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM23
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM24
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM25
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM26
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM27
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM28
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM29
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM30
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM31
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM32
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM33
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM34
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM35
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM36
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM37
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM38
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM39
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM40
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM41
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM42
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM43
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM44
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM45
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM46
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM47
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM48
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM49
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM50
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM51
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM52
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM53
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM54
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM55
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM56
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM57
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM58
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM59
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM60
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM61
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM62
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM63
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM64
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM65
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM66
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM67
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM68
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM69
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM70
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM71
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM72
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM73
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM74
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM75
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM76
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM77
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM78
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM79
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM80
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM81
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM82
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM83
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM84
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM85
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM86
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM87
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM88
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM89
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM90
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM91
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM92
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM93
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM94
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM95
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM96
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM97
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM98
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM99
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM100
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM101
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM102
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM103
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM104
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM105
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM106
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM107
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM108
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM109
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM110
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM111
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM112
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM113
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM114
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM115
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM116
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM117
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM118
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM119
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM120
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM121
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM122
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM123
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM124
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM125
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM126
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM127
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM128
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM129
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM130
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM131
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM132
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM133
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM134
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM135
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM136
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM137
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM138
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM139
	.byte	0x19
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM140
	.byte	0x18
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LM141
	.byte	0x1a
	.byte	0
	.uleb128 0x9
	.byte	0x2
	.8byte	.LFE5
	.byte	0
	.uleb128 0x1
	.byte	0x1
.LELT0:
	.section	.debug_str,"MS",%progbits,1
.LASF5:
	.string	"long long int"
.LASF4:
	.string	"unsigned int"
.LASF15:
	.string	"main"
.LASF7:
	.string	"long unsigned int"
.LASF6:
	.string	"long long unsigned int"
.LASF1:
	.string	"unsigned char"
.LASF9:
	.string	"char"
.LASF12:
	.ascii	"GNU C 4.8 -mbionic -fpic -mlittle-endian -mgeneral-regs-only"
	.ascii	" -g -g -O2 -fno-stri"
	.string	"ct-aliasing -fno-common -fno-delete-null-pointer-checks -fno-reorder-blocks -fno-ipa-cp-clone -fno-partial-inlining -fno-stack-protector -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-var-tracking-assignments -fno-strict-overflow -fconserve-stack"
.LASF10:
	.string	"long int"
.LASF11:
	.string	"_Bool"
.LASF3:
	.string	"short unsigned int"
.LASF0:
	.string	"signed char"
.LASF14:
	.string	"/home/edu/AOSP/device/HUAWEI/hi6210sft/out"
.LASF13:
	.string	"/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/scripts/mod/devicetable-offsets.c"
.LASF2:
	.string	"short int"
.LASF8:
	.string	"sizetype"
	.ident	"GCC: (GNU) 4.8"
