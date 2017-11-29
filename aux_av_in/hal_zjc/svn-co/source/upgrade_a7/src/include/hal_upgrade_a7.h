/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：hal_upgrade_a7.h
* 文件标识：
* 摘 要：   
*
* 当前版本：1.0
* 作 者：   tpeng@foryouge.com.cn
* 完成日期：2017年6月5日
*/
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __hal_upgrade_a7_H
#define __hal_upgrade_a7_H

#ifdef __cplusplus 
extern "C" 
{ 
#endif
/*----------------------------------------------*
 *                                   			*
 *----------------------------------------------*/
#include "hal.h"

typedef enum
{
     UPDATE_KERNEL_OK 		=0,
     UPDATE_FILE_NAME_ERR	=-1,  
     UPDATE_CHECKE_NAME		=-2,  
     UPDATE_READ_EMMC_FLAG	=-3,  
     UPDATE_COPY_ZIMAGE_V1	=-4,  
     UPDATE_COPY_ZIMAGE_V2	=-5,     
     UPDATE_COPY_DTB_V1		=-6,  
     UPDATE_COPY_DTB_V2		=-7, 
     UPDATE_COPY_MODULES   	=-8,  
     UPDATE_COPY_CSRVISOR  	=-9, 
	 UPDATE_COPY_BOOT_CFG  	=-10, 
     UPDATE_ALL_FILE_ERR   	=-11, 

} t_Update_Kernel_Status;
 

typedef enum
{
     UPDATE_OK   =0,
     UPDATE_ERR  =-1,  
} t_Update_Status;

typedef enum
{
     UPDATE_MOUNT_OK      =0,
     UPDATE_MKDIR_ERR     =-1,
     UPDATE_SYSTEM_ERR    =-2, 
     UPDATE_MOUNT_ERR     =-3,  

} t_Update_Mount_Status;


/*----------------------------------------------*
 * 外部函数说明                                  *
 *----------------------------------------------*/

typedef struct tagUpgradeDevice
{
	T_Device common;        			       /* 通用HAL层操作 */

	int (*emmc_read_flag)(const char *pPath);
	int (*emmc_write_flag)(const char *pPath,char cFlag);
	int (*mount_mmcblk0p2)(void);
	int (*update_kernel)(const char *pFile_Path) ;
	int (*sysre_boot)(void);
	int (*update_dd_kernel)(const char *pFile_Path);
	int (*update_dd_rootfs)(const char *pFile_Path); 
			   
} T_HalUpgradeDevice,*HalUpgradeDevicePtr;

int createUpgradeHal(T_HalUpgradeDevice** device);  /* 创建设备hal控制 */
int destoryUpgradeHal(T_HalUpgradeDevice** device); /* 释放设备hal控制 */



#ifdef __cplusplus 
}
#endif 

#endif /*__hal_upgrade_a7_H */


