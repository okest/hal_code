/*
* Copyright (c) 2017,����ͨ�õ������޹�˾�������������
* All rights reserved.
*
* �ļ����ƣ�gps_atlas7.c
* �ļ���ʶ��
* ժ Ҫ��   HAL��GPSʵ���ļ�
*
* ��ǰ�汾��1.0
* �� �ߣ�   rxhu@foryouge.com.cn
* ������ڣ�2017��3��20��
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
	UINT time;                  //��������ʱ��
    char pos_state;             //��λ״̬
    float latitude;             //γ��
    float longitude;            //����
    float speed;                //�ƶ��ٶ�
    float direction;            //����
    UINT date;                  //����
    float declination;          //��ƫ��
    char dd;                    //��ƫ�Ƿ���
    char mode;
}GPRMC;

/********************************************************************
  ��������:  OpenGps 
  ����˵��:  ����GPS����
  �������:  ��
  �������:  ��
  ����ֵ:    ���ڵ���0���򿪳ɹ�; С��0����ʧ��
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
  ��������:  CloseGps 
  ����˵��:  �ر�GPS
  �������:  ��
  �������:  ��
  ����ֵ:    ���ڵ���0:�رճɹ�; С��0:�ر�ʧ��
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
 *  ��������:	gps_analysis
 *	����˵��:	��GPS���ݰ��г�ȡ��GPRMC��С��λ��Ϣ
 *  �������:	buff��GPS���ص����ݰ� gps_date��GPRMC��Ϣ�洢�ṹ��
 *  �������:	��
 *  ����ֵ:		����0:�ɹ�;
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
 *  ��������:	print_gps
 *	����˵��:	����GPRMC��С��λ��Ϣ������ӡ����Ļ��
 *  �������:	gps_date��gps_analysis������ȡ��GPRMC��Ϣ
 *  �������:	��
 *  ����ֵ:		����0:�ɹ�;
 ********************************************************************/
 static int print_gps(GPRMC *gps_date)
{
    printf("                                                           \n");
    printf("                                                           \n");
    printf("                                                         \n");
    printf("===========================================================\n");
    printf("==                                                       \n");
    printf("==   GPS״̬λ : %c  [A:��Ч״̬ V:��Ч״̬]             \n",gps_date->pos_state);
    printf("==   GPSģʽλ : %c  [A:������λ D:��ֶ�λ]             \n", gps_date->mode);
    printf("==   ���� : 20%02d-%02d-%02d                             \n",gps_date->date%100,(gps_date->date%10000)/100,gps_date->date/10000);
    printf("==   ʱ�� : %02d:%02d:%02d                               \n",(gps_date->time/10000+8)%24,(gps_date->time%10000)/100,gps_date->time%100);
    printf("==   γ�� : ��γ:%.3f                                    \n",(gps_date->latitude/100));
    printf("==   ���� : ����:%.3f                                    \n",(gps_date->longitude/100));
    printf("==   �ٶ� : %.3f                                         \n",gps_date->speed);
    printf("==                                                       \n");
    printf("===========================================================\n");
    return 0;
} /* ----- End of print_gps()  ----- */

/********************************************************************
 *  ��������:	Print_Gps_Data
 *	����˵��:	���GPS�ɶ��Ե�����
 *  �������:	��
 *  �������:	��
 *  ����ֵ:		����0:�ɹ�;
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
 *  ��������:	Read_Data
 *	����˵��:	���GPSԭʼ����
 *  �������:	gps_buff:�������ݻ����� len:�������ݵĳ���
 *  �������:	��
 *  ����ֵ:		����0:�ɹ�;
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
  ��������:  CreateGpsHal 
  ����˵��:  ����HAL���豸 
  �������:  device��HAL���豸�ṹ��ָ���ַ 
  �������:  �� 
  ����ֵ:    ����0�������ɹ�; С��0������ʧ�� 
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
  ��������:  DestoryGpsHal 
  ����˵��:  �ͷ�HAL���豸
  �������:  device��HAL���豸�ṹ��ָ���ַ 
  �������:  �� 
  ����ֵ:    ����0���ͷųɹ�; 
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







