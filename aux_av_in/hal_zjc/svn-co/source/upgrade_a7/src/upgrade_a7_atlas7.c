/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：upgrade_a7_atlas7.c
* 文件标识：
* 摘 要：   
*
* 当前版本：1.0
* 作 者：   tpeng@foryouge.com.cn
* 完成日期：2017年6月5日
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/reboot.h>
#include<sys/mount.h>  
#include<errno.h> 　
#include<dirent.h>
#include<signal.h>
#include<ctype.h>
#include<termios.h>
#include<sys/mman.h>

#include "hal_upgrade_a7.h"

/*----------------------------------------------*
 * 宏定义                                     *
 *----------------------------------------------*/
#define SIZE_LEN 1
#define BUF_SIZE 1024
#define PATH_LEN 128

#define PATH_DEV_FILE2   	  "/dev/mmcblk0p2"
#define PATH_DEV_FILE11  	  "/dev/mmcblk0p11"
#define PATH_MOUNT   	   	  "/mnt/mmcblk0p2"

#define PATH_FILE_D_ZIMAGE1  "/mnt/mmcblk0p2/zImage-v1"
#define PATH_FILE_D_DTB1  	 "/mnt/mmcblk0p2/dtb-v1"

#define PATH_FILE_D_ZIMAGE2  "/mnt/mmcblk0p2/zImage-v2"
#define PATH_FILE_D_DTB2  	 "/mnt/mmcblk0p2/dtb-v2"

#define PATH_FILE_D_CSRVISOR  "/mnt/mmcblk0p2/"
#define PATH_FILE_D_BOOT_CFG  "/mnt/mmcblk0p2/" 
 
#define PATH_FILE_LIB_MODULES  "/media/mmcblk0p3/lib/"

#define VERSION   "Upgrade a7 HAL V3.9"


/*----------------------------------------------*
 * 变量定义                                      *
 *----------------------------------------------*/

int g_iM3_status =0;
int g_iFd        =0;
int g_iFpmsg_fd;

FILE * g_pFile;
char * g_pBuffer;

unsigned long  g_dwSize		=0;
unsigned long  g_dwChecknum	=0;

char g_cMountFlag =0;
char g_cDirFlag =0;

char g_cZimageFlag =0;
char g_cDtbFlag =0;
char g_CmodulesFlag =0;
char g_CcsrvisorFlag =0;
char g_CbootcfgFlag  =0;

/*----------------------------------------------*
 * 内部函数说明                                	*
 *----------------------------------------------*/

int my_err(char *pErr_string, int iLine );

int copy_data(const int iFrd,const int iFwd);

int do_my_cp(int argc,const char *pPath_s,const char *pPath_d);

int CreateDir(const char *pPathName);


/*----------------------------------------------*
 * CSR A7 				                       	*
 *----------------------------------------------*/

char rpmsg_write()
{
	char iRet 	= -1;
	int i =0;

	unsigned char buf[12];

	buf[0] = 0x6a ;
	buf[1] = 0xa6 ;
	buf[2] = 0x00 ;
	buf[3] = 0x00 ;
	buf[4] = 0x03 ;
	buf[5] = 0x04 ;
	buf[6] = 0x00 ;
	buf[7] = 0x04 ;

	buf[8] =  buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7];
	buf[8] =(~ buf[8])+1;

	printf("[%s][%d] =========rpmsgrpmsg write=============  \n",__func__,__LINE__);

	for(i=0;i<9;i++)
	{
		printf("  %x  ",buf[i]);
	}

	iRet= write( g_iFpmsg_fd,buf, 9 );	
	if(iRet<0)
	{
		printf("[%s][%d]: write failed!!  \n",__func__,__LINE__);
		return -1;
	}

    return 0;
}

int sys_reboot(void)
{
	int iRet = -1;
	int i=0;

	printf("[%s][%d]: rpmsgrpmsg reboot=============  \n",__func__,__LINE__);

	g_iFpmsg_fd = open("/dev/tty_rpmsg",O_RDWR);
	if(g_iFpmsg_fd < 0)
	{
		printf("[%s][%d]:  open failed!!  \n",__func__,__LINE__);
		return -1;
	}
	
	for(i=0;i<3;i++)
	{
		iRet= rpmsg_write();
		if(iRet<0)
		{
			printf("[%s][%d]:  rpmsg_write failed!!  \n",__func__,__LINE__);
			return -1;
		}
		sync();
		sleep(1);
	}
	close(g_iFpmsg_fd);
	return 0;
}

int emmc_read_flag(const char *pPath)
{

	int iFd = -1;
	int iRet =-1;
	char * pBuffer = NULL;
	int i=0;
	char cFlag=0;

	iFd = open(pPath,O_RDWR);
	if(iFd < 0)
	{
		printf("[%s][%d]:  open failed!!  \n",__func__,__LINE__);
		goto f_open;
	}
	
	pBuffer = malloc( sizeof(char) * SIZE_LEN );
	if(NULL == pBuffer)
	{
		printf("[%s][%d]:  malloc failed!!  \n",__func__,__LINE__);
		goto f_malloc;
	}

	memset(pBuffer,0,SIZE_LEN);

	for(i=0;i<SIZE_LEN;i++)
	{
		printf(" %x ",pBuffer[i]); 	
	}

	iRet =lseek(iFd,0,SEEK_SET);
	if(iRet<0)
	{
		printf("[%s][%d]:  lseek failed!!  \n",__func__,__LINE__);
		return  UPDATE_ERR;
	}

	iRet = read(iFd,pBuffer,SIZE_LEN);
	if(iRet<0)
	{
		printf("[%s][%d]:  read failed!!  \n",__func__,__LINE__);
		return  UPDATE_ERR;
	}
	fsync(iFd);

	cFlag = pBuffer[0];
	
	printf("[%s][%d]:  pBuffer[0]=%d   cFlag=%d  \n",__func__,__LINE__,pBuffer[0],cFlag);

	free(pBuffer);

	return cFlag;

f_malloc:
	close(iFd);
f_open:
	free(pBuffer);
	return UPDATE_ERR;
}

int emmc_write_flag(const char *pPath,char cFlag)
{

	int iFd = -1;
	int iRet =-1;
	char * pBuffer1 = NULL;
	char * pBuffer2 = NULL;
	int i=0;

	iFd = open(pPath,O_RDWR);
	if(iFd < 0)
	{
		printf("[%s][%d]:  open failed!!  \n",__func__,__LINE__);
		goto f_open;
	}
	
	pBuffer1 = malloc( sizeof(char) * SIZE_LEN );
	if(NULL == pBuffer1)
	{
		printf("[%s][%d]:  malloc failed!!  \n",__func__,__LINE__);
		goto f_malloc;
	}

	memset(pBuffer1,cFlag,SIZE_LEN);
	for(i=0;i<SIZE_LEN;i++)
	{
		printf(" %x ",pBuffer1[i]); 	
	}

	iRet = lseek(iFd,0,SEEK_SET);
	if(iRet<0)
	{
		printf("[%s][%d]:  lseek failed!!  \n",__func__,__LINE__);
		return  UPDATE_ERR;
	}

	iRet = write(iFd,pBuffer1,SIZE_LEN);
	if(iRet<0)
	{
		printf("[%s][%d]:  write failed!!  \n",__func__,__LINE__);
		return  UPDATE_ERR;
	}
	fsync(iFd);

	pBuffer2 = malloc( sizeof(char) * SIZE_LEN );
	if(NULL == pBuffer2)
	{
		printf("[%s][%d]:  malloc failed!!  \n",__func__,__LINE__);
		goto f_malloc;
	}

	memset(pBuffer2,0,SIZE_LEN);

	iRet =lseek(iFd,0,SEEK_SET);
	if(iRet<0)
	{
		printf("[%s][%d]:  lseek failed!!  \n",__func__,__LINE__);
		return  UPDATE_ERR;
	}

	iRet =read(iFd,pBuffer2,SIZE_LEN);
	if(iRet<0)
	{
		printf("[%s][%d]:  read failed!!  \n",__func__,__LINE__);
		return  UPDATE_ERR;
	}
	fsync(iFd);

	for(i=0;i<SIZE_LEN;i++)
	{
		printf(" %x ",pBuffer2[i]); 	
	}
	printf(" \n");

	free(pBuffer1);
	free(pBuffer2);
	
	return UPDATE_OK;

f_malloc:
	close(iFd);
f_open:
	free(pBuffer1);
	free(pBuffer2);
	return  UPDATE_ERR;
}

int my_err(char *pErr_string, int iLine )
{
    fprintf(stderr,"iLine:%d ",iLine);
    perror(pErr_string); 
    return -1;
}

int copy_data(const int iFrd,const int iFwd)
{
    int iRead_len = 0, iWrite_len = 0;
    unsigned char cBuf[BUF_SIZE];
    unsigned char *pBuf;

    while ( (iRead_len = read(iFrd,cBuf,BUF_SIZE))) 
	{
        if (-1 == iRead_len) 
		{
            return my_err("Read error", __LINE__);
        }
        else if (iRead_len > 0) 
		{ 
            pBuf = cBuf;
            while ( (iWrite_len = write(iFwd,pBuf,iRead_len)) ) 
			{
                if(iWrite_len == iRead_len) 
				{
                    break;
                }
                else if (iWrite_len > 0) 
				{ 
                    pBuf += iWrite_len;
                    iRead_len -= iWrite_len;
                }
                else if(-1 == iWrite_len) 
				{
                    return my_err("Write error", __LINE__);
                }
            }
            ////if (-1 == iWrite_len) break;
       }
    }
	return 0;
}

int do_copy(int argc,const char *pPath_s,const char *pPath_d) 
{
    int iFrd, iFwd; 
    int iLen = 0;
    char *pSrc, *pDes; 
    struct stat src_st,des_st;
	int iRet =-1;
    
    if (argc < 3) 
	{
        return my_err("arguments error ", __LINE__);
    }
  
    iFrd = open(pPath_s,O_RDONLY);
    if (iFrd == -1) 
	{
        return my_err("Can not opne file", __LINE__);
    }

    if (fstat(iFrd,&src_st) == -1) {
        return my_err("stat error",__LINE__);
    }

    if (S_ISDIR(src_st.st_mode)) {
        return my_err("err dir",__LINE__);
    }

    pDes = pPath_d;
    stat(pPath_d,&des_st);

    if (S_ISDIR(des_st.st_mode)) 
	{ 
        iLen = strlen(pPath_s);
        pSrc = pPath_s + (iLen-1);

        while (pSrc >= pPath_s && *pSrc != '/') {
            pSrc--;
        }

        pSrc++;
        iLen = strlen(pPath_d); 

        if (1 == iLen && '.' == *(pPath_d)) {
            iLen = 0; 
            pDes = pSrc;
        }
        else 
		{ 
            pDes = (char *)malloc(sizeof(char)*PATH_LEN);
            if (NULL == pDes) 
			{
                return my_err("malloc error ", __LINE__);
            }
            strcpy(pDes,pPath_d);
       
            if ( *(pDes+(iLen-1)) != '/' ) 
			{ 
                strcat(pDes,"/");
            }
            strcat(pDes+iLen,pSrc);
        }
    }

    iFwd = open(pDes,O_WRONLY | O_CREAT | O_TRUNC,src_st.st_mode);
    if (iFwd == -1) 
	{
        return my_err("Can not creat file", __LINE__);
    }

    iRet = copy_data(iFrd,iFwd);
	if(iRet<0)
	{
		printf("[%s][%d]:  copy_data err!  \n",__func__,__LINE__);
		return  -1;
	}

    if (iLen > 0 && pDes != NULL)
	{
        free(pDes);
	}

	close(iFrd);
	close(iFwd);
	return 0;
}

int  CreateDir(const char *pPathName)  
{  
	  char   cDirName[256];  
	  int    i;
	  int    iLen=0;

	  strcpy(cDirName,pPathName); 
	 
	  iLen = strlen(cDirName); 
	 
	  if(cDirName[iLen-1]!='/') 
	  { 
	  	  strcat(cDirName,   "/");  
	  }

	  iLen = strlen(cDirName);  
	   
	  for(i=1;i<iLen;i++)  
	  {  
		  if(cDirName[i]=='/')  
		  {  
			  cDirName[i] = 0;  

			  if(access(cDirName,NULL)!=0)  
			  {  
				  if(mkdir(cDirName,0755)==-1)  
				  {   
			          printf("[%s][%d]:  mkdir err!  \n",__func__,__LINE__);  
			          return   -1;   
				  }  
			  }  
			  cDirName[i] = '/';  
		  }  
	  }     
	  return   0;  
} 

int create_dir(void)
{
	int iRet   =-1;
	DIR *dirptr = NULL;

	printf("[%s][%d]=========================\n",__func__,__LINE__);

	dirptr = opendir(PATH_MOUNT);

	if( dirptr == NULL )
	{
		printf("[%s][%d]===========NULL=========\n",__func__,__LINE__);

		iRet =CreateDir(PATH_MOUNT);
		if(iRet<0)
		{
			printf("[%s][%d]:  CreateDir failed!!  \n",__func__,__LINE__);
			return  -1;
		}
		sync();
	}

	closedir(dirptr);

	return 0;
}


int check_update_file_name(const char *pFile_name) 
{
	int iRet   =-1;

	if(NULL == pFile_name )
	{
		printf("[%s][%d]: pFile_name null \n",__func__,__LINE__);
		return -1;
	}

	printf("[%s][%d]: pFile_name=%s  \n",__func__,__LINE__,pFile_name);

	if(strcmp(pFile_name,"zImage") == 0)  
	{
		printf("zImage file ok \n");
		return UPDATE_OK;	
	} 
	else if(strcmp(pFile_name,"dtb") == 0)  
	{
		printf("dtb file ok \n");
		return UPDATE_OK;
	} 
	else if(strcmp(pFile_name,"modules") == 0)  
	{
		printf("modules file ok \n");
		return UPDATE_OK;
	} 
	else if(strcmp(pFile_name,"csrvisor.bin") == 0)  
	{
		printf("csrvisor.bin file ok \n");
		return UPDATE_OK;
	} 
	else if(strcmp(pFile_name,"boot.cfg") == 0)  
	{
		printf("boot.cfg file ok \n");
		return UPDATE_OK;
	} 
	else if(strcmp(pFile_name,"u-boot.csr") == 0)  
	{
		printf("u-boot.csr file ok \n");
		return UPDATE_OK;
	}
	else if(strcmp(pFile_name,"kernel.img") == 0)  
	{
		printf("kernel.img file ok \n");
		return UPDATE_OK;
	} 
	else if(strcmp(pFile_name,"rootfs.img") == 0)  
	{
		printf("rootfs.img file ok \n");
		return UPDATE_OK;
	} 

	return UPDATE_ERR;
}

char* get_filename(char *pPath)
{
	int  iLen=0;
	char *pCurrent=NULL;

	if(NULL ==pPath )
	{
		printf("[%s][%d]: path null \n",__func__,__LINE__);
		return NULL;
	}
	iLen=strlen(pPath);

	for (;iLen>0;iLen--)
	{
		if(pPath[iLen]=='/')
		{
			pCurrent=&pPath[iLen+1];
			break;
		}
	}
	printf("[%s][%d]: pCurrent=%s iLen =%d \n",__func__,__LINE__,pCurrent,iLen);

	return pCurrent;
}

int  get_path_len(char *pPath)
{
	int  iLen=0;
	int  iRetLen=0;
	char *pCurrent=NULL;

	if(NULL ==pPath )
	{
		printf("[%s][%d]: path null \n",__func__,__LINE__);
		return NULL;
	}
	iLen=strlen(pPath);
	iRetLen =iLen;

	for (;iLen>0;iLen--)
	{
		if(pPath[iLen]=='/')
		{
			pCurrent=&pPath[iLen+1];
			break;
		}
	}
	printf("[%s][%d]: pPath=%s iRetLen =%d \n",__func__,__LINE__,pPath,iRetLen);

	return iRetLen;
}

int mount_mmcblk0p2(void)
{
	int iRet   =-1;

	printf("[%s][%d]: mkdir  g_cDirFlag= %d\n",__func__,__LINE__,g_cDirFlag);
		
	if( !g_cDirFlag )
	{
		iRet = create_dir();
		if( iRet <0)
		{
			printf("[%s][%d]: create_dir err \n",__func__,__LINE__);
			return -1;
		}

		printf("[%s][%d]: mount  g_cDirFlag= %d\n",__func__,__LINE__,g_cDirFlag);

		iRet =mount(PATH_DEV_FILE2,PATH_MOUNT,"ext4",0,NULL);
		if(iRet<0)
		{
			if(EBUSY == errno)
				printf("mount error:%s\n", strerror(errno));

			iRet = system("umount /dev/mmcblk0p2");
			if(iRet==-1)
			{
				printf("[%s][%d]:  system umount  err \n",__func__,__LINE__);
				return -2;
			}
			else
			{
				printf("[%s][%d]: remount  iRet= %d \n",__func__,__LINE__,iRet); 

				iRet =mount(PATH_DEV_FILE2,PATH_MOUNT,"ext4",0,NULL);
				if(iRet<0)
				{
					if(EBUSY == errno)
						printf("[%s][%d]:  mount  err \n",__func__,__LINE__);
					return -3;
				}
				g_cDirFlag =1;
			}
		}
		else
		{
			g_cDirFlag =1;

			printf("[%s][%d]: mount  iRet= %d g_cMountFlag =%d\n",__func__,__LINE__,iRet,g_cDirFlag); 
		}
	}
	return 0;
}

int umount_mmcblk0p2(void)
{
	int iRet   =-1;

	iRet = system("umount /dev/mmcblk0p2");
	if(iRet<0)
	{
		printf("[%s][%d]:  cp csrvisor err \n",__func__,__LINE__);
		return UPDATE_ERR;
	}

	g_cDirFlag =0;
	printf("[%s][%d]==========update_finish umount================= \n",__func__,__LINE__);

	return UPDATE_OK;
}

int update_kernel(const char *pFile_Path) 
{
	int  iRet   =-1;
	int  iCheck =-1;
	char cFlag =0;
	char *pFile_Path_Name =NULL;
	int  iPathLen =0;
	char *pCmd_Cp =  "cp -rf";
	char pCmd[256]= {0};

	printf("[%s][%d] ****************VERSION=%s**************** \n",__func__,__LINE__,VERSION);

	printf("[%s][%d]: File_Path=%s \n",__func__,__LINE__,pFile_Path);

	pFile_Path_Name = get_filename(pFile_Path);
	if(pFile_Path_Name == NULL)
	{
		printf("[%s][%d]: get_filename NULL \n",__func__,__LINE__);
		return UPDATE_FILE_NAME_ERR;
	}

	iCheck = check_update_file_name(pFile_Path_Name);
	if(iCheck<0)
	{
		printf("[%s][%d]: check_update_file_name err \n",__func__,__LINE__);
		return UPDATE_CHECKE_NAME;
	}

	cFlag = emmc_read_flag(PATH_DEV_FILE11);
	if( cFlag <0)
	{
		printf("[%s][%d]: emmc_read_flag err \n",__func__,__LINE__);
		return UPDATE_READ_EMMC_FLAG;
	}

	printf("[%s][%d]============ cFlag=%d \n",__func__,__LINE__,cFlag);

	if(strcmp(pFile_Path_Name,"zImage") == 0)  
	{
			iRet = do_copy(3,pFile_Path,PATH_FILE_D_ZIMAGE1);
			if(iRet<0)
			{
				printf("[%s][%d]:  cp zImage-v1 err \n",__func__,__LINE__);
				return UPDATE_COPY_ZIMAGE_V1;
			}
			sync();

			iRet =do_copy(3,pFile_Path,PATH_FILE_D_ZIMAGE2);
			if(iRet<0)
			{
				printf("[%s][%d]:  cp zImage-v2 err \n",__func__,__LINE__);
				return UPDATE_COPY_ZIMAGE_V2;
			}

			sync();
			g_cZimageFlag =1;

			printf("[%s][%d]==============g_cZimageFlag=%d  \n",__func__,__LINE__,g_cZimageFlag); 
	} 

	if(strcmp(pFile_Path_Name,"dtb") == 0)  
	{
			iRet =do_copy(3,pFile_Path ,PATH_FILE_D_DTB1);
			if(iRet<0)
			{
				printf("[%s][%d]:  cp dtb-v1 err \n",__func__,__LINE__);
				return UPDATE_COPY_DTB_V1;
			}
			sync();

			iRet =do_copy(3,pFile_Path ,PATH_FILE_D_DTB2);
			if(iRet<0)
			{
				printf("[%s][%d]:  cp dtb-v2 err \n",__func__,__LINE__);
				return UPDATE_COPY_DTB_V2;
			}
			sync();
			g_cDtbFlag =1;

			printf("[%s][%d]: g_cZimageFlag=%d  \n",__func__,__LINE__,g_cDtbFlag); 
	}

	if(strcmp(pFile_Path_Name,"modules") == 0)  
	{
			printf("[%s][%d]: File_Path=%s \n",__func__,__LINE__,pFile_Path);

			printf("[%s][%d]: PATH_FILE_LIB_MODULES=%s  \n",__func__,__LINE__,PATH_FILE_LIB_MODULES); 

			sprintf(pCmd,"%s  %s  %s",pCmd_Cp,pFile_Path,PATH_FILE_LIB_MODULES);

			printf("[%s][%d]: pCmd=%s  \n",__func__,__LINE__,pCmd); 

			iPathLen = get_path_len(pCmd);

			printf("[%s][%d]: iPathLen=%d\n",__func__,__LINE__,iPathLen);  

			iRet = system(pCmd);
			if(iRet==-8)
			{
				printf("[%s][%d]: system modules  err \n",__func__,__LINE__);
				return UPDATE_COPY_MODULES;
			}
			g_CmodulesFlag =1;

			printf("[%s][%d]: g_CmodulesFlag=%d  \n",__func__,__LINE__,g_CmodulesFlag); 
	}

	if(strcmp(pFile_Path_Name,"csrvisor.bin") == 0)  
	{
			printf("[%s][%d]: File_Path=%s \n",__func__,__LINE__,pFile_Path);

			iRet =do_copy(3,pFile_Path ,PATH_FILE_D_CSRVISOR);
			if(iRet<0)
			{
				printf("[%s][%d]: cp csrvisor err \n",__func__,__LINE__);
				return UPDATE_COPY_CSRVISOR;
			}
			sync();

			g_CcsrvisorFlag =1;

			printf("[%s][%d]: g_CcsrvisorFlag=%d  \n",__func__,__LINE__,g_CcsrvisorFlag); 
	}

	if(strcmp(pFile_Path_Name,"boot.cfg") == 0)  
	{
			printf("[%s][%d]: File_Path=%s \n",__func__,__LINE__,pFile_Path);

			iRet =do_copy(3,pFile_Path ,PATH_FILE_D_BOOT_CFG);
			if(iRet<0)
			{
				printf("[%s][%d]:  cp csrvisor err \n",__func__,__LINE__);
				return UPDATE_COPY_BOOT_CFG;
			}
			sync();

			g_CbootcfgFlag =1;

			printf("[%s][%d]: g_CbootcfgFlag=%d  \n",__func__,__LINE__,g_CbootcfgFlag); 
	}

	printf("[%s][%d]: g_cZimageFlag=%d g_cDtbFlag =%d  g_CmodulesFla =%d g_CcsrvisorFlag=%d	g_CbootcfgFlag=%d \n",__func__,__LINE__,   \
                               g_cZimageFlag,g_cDtbFlag,g_CmodulesFlag,g_CcsrvisorFlag,g_CbootcfgFlag);

	if( ( g_cZimageFlag == 1 ) && ( g_cDtbFlag == 1 ) && ( g_CmodulesFlag == 1 ) && ( g_CcsrvisorFlag == 1 ) && ( g_CbootcfgFlag == 1 ))
	{
		g_cZimageFlag   = 0;
		g_cDtbFlag      = 0;
		g_CmodulesFlag  = 0;
		g_CcsrvisorFlag = 0;
		g_CbootcfgFlag = 0;

		sync();
		umount_mmcblk0p2();

		printf("[%s][%d] *********** update_finish ***********  \n",__func__,__LINE__);

		sync();
	}

	return UPDATE_OK;
}

int update_dd_uboot(const char *pFile_Path) 
{
	int  iRet   =-1;
	int  iCheck =-1;	
	char *pFile_Path_Name =NULL;
	char *pCmd_dd1 =  "dd if=";
	char *pCmd_dd2 =  "of=/dev/mmcblk0 seek=1 bs=512 count=3052 && sync";
	char pCmd_dd[256]= {0};

	printf("[%s][%d] ****************VERSION=%s******************* \n",__func__,__LINE__,VERSION);
	printf("[%s][%d]: File_Path=%s \n",__func__,__LINE__,pFile_Path);

	pFile_Path_Name = get_filename(pFile_Path);
	if(pFile_Path_Name == NULL)
	{
		printf("[%s][%d]:  get_filename NULL \n",__func__,__LINE__);
		return UPDATE_FILE_NAME_ERR;
	}

	iCheck = check_update_file_name(pFile_Path_Name);
	if(iCheck<0)
	{
		printf("[%s][%d]:  check_update_file_name err \n",__func__,__LINE__);
		return UPDATE_CHECKE_NAME;
	}
	sprintf(pCmd_dd,"%s%s  %s",pCmd_dd1,pFile_Path,pCmd_dd2);

	printf("[%s][%d]======system UBOOT=====pCmd_dd===%s \n",__func__,__LINE__,pCmd_dd); 

	iRet = system(pCmd_dd);
	if(iRet==-1)
	{
		printf("[%s][%d]:  system UBOOT err \n",__func__,__LINE__);
		return -1;
	}
	return 0;
}	

int update_dd_kernel(const char *pFile_Path) 
{
	int  iRet   =-1;
	int  iCheck =-1;
	char *pFile_Path_Name =NULL;
	char *pCmd_dd1 =  "dd if=";
	char *pCmd_dd2 =  "of=/dev/mmcblk0p2  && sync";
	char pCmd_dd[256]= {0};

	printf("[%s][%d] ****************VERSION=%s******************* \n",__func__,__LINE__,VERSION);
	printf("[%s][%d]: File_Path=%s \n",__func__,__LINE__,pFile_Path);

	pFile_Path_Name = get_filename(pFile_Path);
	if(pFile_Path_Name == NULL)
	{
		printf("[%s][%d]: get_filename NULL \n",__func__,__LINE__);
		return UPDATE_FILE_NAME_ERR;
	}

	iCheck = check_update_file_name(pFile_Path_Name);
	if(iCheck<0)
	{
		printf("[%s][%d]: check_update_file_name err \n",__func__,__LINE__);
		return UPDATE_CHECKE_NAME;
	}

	sprintf(pCmd_dd,"%s%s  %s",pCmd_dd1,pFile_Path,pCmd_dd2);

	printf("[%s][%d] ====== system KERNEL====== pCmd_dd=%s \n",__func__,__LINE__,pCmd_dd); 

	iRet = system(pCmd_dd);
	if(iRet==-1)
	{
		printf("[%s][%d]: system KERNEL err \n",__func__,__LINE__);
		return -1;
	}
	return 0;
}		


int update_dd_rootfs(const char *pFile_Path) 
{
	int  iRet   =-1;
	int  iCheck =-1;	
	char *pFile_Path_Name =NULL;
	char *pCmd_dd1 =  "dd if=";
	char *pCmd_dd2 =  "of=/dev/mmcblk0p3  && sync";
	char pCmd_dd[256]= {0};

	printf("[%s][%d] ****************VERSION=%s******************* \n",__func__,__LINE__,VERSION);
	printf("[%s][%d]: File_Path=%s \n",__func__,__LINE__,pFile_Path);

	pFile_Path_Name = get_filename(pFile_Path);
	if(pFile_Path_Name == NULL)
	{
		printf("[%s][%d]: get_filename NULL \n",__func__,__LINE__);
		return UPDATE_FILE_NAME_ERR;
	}

	iCheck = check_update_file_name(pFile_Path_Name);
	if(iCheck<0)
	{
		printf("[%s][%d]: check_update_file_name err \n",__func__,__LINE__);
		return UPDATE_CHECKE_NAME;
	}

	sprintf(pCmd_dd,"%s%s  %s",pCmd_dd1,pFile_Path,pCmd_dd2);

	printf("[%s][%d] ========== system ROOTFS ======= pCmd_dd=%s \n",__func__,__LINE__,pCmd_dd); 

	iRet = system(pCmd_dd);
	if(iRet==-1)
	{
		printf("[%s][%d]: system ROOTFS err \n",__func__,__LINE__);
		return -1;
	}
	return 0;
}	

/********************************************************************
  函数名称:  Create
  功能说明:  创建HAL层设备 
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:    等于0：创建成功; 小于0：创建失败 
********************************************************************/	
int createUpgradeHal(T_HalUpgradeDevice** device)
{
    T_HalUpgradeDevice *dev;
	dev = (T_HalUpgradeDevice *)malloc(sizeof(T_HalUpgradeDevice));	
    if (dev == NULL)
	{
		return -1;
	}

	strcpy(dev->common.version, VERSION);
	strcpy(dev->common.name, "Upgrade HAL For CSRatlas7");

	dev->emmc_read_flag 		= emmc_read_flag;
	dev->emmc_write_flag 		= emmc_write_flag;
	dev->mount_mmcblk0p2 		= mount_mmcblk0p2;
	dev->update_kernel 			= update_kernel;
	dev->sysre_boot 			= sys_reboot;

	dev->update_dd_kernel 		= update_dd_kernel;
	dev->update_dd_rootfs 		= update_dd_rootfs;

	*device = dev;

	return 0;
}

/********************************************************************
  函数名称:  DestoryUpgradeHal
  功能说明:  释放HAL层设备
  输入参数:  device：HAL层设备结构体指针地址 
  输出参数:  无 
  返回值:     
********************************************************************/ 
int destoryUpgradeHal(T_HalUpgradeDevice** device)
{
	if (*device)
	{		
		free(*device);
		*device = NULL;
	}
	
}


/************************ (C) COPYRIGHT Foryou *****END OF FILE****/




