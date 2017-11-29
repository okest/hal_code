/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：backlight_csratlas7.c
* 文件标识：
* 摘 要：   HAL层背光实现文件
*
* 当前版本：1.0
* 作 者：   jczhang
* 完成日期：2017年4月6日
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include "hal_backlight.h"
#include <sys/types.h>
#include <signal.h>
#define DEVICE_NAME "/sys/class/backlight/sirf-backlight/brightness" /* 设备名称 */
#define TMP_BACKLIGHT "/tmp/tmp_backlight" /* 设备名称 */



char before_sleep_value[3];

#if 1
/********************************************************************
  函数名称:  set_tmp_brightness 
  功能说明:  设置
  输入参数:  等级
  输出参数:  无
  返回值:    0:成功; -1:失败
********************************************************************/
int set_brightness_t()
{
	FILE *filp_percent = NULL;
	if(access(TMP_BACKLIGHT,F_OK) == 0) //文件存在 直接读取文件的百分比
	{
		filp_percent = fopen(TMP_BACKLIGHT, "r");
		if(!filp_percent) 
		{
			perror("fopen");
			return -1;
		}
		char buf[3];
		fgets(buf,3, filp_percent);
		//brightness = atoi(buf);
		FILE *filp = NULL;
		filp = fopen(DEVICE_NAME, "w+");/* 文件存在则清零，不存在就新建文件*/
		
		fputs(buf,filp);
		
		fclose(filp_percent);
		filp_percent = NULL;
		fclose(filp);
		filp = NULL;
	}
	
	return 0;
}

#endif

/********************************************************************
  函数名称:  set_brightness 
  功能说明:  设置背光值
  输入参数:  等级
  输出参数:  无
  返回值:    0:成功; -1:失败
********************************************************************/
int set_brightness(int brightness)
{
    int value = brightness;
	if(( value < 0) || (value > 100))
	{
		perror("wrong number \n");
		return false;
	}
	  
    FILE *filp = NULL;
	  
    filp = fopen(DEVICE_NAME, "w+");/* 文件存在则清零，不存在就新建文件*/
    if(!filp) 
	{
	    printf("fopen\n");
	    return -1;
    }
	  
	if(0 == value)
	{
		  fputs("0", filp);
	}else if( value <= 6)
	{
		fputs("1", filp);
	}else if( value <= 12)
	{
		fputs("2", filp);
	}else if( value <= 18)
	{
		fputs("3", filp);
	}else if( value <= 25)
	{
		fputs("4", filp);
	}else if( value <= 31)
	{
		fputs("5", filp);
	}else if( value <= 37)
	{
		fputs("6", filp);
	}else if( value <= 43)
	{
		fputs("7", filp);
	}else if( value <= 50)
	{
		fputs("8", filp);
	}else if( value <= 56)
	{
		fputs("9", filp);
	}else if( value <= 62)
	{
		 fputs("10", filp);
	}else if( value <= 68)
	{
		fputs("11", filp);
	}else if( value <= 75)
	{
		fputs("12", filp);
	}else if( value <= 81)
	{
		fputs("13", filp);
	}else if( value <= 87)
	{
		fputs("14", filp);
	}else if( value <= 93)
	{
		fputs("15", filp);
	}
	else if( value <= 100)
	{
		fputs("16", filp);
	}
	else
	{
	}
    fclose(filp);
    filp = NULL;
	return 0;
}


/********************************************************************
  函数名称:  int open_screen(void); 
  功能说明:  打开熄灭的屏幕,还原原始的亮度
  输入参数:  无
  输出参数:  无
  返回值:    -1->fail   0 ->success
********************************************************************/
int open_screen(void)
{
	printf("-----wake_up_screen-%d-----\n",atoi(before_sleep_value));
	FILE *filp = NULL;
	filp = fopen(DEVICE_NAME, "w+");
	if(!filp) 
	{
		perror("fopen");
		return -1;
	}
	fputs(before_sleep_value,filp);
	fclose(filp);
    filp = NULL;
	printf("******open screen *********\n");
	return 0;
}

/********************************************************************
  函数名称:  int sleep_screen(void);
  功能说明:  熄屏，记忆亮度
  输入参数:  无
  输出参数:  无
  返回值:    -1->fail   0 ->success
********************************************************************/
int close_screen(void)
{
	char current[3];
	FILE *filp = NULL;
    filp = fopen(DEVICE_NAME, "r");/* 文件存在则清零，不存在就新建文件*/
    if(!filp) {
	    perror("fopen");
		return -1;
    }
	fgets(current,3, filp);
	printf("----current is %s\n",current);
	if( atoi(current) == 0)
	{
		printf(" [HAL] you backlight is 0 ,don't close screen again !!!\n");
		return -1;
	}
	strcpy(before_sleep_value,current);
	
	printf("-------save brightness level is %d\n",atoi(before_sleep_value));
	fclose(filp);
	
	filp = fopen(DEVICE_NAME, "w+");
	if(!filp) 
	{
		printf("fopen\n");
		return -1;
	}
	fputs("0",filp);
	fclose(filp);
    filp = NULL;
	printf("******close screen *********\n");
	
#if 1 //add for_daoche
	FILE *filp_percent = NULL;
	filp_percent = fopen(TMP_BACKLIGHT, "w+");	/* 文件存在则清零，不存在就新建文件*/
	fputs(before_sleep_value,filp_percent);
	fclose(filp_percent);
#endif	
	return 0;
}
/********************************************************************
  函数名称:  CreateBacklightHal 
  功能说明:  创建HAL层设备 
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：创建成功; 小于0：创建失败 
********************************************************************/	
int createBacklightHal(T_HalBacklightDevice** device)
{
	T_HalBacklightDevice *dev;
	dev = (T_HalBacklightDevice *)malloc(sizeof(T_HalBacklightDevice));	
    if (dev == NULL)
	{
		return -1;
	}
	strcpy(dev->common.version, "1.0");
	strcpy(dev->common.name, "Backlight HAL For CSRatlas7");
	dev->setBrightnessValue = set_brightness;
	dev->offBacklight = close_screen;
	dev->onBacklight = open_screen;
	#if 1
	dev->setBacklightTmp = set_brightness_t;	//add for tmp
	#endif
	*device = dev;

	return 0;
}

/********************************************************************
  函数名称:  DestoryBacklightHal 
  功能说明:  释放HAL层设备
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:     
********************************************************************/ 
int destoryBacklightHal(T_HalBacklightDevice** device)
{
	if (*device)
	{		
		free(*device);
		*device = NULL;
	}
	return 0;
}
