/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_cvbs_signal_check.c
* 文件标识：
* 摘 要：   HAL层视频源切换器实现文件
*
* 当前版本：1.0
* 作 者：   tpeng@foryouge.com.cn
* 完成日期：2017年5月23日
*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/reboot.h>
#include<sys/mount.h>  
#include<errno.h> 　
#include<dirent.h>
#include<signal.h>
#include<ctype.h>
#include<termios.h>
#include<sys/mman.h>
#include <time.h>

#include "hal_cvbs_check.h"

/*----------------------------------------------*
 * 宏定义                                     *
 *----------------------------------------------*/
#define  MAP_SIZE 4096UL
#define  MAP_MASK (MAP_SIZE - 1)

#define  CVBSADDR  0x10db3010  

int g_iFd=0;

/*----------------------------------------------*/
/*
功能：readResult	   cvbsCheck  from A7 Reg	
输入参数：void			
返回：成功dwRead_result   为CVBS寄存器中的状态  
   －1表示失败		
*/

int readResult(void)
{
    char *pMap_base, *pVirt_addr; 
    unsigned long dwRead_result;

    /* Map one page */
    pMap_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, g_iFd, CVBSADDR & ~MAP_MASK);
    if( pMap_base == (void *) -1) 
	{
		printf("[%s][%d]:  hal_rvc mmap err!  \n",__func__,__LINE__);
		return  -1;
	}
    //fflush(stdout);

    pVirt_addr = pMap_base + (CVBSADDR & MAP_MASK);

    dwRead_result = *((unsigned long *) pVirt_addr);

    //printf("hal_cvbs_signal_check Read :   0x%X (%p): 0x%X\n", CVBSADDR, pVirt_addr, dwRead_result); 
    //fflush(stdout);

    if(munmap(pMap_base, MAP_SIZE) == -1)
	{
		printf("[%s][%d]:  hal_rvc munmap err!  \n",__func__,__LINE__);
		return  -1;
	}
    return dwRead_result;
}

/*
功能：cvbsCheck	
输入参数：void			
返回：成功 iResult   根据状态来判断信号的状态
1 表示有信号
0表示无信号
-1表示读取失败
-2表示CAMERA_NO_CONNECTE CAMERA没有连接				
*/
int cvbsCheck(void)
{
	int  dwReadValue=0;
	int  iResult=0;

	dwReadValue= readResult();

	if( dwReadValue ==-1 )
	{	
		iResult = -1;
		printf("[%s][%d][v1.3]  readResult error\n",__func__,__LINE__);
		return  iResult;
	}

	if( dwReadValue ==-2 )
	{	
		iResult = -2;
		printf("[%s][%d][v1.3]  readResult Not CONNECTE CAMERA \n",__func__,__LINE__);
		return  iResult;
	}

	if( dwReadValue ==0x03 || dwReadValue ==0x0B || dwReadValue ==0x05 || dwReadValue ==0x0D )
	{
		if( ((dwReadValue >>2) & 0x01) == 0x01 )
		{
			iResult =0;

			//printf("[%s][%d]  iResult  =%d \n",__func__,__LINE__,iResult);
		}
		else if( ((dwReadValue >>2) & 0x01) == 0x00 )
		{
			iResult =1;

			//printf("[%s][%d]  iResult  =%d \n",__func__,__LINE__,iResult);
		}
	}
	else
	{
		iResult =0;
	}
	//printf("\n[%s][%d]  iResult  =%d \n",__func__,__LINE__,iResult);

    return iResult;
}

int openMem(void)
{

	g_iFd=open("/dev/mem",O_RDWR|O_SYNC);
	if (g_iFd < 0)
	{
		printf("[%s][%d]: hal_cvbs_signal_check open /dev/mem err!  \n",__func__,__LINE__);
		return  -1;
	}
    fflush(stdout);
	return 0;
}

int closeMem(void)
{
	
	close(g_iFd);

	return 0;
}

int createCvbsCheckHal(T_HalCvbsCheckDevice** device)
{
	T_HalCvbsCheckDevice *dev;
	dev = (T_HalCvbsCheckDevice *)malloc(sizeof(T_HalCvbsCheckDevice));	
    if (dev == NULL)
	{
		return -1;
	}
	
	strcpy(dev->common.version, "1.3");
	strcpy(dev->common.name, "CvbsCheck HAL for video signal");
    dev->common.open  = openMem;
	dev->common.close = closeMem;
	dev->cvbsCheck    = cvbsCheck;
	
	*device = dev;

	return 0;
}

int destoryCvbsCheckHal(T_HalCvbsCheckDevice** device)
{
	if (*device)
	{		
		free(*device);
		*device = NULL;
	}

	return 0;
}
 
 

/************************ (C) COPYRIGHT Foryou *****END OF FILE****/



