/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_beeper.h
* 文件标识：
* 摘 要：   HAL层蜂鸣器音头文件
*
* 当前版本：1.0
* 作 者：   yctang@foryouge.com.cn
* 完成日期：2017年3月20日
*/

#ifndef _BEEPERE_HAL_H
#define _BEEPERE_HAL_H

#include "hal.h"

#ifdef __cplusplus 
extern "C" 
{ 
#endif

typedef enum 
{ 	
	ENUM_HAL_OFF = 0,
	ENUM_HAL_SINGLE_1KHZ = 1000,
	ENUM_HAL_SINGLE_2KHZ = 2000	
}CONFIG_BEEPER_HAL;

struct beeper_param{
        unsigned int delay_ms;
        unsigned int pwm_value;
};

typedef struct beeper_param beeper_param_t;

typedef struct tagBeeperDevice
{
	T_Device common;         /* 通用HAL层操作 */
	int (*setBeeper)(unsigned int value,unsigned int msecs);	 /* 设置蜂鸣器*/
} T_HalBeeperDevice,*HalBeeperDevicePtr;

int createBeeperHal(T_HalBeeperDevice** device);  /* 创建设备hal控制 */
int destoryBeeperHal(T_HalBeeperDevice** device); /* 释放设备hal控制 */

#ifdef __cplusplus 
}
#endif 

#endif /* _BEEPERE_HAL_H */
