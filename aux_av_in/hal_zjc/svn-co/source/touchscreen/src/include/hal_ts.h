/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_ts.h
* 文件标识：
* 摘 要：   HAL层触摸屏操作方法
*
* 当前版本：1.0
* 作 者：   jczhang
* 完成日期：2017年5月7日
*/

#ifndef _TS_HAL_H
#define _TS_HAL_H

#include "hal.h"

#ifdef __cplusplus 
extern "C" 
{ 
#endif

typedef struct tagTsDevice
{
	T_Device common;         /* 通用HAL层操作 */
	int (*onScreen)(void);	 /* 使能触摸屏 */
	int (*offScreen)(void);  /* 失能触摸屏 */
} T_HalTsDevice,*HalBTsDevicePtr;

int createTsHal(T_HalTsDevice** device);  /* 创建设备hal控制 */
int destoryTsHal(T_HalTsDevice** device); /* 释放设备hal控制 */

#ifdef __cplusplus 
}
#endif 

#endif 