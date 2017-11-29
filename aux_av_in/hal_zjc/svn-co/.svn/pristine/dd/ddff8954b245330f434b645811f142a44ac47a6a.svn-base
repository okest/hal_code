/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_iap_auth.h
* 文件标识：
* 摘 要：   HAL层MFi鉴权芯片头文件
*
* 当前版本：1.0
* 作 者：   ylv@foryouge.com.cn
* 完成日期：2017年6月9日
*/

#ifndef _IAP_AUTH_HAL_H
#define _IAP_AUTH_HAL_H

#include "hal.h"

#ifdef __cplusplus 
extern "C" 
{ 
#endif


/*
 *	苹果2.0c鉴权芯片寄存器地址列表
 */
typedef enum
{
	iAPAuthICRegisterDeviceVersion						= 0x00,///> R	0x05
	iAPAuthICRegisterFirmwareVersion					= 0x01,///> R	0x01
	iAPAuthICRegisterDeviceID							= 0x04,///> R	0x00000200
	iAPAuthICRegisterErrorCode							= 0x05,///> R	0x00
	iAPAuthICRegisterAuthenticationControlandStatus		= 0x10,///> R/W 0x00
	iAPAuthICRegisterChallengeResponseDataLength		= 0x11,///> R/W 128
	iAPAuthICRegisterChallengeResponseData				= 0x12,///> R/W Undefined
	iAPAuthICRegisterChallengeDataLength				= 0x20,///> R/W 20
	iAPAuthICRegisterChallengeData						= 0x21,///> R/W Undefined
	iAPAuthICRegisterAccessoryCertificateDataLength		= 0x30,///> R	<=1280
	iAPAuthICRegisterAccessoryCertificateData			= 0x31,///> R	0x31~0x3A Undefined
	iAPAuthICRegisterSelfTestControlandStatus			= 0x40,///> R/W 0x00
	iAPAuthICRegisterSystemEventCounter					= 0x4D,///> R	Undefined
	iAPAuthICRegisterAccessoryCertificateSerialNumber	= 0x4E,///> R	Null-terminated string
	iAPAuthICRegisterDeviceCertificateDataLength		= 0x50,///> R	0x0000
	iAPAuthICRegisterDeviceCertificateData				= 0x51,///> R/W 0x51~0x58 Undefined
}iAPAuthICRegisterAddressMap;

typedef enum                                              //只写控制信息和只读状态信息定义
{
	iAPAuthICProcControlWriteChallengeData				= 1,
	iAPAuthICProcResultsReadChallengeResponse			= 1,
	iAPAuthICProcControlReadChallengeData				= 2,
	iAPAuthICProcResultsReadChallengeData				= 2,
	iAPAuthICProcControlWriteAuthentication				= 3,
	iAPAuthICProcResultsWriteAuthentication				= 3,
	iAPAuthICProcControlWriteCertificateData			= 4,
	iAPAuthICProcResultsReadWriteCertificateData		= 4,
	iAPAuthICControlAndStatusDataLength					= 1,
}iAPAuthICControlAndStatus;

typedef struct iic_param_st                      //和驱动一致，read调用的buf必须初始化为此结构体，内含参数
{
    unsigned char ucIdIIC;      	// IIC ID: Channel 1~7
    unsigned char ucClockIIC;   	// IIC clock speed
    unsigned char ucSlaveIdIIC;    	// Device slave ID
    unsigned char ucAddrSizeIIC;	// Address length in bytes
    unsigned char ucAddrIIC[4];	    // Starting address inside the device
    unsigned char *pucBufIIC;     	// buffer
    unsigned int dwDataSizeIIC;		// size of buffer
}iic_param_t;

typedef struct                                  //鉴权数据的组织形式
{
	unsigned char*										data;
	unsigned short						 				length;
}Uint8Data_t;


typedef struct tagIapAuthDevice
{
	T_Device common;         /* 通用HAL层操作 */
	//手机对配件进行鉴权
	int (*readCertificateSN)(Uint8Data_t* dataU8);  							/* 获取芯片序列号 */
	int (*readCertificateData)(Uint8Data_t* dataU8); 							/* 获取鉴权数据*/
	int (*readChallengeResponse)(Uint8Data_t* dataIn,Uint8Data_t* dataOut);		/* 写入随机数并从芯片获取计算结果 */
	//配件对手机进行鉴权
	int (*writeCertificateData)(Uint8Data_t* dataIn,Uint8Data_t* dataOut);		/* 将从手机获取的鉴权数据写入芯片，获取随机数 */
	int (*writeChallengeResponse)(Uint8Data_t* dataIn);							/* 将手机端随机数计算结果写入芯片，对比得出鉴权结果 */
}T_HalIapAuthDevice,*HalIapAuthDevicePtr;

int createIapAuthHal(T_HalIapAuthDevice** device);  /* 创建设备hal控制 */
int destoryIapAuthHal(T_HalIapAuthDevice** device); /* 释放设备hal控制 */

#ifdef __cplusplus 
}
#endif 

#endif /* __IAP_AUTH_HAL_H */
