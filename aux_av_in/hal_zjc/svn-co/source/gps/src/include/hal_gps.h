/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_gps.h
* 文件标识：
* 摘 要：   GPS层头文件
*
* 当前版本：1.0
* 作 者：   rxhu@foryouge.com.cn
* 完成日期：2017年3月20日
*/

#ifndef _HAL_GPS_H
#define _HAL_GPS_H

#include "hal.h"
#ifdef __cplusplus 
extern "C" 
{ 
#endif

typedef struct tagGpsDevice
{
	T_Device common;         /* 通用HAL层操作 */
	int (*readRawGpsData)(char *gps_buff,int len);  /* 读取GPS原始数据 */
	int (*printGpsData)(void);	/* 打印解析后的GPS数据，用于确认定位信息是否正确 */
} T_HalGpsDevice,*HalGpsDevicePtr;


int createGpsHal(T_HalGpsDevice** device);  /* 创建设备hal控制 */
int destoryGpsHal(T_HalGpsDevice** device); /* 释放设备hal控制 */


#ifdef __cplusplus 
}
#endif 

#endif /* _GPS_HAL_H */