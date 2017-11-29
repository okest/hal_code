#include "hal_av_in.h"
#include "hal.h"
#include <stdio.h>
#include <time.h>

int main()
{
	T_HalAvinDevice   * pDvr = NULL;
	createAvinHal(&pDvr);
	if (pDvr == NULL)
	{
		perror("create dvr  failed");
		return -1;
	}
	pDvr->av_in_init();
	pDvr->av_in_start();
	printf("my test start - > \n");
	while(1)
	{
	}
	pDvr->av_in_stop();
	printf("my test stop - > \n");
	
	//pDvr->dvr_deinit();
	destoryAvinHal(&pDvr);
	return 0;
	
}
