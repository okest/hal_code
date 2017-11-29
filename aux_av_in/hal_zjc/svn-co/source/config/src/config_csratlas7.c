/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：config_csratlas7.c
* 文件标识：
* 摘 要：   HAL层读写eMMC配置分区数据实现文件
*
* 当前版本：1.0
* 作 者：  czhan@foryouge.com.cn yctang@foryouge.com.cn	  
* 完成日期：2017年8月4日
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include "hal_config.h"

#define DEV_NAME_CFG_EMMC "/dev/mmcblk0p11"	/* eMMC配置分区 */
#define DEV_NAME_CFG_DDR "/dev/rw_ddr_cfg"	/* 读写拷贝到DDR的配置数据区域 */

static int fd_emmc_cfg = -1; /* eMMC配置分区设备描述符 */
static int fd_ddr_cfg = -1; /* DDR配置数据区域设备描述符 */

#define USER_CFG_OFFSET	(0) /* 用户配置信息起始在分区偏移0字节位置*/
#define FACTORY_CFG_OFFSET	(512*1024) /* 工厂配置信息起始在分区偏移512K字节位置*/

/********************************************************************
  函数名称:  openConfig 
  功能说明:  打开驱动设备
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0：打开成功; 小于0：打开失败
********************************************************************/
static int openConfig(void)
{
	fd_emmc_cfg = open(DEV_NAME_CFG_EMMC, O_RDWR);
	if (fd_emmc_cfg < 0)
	{
		LOGD("Open "DEV_NAME_CFG_EMMC" failed");
		return fd_emmc_cfg;
	}

	fd_ddr_cfg = open(DEV_NAME_CFG_DDR, O_RDWR);
	if (fd_ddr_cfg < 0)
	{
		LOGD("Open "DEV_NAME_CFG_DDR" failed");
		return fd_ddr_cfg;
	}

	return 0;
}

/********************************************************************
  函数名称:  closeConfig 
  功能说明:  关闭驱动设备
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0:关闭成功; 小于0:关闭失败
********************************************************************/
static int closeConfig(void)
{
	int ret = -1;

	ret = close(fd_emmc_cfg);
	if(ret < 0)
	{
		LOGD("Open "DEV_NAME_CFG_EMMC" failed");
		return ret;
	}

	ret = close(fd_ddr_cfg);
	if(ret < 0)
	{
		LOGD("Open "DEV_NAME_CFG_DDR" failed");
		return ret;
	}

	return 0;	
}

/********************************************************************
  函数名称:  ddrRead 
  功能说明:  从DDR中读取配置数据
  输入参数:  offset	偏移地址
			buf    读出数据的存放起始地址
			count  读出的字节数
  输出参数:  无
  返回值:    大于等于0:写入成功; 小于0:写入失败
********************************************************************/
static int ddrRead(unsigned int offset, void* buf, unsigned int count)
{
	int ret = -1;

	ret = lseek(fd_ddr_cfg, offset, SEEK_SET);
	if (ret < 0)
	{
		LOGD("lseek(fd_ddr_cfg, offset, SEEK_SET) failed");
		return  -1;
	}

	ret = read(fd_ddr_cfg, buf, count);
	if (ret < 0)
	{
		LOGD("read(fd_ddr_cfg, buf, count) failed");
		return  -1;
	}
	
	return 0;
}

/********************************************************************
  函数名称:  ddrWirte 
  功能说明:  配置数据写入DDR
  输入参数:  offset	偏移地址
			buf    待写入数据的起始地址
			count  写入的字节数
  输出参数:  无
  返回值:    大于等于0:写入成功; 小于0:写入失败
********************************************************************/
static int ddrWirte(unsigned int offset, const void* buf, unsigned int count)
{
	int ret = -1;

	ret = lseek(fd_ddr_cfg, offset, SEEK_SET);
	if (ret < 0)
	{
		LOGD("lseek(fd_ddr_cfg, offset, SEEK_SET) failed");
		return  -1;
	}

	ret = write(fd_ddr_cfg, buf, count);
	if (ret < 0)
	{
		LOGD("write(fd_ddr_cfg, buf, count) failed");
		return  -1;
	}
	
	return 0;
}

/********************************************************************
  函数名称:  emmcWirte 
  功能说明:  配置数据写入eMMC
  输入参数:  offset	偏移地址
			buf    待写入数据的起始地址
			count  写入的字节数
  输出参数:  无
  返回值:    大于等于0:写入成功; 小于0:写入失败
********************************************************************/
static int emmcWirte(unsigned int offset, const void* buf, unsigned int count)
{
	int ret = -1;

	ret = lseek(fd_emmc_cfg, offset, SEEK_SET);
	if (ret < 0)
	{
		LOGD("lseek(fd_emmc_cfg, offset, SEEK_SET) failed");
		return  -1;
	}

	ret = write(fd_emmc_cfg, buf, count);
	if (ret < 0)
	{
		LOGD("write(fd_emmc_cfg, buf, count) failed");
		return  -1;
	}
	
	return fsync(fd_emmc_cfg);
}

/********************************************************************
  函数名称:  setUserConfig 
  功能说明:  配置用户数据
  输入参数:  config	待写入的用户配置数据T_UserConfig结构体指针
  输出参数:  无
  返回值:    大于等于0:写入成功; 小于0:写入失败
********************************************************************/
int setUserConfig(T_UserConfig *config)
{
	int ret = -1;

	if (config == NULL)
	{
		LOGD("\"config\" should not be NULL");
		return -1;
	}
	
	ret = ddrWirte(USER_CFG_OFFSET, config, sizeof(T_UserConfig));
	if(ret < 0)
	{
		LOGD("ddrWirte(USER_CFG_OFFSET, config, sizeof(T_UserConfig)) failed");
		return -2;
	}

	ret = emmcWirte(USER_CFG_OFFSET, config, sizeof(T_UserConfig));
	if(ret < 0)
	{
		LOGD("emmcWirte(USER_CFG_OFFSET, config, sizeof(T_UserConfig)) failed");
		return -3;
	}

	return 0;
}

/********************************************************************
  函数名称:  getUserConfig 
  功能说明:  获取用户配置数据
  输入参数:  config	待配置的用户配置数据T_UserConfig结构体指针
  输出参数:  无
  返回值:    大于等于0:写入成功; 小于0:写入失败
********************************************************************/
int getUserConfig(T_UserConfig *config)
{
	int ret = -1;

	if (config == NULL)
	{
		LOGD("\"config\" should not be NULL");
		return -1;
	}

	ret = ddrRead(USER_CFG_OFFSET, config,  sizeof(T_UserConfig));
	if(ret < 0)
	{
		LOGD("ddrRead(USER_CFG_OFFSET, config,  sizeof(T_UserConfig)) failed");
		return -2;
	}

	return 0;
}

/********************************************************************
  函数名称:  getFactoryConfig 
  功能说明:  获取工厂配置数据
  输入参数:  config	待配置的工厂配置数据T_FactoryConfig结构体指针
  输出参数:  无
  返回值:    大于等于0:写入成功; 小于0:写入失败
********************************************************************/
int getFactoryConfig(T_FactoryConfig *config)
{
	int ret = -1;

	if(config == NULL)
	{
		LOGD("\"config\" should not be NULL");
		return -1;
	}

	ret = ddrRead(FACTORY_CFG_OFFSET, config, sizeof(T_FactoryConfig));
	if(ret < 0)
	{
		LOGD("ddrRead(FACTORY_CFG_OFFSET, config, sizeof(T_FactoryConfig)) failed");
		return -2;
	}

	return 0;
}

/********************************************************************
  函数名称:  createConfigHal 
  功能说明:  创建HAL层设备 
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：创建成功; 小于0：创建失败 
********************************************************************/	
int createConfigHal(T_HalConfigDevice** device)
{
	T_HalConfigDevice *dev;
	dev = (T_HalConfigDevice *)malloc(sizeof(T_HalConfigDevice));	
    if (dev == NULL)
	{
		LOGD("\"device\" should not be NULL");
		return -1;
	}
	strcpy(dev->common.version, "1.0");
	strcpy(dev->common.name, "Config HAL For CSRatlas7");
    dev->common.open = openConfig;
	dev->common.close = closeConfig;	
	dev->setUserConfig = setUserConfig;
	dev->getUserConfig = getUserConfig;	
	dev->getFactoryConfig = getFactoryConfig;	
	
	*device = (T_HalConfigDevice *)dev;

	return 0;
}

/********************************************************************
  函数名称:  destoryConfigHal 
  功能说明:  释放HAL层设备
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：删除成功; 小于0：删除失败  
********************************************************************/ 
int destoryConfigHal(T_HalConfigDevice** device)
{
	if (*device)
	{		
		free(*device);
		*device = NULL;
		return 0;
	}

	LOGD("destory ConfigHal failed");
	return -1;
}
 
 
 
