/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：log.h
* 文件标识：
* 摘 要：   log头文件
*
* 当前版本：1.0
* 作 者：   yctang@foryouge.com.cn
* 完成日期：2017年6月23日
*/

#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>

#define DEBUG
#define LOGR(format, ...) fprintf(stdout, ""format"\n", ##__VA_ARGS__) 

#ifdef DEBUG 
#define LOGD(format, ...) fprintf(stdout, "[File:%s][Line:%d] "format"\n",__FILE__, __LINE__,##__VA_ARGS__)
#else 
#define LOGD(format, ...) 
#endif

#endif /* _LOG_H */
