/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：setting_value_csratlas7.c
* 文件标识：
* 摘 要：  
*
* 当前版本：1.0
* 作 者：   tpeng@foryouge.com.cn
* 完成日期：2017年6月20日
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "hal_setting_value.h"

/*----------------------------------------------*
 * 宏定义                                     *
 *----------------------------------------------*/
#define SIZE_LEN      8
#define DEF_LEN       0
#define OFFSET_LEN    8
#define DEVFILE       "/dev/mmcblk0p11"

int g_iFd=0;

/*----------------------------------------------*/

int opendev(void)
{
	g_iFd=open( DEVFILE ,O_RDWR|O_SYNC);
	if (g_iFd < 0)
	{
		printf("[%s][%d]: opendev err!  \n",__func__,__LINE__);
		return  -1;
	}
    fflush(stdout);
	return 0;
}

int closedev(void)
{
	close(g_iFd);

	return 0;
}

int emmc_read_flag( int off_set )
{
	int ret =-1;
	char * buffer = NULL;
	int i=0;
	int flag =-1;

	buffer = malloc( sizeof(char) * SIZE_LEN );
	if(NULL == buffer)
	{
		printf("[%s][%d]: malloc err!  \n",__func__,__LINE__);
		return  -1;
	}

	memset(buffer,0,SIZE_LEN);

	ret =lseek(g_iFd,off_set,SEEK_SET);
	if(ret<0)
	{
		printf("[%s][%d]: lseek err!  \n",__func__,__LINE__);
		return  -1;
	}

	ret = read(g_iFd,buffer,SIZE_LEN);
	if(ret<0)
	{
		printf("[%s][%d]: read err!  \n",__func__,__LINE__);
		return  -1;
	}
	fsync(g_iFd);

	flag = buffer[0];

	free(buffer);
	return flag;

}

int emmc_write_flag(int off_set ,char flag)
{
	int ret =-1;
	char * buffer = NULL;

	buffer = malloc( sizeof(char) * SIZE_LEN );
	if(NULL == buffer)
	{
		printf("[%s][%d]: malloc err!  \n",__func__,__LINE__);
		return  -1;
	}

	memset(buffer,flag,SIZE_LEN);
	ret = lseek( g_iFd, off_set , SEEK_SET );
	if(ret<0)
	{
		printf("[%s][%d]: lseek err!  \n",__func__,__LINE__);
		return  -1;
	}

	ret = write(g_iFd,buffer,SIZE_LEN);
	if(ret<0)
	{
		printf("[%s][%d]: write err!  \n",__func__,__LINE__);
		return  -1;
	}
	fsync(g_iFd);

	free(buffer);
	return 0;

}

int set_brightness_value(unsigned int value)
{
	int ret =-1;

	ret= emmc_write_flag(DEF_LEN + OFFSET_LEN*2,value);
	if(ret<0)
	{
		printf("[%s][%d] emmc_write_flag err \n",__func__,__LINE__);
		return -1;
	}
	return 0;
}

int get_brightness_value(void)
{
	int iReadBrightnessValue = -1;

	iReadBrightnessValue = emmc_read_flag(DEF_LEN + OFFSET_LEN*2);
	if(iReadBrightnessValue<0)
	{
		printf("[%s][%d] emmc_read_flag err \n",__func__,__LINE__);
		return -1;
	}

	//printf("[%s][%d]======iReadBrightnessValue =%d \n",__func__,__LINE__,iReadBrightnessValue);

	return iReadBrightnessValue;
}

int set_language_value(unsigned int value)
{
	int ret =-1;
	ret= emmc_write_flag( DEF_LEN + OFFSET_LEN ,value);
	if(ret<0)
	{
		printf("[%s][%d] emmc_write_flag err \n",__func__,__LINE__);
		return -1;
	}
	return 0;
}

int get_language_value(void)
{
	int iReadLanguageValue = -1;

	iReadLanguageValue = emmc_read_flag( DEF_LEN + OFFSET_LEN );
	if(iReadLanguageValue<0)
	{
		printf("[%s][%d] emmc_read_flag err \n",__func__,__LINE__);
		return -1;
	}

	//printf("[%s][%d]=======iReadLanguageValue =%d \n",__func__,__LINE__,iReadLanguageValue);

	return iReadLanguageValue;
}

int createSettingValueHal(T_HalSettingValueDevice** device)
{
	T_HalSettingValueDevice *dev;
	dev = (T_HalSettingValueDevice *)malloc(sizeof(T_HalSettingValueDevice));	
    if (dev == NULL)
	{
		return -1;
	}
	
	strcpy(dev->common.version, "1.0");
	strcpy(dev->common.name, "SettingValue HAL for Setting Valuel");

	dev->setEmmcBrightnessValue = set_brightness_value;
	dev->getEmmcBrightnessValue = get_brightness_value;
	dev->setEmmcLanguageValue   = set_language_value;
	dev->getEmmcLanguageValue   = get_language_value;
	dev->common.open   = opendev;
	dev->common.close  = closedev;
	*device = dev;

	return 0;
}

int destorySettingValueHal(T_HalSettingValueDevice** device)
{
	if (*device)
	{		
		free(*device);
		*device = NULL;
	}

	return 0;
}
 
 

/************************ (C) COPYRIGHT Foryou *****END OF FILE****/



