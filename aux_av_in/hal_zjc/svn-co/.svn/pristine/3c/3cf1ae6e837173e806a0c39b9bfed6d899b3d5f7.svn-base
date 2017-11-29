/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_dvr_display.h
* 文件标识：
* 摘 要：   HAL层视频源切换器实现文件
*
* 当前版本：1.0
* 作 者：   tpeng@foryouge.com.cn
			ganrzhang@foryouge.com.cn
* 完成日期：2017年7月28日
*/
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __hal_dvr_display_H
#define __hal_dvr_display_H

#include "hal.h"

#ifdef __cplusplus 
extern "C" 
{ 
#endif

/*
功能：DVR 信号检测
输入参数：	int *p_bconnect		
返回：返回值：成功为0，出错为-1	
int (*dvr_status_get)(int *p_bconnect);
p_bconnect
0------------无信号
1------------有信号	
*/	


typedef struct tagDvrDisplayDevice
{
	T_Device common;            /* 通用HAL层操作 */

	int (*dvr_init)(void);
	int (*dvr_start)(void);
	int (*dvr_stop)(void);
	int (*dvr_deinit)(void);
	int (*dvr_status_get)(int *p_bconnect);

} T_HalDvrDisplayDevice,*HalDvrDisplayDevicePtr;

int createDvrDisplayHal(T_HalDvrDisplayDevice** device);  /* 创建设备hal控制 */
int destoryDvrDisplayHal(T_HalDvrDisplayDevice** device); /* 释放设备hal控制 */

#ifdef __cplusplus 
}
#endif 

#endif 


