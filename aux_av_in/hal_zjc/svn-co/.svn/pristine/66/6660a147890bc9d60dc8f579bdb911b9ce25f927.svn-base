/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：beeper_csratlas7.c
* 文件标识：
* 摘 要：   HAL层蜂鸣器音实现文件
*
* 当前版本：1.0
* 作 者：   yctang@foryouge.com.cn
* 完成日期：2017年3月20日
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <linux/input.h>
#include "hal_beeper.h"

#define DEVICE_NAME "/dev/input/event2" /* 设备名称 */

static int fd = -1; /* 设备描述符 */

/********************************************************************
  函数名称:  openBeeper 
  功能说明:  打开驱动设备
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0：打开成功; 小于0：打开失败
********************************************************************/
static int openBeeper(void)
{
	fd = open(DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		perror("OpenBeeper() failed");
	}
	
	return fd;
}

/********************************************************************
  函数名称:  closeBeeper 
  功能说明:  关闭驱动设备
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0:关闭成功; 小于0:关闭失败
********************************************************************/
static int closeBeeper(void)
{
	return close(fd);	
}

/********************************************************************
  函数名称:  beeperSet 
  功能说明:  控制蜂鸣器
  输入参数:  c：频率
  输出参数:  无
  返回值:    大于等于0:成功; 小于0:失败
********************************************************************/
static int beeperSet(CONFIG_BEEPER_HAL value)
{
	struct input_event event;
	event.type = EV_SND;
	event.code = SND_TONE; 
	event.value = value;
	
	return write(fd, &event, sizeof(struct input_event));
}


/********************************************************************
  函数名称:  createBeeperHal 
  功能说明:  创建HAL层设备 
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：创建成功; 小于0：创建失败 
********************************************************************/	
int createBeeperHal(T_HalBeeperDevice** device)
{
	T_HalBeeperDevice *dev;
	dev = (T_HalBeeperDevice *)malloc(sizeof(T_HalBeeperDevice));	
    if (dev == NULL)
	{
		return -1;
	}
	strcpy(dev->common.version, "1.0");
	strcpy(dev->common.name, "Beeper HAL For CSRatlas7");
    dev->common.open = openBeeper;
	dev->common.close = closeBeeper;	
	dev->setBeeper = beeperSet;
	
	*device = (T_HalBeeperDevice *)dev;

	return 0;
}

/********************************************************************
  函数名称:  destoryBeeperHal 
  功能说明:  释放HAL层设备
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:     
********************************************************************/ 
int destoryBeeperHal(T_HalBeeperDevice** device)
{
	T_HalBeeperDevice **dev = (T_HalBeeperDevice **)device;	
	if (*dev)
	{		
		free(*dev);
		*dev = NULL;
	}

	return 0;
}
 
 
 
