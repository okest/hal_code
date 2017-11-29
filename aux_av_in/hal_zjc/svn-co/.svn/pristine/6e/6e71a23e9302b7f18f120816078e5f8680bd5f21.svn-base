/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：video_switcher_atlas7.c
* 文件标识：
* 摘 要：   HAL层视频源切换器实现文件
*
* 当前版本：1.0
* 作 者：   tpeng@foryouge.com.cn
			czhan$foryouge.com.cn
* 完成日期：2017年4月18日
*/
#include <stdio.h>  
#include <fcntl.h>  
#include <stdlib.h>  
#include <string.h>  
#include <sys/types.h>  
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "hal_video_switcher.h"


#define CAMERA_POWER_GPIO_TYPE       'Z' 
#define CAMERA_POWER_GPIO_OFF        _IOW(CAMERA_POWER_GPIO_TYPE, 0, int)
#define CAMERA_POWER_GPIO_ON         _IOW(CAMERA_POWER_GPIO_TYPE, 1, int)
#define CAMERA_POWER_GPIO_STATUS     _IOW(CAMERA_POWER_GPIO_TYPE, 2, int)

#define HAL_VIDEO_SWITCHER_DEBUG_EN 0

#define DEVICE_NAME_FMS6502 "/dev/fms6502"
#define DEVICE_NAME_REARVIEW "/dev/rearview"

static int g_iFms6502_fd = -1;
static int g_iCvbs_fd = -1;

#define 	FMS6502_ADD			0X06
#define 	OUTPUT12_REG0		0x00		//bit0~bit3:1 bit4~bit7:2
#define 	OUTPUT34_REG1		0x01		//bit0~bit3:3 bit4~bit7:4
#define 	OUTPUT56_REG2		0x02		//bit0~bit3:5 bit4~bit7:6
#define 	INPUT_REG3			0x03		//bit0~bit7
#define 	GAIN_REG4			0x04		//bit0~bit5

#define 	OUTPUT_ONE_TWO	    0x00
#define 	OUTPUT_THR_FOR		0x01
#define 	OUTPUT_FIV_SIX		0x02

#define 	OUT_PUT1_ADDR 		0x01   
#define 	OUT_PUT2_ADDR 		0x02
#define 	OUT_PUT3_ADDR 		0x03
#define 	OUT_PUT4_ADDR 		0x04
#define 	OUT_PUT5_ADDR 		0x05
#define 	OUT_PUT6_ADDR 		0x06

typedef  struct Fms6502_Param
{
	unsigned char bFlag; 			// I2C_CHANNEL/GPIO_CHANNEL 
	unsigned char bDirect;			// Reserved 	
	unsigned char u8SlaveId;		// Device slave ID: 0xXX	
	unsigned char u8GpioId; 		// GPIO ID: 0 - 178
	unsigned char u8Addr;			// i2c reg addr: 0xXX
	unsigned char *u8Data;			// gpio or i2c data: 0xXX	
	unsigned int  u32Len;
}Fms6502_Param_t;

typedef  struct cvbs_Param
{
	unsigned char bFlag; 		
	unsigned char *u8Data;		
	unsigned int  u32Len;
}cvbs_Param_t;

unsigned char FMS6502_REG[3];
unsigned char CH_FMS6502[8];

static int open_cvbs(void)
{
	g_iCvbs_fd = open(DEVICE_NAME_REARVIEW, O_RDWR);		
    
	if (g_iCvbs_fd < 0)
	{
        #if HAL_VIDEO_SWITCHER_DEBUG_EN == 1
        printf("open cvbs error\n");
        #endif
		return -1;
	}
    
	return 0;
}

static int close_cvbs(void)
{
  	close(g_iCvbs_fd);

	return 0;
}

static int open_fms6502(void)
{
	g_iFms6502_fd = open(DEVICE_NAME_FMS6502, O_RDWR);		
    
	if (g_iFms6502_fd < 0)
	{
        #if HAL_VIDEO_SWITCHER_DEBUG_EN == 1
		printf("open fms6502 error\n");
        #endif
		return -1;
	}
    
	return 0;
}

static int close_fms6502(void)
{
  	close(g_iFms6502_fd);

	return 0;
}

static char fms6502_i2c_Write(unsigned char reg,unsigned char data,unsigned int u32WriteCnt)
{
	char cRet 	= -1;
	Fms6502_Param_t Fms6502_WriteParam;
	
	Fms6502_WriteParam.bFlag 		= 1; 
	Fms6502_WriteParam.bDirect 		= 0; 		
	Fms6502_WriteParam.u8SlaveId  	= FMS6502_ADD;	
	Fms6502_WriteParam.u8Addr 		= reg;	
	Fms6502_WriteParam.u32Len 		= u32WriteCnt;	

	Fms6502_WriteParam.u8Data = (unsigned char *)malloc(sizeof(unsigned int) * (Fms6502_WriteParam.u32Len + 1));
	if(Fms6502_WriteParam.u8Data == NULL)	
	{	
        #if HAL_VIDEO_SWITCHER_DEBUG_EN == 1
		printf("malloc() in fms6502_i2c_Write() failed\n");
        #endif
		return -1;
	}

    #if HAL_VIDEO_SWITCHER_DEBUG_EN == 1
    printf("[u8SlaveId =0x%x  u8Addr =0x%x  data = 0x%x  u32Len =0x%x] \n",Fms6502_WriteParam.u8SlaveId ,Fms6502_WriteParam.u8Addr ,data, Fms6502_WriteParam.u32Len );
    #endif

    memcpy(Fms6502_WriteParam.u8Data,&data,u32WriteCnt);
	
	cRet= write( g_iFms6502_fd, &Fms6502_WriteParam, sizeof(Fms6502_WriteParam) );	
	if(cRet<0)
	{
        #if HAL_VIDEO_SWITCHER_DEBUG_EN == 1
		printf("\nwrite err \n");
        #endif
		return -1;
	}

	if(Fms6502_WriteParam.u8Data != NULL)	
	{	
        #if HAL_VIDEO_SWITCHER_DEBUG_EN == 1
		printf("\nfree  \n");
        #endif
		free(Fms6502_WriteParam.u8Data);	
	}

    return 0;
}

static int select_video_source(unsigned char input_number,unsigned char output_addr)
{
	unsigned char data,out_addr_sel;

	//初始化SGM6502 关闭所有输入通道
	fms6502_i2c_Write(OUTPUT12_REG0, 0x00, 1);//0x00
	fms6502_i2c_Write(OUTPUT34_REG1, 0x00, 1);//0x01
	fms6502_i2c_Write(OUTPUT56_REG2, 0x00, 1);//0x02

    #if HAL_VIDEO_SWITCHER_DEBUG_EN == 1
	printf("  OUT=%d IN=%d  \n",output_addr,input_number);
    #endif

	CH_FMS6502[output_addr]=input_number;

	switch(output_addr)
	{
		case OUT_PUT1_ADDR:
			out_addr_sel = OUTPUT_ONE_TWO;
			data = (unsigned char)(input_number&0x0F);	 //write input number to low four bits in one resigter
			FMS6502_REG[out_addr_sel]&=0xF0;
			break;

		case OUT_PUT2_ADDR:
			out_addr_sel = OUTPUT_ONE_TWO;
			data = (unsigned char)((input_number<<4)&0xF0);//write input number to high four bits in one resigter
			FMS6502_REG[out_addr_sel]&=0x0F;
			break;

		case OUT_PUT3_ADDR:
			out_addr_sel = OUTPUT_THR_FOR;
			data = (unsigned char)(input_number&0x0F);	 //write input number to low four bits in two resigter
			FMS6502_REG[out_addr_sel]&=0xF0;
			break;

		case OUT_PUT4_ADDR:
			out_addr_sel = OUTPUT_THR_FOR;
			data = (unsigned char)((input_number<<4)&0xF0);//write input number to high four bits in two resigter
			FMS6502_REG[out_addr_sel]&=0x0F;
			break;

		case OUT_PUT5_ADDR:
			out_addr_sel = OUTPUT_FIV_SIX;
			data = (unsigned char)(input_number&0x0F);	  //write input number to low four bits in three  resigter
			FMS6502_REG[out_addr_sel]&=0xF0;
			break;

		case OUT_PUT6_ADDR:
			out_addr_sel = OUTPUT_FIV_SIX;
			data = (unsigned char)((input_number<<4)&0xF0);//write input number to high four bits in three resigter
			FMS6502_REG[out_addr_sel]&=0x0F;
			break;

		default:
			return -1;
		
	}

	FMS6502_REG[out_addr_sel]|= data;

	fms6502_i2c_Write(out_addr_sel, FMS6502_REG[out_addr_sel], 1);	

	fms6502_i2c_Write(INPUT_REG3, 0xFF, 1);	//set input is clamp  0x03

	/*0=6db,1=0db */
	fms6502_i2c_Write(GAIN_REG4, 0x3f, 1);	//set input is gain   0x04
			
	return 0;
}

static char cvbs_write(unsigned char *data,unsigned int u32WriteLen)
{
	char cRet 	= -1;
	cvbs_Param_t cvbs_WriteParam;
	
	cvbs_WriteParam.bFlag 		= 1; 
	cvbs_WriteParam.u32Len 		= u32WriteLen;	

	cvbs_WriteParam.u8Data = (unsigned char *)malloc(sizeof(unsigned int) * (cvbs_WriteParam.u32Len + 1));
	if(cvbs_WriteParam.u8Data == NULL)	
	{
        #if HAL_VIDEO_SWITCHER_DEBUG_EN == 1
		printf("malloc() in cvbs_write() failed\n");
        #endif
		return -1;
	}

	memcpy(cvbs_WriteParam.u8Data,data,u32WriteLen);

	cRet= write(g_iCvbs_fd, &cvbs_WriteParam, sizeof(cvbs_WriteParam));	
	if(cRet<0)
	{
        #if HAL_VIDEO_SWITCHER_DEBUG_EN == 1
		printf("write() in cvbs_write() error\n");
        #endif
		return -1;
	}

	if(cvbs_WriteParam.u8Data != NULL)	
	{
        #if HAL_VIDEO_SWITCHER_DEBUG_EN == 1
		printf("\nfree...  \n");
        #endif
		free(cvbs_WriteParam.u8Data);	
	}

    return cRet;
}

int open_video_channel(void)
{
	int ret;
	
	ret = open_cvbs();
	if(ret != 0)
	{
		printf("[%s:%d]open_cvbs() in open_video_channel() failed\n", __FILE__, __LINE__);
		return -1;
	}

	return 0;
}

int close_video_channel(void)
{
	int ret = 0;
	
	ret = close_cvbs();
	if(ret != 0)
	{
		printf("[%s:%d]close_cvbs() in close_video_channel() failed\n", __FILE__, __LINE__);
		return -1;
	}
	
	return 0;
}

int select_VideoSource(unsigned char video_src_name, unsigned char state)
{
    int ret = 0;
    unsigned char u8Data[2] = {0};
	
	select_video_source(2,1);
	
	switch (video_src_name)
	{
		case VIDEO_SRC_REARVIEW:
			if(state == VIDEO_CHANNEL_ON)
			{
				u8Data[0] = 0;
				u8Data[1] = 1;
				ret = cvbs_write(u8Data, 2);
                printf("rearview on!\n");
			}
			else if(state == VIDEO_CHANNEL_OFF)
			{
				u8Data[0] = 0;
				u8Data[1] = 0;
				ret = cvbs_write(u8Data, 2);
                printf("rearview off!\n");
			}
			break;
		
		case VIDEO_SRC_DVR:
			if(state == VIDEO_CHANNEL_ON)
			{
				u8Data[0] = 1;
				u8Data[1] = 1;
				ret = cvbs_write(u8Data, 2);
                printf("DVR on!\n");
			}
			else if(state == VIDEO_CHANNEL_OFF)
			{
				u8Data[0] = 1;
				u8Data[1] = 0;
				ret = cvbs_write(u8Data, 2);
                printf("DVR off!\n");
			}
			break;
            
		default:
			printf("video_src_name error!\n");
            ret = -1;
			break;
	}

    return ret;
}

int read_RvcGpioData(void)
{
	int ret =-1;
	char buffer[1]={1};
	int iGpiodata = 1;

	ret = read( g_iCvbs_fd,buffer,1);
	if(ret<0)
	{
		printf("[%s][%d]:  read err!  \n",__func__,__LINE__);
		return  -1;
	}
	fsync(g_iCvbs_fd);

	iGpiodata = buffer[0];

	//printf("\n[%s][%d]:  iGpiodata =%d  \n\n",__func__,__LINE__,iGpiodata); 

	return iGpiodata;
}

int ioctl_rvc_power_status(int level)
{
	int ret =-1;
	int iGpiodata = 0;

	//printf("\n[%s][%d]:  level =%d  \n\n",__func__,__LINE__,level); 

	switch (level) 
	{
		case 0:	
			ret = ioctl( g_iCvbs_fd,CAMERA_POWER_GPIO_OFF,&iGpiodata );
			if(ret<0)
			{
				printf("[%s][%d]:  ioctl err!  \n",__func__,__LINE__);
				return  -1;
			}
			break;
		
		case 1:
			ret = ioctl( g_iCvbs_fd,CAMERA_POWER_GPIO_ON,&iGpiodata );
			if(ret<0)
			{
				printf("[%s][%d]:  ioctl err!  \n",__func__,__LINE__);
				return  -1;
			}
			break;
			
		case 2:
			ret = ioctl( g_iCvbs_fd,CAMERA_POWER_GPIO_STATUS,&iGpiodata );
			if(ret<0)
			{
				printf("[%s][%d]:  ioctl err!  \n",__func__,__LINE__);
				return  -1;
			}
			break;

		default:
			return -1;
	}	
	return iGpiodata;
}

int createVideoSwitcherHal(T_HalVideoSwitcherDevice** device)
{
	T_HalVideoSwitcherDevice *dev;
	dev = (T_HalVideoSwitcherDevice *)malloc(sizeof(T_HalVideoSwitcherDevice));	
    if (dev == NULL)
	{
        #if HAL_VIDEO_SWITCHER_DEBUG_EN == 1
        printf("malloc(sizeof(T_HalVideoSwitcherDevice) failed\n");
        #endif
		return -1;
	}
	
	strcpy(dev->common.version, "1.1");
	strcpy(dev->common.name, "Video Switcher HAL For FMS6502");
    dev->common.open          = open_video_channel;
	dev->common.close         = close_video_channel;
	dev->selectVideoSource    = select_VideoSource;
	dev->readRvcGpioData      = read_RvcGpioData;
	dev->ioctlRvcPowerStatus  = ioctl_rvc_power_status;
	
	*device = dev;

	return 0;
}

int destoryVideoSwitcherHal(T_HalVideoSwitcherDevice** device)
{
	if (*device)
	{		
		free(*device);
		*device = NULL;
	}

	return 0;
}
 
 
 
