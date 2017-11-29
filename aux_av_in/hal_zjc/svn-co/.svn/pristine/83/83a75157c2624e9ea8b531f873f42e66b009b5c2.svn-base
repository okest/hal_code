/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_usb_power.h
* 文件标识：
* 摘 要：   HAL层usb电源管理头文件
*
* 当前版本：1.0
* 作 者：   khli@foryouge.com.cn
* 完成日期：2017年4月6日
*/

#ifndef _HAL_USB_POWER_H
#define _HAL_USB_POWER_H

#include "hal.h"

#ifdef __cplusplus 
extern "C" 
{
#endif

typedef struct tagUsb_powerDevice
{
	T_Device common;         /* 通用HAL层操作 */
	int (*setDcp)(void);     /*设置usb电源管理模式为dcp模式*/
	int (*setCdp)(void);     /*设置usb电源管理模式为cdp模式*/
	int (*setSdp)(void);     /*设置usb电源管理模式为sdp模式*/
	int (*readusbpowerGpioData)(void);

} T_HalUsb_powerDevice,*HalUsb_powerDevicePtr;

int createUsbPowerHal(T_HalUsb_powerDevice** device);  /* 创建设备hal控制 */
int destoryUsbPowerHal(T_HalUsb_powerDevice** device); /* 释放设备hal控制 */

#ifdef __cplusplus 
}
#endif 

#endif /* _HAL_USB_POWER_H */
