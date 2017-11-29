/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：aux_in_status_check_csratlas7.c
* 文件标识：
* 摘 要：   HAL层audio_sw实现文件
*
* 当前版本：1.0
* 作 者：   jczhang@foryouge.com.cn
* 完成日期：2017年11月16日
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <linux/input.h>

#include <signal.h>
#include "hal_aux_in_status_check.h"

#include <unistd.h>
#include <stdlib.h>

#include <pthread.h>



#define DEVICE_NAME "/dev/aux_det_dev" /* 设备名称 */


static AUX_STATUS aux_status;

static int fd = -1; /* 设备描述符 */

typedef void (*auxStateChangedCallBack)(AUX_STATUS  *  paux_in_status_check);

static auxStateChangedCallBack  pAuxStateChangedCallBack = NULL;


void sync_handler(int num)
{
		sleep(1);
		read(fd,&aux_status,sizeof(AUX_STATUS));
		printf("{HAL:foryou}--recv async singnal-(out :[0] in:[1]) {current aux_status.is_aux_used = %d}\n",aux_status.is_aux_used);
		pAuxStateChangedCallBack(&aux_status);			//调用注册的函数（处理一些逻辑）
		
}
	
/********************************************************************
  函数名称:  __registerAuxinStateChangedCallBack 
  功能说明:  注册回调函数
  输入参数:  
  输出参数:  无
  返回值:    大于等于0:成功; 小于0:失败
********************************************************************/
int __registerAuxinStateChangedCallBack(auxStateChangedCallBack pAuxStateCb)
{
	if(pAuxStateCb == NULL)
	{
		return -1;
	}else
	{
		printf("<HAL:foryou>register a callback function\n");
		pAuxStateChangedCallBack = pAuxStateCb;
		return 0;
	}
}

/********************************************************************
  函数名称:  __aux_in_status_Init 
  功能说明:  aux in 的状态监测
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0:成功; 小于0:失败
********************************************************************/
int  __aux_in_status_Init(void)
{
	int oflags;
	
	fd = open (DEVICE_NAME, O_RDWR);//,S_IRUSR | S_IWUSR);

	if(fd > 0)
	{ 
		printf("open  /dev/aux_det_dev ok\n");
	} 
	else
	{	
		printf("open  /dev/aux_det_dev fail\n");
	}

	signal(SIGIO, sync_handler);
	fcntl(fd, F_SETOWN, getpid());
	oflags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, oflags | FASYNC);

	while (1);
	
	return 0;
}


/********************************************************************
  函数名称:  createAuxInStatusCheckHal 
  功能说明:  创建HAL层设备 
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：创建成功; 小于0：创建失败 
********************************************************************/	
int createAuxInStatusCheckHal(T_HalAuxInStatusCheckDevice** device)
{
	T_HalAuxInStatusCheckDevice *dev;
	dev = (T_HalAuxInStatusCheckDevice *)malloc(sizeof(T_HalAuxInStatusCheckDevice));	
    if (dev == NULL)
	{
		return -1;
	}
	strcpy(dev->common.version, "1.0");
	strcpy(dev->common.name, "aux in status check  HAL For CSRatlas7");
    	
	dev->aux_in_status_Init = __aux_in_status_Init;
	dev->registerAuxinStateChangedCallBack =  __registerAuxinStateChangedCallBack;
	
	*device = (T_HalAuxInStatusCheckDevice *)dev;

	return 0;
}

/********************************************************************
  函数名称:  destoryAuxInStatusCheckHal 
  功能说明:  释放HAL层设备
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:     
********************************************************************/ 
int destoryAuxInStatusCheckHal(T_HalAuxInStatusCheckDevice** device)
{
	T_HalAuxInStatusCheckDevice **dev = (T_HalAuxInStatusCheckDevice **)device;	
	if (*dev)
	{		
		free(*dev);
		*dev = NULL;
	}

	return 0;
}
 
 
 
