/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal.h
* 文件标识：
* 摘 要：   HAL层通用头文件
*
* 当前版本：1.0
* 作 者：   yctang@foryouge.com.cn
* 完成日期：2017年3月20日
*/

#ifndef _HAL_H
#define _HAL_H

typedef struct tagDevice
{
    char version[10];	  /* 版本号 */
    char name[30];        /* 名称 */
	int (*open)(void);    /* 打开设备 */
    int (*close)(void);   /* 关闭设备 */
} T_Device,*DevicePtr;

#endif /* _HAL_H */