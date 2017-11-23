
/*
 * auto_track.h
 *
 * ============================================================================
 * Copyright (c) Texas Instruments Inc 2012
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied or provided.
 * ============================================================================
 */

#ifndef _AUTO_TRACK_H
#define _AUTO_TRACK_H

#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include "rh_motion_detect.h"

/**
* @	每个统计时间段的长度，分钟表示
*/
#define CLASSINFO_PER_TIME              (15)

/**
* @	最大活跃时间段数
*/
#define MAX_INTERACTION_NUM					(20)

/**
* @	最大起立次数
*/
#define MAX_STANDUP_NUM					(50)



/**
* @	描述板书区域个数
*/
#define TRACK_BROADCAST_NUM				(2)

/**
* @	用来描述一个板书区域的点数
*/
#define ONE_BROADCAST_POINT_NUM			(2)

/**
* @	描述板书区域的点数，是板书区域的2倍
*/
#define TRACK_BROADCAST_POINT_NUM				(TRACK_BROADCAST_NUM * ONE_BROADCAST_POINT_NUM)

/**
* @ 链接学生的最大连接数
*/
#define		STUCONNECT_NUM_MAX		(2)		
/**
* @ 链接板书的最大连接数
*/
#define		BLACKCONNECT_NUM_MAX		(2)		




/**
* @ 摄像机角度个数
*/
#define CAM_ANGLE_NUM	(36)

/**
* @	角度对应的最小单位的像素点
*/
#define ANGLE_POINT_NUM	(20)




/**
* @	文件名称的长度
*/
#define FILE_NAME_LEN		(64)

/**
* @ 教师跟踪参数保存文件
*/
#define TRACK_TEACHER_POINT_CAM_FILE		"track_teacher_point_cam.ini"

#define DEBUG_INFO "debug_info"
#define DEBUG_IPADDR "debug_ipaddr"

/**
* @	保存课前取景的图片路径
*/
#define	CLASS_VIEW_JPG	"/opt/reach/class_view.yuv"

/**
* @	确定跟踪框点的个数
*/
#define TRACK_RANGE_NUM					(10)

/**
* @	确定触发框点的个数
*/
#define TRACK_TRIGGER_NUM				(4)


/**
* @	摄像机绝对位置所占的字节数普通VISCA占用9个字节，索尼BRC-Z330占用8个字节
*/
#define	CAM_PAN_TILT_LEN	(9)

/**
* @	摄像机绝对焦距所占字节数为4个字节
*/
#define	CAM_ZOOM_LEN		(4)




/**
* @	预置位的最大个数
*/
#define PRESET_NUM_MAX					(7)


/**
* @	固定消息字
*/
#define FIXED_MSG						(0xFFF0)


//==============================设置参数============================
/**
* @	设定跟踪范围的消息字
*/
#define SET_TRACK_RANGE					(0x0001)

/**
* @	设定触发区域消息字
*/
#define SET_TRIGGER_RANGE				(0x0002)

/**
* @	设置人头像的跟踪线的高				
*/
#define	SET_TRACK_LIMIT_HEIGHT			(0x0003)

/**
* @	跟踪类型消息字 
*/
#define SET_TRACK_TYPE					(0x0004)


/**
* @	设置画线类型,调试线,如跟踪框线,跟踪区域线,轨迹线等
*/
#define SET_DRAW_LINE_TYPE				(0x0005)

/**
* @	设置摄像头控制速度的基数消息字 
*/
#define SET_CAM_SPEED_TYPE				(0x0006)

/**
* @	设置触发点总数消息字 
*/
#define SET_TRIGGER_NUM_TYPE			(0x0007)

/**
* @	设置预置位消息字 
*/
#define SET_PRESET_POSITION_TYPE		(0x0008)

/**
* @	设置摄像头上下左右限位消息字 
*/
#define SET_LIMIT_POSITION_TYPE			(0x0009)

/**
* @	调用预置位消息字
*/
#define CALL_PRESET_POSITION_TYPE		(0x000a)

/**
* @	设置跟踪距离
*/
#define SET_TRACK_DISTANCE_TYPE			(0x000b)

/**
* @	设置跟踪机是否编码的开关
*/
#define SET_TRACK_IS_ENCODE				(0x000c)

/**
* @ 导入跟踪参数消息字
*/
#define SET_TRACK_PARAM					(0x000d)

/**
* @ 设置预跟踪时间
*/
#define SET_START_TRACK_TIME			(0x000e)

/**
* @ 设置跟踪复位时间
*/
#define SET_RESET_TIME					(0x000f)


/**
* @ 设置检测变化系数sens值
*/
#define SET_SENS_VALUE					(0x0010)

/**
* @ 设置手动命令
*/
#define SET_MANUAL_COMMOND				(0x0012)

/**
* @ 设置设置跟踪模式
*/
#define SET_TRACK_MODE					(0x0013)

/**
* @ 设置机位
*/
#define SET_STRATEGY_NO					(0x0018)



/**
* @ 设置学生全景保持时间
*/
#define SET_STUDENTS_PANORAMA_SWITCH_NEAR_TIME				(0x001a)

/**
* @ 设置老师走到板书1区域切板书的时间
*/
#define SET_TEACHER_BLACKBOARD_TIME1				(0x001b)

/**
* @ 设置离开板书1区域切老师的时间
*/
#define SET_TEACHER_LEAVE_BLACKBOARD_TIME1				(0x001c)

/**
* @ 设置学生下讲台后不启用跟踪的时间
*/
#define SET_STUDENTS_DOWN_TIME				(0x001d)

/**
* @ 设置老师近景摄像头移动切换老师全景的时间
*/
#define SET_TEACHER_PANORAMA_TIME				(0x001e)

/**
* @ 设置老师摄像头停止移动切换老师近景的时间
*/
#define SET_TEACHER_LEAVE_PANORAMA_TIME				(0x001f)

/**
* @ 设置学生上讲台后保持老师全景的最长时间
*/
#define SET_TEACHER_KEEP_PANORAMA_TIME				(0x0020)

/**
* @ 设置老师切换为学生前的老师镜头保持时间
*/
#define SET_TEACHER_SWITCH_STUDENTS_DELAY_TIME				(0x0021)

/**
* @ 设置学生近景保持的时间
*/
#define SET_STUDENTS_NEAR_KEEP_TIME				(0x0022)

/**
* @ 设置老师丢了时老师全景与学生全景切换延时
*/
#define SET_MV_KEEP_TIME				(0x0023)

/**
* @ 设置vga保持时间
*/
#define SET_VGA_KEEP_TIME				(0x0024)

/**
* @ 设置老师走到板书2区域切板书的时间
*/
#define SET_TEACHER_BLACKBOARD_TIME2				(0x0025)

/**
* @ 设置离开板书2区域切老师的时间
*/
#define SET_TEACHER_LEAVE_BLACKBOARD_TIME2				(0x0026)




/**
* @ 黑板区域触发范围
*/
#define SET_BLACKBOARD_TRIGGER				(0x0027)



/**
* @设置摄像机类型，即不同型号摄像机的选择
*/
#define	SET_CAM_TYPE					(0x0028)

/**
* @ 设置学生轮询信息
*/
#define SET_TRACK_TURN_INFO				(0x002a)

/**
* @ 设置板书方案
*/
#define SET_BLACKBOARD_SCHEME				(0x002b)


/**
* @	设置定位摄像机所在位置教室的长宽高
*/
#define	SET_POSITION_CAM_INFO			(0x0040)


/**
* @	设置跟踪摄像机所在位置的长宽高
*/
#define	SET_TRACK_CAM_INFO				(0x0041)


/**
* @	设置跟踪摄像机水平，垂直，焦距的偏移值
*/
#define	SET_TRACK_CAM_OFFSET			(0x0042)

/**
* @	设置本机的IP信息
*/
#define	SET_LOCAL_IP_INFO					(0x4D)

/**
* @	设置学生机的IP信息
*/
#define	SET_STUDENT_IP_INFO					(0x4E)

/**
* @	设置板书机0的IP信息
*/
#define	SET_BLOCKBOARD_0_IP_INFO					(0x4F)

/**
* @	设置板书机1的IP信息
*/
#define	SET_BLOCKBOARD_1_IP_INFO					(0x50)

/**
* @	设置对面定位摄像机的位置
*/
#define	SET_OTHER_CAM_POSITION			(0x0043)


/**
* @	设置跟踪机类型(是主跟踪机还是辅学生机)
*/
#define	SET_HOST_TYPE					(0x0044)


/**
* @	设置背景刷新是否刷新，以及刷新时间
*/
#define	SET_REFRESH_BACKGROUND			(0x0045)

/**
* @	恢复出厂设置
*/
#define	SET_DEFAULT_PARAM					(0x0046)

/**
* @	设置是否开始高老师预置位设置
*/
#define	SET_HIGH_PRESET					(0x0047)

/**
* @	设置丢失模式，老师丢失后跟踪模式
*/
#define	SET_LOST_MODE					(0x0048)

/**
* @ 设置老师特写区域的上边沿的Y坐标
*/
#define SET_FEATURE_VIEW_Y_START				(0x0049)



//=========================获取参数130开始====================
/**
* @	获取跟踪范围的消息字
*/
#define GET_TRACK_RANGE					(0x0100)

/**
* @	获取触发区域消息字
*/
#define GET_TRIGGER_RANGE				(0x0101)

/**
* @	获取人头像的跟踪线的高				
*/
#define	GET_TRACK_LIMIT_HEIGHT			(0x0102)

/**
* @	获取跟踪类型				
*/
#define	GET_TRACK_TYPE					(0x0103)

/**
* @	获取画线类型				
*/
#define	GET_DRAW_LINE_TYPE				(0x0104)

/**
* @	获取摄像头速度基数			
*/
#define	GET_CAM_SPEED_TYPE				(0x0105)

/**
* @	获取触发点总数			
*/
#define	GET_TRIGGER_NUM_TYPE			(0x0106)

/**
* @	获取跟踪距离			
*/
#define	GET_TRACK_DISTANCE_TYPE			(0x0107)

/**
* @	获取跟踪机是否编码的开关
*/
#define GET_TRACK_IS_ENCODE				(0x0108)

/**
* @ 导出跟踪参数消息字
*/
#define GET_TRACK_PARAM					(0x0109)

/**
* @ 获取预跟踪时间
*/
#define GET_START_TRACK_TIME			(0x010a)

/**
* @ 获取跟踪复位时间
*/
#define GET_RESET_TIME					(0x010b)

/**
* @ 获取检测变化系数sens值
*/
#define GET_SENS_VALUE					(0x010c)

/**
* @获取跟踪模式
*/
#define GET_TRACK_MODE					(0x010d)

/**
* @获取机位
*/
#define GET_STRATEGY_NO					(0x0112)



/**
* @ 获取学生全景保持时间
*/
#define GET_STUDENTS_PANORAMA_SWITCH_NEAR_TIME				(0x0114)

/**
* @获取老师走到板书1区域切板书的时间
*/
#define GET_TEACHER_BLACKBOARD_TIME1				(0x0115)

/**
* @ 获取离开板书1区域切老师的时间
*/
#define GET_TEACHER_LEAVE_BLACKBOARD_TIME1				(0x0116)

/**
* @ 获取学生下讲台后不启用跟踪的时间
*/
#define GET_STUDENTS_DOWN_TIME				(0x0117)

/**
* @ 获取老师近景摄像头移动切换老师全景的时间
*/
#define GET_TEACHER_PANORAMA_TIME				(0x0118)

/**
* @ 获取老师摄像头停止移动切换老师近景的时间
*/
#define GET_TEACHER_LEAVE_PANORAMA_TIME				(0x0119)

/**
* @ 获取学生上讲台后保持老师全景的最长时间
*/
#define GET_TEACHER_KEEP_PANORAMA_TIME				(0x011A)

/**
* @获取老师切换为学生前的老师镜头保持时间
*/
#define GET_TEACHER_SWITCH_STUDENTS_DELAY_TIME				(0x011B)

/**
* @ 获取学生近景保持的时间
*/
#define GET_STUDENTS_NEAR_KEEP_TIME				(0x011C)

/**
* @ 获取老师丢了时老师全景与学生全景切换延时
*/
#define GET_MV_KEEP_TIME				(0x011D)

/**
* @获取vga保持时间
*/
#define GET_VGA_KEEP_TIME				(0x011E)


/**
* @获取老师走到板书2区域切板书的时间
*/
#define GET_TEACHER_BLACKBOARD_TIME2				(0x011f)

/**
* @ 获取离开板书2区域切老师的时间
*/
#define GET_TEACHER_LEAVE_BLACKBOARD_TIME2				(0x0120)

/**
* @ 获取黑板触发范围
*/
#define GET_BLACKBOARD_TRIGGER			(0x0121)


/**
* @获取摄像机类型，即不同型号摄像机的选择
*/
#define	GET_CAM_TYPE					(0x0122)

/**
* @ 获取学生轮询信息
*/
#define GET_TRACK_TURN_INFO				(0x0124)

/**
* @ 获取板书方案
*/
#define GET_BLACKBOARD_SCHEME				(0x0125)

/**
* @	获取定位摄像机所在位置教室的长宽高
*/
#define	GET_POSITION_CAM_INFO			(0x0130)


/**
* @	获取跟踪摄像机所在位置的长宽高
*/
#define	GET_TRACK_CAM_INFO				(0x0131)


/**
* @	获取跟踪摄像机水平，垂直，焦距的偏移值
*/
#define	GET_TRACK_CAM_OFFSET			(0x0132)


/**
* @	获取对面定位摄像机的位置
*/
#define	GET_OTHER_CAM_POSITION			(0x0133)



/**
* @	获取跟踪机类型(是主跟踪机还是辅学生机)
*/
#define	GET_HOST_TYPE					(0x0134)

/**
* @	获取背景刷新是否刷新，以及刷新时间
*/
#define	GET_REFRESH_BACKGROUND			(0x0135)

/**
* @	获取是否开始高老师预置位设置
*/
#define	GET_HIGH_PRESET			(0x0136)

/**
* @	获取本机的IP信息
*/
#define	GET_LOCAL_IP_INFO					(0x0137)

/**
* @	获取学生机的IP信息
*/
#define	GET_STUDENT_IP_INFO					(0x0138)

/**
* @	获取板书机0的IP信息
*/
#define	GET_BLOCKBOARD_0_IP_INFO					(0x0139)

/**
* @	获取板书机1的IP信息
*/
#define	GET_BLOCKBOARD_1_IP_INFO					(0x013A)


/**
* @	获取系统网络连接信息
*/
#define	GET_NETWORK_CONNECT_STATUS					(0x013C)

/**
* @	重启设备
*/
#define	REBOOT_DEVICE					(0x013D)


#define	MSG_SET_TRACK_PARAM	0xA3	//教师跟踪参数设置

typedef enum
{
	PANORAMA_PRESET	=1,	//全景预置位	
	LOW_TEACHER_PRESET = 2,//矮老师预置位
	HIGH_TEACHER_PRESET = 3, //高老师预置位
	BIG_PANORAMA_PRESET = 6, //大全景预置位
}PRESET_POSITION;

/**
* @	老师丢了后切换模式，
0表示不跟踪，老师丢了切老师全景(全景触发)/学生全景(无全景触发)
1表示继续跟踪学生，老师丢了切学生全景
2表示继续跟踪学生，老师丢了切老师全景
*/

typedef enum
{
	LOST_NOTRACK = 0,
	LOST_TRACK = 1,
	LOST_TRACK_TEACH = 2, 
}LOSTMODE;


/**
* @	摄像机转动方向类型
*/
typedef enum _cam_direct_type
{
	CAM_DIRECT_STOP = 0,
	CAM_DIRECT_LEFT,
	CAM_DIRECT_RIGHT,
	CAM_DIRECT_NUM
}cam_direct_type_e;


typedef struct _stTeacherConnectState    //教师机连接状态   1连接   0断开
{
	int iStudent;                        //与学生连接
	int iBroad1;                         //与板书1连接
	int iBroad2;                         //与板书2连接
	int iRec;                            //与录播连接
	char cIPStudent[32];	
	char cIPBroad1[32];	
	char cIPBroad2[32];	
}ST_TEACHER_CONNECT_STATE;



/**
* @ ENC110发送数据消息头结构
*/
typedef struct _msg_header_110
{
	unsigned char	nMsg;					//消息类型
	unsigned short	nLen;					//通过htons转换的值,包括结构体本身和数据
	
} msg_header_110_t;

/**
* @	坐标点的描述 
*/
typedef struct _point_info
{
	unsigned short x;	//x坐标
	unsigned short y;	//y坐标
}point_info_t;


/**
* @ 跟踪消息头结构
*/
typedef struct _track_header
{
	unsigned short len;
	unsigned short fixd_msg;		//固定消息字
	unsigned short msg_type;		//消息类型
	unsigned short reserve;			//预留
}track_header_t;

/**
* @ 控制摄像头方式
*/
typedef enum _control_cam_mode
{
	AUTO_CONTROL = 0,		//自动控制
	MANUAL_CONTROL,			//手动控制
	MAX_NUM_CONTROL
}control_cam_mode_e;



/**
* @ 控制画线的类型
*/
typedef enum _draw_line_type
{
	DRAW_NO = 0,			//不画线
	DRAW_LINE = 1,			//划线
	MAX_NUM_DRAW
}draw_line_type_e;




/**
* @	自动或手动控制
*/
typedef struct _control_type_info
{
	unsigned char 	state;			//启用或撤销操作
	control_cam_mode_e 	control_type;	//控制类型0是自动控制,1为手动控制
}control_type_info_t;


/**
* @	跟踪框信息
*/
typedef struct _track_range_info
{
	unsigned char state;						//启用或撤销操作
	unsigned char point_num;					//确定跟踪区域的点数
	unsigned short video_width;					//视频的宽
	unsigned short video_height;				//视频的高
	point_info_t point[TRACK_RANGE_NUM];		//跟踪框
}track_range_info_t;

/**
* @	触发框信息
*/
typedef struct _trigger_range_info
{
	unsigned char state;						//启用或撤销操作
	unsigned char point_num;					//确定跟踪区域的点数
	unsigned short video_width;					//视频的宽
	unsigned short video_height;				//视频的高
	point_info_t point[TRACK_TRIGGER_NUM];		//触发框
}trigger_range_info_t;

/**
* @	设置跟踪机是否编码(协议中用的结构体)
*/
typedef struct _track_is_encode_info
{
	unsigned char 	state;			//启用或撤销操作
	unsigned short 	isencode;		//跟踪机是否编码的标志,0表示不编码,1表示编码
}track_is_encode_info_t;


/**
* @ 和编码有关的一些全局变量(保存全局变量中用到的结构体)
*/
typedef struct _track_encode_info
{
	short	is_encode;		//为0表示不编码,为1表示编码
	int		is_track;			//是否跟踪1表示跟踪,0表示不跟踪
	int		is_save_class_view;	//保存课前取景图片，1是要保存，0不需要保存
	int		students_status;	//学生站立状态，1为站立，2为多人站立，0为坐下
	int		last_students_status;	//上次学生站立状态，1为站立，2为坐下，0为不做任何操作
	int		host_type;				//主机类型，0为主学生机，1为辅学生机
	int		send_cmd;				//命令类型，是切换老师还是学生
	int		nTrackSwitchType;		//跟踪类型，0表示跟踪，1表示只切换不跟踪
	int		track_status;			//跟踪机状态0表示未跟踪上,1表示跟踪上
	int		refresh_time;			//背景刷新时间单位是秒，0表示不刷新背景
	int		record_flag;//已经开始录制标志
	int		call_position_no;//每次要调用的预置位号
	int		detect_high_teach;//检测到高老师标志，表示算法是否检测到了高老师触发
	int		highpreset;//高老师标志，表示是否启用高老师检测
}track_encode_info_t;




/**
* @	设置检测变化的系数sens值
*/
typedef struct _sens_info
{
	unsigned char 	state;				//启用或撤销操作
	unsigned short 	sens;				//sens值
}sens_info_t;


/**
* @	跟踪模式设置，是和原始图像比较还是和上帧图像比较
*/
typedef struct _track_mode_info
{
	unsigned char 	state;			//启用或撤销操作
	int16_t 	track_mode;	//控制类型0是自动控制,1为手动控制
}track_mode_info_t;

/**
* @	设置手动命令参数
*/
typedef struct _manual_commond_info
{
	unsigned char 	state;			//启用或撤销操作
	unsigned short 	type;			//手动命令中的子类型，大小为0-65535
	unsigned short 	value;			//手动命令中的值，大小为0-65535
}manual_commond_info_t;


/**
* @	画调试线的控制
*/
typedef struct _draw_line_info
{
	unsigned char 		state;		//启用或撤销操作
	draw_line_type_e 	message;	//message类型
}draw_line_info_t;

/**
* @	设置触发点总数
*/
typedef struct _trigger_num_info
{
	unsigned char 		state;		//启用或撤销操作
	unsigned short 	trigger_num;	//触发点总数,即触发区域检测到有这么多个点触发才认为是触发
}trigger_num_info_t;


/**
* @ 模版教室信息 
*/
typedef struct _model_classroom_info
{
	int 	height;					//摄像机离模版的垂直距离
	int 	width[CAM_ANGLE_NUM];	//模版取点离垂心的距离
	double	angle[CAM_ANGLE_NUM];	//摄像机与垂线的夹角
}model_classroom_info_t;


/**
* @	设置参数类型
*/
typedef enum	_set_cmd_type
{
	SET_PRESET_POSITION = 1,
	SET_LIMITE_POSITION = 2,
	GET_CUR_POSITION
}set_cmd_type_e;


/**
* @	设置预置位
*/
typedef struct _preset_position_info
{
	unsigned char 		state;		//启用或撤销操作
	unsigned short 		preset_position;//设置的预置位,范围从1到255
}preset_position_info_t;

/**
* @ 跟踪设置软件上次设置命令信息
*/
typedef struct _track_save_file_info
{
	pthread_mutex_t save_track_m;	//用于修改跟踪机配置文件时加的锁
	set_cmd_type_e		set_cmd;	//设置软件设置命令
	int		cmd_param;				//命令的详细值
}track_save_file_info_t;




/**
* @	摄像机型号类型
*/
typedef enum _cam_type_info
{
	BLM_500R		= 0,		//标准VISCA协议，博胜智达的摄像机
	SONY_BRC_Z330	= 1,		//SONY的BRC-Z330协议摄像机协议
	HITACHI_CAM 	= 2,		//日立的摄像机支持
	KXWELL_CAM		= 3,		//科旭威尔的摄像机支持
	SONY_D100		= 4,		//索尼D100
	VHD_V100N		= 5,		//迎风传讯VHD-V100N摄像机
	SONY_H100S		= 6,		//SONY EVI_H100S摄像机
	HUAWEI		= 7,		//HUAWEI摄像机
	VHD_PTZ		= 8,		//    VHD_PTZ
	SONY_HD1    = 9,
	MAX_CAMNUM,
	CAM_NUM
}cam_type_info_e;



/**
* @ 保存各个预置位的信息
*/
typedef struct _cam_preset_position_info
{
	
	unsigned char pan_tilt[CAM_PAN_TILT_LEN];	//前四个字节为摄像头水平位置,后四个字节为摄像头垂直位置
	unsigned char zoom[CAM_ZOOM_LEN];		//摄像头zoom位置,共有四个字节描述
	int cur_preset_value;		//上次执行的预置位值
}cam_preset_position_info_t;


/**
* @	跟踪有关的全局参数
*/
typedef struct _cam_control_info
{
	//int		cam_position_value;				//预置位数
	cam_preset_position_info_t	cam_position[PRESET_NUM_MAX];
	//unsigned char	cam_addr;						//摄像机地址
}cam_control_info_t;


/**
* @	设置或获取摄像机类型协议
*/
typedef struct _cam_protocol_type_info
{
	unsigned char		state;		//启用或撤销操作
	unsigned short		cam_type;	//摄像机协议类型
}cam_protocol_type_info_t;


/**
* @	设置或获取定位摄像机所在教室的长宽高(高表示定位摄像机的高)
*/
typedef struct _position_cam_info
{
	unsigned char		state;		//启用或撤销操作
	unsigned short		lenth;		//教室定位摄像机向后面教室的长度
	unsigned short		width;		//教室两定位摄像机的距离
	unsigned short		height;		//教室定位摄像机的高度
}position_cam_info_t;


/**
* @	设置或获取跟踪摄像机所在教室的长宽高(高表示定位摄像机的高)
*/
typedef struct _track_cam_info
{
	unsigned char		state;		//启用或撤销操作
	unsigned short		lenth;		//教室定位摄像机向后面教室的长度
	unsigned short		width;		//教室两定位摄像机的距离
	unsigned short		height;		//教室定位摄像机的高度
}track_cam_info_t;


/**
* @	设置或获取跟踪摄像机水平，垂直方向，焦距的偏移值
*/
typedef struct _track_cam_offset_info
{
	unsigned char		state;		//启用或撤销操作
	short		pan;		//跟踪摄像机水平方向上的角度偏移值
	short		tilt;		//跟踪摄像机垂直方向上的角度偏移值
	short		zoom;		//跟踪摄像机焦距偏移值
}track_cam_offset_info_t;


/**
* @	设置或获取学生机的IP信息
*/
typedef struct _track_ip_info
{
	unsigned char		state;		///1:启用; 0:撤销操作
	char 				ip[16];		//IP的值
	char 				mask[16];		//mast的值
	char 				route[16];		//route的值
}track_ip_info_t;


/**
* @	设置另外一个定位摄像机位置信息
*/
typedef struct _other_position_cam_info
{
	unsigned char		state;	//启用或撤销操作
	unsigned short		x;		//另外一个定位摄像机在本摄相机中德x坐标
	unsigned short		y;		//另外一个定位摄像机在本摄相机中德y坐标
	unsigned short		width;	//另外一个定位摄像机在本摄相机中的宽
	unsigned short		height;	//另外一个定位摄像机在本摄相机中的高
}other_position_cam_info_t;


/**
* @	设置主机类型
*/
typedef struct _track_host_type
{
	unsigned char		state;	//启用或撤销操作
	unsigned short		host_type;		//主机类型0为主学生机，1为辅学生机
}track_host_type_t;


/**
* @ 定义速度级别
*/
typedef enum _cam_speed_level
{
	SPEED_LEVEL_0 = 0,
	SPEED_LEVEL_1 = 1,
	SPEED_LEVEL_2 = 2,
	SPEED_LEVEL_3 = 3,
	SPEED_LEVEL_4 = 4,
	SPEED_LEVEL_5 = 5,
	SPEED_LEVEL_NUM
}cam_speed_level_t;

/**
* @	左右上下限位的的分类
*/
typedef enum _limit_position_type
{
	LIMIT_DOWN_LEFT = 0,
	LIMIT_UP_RIGHT,	// 1
	LIMIT_CLEAR, // 2
	LIMIT_KXWELL_UP,  // 3
	LIMIT_KXWELL_DOWN,// 4
	LIMIT_KXWELL_LEFT,// 5
	LIMIT_KXWELL_RIGHT //6
}limit_position_type_e;



/**
* @	存放计算角度结果的信息,所有宽度，高度，长度，都是以主定位摄像机在地面
* @  在地面上的映射为0点的
*/
typedef struct _angle_info
{
	int		cam_addr;				//跟踪摄像机的地址位
	cam_type_info_e		cam_type;	//摄像机类型
	unsigned short cam_speed;		//摄像头转动速度
	int		last_cam_cmd;			//上次向摄像机发送的命令
	int		last_cam_speed;			//上次向摄像机发送的速度
	int		track_cam_cur_pan;		//跟踪摄像机当前水平方向位置值
	int		other_cam_x;			//另外一个摄像机所在图像中的x值
	int		other_cam_y;			//另外一个摄像机所在图像中的y值
	int		other_cam_width;		//另外一个摄像机所在图像中的宽
	int		other_cam_height;		//另外一个摄像机所在图像中的高
	int		host_status;			//主学生机状态
	int		aux_status;				//辅学生机状态
	int		last_host_status;		//主学生机上次状态
	int		last_aux_status;		//辅学生机上次状态
	int		position_camera_width;	//教室宽度(即x坐标)
	int		position_camera_lenth;	//教室长度(即y坐标)
	int		position_camera_height;	//定位摄像机高度(即z坐标)
	int		track_camera_width;		//跟踪摄像机的宽度(即x坐标)
	int		track_camera_lenth;		//跟踪摄像机的长度(即y坐标)
	int		track_camera_height;	//跟踪摄像机高度(即z坐标)
	double	camera_pan_offset;		//摄像机水平方向的偏移角度
	double	camera_tilt_offset;		//摄像机垂直方向的偏移
	int		camera_zoom_offset;		//摄像机焦距方向的偏移
	double 	host_angle;			//主学生机角度
	double	host_base_angle;	//辅摄像机在主摄像机图像中的位置与主摄像机连线所在摄像机的角度
	double  aux_angle;			//辅学生机角度
	double	aux_base_angle;		//主摄像机在辅摄像机图像中的位置与辅摄像机连线所在摄像机的角度
}angle_info_t;

/**
* @	与kite通讯中描述板书框的信息
*/
typedef struct _broadcast_range_info
{
	unsigned char state;						//启用或撤销操作
	unsigned char point_num;					//确定跟踪区域的点数
	unsigned short video_width;					//视频的宽
	unsigned short video_height;				//视频的高
	point_info_t point[TRACK_BROADCAST_POINT_NUM];		//触发框
}broadcast_range_info_t;

/**
* @	板书框的信息
*/
typedef struct _broadcast_info
{
	point_info_t point[ONE_BROADCAST_POINT_NUM];				
}broadcast_info_t;

/**
* @	保存和跟踪策略有关的参数
*/
typedef struct _track_tactics_info
{
	broadcast_info_t	broadcast_info[TRACK_BROADCAST_NUM];
	broadcast_info_t	teacher_feature_info[TRACK_BROADCAST_NUM];
}track_tactics_info_t;


/**
* @	用于设置黑板区域，以及获取时间值等通用结构体
*/
typedef struct _default_msg_info
{
	unsigned char 	state;		//启用或撤销操作
	unsigned short 	value;	
}default_msg_info_t;


/**
* @ 课堂信息统计结构体.上报给web的信息结构体
*/
typedef struct _send_class_info
{
	unsigned short  nClassType;//授课类型
	unsigned short  nUpTimesSum;//起立总次数
	unsigned short	nStandupPos[4];			//前面左  右，  后面左右分别对应起立次数
	unsigned short	nUpToPlatformTimes;			//学生上台总次数
	unsigned short	nTeacherToStudentsAreaTimes;//老师下台总次数
	unsigned short  nPerTimeLen;//每个时间段长度
	unsigned short 	nAllTimesNum;//总共的时间断数
	unsigned short  nInteractionNum[MAX_INTERACTION_NUM];//分别是各个时间段的活跃次数
	unsigned short 	nStandupTimePoint[MAX_STANDUP_NUM];//分别是各个起立动作的起立时间点
}send_class_info_t;

/**
* @ 课堂信息统计结构体.上报给web的信息结构体
*/
typedef struct _final_class_info
{
	send_class_info_t tSendClassInfo;
	unsigned int nClassInfoFlag;
	unsigned long long nClassStartTime;
	unsigned long long nClassEndTime;
}final_class_info_t;


/**
* @	设置摄像头速度基数
*/
typedef struct _cam_speed_info
{
	unsigned char 		state;		//启用或撤销操作
	unsigned short 	cam_speed;		//摄像头速度基数,即用要移动的距离除以此值,则为摄像头速度
}cam_speed_info_t;


/**
* @	设置限位开关
*/
typedef struct _limit_position_info
{
	unsigned char 			state;			//启用或撤销操作
	//limit_position_type_e 	limit_position;	//设置摄像头的上下左右的最大移动位置
	unsigned short 	limit_position;	//设置摄像头的上下左右的最大移动位置
}limit_position_info_t;



/**
* @	切换命令发起者类型
*/
typedef enum _switch_cmd_author_info
{//这里学生不能随便修改，只能从后面累加学生，且值需要连续累加，详见ProcessMsgTrackAuto 函数用法
	AUTHOR_TEACHER	= 1,	//切换命令是老师发起的
	AUTHOR_STUDENTS	= 2,	//是学生机发送的切换命令
	AUTHOR_NUM
}switch_cmd_author_info_e;


/**
* @	多机位策略需求信息表
*/
typedef struct _track_strategy_info
{
	int	left_pan_tilt1;	//板书1左边	//前四个或者前五个字节为摄像头水平位置,后四个字节为摄像头垂直位置，表示黑板最左边对应摄像机位置
	int	right_pan_tilt1;//板书1右边		//前四个或者前五个字节为摄像头水平位置,后四个字节为摄像头垂直位置，表示黑板最右边对应摄像机位置
	int	left_pan_tilt2;	//板书2左边	//前四个或者前五个字节为摄像头水平位置,后四个字节为摄像头垂直位置，表示黑板最左边对应摄像机位置
	int	right_pan_tilt2;//板书2右边		//前四个或者前五个字节为摄像头水平位置,后四个字节为摄像头垂直位置，表示黑板最右边对应摄像机位置
	int cur_pan_tilt;		//前四个或者前五个字节为摄像头水平位置,后四个字节为摄像头垂直位置，表示摄像机当前所处的位置
	int	blackboard_region_flag1;//是否在板书区域标志，1为在板书区域，0为不在板书区域
	int	blackboard_region_flag2;//是否在板书区域标志，1为在板书区域，0为不在板书区域
	int	students_panorama_switch_near_time;		//学生站起来后，切学生全景摄像机，再切学生特写摄像机的时间，单位是秒
	int	teacher_blackboard_time1;				//老师停留在黑板区域的时间大于这个时间即认为是老师在黑板区域，单位是秒
	int	teacher_leave_blackboard_time1;			//老师离开黑板区域的时间大于这个时间即认为是老师不在黑板区域，单位是秒
	int	teacher_blackboard_time2;				//老师停留在黑板区域的时间大于这个时间即认为是老师在黑板区域，单位是秒
	int	teacher_leave_blackboard_time2;			//老师离开黑板区域的时间大于这个时间即认为是老师不在黑板区域，单位是秒
	int	teacher_panorama_time;					//老师移动后多长时间切换老师全景，单位是秒
	int	teacher_leave_panorama_time;			//老师停下来多久后切换到老师特写，单位是秒
	int	teacher_keep_panorama_time;				//老师全景镜头最大保留时间，单位是秒
	int	move_flag;								//移动标志,老师跟踪机摄像头移动标志
	int	send_cmd;		//老师机当前要发送的命令
	int	last_send_cmd;		//上一次老师机发送的命令
	switch_cmd_author_info_e	switch_cmd_author;			//切换命令发起者是老师机还是学生机
	int teacher_panorama_flag;//老师全景,表示学生上讲台了,多目标检测时用
	int	students_down_time;			//学生下讲台时间大于这个时间即再次接收学生起立信息，单位是秒
	int	students_track_flag;			//是否接收学生起立以及上台信息标志，单位是秒
	int switch_vga_flag;//是否切换vga
	int teacher_switch_students_delay_time;//老师镜头切换到学生镜头延时
	int	students_near_keep_time;			//学生近景镜头保持时间
	int	vga_keep_time;//vga 首画面保持时间,超过这个时间,就由vga镜头切换到别的镜头
	int cam_left_limit;//左限位
	int cam_right_limit;//右限位
	int position1_mv_flag;//发现老师丢了后有移动目标
	int mv_keep_time;//老师丢了，发现移动目标时学生全景和老师全景镜头切换延时
	int lostmodestutime;//老师丢了，学生全景保持时间
	int strategy_no;//机位数，目前分两类，3机位和4\5\6机位
	int cam_jump_flag;// 1表示调了预置位1 (老师全景) ,2表示调用了老师近景，0表示没有任何调用，用于屏蔽触发时老师的拉升镜头，直接等老师跟踪上以后再切老师
	int stu_num;//链接的学生机个数
	int lostmode;//老师丢失后跟踪模式
	int	vga_keep_time_toserver;//vga 保持时间,发送给server，
	int blackboard_scheme;//板书跟踪方案
	int blackboard_num;//板书机跟踪数量，0表示无板书机(老师机做板书跟踪)，1表示有板书机
	track_ip_info_t student_ip_info;	//学生机的ip信息
	track_ip_info_t local_ip_info;	//老师机本机的ip信息
	track_ip_info_t blackboard_ip_info[BLACKCONNECT_NUM_MAX];	//板书机的ip信息

	int	teacher_detect_heart_timeout;		//老师检测模块与策略模块心跳的超时时间
}track_strategy_info_t;

/**
* @	多机位策略需求信息表,用于统计 
*/
typedef struct _track_strategy_timeinfo
{
	int	last_strategy_no;//上一次的机位号,如果跟当前不一样会做相关初始化变量操作
	int	students_panorama_switch_near_time;//学生全景切换学生前的累积时间,这个时间如果超过策略的延时就切学生近景
	int	blackboard_time1;//老师走入板书区域1的时间,如果只有一个板书区域,则板书区域1生效
	int	leave_blackboard_time1;//老师离开板书区域1的时间,如果只有一个板书区域,则板书区域1生效
	int	blackboard_time2;//老师走入板书2区域时间
	int	leave_blackboard_time2;//老师离开板书2区域时间
	int	panorama_time;//5机位,老师全景时间
	int	leave_panorama_time;//5机位,离开老师全景时间
	
	int	teacher_panorama_switch_near_time;//老师全景切换到老师近景之前的时间,时间统计到了策略设置的时间就切换老师近景
	int teacher_panorama_flag;//当前镜头是否是老师全景
	int	students_down_time;//学生下讲台的时间统计
	int	students_to_platform_time;//学生上讲台的时间统计
	int	switch_flag;// 1可以切换,0 延时未到 不可以切换
	int	switch_delaytime;//标准延时时间,3s
	int	delaytime;//每次切换的延时时间,根据不同切换镜头,得到的延时时间也不一致
	int	on_teacher_flag;//当前镜头是否在老师上,包括老师和板书和老师全景,用于老师切学生时的延时处理
	int	on_teacher_delaytime;//老师镜头的保持时间
	int	vga_delaytime;//vga的时间
	int	mv_time;//老师丢失时,发现运动目标的时间
	int	stu_time;//老师丢失时,切换到学生全景的时间
	int stu_state[STUCONNECT_NUM_MAX];//多个学生机的当前状态 ，有多个学生时才有用到
	unsigned long long stu_uptime[STUCONNECT_NUM_MAX];//多个学生机检测到起立时间，最后一次起立时间
	int nStuStandFlag;//学生起立了，用于老师丢失后的学生跟踪

}track_strategy_timeinfo_t;


/**
* @ 轮询相关结构体
*/
typedef struct _track_turn_info
{
	unsigned short 	nTrackTurnType;
	unsigned short 	nTrackTurnTime;
	unsigned short 	nTrackTurnShowTime;
}track_turn_info_t;


/**
* @	学生机轮询信息
*/
typedef struct _stu_turn_info
{
	unsigned char 	state;		//启用或撤销操作
	track_turn_info_t 	tStuTrackTurnInfo[STUCONNECT_NUM_MAX];	
}stu_turn_info_t;


/**
* @	目标个数变化确认时间信息
*/
typedef struct _target_change_time_info
{
	unsigned int	targettime0;		//目标丢失时间，单位是帧
	unsigned int	targettime1;		//有一个目标的时间，单位是帧
	unsigned int	targettime2;		//有多个目标的时间，单位是帧
}target_change_time_info_t;


//#define		TRIGGER_NUM				(150)		//一个检测周期内把所有周期分的段数
#define		TRIGGER_POINT_NUM		(30)		//一个刷新周期内总共检测到的触发框数
//#define		DETECTION_SECTION_NUM	(50)		//多少帧为一个检测段
#define     DETECTION_TARGET    (100)        //多少个目标
#define 	DETECTION_FRAMES_SECTION (20)       //每段多少帧
#define 	DETECTION_SECTION_NUM (400)
typedef struct _target
{
	int x0;
	int y0;
	int x1;
	int y1;
	int trigger_flag[DETECTION_SECTION_NUM];
 
}Target;



/**
* @	设置刷新背景信息
*/
typedef struct _nomal_info
{
	
	unsigned char 			state;			//启用或撤销操作
	unsigned short 			val0;		//0表示关闭，1表示开启
	unsigned short			val1;	//刷新时间，单位是秒
	unsigned short			val2;			//备用
	unsigned short			val3;			//备用
}nomal_info_t;

/**
* @	设置刷新背景信息
*/
typedef struct _refresh_background_info
{
	
	unsigned char 			state;			//启用或撤销操作
	unsigned short 			is_open;		//0表示关闭，1表示开启
	unsigned short			refresh_time;	//刷新时间，单位是秒
	unsigned short			base1;			//备用
	unsigned short			base2;			//备用
}refresh_background_info_t;

/**
* @ 点的坐标信息
*/
typedef struct _trigger_point_info
{
	int		x0;
	int		y0;
	int		x1;
	int		y1;
	int		trigger_flag;		//本点的触发标志
}trigger_point_info_t;

/**
* @	像素点描述
*/
typedef struct _pixel_point_info
{	
	int16_t	x;		//x坐标
	int16_t	y;		//y坐标
}vertex_t;

/**
* @	触发框信息的结构体
*/
typedef struct _trigger_info
{
	int16_t	trigger_x0;		//表示触发框的左上顶点坐标的x值(以源图像为基准的)
	int16_t	trigger_y0;		//表示触发框的左上顶点坐标的y值(以源图像为基准的)
	int16_t	trigger_x1;		//表示触发框的右下顶点坐标的x值(以源图像为基准的)
	int16_t	trigger_y1;		//表示触发框的右下顶点坐标的y值(以源图像为基准的)
}trigger_info_t;


/**
* @	调用control时是否需要重新初始化参数类型
*/
typedef enum _control_init_type
{
	NO_INIT = 0,		//无需调用control
	RE_INIT,			//需要初始化部分参数
	RE_START,			//需要初始化所有参数
	RE_INIT_NUM
}control_init_type_e;

/**
* @ 存放自动跟踪的静态参数
*/
typedef struct 	_track_students_right_side_static_param
{
	int32_t	size;
	int16_t	video_width;			//输入视频的宽
	int16_t	video_height;			//输入视频的高
	int16_t	pic_width;				//输入缩放后图像的宽
	int16_t	pic_height;				//输入缩放后的图像的高
}ITRACK_STUDENTS_RIGHT_SIDE_static_param_t;

/**
* @	跟踪区域点设置的最大点数
*/
#define	TRACK_STUDENTS_RIGHT_SIDE_AREA_POINT_MAX	(10)


/**
* @	跟踪框最大个数
*/
#define	TRACK_STUDENTS_RIGHT_SIDE_TRIGGER_MAX_NUM 	(2)	

/**
* @ 存放自动跟踪的动态参数
*/
typedef struct 	_track_students_right_side_dynamic_param
{
	int32_t	size;
	int16_t	trigger_sum;		//触发点总数
	int16_t	track_mode;			//跟踪模式，0表示和采集好的原始帧比较，1表示和上帧比较看是否有变化
	int16_t	start_track_time;	//阀值时间，触发后多长时间开始跟踪,单位是帧
	int16_t	reset_time;			//跟踪丢失后到下次开始触发的时间
	int16_t	control_mode;	//控制类型,0为自动,1为手动
	int16_t	sens;			//
	int16_t	message;		//主要用来设定画那些线，比如是否画人得轮廓线等,1:显示所有边缘点状态；
							//2:符合45度，90度，135度，180度的线画出来；3:画模版;4:画跟踪框;5:画跟踪框内的肤色点
	int16_t	track_point_num;//跟踪框的设置，最大有几个点，(可由这些点来连接成框，组成了跟踪区域)
	vertex_t	track_point[TRACK_STUDENTS_RIGHT_SIDE_AREA_POINT_MAX];	//跟踪区域点的x，y坐标描述,是相对于缩放之前图像所说
	trigger_info_t trigger[TRACK_STUDENTS_RIGHT_SIDE_TRIGGER_MAX_NUM];
	control_init_type_e	reset_level;	//0表示不要重新初始话,1表示需要重新初始化
	int16_t classroom_study_mode;	//教室学习模式，0表示主讲模式；1表示听讲模式
	int16_t student_enable_state;	//学生屏蔽标志，1 开启 0 关闭
}ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t;

/**
* @ 存放自动跟踪的输入参数
*/
typedef struct 	ITRACK_STUDENTS_RIGHT_SIDE_Params
{
	int32_t	size;
	ITRACK_STUDENTS_RIGHT_SIDE_static_param_t 	static_param;
	ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t	dynamic_param;
}ITRACK_STUDENTS_RIGHT_SIDE_Params;

int track_students_right_side_param(unsigned char *data, int socket);


extern trigger_point_info_t	g_trigger_info[TRIGGER_POINT_NUM];
extern unsigned long long 			g_refresh_start_time;



extern int init_dev_type(void);
//extern int classroom_location_calculation(ITRACK_STUDENTS_RIGHT_SIDE_OutArgs *output_param);
extern int jump_appoint_position(int cam_addr, int cam_pan, int cam_tilt, int fd);
extern int jump_appoint_zoom(int cam_addr, int zoom, int fd);
extern int set_cam_addr();
extern int save_position_zoom(unsigned char *data);
extern int save_position_zoom_with_network_data(int ePresetType, unsigned char *data);
extern int __get_cam_position(int fd);
extern int get_strategy_info_form_file(void);
extern int init_limit_position_set(void);
extern int teacher_target_validaation(int target_num, int x,int is_trigger,rect_t *rect);
extern int server_set_track_type(short type);
extern int server_set_classroom_study_mode(short mode);
#endif

