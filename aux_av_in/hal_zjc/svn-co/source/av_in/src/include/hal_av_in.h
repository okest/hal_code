/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_av_in.h
* 文件标识：
* 摘 要：   HAL层头文件
*
* 当前版本：1.0
* 作 者：   jczhang@foryouge.com.cn
* 完成日期：2017年3月20日
*/

#ifndef _AV_IN_HAL_H
#define _AV_IN_HAL_H

#include "hal.h"

#ifdef __cplusplus 
extern "C" 
{ 
#endif


typedef struct beeper_param beeper_param_t;

typedef struct tagBeeperDevice
{
	T_Device common;         						/* 通用HAL层操作 */
	int (*av_in_init)(void);						/*初始换 av_in 状态*/
	int (*av_in_start)(void);						/*启动  av_in 的视频信号*/
	int (*av_in_stop)(void);						/*停止	av_in 的视频信号*/
	int (*av_in_deinit)(void);						/**/
	int (*check_av_in_status)(int *p_bconnect);		/*检测av_in 的信号状态*/
	
} T_HalAvinDevice,*HalAvinDevicePtr;

int createAvinHal(T_HalAvinDevice** device);  /* 创建设备hal控制 */
int destoryAvinHal(T_HalAvinDevice** device); /* 释放设备hal控制 */

#ifdef __cplusplus 
}
#endif 

#endif 
