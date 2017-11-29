/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_diagnosis.h
* 文件标识：
* 摘 要：   HAL故障码头文件
*
* 当前版本：1.0
* 作 者：   jczhang@foryouge.com.cn
* 完成日期：2017年4月27日
*/

#ifndef _HAL_DIAGNOSIS_H
#define _HAL_DIAGNOSIS_H

#include "hal.h"
#include "hal_dia_enum.h"
#ifdef __cplusplus 
extern "C" 
{ 
#endif




typedef struct tagDiagnosisDevice
{
	T_Device common;                                     /* 通用HAL层操作 */
	//int (*writeDiagnosis)(char *dia_str);              /*上报故障码，存储*/
	
	int (*writeDiagnosis)(CONFIG_DIAGNOSIS_HEAD head,CONFIG_DIAGNOSIS_CODE code,CONFIG_DIAGNOSIS_PARA para);   /*上报故障码，存储*/
	const char *(*readDiagnosis)();                      /*获取故障码的信息*/
	int (*writeDiagnosisUpdateZone)(char *update_info);  /*写更新信息*/
	char *(*readDiagnosisUpdateZone)();                  /*读更新信息*/
	int (*writeDiagnosisHeadZone)(char *head_info);      /*写故障码的头部信息*/
	char *(*readDiagnosisHeadZone)();                    /*读故障码的头部信息*/
	
} T_HalDiagnosisDevice,*HalDiagnosisDevicePtr;

int createDiagnosisHal(T_HalDiagnosisDevice** device);  /* 创建设备hal控制 */
int destoryDiagnosisHal(T_HalDiagnosisDevice** device); /* 释放设备hal控制 */

#ifdef __cplusplus 
}
#endif 

#endif 