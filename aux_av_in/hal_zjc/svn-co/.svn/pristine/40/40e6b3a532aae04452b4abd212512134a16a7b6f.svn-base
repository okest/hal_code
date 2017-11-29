/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_iphone_conf.h
* 文件标识：
* 摘 要：   HAL层iphone配置切换头文件
*
* 当前版本：1.0
* 作 者：   zjc
* 完成日期：2017年5月9日
*/

#ifndef _HAL_IPHONE_CONFIGURATION_H
#define _HAL_IPHONE_CONFIGURATION_H

#include "hal.h"

#ifdef __cplusplus 
extern "C" 
{ 
#endif

typedef enum 
{ 
	ENUM_HAL_PTP  = 1,  
	ENUM_HAL_IPOD =2,   				   /* ipod */
	ENUM_HAL_PTP_Apple_Mobile_Device =3,  
	ENUM_HAL_ETHERNET = 4                  /* Ethernet*/
}CONFIG_DEF_HAL;


typedef struct tagIphoneDevice
{
	T_Device common;                          /* 通用HAL层操作 */
	int (*setConfigIphone)(CONFIG_DEF_HAL c); /* 设置iPhone配置值 */             	
	int (*getConfigIphone)(void);			  /* 获取iPhone配置值 */
} T_HalIphoneDevice,*HalIphoneDevicePtr;

int createIphoneHal(T_HalIphoneDevice** device);  /* 创建设备hal控制 */
int destoryIphoneHal(T_HalIphoneDevice** device); /* 释放设备hal控制 */

#ifdef __cplusplus 
}
#endif 

#endif 