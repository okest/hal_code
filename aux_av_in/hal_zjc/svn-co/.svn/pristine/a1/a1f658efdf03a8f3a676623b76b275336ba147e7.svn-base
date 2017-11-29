/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：usb_power_tps2549.c
* 文件标识：
* 摘 要：   HAL层usb电源管理实现文件
*
* 当前版本：1.0
* 作 者：   khli@foryouge.com.cn
* 完成日期：2017年4月12日
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "hal_usb_power.h"

#define DEVICE_NAME "/dev/tps254x" /* 设备名称 */

static int fd = -1; /* 设备描述符 */

/********************************************************************
  函数名称:  openUsbPower
  功能说明:  打开驱动设备
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0：打开成功; 小于0：打开失败
********************************************************************/
static int openUsbPower(void)
{
	fd = open(DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		perror("openUsb_Power() failed");
	}
	
	return fd;
}

/********************************************************************
  函数名称:  closeUsbPower 
  功能说明:  关闭驱动设备
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0:关闭成功; 小于0:关闭失败
********************************************************************/
static int closeUsbPower(void)
{
	return close(fd);	
}

/********************************************************************
  函数名称:  setUsbDcp
  功能说明:  设置usb电源管理为dcp模式
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0:成功; 小于0:失败
********************************************************************/
static int setUsbDcp(void)
{
	int value = 1;
	int ret;
	ret=write(fd, &value, sizeof(char));
	return ret;
}

/********************************************************************
  函数名称:  setUsbCdp
  功能说明:  设置usb电源管理为cdp模式
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0:成功; 小于0:失败
********************************************************************/
static int setUsbCdp(void)
{
	int value = 0;
	int ret;
	ret=write(fd, &value, sizeof(char));
	return ret;
}

/********************************************************************
  函数名称:  setUsbSdp
  功能说明:  设置usb电源管理为sdp模式
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0:成功; 小于0:失败
********************************************************************/
static int setUsbSdp(void)
{
	return 0;
}

/*
功能：	readusbpowerGpioData
输入参数：void
返回：返回的值表示iGpiodata
0表示倒车状态
1表示正常状态
－1表示失败			
*/
int readusbpowerGpioData(void)
{

	int ret =-1;
	char buffer[0]={1};
	int iGpiodata =1;

	ret = read( fd,buffer,1 );
	if(ret<0)
	{
		printf("[%s][%d]:  read err!  \n",__func__,__LINE__);
		return  -1;
	}
	fsync(fd);

	iGpiodata = buffer[0];

	//printf("[%s][%d]:  iGpiodata =  0x%x \n\n",__func__,__LINE__,iGpiodata); 

	return iGpiodata;
}

/********************************************************************
  函数名称:  createUsbPowerHal 
  功能说明:  创建HAL层设备 
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：创建成功; 小于0：创建失败 
********************************************************************/	
int createUsbPowerHal(T_HalUsb_powerDevice** device)
{
	T_HalUsb_powerDevice *dev;
	dev = (T_HalUsb_powerDevice *)malloc(sizeof(T_HalUsb_powerDevice));	
    if (dev == NULL)
	{
		return -1;
	}
	strcpy(dev->common.version, "1.0");
	strcpy(dev->common.name, "Usb_power HAL For TPS2549");
    dev->common.open = openUsbPower;
	dev->common.close = closeUsbPower;	
	dev->setDcp = setUsbDcp;
	dev->setCdp = setUsbCdp;
	dev->setSdp = setUsbSdp;
	dev->readusbpowerGpioData = readusbpowerGpioData;

	*device = (T_HalUsb_powerDevice *)dev;

	return 0;
}

/********************************************************************
  函数名称:  destoryUsbPowerHal 
  功能说明:  释放HAL层设备
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:     
********************************************************************/ 
int destoryUsbPowerHal(T_HalUsb_powerDevice** device)
{
	T_HalUsb_powerDevice **dev = (T_HalUsb_powerDevice **)device;	
	if (*dev)
	{		
		free(*dev);
		*dev = NULL;
	}

	return 0;
}
 
