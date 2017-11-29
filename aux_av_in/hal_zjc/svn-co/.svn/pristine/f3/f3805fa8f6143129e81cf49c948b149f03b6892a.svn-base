/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_backlight.h
* 文件标识：
* 摘 要：   HAL层背光头文件
*
* 当前版本：1.0
* 作 者：   jczhang
* 完成日期：2017年4月6日
*/

#ifndef _HAL_BACKLIGHT_H
#define _HAL_BACKLIGHT_H

#include "hal.h"

#ifdef __cplusplus 
extern "C" 
{ 
#endif


typedef struct tagBackLightDevice
{
	T_Device common;        			       /* 通用HAL层操作 */
	int (*setBrightnessValue)(int brightness); /* 设置背光值 */
	int (*offBacklight)(void);                 /* 关闭背光值*/
	int (*onBacklight)(void);				   /* 打开背光值*/
	int (*setBacklightTmp)(void);			  	/*配合offBacklight （poweroff）打开背光值*/
} T_HalBacklightDevice,*HalBacklightDevicePtr;

int createBacklightHal(T_HalBacklightDevice** device);  /* 创建设备hal控制 */
int destoryBacklightHal(T_HalBacklightDevice** device); /* 释放设备hal控制 */


#ifdef __cplusplus 
}
#endif 

#endif
