/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：gps_atlas7.c
* 文件标识：
* 摘 要：   HAL层GPS实现文件
*
* 当前版本：1.0
* 作 者：   rxhu@foryouge.com.cn
* 完成日期：2017年3月20日
*/
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>


#include "hal_gps.h"

typedef unsigned int pit_t;
pid_t demon;

typedef unsigned int UINT;
typedef int BYTE;
typedef long int WORD;
#define GPS_LEN 512 

typedef struct __gprmc__
{
	UINT time;                  //格林威治时间
    char pos_state;             //定位状态
    float latitude;             //纬度
    float longitude;            //经度
    float speed;                //移动速度
    float direction;            //方向
    UINT date;                  //日期
    float declination;          //磁偏角
    char dd;                    //磁偏角方向
    char mode;
}GPRMC;

/********************************************************************
  函数名称:  OpenGps 
  功能说明:  启动GPS功能
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0：打开成功; 小于0：打开失败
********************************************************************/
static int OpenGps(void)
{
	pit_t pid;
	printf("open the Gps now please wait....\n");
	if((pid = fork()) < 0)
	{
		perror("fork error");
		return -1;
	}
	else if(pid > 0)
	{
		demon = getpid();
		printf("demon = %d\n",demon);	
	}
	else if(pid == 0)
	{
		printf("come to the child\n");
		if(execl("/usr/bin/GnssSllDaemon","&",(char *)0) < 0)
			perror("execl error");	
	}
	demon++;
	sleep(10);
	
	return 0;
}

/********************************************************************
  函数名称:  CloseGps 
  功能说明:  关闭GPS
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0:关闭成功; 小于0:关闭失败
********************************************************************/
static int CloseGps(void)
{
	int ret;
	ret = kill(demon,SIGKILL);
	if(ret < 0)
	{
		printf("failed to close the GPS\n");
		return -1;
	}
	return 0;	
}

/********************************************************************
 *  函数名称:	gps_analysis
 *	功能说明:	从GPS数据包中抽取出GPRMC最小定位信息
 *  输入参数:	buff：GPS返回的数据包 gps_date：GPRMC信息存储结构体
 *  输出参数:	无
 *  返回值:		等于0:成功;
 ********************************************************************/ 
static int gps_analysis(char *buff,GPRMC *gps_date)
{
	char *ptr=NULL;
	if(gps_date==NULL)
      return -1;
	
	if(strlen(buff)<10)
      return -1;
  
	if(NULL==(ptr=strstr(buff,"$GPRMC")))
     return -1;
 
	sscanf(ptr,"$GPRMC,%d.000,%c,%f,N,%f,E,%f,%f,%d,,,%c*",&(gps_date->time),&(gps_date->pos_state),
		&(gps_date->latitude),&(gps_date->longitude),&(gps_date->speed),&(gps_date->direction),
		&(gps_date->date),&(gps_date->mode));
	
	return 0;
}

/********************************************************************
 *  函数名称:	print_gps
 *	功能说明:	解析GPRMC最小定位信息，并打印到屏幕上
 *  输入参数:	gps_date：gps_analysis函数抽取的GPRMC信息
 *  输出参数:	无
 *  返回值:		等于0:成功;
 ********************************************************************/
 static int print_gps(GPRMC *gps_date)
{
    printf("                                                           \n");
    printf("                                                           \n");
    printf("                                                         \n");
    printf("===========================================================\n");
    printf("==                                                       \n");
    printf("==   GPS状态位 : %c  [A:有效状态 V:无效状态]             \n",gps_date->pos_state);
    printf("==   GPS模式位 : %c  [A:自主定位 D:差分定位]             \n", gps_date->mode);
    printf("==   日期 : 20%02d-%02d-%02d                             \n",gps_date->date%100,(gps_date->date%10000)/100,gps_date->date/10000);
    printf("==   时间 : %02d:%02d:%02d                               \n",(gps_date->time/10000+8)%24,(gps_date->time%10000)/100,gps_date->time%100);
    printf("==   纬度 : 北纬:%.3f                                    \n",(gps_date->latitude/100));
    printf("==   经度 : 东经:%.3f                                    \n",(gps_date->longitude/100));
    printf("==   速度 : %.3f                                         \n",gps_date->speed);
    printf("==                                                       \n");
    printf("===========================================================\n");
    return 0;
} /* ----- End of print_gps()  ----- */

/********************************************************************
 *  函数名称:	Print_Gps_Data
 *	功能说明:	输出GPS可读性的数据
 *  输入参数:	无
 *  输出参数:	无
 *  返回值:		等于0:成功;
 ********************************************************************/
static int Print_Gps_Data(void)
{	
     int fd = 0;
     int nread = 0;
	 int i = 0;
     GPRMC gprmc;
     char gps_buff[GPS_LEN];
     char *dev_name = "/tmp/agiin";
	 printf("come to Read_Data<========================================>\n");
     fd=open(dev_name,O_RDWR|O_NOCTTY|O_NDELAY);
     if(fd<0)
     {
         printf("open ttyS1 error!!\n");
         return -1;
     }
    // set_opt( fd,4800,8,'N',1);
     while(i < 3)
     {
         sleep(2);
         nread = read(fd,gps_buff,sizeof(gps_buff));
         if(nread<0)
         {
             printf("read GPS date error!!\n");
             return -2;
         }
 //        printf("gps_buff: %s\n", gps_buff);
         memset(&gprmc, 0 , sizeof(gprmc));
         gps_analysis(gps_buff,&gprmc);
         print_gps(&gprmc);
		 i++;
     }
    close(fd);
    return 0;
}

/********************************************************************
 *  函数名称:	Read_Data
 *	功能说明:	输出GPS原始数据
 *  输入参数:	gps_buff:接收数据缓冲区 len:接收数据的长度
 *  输出参数:	无
 *  返回值:		等于0:成功;
 ********************************************************************/
static int Read_Data(char *gps_buff,int len)
{	
     int fd = 0;
     int nread = 0;
     char *dev_name = "/tmp/agiin";
     fd=open(dev_name,O_RDWR|O_NOCTTY|O_NDELAY);
     if(fd<0)
     {
         printf("open ttyS1 error!!\n");
         return -1;
     }
	 sleep(1);
     nread = read(fd,gps_buff,len);
     if(nread<0)
     {
         printf("read GPS date error!!\n");
         return -2;
     }
    close(fd);
    return 0;
}

/********************************************************************
  函数名称:  CreateGpsHal 
  功能说明:  创建HAL层设备 
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：创建成功; 小于0：创建失败 
********************************************************************/	
int createGpsHal(T_HalGpsDevice** device)
{
	T_HalGpsDevice *dev;
	dev = (T_HalGpsDevice *)malloc(sizeof(T_HalGpsDevice));	
    if (dev == NULL)
	{
		return -1;
	}
	strcpy(dev->common.version, "1.0");
	strcpy(dev->common.name, "Gps HAL For CSRatlas7");
	dev->common.open = OpenGps;
	dev->common.close = CloseGps;
	dev->readRawGpsData = Read_Data;
	dev->printGpsData = Print_Gps_Data;
//	printf("=================================>we not get the function !!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	*device = dev;
	return 0;
}

/********************************************************************
  函数名称:  DestoryGpsHal 
  功能说明:  释放HAL层设备
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：释放成功; 
********************************************************************/ 
int destoryGpsHal(T_HalGpsDevice** device)
{
	if (*device)
	{		
		free(*device);
		*device = NULL;
	}

	return 0;
}







