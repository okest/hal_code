/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：diagnosis_atlas7.c
* 文件标识：
* 摘 要：   HAL层故障码
*
* 当前版本：2.0
* 作 者：   jczhang@foryouge.com.cn
* 完成日期：2017年5月2日
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mtd/mtd-user.h>
#include "hal_diagnosis.h"
#include "json.h"
#include "hal_dia_enum.h"
#include <time.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define DEVICE_NAME "/dev/mmcblk0p13"
#define SIZE_DIA 8
#define COUNT_MAX 254
#define MAX_NUM_ZONE 1024

static char  addr_current[5];
static char  addr_current_pre[5];
struct json_object *errcode_array;

char repo_buf_head[16];            //头部上报数据
char repo_buf_update[8];           //升级上报数据

//******************************************************************
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
/*创建或打开信号量 */
int create_sem(key_t key, int nsems) 
{
    return semget(key, nsems, IPC_CREAT|0666);
}

/*初始化信号量 */
int init_sem(int semid, int semnum, int val)
{
    union semun sun;
    sun.val = val;
    return semctl(semid, semnum, SETVAL, sun);
}

/* 删除信号量*/
int del_sem(int semid)
{   
    union semun sun;
    return semctl(semid, 0, IPC_RMID, sun);
}

/* P操作 */
int sem_p(int semid,int sem_num, size_t nops)
{
    struct sembuf ops;
    ops.sem_num = sem_num;
    ops.sem_op = -1;
    ops.sem_flg = SEM_UNDO;
    return semop(semid, &ops, nops);
}

/* V操作 */
int sem_v(int semid,int sem_num, size_t nops)
{
    struct sembuf ops;
    ops.sem_num = sem_num;
    ops.sem_op = 1; 
    ops.sem_flg = SEM_UNDO;
    return semop(semid, &ops, nops);
}
//*******************************************************************

/********************************************************************
  函数名称:  clean_all_emmc (int fd,u_int32_t offset,u_int32_t len,char *filename)
  功能说明:  数据存储满了，清空emmc
  输入参数:  有
  输出参数:  无
  返回值:    int
********************************************************************/
char errcode_set[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
int clean_all_emmc (int fd,u_int32_t offset,u_int32_t len,char *filename)
{
	char *buf = NULL;
	int err;
	int size = len * sizeof (u_int8_t);

	if (offset != lseek (fd,offset,SEEK_SET))
	{
		perror ("lseek()");
		return (1);
	}
    //开辟一个空间存储数据
	if ((buf = (char *) malloc (size)) == NULL)
	{
		perror ("malloc()");
		return (1);
	}
	char * buf_twice = buf;
	int i ;
	for(i =0 ;i <8 ;i++)
	{
		*buf = filename[i];
		buf++;
	}
	//buf= buf - 3;
	//printf("--%x\n",*buf);
	//printf("--%x\n",*buf_twice);
	//buf_twice = buf_twice + 7;
	//printf("--%x\n",*buf_twice);

	err = write (fd,buf_twice,size);
	if (err < 0)
	{
		perror ("write()");	
		return (1);
	}

	if (buf_twice != NULL)
		free (buf_twice);

	//printf ("Copied %d bytes to address 0x%.8x in flash\n",len,offset);
	return (0);
}

int __clean_all_emmc()
{
	int err = 0,fd;
	/* open device */
	if ((fd = open ("/dev/mmcblk0p13",O_SYNC | O_RDWR)) < 0)
	{
		perror ("open()");
		exit (1);
	}
	int i =1;
	char addr[5];
	//int count= 1 ;
	printf("start to clean all dia zone......\n");
	for(i = 1; i < 1025 ;i++)
	{
		sprintf(addr,"%d", (20480 + (i -1)*8));
	//	printf("count = %d  clear addr is %s\n",count,addr);
		err = clean_all_emmc(fd,strtol (addr,NULL,0),8,errcode_set);
	//	count++;
	}
	
	/* close device */
	if (close (fd) < 0)
	perror ("close()");
	
	//清空uboot区域
	if ((fd = open ("/dev/mmcblk0p13",O_SYNC | O_RDWR)) < 0)
	{
		perror ("open()");
		exit (1);
	}
	//addr[5];
	//count= 1 ;
	for(i =1; i < 65 ;i++)
	{
		sprintf(addr,"%d", (512 + (i -1)*8));
	//	printf("count = %d  clear addr is %s\n",count,addr);
		err = clean_all_emmc(fd,strtol (addr,NULL,0),8,errcode_set);
		//count++;
	}
	
	/* close device */
	if (close (fd) < 0)
	perror ("close()");

	printf("finish  to clean all dia zone......\n");
	return err;
}
//*******************************************************************



/********************************************************************
  函数名称:  int pack(char buf[50])
  功能说明:  上报数据的打包
  输入参数:  有
  输出参数:  无
  返回值:    int
********************************************************************/
int pack(char buf[8])
{		
	json_object_array_add(errcode_array, json_object_new_string(buf));	
	return 0;
}

/********************************************************************
  函数名称:  int file_to_flash (int fd,u_int32_t offset,u_int32_t len,char *filename)
  功能说明:  写数据到emmc，通用api
  输入参数:  有
  输出参数:  无
  返回值:    int
********************************************************************/
int file_to_flash (int fd,u_int32_t offset,u_int32_t len,char *filename)
{
	
	char *buf = NULL;
	int err;
	int size = len * sizeof (u_int8_t);

	if (offset != lseek (fd,offset,SEEK_SET))
	{
		perror ("lseek()");
		return -1;
	}
    //开辟一个空间存储数据
	if ((buf = (char *) malloc (size)) == NULL)
	{
		perror ("malloc()");
		return -1;
	}
	char * buf_twice = buf;

	int i ;
	for(i =0 ;i <size ;i++)
	{
		*buf = filename[i];
		buf++;
	}
//test
	//buf= buf - 3;
	//printf("--%x\n",*buf);
	//printf("--%x\n",*buf_twice);


	err = write (fd,buf_twice,size);
	if (err < 0)
	{
		perror ("write()");	
		return -1;
	}

	if (buf_twice != NULL)
		free (buf_twice);
	
	printf(" ########## add a new code ###########\n");
	printf ("Copied %d bytes to address 0x%.8x in flash\n",len,offset);
	printf ("Copied %d bytes to address %d in flash\n",len,offset);
	return 0;
}
/********************************************************************
  函数名称:  int check_info(void)
  功能说明:  检查下一条故障信息是否存在，通用api
  输入参数:  有
  输出参数:  无
  返回值:    int
********************************************************************/
int CURRENT_I = 0;
char * check_last_info(void)
{
	int fd;
	/* open device */
	if ((fd = open (DEVICE_NAME,O_SYNC | O_RDWR)) < 0)
	{
		perror ("open()");
		exit (1);
	}
	int i;
	char addr[5];
	for(i=1 ;i <1025;i++)
	{
		sprintf(addr,"%d", (20480 + (i -1)*8));
		lseek (fd,strtol (addr,NULL,0),SEEK_SET);
		char buf_three[8];
		read(fd,buf_three,8);
		
		if( (buf_three[0] & 0xf0 ) != 0xa0 )
		{
			strcpy(addr_current,addr);
			printf("<<< addr_current [%s] >>>\n",addr_current);
			close(fd);
			break;
		}
		bzero(buf_three,8);
	}
	if(i > MAX_NUM_ZONE )
	{
		CURRENT_I = i;
		printf("emmc_dia zone full [CURRENT_I = %d]\n",CURRENT_I);
	}
	return addr_current;
}
/********************************************************************
  函数名称:  check_repo_info
  功能说明:  检测写入的信息时候和最后一个信息相同
  输入参数:  有
  输出参数:  无
  返回值:    int 
********************************************************************/

int check_repo_info(char str[])
{
	int fd;
	/* open device */
	if ((fd = open (DEVICE_NAME,O_SYNC | O_RDWR)) < 0)
	{
		perror ("open()");
		return -1;
	}
	int addr_pre = atoi(addr_current)-8;
	printf(" <<<addr_current_pre = [%d] >>>\n",addr_pre);
	sprintf(addr_current_pre,"%d",addr_pre);
	lseek (fd,strtol (addr_current_pre,NULL,0),SEEK_SET);
	char buf_read[8];
	read(fd,buf_read,8);
	
	//判断 头部，类型，参数是否一样,是的话就立刻更新数据
	if((buf_read[0] == str[0]) && (buf_read[1] == str[1]) && (buf_read[2] == str[2]))
	{
		char tmp = 0xfd;
		if(buf_read[3]  > tmp)
		{
			printf(" ***********!!!! this code count is %x !!!***********\n",buf_read[3]);
			close(fd);
			return -1;
		}
		printf(" *********** update before last code ***********\n");
		buf_read[3] = buf_read[3] + 0x01;
		buf_read[4] = str[4];
		buf_read[5] = str[5];
		buf_read[6] = str[6];
		buf_read[7] = str[7];
		lseek (fd,strtol (addr_current_pre,NULL,0),SEEK_SET);
		write (fd,buf_read,8);
		close(fd);
		printf(" ******** update before last code finish ***********\n");
		return 0;   // 更新成功
	}
	else
	{
		close(fd);
		return 1;   // 数据不一样，往后面填写数据
	}
}

/********************************************************************
  函数名称:  int write_diagnosis_update_info(char *update_info);
  功能说明:  写更新数据到emmc
  输入参数:  有
  输出参数:  无
  返回值:    
********************************************************************/
int write_diagnosis_update_info(char *update_info)
{
	int fd;
	/* open device */
	if ((fd = open (DEVICE_NAME,O_SYNC | O_RDWR)) < 0)
	{
		perror ("open()");
		exit (1);
	}
	//地址写固定从16开始，传8位信息
	file_to_flash(fd,strtol ("16",NULL,0),strtol ("8",NULL,0),update_info);
	
	/* close device */
	if (close (fd) < 0)
	perror ("close()");

	return 0;
}

/********************************************************************
  函数名称:  int read_diagnosis_update_info(char **update_info); 
  功能说明:  读更新数据从emmc
  输入参数:  有
  输出参数:  无
  返回值:    
********************************************************************/
char * read_diagnosis_update_info()
{
	int fd;
	if ((fd = open (DEVICE_NAME,O_SYNC | O_RDWR)) < 0)
	{
		perror ("open()");
		exit (1);
	}
	
	lseek (fd,strtol ("16",NULL,0),SEEK_SET);
	char buf_read[8];
	read(fd,buf_read,8);

	printf("\n");
	int i;
	for(i = 0;i < 8 ; i++)
    {
		repo_buf_update[i] = buf_read[i];
		printf("debug %x\n",buf_read[i]);
    }
	close(fd);
	return repo_buf_update;
}

/********************************************************************
  函数名称:  read_diagnosis
  功能说明:  获取信息
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0:关闭成功; 小于0:关闭失败
********************************************************************/
const char * read_diagnosis()
{
	//int count_one = 0;
	//int count_two = 0;
	errcode_array = json_object_new_array();
	int fd;
	if ((fd = open (DEVICE_NAME,O_SYNC | O_RDWR)) < 0)
	{
		perror ("open()");
		exit (1);
	}
	
	lseek (fd,strtol ("20480",NULL,0),SEEK_SET);
	char buf_read[8];
	char addr[5];
	int i;
	int len_all = 1025;
	//int j;
	for(i = 1;i < len_all ;i++)
	{
		read(fd,buf_read,8);
		printf("buf_read[0] == %02X\n",buf_read[0]);
		if( (buf_read[0] & 0xf0 ) == 0xa0 )
		{
			//test
			//printf("-----test read from %d\n",(20480+(i-1)*8));
			//for(j = 0;j < 8 ; j++)
			//{
			//	printf("debug %x\n",buf_read[j]);
			//}//test
		
			//数据1条条的打包
			pack(buf_read);
			//count_one++;
			//printf("count_one =%d ------!!!!!\n",count_one);

			
			bzero(buf_read,8);
			sprintf(addr,"%d", (20480 + i*8));
			lseek (fd,strtol (addr,NULL,0),SEEK_SET);
		}
		else
		{
			printf("---no read info head -----\n");
			break;
		}
	}
	close(fd);
	
	//----------------------------------------------------
	if ((fd = open (DEVICE_NAME,O_SYNC | O_RDWR)) < 0)
	{
		perror ("open()");
		exit (1);
	}
	bzero(buf_read,8);
	//从 uboot读取数据
	lseek (fd,strtol ("512",NULL,0),SEEK_SET);
	char addr_two[4];
	int j;      						//块大小一个扇区大小
	int k = 0; 							//偏移基数
	int len = 64;
	for(j = 0;j < len ;j++ )			//循环64次，遍历一个扇区的大小
	{
		read(fd,buf_read,8);
		//printf("buf_read[0] == %02X\n",buf_read_two[0]);
		if(  buf_read[0]  == 0xa0 )		//只上报os类型
		{
			//数据1条条的打包
			pack(buf_read);
			//count_two++;
			//printf("count_two =%d ------!!!!!\n",count_two);
			bzero(buf_read,8);
			k++;
			sprintf(addr_two,"%d", (512 + k*8));
			lseek (fd,strtol (addr_two,NULL,0),SEEK_SET);
		}
		else
		{
			printf("---no read info head -----\n");
			break;
		}
	}
	
	close(fd);
	const char * buf_ret =  json_object_to_json_string(errcode_array);
	return buf_ret;
}

/********************************************************************
  函数名称:  write_diagnosis
  功能说明:  写入信息
  输入参数:  无
  输出参数:  无
  返回值:    大于等于0：成功; 小于0：失败
********************************************************************/
int write_diagnosis(CONFIG_DIAGNOSIS_HEAD head,CONFIG_DIAGNOSIS_CODE code,CONFIG_DIAGNOSIS_PARA para)
{
	int semid = -1;
	semid = create_sem(ftok(".", 'b'), 1); /*保护的资源数为1 */
	if( semid <0 ) {
	printf("*****semaphone get error!*******\n");
	return 2;
    }
	init_sem(semid, 0, 1);  			/* 初始化信号量可用*/
	sem_p(semid, 0, 1); //P操作
	
	char dia_str[SIZE_DIA];
	
	
	//获取时间
	time_t tm;
    time(&tm);
    struct tm *tp = localtime(&tm);
    int year  = tp->tm_year+1900;
    int mon   = tp->tm_mon+1;
    int day   = tp->tm_mday;
    int hour  = tp->tm_hour;
    int min   = tp->tm_min;
	dia_str[4] = year - 2000;
    dia_str[5] = (mon << 4) + (0x0F & (day >> 4));
    int minutes = hour * 60 + min;
    dia_str[6] = ((0x0F & day) << 4) + (0x00F &(minutes >> 8));
    dia_str[7] = (0xFF & minutes);
	//收集时间
	
	dia_str[1] = code;
	dia_str[2] = para;
	dia_str[3] = 0x01;
	
	switch(head)
	{
		case ENUM_HAL_HEAD_MCU: dia_str[0] = ENUM_HAL_HEAD_MCU;break;
	
		case ENUM_HAL_HEAD_OS:  dia_str[0] = ENUM_HAL_HEAD_OS; break;
								
		case ENUM_HAL_HEAD_MPU: dia_str[0] = ENUM_HAL_HEAD_MPU;break;

		default :printf(" !!! no this dia head\n "); return -2;
	}
	//---------------------------------------------------------------------------数据打包
	int err = 0,fd;
	//检索最后一个地址是否含有故障信息
	check_last_info();
	//判断
	if(CURRENT_I > MAX_NUM_ZONE )
	{
		CURRENT_I = 0;
		__clean_all_emmc();
		return 4; // 清完数据、直接退出
	}
	//检测即将写入的信息是否是同一个类型的故障信息
	int ret = check_repo_info(dia_str);  //返回为1 （往后写数据） 、返回为0 （更新成功）、-1（次数达到0xfe 不在累加，直接退出程序）
	printf("check_repo_info ret = %d\n",ret);
	if( 1 == ret )
	{
		printf("----------------ready to write a code to last zone\n");
		/* open device */
		if ((fd = open (DEVICE_NAME,O_SYNC | O_RDWR)) < 0)
		{
			perror ("open()");
			return -1;
		}
	
		//地址写固定，传8位
		err = file_to_flash(fd,strtol (addr_current,NULL,0),strtol ("8",NULL,0),dia_str);
		//printf("< err = %d> line = %d\n",err,__LINE__);
		/* close device */
		close(fd);
	}
	else if ( 0 == ret )
	{
		//printf("<ret = %d>\n",ret);
		err = 0;
	}
	else if( -1 == ret)
	{
		printf("<write error ret = %d>\n",ret);
		err = -1;
	}
	else
	{
		
	}
	
	sem_v(semid, 0, 1);  //V操作
	del_sem(semid);

	return err;
}

/********************************************************************
  函数名称:  write_diagnosis_head_info
  功能说明:  写故障头部信息
  输入参数:  有
  输出参数:  无
  返回值:    int
********************************************************************/
int write_diagnosis_head_info(char *head_info)
{
	int err = 0,fd;
	/* open device */
	if ((fd = open (DEVICE_NAME,O_SYNC | O_RDWR)) < 0)
	{
		perror ("open()");
		exit (1);
	}
	//地址写固定,从0x0开始，传16位
	err = file_to_flash(fd,strtol ("0x0",NULL,0),strtol ("16",NULL,0),head_info);
	
	/* close device */
	if (close (fd) < 0)
	perror ("close()");

	exit (err);
}

/********************************************************************
  函数名称:  read_diagnosis_head_info
  功能说明:  写故障头部信息
  输入参数:  有
  输出参数:  无
  返回值:    int
********************************************************************/
char * read_diagnosis_head_info()
{
	int fd;
	if ((fd = open (DEVICE_NAME,O_SYNC | O_RDWR)) < 0)
	{
		perror ("open()");
		exit (1);
	}
	lseek (fd,strtol ("0x0",NULL,0),SEEK_SET);
	char buf_read[16];
	read(fd,buf_read,16);
	
	printf("\n");
	int i;
	for(i = 0;i < 16 ; i++)
    {	
		repo_buf_head[i] = buf_read[i];
		printf("debug %x\n",buf_read[i]);	
    }
	
	close(fd);
	return repo_buf_head;
}


/*****************************************************************************************************/


/********************************************************************
  函数名称:  CreateDiagnosisHal 
  功能说明:  创建HAL层设备 
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：创建成功; 小于0：创建失败 
********************************************************************/	
int createDiagnosisHal(T_HalDiagnosisDevice** device)
{
	T_HalDiagnosisDevice *dev;
	dev = (T_HalDiagnosisDevice *)malloc(sizeof(T_HalDiagnosisDevice));	
    if (dev == NULL)
	{
		return -1;
	}
	strcpy(dev->common.version, "2.0");
	strcpy(dev->common.name, " Diagnosis HAL For CSRatlas7");
	
	dev->writeDiagnosisHeadZone = write_diagnosis_head_info;
	dev->readDiagnosisHeadZone = read_diagnosis_head_info;
	
	dev->writeDiagnosisUpdateZone = write_diagnosis_update_info;
	dev->readDiagnosisUpdateZone = read_diagnosis_update_info;
	
	dev->writeDiagnosis = write_diagnosis;
	dev->readDiagnosis = read_diagnosis;
	
	
	*device = dev;
	return 0;
}

/********************************************************************
  函数名称:  DestoryDiagnosisHal 
  功能说明:  释放HAL层设备
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:     
********************************************************************/ 
int destoryDiagnosisHal(T_HalDiagnosisDevice** device)
{
	if (*device)
	{		
		free(*device);
		*device = NULL;
	}
	return 0;
}
