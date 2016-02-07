

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#ifdef CONFIG_HI3635_USB
#include"lm.h"
#include"dwc_otg_driver.h"
#include <linux/huawei/usb/hisi_usb.h>

extern struct hiusb_info *g_hiusb_info;
struct lm_device lm_dev;

extern int hiusb_charger_registe_notifier(struct notifier_block *nb);

struct hiusb_info test_hiusb_info;

struct notifier_block   nb;

static int notifier_call(struct notifier_block *nb,   unsigned long event, void *data)
{
      return 0;
}

int test_get_charger_name(int process)
{
    int ret;

    switch (process) {
        case 0:
        case 2://down_interrupt
            g_hiusb_info = &test_hiusb_info;
            g_hiusb_info->charger_type  = 0x55555555;
            break;
        case 1:
        default:
            g_hiusb_info = NULL;
            break;            
    }

    ret = get_charger_name();
        
    return (ret==0x55555555)?0:-1;
}

int test_hiusb_charger_registe_notifier(int process)
{
    int ret;

	nb.notifier_call = notifier_call;
    
    switch (process) {
        case 0:
            g_hiusb_info = &test_hiusb_info;
            break;
        case 1:
        default:
            g_hiusb_info = NULL;
            break;            
    }

    ret = hiusb_charger_registe_notifier(&nb);
        

    return ret;
}

int test_hiusb_charger_unregiste_notifier(int process)
{
    int ret;
    
    switch (process) {
        case 0:
            g_hiusb_info = &test_hiusb_info;
            break;
        case 1:
        default:
            g_hiusb_info = NULL;
            break;            
    }

    ret = hiusb_charger_unregiste_notifier(0);
        

    return ret;
}

int test_switch_init(int process)
{
    int ret;

    lm_dev.id = -1;
    lm_dev.dev.init_name = "hi6xxx-usb-otg";
    lm_dev.hiusb_info = &test_hiusb_info;
    g_hiusb_info = &test_hiusb_info;
    switch (process) {
        case 0:
        case 2:
        case 3:
            g_hiusb_info->switch_pin1 = 1;
            g_hiusb_info->switch_pin2 = 1;
            break;
        case 1:
        default:
            g_hiusb_info->switch_pin1 = 0;
            g_hiusb_info->switch_pin2 = 0;
            break;            
    }

    ret = switch_init(&lm_dev);
        

    return ret;
}

int test_switch_reset(int process)
{
    int ret;

    lm_dev.hiusb_info = &test_hiusb_info;
    g_hiusb_info = &test_hiusb_info;
    switch (process) {
        case 0:
            g_hiusb_info->switch_pin1 = 1;
            g_hiusb_info->switch_pin2 = 1;
            break;
        case 1:
            g_hiusb_info = NULL;
            break;
        case 2:            
        default:    
            g_hiusb_info->switch_pin1 = 0;
            g_hiusb_info->switch_pin2 = 0;
            break;           
    }

    ret = switch_reset();
        

    return ret;
}

int test_switch_usb(int process)
{
    int ret;

    lm_dev.hiusb_info = &test_hiusb_info;
    g_hiusb_info = &test_hiusb_info;
    switch (process) {
        case 0:
            g_hiusb_info->switch_pin1 = 1;
            g_hiusb_info->switch_pin2 = 1;
            break;
        case 1:
            g_hiusb_info = NULL;
            break;
        case 2:
        default:    
            g_hiusb_info->switch_pin1 = 0;
            g_hiusb_info->switch_pin2 = 0;
            break;         
    }

    ret = switch_usb();
        

    return ret;
}

int test_switch_mhl(int process)
{
    int ret;

    lm_dev.hiusb_info = &test_hiusb_info;
    g_hiusb_info = &test_hiusb_info;
    switch (process) {
        case 0:
            g_hiusb_info->switch_pin1 = 1;
            g_hiusb_info->switch_pin2 = 1;
            break;
        case 1:
            g_hiusb_info = NULL;
            break;
        case 2:
        default:    
            g_hiusb_info->switch_pin1 = 0;
            g_hiusb_info->switch_pin2 = 0;
            break;         
    }

    ret = switch_mhl();
        

    return ret;
}

int test_hiusb_otg_intr_work(int process)
{
    int ret = -1;
    struct work_struct *usb_work;
    
    lm_dev.hiusb_info = &test_hiusb_info;
dwc_otg_device_t  otg; 
	
lm_dev.dev.p = &otg;;

    g_hiusb_info = &test_hiusb_info;
    usb_work = &g_hiusb_info->otg_intr_work.work;
    g_hiusb_info->lm_dev = &lm_dev;
    switch (process) {
        case 0:
            g_hiusb_info->intr_flag = CHARGER_CONNECT_EVENT;
            g_hiusb_info->quirks = HIUSB_QUIRKS_CHARGER;
            g_hiusb_info->hiusb_status = HIUSB_OFF;
            break;  
        case 1:
            g_hiusb_info->intr_flag = CHARGER_DISCONNECT_EVENT;
            g_hiusb_info->quirks = HIUSB_QUIRKS_CHARGER;
            g_hiusb_info->hiusb_status = HIUSB_DEVICE;
            break;
        case 2:
            g_hiusb_info->intr_flag = ID_FALL_EVENT;
            g_hiusb_info->hiusb_status = HIUSB_OFF;
            g_hiusb_info->vbus_pin = 1;
            break;
        case 3:
            g_hiusb_info->intr_flag = ID_RISE_EVENT;
            g_hiusb_info->hiusb_status = HIUSB_HOST;
            g_hiusb_info->vbus_pin = 1;
            break;
        case 4:
	  case 5:		
        default: 
            g_hiusb_info->intr_flag = 0x55555555;
            g_hiusb_info->hiusb_status = HIUSB_SUSPEND;
            break;         
    }

    hiusb_otg_intr_work(usb_work);
        
    switch (process) {
        case 0:
            if (g_hiusb_info->charger_type == 0x55555555 &&
                g_hiusb_info->hiusb_status == HIUSB_DEVICE) {
                ret = 0;
            }
            break;  
        case 1:
            if (g_hiusb_info->charger_type == CHARGER_REMOVED &&
                g_hiusb_info->hiusb_status == HIUSB_OFF) {
                ret = 0;
            }
            break;
        case 2:
            if (g_hiusb_info->hiusb_status == HIUSB_HOST) {
                ret = 0;
            }
            break;
        case 3:
            if (g_hiusb_info->hiusb_status == HIUSB_OFF) {
                ret = 0;
            }
            break;
        case 4:    
	 case 5:
        default: 
            if (g_hiusb_info->hiusb_status == HIUSB_SUSPEND) {
                ret = 0;
            }
            break;         
    }
    
    return ret;
}

int test_hisi_usb_id_change(int process)
{
    int ret;
    int flag;

    lm_dev.hiusb_info = &test_hiusb_info;
    g_hiusb_info = &test_hiusb_info;
    switch (process) {
        case 0:
            flag = ID_FALL_EVENT;
            break;
        case 1:
            g_hiusb_info = NULL;
            break;
        case 2:
        default:    
            flag = 0x55555555;
            break;         
    }

    ret = hisi_usb_id_change(flag);
        

    return ret?-1:0;
}

int test_hiusb_vbus_intr(int process)
{
    int ret = -1;
    int irq;
    
    lm_dev.hiusb_info = &test_hiusb_info;
    g_hiusb_info = &test_hiusb_info;
    
    switch (process) {
        case 0:
            irq = 0x1;
            g_hiusb_info->insert_irq = irq;
            g_hiusb_info->draw_irq = irq + 1;
            break;
        case 1:
        default:    
            irq = 0x2;;
            g_hiusb_info->insert_irq = irq - 1;
            g_hiusb_info->draw_irq = irq;
            break;         
    }

    hiusb_vbus_intr(irq, &lm_dev);

    switch (process) {
        case 0:
            if (g_hiusb_info->intr_flag == CHARGER_CONNECT_EVENT) {
                ret = 0;
            }
            break;
        case 1:
        default:    
            if (g_hiusb_info->intr_flag == CHARGER_DISCONNECT_EVENT) {
                ret = 0;
            }
            break;         
    }      

    return ret;
}

int test_hiusb_pullup(int process)
{
    int ret = -1;
    int is_on;
    
    lm_dev.hiusb_info = &test_hiusb_info;
    g_hiusb_info = &test_hiusb_info;
    
    switch (process) {
        case 0:
            is_on = 1;
            break;
        case 1:
        default:    
            is_on = 0;
            break;         
    }

    hiusb_pullup(&lm_dev, is_on);

    switch (process) {
        case 0:
            if (g_hiusb_info->gadget_enable == 1) {
                ret = 0;
            }
            break;
        case 1:
        default:    
            if (g_hiusb_info->gadget_enable == 0) {
                ret = 0;
            }
            break;         
    }      

    return ret;
}
#endif
