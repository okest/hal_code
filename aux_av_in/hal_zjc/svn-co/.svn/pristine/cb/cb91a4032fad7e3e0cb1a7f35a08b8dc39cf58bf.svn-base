/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_config.h
* 文件标识：
* 摘 要：   配置分区HAL层头文件
*
* 当前版本：1.0
* 作 者：   yctang@foryouge.com.cn
* 完成日期：2017年7月22日
*/

#ifndef _HAL_CONFIG_H
#define _HAL_CONFIG_H

#include "hal.h"
#include "log.h"
#include "config.h"

#ifdef __cplusplus 
extern "C" 
{ 
#endif

typedef struct tagConfigDevice
{
	T_Device common;         /* 通用HAL层操作 */
	
	int (*setUserConfig)(T_UserConfig *config);	/* 设置用户信息 */
	int (*getUserConfig)(T_UserConfig *config);	/* 获取用户信息 */
	int (*getFactoryConfig)(T_FactoryConfig *config);   /* 获取用户信息 */	
} T_HalConfigDevice,*HalConfigDevicePtr;

int createConfigHal(T_HalConfigDevice** device);  /* 创建设备hal控制 */
int destoryConfigHal(T_HalConfigDevice** device); /* 释放设备hal控制 */

#ifdef __cplusplus 
}
#endif 

#endif /* _HAL_CONFIG_H */
