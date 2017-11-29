/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_setting_value.h
* 文件标识：
* 摘 要：   
*
* 当前版本：1.0
* 作 者：   tpeng@foryouge.com.cn
* 完成日期：2017年6月20日
*/
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __hal_setting_value_H
#define __hal_setting_value_H

#ifdef __cplusplus 
extern "C" 
{ 
#endif

#include "hal.h"

#ifdef __cplusplus 
extern "C" 
{ 
#endif

/*
背光值:1-15
	int (*setEmmcBrightnessValue)(unsigned int value);
Language值:0---chinese  1---english  
	int (*setEmmcLanguageValue)(unsigned int value);
*/
typedef struct tagSettingValueDevice
{
	T_Device common;            /* 通用HAL层操作 */

	int (*setEmmcBrightnessValue)(unsigned int value);
	int (*getEmmcBrightnessValue)(void);
	int (*setEmmcLanguageValue)(unsigned int value);
	int (*getEmmcLanguageValue)(void);

} T_HalSettingValueDevice,*HalSettingValueDevicePtr;

int createSettingValueHal(T_HalSettingValueDevice** device);  /* 创建设备hal控制 */
int destorySettingValueHal(T_HalSettingValueDevice** device); /* 释放设备hal控制 */

#ifdef __cplusplus 
}
#endif 

#endif /*__hal_setting_value_H */


