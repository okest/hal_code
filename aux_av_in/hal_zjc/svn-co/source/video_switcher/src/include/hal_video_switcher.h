/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_video_switcher.h
* 文件标识：
* 摘 要：   HAL层视频源切换器头文件
*
* 当前版本：1.0
* 作 者：   tpeng@foryouge.com.cn
			czhan$foryouge.com.cn
* 完成日期：2017年4月18日
*/

#ifndef _HAL_VIDEO_SWITCHER_H
#define _HAL_VIDEO_SWITCHER_H

#include "hal.h"

#define		VIDEO_SRC_REARVIEW		0
#define		VIDEO_SRC_DVR			1

#define		VIDEO_CHANNEL_ON		1
#define		VIDEO_CHANNEL_OFF		0

#ifdef __cplusplus 
extern "C" 
{ 
#endif

/*
功能：	readRvcGpioData
输入参数：void
返回：返回的值表示gpio data
0表示倒车状态
1表示正常状态
－1表示失败			
*/

/*
功能：	ioctlRvcPowerStatus 倒车控制摄像头电源及读取状态接口
输入参数：
level: 0----设置GPIO状态为低电平,表示关闭摄像头电源    
level: 1----设置GPIO状态为高电平,表示打开摄像头电源 
level: 2---读取GPIO状态

返回：返回的值表示
1 表示RvcPower:  表示GPIO状态为高电平
0 表示RvcPower:  表示GPIO状态为低电平
<0 表示失败			
*/
typedef struct tagVideoSwitcherDevice
{
	T_Device common;            /* 通用HAL层操作 */
	int (*selectVideoSource)(unsigned char video_src_name, unsigned char state);
	int (*readRvcGpioData)(void);
	int (*readRvcPowerGpioData)(void);
	int (*ioctlRvcPowerStatus)(int level);

} T_HalVideoSwitcherDevice,*HalVideoSwitcherDevicePtr;

int createVideoSwitcherHal(T_HalVideoSwitcherDevice** device);  /* 创建设备hal控制 */
int destoryVideoSwitcherHal(T_HalVideoSwitcherDevice** device); /* 释放设备hal控制 */

#ifdef __cplusplus 
}
#endif 

#endif /* _HAL_VIDEO_SWITCHER_H */
