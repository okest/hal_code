/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：config.h
* 文件标识：
* 摘 要：   配置分区通用头文件
*
* 当前版本：1.0
* 作 者：   yctang@foryouge.com.cn
* 完成日期：2017年7月22日
*/

#ifndef _CONFIG_H
#define _CONFIG_H

/* 用户配置信息 */
typedef struct tagUserConfig
{
    unsigned int upgrade_flag1;	    /* 升级标志位1 */
    unsigned int upgrade_flag2;     /* 升级标志位2 */
	unsigned int brightness;        /* 当前亮度值 */
    unsigned int language;     	    /* 当前语言 */
} T_UserConfig,*UserConfigPtr;

/* 工厂配置信息 */
typedef struct tagFactoryConfig
{
    unsigned int cpu_uuid;	   /* cpu UUID */
	char nav_uuid[17];    /* 导航 UUID */

} T_FactoryConfig,*FactoryConfigPtr;

#endif /* _CONFIG_H */
                                                                             
