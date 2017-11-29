/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：iap_auth_2.0C.c
* 文件标识：
* 摘 要：   HAL层MFi鉴权芯片操作实现文件
*
* 当前版本：1.0
* 作 者：   ylv@foryouge.com.cn
* 完成日期：2017年6月9日
*/
#include <stdint.h>	
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include "hal_iap_auth.h"


#define LOGE(format, ...)  fprintf(stdout,">>  "format"\n", ##__VA_ARGS__)

#define DEBUG

#ifdef DEBUG
#define LOGD(format, ...)  fprintf(stdout,">>  "format"\n", ##__VA_ARGS__)
#else
#define LOGD(format, ...)
#endif

//公用的宏定义
#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#define MAKEUINT16(a, b)		((unsigned short)(((unsigned char)(a)) | ((unsigned short)((unsigned char)(b))) << 8))
#define LOUINT8(w)					((unsigned char)(w))
#define HIUINT8(w)					((unsigned char)(((unsigned short)(w) >> 8) & 0xFF))
                        

#define  DEVICE_NAME "/dev/IpodI2c" /* 设备名称 */

#define  BUFFERLENGTH							2048
#define  CERTIFICATESERIALNUMBERDATALENGTH		31
#define  CHIPDATALENGTH							2
#define  CERTIFICATEDATALENGTH					128

static const unsigned char  chipAddr			= 0x11;	
	
static int fd = -1; /* 设备描述符 */


/********************************************************************
  函数名称:  readErrorCode
  功能说明:  读取错误码，每次读取数据失败后要调用本函数获取错误码
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0：打开成功; 小于0：打开失败
********************************************************************/

static int readErrorCode(unsigned char* errorCode)
{
	int ret = 0;
	assert(errorCode);
	iic_param_t I2cParam    = {0};
	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterErrorCode;
	I2cParam.dwDataSizeIIC = iAPAuthICControlAndStatusDataLength;
	I2cParam.pucBufIIC	   = errorCode;
	ret = read(fd, &I2cParam, sizeof(iic_param_t));
    LOGE("IapAuthHAL:@@@%s@@@ authIC errorCode =%u \r\n ",__func__,*errorCode);
	if (ret<0)
	{
		LOGE("IapAuthHAL:@@@%s@@@  readErrorCode failed ! ret=%d",__func__,ret);
	}
	return ret;
}




/********************************************************************
  函数名称:  OpenIapAuth
  功能说明:  打开驱动设备
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0：打开成功; 小于0：打开失败
********************************************************************/
static int openIapAuth(void)
{
	fd = open(DEVICE_NAME, O_RDWR);
	if(fd < 0)
	{  
		LOGE("Open IpodI2c fail\n");		
	}
	else
	{
	    LOGD("Open IpodI2c ok\n");	
	}
	return fd;
}

/********************************************************************
  函数名称:  CloseIapAuth 
  功能说明:  关闭驱动设备
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0:关闭成功; 小于0:关闭失败
********************************************************************/
static int closeIapAuth(void)
{
	return close(fd);	
}

/********************************************************************
  函数名称:  readCertificateSN
  功能说明:  获取芯片序列号
  输入参数:  无
  输出参数:  得到的芯片序列号
  返回值:    设备节点read接口的返回值
********************************************************************/
 int readCertificateSN(Uint8Data_t* dataU8)
 {
	int ret= 0;
	int fd = 0;
	assert(dataU8);
	dataU8->length=CERTIFICATESERIALNUMBERDATALENGTH;
	iic_param_t I2cParam    = {0};
  
	fd = open(DEVICE_NAME, O_RDWR);
	if(fd < 0)
	{  
		 LOGE("Open IpodI2c fail\n");		
	}
	else
	{
	    LOGD("Open IpodI2c ok\n");	
	}

	
	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterAccessoryCertificateSerialNumber;
	I2cParam.dwDataSizeIIC =  dataU8->length;
	I2cParam.pucBufIIC	   = dataU8->data;

	ret = read(fd, &I2cParam, sizeof(iic_param_t));

	if (ret<0)
	{
		LOGE("IapAuthHAL:@@@%s@@@ get AccessoryCertificateSerialNumber failed ! ret=%d",__func__,ret);
		readErrorCode((unsigned char*)&ret);
		LOGE("IapAuthHAL:@@@%s@@@ Error Code  = %d",__func__,ret);
		return -1;
	}

	close(fd);
	return ret;
 }     					
 

/********************************************************************

  函数名称:  readCertificateData
  功能说明:  获取鉴权数据
  输入参数:  无
  输出参数:  得到的鉴权证书数据
  返回值:    大于等于0:成功; 小于0:失败
********************************************************************/
int readCertificateData(Uint8Data_t* dataU8)	
{
	assert(dataU8);
	int ret=0;
	int fd = 0;
	unsigned char  lengthBuff[CHIPDATALENGTH] = {0};
	iic_param_t I2cParam    = {0};
	
	fd = open(DEVICE_NAME, O_RDWR);
	if(fd < 0){  
		 LOGE("Open IpodI2c fail\n");		
	}
	else
	{
	    LOGD("Open IpodI2c ok\n");	
	}
	//Read Certificate Data Length
	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterAccessoryCertificateDataLength;
	I2cParam.dwDataSizeIIC = CHIPDATALENGTH;
	I2cParam.pucBufIIC	   = lengthBuff;

	ret = read(fd, &I2cParam, sizeof(iic_param_t));
	if(ret<0)
	{
		LOGE("IapAuthHAL:@@@%s@@@get Accessory Certificate DataLength failed ! ret=%d",__func__,ret);
		close(fd);
		return ret;
	}
  //Read Certificate Data
	dataU8->length=MAKEUINT16(lengthBuff[1],lengthBuff[0]);
	unsigned int dataLen = dataU8->length;
    unsigned int len = 0;

	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterAccessoryCertificateData;

	while(dataLen>0)
	{
		len = MIN(dataLen,CERTIFICATEDATALENGTH);
		I2cParam.dwDataSizeIIC = len;
		I2cParam.pucBufIIC	   = dataU8->data+(dataU8->length - dataLen);
		ret = read(fd, &I2cParam, sizeof(iic_param_t));
		if(ret<0)
		{
			LOGE("IapAuthHAL:@@@%s@@@read AccessoryCertificateData failed ! ret=%d",__func__,ret);
			close(fd);
			return ret;
		}
		dataLen -= len;
		I2cParam.ucAddrIIC[0]++;
	}
	close(fd);
	return 0;
}			
 

/********************************************************************

  函数名称:  readChallengeResponse
  功能说明:  写入随机数并从芯片获取计算结果 
  输入参数:  从apple device获得的随机数
  输出参数:  芯片的计算结果
  返回值:    大于等于0:成功; 小于0:失败
********************************************************************/
int readChallengeResponse(Uint8Data_t* dataIn,Uint8Data_t* dataOut)	
{
	assert(dataIn);
	assert(dataOut);
	int ret = 0;
	int fd = 0;
	int i = 0;
	iic_param_t I2cParam    = {0};
		fd = open(DEVICE_NAME, O_RDWR);
	if(fd < 0){  
		 LOGE("Open IpodI2c fail\n");		
	}
	else
	{
	    LOGD("Open IpodI2c ok\n");	
	}
	//  Write Challenge Lenth
	unsigned char  lengthBuff[CHIPDATALENGTH] = {0};
	lengthBuff[0] = HIUINT8(dataIn->length);
	lengthBuff[1] = LOUINT8(dataIn->length);
	printf(">>>Write Challenge Lenth,lengthbuff[0]=%d,lengthBuff[1]=%d,length=%d",lengthBuff[0],lengthBuff[1],dataIn->length);
    I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterChallengeDataLength;
	I2cParam.dwDataSizeIIC = CHIPDATALENGTH;
    I2cParam.pucBufIIC	   = lengthBuff;
	ret = write(fd, &I2cParam, sizeof(iic_param_t));
	if(ret<0)
	{
		LOGE("IapAuthHAL:@@@%s@@@write Challenge Lenth failed ! ret=%d",__func__,ret);
		goto exit;
	}


	usleep(100000);


  //  Write Challenge Data
	int dataLen = dataIn->length;
	int len = 0;
	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterChallengeData;
	while(dataLen>0)
	{
		len = MIN(dataLen,CERTIFICATEDATALENGTH);
		I2cParam.dwDataSizeIIC = len;
		I2cParam.pucBufIIC	   = dataIn->data+(dataIn->length - dataLen);
		printf(">>>Write Challenge data:\n");
		for(i=0;i<len;i++)
		{  if(0 == (i%8))
        {
            printf("\n");
        }
		printf("%x\t",*((I2cParam.pucBufIIC)+i));
		}
		printf("\n");


		ret = write(fd, &I2cParam, sizeof(iic_param_t));
		if(ret<0)
		{
			LOGE("IapAuthHAL:@@@%s@@@write Accessory Challenge Data failed ! ret=%d",__func__,ret);
			goto exit;
		}
		dataLen -= len;
		I2cParam.ucAddrIIC[0]++;
	}


  //  Write Authentication Control  PROC_CONTROL=1 代表
	unsigned char controlByte[1]= {0};
	controlByte[0] = iAPAuthICProcControlWriteChallengeData;
	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterAuthenticationControlandStatus;
	I2cParam.dwDataSizeIIC = iAPAuthICControlAndStatusDataLength;
	I2cParam.pucBufIIC	   = controlByte;

	printf(">>>Write Authentication Control:PROC_CONTROL=%d \n",controlByte[0]);

	ret = write(fd, &I2cParam, sizeof(iic_param_t));
	if(ret<0)
    {
		LOGE("IapAuthHAL:@@@%s@@@ Write Authentication Control failed ! ret=%d",__func__,ret);
		goto exit;
    }

	usleep(200000);


  //  Read Status   PROC_RESULTS=1 代表
	for (i = 0; i < 30 ; i++)
	{ 
		I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterAuthenticationControlandStatus;
		I2cParam.dwDataSizeIIC = iAPAuthICControlAndStatusDataLength;
		I2cParam.pucBufIIC	   = controlByte;
		ret = read(fd, &I2cParam, sizeof(iic_param_t));
		if (ret<0)
		{
			usleep(2000);
			printf(">>>Read Status failed,i=%x\n",controlByte[0]);
			continue;
		}
		if (iAPAuthICProcResultsReadChallengeResponse == ((controlByte[0] >> 4) & 0x7))
		{
			break;
		}
	}
	printf(">>>Read Status:PROC_RESULTS=%x\n",controlByte[0]);
 
	if (iAPAuthICProcResultsReadChallengeResponse != ((controlByte[0] >> 4) & 0x7))
	{
		LOGE("IapAuthHAL:@@@%s@@@ Read Status failed ! ret=%d",__func__,ret);
		goto exit;
	}
  //  Read Challenge Response Length


	lengthBuff[0] = 0;
	lengthBuff[1] = 0;
	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterChallengeResponseDataLength;
	I2cParam.dwDataSizeIIC = CHIPDATALENGTH;
	I2cParam.pucBufIIC	   = lengthBuff;

	ret = read(fd, &I2cParam, sizeof(iic_param_t));
	if(ret<0)
	{
		LOGE("IapAuthHAL:@@@%s@@@get Challenge Response Data Length failed ! ret=%d",__func__,ret);
		goto exit;
	}
  
  //Read Challenge Response Data
	dataOut->length=MAKEUINT16(lengthBuff[1],lengthBuff[0]);
	printf(">>>Read Challenge Response Length,lengthbuff[0]=%d,lengthBuff[1]=%d,length=%d\n",lengthBuff[0],lengthBuff[1],dataOut->length);
	dataLen = dataOut->length;
	len = 0;

	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterChallengeResponseData;

	while(dataLen>0)
	{
		len = MIN(dataLen,CERTIFICATEDATALENGTH);
		I2cParam.dwDataSizeIIC = len;
		I2cParam.pucBufIIC	   = dataOut->data+(dataOut->length - dataLen);
		ret = read(fd, &I2cParam, sizeof(iic_param_t));
    if(ret<0)
    {
		LOGE("IapAuthHAL:@@@%s@@@get Challenge Response Data failed ! ret=%d",__func__,ret);
		goto exit; 
    }
	dataLen -= len;
	I2cParam.ucAddrIIC[0]++;
	}

exit:
	if (ret<0)
	{
		readErrorCode((unsigned char*)&ret);
		LOGE("IapAuthHAL:@@@%s@@@ Error Code  = %d",__func__,ret);
	}
	close(fd);	
	return ret;

}
/********************************************************************

  函数名称:  writeCertificateData
  功能说明:  将从手机获取的鉴权数据写入芯片并获取随机数 
  输入参数:  从手机获得的鉴权数据
  输出参数:  芯片
  返回值:    大于等于0:成功; 小于0:失败
********************************************************************/
 int writeCertificateData(Uint8Data_t* dataIn,Uint8Data_t* dataOut)
 {
	assert(dataIn);
	assert(dataOut);
	int ret = 0;
	int i;
	iic_param_t I2cParam    = {0};
	uint8_t controlByte [1]= {0};
	unsigned char  lengthBuff[CHIPDATALENGTH] = {0};
 
  // Write Certificate Length
	lengthBuff[0] = HIUINT8(dataIn->length);
	lengthBuff[1] = LOUINT8(dataIn->length);
	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterDeviceCertificateDataLength;
	I2cParam.dwDataSizeIIC = CHIPDATALENGTH;
	I2cParam.pucBufIIC	   = lengthBuff;
	ret = write(fd, &I2cParam, sizeof(iic_param_t));
	if(ret<0)
	{
		LOGE("IapAuthHAL:@@@%s@@@write Device Certificate Data Length failed ! ret=%d",__func__,ret);
		goto exit;
	}
	
	usleep(100000);


  // Write Certificate Data
	int dataLen = dataIn->length;
	int len = 0;
	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterDeviceCertificateData;
	while(dataLen>0)
	{
		len = MIN(dataLen,CERTIFICATEDATALENGTH);
		I2cParam.dwDataSizeIIC = len;
		I2cParam.pucBufIIC	   = dataIn->data+(dataIn->length - dataLen);
		ret = write(fd, &I2cParam, sizeof(iic_param_t));
    if(ret<0)
    {
		LOGE("IapAuthHAL:@@@%s@@@write Accessory Challenge Data failed ! ret=%d",__func__,ret);
		goto exit;
    }
	dataLen -= len;
	I2cParam.ucAddrIIC[0]++;
	}

  // Write Authentication Control   PROC_CONTROL=4
	controlByte[0] = iAPAuthICProcControlWriteChallengeData;
	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterAuthenticationControlandStatus;
	I2cParam.dwDataSizeIIC = iAPAuthICControlAndStatusDataLength;
	I2cParam.pucBufIIC	   = controlByte;
	ret = write(fd, &I2cParam, sizeof(iic_param_t));
	if(ret<0)
    {
		LOGE("IapAuthHAL:@@@%s@@@ Write Authentication Control=4 failed ! ret=%d",__func__,ret);
		goto exit;
    }

	usleep(200000);


  // Read Status                    PROC_RESULTS=4
	for (i = 0; i < 30 ; i++)
	{ 
		I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterAuthenticationControlandStatus;
		I2cParam.dwDataSizeIIC = iAPAuthICControlAndStatusDataLength;
		I2cParam.pucBufIIC	   = controlByte;
		ret = read(fd, &I2cParam, sizeof(iic_param_t));
		if (ret<0)
		{
			usleep(2000);
			continue;
		}
		if ( 4 ==controlByte[0])
		{
			break;
		}
	}
 
 
	if ( 4 == controlByte[0])
	{
		LOGE("IapAuthHAL:@@@%s@@@ Read Status failed ! ret=%d",__func__,ret);
		goto exit;
	}

  // Write Authentication Control   PROC_CONTROL=2
	controlByte[0] = iAPAuthICProcControlReadChallengeData;
	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterAuthenticationControlandStatus;
	I2cParam.dwDataSizeIIC = iAPAuthICControlAndStatusDataLength;
	I2cParam.pucBufIIC	   = controlByte;
	ret = write(fd, &I2cParam, sizeof(iic_param_t));
	if(ret<0)
    {
		LOGE("IapAuthHAL:@@@%s@@@ Write Authentication Control=2 failed ! ret=%d",__func__,ret);
		goto exit;
    }

  
  // Read Status                    PROC_RESULTS=2
	for (i = 0; i < 30 ; i++)
	{ 
		I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterAuthenticationControlandStatus;
		I2cParam.dwDataSizeIIC = iAPAuthICControlAndStatusDataLength;
		I2cParam.pucBufIIC	   = controlByte;
		ret = read(fd, &I2cParam, sizeof(iic_param_t));
		if (ret<0)
		{
			usleep(2000);
			continue;
		}
		if ( 2 ==controlByte[0])
		{
			break;
		}
	}

  if ( 2 == controlByte[0])
	{
		LOGE("IapAuthHAL:@@@%s@@@ Read Status failed ! ret=%d",__func__,ret);
		goto exit;
	}


  // Read Challenge Data Lenth
	lengthBuff[0] = 0;
	lengthBuff[1] = 0;
	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterChallengeDataLength;
	I2cParam.dwDataSizeIIC = CHIPDATALENGTH;
	I2cParam.pucBufIIC	   = lengthBuff;

	ret = read(fd, &I2cParam, sizeof(iic_param_t));
	if(ret<0)
	{
		LOGE("IapAuthHAL:@@@%s@@@get Challenge Data  Length failed ! ret=%d",__func__,ret);
		goto exit;
	}	
	// Read Challenge Data
	dataOut->length=MAKEUINT16(lengthBuff[1],lengthBuff[0]);
	dataLen = dataOut->length;
	len = 0;

	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterChallengeData;

	while(dataLen>0)
	{
		len = MIN(dataLen,CERTIFICATEDATALENGTH);
		I2cParam.dwDataSizeIIC = len;
		I2cParam.pucBufIIC	   = dataOut->data+(dataOut->length - dataLen);
		ret = read(fd, &I2cParam, sizeof(iic_param_t));
		if(ret<0)
		{
		LOGE("IapAuthHAL:@@@%s@@@get ChallengeData failed ! ret=%d",__func__,ret);
		goto exit; 
		}
		dataLen -= len;
		I2cParam.ucAddrIIC[0]++;
	}



 exit:
	if (ret < 0)
	{
		readErrorCode((unsigned char*)&ret);
		LOGE("IapAuthHAL:@@@%s@@@ Error Code  = %d",__func__,ret);
	}
	return ret;
 }		
/********************************************************************

  函数名称:  writeChallengeResponse
  功能说明:  将手机端随机数计算结果写入芯片，得出鉴权结果
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0:成功; 小于0:失败
********************************************************************/
 int writeChallengeResponse(Uint8Data_t* dataIn)
 {
	assert(dataIn);
	int ret = 0;
	int i = 0;
	iic_param_t I2cParam    = {0};
	uint8_t controlByte [1]= {0};
	unsigned char  lengthBuff[CHIPDATALENGTH] = {0};

// Write Challenge length
	lengthBuff[0] = HIUINT8(dataIn->length);
	lengthBuff[1] = LOUINT8(dataIn->length);
	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterChallengeResponseDataLength;
	I2cParam.dwDataSizeIIC = CHIPDATALENGTH;
	I2cParam.pucBufIIC	   = lengthBuff;
	ret = write(fd, &I2cParam, sizeof(iic_param_t));
	if(ret<0)
	{
		LOGE("IapAuthHAL:@@@%s@@@write Device Certificate Data Length failed ! ret=%d",__func__,ret);
		goto exit;
	}
	
	usleep(100000);

// Write Challenge Data
	int dataLen = dataIn->length;
	int len = 0;
	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterChallengeResponseData;
	while(dataLen>0)
	{
		len = MIN(dataLen,CERTIFICATEDATALENGTH);
		I2cParam.dwDataSizeIIC = len;
		I2cParam.pucBufIIC	   = dataIn->data+(dataIn->length - dataLen);
		ret = write(fd, &I2cParam, sizeof(iic_param_t));
		if(ret<0)
		{
		LOGE("IapAuthHAL:@@@%s@@@write Accessory Challenge Data failed ! ret=%d",__func__,ret);
		goto exit;
		}
		dataLen -= len;
		I2cParam.ucAddrIIC[0]++;
	}


// Write Authentication Control
	controlByte[0] = iAPAuthICProcControlWriteAuthentication;
	I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterAuthenticationControlandStatus;
	I2cParam.dwDataSizeIIC = iAPAuthICControlAndStatusDataLength;
	I2cParam.pucBufIIC	   = controlByte;
	ret = write(fd, &I2cParam, sizeof(iic_param_t));
	if(ret<0)
    {
		LOGE("IapAuthHAL:@@@%s@@@ Write Authentication Control=3 failed ! ret=%d",__func__,ret);
		goto exit;
    }

// Read Status
	for (i = 0; i < 30 ; i++)
	{ 
		I2cParam.ucAddrIIC[0]  = iAPAuthICRegisterAuthenticationControlandStatus;
		I2cParam.dwDataSizeIIC = iAPAuthICControlAndStatusDataLength;
		I2cParam.pucBufIIC	   = controlByte;
		ret = read(fd, &I2cParam, sizeof(iic_param_t));
		if (ret<0)
		{
			usleep(2000);
			continue;
		}
		if ( 3 ==controlByte[0])
		{
			break;
		}
	}

  if ( 3 == controlByte[0])
	{
	    LOGE("IapAuthHAL:@@@%s@@@ Read Status failed ! ret=%d",__func__,ret);
        goto exit;
	}
  
  // exit
exit:
	if (ret < 0)
	{
		readErrorCode((unsigned char*)&ret);
		LOGE("IapAuthHAL:@@@%s@@@ Error Code  = %d",__func__,ret);
	}
	return ret;
 }			
	

/********************************************************************
  函数名称:  createIapAuthHal
  功能说明:  创建HAL层设备 
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：创建成功; 小于0：创建失败 
********************************************************************/	
int createIapAuthHal(T_HalIapAuthDevice** device)
{
	T_HalIapAuthDevice *dev;
	dev = (T_HalIapAuthDevice *)malloc(sizeof(T_HalIapAuthDevice));	
    if (dev == NULL)
	{
		return -1;
	}
	strcpy(dev->common.version, "1.0");
	strcpy(dev->common.name, "IapAuth HAL For CSRatlas7");
	dev->common.open = openIapAuth;
	dev->common.close = closeIapAuth;	
	dev->readCertificateSN = readCertificateSN;
	dev->readCertificateData = readCertificateData;
	dev->readChallengeResponse = readChallengeResponse;
	dev->writeCertificateData = writeCertificateData;
	dev->writeChallengeResponse = writeChallengeResponse;
	
	*device = dev;

	return 0;
}


/********************************************************************
  函数名称:  destoryIapAuthrHal 
  功能说明:  释放HAL层设备
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：释放成功; 
********************************************************************/ 
int destoryIapAuthHal(T_HalIapAuthDevice** device)
{
	if (*device)
	{		
		free(*device);
		*device = NULL;
	}
	return 0;
}
 

