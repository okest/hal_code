/*
* Copyright (c) 2017,华阳通用电子有限公司，软件工程中心
* All rights reserved.
*
* 文件名称：
* 文件标识：
* 摘 要：   
*
* 当前版本：1.0
* 作 者：   tpeng@foryouge.com.cn
			ganrzhang@foryouge.com.cn
* 完成日期：2017年7月28日
*/

#include "stdio.h"
#include "string.h"
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <pthread.h>  
#include <sched.h>  
#include <unistd.h> 
#include <sys/times.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "zlib.h"
#include <errno.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <poll.h>
#include <pthread.h>  

#include "hal_dvr_display.h"

/*----------------------------------------------*
 * 宏定义                                     	*
 *----------------------------------------------*/
#define V4L2_OSD_WIDTH              720
#define V4L2_OSD_HEIGHT             480
#define HANDLE                      void *
#define DVR_INPUT_DEV	            "/dev/sirf-cvbs"
#define DVR_OUTPUT_DEV			    "/dev/sirf-display0-vout0"
#define DVR_INPUT_BUFFER_CNT			6
#define DVR_OUTPUT_BUFFER_CNT			6
#define DVR_DMA_CNT						6
#define VERSION					"20170825-v1.5"
#define DVR_TIMEOUT				500
#define DVR_TIMESOUT			6

typedef struct
{
    unsigned int *pui_addr;
    unsigned int ui_addrlen;
    unsigned int ui_offset;
}V4L2_MAMP_ADDR_INFO_S;

typedef struct
{
    int ai_dma_fd[DVR_DMA_CNT];
    unsigned int aui_dmalen[DVR_DMA_CNT];
}V4L2_DMA_INFO_S;

#define DVR_SHOW_LOCK() do{\
    pthread_mutex_lock(&g_st_dvr_info.st_mutex);\
}while(0)

#define DVR_SHOW_UNLOCK() do{\
    pthread_mutex_unlock(&g_st_dvr_info.st_mutex);\
}while(0)


typedef struct
{
    int i_video_width;
    int i_video_height;
    unsigned int ui_pixelformat;
    int i_perpix_tytes;
}DVR_SHOW_FMT_INFO_S;

typedef struct
{
    int i_b_open;
    int i_devfd;
 //   unsigned int ui_memamap_num;
    V4L2_MAMP_ADDR_INFO_S st_memamap[DVR_INPUT_BUFFER_CNT];
}DVR_SHOW_INPUT_S;

typedef struct
{
    int i_b_open;
    int i_devfd;
 //   unsigned int ui_memamap_num;
    V4L2_MAMP_ADDR_INFO_S st_memamap[DVR_INPUT_BUFFER_CNT];
}DVR_SHOW_OUTPUT_S;


typedef struct
{
    int i_binit;

    int i_dvrintask_do;
    pthread_t pst_dvrintask;

    int i_dvrouttask_do;
    pthread_t pst_dvrouttask;
    
    pthread_mutex_t st_mutex;  

    int i_btimeout;

    DVR_SHOW_FMT_INFO_S st_fmtinfo;
    V4L2_DMA_INFO_S st_dma_info;
    DVR_SHOW_INPUT_S st_input;
    DVR_SHOW_OUTPUT_S st_output;
}DVR_SHOW_INFO_S;

static DVR_SHOW_INFO_S g_st_dvr_info=
{
    .i_binit = 0,
    .i_dvrintask_do = 0,
    .i_dvrouttask_do = 0,
};

static int __dvr_input_config(int i_devfd, DVR_SHOW_FMT_INFO_S *pst_fmtinfo)
{
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    struct v4l2_input input;
    struct v4l2_fmtdesc fmtdesc;
    v4l2_std_id std, std_read;
    struct v4l2_control ctrl;
    int ret = -1;
    int video_width = 720, video_height = 480 ; 
    unsigned int pixelformat;

    ret = ioctl(i_devfd, VIDIOC_QUERYCAP, &cap);
    if (ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);	
        return -1;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        printf("[%s][%d]  Not a capture device \n",__func__,__LINE__);	
        return -1;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING))
    {
        printf("[%s][%d]  Doesn't support streaming I/O \n",__func__,__LINE__);	
        return -1;
    }

    memset(&input, 0, sizeof(input));
    while (ioctl(i_devfd, VIDIOC_ENUMINPUT, &input) >= 0)
    {
        printf("ENUM input port%d: name = %s, type = 0x%x, %s\n",input.index, input.name, input.type,
        input.status == V4L2_IN_ST_NO_SIGNAL ? "No signal !" : "Signal detected !");

        input.index++;
    }

    memset(&input, 0, sizeof(input));
    input.index = 1;
    ret = ioctl(i_devfd, VIDIOC_S_INPUT, &input.index);
    if (ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);	
        return -1;
    }

    memset(&input, 0, sizeof(input));
    ret = ioctl(i_devfd, VIDIOC_G_INPUT, &input.index);
    if (ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);	
        return -1;
    }
    printf("Current input: %d\n", input.index);

    memset(&input, 0, sizeof(input));
    ret = ioctl(i_devfd, VIDIOC_QUERYSTD, &std);
    if (ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);	
        return -1;
    }

    if ((std & V4L2_STD_NTSC) || (std == V4L2_STD_NTSC_443)) 
    {
        printf("[%s][%d] : set to NTSC:  0x%x \n",__func__,__LINE__,(unsigned int)std );	
        std = V4L2_STD_NTSC;
        video_width = 720;
        video_height = 480;
    }
    else if ((std & V4L2_STD_PAL) || (std == V4L2_STD_PAL_Nc)) 
    {
        printf("[%s][%d] : set to PAL:  0x%x \n",__func__,__LINE__,(unsigned int)std );	

        std = V4L2_STD_PAL;
        video_width = 720;
        video_height = 576;
    }
    else 
    {
        printf("[%s][%d] : VIDIOC_QUERYSTD fails,set to PAL:  0x%x \n",__func__,__LINE__,(unsigned int)std );	
        std = V4L2_STD_PAL;
        video_width = 720;
        video_height = 576;
    }

    ret = ioctl(i_devfd, VIDIOC_S_STD, &std);
    if (ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);
        return -1;			
    }

    ret = ioctl(i_devfd, VIDIOC_G_STD, &std_read);
    if (ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);	
        return -1;
    }

    memset(&fmtdesc, 0, sizeof(fmtdesc));
    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    while (ioctl(i_devfd, VIDIOC_ENUM_FMT, &fmtdesc) >= 0)
    {
        fmtdesc.index++;
        pixelformat = fmtdesc.pixelformat;
        printf("ENUM pixelformat = '%c%c%c%c', description = '%s'\n",
        fmtdesc.pixelformat & 0xFF,
        (fmtdesc.pixelformat >> 8) & 0xFF,
        (fmtdesc.pixelformat >> 16) & 0xFF, 
        (fmtdesc.pixelformat >> 24) & 0xFF,
        fmtdesc.description);
    }
    memset(&fmt, 0, sizeof(fmt));
    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = video_width;
    fmt.fmt.pix.height      = video_height;

    fmt.fmt.pix.pixelformat = pixelformat;

    ret = ioctl(i_devfd, VIDIOC_S_FMT, &fmt);
    if (ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);	
        return -1;
    }

    ret = ioctl(i_devfd, VIDIOC_G_FMT, &fmt);
    if (ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);	
        return -1;
    }

    printf("G_FMT(f_in):  width = %u, height = %u, 4cc = %.4s, field = %d, [%d.%d]\n",
        fmt.fmt.pix.width, fmt.fmt.pix.height,(char *)&fmt.fmt.pix.pixelformat,
        fmt.fmt.pix.field, fmt.fmt.pix.bytesperline, fmt.fmt.pix.sizeimage);

    video_width = fmt.fmt.pix.width;
    video_height = fmt.fmt.pix.height;

    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = V4L2_CID_BRIGHTNESS;
    ctrl.value = 30;
    ret = ioctl(i_devfd, VIDIOC_S_CTRL, &ctrl);
    if (ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);	
        return -1;
    }

    ctrl.id = V4L2_CID_CONTRAST;
    ctrl.value = 100;
    ret = ioctl(i_devfd, VIDIOC_S_CTRL, &ctrl);
    if (ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);	
        return -1;
    }

    ctrl.id = V4L2_CID_SATURATION;
    ctrl.value = 144;
    ret = ioctl(i_devfd, VIDIOC_S_CTRL, &ctrl);
    if (ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);	
        return -1;
    }

    ctrl.id = V4L2_CID_HUE;
    ctrl.value = 10;
    ret = ioctl(i_devfd, VIDIOC_S_CTRL, &ctrl);
    if (ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);			
        return -1;
    }

    pst_fmtinfo->i_video_width = video_width;
    pst_fmtinfo->i_video_height = video_height;
    pst_fmtinfo->ui_pixelformat = pixelformat;
    pst_fmtinfo->i_perpix_tytes = 2;
    return 0;
}

static int __dvr_input_dma_req(int i_devfd, V4L2_DMA_INFO_S *pst_dma)
{
    struct v4l2_requestbuffers reqbuf;
    int i_ret = -1;
	struct v4l2_exportbuffer expbuf;
    unsigned int ui_idx = 0;
	struct v4l2_buffer st_buf;

    memset(&reqbuf, 0, sizeof(reqbuf));
    reqbuf.count  = DVR_DMA_CNT;
    reqbuf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    i_ret = ioctl(i_devfd, VIDIOC_REQBUFS, &reqbuf);
    if (i_ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);	
        return -1;
    }

    do
    {
        memset(&expbuf, 0, sizeof(expbuf));
		expbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		expbuf.index = ui_idx;
		i_ret = ioctl(i_devfd, VIDIOC_EXPBUF, &expbuf);
		if (i_ret == -1)
		{
			printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);	
            return -1;
		}
        pst_dma->ai_dma_fd[ui_idx] = expbuf.fd;

        memset(&st_buf, 0, sizeof(st_buf));
		st_buf.index = ui_idx;
		st_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		st_buf.memory = V4L2_MEMORY_MMAP;
		i_ret = ioctl(i_devfd, VIDIOC_QBUF, &st_buf);
		if (i_ret == -1)
		{
			printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);	
			return -1;
		}
        
        ui_idx++;
		
        if (ui_idx >= DVR_DMA_CNT)
        {
            break;
        }
    }while(1);

    return 0;
}

static int __dvr_input_dma_rel(int i_devfd, V4L2_DMA_INFO_S *pst_dma)
{
    struct v4l2_requestbuffers reqbuf;
    int i_ret = -1;
    unsigned int ui_idx = 0;

    do
    {
        i_ret = close(pst_dma->ai_dma_fd[ui_idx]);
        ui_idx++;
		
        if (ui_idx >= DVR_DMA_CNT)
        {
            break;
        }
    }while(1);

    memset(&reqbuf, 0, sizeof(reqbuf));
    reqbuf.count  = 0;
    reqbuf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    i_ret = ioctl(i_devfd, VIDIOC_REQBUFS, &reqbuf);
    if (i_ret < 0)
    {
        printf("[%s][%d]  ioctl faild i_ret=%d  i_devfd=%d \n",__func__,__LINE__, i_ret, i_devfd);	
        return -1;
    }
    return 0;
}

static int __dvr_output_config(int i_devfd, DVR_SHOW_FMT_INFO_S *pst_fmtinfo)
{
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    struct v4l2_requestbuffers reqbuf;
    int i_ret;
    i_ret = ioctl(i_devfd, VIDIOC_QUERYCAP, &cap);
    if (i_ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);
        return -1;		
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT))
    {
        printf("[%s][%d]  Not a output device  \n",__func__,__LINE__);	
        return -1;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING))
    {
        printf("[%s][%d]  Doesn't support streaming I/O  \n",__func__,__LINE__);
        return -1;		
    }

    memset(&fmt, 0, sizeof(fmt));
    fmt.type                = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    fmt.fmt.pix.width       = pst_fmtinfo->i_video_width;
    fmt.fmt.pix.height      = pst_fmtinfo->i_video_height;

    fmt.fmt.pix.pixelformat = pst_fmtinfo->ui_pixelformat;

    i_ret = ioctl(i_devfd, VIDIOC_S_FMT, &fmt);
    if (i_ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);
        return -1;		
    }

    i_ret = ioctl(i_devfd, VIDIOC_G_FMT, &fmt);
    if (i_ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);	
        return -1;
    }

    printf("G_FMT(f_out): width = %u, height = %u, 4cc = %.4s\n",
        fmt.fmt.pix.width, fmt.fmt.pix.height,(char *)&fmt.fmt.pix.pixelformat);

    return 0;
}

static int __dvr_output_dma_req(int i_devfd, DVR_SHOW_FMT_INFO_S *pst_fmtinfo, V4L2_DMA_INFO_S *pst_dma)
{
    struct v4l2_requestbuffers reqbuf;
    int i_ret = -1;

    memset(&reqbuf, 0, sizeof(reqbuf));
    reqbuf.count  = DVR_DMA_CNT;
    reqbuf.type   = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    reqbuf.memory = V4L2_MEMORY_DMABUF;
    i_ret = ioctl(i_devfd, VIDIOC_REQBUFS, &reqbuf);
    if (i_ret == -1)
    {
        printf("[%s][%d]  ioctl faild  \n",__func__,__LINE__);	
        return -1;
    }
    
    return 0;
}

static int __dvr_output_dma_rel(int i_devfd)
{
    struct v4l2_requestbuffers reqbuf;
    int i_ret;

    memset(&reqbuf, 0, sizeof(reqbuf));
    reqbuf.count  = 0;
    reqbuf.type   = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    reqbuf.memory = V4L2_MEMORY_DMABUF;
    i_ret = ioctl(i_devfd, VIDIOC_REQBUFS, &reqbuf);
    if (i_ret < 0)
    {
        printf("[%s][%d]  ioctl faild i_ret=%d  i_devfd=%d \n",__func__,__LINE__, i_ret, i_devfd);	
        return -1;
    }

    return 0;
}

static void *__dvr_in_do_thread(void *arg)
{
	struct pollfd st_in_fds[1] = 
	{
		{ .fd = g_st_dvr_info.st_input.i_devfd, .events = POLLIN },
	};
    int i_ret;
    int i_timeout_cnt = 0;
    int i_do_cnt = 0;
	struct v4l2_buffer st_buf;
    unsigned int ui_buffsize;
    
    DVR_SHOW_INPUT_S *pst_input = &(g_st_dvr_info.st_input);
    DVR_SHOW_OUTPUT_S *pst_output = &(g_st_dvr_info.st_output);
    V4L2_DMA_INFO_S *pst_dma_info = &(g_st_dvr_info.st_dma_info);

 	printf("[%s][%d][in] **********VERSION=%s   \n", __FUNCTION__, __LINE__, VERSION);

    ui_buffsize = g_st_dvr_info.st_fmtinfo.i_video_width * g_st_dvr_info.st_fmtinfo.i_video_height * g_st_dvr_info.st_fmtinfo.i_perpix_tytes;

    while (g_st_dvr_info.i_dvrintask_do != 0)   /* 2s timeout */
    {	
        i_ret = poll(st_in_fds, 1, DVR_TIMEOUT);
        if (i_ret <= 0)
        {
            i_timeout_cnt++;
			if(i_timeout_cnt > 30000)i_timeout_cnt = 0;

            if ( i_timeout_cnt > DVR_TIMESOUT )
            {
                DVR_SHOW_LOCK();
                g_st_dvr_info.i_btimeout = 1;
                DVR_SHOW_UNLOCK();
            }
        	printf("[%s][%d] cap poll timeout i_timeout_cnt=%d, i_do_cnt=%d \n", __FUNCTION__, __LINE__, i_timeout_cnt, i_do_cnt);
            continue;
        }
        else if ((st_in_fds[0].revents & POLLIN) == 0)
        {
            continue;
        }
        //else if (st_in_fds[0].revents & POLLIN)

        i_do_cnt++;

        //1 do
        memset(&st_buf, 0, sizeof(st_buf));
        st_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        st_buf.memory = V4L2_MEMORY_MMAP;
        i_ret = ioctl(pst_input->i_devfd, VIDIOC_DQBUF, &st_buf); 
        if (i_ret < 0)
        {
    		printf("[%s][%d]  ioctl faild \n", __FUNCTION__, __LINE__);
            continue;
        }
    	//printf("[%s][%d]   i_ret=%d  st_input_buf=%d fd=%d.%d, flags=%d,field=%d, length=%d  \n", \
            __FUNCTION__, __LINE__, i_ret, st_buf.index, st_buf.m.fd, \
            pst_dma_info->ai_dma_fd[st_buf.index], st_buf.flags, st_buf.field, st_buf.length);

        st_buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        st_buf.memory = V4L2_MEMORY_DMABUF;
        st_buf.bytesused = ui_buffsize;
        st_buf.m.fd = pst_dma_info->ai_dma_fd[st_buf.index];
        i_ret = ioctl(pst_output->i_devfd, VIDIOC_QBUF, &st_buf);
        if (i_ret < 0)
        {
    		printf("[%s][%d]  ioctl faild \n", __FUNCTION__, __LINE__);
            continue;
        }  
    }

   	printf("[%s][%d][end] *****VERSION=%s  i_ret =%d \n", __FUNCTION__, __LINE__, VERSION,i_ret);
    
    return (void *)NULL;
}

static void *__dvr_out_do_thread(void *arg)
{
	struct pollfd st_out_fds[1] = 
	{
		{ .fd = g_st_dvr_info.st_output.i_devfd, .events = POLLOUT },
	};
    int i_ret;
    int i_timeout_cnt = 0;
    int i_do_cnt = 0;
	struct v4l2_buffer st_buf;
    unsigned int ui_buffsize;
    
    DVR_SHOW_INPUT_S *pst_input = &(g_st_dvr_info.st_input);
    DVR_SHOW_OUTPUT_S *pst_output = &(g_st_dvr_info.st_output);
    V4L2_DMA_INFO_S *pst_dma_info = &(g_st_dvr_info.st_dma_info);

 	printf("[%s][%d][in] **********VERSION=%s   \n", __FUNCTION__, __LINE__, VERSION);

    ui_buffsize = g_st_dvr_info.st_fmtinfo.i_video_width * g_st_dvr_info.st_fmtinfo.i_video_height * g_st_dvr_info.st_fmtinfo.i_perpix_tytes;

    while (g_st_dvr_info.i_dvrouttask_do != 0)   /* 2s timeout */
    {	
        i_ret = poll(st_out_fds, 1, DVR_TIMEOUT);
        if (i_ret <= 0)
        {
            i_timeout_cnt++;
			if(i_timeout_cnt > 30000)i_timeout_cnt =0;
        	printf("[%s][%d] out poll timeout i_timeout_cnt=%d, i_do_cnt=%d \n", __FUNCTION__, __LINE__, i_timeout_cnt, i_do_cnt);
            continue;
        }
        else if ((st_out_fds[0].revents & POLLOUT) == 0)
        {
            continue;
        }
        
        //else if (st_out_fds[0].revents & POLLOUT)
        i_do_cnt++;
        
        //1 do
        memset(&st_buf, 0, sizeof(st_buf));
        st_buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        st_buf.memory = V4L2_MEMORY_DMABUF;
        i_ret = ioctl(pst_output->i_devfd, VIDIOC_DQBUF, &st_buf);  
        if (i_ret < 0)
        {
    		printf("[%s][%d]  ioctl faild \n", __FUNCTION__, __LINE__);
            continue;
        }
    	//printf("[%s][%d]   i_ret=%d  st_input_buf=%d fd=%d.%d \n", \
            __FUNCTION__, __LINE__, i_ret, st_buf.index, st_buf.m.fd, pst_dma_info->ai_dma_fd[st_buf.index]);
        
    	st_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    	st_buf.memory = V4L2_MEMORY_MMAP;
    	st_buf.bytesused = ui_buffsize;
    	i_ret = ioctl(pst_input->i_devfd, VIDIOC_QBUF, &st_buf);
        if (i_ret < 0)
        {
    		printf("[%s][%d]  ioctl faild \n", __FUNCTION__, __LINE__);
            continue;
        }        
    }

   	printf("[%s][%d][end] *****VERSION=%s  i_ret =%d \n", __FUNCTION__, __LINE__, VERSION,i_ret);
    
    return (void *)NULL;
}

static int _dvr_input_open(void)
{
    int i_ret = 0;
    DVR_SHOW_INPUT_S *pst_input = &(g_st_dvr_info.st_input);
    
 	printf("[%s][%d][in] **********VERSION=%s   \n", __FUNCTION__, __LINE__, VERSION);

    if (pst_input->i_b_open != 0)
    {
   		printf("[%s][%d]  pst_input->i_b_open =%d \n", __FUNCTION__, __LINE__,pst_input->i_b_open);
        goto __ret;
    }

    pst_input->i_devfd = -1;
	pst_input->i_devfd = open(DVR_INPUT_DEV, O_RDWR);
	if (pst_input->i_devfd < 0)
	{
		i_ret = -1;
    	printf("[%s][%d] open[%s] error  i_ret=%d \n", __FUNCTION__, __LINE__, DVR_INPUT_DEV,i_ret);
        goto __ret;
	}

    if (__dvr_input_config(pst_input->i_devfd, &(g_st_dvr_info.st_fmtinfo)) != 0)
    {
    	printf("[%s][%d] __dvr_input_config error !\n", __FUNCTION__, __LINE__);
        close(pst_input->i_devfd);
        pst_input->i_devfd = -1;
		i_ret = -1;
        goto __ret;
    }

    if (__dvr_input_dma_req(pst_input->i_devfd, &(g_st_dvr_info.st_dma_info)) != 0)
    {
    	printf("[%s][%d] __dvr_input_dma error !\n", __FUNCTION__, __LINE__);
        close(pst_input->i_devfd);
        pst_input->i_devfd = -1;
		i_ret = -1;
        goto __ret;
    }

    pst_input->i_b_open = 1;

__ret:    
   	printf("[%s][%d][end] *****VERSION=%s  i_ret =%d \n", __FUNCTION__, __LINE__, VERSION,i_ret);
    
    return i_ret; 
}

static int _dvr_input_close(void)
{
    int i_ret = 0;
    DVR_SHOW_INPUT_S *pst_input = &(g_st_dvr_info.st_input);

 	printf("[%s][%d][in] **********VERSION=%s   \n", __FUNCTION__, __LINE__, VERSION);

    if (pst_input->i_b_open == 0)
    {
   		printf("[%s][%d]  pst_input->i_b_open =%d \n", __FUNCTION__, __LINE__,pst_input->i_b_open);
        goto __ret;
    }
    __dvr_input_dma_rel(pst_input->i_devfd, &(g_st_dvr_info.st_dma_info));

    close(pst_input->i_devfd);
    pst_input->i_devfd = -1;
    pst_input->i_b_open = 0;
    
__ret:    
   	printf("[%s][%d][end] *****VERSION=%s  i_ret =%d \n", __FUNCTION__, __LINE__, VERSION,i_ret);
    
    return i_ret; 
}

static int _dvr_output_open()
{
    int i_ret = 0;
    DVR_SHOW_OUTPUT_S *pst_output = &(g_st_dvr_info.st_output);
    
 	printf("[%s][%d][in] **********VERSION=%s   \n", __FUNCTION__, __LINE__, VERSION);

    if (pst_output->i_b_open != 0)
    {
     	printf("[%s][%d]  pst_output->i_b_open =%d \n", __FUNCTION__, __LINE__,pst_output->i_b_open);
        goto __ret;
    }

    pst_output->i_devfd = -1;
	pst_output->i_devfd = open(DVR_OUTPUT_DEV, O_RDWR);
	if (pst_output->i_devfd < 0)
	{
		i_ret = -1;
    	printf("[%s][%d] open[%s] error  i_ret =%d \n", __FUNCTION__, __LINE__, DVR_INPUT_DEV,i_ret);
        goto __ret;
	}
    
    if (__dvr_output_config(pst_output->i_devfd, &(g_st_dvr_info.st_fmtinfo)) != 0)
    {
    	printf("[%s][%d] __dvr_output_config error !\n", __FUNCTION__, __LINE__);
        close(pst_output->i_devfd);
        pst_output->i_devfd = -1;
		i_ret = -1;
        goto __ret;
    }
    if (__dvr_output_dma_req(pst_output->i_devfd, &(g_st_dvr_info.st_fmtinfo), &(g_st_dvr_info.st_dma_info)) != 0)
    {
    	printf("[%s][%d] __dvr_output_dma_req error !\n", __FUNCTION__, __LINE__);
        close(pst_output->i_devfd);
        pst_output->i_devfd = -1;
		i_ret = -1;
        goto __ret;
    }
    
    pst_output->i_b_open = 1;

__ret:    
   	printf("[%s][%d][end] *****VERSION=%s  i_ret =%d \n", __FUNCTION__, __LINE__, VERSION,i_ret);
    
    return i_ret; 
}

static int _dvr_output_close()
{
    int i_ret = 0;
    DVR_SHOW_OUTPUT_S *pst_output = &(g_st_dvr_info.st_output);

  	printf("[%s][%d][in] **********VERSION=%s   \n", __FUNCTION__, __LINE__, VERSION);

    if (pst_output->i_b_open == 0)
    {
     	printf("[%s][%d]  pst_output->i_b_open =%d \n", __FUNCTION__, __LINE__,pst_output->i_b_open);
        goto __ret;
    }
    
    __dvr_output_dma_rel(pst_output->i_devfd);

    close(pst_output->i_devfd);
    pst_output->i_devfd = -1;
    pst_output->i_b_open = 0;
    
__ret:    
   	printf("[%s][%d][end] *****VERSION=%s  i_ret =%d \n", __FUNCTION__, __LINE__, VERSION,i_ret);
    
    return i_ret; 
}

static int _dvr_dev_start()
{
	int i_type;
    int i_ret = 0;
    DVR_SHOW_INPUT_S *pst_input = &(g_st_dvr_info.st_input);
    DVR_SHOW_OUTPUT_S *pst_output = &(g_st_dvr_info.st_output);
   
  	printf("[%s][%d][in] **********VERSION=%s   \n", __FUNCTION__, __LINE__, VERSION);

    if ((pst_input->i_b_open == 0) || (pst_output->i_b_open == 0))
    {
    	printf("[%s][%d] ****input or output no open**** i_ret=%d \n", __FUNCTION__, __LINE__,i_ret);
        goto __ret;
    }

    if (g_st_dvr_info.i_dvrintask_do != 0)
    {
    	printf("[%s][%d]  g_st_dvr_info.i_dvrintask_do=%d \n", __FUNCTION__, __LINE__,g_st_dvr_info.i_dvrintask_do);
        goto __ret;
    }

    i_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	i_ret = ioctl(pst_input->i_devfd, VIDIOC_STREAMON, &i_type);
    
    i_type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	i_ret |= ioctl(pst_output->i_devfd, VIDIOC_STREAMON, &i_type);
    if (i_ret != 0)
    {
    	//printf("[%s][%d] streamon error \n", __FUNCTION__, __LINE__);

        i_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    	ioctl(pst_input->i_devfd, VIDIOC_STREAMOFF, &i_type);     
        i_type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    	ioctl(pst_input->i_devfd, VIDIOC_STREAMOFF, &i_type);     
        
        i_ret = -1;
    	printf("[%s][%d] streamon error i_ret=%d \n", __FUNCTION__, __LINE__,i_ret);
        goto __ret;
    }

    g_st_dvr_info.i_dvrintask_do = 1;
    g_st_dvr_info.i_dvrouttask_do = 1;
    
    pthread_create(&(g_st_dvr_info.pst_dvrintask), NULL, __dvr_in_do_thread, NULL);
    pthread_create(&(g_st_dvr_info.pst_dvrouttask), NULL, __dvr_out_do_thread, NULL);
    
__ret:    

   	printf("[%s][%d][end] *****VERSION=%s  i_ret =%d \n", __FUNCTION__, __LINE__, VERSION,i_ret);
    
    return i_ret; 
}

static int _dvr_dev_stop()
{
	int i_type;
    int i_ret = 0;

    DVR_SHOW_INPUT_S *pst_input = &(g_st_dvr_info.st_input);
    DVR_SHOW_OUTPUT_S *pst_output = &(g_st_dvr_info.st_output);

  	printf("[%s][%d][in] **********VERSION=%s   \n", __FUNCTION__, __LINE__, VERSION);

    if ((pst_input->i_b_open == 0) || (pst_output->i_b_open == 0))
    {
    	printf("[%s][%d] ****input or output no open**** i_ret=%d \n", __FUNCTION__, __LINE__,i_ret);
        goto __ret;
    }

    if (g_st_dvr_info.i_dvrintask_do == 0)
    {
   		printf("[%s][%d] *****VERSION=%s  g_st_dvr_info.i_dvrintask_do = %d  \n", __FUNCTION__, __LINE__, VERSION, g_st_dvr_info.i_dvrintask_do );
        goto __ret;
    }

    g_st_dvr_info.i_dvrouttask_do = 0;
    pthread_join(g_st_dvr_info.pst_dvrouttask, NULL);
    
    g_st_dvr_info.i_dvrintask_do = 0;
    pthread_join(g_st_dvr_info.pst_dvrintask, NULL);

    i_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(pst_input->i_devfd, VIDIOC_STREAMOFF, &i_type);     
    i_type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ioctl(pst_output->i_devfd, VIDIOC_STREAMOFF, &i_type);     

__ret:
   	printf("[%s][%d][end] *****VERSION=%s  i_ret =%d \n", __FUNCTION__, __LINE__, VERSION,i_ret);
     
    return 0;
}

static int __dvr_init(void)
{

    if ( g_st_dvr_info.i_binit != 0)
    {
  		printf("[%s][%d] *****VERSION=%s  g_st_dvr_info.i_binit = %d  \n", __FUNCTION__, __LINE__, VERSION,g_st_dvr_info.i_binit);
        return 0;
    }
  	printf("[%s][%d][in] **********VERSION=%s   \n", __FUNCTION__, __LINE__, VERSION);

    memset(&g_st_dvr_info, 0, sizeof(g_st_dvr_info));

    pthread_mutex_init(&g_st_dvr_info.st_mutex, NULL);  

    g_st_dvr_info.i_binit = 1;

    return 0;
}

static int __dvr_deinit(void)
{
    if (g_st_dvr_info.i_binit == 0)
    {
  		printf("[%s][%d] *****VERSION=%s  g_st_dvr_info.i_binit = %d  \n", __FUNCTION__, __LINE__, VERSION, g_st_dvr_info.i_binit);
        return 0;
    }
  	printf("[%s][%d][in] **********VERSION=%s   \n", __FUNCTION__, __LINE__, VERSION);

    DVR_SHOW_LOCK();

    _dvr_dev_stop();
    _dvr_output_close();
    _dvr_input_close();
    
    DVR_SHOW_UNLOCK();
    g_st_dvr_info.i_binit = 0;

   	printf("[%s][%d][end] *****VERSION=%s  \n", __FUNCTION__, __LINE__, VERSION);
      
    return 0;
}

static int __dvr_start(void)
{
    int i_ret;

    if (g_st_dvr_info.i_binit == 0)
    {
  		printf("[%s][%d] *****VERSION=%s  g_st_dvr_info.i_binit = %d  \n", __FUNCTION__, __LINE__, VERSION,g_st_dvr_info.i_binit);
        return 0;
    }

  	printf("[%s][%d][in] **********VERSION=%s   \n", __FUNCTION__, __LINE__, VERSION);

    DVR_SHOW_LOCK();

    g_st_dvr_info.i_btimeout = 0;

    i_ret = _dvr_input_open();
    if (i_ret != 0)
    {
 		printf("[%s][%d][in] *** VERSION=%s  dvr_input_open_ret=%d \n", __FUNCTION__, __LINE__, VERSION,i_ret);
        goto __ret;
    }
    
    i_ret = _dvr_output_open();
    if (i_ret != 0)
    {
 		printf("[%s][%d][in] *** VERSION=%s  dvr_output_open_ret=%d \n", __FUNCTION__, __LINE__, VERSION,i_ret);
        _dvr_input_close();
        goto __ret;
    }

    i_ret = _dvr_dev_start();
    if (i_ret != 0)
    {
 		printf("[%s][%d][in] *** VERSION=%s  dvr_dev_start_ret=%d \n", __FUNCTION__, __LINE__, VERSION,i_ret);
        _dvr_output_close();
        _dvr_input_close();
        goto __ret;
    }
   
__ret:    
    DVR_SHOW_UNLOCK();
   	printf("[%s][%d][end] *****VERSION=%s  i_ret =%d \n", __FUNCTION__, __LINE__, VERSION,i_ret);
    
    return i_ret;
}

static int __dvr_stop(void)
{
    int i_ret = 0;

    if (g_st_dvr_info.i_binit == 0)
    {
  		printf("[%s][%d] *****VERSION=%s  g_st_dvr_info.i_binit  = %d  \n", __FUNCTION__, __LINE__, VERSION,g_st_dvr_info.i_binit );
        return 0;
    }

  	printf("[%s][%d][in] **********VERSION=%s   \n", __FUNCTION__, __LINE__, VERSION);

    DVR_SHOW_LOCK();
    g_st_dvr_info.i_btimeout = 0;

    _dvr_dev_stop();
    _dvr_output_close();
    _dvr_input_close();
 
__ret:    
    DVR_SHOW_UNLOCK();
   	printf("[%s][%d][end] *****VERSION=%s  i_ret =%d \n", __FUNCTION__, __LINE__, VERSION, i_ret);
    
    return i_ret;
}

static int __dvr_status_get(int *p_bconnect)
{
    struct v4l2_input input;
    DVR_SHOW_INPUT_S *pst_input = &(g_st_dvr_info.st_input);
    int i_ret = 0;

    memset(&input, 0, sizeof(input));
    
    DVR_SHOW_LOCK();

    if (pst_input->i_b_open == 0)
    {
        *p_bconnect = 0;
        DVR_SHOW_UNLOCK(); 
        return -1;
    }

    if (g_st_dvr_info.i_btimeout == 1)
    {
        *p_bconnect = 1;
    }
    else
    {
        input.index = 1;
        if (ioctl(pst_input->i_devfd, VIDIOC_ENUMINPUT, &input) < 0)
        {
            printf("[%s][%d] get dvd status is error \n", __FUNCTION__, __LINE__);
            i_ret = -1;
        }
        else
        {
            i_ret = 0;
            if (input.status == V4L2_IN_ST_NO_SIGNAL)
            {
                *p_bconnect = 0;
            }
            else
            {
                *p_bconnect = 1;
            }
        }
    }

    DVR_SHOW_UNLOCK(); 
    return i_ret;
}

int createDvrDisplayHal(T_HalDvrDisplayDevice** device)
{
	T_HalDvrDisplayDevice *dev;
	dev = (T_HalDvrDisplayDevice *)malloc(sizeof(T_HalDvrDisplayDevice));	
    if (dev == NULL)
	{
		return -1;
	}
	
	strcpy(dev->common.version, "1.4");
	strcpy(dev->common.name, "dvr_display for dvr");

	dev->dvr_init		    = __dvr_init;
	dev->dvr_start   		= __dvr_start ;
	dev->dvr_stop  			= __dvr_stop ;
	dev->dvr_deinit   		= __dvr_deinit ;
	dev->dvr_status_get   	= __dvr_status_get ;

	*device = dev;

	return 0;
}

int destoryDvrDisplayHal(T_HalDvrDisplayDevice** device)
{
	if (*device)
	{		
		free(*device);
		*device = NULL;
	}

	return 0;
}


