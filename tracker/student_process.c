#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>  


#include "rh_interface.h"
#include "rh_encoder_send.h"
#include "auto_track.h"
#include "student_process.h"
#include "track_students.h"


extern StuITRACK_Params  g_track_param;
extern track_class_info_t g_class_info;

#define VI_WIDTH 3840
#define VI_HEIGHT 2160
#define CUT_WIDTH 1920
#define CUT_HEIGHT 1080


int Reach_Track_start()
{
	HI_S32 s32ChnNum = 1;  /*1 channel*/
	HI_U32 s32Ret = HI_SUCCESS;

	int result;
	pthread_t  id_listen,init_thread;
    StuITRACK_Params Params;
	int video_width, video_height, track_num_row, track_point_num;
	
	printf("start studentTrace\n");
	
	result = pthread_create(&id_listen, NULL, (void *)studentTrace, (void *)NULL);
	if(result < 0) {
		printf( "create studentTrace() failed\n");
		return -1;
	}
	
	printf("student Params init\n");

	student_init_params();//读取配置文件参数
	
	memcpy(&Params,&g_track_param,sizeof(StuITRACK_Params));
	memset(&outArgs, 0, sizeof(StuITRACK_OutArgs));
	
	printf("start student init\n");
	
	input = (uint8_t*)malloc(sizeof(uint8_t)* 640*360);
	if (TRACK_STUDENTS_initObj(&handles, &Params) == -1) 
		{
		printf("student init error\n");
		return -1;
		}
	
	return 1;
}


int Reach_Track_run(HI_U8* pVirAddr,rect_t* rect)
{
	int64_t t1,t2;
	int8_t* input_data;
	int Active = 0;
	
	static int bFlag = 0;
	int memLen = sizeof(uint8_t)*handles.input_param.static_param.video_width*handles.input_param.static_param.video_height;
	memset(input,0,memLen);
	//printf("size = %d\n",memLen);
	memcpy(input,pVirAddr,memLen);
	input_data = input;
	t1 = getCurrentTime();
	Track_Students_process(&handles, input_data, &outArgs);

	if(outArgs.up_num > 0)
	{
		outArgs.Up_List[0].x = outArgs.Up_List[0].x*3;
		outArgs.Up_List[0].y = outArgs.Up_List[0].y*3;
		if(outArgs.Up_List[0].x - CUT_WIDTH/2 <0) 
			rect->min_x = 0;
		else if(outArgs.Up_List[0].x - CUT_WIDTH/2 > VI_WIDTH)
			rect->min_x = VI_WIDTH - CUT_WIDTH;
		else 
			rect->min_x = outArgs.Up_List[0].x - CUT_WIDTH/2;

		if(outArgs.Up_List[0].y - CUT_HEIGHT/2 <0) 
			rect->min_y = 0;
		else if(outArgs.Up_List[0].y - CUT_HEIGHT/2 > VI_HEIGHT)
			rect->min_y = VI_HEIGHT - CUT_HEIGHT;
		else 
			rect->min_y = outArgs.Up_List[0].y - CUT_HEIGHT/2;
		
		
		rect->max_x = rect->min_x + CUT_WIDTH;
		rect->max_y = rect->min_y + CUT_WIDTH;
		Active = 2;
	}
		
	t2 = getCurrentTime();
	//if (t2 - t1>=150 || active == 1)
	//	printf("process used time = %lld ms\n",t2 - t1);
	Active = 1;
	return Active;
}


void Reach_Track_stop()
{
	g_track_param.dynamic_param.control_mode = 1;
}


