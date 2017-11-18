/*
****************************************************
Copyright (C), 1999-1999, Reach Tech. Co., Ltd.

File name:     	auto_track.c

Description:    学生跟踪

Date:     		2012-12-11

Author:	  		sunxl

version:  		V1.0

*****************************************************
*/

#include "auto_track.h"
#include "common.h"
#include "netcom.h"
#include "studentTcp.h"
#include "rh_broadcast.h"
#ifdef SUPPORT_TRACK
int gRemoteFD;

/**
* @ 帧率,主要用做时间换算上
*/
#define FRAME_NUM		(20)

#define SECOND_NUM		(1000)

/**
* @	tcp命令长度
*/
#define TCP_MSG_LEN		(256)

/**
* @	设备名称字符串长度的宏	
*/
#define DEV_NAME_LEN	(32)

/**
* @	发送控制摄像头的命令的长度
*/
#define CTRL_CMD_LEN	(16)

angle_info_t	g_angle_info = {0.0};


//===========================教师静态参数相关的宏定义=====================

/**
* @ 教师跟踪的静态参数类型名称
*/
#define STATIC_NAME				"static"

/**
* @ 静态参数的名称
*/
#define VIDEO_WIDTH				"video_width"
#define	VIDEO_HEIGHT			"video_height"
#define PIC_WIDTH				"pic_width"
#define PIC_HEIGHT				"pic_height"

//===========================教师动态参数相关的宏定义=====================

/**
* @ 动态参数的名称
*/
#define ZOOM_DISTACE			"zoom_distance"
#define X_OFFSET 				"x_offset"
#define Y_OFFSET				"y_offset"
#define TRIGGER_NUM 			"trigger_num"
#define TRIGGER_X				"trigger_x"
#define TRIGGER_Y				"trigger_y"
#define TRIGGER_WIDTH			"trigger_width"
#define TRIGGER_HEIGHT			"trigger_height"
#define TRIGGER_POSITION		"trigger_position"
#define SHIELD_X				"shield_x"
#define SHIELD_Y				"shield_y"
#define SHIELD_WIDTH			"shield_width"
#define SHIELD_HEIGHT			"shield_height"
#define MULTITARGET_X				"multitarget_x"
#define MULTITARGET_Y				"multitarget_y"
#define MULTITARGET_WIDTH			"multitarget_width"
#define MULTITARGET_HEIGHT			"multitarget_height"
#define CONTROL_MODE			"control_mode"
#define SENS					"sens"
#define ROW_NUM					"row_num"
#define	MESSAGE					"message"
#define	MODEL_SUM				"model_sum"
#define TRACK_SEL				"track_sel"
#define MID_X					"mid_x"
#define	MID_Y					"mid_y"
#define	LIMIT_HEIGHT			"limit_height"
#define MODEL_MULTIPLE			"model_multiple"
#define	MODEL_LEVEL				"model_level"
#define TRACK_POINT_NUM			"track_point_num"
#define TRACK_POINTX			"track_pointx"
#define TRACK_POINTY			"track_pointy"
#define RESET_TIME				"reset_time"
#define CAM_ADDR                 "cam_addr"
#define CAM_PARAM				"CAM_PARAM"
#define CAM_MASK				"cam_mask"
#define CAM_ROUTE				"cam_gw"
//===========================教师摄像头控制的相关参数宏定义设置=====================
/**
* @ 教师摄像头类型
*/
#define CAM_TYPE			"cam_type"

/**
* @ 教师摄像头控制参数类型名称
*/
#define TEACH_CAM_NAME			"cam_control"

/**
* @	教师摄像头控制参数
*/
#define TEACH_CAM_SPEED			"cam_speed"

/**
* @ 教师摄像头限位控制类型名称
*/
#define TEACH_CAM_LIMIT_POSITION	"limit_position"

/**
* @ 教师摄像头左下限位名称
*/
#define LIMIT_LEFT_DOWN_NAME	"left_down"

/**
* @ 教师摄像头右上限位名称
*/
#define LIMIT_RIGHT_UP_NAME		"right_up"


/**
* @ 学生摄像头预置位控制类型名称
*/
#define CAM_PRESET_POSITION	"preset_position"

/**
* @ 教师摄像头预置位位置的上下左右值
*/
#define PRESET_POSITION_VALUE		"position_value"

/**
* @ 教师摄像头预置位焦距位置值
*/
#define PRESET_ZOOM_VALUE			"zoom_value"
/**
* @ 教师摄像头预置位推送时间
*/
#define PRESET_POSITION_TIME			"position_time"


/**
* @ 跟踪机对应的编码参数
*/
#define STUDENTS_TRACK_ENCODE			"students_track_encode"

/**
* @	是否编码标志
*/
#define STUDENTS_IS_ENCODE				"students_is_encode"

#define TRACK_CAM_X_OFFSET           "students_cam_x_offset"
#define TRACK_CAM_Y_OFFSET          "students_cam_y_offset"
#define TRACK_CAM_OFFSET_ROW           "track_num_row"


/**
* @	学生摄像机控制方式
*/
#define CAM_CONTROL_TYPE				"cam_control_type"


/**
* @	设置学生跟踪机是否推近景的参数名称
*/
#define IS_CONTROL_CAM					"is_control_cam"

/**
* @	设置老师机调用预制位时两条命令的间隔时间
*/
#define CAM_ZOOM_PAN_DELAY					"cam_zoom_pan_delay"


/**
* @	表明是否需要重设动态参数标志 
*/
int g_recontrol_flag = 0;

/**
* @ 控制摄像头需要的参数
*/
cam_control_info_t	g_cam_info = {0};


/**
* @	和跟踪有关的编码的一些全局参数
*/
track_encode_info_t	g_track_encode_info = {0};
/**
* @	和课堂信息统计相关的一些全局参数
*/
track_class_info_t g_class_info={0};

/**
* @	和写跟踪配置文件相关
*/
track_save_file_info_t	g_track_save_file = {0};

/**
* @	摄像机型号全局变量 
*/
track_cam_model_info_t	g_track_cam_model = {0};

/**
* @	学生辅助机上报的消息
*/
rightside_trigger_info_t	g_rightside_trigger_info = {0};

/**
* @	查询云台预置位请求
*/

cam_preset_teacher_and_student_req g_preset_student_req = {0};

/**
* @	教师跟踪的初始化时传入的参数
*/
StuITRACK_Params 	g_track_param		= {0};

/**
* @	查询云台预置位
*/
call_cam_preset_teacher_and_student_req c_preset_student_req = {0};



//#define		ENC_1200_DEBUG

//初始化写跟踪参数配置文件的信号量
int init_save_track_mutex(void)
{
	pthread_mutex_init(&g_track_save_file.save_track_m, NULL);
	
	return 0;
}

int lock_save_track_mutex(void)
{
	pthread_mutex_lock(&g_track_save_file.save_track_m);
	return 0;
}

int unlock_save_track_mutex(void)
{
	pthread_mutex_unlock(&g_track_save_file.save_track_m);
	return 0;
}

int destroy_save_track_mutex(void)
{
	pthread_mutex_destroy(&g_track_save_file.save_track_m);
	return 0;
}


static int reboot_device()
{
 DEBUG(DL_DEBUG,"reboot device.......................\n");
 sleep(2);
 system("reboot -f");
 return 0;
}




/**
* @ 函数名称: write_track_static_file()
* @ 函数功能: 写跟踪的静态参数到文件中
* @ 输入参数: static_param -- 算法的静态参数
* @ 输出参数: 无
* @ 返回值:   无
*/
int write_track_static_file(stutrack_static_param_t *static_param)
{
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;

	lock_save_track_mutex();
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);

	//写静态参数视频宽
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", static_param->video_width);
	ret =  ConfigSetKey(config_file, STATIC_NAME, VIDEO_WIDTH, temp);

	//写静态参数视频高
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", static_param->video_height);
	ret =  ConfigSetKey(config_file, STATIC_NAME, VIDEO_HEIGHT, temp);

	//写静态参数缩放后的视频宽
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", static_param->pic_width);
	ret =  ConfigSetKey(config_file, STATIC_NAME, PIC_WIDTH, temp);

	//写静态参数缩放后的视频高
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", static_param->pic_height);
	ret =  ConfigSetKey(config_file, STATIC_NAME, PIC_HEIGHT, temp);
	unlock_save_track_mutex();
	return 0;
}



/**
* @ 函数名称: set_cam_addr()
* @ 函数功能: 设置摄像机ip
*/
int set_cam_addr(unsigned char *data)
{
	int size = 0;
	int fd = gRemoteFD; 
	int i,j;
	FILE *fp = NULL;
	FILE *fw = NULL;
	track_ip_info_t* cam_info = (track_ip_info_t*)data;
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	char	text[FILE_NAME_LEN] 		= {0};
	size = 4;
	struct in_addr addr1, addr2,addr3;
	int id_num = 0;
	int netmask = 0;
	int gw = 0;
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);
	
	memset(temp, 0, FILE_NAME_LEN);
	memset(text, 0, FILE_NAME_LEN);
	strcat(temp,"ifconfig eth0 ");
	id_num = inet_addr(cam_info->ip); 
	netmask = inet_addr(cam_info->mask);
	gw = inet_addr(cam_info->route);
	
	memcpy(&addr1,&id_num,4);
	printf("id_num=%d\n",id_num);
	strcat(temp,inet_ntoa(addr1));
	memcpy(&addr2,&netmask,4);
	strcat(temp," netmask ");
	strcat(temp,inet_ntoa(addr2));
	printf("out = %s\n",temp);
	if(CheckIPNetmask(id_num,netmask,gw) == 0)
	{
		DEBUG(DL_ERROR, "ERROR ID %s\n", inet_ntoa(addr1));
	}

//执行更换ip命令
	ret = system(temp); 
	if (ret <0)
	{
		DEBUG(DL_ERROR, "ifconfig %s up ERROR\n", inet_ntoa(addr1));
	}
	
	//write to file 
	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(temp,inet_ntoa(addr1));
	ret = ConfigSetKey(config_file, CAM_PARAM,CAM_ADDR , temp);
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(temp,inet_ntoa(addr2));
	ret = ConfigSetKey(config_file, CAM_PARAM,CAM_MASK , temp);
	unlock_save_track_mutex();
	
	memset(temp, 0, FILE_NAME_LEN);
	memcpy(&addr3,&gw,4);
	strcat(temp,"route add default gw ");
	strcat(temp,inet_ntoa(addr3));
	printf("out = %s\n",temp);

	ret = system(temp); 
	if (ret <0)
	{
		DEBUG(DL_ERROR, "route add default gw %s up ERROR\n", inet_ntoa(addr3));
	}
	
	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(temp,inet_ntoa(addr3));
	ret = ConfigSetKey(config_file, CAM_PARAM,CAM_ROUTE , temp);
	unlock_save_track_mutex();
	ResetIpInfo(inet_ntoa(addr1));
	return size;
}

/**
* @ 函数名称: jump_zoom_position()
* @ 函数功能: 控制摄像头转到预置位
* @ 输入参数: cam_position -- 预置位编号
* @ 输出参数: data -- 向摄像头发送的整个命令包
* @ 返回值:   发送的数据的长度
*/
static int jump_zoom_position(unsigned char *data, int cam_position)
{
	int size = 0;

	int 			fd 						= gRemoteFD; 

	data[0] = g_cam_info.cam_addr;
	data[1] = 0x01;
   	data[2] = 0x04;
   	data[3] = 0x47;

	data[4] = g_cam_info.cam_position[cam_position].zoom[0];	//zoom的位置信息
	data[5] = g_cam_info.cam_position[cam_position].zoom[1];	//zoom的位置信息
	data[6] = g_cam_info.cam_position[cam_position].zoom[2];	//zoom的位置信息
	data[7] = g_cam_info.cam_position[cam_position].zoom[3];	//zoom的位置信息
	
	data[8] = 0xff;

	size = 9;
	return size;
}

/**
* @ 函数名称: jump_absolute_position()
* @ 函数功能: 控制摄像头转到预置位
* @ 输入参数: cam_position -- 预置位编号
* @ 输出参数: data -- 向摄像头发送的整个命令包
* @ 返回值:   发送的数据的长度
*/
static int jump_absolute_position(unsigned char *data, int cam_position)
{
	int size = 0;

	int 			fd						= gRemoteFD; 

	data[0] = g_cam_info.cam_addr;
	data[1] = 0x01;
	data[2] = 0x06;
	data[3] = 0x02;
	
	if(SONY_BRC_Z330 == g_track_cam_model.cam_type)
	{
		data[4] = 0x18; 	//水平方向速度

		data[5] = 0x00; 	//垂直方向速度

		data[6] = g_cam_info.cam_position[cam_position].pan_tilt[0];	//水平方向位置信息
		data[7] = g_cam_info.cam_position[cam_position].pan_tilt[1];	//水平方向位置信息
		data[8] = g_cam_info.cam_position[cam_position].pan_tilt[2];	//水平方向位置信息
		data[9] = g_cam_info.cam_position[cam_position].pan_tilt[3];	//水平方向位置信息
		data[10] = g_cam_info.cam_position[cam_position].pan_tilt[4];	//水平方向位置信息
		
		data[11] = g_cam_info.cam_position[cam_position].pan_tilt[5];	//垂直方向位置信息
		data[12] = g_cam_info.cam_position[cam_position].pan_tilt[6];	//垂直方向位置信息
		data[13] = g_cam_info.cam_position[cam_position].pan_tilt[7];	//垂直方向位置信息
		data[14] = g_cam_info.cam_position[cam_position].pan_tilt[8];	//垂直方向位置信息
		

		data[15] = 0xff;

		size = 16;
		DEBUG(DL_WARNING, "jump_absolute_position 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n",data[6],data[7],data[8],data[9],data[10],data[11],data[12],data[13],data[14],data[15]);
	}
	else
	{
		data[4] = 0x18; 	//水平方向速度
		data[5] = 0x14; 	//垂直方向速度
		data[6] = g_cam_info.cam_position[cam_position].pan_tilt[0];	//水平方向位置信息
		data[7] = g_cam_info.cam_position[cam_position].pan_tilt[1];	//水平方向位置信息
		data[8] = g_cam_info.cam_position[cam_position].pan_tilt[2];	//水平方向位置信息
		data[9] = g_cam_info.cam_position[cam_position].pan_tilt[3];	//水平方向位置信息
		data[10] = g_cam_info.cam_position[cam_position].pan_tilt[4];	//垂直方向位置信息
		data[11] = g_cam_info.cam_position[cam_position].pan_tilt[5];	//垂直方向位置信息
		data[12] = g_cam_info.cam_position[cam_position].pan_tilt[6];	//垂直方向位置信息
		data[13] = g_cam_info.cam_position[cam_position].pan_tilt[7];	//垂直方向位置信息
		
		data[14] = 0xff;

		size = 15;
	}
	return size;
}


/**
* @	函数名称: set_track_range()
* @	函数功能: 设置跟踪区域参数
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int set_track_range(unsigned char *data, StuITRACK_DynamicParams *dynamic_param)
{
	track_range_info_t *track_info = NULL;
	track_range_info_t real_track_info[TRACK_AREA_POINT_MAX];
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	char	text[FILE_NAME_LEN]			= {0};
	int 	ret 	= -1;

	int		index 	= 0;
	
	unsigned short maxy,miny;
	
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);

	track_info = (track_range_info_t *)data;

	if(track_info->point_num > TRACK_AREA_POINT_MAX)
	{
		track_info->point_num = TRACK_AREA_POINT_MAX;
	}
	
	dynamic_param->track_point_num = track_info->point_num;
	dynamic_param->reset_level = RE_START;
#ifdef ENC_1200_DEBUG
	PRINTF("track_info->point_num = %d\n", track_info->point_num);
#else
	DEBUG(DL_WARNING, "track_info->point_num = %d\n",track_info->point_num);
#endif

	lock_save_track_mutex();
	//存放在配置文件中
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->track_point_num);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRACK_POINT_NUM, temp);

	//对读取的点进行处理分配
	//处理顺序不对的点
	if(track_info->point[0].x > track_info->point[1].x || track_info->point[0].y < track_info->point[3].y ||
		track_info->point[0].y < track_info->point[2].y || track_info->point[1].y < track_info->point[3].y ||
		track_info->point[1].y < track_info->point[2].y || track_info->point[3].x > track_info->point[2].x) 
		DEBUG(DL_WARNING, "Error Point Order\n");
	//保持水平平行
	maxy = track_info->point[0].y > track_info->point[1].y ? track_info->point[0].y :track_info->point[1].y;
	miny = track_info->point[3].y < track_info->point[2].y ? track_info->point[3].y : track_info->point[2].y;
	
	for(index = 0; index < track_info->point_num; index++)
	{
		dynamic_param->track_point[index].x = track_info->point[index].x;
		if (index < track_info->point_num/2)
			dynamic_param->track_point[index].y = maxy;
		else
			dynamic_param->track_point[index].y = miny;
		
			//dynamic_param->track_point[index].y = track_info->point[index].y;
#ifdef ENC_1200_DEBUG
		PRINTF("track_info->point[index].x = %d,index = %d\n", track_info->point[index].x, index);
		PRINTF("track_info->point[index].y = %d,index = %d\n", track_info->point[index].y, index);
#else		
		DEBUG(DL_WARNING, "track_info->point[index].x = %d,index = %d\n",dynamic_param->track_point[index].x, index);
		DEBUG(DL_WARNING, "track_info->point[index].y = %d,index = %d\n",dynamic_param->track_point[index].y, index);
#endif		
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_POINTX);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", track_info->point[index].x);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);

		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_POINTY);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", track_info->point[index].y);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
	}
	unlock_save_track_mutex();
	return 0;
}

/**
* @	函数名称: set_trigger_range()
* @	函数功能: 设置触发区域参数
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int set_trigger_range(unsigned char *data, StuITRACK_DynamicParams *dynamic_param)
{
	trigger_range_info_t *trigger_info = NULL;
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	char	text[FILE_NAME_LEN]			= {0};
	int 	ret 						= -1;
	int		index						= 0;
	
	
	trigger_info = (trigger_range_info_t *)data;
	
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);

	dynamic_param->reset_level = RE_INIT;

	lock_save_track_mutex();

#if 0
	dynamic_param->trigger_num = trigger_info->point_num;
	sprintf(temp, "%d", dynamic_param->trigger_num);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER_NUM, temp);
#endif	

	for(index = 0; index < STUDENTS_TRIGGER_NUM; index++)
	{
		dynamic_param->trigger_info[index].x = trigger_info->rectangle[index].x;
		dynamic_param->trigger_info[index].y = trigger_info->rectangle[index].y;
		dynamic_param->trigger_info[index].width = trigger_info->rectangle[index].width;
		dynamic_param->trigger_info[index].height = trigger_info->rectangle[index].height;

#ifdef ENC_1200_DEBUG
	PRINTF("trigger_info->point[0].x = %d\n",trigger_info->rectangle[0].x);
	PRINTF("trigger_info->point[0].y = %d\n",trigger_info->rectangle[0].y);
	PRINTF("trigger_info->point[1].x = %d\n",trigger_info->rectangle[1].x);
	PRINTF("trigger_info->point[1].y = %d\n",trigger_info->rectangle[1].y);
#else	
	DEBUG(DL_WARNING, "trigger_info->point[%d].x = %d\n",index,trigger_info->rectangle[index].x);
	DEBUG(DL_WARNING, "trigger_info->point[%d].y = %d\n",index,trigger_info->rectangle[index].y);
	DEBUG(DL_WARNING, "trigger_info->point[%d].width = %d\n",index,trigger_info->rectangle[index].width);
	DEBUG(DL_WARNING, "trigger_info->point[%d].height = %d\n",index,trigger_info->rectangle[index].height);
#endif	
		//存放在配置文件中
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRIGGER_X);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", dynamic_param->trigger_info[index].x);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
		usleep(5000);
		
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRIGGER_Y);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", dynamic_param->trigger_info[index].y);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
		usleep(5000);

		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRIGGER_WIDTH);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", dynamic_param->trigger_info[index].width);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
		usleep(5000);

		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRIGGER_HEIGHT);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", dynamic_param->trigger_info[index].height);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
		usleep(5000);
	}
	unlock_save_track_mutex();
	return 0;
}



/**
* @	函数名称: set_track_type()
* @	函数功能: 设置跟踪类型,0为自动跟踪,1为手动跟踪,并保存到配置文件中
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int set_track_type(unsigned char *data, StuITRACK_DynamicParams *dynamic_param)
{
	control_type_info_t *track_info = NULL;

	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	track_info = (control_type_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);
	if(g_track_encode_info.nTrackSwitchType==0)
	{
		if(track_info->control_type==AUTO_CONTROL)
		{			
			g_cam_info.cam_position_lastvalue=0;
			//g_cam_info.cam_position_value=TRIGGER_POSITION42;
			//__call_preset_position(TRIGGER_POSITION42);
		}
	}

	dynamic_param->control_mode = track_info->control_type;
	dynamic_param->reset_level = NO_INIT;

#ifdef ENC_1200_DEBUG	
	PRINTF("track_info->control_type = %d\n",track_info->control_type);
#else
	DEBUG(DL_WARNING, "track_info->control_type = %d\n",track_info->control_type);
#endif
	
	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->control_mode);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, CONTROL_MODE, temp);
	unlock_save_track_mutex();
	return 0;
}

/**
* @	函数名称: set_draw_line_type()
* @	函数功能: 设置跟踪类型,0为自动跟踪,1为手动跟踪,并保存到配置文件中
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int set_draw_line_type(unsigned char *data, StuITRACK_DynamicParams *dynamic_param)
{
	draw_line_info_t *track_line_info = NULL;

	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	track_line_info = (draw_line_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);

	dynamic_param->message = track_line_info->message;
	dynamic_param->reset_level = NO_INIT;
#ifdef ENC_1200_DEBUG	
	PRINTF("track_line_info->message = %d\n",track_line_info->message);
#else
	DEBUG(DL_WARNING, "track_line_info->message = %d\n",track_line_info->message);
#endif
	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->message);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, MESSAGE, temp);
	unlock_save_track_mutex();
	return 0;
}



/**
* @	函数名称: get_cam_position()
* @	函数功能: 获取摄像头上下左右位置
* @	输入参数: 无
* @ 输出参数: data,返回获取的摄像头的位置信息
* @ 返回值:   无
*/
static int get_cam_position(unsigned char 	*data)
{
	int 			fd 				= gRemoteFD; 
	unsigned char 	cmd_data[20]	= {0};
	int 			ret 			= -1;
	int 			size 			= 0;

	cmd_data[0] = g_cam_info.cam_addr;
   	cmd_data[1] = 0x09;
   	cmd_data[2] = 0x06;
   	cmd_data[3] = 0x12;
   	cmd_data[4] = 0xFF;
	size = 5;
	SendDataToCom(fd, cmd_data, size);	
	return 0;
}

/**
* @	函数名称: get_cam_zoom()
* @	函数功能: 获取镜头现在的焦距位置
* @	输入参数: 无
* @ 输出参数: data,返回获取的摄像头的焦距信息
* @ 返回值:   无
*/
static int get_cam_zoom(unsigned char 	*data)
{
	int 			fd 				= gRemoteFD; 
	unsigned char 	cmd_data[20]	= {0};
	int 			ret 			= -1;
	int 			size 			= 0;

	cmd_data[0] = g_cam_info.cam_addr;
   	cmd_data[1] = 0x09;
   	cmd_data[2] = 0x04;
   	cmd_data[3] = 0x47;
   	cmd_data[4] = 0xFF;
	size = 5;
	SendDataToCom(fd, cmd_data, size);
	return 0;
}


/**
* @	函数名称: set_preset_position()
* @	函数功能: 设置预置位
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: 无
* @ 返回值:   无
*/
static int set_preset_position(unsigned char *data)
{
	preset_position_info_t 	*preset_position 			= NULL;
	int 					fd 							= gRemoteFD; 
	char 					temp[FILE_NAME_LEN]			= {0};
	char 					config_file[FILE_NAME_LEN] 	= {0};
	char					param_name[FILE_NAME_LEN] 	= {0};
	char					text[FILE_NAME_LEN]			= {0};
	unsigned char 			recv_data[32]				= {0};
	unsigned char 			temp_data[32]				= {0};
	int 					ret 						= -1;
	int						index						= 0;

	unsigned char	cmd_data[20]	= {0};
	int 			size			= 0;
	
	preset_position = (preset_position_info_t *)data;

#ifdef ENC_1200_DEBUG
	PRINTF("preset_position->preset_position = %d\n",preset_position->preset_position);
#else
	DEBUG(DL_WARNING, "preset_position->preset_position = %d\n",preset_position->preset_position);
#endif

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);
	g_preset_student_req.ePresetType = preset_position->preset_position;
	
	if(preset_position->preset_position > (PRESET_NUM_MAX))
	{
		DEBUG(DL_WARNING, "preset value must less than 5\n");
		return 0;
	}	
	SendPresetInfotoTeacher();
	return 0;
}
//保存预制位信息
void save_position(cam_preset_teacher_and_student_ack* g_preset_student_ack)
{
	int 	ret = -1;
	char	config_file[FILE_NAME_LEN]	= {0};
	char	temp[FILE_NAME_LEN] 		= {0};
	char	text[FILE_NAME_LEN] 		= {0};
	char	param_name[FILE_NAME_LEN]	= {0};
	int 	index						= 0;
	int 	i							= 0;
	unsigned char pan_tilt[CAM_PAN_TILT_LEN];
	unsigned char zoom[CAM_ZOOM_LEN];
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);
	
	lock_save_track_mutex();
	
	strcpy(text, PRESET_POSITION_VALUE);
	index = g_preset_student_ack->ePresetType;
	printf("index=%d\n",index);
	for(i=0;i<8;i++)
	{
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(param_name, "%s%d_%d",text, g_preset_student_ack->ePresetType-1,i);
		pan_tilt[i] = g_preset_student_ack->cCamCoordInfo.pan_tilt[i];
		sprintf(temp,"%d",pan_tilt[i]);
		ret =  ConfigSetKey(config_file, CAM_PRESET_POSITION, param_name, temp);

		if(i < 4)
				g_track_param.dynamic_param.CamPrePos[index-1].x[i] = pan_tilt[i];
			else
				g_track_param.dynamic_param.CamPrePos[index-1].y[i-4] = pan_tilt[i];
	}
	
	printf("pan_tilt=%x %x %x %x %x %x %x %x\n",pan_tilt[0],pan_tilt[1],pan_tilt[2],pan_tilt[3],pan_tilt[4],pan_tilt[5],pan_tilt[6],pan_tilt[7]);
	strcpy(text, PRESET_ZOOM_VALUE);
	for(i=0;i<4;i++)
	{
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(param_name, "%s%d_%d",text, g_preset_student_ack->ePresetType-1,i);
		zoom[i] = 
g_preset_student_ack->cCamCoordInfo.zoom[i];
		sprintf(temp,"%d",zoom[i]);
		ret =  ConfigSetKey(config_file, CAM_PRESET_POSITION, param_name, temp);

		g_track_param.dynamic_param.CamPrePos[index-1].z[i] = zoom[i];
	}
	printf("zoom =%x %x %x %x\n",zoom[0],zoom[1],zoom[2],zoom[3]);
	
	
	unlock_save_track_mutex();

    g_track_param.dynamic_param.reset_level = RE_INIT;

	

}



/**
* @	函数名称: __call_preset_position()
* @	函数功能: 调用预置位
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: 无
* @ 返回值:   无
*/
int __call_preset_position(short position)
{
	int 					fd 							= gRemoteFD; 
	unsigned char 			cmd_code[CTRL_CMD_LEN] 		= {0};
	int						size						= 0;

	unsigned char	cmd_data[20]	= {0};
	int nLastPositionTime=0,nCurPositionTime=0;
	int nDifTime=0;
	g_cam_info.cam_position_lastvalue=position;
	if(SONY_BRC_Z330 == g_track_cam_model.cam_type)
	{
		if(position > 15)
		{
			position = 0;
		}

		cmd_data[0] = g_cam_info.cam_addr;
		cmd_data[1] = 0x01;
		cmd_data[2] = 0x04;
		cmd_data[3] = 0x3F;
		cmd_data[4] = 0x02;

		cmd_data[5] = position;
		cmd_data[6] = 0xff;
		size = 7;
		SendDataToCom(fd, cmd_data, size);
	}
	else if(KXWELL_CAM == g_track_cam_model.cam_type)
	{
		DEBUG(DL_WARNING, "KXWELL_CAM call position = %d\n",position);
		size = jump_absolute_position(&cmd_code, position);
		if(size > 0)
		{
			SendDataToCom(fd, cmd_code, size);
		}
		usleep(50000);
		//nCurPositionTime=g_cam_info.cam_position[position].position_time*1000;

		if((AUTO_CONTROL == g_track_param.dynamic_param.control_mode)&&(position!=42))
		{//自动模式下用相对时间来推送
			if(nCurPositionTime<nLastPositionTime)
			{//只拉远
				DEBUG(DL_WARNING, "KXWELL_CAM call AUTO_CONTROL far,sleeptime=%d\n",nDifTime);
				nDifTime=nLastPositionTime-nCurPositionTime;
				//CCtrlZoomDecStart(fd, g_cam_info.cam_addr&0xf);
				usleep(nDifTime);
				//CCtrlZoomDecStop(fd, g_cam_info.cam_addr&0xf);
			}
			else if(nCurPositionTime>nLastPositionTime)
			{//只推近
				DEBUG(DL_WARNING, "KXWELL_CAM call AUTO_CONTROL near,sleeptime=%d\n",nDifTime);
				nDifTime=nCurPositionTime-nLastPositionTime;
				//CCtrlZoomAddStart(fd, g_cam_info.cam_addr&0xf);
				usleep(nDifTime);
				//CCtrlZoomAddStop(fd, g_cam_info.cam_addr&0xf);
			}
		}
		else
		{
			//FOCUS DEC Start 拉远DDEC_Start=^,01,04,07,37,FF

			//CCtrlZoomDecStart(fd, addr);
			cmd_data[0] = g_cam_info.cam_addr;
			cmd_data[1] = 0x01;
			cmd_data[2] = 0x04;
			cmd_data[3] = 0x07;
			cmd_data[4] = 0x37;
			cmd_data[5] = 0xff;
			size = 6;
			SendDataToCom(fd, cmd_data, size);
			DEBUG(DL_DEBUG, "KXWELL_CAM call far\n");
			usleep(8000000);

			//CCtrlZoomDecStop(fd, g_cam_info.cam_addr&0xf);
			DEBUG(DL_DEBUG, "KXWELL_CAM call far stop\n");

			usleep(100000);
			//CCtrlZoomAddStart(fd, g_cam_info.cam_addr&0xf);
			DEBUG(DL_DEBUG, "KXWELL_CAM call near\n");
			
			//DEBUG(DL_DEBUG, "KXWELL_CAM call near sleep = %d ms\n",g_cam_info.cam_position[position].position_time);
			usleep(nCurPositionTime);
			//CCtrlZoomAddStop(fd, g_cam_info.cam_addr&0xf);
			DEBUG(DL_DEBUG, "KXWELL_CAM call near stop\n");
		}

	}
	else
	{
		size = jump_zoom_position(&cmd_code, position);
		if(size > 0)
		{
			SendDataToCom(fd, cmd_code, size);
		}
		usleep(50000);

		size = jump_absolute_position(&cmd_code, position);
		if(size > 0)
		{
			SendDataToCom(fd, cmd_code, size);
		}
	}
	return 0;
}

/**
* @	函数名称: auto_call_preset_position_zoom()
* @	函数功能: 调用预置位对应的焦距位置
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: 无
* @ 返回值:   无
*/
int auto_call_preset_position_zoom(short position)
{
	int 					fd 							= gRemoteFD; 
	unsigned char 			cmd_code[CTRL_CMD_LEN] 		= {0};
	int						size						= 0;

	
	size = jump_zoom_position(&cmd_code, position);
	if(size > 0)
	{
		SendDataToCom(fd, cmd_code, size);
	}

	return 0;
}

/**
* @	函数名称: __call_preset_position()
* @	函数功能: 调用预置位
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: 无
* @ 返回值:   无
*/
int auto_call_preset_position_absolute(short position)
{
	int 					fd 							= gRemoteFD; 
	unsigned char 			cmd_code[CTRL_CMD_LEN] 		= {0};
	int						size						= 0;

	
	size = jump_absolute_position(&cmd_code, position);
	if(size > 0)
	{
		SendDataToCom(fd, cmd_code, size);
	}
	return 0;
}



/**
* @	函数名称: call_preset_position()
* @	函数功能: 调用预置位
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: 无
* @ 返回值:   无
*/
static int call_preset_position(unsigned char *data,StuITRACK_DynamicParams *dynamic_param)
{
	preset_position_info_t 	*preset_position 			= NULL;
	int 					fd 							= gRemoteFD; 
	unsigned char 			cmd_code[CTRL_CMD_LEN] 	= {0};
	int						size						= 0;

	preset_position = (preset_position_info_t *)data;

#ifdef ENC_1200_DEBUG
	PRINTF("call_preset_position preset_position = %d\n",preset_position->preset_position);
#else
	DEBUG(DL_WARNING, "call_preset_position preset_position = %d\n",preset_position->preset_position);
#endif

	if(preset_position->preset_position > (PRESET_NUM_MAX ))
	{
#ifdef ENC_1200_DEBUG
		PRINTF("call preset value must less than 5\n");
#else
		DEBUG(DL_WARNING, "call preset value must less than 5\n");
#endif
		return 0;
	}
	unsigned short position = preset_position->preset_position ;
	c_preset_student_req.cCamCoordInfo.cur_preset_value = position;
	memcpy(c_preset_student_req.cCamCoordInfo.pan_tilt,dynamic_param->CamPrePos[position-1].x,sizeof(unsigned char)*4);
	memcpy(c_preset_student_req.cCamCoordInfo.pan_tilt+4,dynamic_param->CamPrePos[position-1].y,sizeof(unsigned char)*4);
	memcpy(c_preset_student_req.cCamCoordInfo.zoom,dynamic_param->CamPrePos[position-1].z,sizeof(unsigned char)*4);
	SendPresetCoordTeacher();
	//__call_preset_position(preset_position->preset_position);
	return 0;
}

/**
* @	函数名称: set_track_is_encode()
* @	函数功能: 设置是否进行编码的标志
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: 无
* @ 返回值:   无
*/
static int set_track_is_encode(unsigned char *data)
{
	track_is_encode_info_t *encode_info 			= NULL;
	char 					temp[FILE_NAME_LEN]			= {0};
	char 					config_file[FILE_NAME_LEN] 	= {0};
	int 					ret 						= -1;

	encode_info = (track_is_encode_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);

	if(encode_info->isencode > 1)
	{
		encode_info->isencode = 1;
	}

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", encode_info->isencode);
	ret =  ConfigSetKey(config_file, STUDENTS_TRACK_ENCODE, STUDENTS_IS_ENCODE, temp);
	unlock_save_track_mutex();
	
	g_track_encode_info.is_encode = encode_info->isencode;

#ifdef ENC_1200_DEBUG
	PRINTF("encode_info->isencode = %d\n",encode_info->isencode);
#else
	DEBUG(DL_WARNING, "encode_info->isencode = %d\n",encode_info->isencode);
#endif
	return 0;
}

/**
* @	函数名称: set_track_param()
* @	函数功能: 导入跟踪参数表
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 		  len -- 参数表长度
* @ 输出参数: 无
* @ 返回值:   无
*/
static int set_track_param(unsigned char *data, int len)
{
	char cmd_name[256] = {0};

	FILE *fp;
	fp = fopen("/opt/reach/track_student_param.ini","w");
	fwrite(data,len,1,fp);
	fclose(fp);
	lock_save_track_mutex();
	sprintf(cmd_name, "mv /opt/reach/track_student_param.ini /opt/reach/%s", STUDENTS_TRACK_FILE);
	system(cmd_name);
	unlock_save_track_mutex();

	sleep(1);
	system("reboot -f");
	return 0;
}

/**
* @	函数名称: set_reset_time()
* @	函数功能: 设置复位时间,即目标丢失后多久开始触发
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int set_reset_time(unsigned char *data, StuITRACK_DynamicParams *dynamic_param)
{
	reset_time_info_t *reset_time_info = NULL;

	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	reset_time_info = (reset_time_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);

#ifdef ENC_1200_DEBUG
	PRINTF("reset_time_info->reset_time = %d\n",reset_time_info->reset_time);
#else
	DEBUG(DL_WARNING, "reset_time_info->reset_time = %d\n",reset_time_info->reset_time);
#endif

	dynamic_param->reset_time = reset_time_info->reset_time * SECOND_NUM;
	dynamic_param->reset_level = RE_INIT;

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->reset_time);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, RESET_TIME, temp);
	unlock_save_track_mutex();
	return 0;
}

/**
* @	函数名称: set_sens_value()
* @	函数功能: 设置检测变化的sens值
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int set_sens_value(unsigned char *data, StuITRACK_DynamicParams *dynamic_param)
{
	sens_info_t *sens_info = NULL;

	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	sens_info = (sens_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);

#ifdef ENC_1200_DEBUG
	PRINTF("reset_time_info->reset_time = %d\n",sens_info->sens);
#else
	DEBUG(DL_WARNING, "reset_time_info->reset_time = %d\n",sens_info->sens);
#endif

	dynamic_param->sens = sens_info->sens;
	dynamic_param->reset_level = RE_INIT;

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->sens);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, SENS, temp);
	unlock_save_track_mutex();
	return 0;
}

/**
* @ 函数名称: set_track_students_right_side_track_cam_offset_info()
* @ 函数功能: 设置跟踪摄像机角度偏移值
* @ 输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: 无
* @ 返回值:   无
*/
static int set_track_students_right_side_track_cam_offset_info(unsigned char *data, angle_info_t *angle_info)
{
 track_cam_offset_info_t *track_cam_offset_info = NULL;
 
 char  temp[FILE_NAME_LEN]   = {0};
 char  config_file[FILE_NAME_LEN]  = {0};
 char param_name[FILE_NAME_LEN]  = {0};
 int  ret       = -1;
 
 track_cam_offset_info = (track_cam_offset_info_t *)data;

 memset(config_file, 0, FILE_NAME_LEN);
 strcpy(config_file, STUDENTS_TRACK_FILE);

 //dynamic_param->cam_x_offset = track_cam_offset_info->x_offset;
 //dynamic_param->cam_y_offset = track_cam_offset_info->y_offset;
 //dynamic_param->track_num_row= track_cam_offset_info->row_width;
 angle_info->camera_pan_offset = track_cam_offset_info->x_offset;
 angle_info->camera_tilt_offset = track_cam_offset_info->y_offset;
 angle_info->camera_zoom_offset = track_cam_offset_info->row_width;
 
 g_track_param.dynamic_param.track_num_row = track_cam_offset_info->row_width*2;

 DEBUG(DL_DEBUG, "track_cam_offset_info->pan = %d\n",track_cam_offset_info->x_offset);
 DEBUG(DL_DEBUG, "track_cam_offset_info->tilt = %d\n",track_cam_offset_info->y_offset);
 DEBUG(DL_DEBUG, "track_cam_offset_info->zoom = %d\n",track_cam_offset_info->row_width);

 lock_save_track_mutex();
 memset(temp, 0, FILE_NAME_LEN);
 sprintf(temp, "%d", angle_info->camera_pan_offset);
 ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRACK_CAM_X_OFFSET, temp);

 memset(temp, 0, FILE_NAME_LEN);
 sprintf(temp, "%d", angle_info->camera_tilt_offset);
 ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRACK_CAM_Y_OFFSET, temp);

 memset(temp, 0, FILE_NAME_LEN);
 sprintf(temp, "%d", angle_info->camera_zoom_offset);
 ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRACK_CAM_OFFSET_ROW, temp);
 unlock_save_track_mutex();
 return 0;
}




/**
* @	函数名称: set_shield_range()
* @	函数功能: 设置屏蔽区域参数
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int set_shield_range(unsigned char *data, StuITRACK_DynamicParams *dynamic_param)
{
	shield_range_info_t *shield_info = NULL;
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	char	text[FILE_NAME_LEN]			= {0};
	int 	ret 						= -1;
	int		index						= 0;
	
	
	shield_info = (shield_range_info_t *)data;
	
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);

	dynamic_param->reset_level = RE_INIT;

	lock_save_track_mutex();

	for(index = 0; index < STUDENTS_SHIELD_NUM; index++)
	{
		dynamic_param->shield_info[index].x = shield_info->rectangle[index].x;
		dynamic_param->shield_info[index].y = shield_info->rectangle[index].y;
		dynamic_param->shield_info[index].width = shield_info->rectangle[index].width;
		dynamic_param->shield_info[index].height = shield_info->rectangle[index].height;
		printf("shield x=%d,y=%d,w=%d,h=%d\n",dynamic_param->shield_info[index].x,dynamic_param->shield_info[index].y,
			dynamic_param->shield_info[index].width,dynamic_param->shield_info[index].height);
		//存放在配置文件中
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, SHIELD_X);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", dynamic_param->shield_info[index].x);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
		usleep(5000);
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, SHIELD_Y);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", dynamic_param->shield_info[index].y);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
		usleep(5000);
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, SHIELD_WIDTH);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", dynamic_param->shield_info[index].width);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
		usleep(5000);
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, SHIELD_HEIGHT);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", dynamic_param->shield_info[index].height);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
		usleep(5000);
	}
	unlock_save_track_mutex();
	return 0;
}

/**
* @	函数名称: set_manual_commond()
* @	函数功能: 设置手动命令参数
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: 无
* @ 返回值:   无
*/
static int set_manual_commond(unsigned char *data)
{
	manual_commond_info_t *manual_commond_info 			= NULL;
	char 					temp[FILE_NAME_LEN]			= {0};
	char 					config_file[FILE_NAME_LEN] 	= {0};
	int 					ret 						= -1;

	manual_commond_info = (manual_commond_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);

	switch(manual_commond_info->type)
	{
		case SET_IS_CONTROL_CAM:
			
			if(manual_commond_info->value > 1)
			{
				manual_commond_info->value = 1;
			}
			
			lock_save_track_mutex();
			memset(temp, 0, FILE_NAME_LEN);
			sprintf(temp, "%d", manual_commond_info->value);
			ret =  ConfigSetKey(config_file, CAM_CONTROL_TYPE, IS_CONTROL_CAM, temp);
			unlock_save_track_mutex();
			DEBUG(DL_WARNING, "manual_commond_info->value = %d\n",manual_commond_info->value);
			
			g_track_encode_info.is_control_cam = manual_commond_info->value;
			break;
			
		case SET_ZOOM_PAN_DELAY:
			if(manual_commond_info->value < 30)
			{
				manual_commond_info->value = 30;
			}
			
			lock_save_track_mutex();
			memset(temp, 0, FILE_NAME_LEN);
			sprintf(temp, "%d", manual_commond_info->value);
			ret =  ConfigSetKey(config_file, CAM_CONTROL_TYPE, CAM_ZOOM_PAN_DELAY, temp);
			unlock_save_track_mutex();
			DEBUG(DL_WARNING, "manual_commond_info->value = %d\n",manual_commond_info->value);			
			g_track_encode_info.zoom_pan_delay = manual_commond_info->value;
			break;	
		case SET_DEBUG_LEVEL:
			lock_save_track_mutex();
			memset(temp, 0, FILE_NAME_LEN);
			sprintf(temp, "%d", manual_commond_info->value);
			ret =  ConfigSetKey(config_file, DEBUG_INFO, DEBUG_LEVEL_VAL, temp);
			unlock_save_track_mutex();
			g_nDebugLevel=manual_commond_info->value;
			DEBUG(DL_WARNING, "set debug level = %d\n",manual_commond_info->value);			
			break;
		default:
			break;
	}
	return 0;
}

/**
* @	函数名称: set_multitarget_range()
* @	函数功能: 设置多目标检测区域参数
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int set_multitarget_range(unsigned char *data, StuITRACK_DynamicParams *dynamic_param)
{
	multitarget_range_info_t *multitarget_info = NULL;
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	char	text[FILE_NAME_LEN]			= {0};
	int 	ret 						= -1;
	int		index						= 0;
	
	
	multitarget_info = (multitarget_range_info_t *)data;
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);

	dynamic_param->reset_level = RE_INIT;

	lock_save_track_mutex();

	for(index = 0; index < STUDENTS_MULTITARGET_NUM; index++)
	{
		dynamic_param->multitarget_info[index].x = multitarget_info->rectangle[index].x;
		dynamic_param->multitarget_info[index].y = multitarget_info->rectangle[index].y;
		dynamic_param->multitarget_info[index].width = multitarget_info->rectangle[index].width;
		dynamic_param->multitarget_info[index].height = multitarget_info->rectangle[index].height;

		//存放在配置文件中
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, MULTITARGET_X);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", dynamic_param->multitarget_info[index].x);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
		usleep(5000);
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, MULTITARGET_Y);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", dynamic_param->multitarget_info[index].y);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
		usleep(5000);
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, MULTITARGET_WIDTH);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", dynamic_param->multitarget_info[index].width);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
		usleep(5000);
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, MULTITARGET_HEIGHT);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", dynamic_param->multitarget_info[index].height);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
		usleep(5000);
	}
	unlock_save_track_mutex();
	return 0;
}

/**
* @	函数名称: set_cam_type()
* @	函数功能: 设置摄像头类型
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int set_cam_type(unsigned char *data)
{
	cam_type_info_t *cam_type_info = NULL;

	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	cam_type_info = (cam_type_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);

#ifdef ENC_1200_DEBUG
	PRINTF("cam_type_info->type = %d\n",cam_type_info->type);
#else
	DEBUG(DL_WARNING, "cam_type_info->type = %d\n",cam_type_info->type);
#endif

	g_track_cam_model.cam_type=cam_type_info->type;
	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", cam_type_info->type);
	ret =  ConfigSetKey(config_file, CAM_TYPE, CAM_TYPE, temp);
	unlock_save_track_mutex();
	return 0;
}

/**
* @	函数名称: send_track_range()
* @	函数功能: 发送跟踪框信息给encodemanage
* @	输入参数: socket -- 和encodemanage连接的socket
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/

static int send_track_range(int socket, StuITRACK_DynamicParams *dynamic_param)
{
#ifdef ENC_1200_DEBUG
	unsigned char send_buf[256] 	= {0};
	
	msg_header_t		*msg_header 	= NULL;
	track_header_t		*track_header 	= NULL;
	track_range_info_t	*track_range 	= NULL;
	int				len				= 0;
	int				index			= 0;

	memset(&send_buf, 0, 256);

	msg_header = (msg_header_t *)send_buf;
	len = sizeof(msg_header_t) + sizeof(track_header_t) + sizeof(track_range_info_t);
	msg_header->nLen = htons(len);
	msg_header->nMsg = 0xA1;

	track_header = (track_header_t *)&send_buf[sizeof(msg_header_t)];
	track_header->len = len - sizeof(msg_header_t);
	track_header->fixd_msg = FIXED_MSG;
	track_header->msg_type	= GET_TRACK_RANGE;

	track_range = (track_range_info_t *)&send_buf[sizeof(msg_header_t) + sizeof(track_header_t)];

	track_range->point_num = dynamic_param->track_point_num;
	track_range->state = 1;
	
	for(index = 0; index < dynamic_param->track_point_num; index++)
	{
		track_range->point[index].x = dynamic_param->track_point[index].x;
		track_range->point[index].y = dynamic_param->track_point[index].y;
	}
	
	send(socket, send_buf, len, 0);
	return 0;
#else
	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	track_range_info_t	track_range 	= {0};
	int				len				= 0;
	int				index			= 0;
	int				len_next		= 0;

	memset(send_buf, 0, 256);
	
	 
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(track_range_info_t);

	msg_len = htons(len);
	msg_type = 0xA1;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));
	
	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_TRACK_RANGE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));


	track_range.point_num = dynamic_param->track_point_num;
	track_range.state = 1;
	
	for(index = 0; index < dynamic_param->track_point_num; index++)
	{
		track_range.point[index].x = dynamic_param->track_point[index].x;
		track_range.point[index].y = dynamic_param->track_point[index].y;
	}

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &track_range, sizeof(track_range_info_t));
	DEBUG(DL_WARNING, "send_track_range len = %d\n",len);
	
	send(socket, send_buf, len, 0);
	return 0;

#endif
}


/**
* @	函数名称: send_trigger_range()
* @	函数功能: 发送触发框信息给encodemanage
* @	输入参数: socket -- 和encodemanage连接的socket
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int send_trigger_range(int socket, StuITRACK_DynamicParams *dynamic_param)
{
#ifdef ENC_1200_DEBUG
	unsigned char send_buf[256] 	= {0};
	
	msg_header_t		*msg_header 	= NULL;
	track_header_t		*track_header 	= NULL;
	trigger_range_info_t	*trigger_range 	= NULL;
	int				len				= 0;

	memset(&send_buf, 0, 256);

	msg_header = (msg_header_t *)send_buf;
	len = sizeof(msg_header_t) + sizeof(track_header_t) + sizeof(trigger_range_info_t);
	msg_header->nLen = htons(len);
	msg_header->nMsg = 0xA1;
	track_header = (track_header_t *)&send_buf[sizeof(msg_header_t)];
	track_header->len = len - sizeof(msg_header_t);
	track_header->fixd_msg = FIXED_MSG;
	track_header->msg_type	= GET_TRIGGER_RANGE;
	trigger_range = (trigger_range_info_t *)&send_buf[sizeof(msg_header_t) + sizeof(track_header_t)];

	trigger_range->point_num = 2;
	trigger_range->state = 1;
	
	trigger_range->point[0].x = dynamic_param->trigger_x0;
	trigger_range->point[0].y = dynamic_param->trigger_y0;
	trigger_range->point[1].x = dynamic_param->trigger_x1;
	trigger_range->point[1].y = dynamic_param->trigger_y1;
	send(socket, send_buf, len, 0);
	
	return 0;
#else
	unsigned char send_buf[2048] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	trigger_range_info_t	trigger_range 	= {0};

	int				len_next		= 0;
	int				len				= 0;
	int				index			= 0;

	memset(send_buf, 0, 2048);
	len = sizeof(unsigned short) + sizeof(unsigned char) + 
		sizeof(track_header_t) + sizeof(trigger_range_info_t);

	msg_len = htons(len);
	msg_type = 0xA1;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_TRIGGER_RANGE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	trigger_range.point_num = STUDENTS_TRIGGER_NUM;
	trigger_range.state = 1;

	for(index = 0; index < STUDENTS_TRIGGER_NUM; index++)
	{
		trigger_range.rectangle[index].x = dynamic_param->trigger_info[index].x;
		trigger_range.rectangle[index].y = dynamic_param->trigger_info[index].y;
		trigger_range.rectangle[index].width = dynamic_param->trigger_info[index].width;
		trigger_range.rectangle[index].height = dynamic_param->trigger_info[index].height;
	}

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &trigger_range, sizeof(trigger_range_info_t));

	send(socket, send_buf, len, 0);
	return 0;
#endif
}


/**
* @	函数名称: send_track_type()
* @	函数功能: 发送跟踪框信息给encodemanage
* @	输入参数: socket -- 和encodemanage连接的socket
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int send_track_type(int socket, StuITRACK_DynamicParams *dynamic_param)
{
#ifdef ENC_1200_DEBUG
	unsigned char send_buf[256] 	= {0};
	
	msg_header_t		*msg_header 	= NULL;
	track_header_t		*track_header 	= NULL;
	control_type_info_t	*control_type 	= NULL;
	int				len				= 0;
	int				index			= 0;


	memset(&send_buf, 0, 256);

	msg_header = (msg_header_t *)send_buf;
	len = sizeof(msg_header_t) + sizeof(track_header_t) + sizeof(control_type_info_t);
	msg_header->nLen = htons(len);
	msg_header->nMsg = 0xA1;


	track_header = (track_header_t *)&send_buf[sizeof(msg_header_t)];
	track_header->len = len - sizeof(msg_header_t);
	track_header->fixd_msg = FIXED_MSG;
	track_header->msg_type	= GET_TRACK_TYPE;

	control_type = (control_type_info_t *)&send_buf[sizeof(msg_header_t) + sizeof(track_header_t)];

	control_type->state = 1;
	
	control_type->control_type = dynamic_param->control_mode;
	
	send(socket, send_buf, len, 0);
	return 0;
#else
	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	control_type_info_t	control_type 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(control_type_info_t);

	msg_len = htons(len);
	msg_type = 0xA1;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_TRACK_TYPE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	control_type.state = 1;
	control_type.control_type = dynamic_param->control_mode;
	
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &control_type, sizeof(control_type_info_t));

	send(socket, send_buf, len, 0);
	return 0;
#endif
}

/**
* @	函数名称: send_draw_line_type()
* @	函数功能: 发送跟踪框信息给encodemanage
* @	输入参数: socket -- 和encodemanage连接的socket
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int send_draw_line_type(int socket, StuITRACK_DynamicParams *dynamic_param)
{
#ifdef ENC_1200_DEBUG
	unsigned char send_buf[256] 	= {0};
	
	msg_header_t		*msg_header 	= NULL;
	track_header_t		*track_header 	= NULL;
	draw_line_info_t	*draw_line_info = NULL;
	int				len				= 0;
	int				index			= 0;


	memset(&send_buf, 0, 256);

	msg_header = (msg_header_t *)send_buf;
	len = sizeof(msg_header_t) + sizeof(track_header_t) + sizeof(draw_line_info_t);
	msg_header->nLen = htons(len);
	msg_header->nMsg = 0xA1;

	track_header = (track_header_t *)&send_buf[sizeof(msg_header_t)];
	track_header->len = len - sizeof(msg_header_t);
	track_header->fixd_msg = FIXED_MSG;
	track_header->msg_type	= GET_DRAW_LINE_TYPE;

	draw_line_info = (draw_line_info_t *)&send_buf[sizeof(msg_header_t) + sizeof(track_header_t)];

	draw_line_info->state = 1;
	
	draw_line_info->message = dynamic_param->message;
	
	send(socket, send_buf, len, 0);
	return 0;
#else
	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	draw_line_info_t	draw_line_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(draw_line_info_t);

	msg_len = htons(len);
	msg_type = 0xA1;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_DRAW_LINE_TYPE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	draw_line_info.state = 1;
	draw_line_info.message = dynamic_param->message;

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &draw_line_info, sizeof(draw_line_info_t));

	send(socket, send_buf, len, 0);
	return 0;
#endif
}

/**
* @	函数名称: send_track_is_encode()
* @	函数功能: 发送跟踪距离参数信息给encodemanage
* @	输入参数: socket -- 和encodemanage连接的socket
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int send_track_is_encode(int socket, short is_encode)
{
#ifdef ENC_1200_DEBUG
	unsigned char send_buf[256] 	= {0};
	
	msg_header_t		*msg_header 	= NULL;
	track_header_t		*track_header 	= NULL;
	track_is_encode_info_t *track_is_encode = NULL;
	int				len				= 0;
	int				index			= 0;


	memset(&send_buf, 0, 256);

	msg_header = (msg_header_t *)send_buf;
	len = sizeof(msg_header_t) + sizeof(track_header_t) + sizeof(track_is_encode_info_t);
	msg_header->nLen = htons(len);
	msg_header->nMsg = 0xA1;

	track_header = (track_header_t *)&send_buf[sizeof(msg_header_t)];
	track_header->len = len - sizeof(msg_header_t);
	track_header->fixd_msg = FIXED_MSG;
	track_header->msg_type	= GET_TRACK_IS_ENCODE;

	track_is_encode = (track_is_encode_info_t *)&send_buf[sizeof(msg_header_t) + sizeof(track_header_t)];

	track_is_encode->state = 1;
	
	track_is_encode->isencode = is_encode;
	
	send(socket, send_buf, len, 0);
	return 0;
#else
	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	track_is_encode_info_t	track_is_encode 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(track_is_encode_info_t);

	msg_len = htons(len);
	msg_type = 0xA1;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_TRACK_IS_ENCODE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	track_is_encode.state = 1;
	track_is_encode.isencode = is_encode;

	DEBUG(DL_WARNING, "track_is_encode.isencode = %d\n",track_is_encode.isencode);
	
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &track_is_encode, sizeof(track_is_encode_info_t));

	send(socket, send_buf, len, 0);
	return 0;

#endif
}

/**
* @	函数名称: send_track_param()
* @	函数功能: 发送跟踪距离参数信息给encodemanage
* @	输入参数: socket -- 和encodemanage连接的socket
* @ 输出参数: 无
* @ 返回值:   无
*/
static int send_track_param(int socket)
{
	unsigned char send_buf[8192] 	= {0};
	unsigned char temp_buf[8192] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};

	int				len_next		= 0;
	int				len				= 0;
	int				file_len		= 0;

	char cmd_name[256] = {0};
	FILE *fp;
	sprintf(cmd_name, "/opt/reach/%s",STUDENTS_TRACK_FILE);
	lock_save_track_mutex();
	fp = fopen(cmd_name,"r");

	fread(temp_buf,8192,1,fp);
	
	fseek(fp,0,SEEK_END);
	file_len = ftell(fp);
	
	
	fclose(fp);
	unlock_save_track_mutex();

	DEBUG(DL_WARNING,"cmd_name = %s\n",cmd_name);
	DEBUG(DL_WARNING,"file_len = %d\n",file_len);

	memset(send_buf, 0, 8192);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + file_len;

	msg_len = htons(len);
	msg_type = 0xA1;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_TRACK_PARAM;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, temp_buf, file_len);

	send(socket, send_buf, len, 0);
	return 0;
}



/**
* @	函数名称: send_reset_time()
* @	函数功能: 发送跟踪框信息给encodemanage
* @	输入参数: socket -- 和encodemanage连接的socket
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int send_reset_time(int socket, StuITRACK_DynamicParams *dynamic_param)
{
#ifdef ENC_1200_DEBUG
	unsigned char send_buf[256] 	= {0};
	
	msg_header_t		*msg_header 	= NULL;
	track_header_t		*track_header 	= NULL;
	reset_time_info_t	*reset_time_info 	= NULL;
	int				len				= 0;
	int				index			= 0;

	memset(&send_buf, 0, 256);

	msg_header = (msg_header_t *)send_buf;
	len = sizeof(msg_header_t) + sizeof(track_header_t) + sizeof(reset_time_info_t);
	msg_header->nLen = htons(len);
	msg_header->nMsg = 0xA1;


	track_header = (track_header_t *)&send_buf[sizeof(msg_header_t)];
	track_header->len = len - sizeof(msg_header_t);
	track_header->fixd_msg = FIXED_MSG;
	track_header->msg_type	= GET_RESET_TIME;

	reset_time_info = (reset_time_info_t *)&send_buf[sizeof(msg_header_t) + sizeof(track_header_t)];

	reset_time_info->state = 1;
	
	reset_time_info->reset_time = dynamic_param->reset_time/FRAME_NUM;
	
	send(socket, send_buf, len, 0);
	return 0;
#else
	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	reset_time_info_t	reset_time_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(reset_time_info_t);

	msg_len = htons(len);
	msg_type = 0xA1;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_RESET_TIME;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	reset_time_info.state = 1;
	reset_time_info.reset_time = dynamic_param->reset_time/FRAME_NUM;

	DEBUG(DL_WARNING,"reset_time_info.reset_time  = %d\n",reset_time_info.reset_time );

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &reset_time_info, sizeof(reset_time_info_t));

	send(socket, send_buf, len, 0);
	return 0;
#endif
}

/**
* @	函数名称: send_sens_value()
* @	函数功能: 发送检测变化系数sens值给encodemanage
* @	输入参数: socket -- 和encodemanage连接的socket
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int send_sens_value(int socket, StuITRACK_DynamicParams *dynamic_param)
{
#ifdef ENC_1200_DEBUG
	unsigned char send_buf[256] 	= {0};
	
	msg_header_t		*msg_header 		= NULL;
	track_header_t		*track_header 		= NULL;
	sens_info_t	*sens_info 	= NULL;
	int				len				= 0;
	int				index			= 0;

	memset(&send_buf, 0, 256);

	msg_header = (msg_header_t *)send_buf;
	len = sizeof(msg_header_t) + sizeof(track_header_t) + sizeof(sens_info_t);
	msg_header->nLen = htons(len);
	msg_header->nMsg = 0xA1;


	track_header = (track_header_t *)&send_buf[sizeof(msg_header_t)];
	track_header->len = len - sizeof(msg_header_t);
	track_header->fixd_msg = FIXED_MSG;
	track_header->msg_type	= GET_SENS_VALUE;

	sens_info = (sens_info_t *)&send_buf[sizeof(msg_header_t) + sizeof(track_header_t)];

	sens_info->state = 1;
	
	sens_info->sens = dynamic_param->sens;
	
	send(socket, send_buf, len, 0);
	return 0;
#else
	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	sens_info_t	sens_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(sens_info_t);

	msg_len = htons(len);
	msg_type = 0xA1;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_SENS_VALUE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	sens_info.state = 1;
	sens_info.sens = dynamic_param->sens;

	DEBUG(DL_WARNING,"sens_info.sens  = %d\n",sens_info.sens);

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &sens_info, sizeof(sens_info_t));

	send(socket, send_buf, len, 0);
	return 0;
#endif
}

/**
* @	函数名称: send_shield_range()
* @	函数功能: 发送触发框信息给encodemanage
* @	输入参数: socket -- 和encodemanage连接的socket
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int send_shield_range(int socket, StuITRACK_DynamicParams *dynamic_param)
{
#ifdef ENC_1200_DEBUG
	return 0;
#else
	unsigned char send_buf[2048] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	shield_range_info_t	shield_range 	= {0};

	int				len_next		= 0;
	int				len				= 0;
	int				index			= 0;
	
	printf("end send shield1\n");

	memset(send_buf, 0, 2048);
	len = sizeof(unsigned short) + sizeof(unsigned char) + 
		sizeof(track_header_t) + sizeof(shield_range_info_t);

	msg_len = htons(len);
	msg_type = 0xA1;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));
	printf("end send shield2\n");

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_SHIELD_RANGE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	shield_range.point_num = STUDENTS_SHIELD_NUM;
	shield_range.state = 1;
	printf("end send shield3\n");

	for(index = 0; index < STUDENTS_SHIELD_NUM; index++)
	{
		shield_range.rectangle[index].x = dynamic_param->shield_info[index].x;
		shield_range.rectangle[index].y = dynamic_param->shield_info[index].y;
		shield_range.rectangle[index].width = dynamic_param->shield_info[index].width;
		shield_range.rectangle[index].height = dynamic_param->shield_info[index].height;
	}
	printf("end send shield3.5\n");

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &shield_range, sizeof(shield_range_info_t));
	printf("end send shield4\n");

	send(socket, send_buf, len, 0);
	printf("end send shield5\n");
	return 0;
#endif
}

/**
* @	函数名称: send_multitarget_range()
* @	函数功能: 发送多目标上台检测框信息给encodemanage
* @	输入参数: socket -- 和encodemanage连接的socket
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int send_multitarget_range(int socket, StuITRACK_DynamicParams *dynamic_param)
{
#ifdef ENC_1200_DEBUG
	return 0;
#else
	unsigned char send_buf[2048] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	multitarget_range_info_t	multitarget_range 	= {0};

	int				len_next		= 0;
	int				len				= 0;
	int				index			= 0;

	memset(send_buf, 0, 2048);
	len = sizeof(unsigned short) + sizeof(unsigned char) + 
		sizeof(track_header_t) + sizeof(multitarget_range_info_t);

	msg_len = htons(len);
	msg_type = 0xA1;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_MULTITARGET_RANGE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	multitarget_range.point_num = STUDENTS_MULTITARGET_NUM;
	multitarget_range.state = 1;

	for(index = 0; index < STUDENTS_MULTITARGET_NUM; index++)
	{
		multitarget_range.rectangle[index].x = dynamic_param->multitarget_info[index].x;
		multitarget_range.rectangle[index].y = dynamic_param->multitarget_info[index].y;
		multitarget_range.rectangle[index].width = dynamic_param->multitarget_info[index].width;
		multitarget_range.rectangle[index].height = dynamic_param->multitarget_info[index].height;
	}

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &multitarget_range, sizeof(multitarget_range_info_t));

	send(socket, send_buf, len, 0);
	return 0;
#endif
}

/**
* @	函数名称: send_cam_type()
* @	函数功能: 发送摄像机类型 给encodemanage
* @	输入参数: socket -- 和encodemanage连接的socket
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int send_cam_type(int socket)
{
#ifdef ENC_1200_DEBUG
	unsigned char send_buf[256] 	= {0};
	
	msg_header_t		*msg_header 	= NULL;
	track_header_t		*track_header 	= NULL;
	cam_type_info_t 	*cam_type_info = NULL;
	int				len				= 0;
	int				index			= 0;

	memset(&send_buf, 0, 256);

	msg_header = (msg_header_t *)send_buf;
	len = sizeof(msg_header_t) + sizeof(track_header_t) + sizeof(cam_type_info_t);
	msg_header->nLen = htons(len);
	msg_header->nMsg = 0xA1;


	track_header = (track_header_t *)&send_buf[sizeof(msg_header_t)];
	track_header->len = len - sizeof(msg_header_t);
	track_header->fixd_msg = FIXED_MSG;
	track_header->msg_type	= GET_CAM_TYPE;

	cam_type_info = (cam_type_info_t *)&send_buf[sizeof(msg_header_t) + sizeof(track_header_t)];

	cam_type_info->state = 1;
	
	cam_type_info->type= g_track_cam_model.cam_type;
	
	send(socket, send_buf, len, 0);
	return 0;
#else
	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	cam_type_info_t  cam_type_info = {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(cam_type_info_t);

	msg_len = htons(len);
	msg_type = 0xA1;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_CAM_TYPE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	cam_type_info.state = 1;
	cam_type_info.type= g_track_cam_model.cam_type;

	DEBUG(DL_WARNING,"cam_type_info.type  = %d\n",cam_type_info.type);

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &cam_type_info, sizeof(cam_type_info_t));

	send(socket, send_buf, len, 0);
	return 0;
#endif
}

/**
* @	函数名称: send_track_students_right_side_track_cam_offset_info()
* @	函数功能: 发送跟踪摄像机角度偏移值
* @	输入参数: socket -- 和encodemanage连接的socket
* @ 输出参数: dynamic_param -- 教师跟踪的动态参数
* @ 返回值:   无
*/
static int send_track_students_right_side_track_cam_offset_info(int socket, StuITRACK_DynamicParams *dynamic_param)
{

	unsigned char send_buf[256] 			= {0};
	unsigned short msg_len					= 0;
	unsigned char	msg_type				= 0;
	track_header_t		track_header 		= {0};
	track_cam_offset_info_t	track_cam_offset_info 	= {0}; 	

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(track_cam_offset_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type = GET_TRACK_CAM_OFFSET;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	track_cam_offset_info.state = 1;
	track_cam_offset_info.x_offset= dynamic_param->track_num_row/2;
	track_cam_offset_info.y_offset= 3;
	track_cam_offset_info.row_width = dynamic_param->track_num_row/2;
	

	DEBUG(DL_DEBUG,"track_cam_offset_info.pan  = %d\n",track_cam_offset_info.x_offset);
	DEBUG(DL_DEBUG,"track_cam_offset_info.tilt  = %d\n",track_cam_offset_info.y_offset);
	DEBUG(DL_DEBUG,"track_cam_offset_info.zoom  = %d\n",track_cam_offset_info.row_width);
	printf("row_width = %d\n",track_cam_offset_info.row_width);
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &track_cam_offset_info, sizeof(track_cam_offset_info_t));

	send(socket, send_buf, len, 0);
	return ;
}


/**
* @	函数名称: set_students_track_param()
* @	函数功能: 设置教师跟踪参数
* @	输入参数: data -- 从encodemanage接收到的设置参数数据
* @ 输出参数: 无
* @ 返回值:   无
*/
int set_students_track_param(unsigned char *data, int socket)
{
	
	track_header_t *track_header = (track_header_t *)data;

#ifdef ENC_1200_DEBUG
	PRINTF("track_header->msg_type = %d\n",track_header->msg_type);
#else
	DEBUG(DL_WARNING, "track_header = %d\n",track_header->msg_type);
#endif
printf("msg_type=%d\n",track_header->msg_type);
	

	switch(track_header->msg_type)
	{
		case SET_CAM_ADDR:
			set_cam_addr(data+sizeof(track_header_t));
			g_track_param.dynamic_param.reset_level = RE_INIT;
			g_recontrol_flag = 0;
			break;
		case SET_TRACK_RANGE:
			set_track_range(data+sizeof(track_header_t), &g_track_param.dynamic_param);
			g_track_param.dynamic_param.reset_level = RE_INIT;
			g_recontrol_flag = 1;
			break;
		
		case SET_TRIGGER_RANGE:
			set_trigger_range(data+sizeof(track_header_t), &g_track_param.dynamic_param);
			g_track_param.dynamic_param.reset_level = RE_INIT;
			g_recontrol_flag = 1;
			break;

		case SET_TRACK_TYPE:
			set_track_type(data+sizeof(track_header_t), &g_track_param.dynamic_param);
			g_track_param.dynamic_param.reset_level = RE_INIT;
			g_recontrol_flag = 0;
			break;
			
		case SET_DRAW_LINE_TYPE:
			set_draw_line_type(data+sizeof(track_header_t), &g_track_param.dynamic_param);
			g_track_param.dynamic_param.reset_level = RE_INIT;
			g_recontrol_flag = 1;
			break;
			
			
		case SET_PRESET_POSITION_TYPE:
			set_preset_position(data+sizeof(track_header_t));
			g_track_param.dynamic_param.reset_level = RE_INIT;
			g_recontrol_flag = 0;
			break;

		case CALL_PRESET_POSITION_TYPE:
			call_preset_position(data+sizeof(track_header_t),&g_track_param.dynamic_param);
			g_recontrol_flag = 0;
			break;
			
		case SET_TRACK_IS_ENCODE:
			set_track_is_encode(data+sizeof(track_header_t));
			g_track_param.dynamic_param.reset_level = RE_INIT;
			g_recontrol_flag = 0;
			break;

		case SET_TRACK_PARAM:
			set_track_param(data+sizeof(track_header_t),track_header->len - sizeof(track_header_t));
			g_track_param.dynamic_param.reset_level = RE_INIT;
			g_recontrol_flag = 0;
			break;
		
		case SET_RESET_TIME:
			set_reset_time(data+sizeof(track_header_t),&g_track_param.dynamic_param);
			g_track_param.dynamic_param.reset_level = RE_INIT;
			g_recontrol_flag = 1;
			break;	
			
		case SET_SENS_VALUE:
			set_sens_value(data+sizeof(track_header_t),&g_track_param.dynamic_param);
			g_track_param.dynamic_param.reset_level = RE_INIT;
			g_recontrol_flag = 1;
			break;

		case SET_SHIELD_RANGE:
			set_shield_range(data+sizeof(track_header_t), &g_track_param.dynamic_param);
			g_track_param.dynamic_param.reset_level = RE_INIT;
			g_recontrol_flag = 1;
			break;
		case SET_MULTITARGET_RANGE:
			set_multitarget_range(data+sizeof(track_header_t), &g_track_param.dynamic_param);
			g_track_param.dynamic_param.reset_level = RE_INIT;
			g_recontrol_flag = 1;
			break;

		case SET_MANUAL_COMMOND:
			set_manual_commond(data+sizeof(track_header_t));
			g_track_param.dynamic_param.reset_level = RE_INIT;
			g_recontrol_flag = 0;
			break;
		case SET_CAM_TYPE:
			set_cam_type(data+sizeof(track_header_t));
			g_track_param.dynamic_param.reset_level = RE_INIT;
			g_recontrol_flag = 0;
			break;
		case SET_TRACK_CAM_OFFSET:
			set_track_students_right_side_track_cam_offset_info(data+sizeof(track_header_t),&g_angle_info);
			g_track_param.dynamic_param.reset_level = RE_INIT;
			g_recontrol_flag = 0;
			break;
		case SET_DEFAULT_PARAM:
			//track_init_default_param();	
			g_recontrol_flag = 0;
			break;
		case GET_TRACK_RANGE:
			send_track_range(socket,&g_track_param.dynamic_param);
			g_recontrol_flag = 0;
			break;
/*
		case GET_TRIGGER_RANGE:
			send_trigger_range(socket,&g_track_param.dynamic_param);
			g_recontrol_flag = 0;
			break;
*/
		case GET_TRACK_TYPE:
			send_track_type(socket,&g_track_param.dynamic_param);
			g_recontrol_flag = 0;
			break;

		case GET_DRAW_LINE_TYPE:	
			send_draw_line_type(socket,&g_track_param.dynamic_param);
			g_recontrol_flag = 0;
			break;
			
		case GET_TRACK_IS_ENCODE:
			send_track_is_encode(socket, g_track_encode_info.is_encode);
			g_recontrol_flag = 0;
			break;		

		case GET_TRACK_PARAM:
			send_track_param(socket);
			g_recontrol_flag = 0;
			break;
			
		case GET_RESET_TIME:
			send_reset_time(socket,&g_track_param.dynamic_param);
			g_recontrol_flag = 0;
			break;		
		case GET_SENS_VALUE:
			send_sens_value(socket,&g_track_param.dynamic_param);
			g_recontrol_flag = 0;
			break;
		case GET_TRACK_CAM_OFFSET:
			send_track_students_right_side_track_cam_offset_info(socket, &g_track_param.dynamic_param);
			break;
			
		case GET_SHIELD_RANGE:
			send_shield_range(socket,&g_track_param.dynamic_param);
			g_recontrol_flag = 0;
			break;
		/*	
		case GET_MULTITARGET_RANGE:
			send_multitarget_range(socket,&g_track_param.dynamic_param);
			g_recontrol_flag = 0;
			break;
			*/
		case GET_CAM_TYPE:
			send_cam_type(socket);
			g_recontrol_flag = 0;
			break;
		case REBOOT_DEVICE:
		   reboot_device();
		   break;
		default:
			g_recontrol_flag = 0;
			break;
	}
	return 0;

}
static int get_class_info(StuITRACK_OutArgs *output_param,unsigned char* posCmdOutput)
{

		int32_t i = 0;
		active = 0;
		int32_t x,y,focalDistance;
		unsigned char temp[100];
		cam_preset_position_info_t pos_out = {0};
		for( i=0;i<CAM_PAN_TILT_LEN-1;i++)
		 {
			pos_out.pan_tilt[i] = posCmdOutput[i];
			
		}
		//sprintf(temp,"%x%x%x%x%x%x%x%x\n",pos_out.pan_tilt[0],pos_out.pan_tilt[1],pos_out.pan_tilt[2],pos_out.pan_tilt[3],pos_out.pan_tilt[4],pos_out.pan_tilt[5],pos_out.pan_tilt[6],pos_out.pan_tilt[7]);
		
		
		for(i=0;i<CAM_ZOOM_LEN;i++)
			{
			pos_out.zoom[i] = posCmdOutput[i+CAM_PAN_TILT_LEN-1];
			}
		
		if (output_param->up_num == 0 )//无目标
		{
			
			if(g_class_info.iStudentStandupNum != output_param->up_num)//发生变化，发送
			{
				g_class_info.iStudentStandupNum =0;
				g_class_info.cStudentCamCoord = pos_out;
				SendClassInfotoTeacher();
				active = 1;
			}
		}
		else if (output_param->up_num == 1)//只有一个目标
		{
		
			
			if(g_class_info.iStudentStandupNum != output_param->up_num)//数量不相同，发送
			{
				g_class_info.iStudentStandupNum = 1;
					
				g_class_info.cStudentCamCoord = pos_out;
				
				SendClassInfotoTeacher();
				active = 1;
			}
			else
			{
					//一个位置发生变化，发送
				if(memcmp(g_class_info.cStudentCamCoord.pan_tilt,pos_out.pan_tilt,CAM_PAN_TILT_LEN-1) != 0)
				{
				
					g_class_info.cStudentCamCoord = pos_out;
					SendClassInfotoTeacher();
					active = 1;
				}
			}
		}
		else if (output_param->up_num > 1)//多个目标
		{
			g_class_info.cStudentCamCoord = pos_out;
			if(g_class_info.iStudentStandupNum == 1)
			{
				g_class_info.iStudentStandupNum = 2;
				
				SendClassInfotoTeacher();
				active = 1;
			}
		}
		if (active == 1)
			{
			printf("%x %x %x %x %x %x %x %x\n",pos_out.pan_tilt[0],pos_out.pan_tilt[1],pos_out.pan_tilt[2],pos_out.pan_tilt[3],
				pos_out.pan_tilt[4],pos_out.pan_tilt[5],pos_out.pan_tilt[6],pos_out.pan_tilt[7]);
			for(i =0;i<output_param->up_num;i++)
			{
				printf("x =%d,y=%d\n",output_param->Up_List[i].x,output_param->Up_List[i].y);
			}
			}
	return 0;
}
static int get_class_info_rightside()
{
	memset(&g_class_info,0,sizeof(track_class_info_t));
	if(g_track_encode_info.nStandUpPos!=0)
	{
		DEBUG(DL_WARNING,"send class info nStandUpPos = %d\n",g_track_encode_info.nStandUpPos-1);
		g_class_info.nStandupPos[g_track_encode_info.nStandUpPos-1]=1;
		g_track_encode_info.nStandUpPos=0;
		SendClassInfotoTeacher();
	}
	return 0;
}

/**
* @	函数名称: cam_ctrl_cmd()
* @ 函数功能: 响应跟踪算法,并向摄像头发送转动命令
* @ 输入参数: output_param -- 跟踪算法返回的参数
* @ 输出参数: 无
* @ 返回值:   无
*/
int cam_ctrl_cmd(StuITRACK_OutArgs *output_param,unsigned char* posCmdOutput)
{
	
		get_class_info(output_param,posCmdOutput);
	
	return 0;
}


/**
* @	函数名称: cam_ctrl_cmd_rightside()
* @ 函数功能: 有学生辅助检测时用,响应跟踪算法,并向摄像头发送转动命令
* @ 输入参数: output_param -- 跟踪算法返回的参数
* @ 输出参数: 无
* @ 返回值:   无
*/

int cam_ctrl_cmd_rightside(StuITRACK_OutArgs *output_param)
{
	return 0;
}


/**
* @	函数名称: cam_ctrl_cmd_rightside()
* @ 函数功能: 有学生辅助检测时用,响应跟踪算法,并向摄像头发送转动命令
* @ 输入参数: output_param -- 跟踪算法返回的参数
* @ 输出参数: 无
* @ 返回值:   无
*/
int cam_ctrl_cmd_rightside_OnlySwitch1Stu(StuITRACK_OutArgs *output_param)
{
	return 0;
}

/**
* @	函数名称: cam_ctrl_cmd_rightside()
* @ 函数功能: 有学生辅助检测时用,响应跟踪算法,并向摄像头发送转动命令
* @ 输入参数: output_param -- 跟踪算法返回的参数
* @ 输出参数: 无
* @ 返回值:   无
*/
int cam_ctrl_cmd_rightside_OnlySwitch2Stu(StuITRACK_OutArgs *output_param)
{
	return 0;
}

int server_set_track_type(short type)
{
	g_track_param.dynamic_param.control_mode = type;
	return 0;
}


/**
* @	 自动模式下调用预置位线程
*/
void CamCallPosition(void *pParam)
{
	int nCallPosition=0;
	pthread_detach(pthread_self());
    while(1)
	{
		if(AUTO_CONTROL == g_track_param.dynamic_param.control_mode)
		{
			nCallPosition=g_cam_info.cam_position_value;
			if(nCallPosition!=g_cam_info.cam_position_lastvalue)
			{
				__call_preset_position(nCallPosition);
			}
		}
		usleep(20000);
    }
}


/**
* @	函数名称: track_init()
* @	函数功能: 初始化跟踪参数
* @ 输入参数: 
* @ 输出参数:
* @ 返回值:
*/
int track_init(StuITRACK_Params *track_param)
{
	int 	ret = -1;
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char 	temp[FILE_NAME_LEN]			= {0};
	char	text[FILE_NAME_LEN]			= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int		index						= 0;
	int		i 							= 0;
	
	unsigned char g_acCamPos[4][11] = {{0x90,0x50,0xf,0xd,0x8,0x9,0xf,0xe,0x5,0x0,0xff},
											   {0x90,0x50,0x0,0x1,0x7,0x8,0xf,0xe,0x5,0x0,0xff},
											   {0x90,0x50,0x0,0x0,0xd,0x2,0xf,0xf,0xc,0x6,0xff},
											   {0x90,0x50,0xf,0xe,0xb,0x3,0xf,0xf,0xd,0xd,0xff}};
			unsigned char g_acCamZoom[4][7] = {{0x90,0x50,0x0,0x0,0x0,0x0,0xff},
											   {0x90,0x50,0x0,0x0,0x0,0x0,0xff},
											   {0x90,0x50,0x2,0x5,0x1,0x2,0xff},
											   {0x90,0x50,0x2,0x0,0x5,0x7,0xff}};
	point_info_t pt[4] = {{50,580},{1100,580},{900,30},{250,30}};
	memset(track_param,0,sizeof(StuITRACK_Params));
	track_param->size = sizeof(StuITRACK_Params);

	track_param->dynamic_param.reset_time = 6000;
	track_param->dynamic_param.reset_level = RE_START;

	//track_param->dynamic_param.trigger_num = STUDENTS_TRIGGER_NUM;

	//学生跟踪配置文件名称
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);

	//-----------------------静态参数读取------------------------
	//视频宽度
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, STATIC_NAME, VIDEO_WIDTH, temp);
	if(ret != 0)
	{
	
		track_param->static_param.video_width = WIDTH;
	}
	else
	{
		track_param->static_param.video_width = atoi(temp);
	}
	printf("video_width =%d\n",track_param->static_param.video_width);

#ifdef ENC_1200_DEBUG
	PRINTF("track_param->static_param.video_width = %d\n",track_param->static_param.video_width);
#else
	DEBUG(DL_WARNING, "track_param->static_param.video_width = %d\n",track_param->static_param.video_width);
#endif
	
	//视频高度
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, STATIC_NAME, VIDEO_HEIGHT, temp);
	if(ret != 0)
	{
		track_param->static_param.video_height = HEIGHT;
	}
	else
	{
		track_param->static_param.video_height = atoi(temp);
	}
	printf("video_height =%d\n",track_param->static_param.video_height);

#ifdef ENC_1200_DEBUG
	PRINTF("track_param->static_param.video_height = %d\n",track_param->static_param.video_height);
#else
	DEBUG(DL_WARNING, "track_param->static_param.video_height = %d\n",track_param->static_param.video_height);
#endif	

	//跟踪处理图像的宽度
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, STATIC_NAME, PIC_WIDTH, temp);
	if(ret != 0)
	{
		track_param->static_param.pic_width = WIDTH;
	}
	else
	{
		track_param->static_param.pic_width = atoi(temp);
	}
	printf("pic_width =%d\n",track_param->static_param.pic_width);
#ifdef ENC_1200_DEBUG
	PRINTF("track_param->static_param.pic_width = %d\n",track_param->static_param.pic_width);
#else
	DEBUG(DL_WARNING, "track_param->static_param.pic_width = %d\n",track_param->static_param.pic_width);
#endif	

	//跟踪处理图像的高度
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, STATIC_NAME, PIC_HEIGHT, temp);
	if(ret != 0)
	{
		track_param->static_param.pic_height = HEIGHT;
	}
	else
	{
		track_param->static_param.pic_height = atoi(temp);
	}
	printf("pic_height =%d\n",track_param->static_param.pic_height);
#ifdef ENC_1200_DEBUG
	PRINTF("track_param->static_param.pic_height = %d\n",track_param->static_param.pic_height);
#else
	DEBUG(DL_WARNING, "track_param->static_param.pic_height = %d\n",track_param->static_param.pic_height);
#endif	

	//---------------------------动态参数的读取----------------------------
//shield
	track_param->dynamic_param.shield_info = (student_trigger_info_t*)malloc(sizeof(student_trigger_info_t)*STUDENTS_SHIELD_NUM);

	for(index = 0; index < STUDENTS_SHIELD_NUM; index++)
		{
	
			//存放在配置文件中
			memset(param_name, 0, FILE_NAME_LEN);
			memset(text, 0, FILE_NAME_LEN);
			strcpy(text, SHIELD_X);
			sprintf(param_name, "%s%d",text, index);
			memset(temp, 0, FILE_NAME_LEN);
			ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
			if(ret != 0)
				{
				track_param->dynamic_param.shield_info[index].x = 0;
				}
			else
				{
			track_param->dynamic_param.shield_info[index].x = atoi(temp);
				}
			memset(param_name, 0, FILE_NAME_LEN);
			memset(text, 0, FILE_NAME_LEN);
			strcpy(text, SHIELD_Y);
			sprintf(param_name, "%s%d",text, index);
			memset(temp, 0, FILE_NAME_LEN);
			ret =  ConfigGetKey(config_file, DYNAMIC_NAME, param_name, temp);
			if(ret != 0)
				{
				track_param->dynamic_param.shield_info[index].y = 0;
				}
			else
				{
			track_param->dynamic_param.shield_info[index].y = atoi(temp);
				}
			memset(param_name, 0, FILE_NAME_LEN);
			memset(text, 0, FILE_NAME_LEN);
			strcpy(text, SHIELD_WIDTH);
			sprintf(param_name, "%s%d",text, index);
			memset(temp, 0, FILE_NAME_LEN);
			ret =  ConfigGetKey(config_file, DYNAMIC_NAME, param_name, temp);
			if(ret != 0)
				{
				track_param->dynamic_param.shield_info[index].width = 0;
				}
			else
				{
			track_param->dynamic_param.shield_info[index].width = atoi(temp);
				}
			memset(param_name, 0, FILE_NAME_LEN);
			memset(text, 0, FILE_NAME_LEN);
			strcpy(text, SHIELD_HEIGHT);
			sprintf(param_name, "%s%d",text, index);
			memset(temp, 0, FILE_NAME_LEN);
			ret =  ConfigGetKey(config_file, DYNAMIC_NAME, param_name, temp);
			if(ret != 0)
				{
				track_param->dynamic_param.shield_info[index].height = 0;
				}
			else
				{
			track_param->dynamic_param.shield_info[index].height = atoi(temp);
				}
		}


	//跟踪中丢失目标后,到下次重新跟踪时间,又叫复位时间
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, RESET_TIME, temp);
	if(ret != 0)
	{
		track_param->dynamic_param.reset_time = 6000;
	}
	else
	{
		track_param->dynamic_param.reset_time = atoi(temp);
	}
	//printf("reset_time =%d\n",track_param->dynamic_param.reset_time);

#ifdef ENC_1200_DEBUG
	PRINTF("track_param->dynamic_param.reset_time = %d\n",track_param->dynamic_param.reset_time);
#else
	DEBUG(DL_WARNING, "track_param->dynamic_param.reset_time = %d\n",track_param->dynamic_param.reset_time);
#endif


	    memset(config_file, 0, FILE_NAME_LEN);
		strcpy(config_file, STUDENTS_TRACK_FILE);
		memset(temp, 0, FILE_NAME_LEN);
		ret =  ConfigGetKey(config_file, DYNAMIC_NAME, CONTROL_MODE, temp);
		if(ret != 0)
		{
			track_param->dynamic_param.control_mode= 0;
			
		}
		else
		{
			track_param->dynamic_param.control_mode = atoi(temp);
		}
	//	printf("control_mode =%d\n",track_param->dynamic_param.control_mode);
	
#ifdef ENC_1200_DEBUG
		PRINTF("track_param->dynamic_param.control_mode = %d\n",track_param->dynamic_param.control_mode);
#else
		DEBUG(DL_WARNING, "track_param->dynamic_param.control_mode = %d\n",track_param->dynamic_param.control_mode);
#endif

	
	//横坐标缩放倍数
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, X_OFFSET, temp);
	if(ret != 0)
	{
		track_param->dynamic_param.x_offset = 1;
	}
	else
	{
		track_param->dynamic_param.x_offset = atoi(temp);
	}
	//printf("x_offset =%d\n",track_param->dynamic_param.x_offset);
#ifdef ENC_1200_DEBUG
	PRINTF("track_param->dynamic_param.x_offset = %d\n",track_param->dynamic_param.x_offset);
#else
	DEBUG(DL_WARNING, "track_param->dynamic_param.x_offset = %d\n",track_param->dynamic_param.x_offset);
#endif

	//纵坐标缩放倍数
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, Y_OFFSET, temp);
	if(ret != 0)
	{
		track_param->dynamic_param.y_offset = 1;
	}
	else
	{
		track_param->dynamic_param.y_offset = atoi(temp);
	}
	//printf("y_offset =%d\n",track_param->dynamic_param.y_offset);
#ifdef ENC_1200_DEBUG
	PRINTF("track_param->dynamic_param.y_offset = %d\n",track_param->dynamic_param.y_offset);
#else
	DEBUG(DL_WARNING, "track_param->dynamic_param.y_offset = %d\n",track_param->dynamic_param.y_offset);
#endif

	//sens值,越小边缘值找到越多
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, SENS, temp);
	if(ret != 0)
	{
		track_param->dynamic_param.sens = 30;
	}
	else
	{
		track_param->dynamic_param.sens = atoi(temp);
	}
	//printf("sens =%d\n",track_param->dynamic_param.sens);
#ifdef ENC_1200_DEBUG
	PRINTF("track_param->dynamic_param.sens = %d\n",track_param->dynamic_param.sens);
#else
	DEBUG(DL_WARNING, "track_param->dynamic_param.sens = %d\n",track_param->dynamic_param.sens);
#endif

	//画线标志
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, MESSAGE, temp);
	if(ret != 0)
	{
		track_param->dynamic_param.message = 1;
	}
	else
	{
		track_param->dynamic_param.message = atoi(temp);
	}
#ifdef ENC_1200_DEBUG
	PRINTF("track_param->dynamic_param.message = %d\n",track_param->dynamic_param.message);
#else
	DEBUG(DL_WARNING, "track_param->dynamic_param.message = %d\n",track_param->dynamic_param.message);
#endif


	//跟踪区域点数
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, TRACK_POINT_NUM, temp);
	if(ret != 0)
	{
		track_param->dynamic_param.track_point_num = 4;
	}
	else
	{
		track_param->dynamic_param.track_point_num = atoi(temp);
	}
	//printf("track_point_num =%d\n",track_param->dynamic_param.track_point_num);
#ifdef ENC_1200_DEBUG
	PRINTF("track_param->dynamic_param.track_point_num = %d\n",track_param->dynamic_param.track_point_num);
#else
	DEBUG(DL_WARNING, "track_param->dynamic_param.track_point_num = %d\n",track_param->dynamic_param.track_point_num);
#endif

	//每行学生数量
	
		memset(temp, 0, FILE_NAME_LEN);
		ret =  ConfigGetKey(config_file, DYNAMIC_NAME, TRACK_CAM_OFFSET_ROW, temp);
		if(ret != 0)
		{
			track_param->dynamic_param.track_num_row = 6;
		}
		else
		{
			track_param->dynamic_param.track_num_row = atoi(temp)*2;
		}
		//printf("track_num_row =%d\n",track_param->dynamic_param.track_num_row);
#ifdef ENC_1200_DEBUG
		PRINTF("track_param->dynamic_param.track_point_num = %d\n",track_param->dynamic_param.track_point_num);
#else
		DEBUG(DL_WARNING, "track_param->dynamic_param.track_point_num = %d\n",track_param->dynamic_param.track_point_num);
#endif


	
	for(index = 0; index < track_param->dynamic_param.track_point_num; index++)
	{
		//跟踪区域点的坐标
		memset(text, 0, FILE_NAME_LEN);
		memset(param_name, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_POINTX);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		ret =  ConfigGetKey(config_file, DYNAMIC_NAME, param_name, temp);
		if(ret != 0)
		{
			track_param->dynamic_param.track_point[index].x = pt[index].x;
		}
		else
		{
			track_param->dynamic_param.track_point[index].x = atoi(temp);
		}
		
		memset(text, 0, FILE_NAME_LEN);
		memset(param_name, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_POINTY);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		ret =  ConfigGetKey(config_file, DYNAMIC_NAME, param_name, temp);
		if(ret != 0)
		{
			track_param->dynamic_param.track_point[index].y = pt[index].y;
		}
		else
		{
			track_param->dynamic_param.track_point[index].y = atoi(temp);
		}
		
#ifdef ENC_1200_DEBUG
		PRINTF("track_param->dynamic_param.track_point[%d].x = %d\n",index,track_param->dynamic_param.track_point[index].x);
		PRINTF("track_param->dynamic_param.track_point[%d].y = %d\n",index,track_param->dynamic_param.track_point[index].y);
#else
		DEBUG(DL_WARNING, "track_param->dynamic_param.track_point[%d].x = %d\n",index,track_param->dynamic_param.track_point[index].x);
		DEBUG(DL_WARNING, "track_param->dynamic_param.track_point[%d].y = %d\n",index,track_param->dynamic_param.track_point[index].y);
#endif
	}


	//摄像头预置位的位置信息,0的位置暂时保留,没有使用
	for(index = 1; index <= PRESET_NUM_MAX; index++)
	{
		memset(temp, 0, FILE_NAME_LEN);
		strcpy(text, PRESET_POSITION_VALUE);
		sprintf(param_name, "%s%d",text, index);
		
		
		for(i = 0; i < 8; i++)
		{
			memset(text, 0, FILE_NAME_LEN);
			memset(param_name, 0, FILE_NAME_LEN);
			strcpy(text, PRESET_POSITION_VALUE);
			sprintf(param_name, "%s%d_%d",text, index-1,i);
			memset(temp, 0, FILE_NAME_LEN);
			ret =  ConfigGetKey(config_file, CAM_PRESET_POSITION, param_name, temp);
			if(ret !=0)
			{
				g_cam_info.cam_position[index-1].pan_tilt[i] = g_acCamPos[index-1][i+2];
				
			}
			else
			{
				g_cam_info.cam_position[index-1].pan_tilt[i] = atoi(temp);
				//printf("temp %d =%s\n",i,temp);
			}
			//printf("pan_tilt=%d\n",g_cam_info.cam_position[index-1].pan_tilt[i]);
			if(i < 4)
				track_param->dynamic_param.CamPrePos[index-1].x[i] = g_cam_info.cam_position[index-1].pan_tilt[i];
			else
				track_param->dynamic_param.CamPrePos[index-1].y[i-4] = g_cam_info.cam_position[index-1].pan_tilt[i];
				
		}
	

		
#ifdef ENC_1200_DEBUG
		PRINTF("###########index = %d,%s\n",index, temp);
#else
		DEBUG(DL_WARNING, "###########index = %d,%s\n",index, temp);
#endif
		for(i = 0; i < 4; i++)
		{
			memset(text, 0, FILE_NAME_LEN);
			memset(param_name, 0, FILE_NAME_LEN);
			strcpy(text, PRESET_ZOOM_VALUE);
			sprintf(param_name, "%s%d_%d",text, index-1,i);
			memset(temp, 0, FILE_NAME_LEN);
			ret =  ConfigGetKey(config_file, CAM_PRESET_POSITION, param_name, temp);
			if(ret != 0)
			{
				g_cam_info.cam_position[index-1].zoom[i] = g_acCamZoom[index-1][i+2];
			}
			else
			{
				g_cam_info.cam_position[index-1].zoom[i] = atoi(temp);
			}
			//printf("zoom=%d\n",g_cam_info.cam_position[index-1].zoom[i]);
			track_param->dynamic_param.CamPrePos[index-1].z[i] = g_cam_info.cam_position[index-1].zoom[i];
		

		}
	}
	//跟踪机是否编码
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, STUDENTS_TRACK_ENCODE, STUDENTS_IS_ENCODE, temp);

	
	if(ret != 0)
	{
		g_track_encode_info.is_encode = 1;
	}	
	else
	{
		g_track_encode_info.is_encode= atoi(temp);
	}
#ifdef ENC_1200_DEBUG
	PRINTF("g_track_encode_info.is_encode = %d\n",g_track_encode_info.is_encode);
#else
	DEBUG(DL_WARNING, "g_track_encode_info.is_encode = %d\n",g_track_encode_info.is_encode);
#endif

	//学生机是否推近景标志，为1为推近景，为0不推近景
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_CONTROL_TYPE, IS_CONTROL_CAM, temp);
	if(ret != 0)
	{
		g_track_encode_info.is_control_cam = 1;
	}	
	else
	{
		g_track_encode_info.is_control_cam= atoi(temp);
	}

#ifdef ENC_1200_DEBUG
	PRINTF("g_track_encode_info.is_encode = %d\n",g_track_encode_info.is_control_cam);
#else
	DEBUG(DL_WARNING, "g_track_encode_info.is_encode = %d\n",g_track_encode_info.is_control_cam);
#endif

	//摄像机延时参数读取，为调用预制位时两个命令之间的延时参数
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_CONTROL_TYPE, CAM_ZOOM_PAN_DELAY, temp);
	g_track_encode_info.zoom_pan_delay = atoi(temp);
	
#ifdef ENC_1200_DEBUG
	PRINTF("g_track_encode_info.zoom_pan_delay = %d\n",g_track_encode_info.zoom_pan_delay);
#else
	DEBUG(DL_WARNING, "g_track_encode_info.zoom_pan_delay = %d\n",g_track_encode_info.zoom_pan_delay);
#endif
	if(ret != 0)
	{
		g_track_encode_info.zoom_pan_delay = 500;
	}

//设置摄像头类型
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, STUDENTS_TRACK_FILE);
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_TYPE, CAM_TYPE, temp);
	if(ret != 0)
	{
		g_track_cam_model.cam_type = 0;
	}
	else
	{
		g_track_cam_model.cam_type = atoi(temp);
	}

#ifdef ENC_1200_DEBUG
	PRINTF("g_track_cam_model.cam_type = %d\n",g_track_cam_model.cam_type);
#else
	DEBUG(DL_WARNING, "g_track_cam_model.cam_type = %d\n",g_track_cam_model.cam_type);
#endif

	//摄像头地址，读取配置文件ip以及设置
		track_ip_info_t cam_info;
		memset(temp, 0, FILE_NAME_LEN);
		ret =  ConfigGetKey(config_file, CAM_PARAM, CAM_ADDR, temp);
		if(ret != 0)
		{
			sprintf(temp,"192.168.4.188");
		}
		memcpy(cam_info.ip,temp,16);
		memset(temp, 0, FILE_NAME_LEN);
		ret = ConfigGetKey(config_file, CAM_PARAM,CAM_MASK , temp);
		if(ret != 0)
		{
			sprintf(temp,"255.255.255.0");
		}
		memcpy(cam_info.mask,temp,16);
		memset(temp, 0, FILE_NAME_LEN);
		ret = ConfigGetKey(config_file, CAM_PARAM,CAM_ROUTE , temp);
		if(ret != 0)
		{
			sprintf(temp,"192.168.0.1");
		}
		memcpy(cam_info.route,temp,16);
		int id_num,netmask,gw;
		struct in_addr addr1, addr2,addr3;
		id_num = inet_addr(cam_info.ip); 
		netmask = inet_addr(cam_info.mask);
		gw = inet_addr(cam_info.route);
		if(CheckIPNetmask(id_num,netmask,gw) == 0)
		{
			DEBUG(DL_ERROR, "ERROR ID %s\n", cam_info.ip);
		}
		ret = StartBroadcastServer(AUTHOR_TYPE_STUDENT,cam_info.ip);
		if(ret < 0)
		{
			DEBUG(DL_WARNING,"StartBroadcastServer error");
		}
		memset(temp, 0, FILE_NAME_LEN);
		strcat(temp,"ifconfig eth0 ");
		memcpy(&addr1,&id_num,4);
		strcat(temp,inet_ntoa(addr1));
		memcpy(&addr2,&netmask,4);
		strcat(temp," netmask ");
		strcat(temp,inet_ntoa(addr2));
		printf("out = %s\n",temp);
		
		ret = system(temp); 
		if (ret <0)
		{
			DEBUG(DL_ERROR, "ifconfig %s up ERROR\n", inet_ntoa(addr1));
		}
		
		memset(temp, 0, FILE_NAME_LEN);
		memcpy(&addr3,&gw,4);
		strcat(temp,"route add default gw ");
		strcat(temp,inet_ntoa(addr3));
		
		ret = system(temp); 
		if (ret <0)
		{
			DEBUG(DL_ERROR, "route add default gw %s up ERROR\n", inet_ntoa(addr3));
		}
		

		
	g_track_encode_info.is_track = 1;
	return 0;
}

int student_init_params()
{
		int result;
		//StuITRACK_Params* track_param = (StuITRACK_Params*)param;
		result = track_init(&g_track_param);
		//track_param = &g_track_param ;
		//printf("%d\n",track_param->dynamic_param.track_num_row);
		return result;
}


#endif

