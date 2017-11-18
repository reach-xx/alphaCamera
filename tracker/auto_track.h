
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
#include "itrack_students.h"
/**
* @	文件名称的长度
*/
#define FILE_NAME_LEN		(64)
/**
* @ 学生跟踪参数保存文件
*/
#define STUDENTS_TRACK_FILE		"students_track.ini"

#define DEBUG_INFO "debug_info"
#define DEBUG_IPADDR "debug_ipaddr"
#define DEBUG_LEVEL_VAL	"debug_level_val"

/**
* @ 教师跟踪的动态参数类型名称
*/
#define DYNAMIC_NAME			"dynamic"

/**
* @  支持索尼BRC系列的摄像机的VISCA协议
*/
//#define SUPORT_SONY_BRC_CAM


/**
* @	预置位的最大个数
*/
#define PRESET_NUM_MAX					(4)


/**
* @	固定消息字
*/
#define FIXED_MSG						(0xFFF0)

//--------------------------设定参数消息字------------------------

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


#define SET_CAM_ADDR                    (0x004d)

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
* @ 设置屏蔽区域
*/
#define SET_SHIELD_RANGE				(0x0011)

/**
* @ 设置手动命令
*/
#define SET_MANUAL_COMMOND				(0x0012)

/**
* @ 设置多目标上台检测框
*/
#define SET_MULTITARGET_RANGE				(0x0013)


/**
* @ 设置摄像头协议类型
*/
#define SET_CAM_TYPE				(0x0028)

/**
* @	设置触发区域的最宽的宽度			
*/
#define SET_TRACK_CAM_OFFSET   (0x0042)

/**
* @	恢复出厂设置
*/
#define	SET_DEFAULT_PARAM					(0x0046)


//-----------------------------返回参数消息字-------------------------

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
* @ 获取屏蔽区域
*/
#define GET_SHIELD_RANGE				(0x010d)
/**
* @ 获取多目标上台检测框
*/
#define GET_MULTITARGET_RANGE				(0x010e)


/**
* @ 获取摄像机协议
*/
#define GET_CAM_TYPE					(0x0122)

/**
* @	获取定位摄像机所在位置教室的长宽高
*/
#define	GET_POSITION_CAM_INFO			(0x0130)


/**
* @	获取跟踪摄像机水平，垂直，焦距的偏移值
*/
#define	GET_TRACK_CAM_OFFSET			(0x0132)


//======================================================


/**
* @ 重启设备
*/
#define REBOOT_DEVICE     (0x013D)



//-------------------设置手动命令的子类型---------------------
typedef enum manualMANUAL_COMMAND_ENUM
{
/**
* @	设置是否推拉学生摄像机近景类型
*/
	SET_IS_CONTROL_CAM	=	0x01,


/**
* @	设置预制位时调用跳转到绝对焦距和绝对水平，垂直位置时的时间间隔设置
* @  由于索尼brc-z330摄像机在调完一条命令后必须等待执行完成后，才能发第二条命令，
* @  否则第二条命令将不会执行。
*/
	SET_ZOOM_PAN_DELAY	=	0x02,

/**
* @	设置设置黑板左边区域和右边区域的命令，参数值为0表示左边，为1表示右边 
*/
	SET_BLACKBOARD_REGION	= 0x03,
	
/**
* @	设置debug level，参数值为对应的level 值
*/
	SET_DEBUG_LEVEL	= 0x04
	
}manualMANUAL_COMMAND_e;


#define	PUSH_CLOSE_RANGE		(1)		//推近景
#define NOT_PUSH_CLOSE_RANGE	(0)		//不推近景

/**
* @	确定跟踪框点的个数
*/
#define TRACK_RANGE_NUM					(10)

/**
* @	确定触发框点的个数
*/
#define TRACK_TRIGGER_NUM				(50)

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
* @ 设置或获取跟踪摄像机水平，垂直方向，焦距的偏移值
*/
typedef struct _track_cam_offset_info
{
 unsigned char  state;  //启用或撤销操作
 short  x_offset;  //跟踪摄像机水平方向上的角度偏移值
 short  y_offset;  //跟踪摄像机垂直方向上的角度偏移值
 short  row_width;  //教室宽度
}track_cam_offset_info_t;



/**
* @ 控制画线的类型
*/
typedef enum _draw_line_type
{
	DRAW_NO = 0,			//不画线
	DRAW_TRIGGER_TRACK,		//表示画跟踪框和触发框,限高线,和跟踪区域框
	DRAW_SLANT_LINE,		//画斜线表示画45度,135度,90度,180度线
	DRAW_MODEL,				//画模版
	DRAW_SKIN_COLOR,		//画检测区域的头像的肤色部分
	DRAW_TRACK_TRAJECTORY,	//画跟踪轨迹线
	DRAW_TRACK_DIRECT,		//画跟踪方向线
	DRAW_TRACK_SPEED,		//画跟踪速度线
	MAX_NUM_DRAW
}draw_line_type_e;

/**
* @	左右上下限位的的分类
*/
typedef enum _limit_position_type
{
	LIMIT_DOWN_LEFT = 0,
	LIMIT_UP_RIGHT,	
	LIMIT_CLEAR
}limit_position_type_e;


/**
* @ 发送数据消息头结构
*/
typedef struct _msg_header
{
	unsigned short	nLen;					//通过htons转换的值,包括结构体本身和数据
	unsigned short	nVer;					//版本号(暂不用)
	unsigned char	nMsg;					//消息类型
	unsigned char	szTemp[3];				//保留字节
} msg_header_t;

/**
* @ ENC110发送数据消息头结构
*/
typedef struct _msg_header_110
{
	unsigned char	nMsg;					//消息类型
	unsigned short	nLen;					//通过htons转换的值,包括结构体本身和数据
	
} msg_header_110_t;



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
* @	坐标点的描述 
*/
typedef struct _point_info
{
	unsigned short x;	//x坐标
	unsigned short y;	//y坐标
}point_info_t;

/**
* @	框的描述 
*/
typedef struct _rectangle_info
{
	unsigned short x;	//x坐标
	unsigned short y;	//y坐标
	unsigned short width;	//框的高
	unsigned short height;	//框的宽
}rectangle_info_t;

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
	unsigned char state;							//启用或撤销操作
	unsigned char point_num;						//确定触发区域的点数
	unsigned short video_width;						//视频的宽
	unsigned short video_height;					//视频的高
	rectangle_info_t rectangle[TRACK_TRIGGER_NUM];		//跟踪框
}trigger_range_info_t;

/**
* @	限高信息,在目标的y值大于此值时不在跟踪(即目标太矮)
*/
typedef struct _limit_height_info
{
	unsigned char 	state;			//启用或撤销操作
	unsigned short 	limit_height;	//限高高度
}limit_height_info_t;

/**
* @	自动或手动控制
*/
typedef struct _control_type_info
{
	unsigned char 	state;			//启用或撤销操作
	control_cam_mode_e 	control_type;	//控制类型0是自动控制,1为手动控制
}control_type_info_t;

/**
* @	画调试线的控制
*/
typedef struct _draw_line_info
{
	unsigned char 		state;		//启用或撤销操作
	draw_line_type_e 	message;	//message类型
}draw_line_info_t;

/**
* @	设置摄像头速度基数
*/
typedef struct _cam_speed_info
{
	unsigned char 		state;		//启用或撤销操作
	unsigned short 	cam_speed;		//摄像头速度基数,即用要移动的距离除以此值,则为摄像头速度
}cam_speed_info_t;

/**
* @	设置触发点总数
*/
typedef struct _trigger_num_info
{
	unsigned char 		state;		//启用或撤销操作
	unsigned short 	trigger_num;	//触发点总数,即触发区域检测到有这么多个点触发才认为是触发
}trigger_num_info_t;

/**
* @	设置预置位
*/
typedef struct _preset_position_info
{
	unsigned char 		state;		//启用或撤销操作
	unsigned short 		preset_position;//设置的预置位,范围从1到255
	unsigned short 		position_time;//设置的预置位,范围从1到255
}preset_position_info_t;

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
* @	设置跟踪距离
*/
typedef struct _track_distance_info
{
	unsigned char 			state;			//启用或撤销操作
	unsigned short 	zoom_distance;	//近景的焦距远近选择,值越大,焦距拉的越远,即0为近距离跟踪,1为远距离跟踪
}track_distance_info_t;


/**
* @	设置跟踪机是否编码(协议中用的结构体)
*/
typedef struct _track_is_encode_info
{
	unsigned char 	state;			//启用或撤销操作
	unsigned short 	isencode;		//跟踪机是否编码的标志,0表示不编码,1表示编码
}track_is_encode_info_t;

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
* @ 保存各个预置位的信息
*/
/**
* @ 摄像机绝对位置所占的字节数普通VISCA占用9个字节，索尼BRC-Z330占用8个字节
*/
#define CAM_PAN_TILT_LEN (9)
	
/**
* @ 摄像机绝对焦距所占字节数为4个字节
*/
#define CAM_ZOOM_LEN  (4)

typedef struct _cam_preset_position_info
{
	
	unsigned char pan_tilt[CAM_PAN_TILT_LEN];	//前四个字节为摄像头水平位置,后四个字节为摄像头垂直位置
	unsigned char zoom[CAM_ZOOM_LEN];		//摄像头zoom位置,共有四个字节描述
	int cur_preset_value;		//上次执行的预置位值
	//unsigned short 		position_time;//设置的预置位,范围从1到255
}cam_preset_position_info_t;


//调用云台预置位请求的消息结构,与学生机交互用
typedef struct __call_cam_preset_teacher_and_student_req
{
 cam_preset_position_info_t cCamCoordInfo;
 
}call_cam_preset_teacher_and_student_req;


/**
* @	设置触发后开始跟踪时间
*/
typedef struct _start_track_time_info
{
	unsigned char 	state;				//启用或撤销操作
	unsigned short 	start_track_time;	//开始跟踪时间
}start_track_time_info_t;

/**
* @	设置设置丢失目标后重新开始触发的时间
*/
typedef struct _reset_time_info
{
	unsigned char 	state;				//启用或撤销操作
	unsigned short 	reset_time;			//复位时间
}reset_time_info_t;

/**
* @	设置检测变化的系数sens值
*/
typedef struct _sens_info
{
	unsigned char 	state;				//启用或撤销操作
	unsigned short 	sens;				//sens值
}sens_info_t;

/**
* @	设置检测变化的系数sens值
*/
typedef struct _row_info
{
	unsigned char 	state;				//启用或撤销操作
	unsigned short 	row;				//row值
}row_info_t;

/**
* @	设置cam type
*/
typedef struct _cam_type
{
	unsigned char 	state;				//启用或撤销操作
	unsigned short 	type;				//sens值
}cam_type_info_t;
/**
* @	设置track_switch_type
*/
typedef struct _track_switch_type
{
	unsigned char 	state;				//启用或撤销操作
	unsigned short 	type;				//sens值
}track_switch_type_info_t;

/**
* @	屏蔽框信息
*/
typedef struct _shield_range_info
{
	unsigned char state;							//启用或撤销操作
	unsigned char point_num;						//确定触发区域的点数
	unsigned short video_width;						//视频的宽
	unsigned short video_height;					//视频的高
	rectangle_info_t rectangle[STUDENTS_SHIELD_NUM];		//跟踪框
}shield_range_info_t;
/**
* @	多目标上台检测框信息
*/
typedef struct _multitarget_range_info
{
	unsigned char state;							//启用或撤销操作
	unsigned char point_num;						//确定触发区域的点数
	unsigned short video_width;						//视频的宽
	unsigned short video_height;					//视频的高
	rectangle_info_t rectangle[STUDENTS_MULTITARGET_NUM];		//跟踪框
}multitarget_range_info_t;
/*
typedef struct _cam_coord_info
{
	int iX;     //横向坐标
	int iY;     //纵向坐标
	int iFocalDistance;  //焦距
}cam_coord_info_t;
*/
typedef struct _track_class_info
{
	unsigned short nStandupPos[4];   //前面左  右，  后面左右分别对应起立次数
	unsigned short nUpToPlatformTimes;   //学生上台总次数
	unsigned short nDownToStudentsAreaTimes;//下台总次数

	int iStudentStandupNum;     //起立学生的人数；0表示未检测到学生起立；1表示检测到一个学生起立；2表示检测到多个目标起立
	cam_preset_position_info_t cStudentCamCoord;  //学生机云台摄像机坐标
}track_class_info_t;


/**
* @	移动摄像头类型
*/
typedef enum _move_cam_type
{
	MOVE_NOT = 0,		//不移动
	MOVE_ZOOM,			//移动焦距
	MOVE_PAN_TILT,		//移动位置
	MOVE_NUM
}move_cam_type_e;

/**
* @	跟踪有关的全局参数
*/
typedef struct _cam_control_info
{
	int		cam_last_speed;					//摄像头上次的移动速度
	//camera_move_direction_e		cam_last_direction;	//摄像头上次移动方向
	unsigned short cam_speed;				//摄像头转动速度基数
	int		cam_position_value;				//预置位数
	move_cam_type_e control_flag;			//需要移动zoom为1,需要移动位置为2,都需要为3
	cam_preset_position_info_t	cam_position[PRESET_NUM_MAX];
	unsigned char	cam_addr;						//摄像机地址
	int		cam_position_lastvalue;				//预置位数
}cam_control_info_t;

//查询云台预置位请求的消息结构,与学生机交互用
typedef struct __cam_preset_teacher_and_student_req
{
 int ePresetType;
 
}cam_preset_teacher_and_student_req;


//查询云台预置位的响应，与学生机交互用
typedef struct __cam_preset_teacher_and_student_ack
{
 int ePresetType;
 cam_preset_position_info_t cCamCoordInfo;

}cam_preset_teacher_and_student_ack;


/**
* @ 和编码有关的一些全局变量(保存全局变量中用到的结构体)
*/
typedef struct _track_encode_info
{
	short	is_encode;			//为0表示不编码,为1表示编码
	short	server_cmd;			//向服务器发送的命令,表示现在是切换学生机还老师机
	int		track_status;		//跟踪机状态0表示未跟踪上,1表示跟踪上
	int		send_cmd;			//老师机要发送的命令
	short	last_position_no;	//上次预置位号
	int		is_track;			//是否跟踪1表示跟踪,0表示不跟踪
	unsigned short is_control_cam;//是否控制摄像机推近景，1为推近景，0为不推近景
	int		zoom_pan_delay;	//跳转到绝对位置时，zoom和调上下左右位置时中间的间隔单位是ms
	int		last_send_cmd;			//老师机上一次发送的命令
	int 	nTriggerValDelay;
	int 	nOnlyRightSideUpDelay;
	int 	nLastTriggerVal;
	int		nStandUpPos;
	int		nTrackSwitchType;
}track_encode_info_t;

/**
* @ 设置或获取学生机的IP信息
*/
typedef struct _track_ip_info
{
 unsigned char  state;  ///1:启用; 0:撤销操作
 char     ip[16];  //IP的值
 char     mask[16];
 char    route[16];
}track_ip_info_t;


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
* @	摄像机型号类型
*/
typedef enum _cam_type_info
{
	VISCA_STANDARD 	= 0,		//标准VISCA协议，包括博胜智达的摄像机
	SONY_BRC_Z330  	= 1, 		//SONY的BRC-Z330协议摄像机协议
	HITACHI_CAM 	= 2,		//日立的摄像机支持
	KXWELL_CAM  	= 3,		//科旭威尔的摄像机支持
	CAM_NUM
}cam_type_info_e;

/**
* @ 摄像机型号选择 
*/
typedef struct _track_cam_model_info
{
	cam_type_info_e		cam_type;
	
}track_cam_model_info_t;

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
* @	设置参数类型
*/
typedef enum	_set_cmd_type
{
	SET_PRESET_POSITION = 1,
	SET_LIMITE_POSITION = 2	
}set_cmd_type_e;

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
* @ 学生辅助机上报的消息结构体
*/
typedef struct _rightside_trigger_info
{
	unsigned int	nTriggerType;//课前取景0/课中取景1			
	unsigned int	nTriggerVal;//1表示有触发，2表示坐下(只有课前取景有用)，0不处理
}rightside_trigger_info_t;




extern int set_cam_addr();
extern int track_init(StuITRACK_Params *track_param);
extern int cam_ctrl_cmd(StuITRACK_OutArgs *output_param,unsigned char* posCmdOutput);
extern int cam_ctrl_cmd_rightside(StuITRACK_OutArgs *output_param);
extern int set_students_track_param(unsigned char *data, int socket);
extern int write_track_static_file(stutrack_static_param_t *static_param);
extern int server_set_track_type(short type);
extern int save_position_zoom(unsigned char *data);
extern int init_save_track_mutex(void);
extern int destroy_save_track_mutex(void);
extern int __call_preset_position(short position);
extern void CamCallPosition(void *pParam);
extern int student_init_params();


static int8_t active = 0;//是否发送




#endif

