/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：audio_switcher_74HC4052.c
* 文件标识：
* 摘 要：   HAL层音频源切换实现文件
*
* 当前版本：1.0
* 作 者：   ylv@foryouge.com.cn
* 完成日期：2017年4月20日
*/
#include <stdint.h>	
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <unistd.h>
#include <stdbool.h>
#include "hal_audio_switcher.h"

#define AUDIO_SWITCHER_MAGIC_NUM  'x'
#define OPEN_RADIO _IO(AUDIO_SWITCHER_MAGIC_NUM,1000)
#define OPEN_AUX _IO(AUDIO_SWITCHER_MAGIC_NUM,1001)
#define OPEN_DAB _IO(AUDIO_SWITCHER_MAGIC_NUM,1002)
#define OPEN_ISDB _IO(AUDIO_SWITCHER_MAGIC_NUM,1003)
 
#define DEVICE_NAME "/dev/audio_switcher" /* 设备名称 */

static int fd = -1; /* 设备描述符 */

/********************************************************************
  函数名称:  OpenAudioSwitcher
  功能说明:  打开驱动设备
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0：打开成功; 小于0：打开失败
********************************************************************/
static int openAudioSwitcher(void)
{
	fd = open(DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		perror("OpenAudioSwitcher() failed");
	}
	
	return fd;
}

/********************************************************************
  函数名称:  CloseAudioSwitcher 
  功能说明:  关闭驱动设备
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0:关闭成功; 小于0:关闭失败
********************************************************************/
static int closeAudioSwitcher(void)
{
	return close(fd);	
}

/********************************************************************
  函数名称:  openAudioSource 
  功能说明:  打开指定音频源
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0:成功; 小于0:失败
********************************************************************/
int openAudioSource(AudioSourceType source)
{	
	/*if(source < RADIO || source >ISDB)
	{
		printf("%s:invalid parameter\n",__FUNCTION__);
		return -1;
	}*/
	
  	switch(source)
	{
	case RADIO:
		if(ioctl(fd,OPEN_RADIO)) 
		{
			printf("%s:can't switch to Radio\n",__FUNCTION__);
			return -1;
		}
		return 0;
	case AUX:
		if(ioctl(fd,OPEN_AUX))
		 {
			printf("%s:can't switch to Aux\n",__FUNCTION__);
			return -1;
		}
		return 0;
	case DAB:
		if(ioctl(fd,OPEN_DAB)) 
		{
			printf("%s:can't switch to Dab\n",__FUNCTION__);
			return -1;
		}
		return 0;
	case ISDB:
		if(ioctl(fd,OPEN_ISDB)) 
		{
			printf("%s:can't switch to Isdb\n",__FUNCTION__);
			return -1;
		}
		return 0;
	default:
		{
	         	printf("%s:invalid parameter\n",__FUNCTION__);
			return -1;
		}
	}
//	return 1;
}

/********************************************************************

  函数名称:  closeAudioSource 
  功能说明:  关闭指定音频源 
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0:成功; 小于0:失败
  备注：硬件无法实现单独关闭某个源而不影响其他源，所以这里函数为空 
********************************************************************/
int closeAudioSource(AudioSourceType source)
{
	return 0;
}

/********************************************************************

  函数名称:  isRadioOpened
  功能说明:  查询指定音频源是否打开 
  输入参数:  无
  输出参数:  无
  返回值:    指定音频源打开为true;指定音频源关闭或者查询出错为false
********************************************************************/
bool isAudioSourceOpened(AudioSourceType source)
{
	int ret;
	char buf[128];
	buf[0]='\0';
	/*if(source < RADIO || source >ISDB)
	{
		printf("%s:invalid parameter\n",__FUNCTION__);
		return -1;
	}*/
	
  	switch(source){
	case RADIO:
		if((ret = read(fd, buf, 128))<=0) 
		{
			printf("can't get the audio switcher state\n");
			return false;
		}
		if(buf[0] != '0')
		{
		return false;
		}
		return true;
	case AUX:
		if((ret = read(fd, buf, 128))<=0) 
		{
			printf("can't get the audio switcher state\n");
			return false;
		}
		if(buf[0] != '1')
		{
		return false;
		}
		return true;
	case DAB:
		if((ret = read(fd, buf, 128))<=0) 
		{
			printf("can't get the audio switcher state\n");
			return false;
		}
		if(buf[0] != '2')
		{
		return false;
		}
		return true;
	case ISDB:
		if((ret = read(fd, buf, 128))<=0) 
		{
			printf("can't get the audio switcher state\n");
			return false;
		}
		if(buf[0] != '3')
		{
		return false;
		}
		return true;
	default:
		{
	         printf("%s:invalid parameter\n",__FUNCTION__);
			return false;
		}

	}
	return 0;
}

/********************************************************************
  函数名称:  createAudioSwitcherHal 
  功能说明:  创建HAL层设备 
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：创建成功; 小于0：创建失败 
********************************************************************/	
int createAudioSwitcherHal(T_HalAudioSwitcherDevice** device)
{
	T_HalAudioSwitcherDevice *dev;
	dev = (T_HalAudioSwitcherDevice *)malloc(sizeof(T_HalAudioSwitcherDevice));	
    if (dev == NULL)
	{
		return -1;
	}
	strcpy(dev->common.version, "1.0");
	strcpy(dev->common.name, "AudioSwitcher HAL For CSRatlas7");
        dev->common.open = openAudioSwitcher;
	dev->common.close = closeAudioSwitcher;	
	dev->openAudioSource = openAudioSource;
	dev->closeAudioSource = closeAudioSource;
	dev->isAudioSourceOpened = isAudioSourceOpened;
	
	*device = dev;

	return 0;
}

/********************************************************************
  函数名称:  destoryAudioSwitcherHal 
  功能说明:  释放HAL层设备
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：释放成功; 
********************************************************************/ 
int destoryAudioSwitcherHal(T_HalAudioSwitcherDevice** device)
{
	if (*device)
	{		
		free(*device);
		*device = NULL;
	}
	return 0;
}
 

