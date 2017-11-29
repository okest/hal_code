/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_upgrade_m3.h
* 文件标识：
* 摘 要：   
*
* 当前版本：1.0
* 作 者：   tpeng@foryouge.com.cn
* 完成日期：2017年6月5日
*/
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __hal_upgrade_m3_H
#define __hal_upgrade_m3_H

#ifdef __cplusplus 
extern "C" 
{ 
#endif
#include "hal.h"

/*----------------------------------------------*
 *                                   			*
 *----------------------------------------------*/
typedef enum
{
     MCU_UPDATE_OK   =0,
     MCU_UPDATE_ERR  =-1,  
} t_MCU_Update_Status;

typedef enum
{
     MCU_UPDATE_INIT_OK     =0,
     MCU_UPDATE_INIT_CLEAR_ERR   =-1,
     MCU_UPDATE_INIT_WRITE_ERR   =-2,  
} t_M3_Update_Init_Status;

typedef enum
{
     MCU_UPDATE_WRITE_MEM_OK     		=0,
     MCU_UPDATE_WRITE_MALLOC_ERR   	    =-1,
     MCU_UPDATE_WRITE_WRITE_ERR   	    =-2,
     MCU_UPDATE_WRITE_DDR_CHECK_ERR     =-3,  
} t_M3_Update_Write_Mem;


typedef enum
{
     MCU_UPDATE_READY_OK     =0,
     MCU_UPDATE_READY_WRITE_ERR   =-1,
     MCU_UPDATE_READY_WRITE_A7_ERR   =-2,  
} t_M3_Update_Ready_Status;

/*----------------------------------------------*
 * 外部函数说明                                  *
 *----------------------------------------------*/

typedef struct tagUpgradeMcuDevice
{
	T_Device common;        			       /* 通用HAL层操作 */

	int (*mem_open)(void);

	int (*upgrade_init_status)(void);

	int (*write_mcu_to_mem)(void);

	int (*check_mcu_file)(const char *pPath);

	int (*Close_file)(void);

	int (*clear_a7_status)(unsigned long dwA7addr);

	int (*upgrade_init_ready_upgrade)(unsigned long dwDDRaddr);

	int (*get_mcu_status)(void);

	int (*devmem_read)(unsigned long dwTargetAddr);

	int (*devmem_write)(unsigned long dwTargetAddr,unsigned long dwWritevall);

			   
} T_HalUpgradeMcuDevice,*HalUpgradeMcuDevicePtr;

int createUpgradeMcuHal(T_HalUpgradeMcuDevice** device);  /* 创建设备hal控制 */
int destoryUpgradeMcuHal(T_HalUpgradeMcuDevice** device); /* 释放设备hal控制 */




#ifdef __cplusplus 
}
#endif 

#endif /*__hal_upgrade_m3_H */


