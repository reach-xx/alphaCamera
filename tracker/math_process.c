#include "math_process.h"

extern CeilTrackHand g_motion_detect_input_param;
extern ITRACK_STUDENTS_RIGHT_SIDE_Params 	g_track_param;
extern track_strategy_info_t		g_track_strategy_info;
extern int g_recontrol_flag;

int frame_num = 0;
long long last_time = 0;
long long cur_time = 0;
int frame_vate = 0;


static unsigned long getCurrentTime2(void)
{
	struct timeval tv;
	struct timezone tz;
	unsigned long ultime;

	gettimeofday(&tv , &tz);
	ultime = tv.tv_sec * 1000 + ((tv.tv_usec) / 1000);
	return (ultime);
}

HI_VOID Draw_Analysis_Rect(CeilITRACK_OutArgs *output_param, int iWidth, int iHeight)
{
	int iIndex = 0;
	RH_POLY_ARRAY_S stPoint;
	HI_U8 i = 0;

	for(iIndex = 0; iIndex < output_param->cmd_type; iIndex++)
	{
		/*SAMPLE_PRT("Draw_Analysis_Rect---> iIndex:%d, (%d, %d, %d, %d)\n", 
			iIndex, output_param->trigger_point[iIndex].x0, output_param->trigger_point[iIndex].y0, output_param->trigger_point[iIndex].x1, output_param->trigger_point[iIndex].y1 );
		*/
		output_param->trigger_point[iIndex].x0 = output_param->trigger_point[iIndex].x0 < 0 ? 0 : output_param->trigger_point[iIndex].x0;
		output_param->trigger_point[iIndex].y0 = output_param->trigger_point[iIndex].y0 < 0 ? 0 : output_param->trigger_point[iIndex].y0;
		output_param->trigger_point[iIndex].x1 = output_param->trigger_point[iIndex].x1 < 0 ? 0 : output_param->trigger_point[iIndex].x1;
		output_param->trigger_point[iIndex].y1 = output_param->trigger_point[iIndex].y1 < 0 ? 0 : output_param->trigger_point[iIndex].y1;

		output_param->trigger_point[iIndex].x0 = output_param->trigger_point[iIndex].x0 >= iWidth ? iWidth - 1 : output_param->trigger_point[iIndex].x0;
		output_param->trigger_point[iIndex].y0 = output_param->trigger_point[iIndex].y0 >= iHeight ? iHeight - 1 : output_param->trigger_point[iIndex].y0;
		output_param->trigger_point[iIndex].x1 = output_param->trigger_point[iIndex].x1 >= iWidth ? iWidth - 1 : output_param->trigger_point[iIndex].x1;
		output_param->trigger_point[iIndex].y1 = output_param->trigger_point[iIndex].y1 >= iHeight ? iHeight - 1 : output_param->trigger_point[iIndex].y1;

		output_param->trigger_point[iIndex].x0 = output_param->trigger_point[iIndex].x0 % 2 == 1 ? output_param->trigger_point[iIndex].x0 - 1 : output_param->trigger_point[iIndex].x0;
		output_param->trigger_point[iIndex].y0= output_param->trigger_point[iIndex].y0 % 2 == 1 ? output_param->trigger_point[iIndex].y0 - 1 : output_param->trigger_point[iIndex].y0;
		output_param->trigger_point[iIndex].x1= output_param->trigger_point[iIndex].x1 % 2 == 1 ? output_param->trigger_point[iIndex].x1 - 1 : output_param->trigger_point[iIndex].x1;
		output_param->trigger_point[iIndex].y1= output_param->trigger_point[iIndex].y1 % 2 == 1 ? output_param->trigger_point[iIndex].y1 - 1 : output_param->trigger_point[iIndex].y1;
	}

	stPoint.u16Num = 4 * output_param->cmd_type;
	for(i = 0 ; i < stPoint.u16Num;i++) {
		stPoint.stPolygon[i].u32Color = 0x0000FF;
		stPoint.stPolygon[i].u32Thick = 2;
	}
	
	iIndex = 0;
	for(iIndex = 0; iIndex < output_param->cmd_type; iIndex++)
	{
		stPoint.stPolygon[0 + iIndex * 4].stStartPoint.s32X = output_param->trigger_point[iIndex].x0;
		stPoint.stPolygon[0 + iIndex * 4].stStartPoint.s32Y = output_param->trigger_point[iIndex].y0;
		stPoint.stPolygon[0 + iIndex * 4].stEndPoint.s32X   = output_param->trigger_point[iIndex].x1;
		stPoint.stPolygon[0 + iIndex * 4].stEndPoint.s32Y   = output_param->trigger_point[iIndex].y0;

		stPoint.stPolygon[1 + iIndex * 4].stStartPoint.s32X = output_param->trigger_point[iIndex].x1;
		stPoint.stPolygon[1 + iIndex * 4].stStartPoint.s32Y = output_param->trigger_point[iIndex].y0;
		stPoint.stPolygon[1 + iIndex * 4].stEndPoint.s32X   = output_param->trigger_point[iIndex].x1;
		stPoint.stPolygon[1 + iIndex * 4].stEndPoint.s32Y   = output_param->trigger_point[iIndex].y1;	
		
		stPoint.stPolygon[2 + iIndex * 4].stStartPoint.s32X = output_param->trigger_point[iIndex].x1;
		stPoint.stPolygon[2 + iIndex * 4].stStartPoint.s32Y = output_param->trigger_point[iIndex].y1;	
		stPoint.stPolygon[2 + iIndex * 4].stEndPoint.s32X   = output_param->trigger_point[iIndex].x0;
		stPoint.stPolygon[2 + iIndex * 4].stEndPoint.s32Y   = output_param->trigger_point[iIndex].y1;	
		
		stPoint.stPolygon[3 + iIndex * 4].stStartPoint.s32X = output_param->trigger_point[iIndex].x0;
		stPoint.stPolygon[3 + iIndex * 4].stStartPoint.s32Y = output_param->trigger_point[iIndex].y1;
		stPoint.stPolygon[3 + iIndex * 4].stEndPoint.s32X   = output_param->trigger_point[iIndex].x0;
		stPoint.stPolygon[3 + iIndex * 4].stEndPoint.s32Y   = output_param->trigger_point[iIndex].y0;

/*
		SAMPLE_PRT("Draw_Analysis_Rect---> iIndex:%d, (%d, %d)-(%d, %d), (%d, %d)-(%d, %d), (%d, %d)-(%d, %d), (%d, %d)-(%d, %d)\n", 
			iIndex, 
			stPoint.stPolygon[0 + iIndex * 4].stStartPoint.s32X, stPoint.stPolygon[0 + iIndex * 4].stStartPoint.s32Y, 
			stPoint.stPolygon[0 + iIndex * 4].stEndPoint.s32X, stPoint.stPolygon[0 + iIndex * 4].stEndPoint.s32Y,
			stPoint.stPolygon[1 + iIndex * 4].stStartPoint.s32X, stPoint.stPolygon[1 + iIndex * 4].stStartPoint.s32Y, 
			stPoint.stPolygon[1 + iIndex * 4].stEndPoint.s32X, stPoint.stPolygon[1 + iIndex * 4].stEndPoint.s32Y,
			stPoint.stPolygon[2 + iIndex * 4].stStartPoint.s32X, stPoint.stPolygon[2 + iIndex * 4].stStartPoint.s32Y, 
			stPoint.stPolygon[2 + iIndex * 4].stEndPoint.s32X, stPoint.stPolygon[2 + iIndex * 4].stEndPoint.s32Y,
			stPoint.stPolygon[3 + iIndex * 4].stStartPoint.s32X, stPoint.stPolygon[3 + iIndex * 4].stStartPoint.s32Y, 
			stPoint.stPolygon[3 + iIndex * 4].stEndPoint.s32X, stPoint.stPolygon[3 + iIndex * 4].stEndPoint.s32Y);
*/
	}
	RH_MPI_DrawLine(&stPoint);

}

int Reach_Track_start()
{
	//初始化配置文件的参数至内存中
	track_init(&g_track_param);

	//老师机检测算法初始化
	motion_detect_init(&g_motion_detect_input_param,TRACK_WIDTH,TRACK_HEIGHT,g_track_param.dynamic_param.sens);

	//开启切换策略模块
	strategy_reset();
	strategy_start();

	//开启广播监听服务，处理查询IP的广播消息
	StartBroadcastServer(AUTHOR_TYPE_TEACHER, g_track_strategy_info.local_ip_info.ip);
}

int Reach_Track_run(HI_U8* pVirAddr ,rect_t* target_rect)
{	
	frame_num++;
	CeilITRACK_OutArgs output_param;
	int Active = 0, Active1 = 0;
	cur_time = getCurrentTime();
	//printf("teacher detect frame cur_time = %lld\n",cur_time);
	if((cur_time - last_time) > 5000)
	{
		frame_vate = frame_num /((cur_time - last_time) / 1000);
		printf("teacher detect frame vate = %d, frame_num = %d\n",frame_vate, frame_num);
		last_time = cur_time;
		frame_num = 0;
	}

	//SAMPLE_PRT("RH_MPI_Alg_Analysis--->g_track_param.dynamic_param.control_mode:%d\n", g_track_param.dynamic_param.control_mode);
	if (g_track_param.dynamic_param.control_mode != 1)
	{
		  if(g_recontrol_flag)
		  {
		   g_recontrol_flag = 0;
		   motion_detect_release(&g_motion_detect_input_param);
		   motion_detect_init(&g_motion_detect_input_param, TRACK_WIDTH, TRACK_HEIGHT, g_track_param.dynamic_param.sens);
		  }
		  
		Active1= motion_detect_process(&g_motion_detect_input_param,pVirAddr,&output_param);
		static int iFlag = 0;
		iFlag++;
		if(iFlag % 20 == 0)
		{
			SAMPLE_PRT("0000000output_param.cmd_type:%d\n", output_param.cmd_type);
		}
		
		Active = teacher_target_validaation(output_param.cmd_type,output_param.trigger[0].x,output_param.IsTrigger, target_rect);
		
		if(iFlag % 20 == 0)
		{
			SAMPLE_PRT("1111111output_param.cmd_type:%d\n", output_param.cmd_type);
		}

		Draw_Analysis_Rect(&output_param, TRACK_WIDTH, TRACK_HEIGHT);
	}
	//SAMPLE_PRT("Reach_Track_run Active:%d\n", Active);
	return Active;
}

void Reach_Track_stop()
{
	return ;
}





