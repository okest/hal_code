/*
* Copyright (c) 2017,����ͨ�õ������޹�˾�������������
* All rights reserved.
*
* �ļ����ƣ�hal_gps.h
* �ļ���ʶ��
* ժ Ҫ��   GPS��ͷ�ļ�
*
* ��ǰ�汾��1.0
* �� �ߣ�   rxhu@foryouge.com.cn
* ������ڣ�2017��3��20��
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
	T_Device common;         /* ͨ��HAL����� */
	int (*readRawGpsData)(char *gps_buff,int len);  /* ��ȡGPSԭʼ���� */
	int (*printGpsData)(void);	/* ��ӡ�������GPS���ݣ�����ȷ�϶�λ��Ϣ�Ƿ���ȷ */
} T_HalGpsDevice,*HalGpsDevicePtr;


int createGpsHal(T_HalGpsDevice** device);  /* �����豸hal���� */
int destoryGpsHal(T_HalGpsDevice** device); /* �ͷ��豸hal���� */


#ifdef __cplusplus 
}
#endif 

#endif /* _GPS_HAL_H */