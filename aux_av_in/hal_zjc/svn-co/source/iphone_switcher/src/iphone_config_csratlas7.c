/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：iphone_config_csratlas7.c
* 文件标识：
* 摘 要：   iphone配置切换选择
*
* 当前版本：1.0
* 作 者：   zjc
* 完成日期：2017年5月9日
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <linux/input.h>
#include "hal_iphone_conf.h"
#include <string.h>

#include "libusb.h"
#include <sys/types.h>
#include <dirent.h>

#define SYSFS_DEVICE_PATH "/sys/bus/usb/devices"

int usb_bus;

char usb_path[BUFSIZ];

/***************************************
  函数名称:  set_configuration
  功能说明:  设置配置
  输入参数:  int c
  输出参数:  无
  返回值:    等于0:成功; 小于0:失败
****************************************/
int set_configuration(CONFIG_DEF_HAL c)
{
	//sprintf(usb_path,"%s/usb%d/%d-1",SYSFS_DEVICE_PATH,usb_bus,usb_bus);
	//printf("open dir is %s\n",usb_path);
	
	sprintf(usb_path,"%s/usb%d/%d-1/%s",SYSFS_DEVICE_PATH,usb_bus,usb_bus,"bConfigurationValue");
	//printf("open dir is %s\n",usb_path);
	
	FILE *fp_w = NULL;
 	fp_w = fopen(usb_path, "w+"); /*被写的文件： 文件存在则截断0，不存在就新建文件 */
	if(!fp_w) {
	    perror("fopen");
	    return -1;
      }
	switch(c)
	{
		case ENUM_HAL_PTP:fputs("1", fp_w);break;
		case ENUM_HAL_IPOD:fputs("2", fp_w);break;
		case ENUM_HAL_PTP_Apple_Mobile_Device:fputs("3", fp_w);break;
		case ENUM_HAL_ETHERNET:fputs("4", fp_w);break;
	}
	fclose(fp_w);
	return 0;
}

/***************************************
  函数名称:  select_usb_bus
  功能说明:  选择总线
  输入参数:  devs
  输出参数:  无
  返回值:    等于0:成功; 小于0:失败
****************************************/
static void select_usb_bus(libusb_device **devs,CONFIG_DEF_HAL c)
{
	libusb_device *dev;
	int i = 0;

	while ((dev = devs[i++]) != NULL) 
	{
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			fprintf(stderr, "failed to get device descriptor");
			return;
		}
		if(desc.idVendor == 0x05ac)
		{
			printf("<+SET IC +> %04x:%04x (bus %d, device %d)\n",
			desc.idVendor, desc.idProduct,libusb_get_bus_number(dev), libusb_get_device_address(dev));
			
			usb_bus = libusb_get_bus_number(dev);
			break;
		}
	}
	
	printf("<+SET IC +> change configurationValue\n");
	set_configuration(c);
	
}

/********************************************************************
  函数名称:  int set_configuration_iphone(int c)
  功能说明:  选择配置
  输入参数:  配置值
  输出参数:  无
  返回值:    等于0:成功; 小于0:失败
********************************************************************/
int set_configuration_for_iphone(CONFIG_DEF_HAL c)
{
	libusb_device **devs;
	int r;
	ssize_t cnt;

	r = libusb_init(NULL);
	if (r < 0)
		return r;

	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0)
		return (int) cnt;

	select_usb_bus(devs,c);
	libusb_free_device_list(devs, 1);

	libusb_exit(NULL);
	return 0;
}


/********************************************************************
  函数名称:  int choose_right_usb_device(void)
  功能说明:  获取配置
  输入参数:  无
  输出参数:  无
  返回值:    int 1,2,3,4
********************************************************************/
int choose_right_usb_device(libusb_device **devs)
{
	int c;
	libusb_device *dev;
	int i = 0;

	while ((dev = devs[i++]) != NULL) 
	{
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) 
		{
			fprintf(stderr, "failed to get device descriptor");
			return -1;
		}
		if(desc.idVendor == 0x05ac)
		{
			printf("<-GET IC-> %04x:%04x (bus %d, device %d)\n",desc.idVendor, desc.idProduct,
			libusb_get_bus_number(dev), libusb_get_device_address(dev));
			static struct libusb_device_handle *devh = NULL;
			devh = libusb_open_device_with_vid_pid(NULL, desc.idVendor, desc.idProduct);
			if (!devh) 
			{
				fprintf(stderr, "Error finding USB device\n");
			}
			
			libusb_get_configuration(devh,&c);
			break;
		}
	}
	return c;
}

/********************************************************************
  函数名称:  int get_configuration_iphone(void)
  功能说明:  获取配置
  输入参数:  无
  输出参数:  无
  返回值:    int 1,2,3,4
********************************************************************/
int get_configuration_iphone(void)
{
	libusb_device **devs;
	int r;
	ssize_t cnt;

	r = libusb_init(NULL);
	if (r < 0)
		return r;

	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0)
		return (int) cnt;

	int c = choose_right_usb_device(devs);
	if( c > 0)
	{
		printf("<-GET IC-> iphone conf is %d\n",c);
	}
	
	libusb_free_device_list(devs, 1);

	libusb_exit(NULL);
	return c;
}



/********************************************************************
  函数名称:  CreateBeeperHal 
  功能说明:  创建HAL层设备 
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：创建成功; 小于0：创建失败 
********************************************************************/	
int createIphoneHal(T_HalIphoneDevice** device)
{
	T_HalIphoneDevice *dev;
	dev = (T_HalIphoneDevice *)malloc(sizeof(T_HalIphoneDevice));	
    if (dev == NULL)
	{
		return -1;
	}
	strcpy(dev->common.version, "1.0");
	strcpy(dev->common.name, "Iphone conf CSRatlas7");	
	dev->setConfigIphone = set_configuration_for_iphone;
	dev->getConfigIphone = get_configuration_iphone;
	*device = dev;

	return 0;
}

/********************************************************************
  函数名称:  DestoryBeeperHal 
  功能说明:  释放HAL层设备
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：释放成功; 
********************************************************************/ 
int destoryIphoneHal(T_HalIphoneDevice** device)
{
	if (*device)
	{		
		free(*device);
		*device = NULL;
	}

	return 0;
}
 
 
 