#include <stdio.h>
#include "hal_aux_in_status_check.h"
#include "hal.h"

#include <time.h>
#include <stdlib.h>

/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：aux_in_test.c
* 文件标识：
* 摘 要：   HAL层实现文件
*
* 当前版本：1.0
* 作 者：   jczhang@foryouge.com.cn
* 完成日期：2017年11月16日
*/

/********************************************************************
  函数名称:  ChangedCallBack 
  功能说明:  注册回调函数的实现函数
  输入参数:  AUX_STATUS
  输出参数:  void
  返回值:    void
********************************************************************/
void ChangedCallBack(AUX_STATUS  * paux_in_status_check)
{
	int status;
	status = paux_in_status_check->is_aux_used;
	
	printf("<1:insert> <0:uninsert> --current : %d----\n",paux_in_status_check->is_aux_used);
	
	
	switch(status)
	{
		case 1:
				/*
				*	当前aux设备插入了，可以发送数据给外置mcu，切换音频通道到:aux_in
				*	应用开发人员自己处理业务逻辑,通过devicecomm来发数据给外置mcu：
				*	example:ENUM_HAL_RADIO = 0,  
				*			ENUM_HAL_REAR_AUX,
				*			ENUM_HAL_FRONT_AUX ,
				*			ENUM_HAL_ISDB 	
				*		send(ENUM_HAL_FRONT_AUX);
				*/
				
				break;
				
		case 0:
				/*
				*	当前aux设备没有插入，可以发送数据给外置mcu，切换音频通道到:原来的状态。
				*	应用开发人员自己处理业务逻辑，通过devicecomm来发数据给外置mcu：
				*	example:ENUM_HAL_RADIO = 0,  
				*			ENUM_HAL_REAR_AUX,
				*			ENUM_HAL_FRONT_AUX ,
				*			ENUM_HAL_ISDB 	
				*		send(ENUM_HAL_RADIO);
				*/
				break;
				
				
		default:
				break;
	}
	
}

/********************************************************************
  函数名称:  aux_in_func
  功能说明:  hal层使用模板，不要自己随意使用
  输入参数:  arg
  输出参数:  arg
  返回值:    void
********************************************************************/

void * aux_in_func(void *arg)
{
	T_HalAuxInStatusCheckDevice  *pAIDevice = NULL;
	createAuxInStatusCheckHal(&pAIDevice);
	if (pAIDevice == NULL)
	{
		perror("createAuxInStatusCheckHal failed");
		return NULL;
	}
	
	//注册回调函数
	pAIDevice->registerAuxinStateChangedCallBack(ChangedCallBack);
	
	//初始换并且实现异步等待回调函数被调用
	pAIDevice->aux_in_status_Init();
	
	destoryAuxInStatusCheckHal(&pAIDevice);
	
	return NULL;
}





void  main()
{
	/*
	*	处理  aux_in 的异步通知函数
	*
	*/
	pthread_t tid;
	pthread_create(&tid, NULL, aux_in_func, NULL);
	
	while(1);
}
