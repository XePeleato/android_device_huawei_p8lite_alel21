#ifndef _DWC_OTG_HI3635_
#define _DWC_OTG_HI3635_
#include <linux/wakelock.h>
#include <linux/notifier.h>
#include <linux/huawei/usb/hisi_usb.h>
#include <linux/regulator/consumer.h>

/**
 * usb otg ahbif registers definations
 */
union usbotg2_ctrl0 {
	uint32_t reg;
	struct {
		uint32_t idpullup_sel:1;
		uint32_t idpullup:1;
		uint32_t acaenb_sel:1;
		uint32_t acaenb:1;
		uint32_t id_sel:2;
		uint32_t id:1;
		uint32_t drvvbus_sel:1;
		uint32_t drvvbus:1;
		uint32_t vbusvalid_sel:1;
		uint32_t sessvld_sel:1;
		uint32_t sessvld:1;
		uint32_t dpdmpulldown_sel:1;
		uint32_t dppulldown:1;
		uint32_t dmpulldown:1;
		uint32_t dbnce_fltr_bypass:1;
		uint32_t reserved:16;
	} bits;
};

union usbotg2_ctrl1 {
	uint32_t reg;
	struct {
		uint32_t scaledown_mode:2;
		uint32_t reserved:30;
	} bits;
};

union usbotg2_ctrl2 {
	uint32_t reg;
	struct {
		uint32_t commononn:1;
		uint32_t otgdisable:1;
		uint32_t vbusvldsel:1;
		uint32_t vbusvldext:1;
		uint32_t txbitstuffen:1;
		uint32_t txbitstuffenh:1;
		uint32_t fsxcvrowner:1;
		uint32_t txenablen:1;
		uint32_t fsdataext:1;
		uint32_t fsse0ext:1;
		uint32_t vatestenb:2;
		uint32_t reserved:20;
	} bits;
};

union usbotg2_ctrl3 {
	uint32_t reg;
	struct {
		uint32_t comdistune:3;
		uint32_t otgtune:3;
		uint32_t sqrxtune:3;
		uint32_t txfslstune:4;
		uint32_t txpreempamptune:2;
		uint32_t txpreemppulsetune:1;
		uint32_t txrisetune:2;
		uint32_t txvreftune:4;
		uint32_t txhsxvtune:2;
		uint32_t txrestune:2;
		uint32_t reserved:6;
	} bits;
};

union usbotg2_ctrl4 {
	uint32_t reg;
	struct {
		uint32_t siddq:1;
		uint32_t reserved:31;
	} bits;
};

union usbotg2_ctrl5 {
	uint32_t reg;
	struct {
		uint32_t refclksel:2;
		uint32_t fsel:3;
		uint32_t reserved:27;
	} bits;
};

union bc_ctrl0 {
	uint32_t reg;
	struct {
		uint32_t chrg_det_en:1;
		uint32_t reserved:31;
	} bits;
};
union bc_ctrl1 {
	uint32_t reg;
	struct {
		uint32_t chrg_det_int_clr:1;
		uint32_t reserved:31;
	} bits;
};
union bc_ctrl2 {
	uint32_t reg;
	struct {
		uint32_t chrg_det_int_msk:1;
		uint32_t reserved:31;
	} bits;
};
union bc_ctrl3 {
	uint32_t reg;
	struct {
		uint32_t bc_mode:1;
		uint32_t reserved:31;
	} bits;
};

union bc_ctrl4 {
	uint32_t reg;
	struct {
		uint32_t bc_opmode:2;
		uint32_t bc_xcvrselect:2;
		uint32_t bc_termselect:1;
		uint32_t bc_txvalid:1;
		uint32_t bc_txvalidh:1;
		uint32_t bc_idpullup:1;
		uint32_t bc_dppulldown:1;
		uint32_t bc_dmpulldown:1;
		uint32_t bc_suspendm:1;
		uint32_t bc_sleepm:1;
		uint32_t reserved:20;
	} bits;
};

union bc_ctrl5 {
	uint32_t reg;
	struct {
		uint32_t bc_aca_en:1;
		uint32_t bc_chrg_sel:1;
		uint32_t bc_vdat_src_en:1;
		uint32_t bc_vdat_det_en:1;
		uint32_t bc_dcd_en:1;
		uint32_t reserved:27;
	} bits;
};

union bc_ctrl6 {
	uint32_t reg;
	struct {
		uint32_t bc_chirp_int_clr:1;
		uint32_t reserved:31;
	} bits;
};
union bc_ctrl7 {
	uint32_t reg;
	struct {
		uint32_t bc_chirp_int_msk:1;
		uint32_t reserved:31;
	} bits;
};
union bc_ctrl8 {
	uint32_t reg;
	struct {
		uint32_t filter_len;
	} bits;
};
union bc_sts0 {
	uint32_t reg;
	struct {
		uint32_t chrg_det_rawint:1;
		uint32_t reserved:31;
	} bits;
};

union bc_sts1 {
	uint32_t reg;
	struct {
		uint32_t chrg_det_mskint:1;
		uint32_t reserved:31;
	} bits;
};

union bc_sts2 {
	uint32_t reg;
	struct {
		uint32_t bc_vbus_valid:1;
		uint32_t bc_sess_valid:1;
		uint32_t bc_fs_vplus:1;
		uint32_t bc_fs_vminus:1;
		uint32_t bc_chg_det:1;
		uint32_t bc_iddig:1;
		uint32_t bc_rid_float:1;
		uint32_t bc_rid_gnd:1;
		uint32_t bc_rid_a:1;
		uint32_t bc_rid_b:1;
		uint32_t bc_rid_c:1;
		uint32_t bc_chirp_on:1;
		uint32_t bc_linestate:2;
		uint32_t reserved:18;
	} bits;
};

union bc_sts3 {
	uint32_t reg;
	struct {
		uint32_t bc_rawint:1;
		uint32_t reserved:31;
	} bits;
};
union bc_sts4 {
	uint32_t reg;
	struct {
		uint32_t bc_mskint:1;
		uint32_t reserved:31;
	} bits;
};

struct usb_ahbif_registers {
	union usbotg2_ctrl0     usbotg2_ctrl0;
	union usbotg2_ctrl1     usbotg2_ctrl1;
	union usbotg2_ctrl2     usbotg2_ctrl2;
	union usbotg2_ctrl3     usbotg2_ctrl3;
	union usbotg2_ctrl4     usbotg2_ctrl4;
	union usbotg2_ctrl5     usbotg2_ctrl5;
	union bc_ctrl0          bc_ctrl0;
	union bc_ctrl1          bc_ctrl1;
	union bc_ctrl2          bc_ctrl2;
	union bc_ctrl3          bc_ctrl3;
	union bc_ctrl4          bc_ctrl4;
	union bc_ctrl5          bc_ctrl5;
	union bc_ctrl6          bc_ctrl6;
	union bc_ctrl7          bc_ctrl7;
	union bc_ctrl8          bc_ctrl8;
	union bc_sts0           bc_sts0;
	union bc_sts1           bc_sts1;
	union bc_sts2           bc_sts2;
	union bc_sts3           bc_sts3;
	union bc_sts4           bc_sts4;
};

/**
 * USB related register in PERICRG
 */
#define PERI_CRG_CLK_EN4			(0x40)
#define PERI_CRG_CLK_DIS4			(0x44)
#define PERI_CRG_CLK_CLKEN4			(0x48)
#define PERI_CRG_CLK_STAT4			(0x4C)
#define BIT_CLK_USB2OTG_REF			(1 << 8)
#define BIT_HCLK_USB2OTG			(1 << 1)
#define BIT_HCLK_USB2OTG_PMU			(1 << 0)

#define PERI_CRG_RSTEN4				(0x90)
#define PERI_CRG_RSTDIS4			(0x94)
#define PERI_CRG_RSTSTAT4			(0x98)
#define BIT_RST_USB2OTG_MUX			(1<<30)
#define BIT_RST_USB2OTG_AHBIF			(1<<29)
#define BIT_RST_USB2OTG_PHY_N			(1<<8)
#define BIT_RST_USB2OTG_ADP			(1<<7)
#define BIT_RST_USB2OTG_32K			(1<<6)
#define BIT_RST_USB2OTG				(1<<5)
#define BIT_RST_USB2OTGPHY			(1<<4)
#define BIT_RST_USB2OTGPHYPOR			(1<<3)

/**
 * USB related registers in PRRICRG
 */
#define REG_PMU_SSI_VERSION		0
#define REG_PMU_SSI_STATUS0		(0x01 << 2)
# define VBAT3P2_2D			(1 << 4)
# define VBUS5P5_D10R			(1 << 3)
# define VBUS4P3_D10			(1 << 2)
# define VBUS_COMP_VBAT_D20		(1 << 1)
#define REG_PMU_SSI_STATUS1		(0x02 << 2)
#define REG_PMU_SSI_IRQ0		(0x120 << 2)
#define REG_PMU_SSI_IRQ1		(0x121 << 2)
# define VBUS_COMP_VBAT_F		(1 << 4)
# define VBUS_COMP_VBAT_R		(1 << 3)
#define REG_PMU_SSI_IRQ_MASK_0		(0x102 << 2)
#define REG_PMU_SSI_IRQ_MASK_1		(0x103 << 2)


/**
 * USB ahbif registers
 */
#define USBPHY_ENTER_SIDDQ			(0x1)
#define USBPHY_QUIT_SIDDQ				(0x0)

#define usb_dbg(format, arg...)    \
	do {                 \
		printk(KERN_INFO "[USB][%s]"format, __func__, ##arg); \
	} while (0)
#define usb_err(format, arg...)    \
	do {                 \
		printk(KERN_ERR "[USB]"format, ##arg); \
	} while (0)

enum otg_dev_status {
	OTG_DEV_OFF = 0,
	OTG_DEV_DEVICE,
	OTG_DEV_HOST,
	OTG_DEV_SUSPEND
};

enum otg_hcd_status {
	HCD_OFF = 0,
	HCD_ON,
};

struct otg_dev {
	enum otg_dev_status status;
	enum otg_hcd_status hcd_status;
	enum otg_dev_event_type event;
	enum hisi_charger_type charger_type;

	unsigned gadget_initialized;
	bool dwc_otg_irq_enabled;

	struct lm_device    *lm_dev;
	struct platform_device *pdev;
	struct delayed_work event_work;
	struct wake_lock wake_lock;
	struct mutex lock;
	spinlock_t event_lock;

	struct atomic_notifier_head charger_type_notifier;

	void __iomem *pericrg_base;
	void __iomem *pmu_reg_base;
	void __iomem *usb_ahbif_base;

	unsigned int is_regu_on;
	unsigned int eyePattern;
	struct regulator_bulk_data otgdebugsubsys_regu;
};

void dwc_otg_hi3635_wake_lock(void);
void dwc_otg_hi3635_wake_unlock(void);
int dwc_otg_hi3635_probe_stage2(void);
int dwc_otg_hi3635_is_power_on(void);
void update_charger_type(void);
int dwc_otg_hcd_setup(void);
void dwc_otg_hcd_shutdown(void);

#endif
