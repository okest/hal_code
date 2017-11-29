/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_audio_switcher.h
* 文件标识：
* 摘 要：   HAL层音频切换头文件
*
* 当前版本：1.0
* 作 者：   ylv@foryouge.com.cn
* 完成日期：2017年4月20日
*/

#ifndef _AUDIO_SWITCHER_HAL_H
#define _AUDIO_SWITCHER_HAL_H

#include "hal.h"

#ifdef __cplusplus 
extern "C" 
{ 
#endif
/* the type of audio source,you can modify it if needed */
typedef enum _AudioSource_	
{
  RADIO=0,
  AUX,
  DAB,
  ISDB,
}AudioSourceType;

typedef struct tagAudioSwitcherDevice
{
	T_Device common;         /* 通用HAL层操作 */
	int (*openAudioSource)(AudioSourceType source);/* 打开指定音频源 */
	int (*closeAudioSource)(AudioSourceType source);/* 关闭指定音频源 */
	bool (*isAudioSourceOpened)(AudioSourceType source);/* 查询指定音频源是否打开 */
	
}T_HalAudioSwitcherDevice,*HalAudioSwitcherDevicePtr;

int createAudioSwitcherHal(T_HalAudioSwitcherDevice** device);  /* 创建设备hal控制 */
int destoryAudioSwitcherHal(T_HalAudioSwitcherDevice** device); /* 释放设备hal控制 */

#ifdef __cplusplus 
}
#endif 

#endif /* _AUDIO_SWITCHER_HAL_H */
