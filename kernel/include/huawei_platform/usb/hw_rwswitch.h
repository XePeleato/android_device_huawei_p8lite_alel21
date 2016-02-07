/*
 * huawei rework switch header.
 *
 */

#ifndef __DRIVERS_USB_HW_RWSWITCH_H__
#define __DRIVERS_USB_HW_RWSWITCH_H__


//USB vendor customized request to switch USB port mode;
#define  USB_REQ_VENDOR_SWITCH_MODE	0xA5

/* //must match with frameworks\base\services\java\com\android\server\usb\UsbDeviceManager.java
//USB Port Mode defination for USB_PortMode switch
USB_PortMode = new String[25];
... ...
 */
//USB_PortMode switch index define
#define INDEX_ENDUSER_SWITCH            0x11    //0x11 设置为USB端口模式17: "pcmodem2";
#define INDEX_FACTORY_REWORK            0x0E   //0x0E 设置为USB端口模式14: "manufacture,adb";
#define INDEX_USB_REWORK_SN            0x0D   //0x0D 设置为USB端口模式13;


int usb_port_switch_request(int usb_switch_index);
int usb_port_mode_get(void);

#endif /* __DRIVERS_USB_HW_RWSWITCH_H__ */
