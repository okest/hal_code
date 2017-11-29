/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：upgrade_m3_atlas7.c
* 文件标识：
* 摘 要：   
*
* 当前版本：1.0
* 作 者：   tpeng@foryouge.com.cn
** 完成日期：2017年6月5日
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


#include "hal_upgrade_m3.h"

/*----------------------------------------------*
 * 宏定义                                     *
 *----------------------------------------------*/
#define  MAP_SIZE 4096UL
#define  MAP_MASK (MAP_SIZE - 1)

//============================================
#define  MEM_START_ADDR 	0x45410000
#define  M3_ISO_SIZE_ADDR  	0x45410000
#define  M3_FILE_CRC_ADDR  	0x4540fff4 
#define  M3_STATUS_ADDR    	0x4540fff8
#define  M3_FILE_SIZE_ADDR 	0x4540fffc
#define  A7_STATUS_ADDR    	0x4540fff0 

#define  A7_TO_M3_ADDR1  	0xAAAA6666  
#define  A7_TO_M3_ADDR2  	0xAAAA5555   
#define  A7_TO_M3_ADDR3  	0xAAAA4444

#define  M3_WRAIT_UPDATE 	0xAABBC000

//============================================
#define MEM_PRINT_LEN   128
#define VERSION   "Upgrade M3 HAL V3.9"

/*----------------------------------------------*
 * 变量定义                                      *
 *----------------------------------------------*/

int g_iM3_status =0;
int g_iFd        =0;
int g_iFlag       =0;
FILE * g_pFile;

unsigned long  g_dwFile_Size		=0;
unsigned long  g_dwChecknum	=0;
char * g_pBuffer   =NULL; 


/*----------------------------------------------*
 * 内部函数说明                                	*
 *----------------------------------------------*/
unsigned long get_file_size(void);
unsigned int crc16(unsigned char *pBuf, unsigned int dwLength);
unsigned long CRC16_Check(unsigned char *pBuf, unsigned int dwLength);
unsigned long get_CRC16_Checknum(void);

/*----------------------------------------------*
 *  				                       		*
 *----------------------------------------------*/
unsigned long get_file_size(void)
{
	unsigned long  dwFileSize;
	dwFileSize = g_dwFile_Size;
	return dwFileSize;
}

unsigned int crc16(unsigned char *pBuf, unsigned int dwLength)
{
    unsigned int i;
    unsigned int j;
    unsigned int c;
    unsigned int dwCrc = 0xFFFF;

	if(NULL == pBuf )
	{
		printf("[%s][%d]: pBuf null \n",__func__,__LINE__);
		return 0;
	}

    for (i=0; i<dwLength; i++)
    {
        c = *(pBuf+i) & 0x00FF;
        dwCrc^=c;
        for (j=0; j<8; j++)
        {
             if (dwCrc & 0x0001)
             {
                dwCrc >>= 1;
                dwCrc ^= 0xA001;
             }
             else
             { 
                dwCrc >>= 1;
             }
        }
   }

   dwCrc = (dwCrc>>8) + (dwCrc<<8);

   return(dwCrc);
}

unsigned long CRC16_Check(unsigned char *pBuf, unsigned int dwLength)
{
	unsigned long  dwChecknum = crc16(pBuf,dwLength);
	return dwChecknum;
}

unsigned long get_CRC16_Checknum(void)
{
	unsigned long  dwCheck = g_dwChecknum;
	return dwCheck;
}

int devmem_read(unsigned long dwTargetAddr)
{
    char *pMap_base, *pVirt_addr; 
    unsigned long dwRead_result;

    pMap_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, g_iFd, dwTargetAddr & ~MAP_MASK);
    if( pMap_base == (void *) -1) 
	{
		printf("[%s][%d]:  mmap err!  \n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}

    //printf("Memory mapped at address :  %p. \n", pMap_base); 
    //fflush(stdout);

    pVirt_addr = pMap_base + (dwTargetAddr & MAP_MASK);

    dwRead_result = *((unsigned long *) pVirt_addr);

    printf("Read Value at address 0x%X (%p): 0x%X\n", dwTargetAddr, pVirt_addr, dwRead_result); 
    fflush(stdout);

    if(munmap(pMap_base, MAP_SIZE) == -1)
	{
		printf("[%s][%d]:  munmap err!  \n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}
    return dwRead_result;
}

int devmem_write(unsigned long dwTargetAddr,unsigned long dwWritevall)
{
    char *pMap_base, *pVirt_addr; 
    unsigned long dwRead_result;

    pMap_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, g_iFd, dwTargetAddr & ~MAP_MASK);
    if( pMap_base == (void *) -1) 
	{
		printf("[%s][%d]:  mmap err!  \n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}

    //printf("Memory mapped at address :  %p. \n", pMap_base); 
    //fflush(stdout);

    pVirt_addr = pMap_base + (dwTargetAddr & MAP_MASK);

    dwRead_result = *((unsigned long *) pVirt_addr);

    *((unsigned long *) pVirt_addr) = dwWritevall;
    dwRead_result = *((unsigned long *) pVirt_addr);

    printf("Write  data :  0x%X; readback 0x%X\n", dwWritevall, dwRead_result); 
    fflush(stdout);

    if(munmap(pMap_base, MAP_SIZE) == -1)
	{
		printf("[%s][%d]:  munmap err!  \n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}
    return MCU_UPDATE_OK;
}

int devmem_read_Log(unsigned long dwTargetAddr)
{
    void *pMap_base, *pVirt_addr; 
    unsigned long dwRead_result;

    pMap_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, g_iFd, dwTargetAddr & ~MAP_MASK);
    if( pMap_base == (void *) -1) 
	{
		printf("[%s][%d]:  mmap err!  \n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}

    pVirt_addr = pMap_base + (dwTargetAddr & MAP_MASK);

    dwRead_result = *((unsigned long *) pVirt_addr);

    if(munmap(pMap_base, MAP_SIZE) == -1)
	{
		printf("[%s][%d]:  munmap err!  \n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}
    return dwRead_result;
}

int devmem_write_Log(unsigned long dwTargetAddr,unsigned long dwWritevall)
{
    void *pMap_base, *pVirt_addr; 
    unsigned long dwRead_result;

    pMap_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, g_iFd, dwTargetAddr & ~MAP_MASK);
    if( pMap_base == (void *) -1) 
	{
		printf("[%s][%d]:  mmap err!  \n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}

	pVirt_addr = pMap_base + (dwTargetAddr & MAP_MASK);
	dwRead_result = *((unsigned long *) pVirt_addr);

	*((unsigned long *) pVirt_addr) = dwWritevall;
	dwRead_result = *((unsigned long *) pVirt_addr);

    if(munmap(pMap_base, MAP_SIZE) == -1)
	{
		printf("[%s][%d]:  munmap err!  \n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}
    return dwRead_result;
}

int Read_Status(void)
{
	unsigned long  dwRead_Value[5] ={0};
	unsigned int l=0;

	printf("\n=====================================================================read mem\n");

	dwRead_Value[0] = devmem_read(M3_ISO_SIZE_ADDR);
	if(dwRead_Value[0] ==-1)
	{	
		printf("[%s][%d]  devmem_read error\n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}

	dwRead_Value[1] = devmem_read(M3_FILE_CRC_ADDR);
	if(dwRead_Value[1] ==-1)
	{	
		printf("[%s][%d]  devmem_read error\n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}

	dwRead_Value[2] = devmem_read(M3_STATUS_ADDR);
	if(dwRead_Value[2] ==-1)
	{	
		printf("[%s][%d]  devmem_read error\n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}

	dwRead_Value[3] = devmem_read(M3_FILE_SIZE_ADDR);
	if(dwRead_Value[3] ==-1)
	{	
		printf("[%s][%d]  devmem_read error\n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}

	dwRead_Value[4] = devmem_read(A7_STATUS_ADDR);
	if(dwRead_Value[4] ==-1)
	{	
		printf("[%s][%d]  devmem_read error\n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}

	for(l=0;l<5;l++)
	{
		printf("  0x%x  ",dwRead_Value[l]); 
	}

	printf("\n===================================================================== \n");
	return  MCU_UPDATE_OK;
}

//==================================================================================================

int  get_upgrade_mcu_wait_upgrade_status(void)
{
	unsigned long dwM3_wait_upgrade_status = 0;

	while(1)
	{
		g_iM3_status = devmem_read(M3_STATUS_ADDR);
		if(g_iM3_status==-1)
		{	
			printf("[%s][%d]  devmem_read error  : 0x%x\n",__func__,__LINE__,g_iM3_status);
			return  MCU_UPDATE_ERR;
		}

		if(g_iM3_status==M3_WRAIT_UPDATE)
		{
			printf("[%s][%d] : M3_STATUS_ADDR=0x%x : g_iM3_status = 0x%x\n",__func__,__LINE__,M3_STATUS_ADDR, g_iM3_status); 

			dwM3_wait_upgrade_status=  g_iM3_status;
			break;
		}
	}
	return dwM3_wait_upgrade_status;
}

int write_data_to_ddr(unsigned long dwAddr,unsigned long dwWrite)
{
	int iRet =-1 ;
	iRet = devmem_write(dwAddr,dwWrite);
	if(iRet==-1)
	{	
		printf("[%s][%d]  devmem_write error\n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}
}

int mem_open(void)
{
	printf("[%s][%d] ****************VERSION=%s******************* \n",__func__,__LINE__,VERSION);

	g_iFd=open("/dev/mem",O_RDWR|O_SYNC);
	if (g_iFd < 0)
	{
		printf("[%s][%d]: open /dev/mem err!  \n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}
    fflush(stdout);

	return MCU_UPDATE_OK;
}

/*************************************************************
//1,文件映像 0x45410000  
//2,文件校验 0x4540fff4  
//3,M3状态 0x4540fff8	
//4,文件大小 0x4540fffc  
//5,A7状态 0x4540fff0  

*************************************************************/
/*
功能：	upgrade_init_status		
输入参数：void		
返回：0表示成功，
－1 表示clean失败:	文件映像 文件校验 文件大小 M3状态	 	
－2  表示 devmem_write 0xAAAA6666 失败		
*/
int upgrade_init_status(void)
{
	int iRet =-1 ;
	int i;
	unsigned long  dwAdd[5]={0x45410000,0x4540fff4,0x4540fff8,0x4540fffc,0x4540fff0};

	for(i=0;i<4;i++)
	{
		iRet = write_data_to_ddr(dwAdd[i],0);
		if(iRet==-1)
		{	
			printf("[%s][%d]  devmem_write error\n",__func__,__LINE__);
			return  MCU_UPDATE_INIT_CLEAR_ERR;
		}
	}

	iRet = write_data_to_ddr(dwAdd[4],0xaaaa6666);
	if(iRet==-1)
	{	
		printf("[%s][%d]  devmem_write error\n",__func__,__LINE__);
		return  MCU_UPDATE_INIT_WRITE_ERR;
	}

	Read_Status();

	printf("\n[upgrade][%d] 	 upgrade_init_status finish  	\n",__LINE__); 
	printf("[%s][%d] ****************VERSION=%s******************* \n",__func__,__LINE__,VERSION);	
	return  MCU_UPDATE_INIT_OK ;
}

int  print_ddr1(unsigned long dwLen)
{
	unsigned long  l;
	printf("\n===================================================================== \n");
	for(l=0;l<dwLen;l++)
	{
		printf(" %x ",g_pBuffer[l]); 
	}
	printf("\n===================================================================== \n");
	return  0;
}

int  print_ddr2(char * pWBuffer,unsigned long dwLen)
{
	unsigned long  l;
	printf("\n===================================================================== \n");
	for(l=0;l<dwLen;l++)
	{
		printf(" %x ",pWBuffer[l]); 
	}
	printf("\n===================================================================== \n");
	return  0;
}

int write_mcu_to_mem(void)
{
	int iRet =-1;
	unsigned long  ii;

	char * pWBuffer =NULL; 

	unsigned long dwChecknum_Wmem;
	unsigned long dwChecknum_Rmem;

	pWBuffer = (char*) malloc(sizeof(char)*g_dwFile_Size);
	if(NULL == pWBuffer)
	{
		printf("[%s][%d]:  malloc err!  \n",__func__,__LINE__);
		return MCU_UPDATE_WRITE_MALLOC_ERR;
	}

	g_iFlag =1;
	printf("[%s][%d] ***VERSION=%s*******devmem write start*****g_iFlag=%d****** \n",__func__,__LINE__,VERSION,g_iFlag);

	for(ii=0;ii<g_dwFile_Size;ii++)
	{
		if(ii==0)//0
		{
			printf("[%s][%d] 0k:  MEM_START_ADDR+i=0x%x  , *(g_pBuffer+ii)=0x%x ,  ii=0x%x  \n",__func__,__LINE__,MEM_START_ADDR+ii,*(g_pBuffer+ii),ii);
		}
		else if(ii==0x400)//1k
		{
			printf("[%s][%d] 1k:  MEM_START_ADDR+i=0x%x  , *(g_pBuffer+ii)=0x%x ,  ii=0x%x  \n",__func__,__LINE__,MEM_START_ADDR+ii,*(g_pBuffer+ii),ii);
		}
		else if(ii==2048)//2k
		{
			printf("[%s][%d] 2k:  MEM_START_ADDR+i=0x%x  , *(g_pBuffer+ii)=0x%x ,  ii=0x%x  \n",__func__,__LINE__,MEM_START_ADDR+ii,*(g_pBuffer+ii),ii);
		}
		else if(ii==3072)//3k
		{
			printf("[%s][%d] 3k:  MEM_START_ADDR+i=0x%x  , *(g_pBuffer+ii)=0x%x ,  ii=0x%x  \n",__func__,__LINE__,MEM_START_ADDR+ii,*(g_pBuffer+ii),ii);
		}
		else if(ii==4092)//4092
		{
			printf("[%s][%d] 4092:  MEM_START_ADDR+i=0x%x  , *(g_pBuffer+ii)=0x%x ,  ii=0x%x  \n",__func__,__LINE__,MEM_START_ADDR+ii,*(g_pBuffer+ii),ii);
		}
		else if(ii==0xffd)//4093
		{
			printf("[%s][%d] 4093:  MEM_START_ADDR+i=0x%x  , *(g_pBuffer+ii)=0x%x ,  ii=0x%x  \n",__func__,__LINE__,MEM_START_ADDR+ii,*(g_pBuffer+ii),ii);
		}
		else if(ii==0x1000)//4k  4096
		{
			printf("[%s][%d] 4k: MEM_START_ADDR+i=0x%x  , *(g_pBuffer+ii)=0x%x ,  ii=0x%x  \n",__func__,__LINE__,MEM_START_ADDR+ii,*(g_pBuffer+ii),ii);
		}
		else if(ii==0x1001)//4097
		{
			printf("[%s][%d] 4097: MEM_START_ADDR+i=0x%x  , *(g_pBuffer+ii)=0x%x ,  ii=0x%x  \n",__func__,__LINE__,MEM_START_ADDR+ii,*(g_pBuffer+ii),ii);
		}
		else if(ii==0x40000)//100k
		{
			printf("[%s][%d]  100k: MEM_START_ADDR+i=0x%x  , *(g_pBuffer+ii)=0x%x ,  ii=0x%x  \n",__func__,__LINE__,MEM_START_ADDR+ii,*(g_pBuffer+ii),ii);
		}
		else if(ii==0x80000)//512k
		{
			printf("[%s][%d]  512K:  MEM_START_ADDR+i=0x%x  , *(g_pBuffer+ii)=0x%x ,  ii=0x%x  \n",__func__,__LINE__,MEM_START_ADDR+ii,*(g_pBuffer+ii),ii);
		}
		else if(ii==0x100000)//1M
		{
			printf("[%s][%d] 1M:  MEM_START_ADDR+i=0x%x  , *(g_pBuffer+ii)=0x%x ,  ii=0x%x  \n",__func__,__LINE__,MEM_START_ADDR+ii,*(g_pBuffer+ii),ii);
		}
		else if(ii==0x140000)//1.1M
		{
			printf("[%s][%d] 1.1M: MEM_START_ADDR+i=0x%x  , *(g_pBuffer+ii)=0x%x ,  ii=0x%x  \n",__func__,__LINE__,MEM_START_ADDR+ii,*(g_pBuffer+ii),ii);
		}
		else if(ii==0x180000)//1.5M
		{
			printf("[%s][%d] 1.5M: MEM_START_ADDR+i=0x%x  , *(g_pBuffer+ii)=0x%x ,  ii=0x%x  \n",__func__,__LINE__,MEM_START_ADDR+ii,*(g_pBuffer+ii),ii);
		}
		else if(ii==0x1b0000)//1.8M
		{
			printf("[%s][%d] 1.8M: MEM_START_ADDR+i=0x%x  , *(g_pBuffer+ii)=0x%x ,  ii=0x%x  \n",__func__,__LINE__,MEM_START_ADDR+ii,*(g_pBuffer+ii),ii);
		}
		else if(ii==0x1e0000)//1.9M
		{
			printf("[%s][%d] 1.9M: MEM_START_ADDR+i=0x%x  , *(g_pBuffer+ii)=0x%x ,  ii=0x%x  \n",__func__,__LINE__,MEM_START_ADDR+ii,*(g_pBuffer+ii),ii);
		}

		pWBuffer[ii] = devmem_write_Log(MEM_START_ADDR+ii,*(g_pBuffer+ii));
		if(pWBuffer[ii]==-1)
		{	
			printf("[%s][%d]  devmem_write_Log error\n",__func__,__LINE__);
			return  MCU_UPDATE_WRITE_WRITE_ERR;
		}

	}

	g_iFlag =0;
	printf("[%s][%d] ***VERSION=%s*******devmem write end!!*****g_iFlag=%d****** \n",__func__,__LINE__,VERSION,g_iFlag);

	dwChecknum_Wmem = CRC16_Check(pWBuffer,g_dwFile_Size);
	printf("\n[%s][%d]  CRC16 Checknum  pWBuffer : dwChecknum_Wmem=0x%x  g_dwChecknum=0x%x \n\n",__func__,__LINE__,dwChecknum_Wmem,g_dwChecknum); 

	print_ddr2(pWBuffer,MEM_PRINT_LEN);

	free(pWBuffer);
	free(g_pBuffer);

	if( dwChecknum_Wmem == g_dwChecknum )
	{
		printf("[%s][%d]    write_mcu_to_mem ok   \n",__func__,__LINE__);
	}
	else
	{
		printf("[%s][%d]    pWBuffer checknum err   \n",__func__,__LINE__);
		return  MCU_UPDATE_WRITE_DDR_CHECK_ERR;
	}

	printf("[%s][%d] ****************VERSION=%s******************* \n",__func__,__LINE__,VERSION);

	return MCU_UPDATE_WRITE_MEM_OK;
}

int check_mcu_file(const char *pPath)
{
	int iRet =-1;
	unsigned long  l;
	unsigned long  dwFreadSize;

	g_pFile = fopen(pPath, "rb" );
	if (g_pFile==NULL)
	{
		printf("[%s][%d]:  fopen err!  \n",__func__,__LINE__);
		return MCU_UPDATE_ERR;
	}

	iRet = fseek(g_pFile,0,SEEK_END);
	if(iRet<0)
	{
		printf("[%s][%d]:  fseek err!  \n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}

	g_dwFile_Size =ftell(g_pFile);
	rewind(g_pFile);

	printf("[%s][%d] : g_dwFile_Size=%d  g_dwFile_Size =0x%x  \n",__func__,__LINE__,g_dwFile_Size,g_dwFile_Size); 

	g_pBuffer = (char*) malloc(sizeof(char)*g_dwFile_Size);
	if(NULL == g_pBuffer)
	{
		printf("[%s][%d]:  malloc err!  \n",__func__,__LINE__);
		return MCU_UPDATE_ERR;
	}

	dwFreadSize = fread(g_pBuffer,1,g_dwFile_Size,g_pFile);
	if (dwFreadSize != g_dwFile_Size)
	{
		printf("[%s][%d]:  fread err!  dwFreadSize=0x%x\n",__func__,__LINE__,dwFreadSize );
		free(g_pBuffer);
		return MCU_UPDATE_ERR;
	}

	g_dwChecknum = CRC16_Check(g_pBuffer,g_dwFile_Size);

	printf("\n[%s][%d]  CRC16 Checknum :   g_dwChecknum=%d   g_dwChecknum=0x%x  \n\n",__func__,__LINE__,g_dwChecknum,g_dwChecknum); 
	
	fclose(g_pFile);

	print_ddr1(MEM_PRINT_LEN);

	printf("[%s][%d]  check_mcu_file finish  \n",__func__,__LINE__);
	printf("[%s][%d] ****************VERSION=%s******************* \n",__func__,__LINE__,VERSION);	
	return MCU_UPDATE_OK;
}

int  clear_a7_status(unsigned long dwA7addr)
{
	int iRet =-1;
	unsigned long  dwValue[1] ={0};

	printf("[%s][%d] :  M3_STATUS_ADDR=0x%x \n",__func__,__LINE__,A7_STATUS_ADDR);

	iRet = devmem_write(dwA7addr,0);
	if(iRet==-1)
	{	
		printf("[%s][%d]  devmem_write error\n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}
	return MCU_UPDATE_OK;
}

int upgrade_init_ready_upgrade(unsigned long dwDDRaddr)
{
	int iRet =-1;
	int i    =0;
	unsigned long  dwValue[3];
	unsigned long  dwAddr[3] ={0x4540fff4,0x4540fffc,0x4540fff0};

	unsigned long  dwCheckNum = get_CRC16_Checknum();

	printf("[%s][%d] : 	dwCheckNum= 0x%x    dwDDRaddr= 0x%x  \n",__func__,__LINE__,dwCheckNum,dwDDRaddr); 

	dwValue[0]  = dwCheckNum;
	dwValue[1]  = get_file_size();
	dwValue[2]  = dwDDRaddr;

	printf("[%s][%d] : 	dwValue1 =0x%x  dwValue2 =0x%x  dwValue3= 0x%x    \n",__func__,__LINE__,dwValue[0],dwValue[1],dwValue[2]); 

	for(i=0;i<2;i++)
	{
		iRet = write_data_to_ddr(dwAddr[i],dwValue[i]);
		if(iRet==-1)
		{	
			printf("[%s][%d]  devmem_write error\n",__func__,__LINE__);
			return  MCU_UPDATE_READY_WRITE_ERR;
		}
	}

	iRet = write_data_to_ddr(dwAddr[2],dwValue[2]);
	if(iRet==-1)
	{	
		printf("[%s][%d]  devmem_write  error\n",__func__,__LINE__);
		return  MCU_UPDATE_READY_WRITE_A7_ERR;
	}

	printf("[%s][%d]  : ready upgrade finish    \n",__func__,__LINE__);
	printf("[%s][%d] ****************VERSION=%s******************* \n",__func__,__LINE__,VERSION);				
	return MCU_UPDATE_READY_OK ;
}
	
int  get_mcu_status(void)
{
	int iStatus =0;
	int iRet =-1;
	int dwRead_Value =-1;

	if(	g_iFlag ==1)return 0xAABBC000;

	iStatus = devmem_read(M3_STATUS_ADDR);
	if(iStatus==-1)
	{	
		printf("[%s][%d]  devmem_read error\n",__func__,__LINE__);
		return  MCU_UPDATE_ERR;
	}

	printf("[%s][%d] :  M3_STATUS_ADDR=0x%x : iStatus= 0x%x\n",__func__,__LINE__,M3_STATUS_ADDR, iStatus); 

	return iStatus;
}

int Close_file(void)
{
	close(g_iFd);
	return MCU_UPDATE_OK;
}

/********************************************************************
  函数名称:  Create
  功能说明:  创建HAL层设备 
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：创建成功; 小于0：创建失败 
********************************************************************/	
int createUpgradeMcuHal(T_HalUpgradeMcuDevice** device)
{
    T_HalUpgradeMcuDevice *dev;
	dev = (T_HalUpgradeMcuDevice *)malloc(sizeof(T_HalUpgradeMcuDevice));	
    if (dev == NULL)
	{
		return -1;
	}

	strcpy(dev->common.version, VERSION);

	strcpy(dev->common.name, "Upgrade  HAL For MCU");

	dev->mem_open 				= mem_open;
	dev->upgrade_init_status	= upgrade_init_status;
	dev->write_mcu_to_mem		= write_mcu_to_mem;
	dev->check_mcu_file			= check_mcu_file;
	dev->Close_file				= Close_file;
	dev->clear_a7_status		= clear_a7_status;
	dev->upgrade_init_ready_upgrade = upgrade_init_ready_upgrade;
	dev->get_mcu_status				= get_mcu_status;
	dev->devmem_read				= devmem_read;
	dev->devmem_write				= devmem_write;


	*device = dev;

	return 0;
}

/********************************************************************
  函数名称:  DestoryUpgradeMcuHal
  功能说明:  释放HAL层设备
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:     
********************************************************************/ 
int destoryUpgradeMcuHal(T_HalUpgradeMcuDevice** device)
{
	if (*device)
	{		
		free(*device);
		*device = NULL;
	}
	
}


/************************ (C) COPYRIGHT Foryou *****END OF FILE****/



