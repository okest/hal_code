/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：touchscreen_csratlas7.c
* 文件标识：
* 摘 要：   HAL层触摸屏操作方法
*
* 当前版本：1.0
* 作 者：   jczhang
* 完成日期：2017年5月7日
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <linux/input.h>
#include "hal_ts.h"

#define DEVICE_NAME "/proc/ts_ops" /* 设备名称 */
/********************************************************************
  函数名称:  onScreen 
  功能说明:  打开触摸屏中断
  输入参数:  无
  输出参数:  无
  返回值:    0:成功; -1:失败
********************************************************************/
int on_screen(void)
{
	FILE *filp = NULL;
	filp = fopen(DEVICE_NAME, "w+");
	if(!filp) 
	{
		perror("fopen");
		return -1;
	}
	fputs("01",filp);
	fclose(filp);
	filp = NULL; 
	return 0;
}

/********************************************************************
  函数名称:  offScreen 
  功能说明:  关闭触摸屏
  输入参数:  无
  输出参数:  无
  返回值:    0:成功; -1:失败
********************************************************************/
int off_screen(void)
{
	FILE *filp = NULL;
	filp = fopen(DEVICE_NAME, "w+");
	if(!filp) 
	{
		perror("fopen");
		return -1;
	}
	fputs("02",filp);
	fclose(filp);
	filp = NULL; 
	return 0;
}

/********************************************************************
  函数名称:  createTsHal 
  功能说明:  创建HAL层设备 
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：创建成功; 小于0：创建失败 
********************************************************************/	
int createTsHal(T_HalTsDevice** device)
{
	T_HalTsDevice *dev;
	dev = (T_HalTsDevice *)malloc(sizeof(T_HalTsDevice));	
    if (dev == NULL)
	{
		return -1;
	}
	strcpy(dev->common.version, "1.0");
	strcpy(dev->common.name, "touchscreen HAL For CSRatlas7");
	dev->onScreen = on_screen;
	dev->offScreen = off_screen;
	*device = dev;

	return 0;
}

/********************************************************************
  函数名称:  destoryTsHal 
  功能说明:  释放HAL层设备
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：释放成功; 
********************************************************************/ 
int destoryTsHal(T_HalTsDevice** device)
{
	if (*device)
	{		
		free(*device);
		*device = NULL;
	}

	return 0;
}
 
 
 