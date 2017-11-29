/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_cvbs_signal_check.h
* 文件标识：
* 摘 要：   HAL层视频源切换器实现文件
*
* 当前版本：1.0
* 作 者：   tpeng@foryouge.com.cn
* 完成日期：2017年5月23日
*/
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __hal_cvbs_check_H
#define __hal_cvbs_check_H

#include "hal.h"


#ifdef __cplusplus 
extern "C" 
{ 
#endif

/*
功能：cvbsCheck	
输入参数：void			
返回：成功 iResult   根据状态来判断信号的状态
1 表示有信号
0表示无信号
-1表示读取失败
-2表示CAMERA_NO_CONNECTE CAMERA没有连接				
*/
typedef struct tagCvbsCheckDevice
{
	T_Device common;            /* 通用HAL层操作 */
	int (*cvbsCheck)(void);
} T_HalCvbsCheckDevice,*HalCvbsCheckDevicePtr;

int createCvbsCheckHal(T_HalCvbsCheckDevice** device);  /* 创建设备hal控制 */
int destoryCvbsCheckHal(T_HalCvbsCheckDevice** device); /* 释放设备hal控制 */


#ifdef __cplusplus 
}
#endif 

#endif /*__hal_cvbs_signal_check_H */


