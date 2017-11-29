/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：aux_in_status_check_csratlas7.h
* 文件标识：
* 摘 要：   HAL层头文件
*
* 当前版本：1.0
* 作 者：   jczhang@foryouge.com.cn
* 完成日期：2017年11月16日
*/

#ifndef _AUX_AV_IN_SW_HAL_H
#define _AUX_AV_IN_SW_HAL_H

#include "hal.h"

#ifdef __cplusplus 
extern "C" 
{ 
#endif
/*
*	radio:		swa:0	swb:0	
*	rear aux	swa:1	swb:0
*	front aux	swa:0	swb:1
*	ISDB:		swa:1	swb:1
*/

typedef enum 
{ 	
	ENUM_HAL_RADIO = 0,  
	ENUM_HAL_REAR_AUX,
	ENUM_HAL_FRONT_AUX ,
	ENUM_HAL_ISDB 	
}CONFIG_AUDIO_HAL;

typedef struct 
{
	int is_aux_used;			//data for  app 
	
}AUX_STATUS;

typedef void (*auxStateChangedCallBack)(AUX_STATUS  * paux_in_status_check);

typedef struct tagAuxInStatusCheckDevice
{
	T_Device common;         													/* 通用HAL层操作 */
	int (* aux_in_status_Init)(void);	 										/* 初始化*/
	int (* registerAuxinStateChangedCallBack)(auxStateChangedCallBack pAuxStateCb); 	/*	注册回调函数*/
	int (* chooseAudioLineinChannel)(CONFIG_AUDIO_HAL CMD);
	int (* getCurrentAudioChannelStatus)(CONFIG_AUDIO_HAL CMD,int * status);
	
} T_HalAuxInStatusCheckDevice,*HalAuxInStatusCheckDevicePtr;

int createAuxInStatusCheckHal(T_HalAuxInStatusCheckDevice** device);  /* 创建设备hal控制 */
int destoryAuxInStatusCheckHal(T_HalAuxInStatusCheckDevice** device); /* 释放设备hal控制 */

#ifdef __cplusplus 
}
#endif 

#endif /* _BEEPERE_HAL_H */
