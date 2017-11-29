#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

int fd;

int g_running;

typedef struct 
{
	int is_aux_used;			//data for  app 
	
}AUX_STATUS;

static AUX_STATUS aux_status;

void sync_handler(int num)
{
   sleep(1);
   read(fd,&aux_status,sizeof(AUX_STATUS));
   printf("app--recv sync single------out :[0]    in:[1] aux_status.is_aux_used = %d \n",aux_status.is_aux_used);
}
 

void stop_handler(int signum)
{
    close(fd);
    g_running = 0;
    printf("stop prom\n");
    //return (0);
}


main()
{
 int oflags;

 g_running = 1;

 signal(SIGINT, stop_handler);

 fd = open ("/dev/aux_det_dev", O_RDWR);//,S_IRUSR | S_IWUSR);

 if(fd > 0)
 { 
    printf("open /dev/aux_det_dev ok\n");
 } 
 else
 {
    printf("open /dev/aux_det_dev fail\n");
 }

 signal(SIGIO, sync_handler);

 fcntl(fd, F_SETOWN, getpid());

 oflags = fcntl(fd, F_GETFL);

 fcntl(fd, F_SETFL, oflags | FASYNC);

 while (g_running)
 {
      sleep(100);  
 }
}
















