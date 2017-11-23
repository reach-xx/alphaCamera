#ifndef	_MOTION_DETECT_H_
#define	_MOTION_DETECT_H_

#include <stdio.h>
#include <math.h>
#include <stdarg.h>

#include <stdint.h>  




#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#ifdef __cplusplus
}
#endif

//#define STANDARD_USB_CAPUTURE		//定义了就是使用标准的USB协议的摄像头采集，没定义就使用公司的采集板卡采集


//#define MINRECTFILLSIZE (20)
#define MINRECTWIDTH 	(30)//(10)
#define MINRECTHEIGHT 	(40)//(10)
#define	SHORT_MAX		(65535)
#define	MAXRECTNUM		(1000)		//检测到的可能为一个团的个数
#define	MAX_TARGET_NUM	(100)		//检测到的变化的团数

#define	TRACK_TRIGGER_MAX_NUM 	(2)	
#define	TRIGGER_POINT_NUM		(30)		//一个刷新周期内总共检测到的触发点数

#define SRCBUFWIDTH 1920
#define SRCBUFHEIGHT 1088

/**
* @	像素点描述
*/
typedef struct _motion_pixel_point_info
{
	int16_t	x;		//x坐标
	int16_t	y;		//y坐标
}motion_vertex_t;

/**
* @	区域描述
*/
typedef struct
{
	int16_t min_x;
	int16_t min_y;
	int16_t max_x;
	int16_t max_y;
} rect_t;

/**
* @ 取景方式
*/
typedef enum _track_mode_enum
{
	CLASS_VIEW_FORWARD = 0,		//课前取景
	CLASS_VIEW,					//课中取景
	MAX_NUM_MODE
}track_mode_e;


/**
* @	跟踪区域点设置的最大点数
*/
#define	TRACK_STUDENTS_CEILTRACK_AREA_POINT_MAX	(10)

#define TRIGGER_NUM_MAX (11)

typedef struct TRect
{
	int nTop;
	int nBottom;
	int nLeft;
	int nRight;
	int nFillSize;
	int nMarkVal;
}T_Rect;

/**
* @	触发框信息的结构体
*/
typedef struct _motion_trigger_info
{
	int16_t	trigger_x0;		//表示触发框的左上顶点坐标的x值(以源图像为基准的)
	int16_t	trigger_y0;		//表示触发框的左上顶点坐标的y值(以源图像为基准的)
	int16_t	trigger_x1;		//表示触发框的右下顶点坐标的x值(以源图像为基准的)
	int16_t	trigger_y1;		//表示触发框的右下顶点坐标的y值(以源图像为基准的)
}motion_trigger_info_t;

/**
* @	触发点信息
*/
typedef struct _out_trigger_point_info
{
	int16_t	x0;		//x坐标(左上顶点)
	int16_t	y0;		//y坐标(左上顶点)
	int16_t	x1;		//x坐标(右上顶点)
	int16_t	y1;		//y坐标(右上顶点)
}out_trigger_point_info_t;

/**
* @ 画线的信息
*/
typedef struct _line_info
{
	uint32_t	start_x;	//线的起始点x坐标
	uint32_t	start_y;	//线的起始点y坐标
	uint32_t	end_x;		//线的结束点x坐标
	uint32_t	end_y;		//线的结束点y坐标
}line_info_t;

/**
* @ Y,Cb,Cr值结构体
*/
typedef struct 	_yuv_value_info
{
	uint16_t	y_value;		//y的值
	uint16_t	cb_value;		//cb的值
	uint16_t	cr_value;		//cr的值
}yuv_value_info_t;


/**
* @	调用control时是否需要重新初始化参数类型
*/
typedef enum _motion_control_init_type
{
	MOTION_NO_INIT = 0,		//无需调用control
	MOTION_RE_INIT,			//需要初始化部分参数
	MOTION_RE_START,			//需要初始化所有参数
	MOTION_RE_INIT_NUM
}motion_control_init_type_e;


/**
* @ 点的坐标信息
*/
typedef struct _motion_trigger_point_info
{
	int		x0;
	int		y0;
	int		x1;
	int		y1;
	int		trigger_flag;		//本点的触发标志
}motion_trigger_point_info_t;

/**
* @ 存放自动跟踪的静态参数
*/
typedef struct 	_ITRACK_CEILTRACK_static_param_t
{
	int32_t	size;
	int16_t	video_width;			//输入视频的宽
	int16_t	video_height;			//输入视频的高
	int16_t	pic_width;				//输入缩放后图像的宽
	int16_t	pic_height;				//输入缩放后的图像的高
}ITRACK_CEILTRACK_static_param_t;

/**
* @ 存放自动跟踪的动态参数
*/

typedef struct 	_ITRACK_CEILTRACK_dynamic_params_t
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
	motion_vertex_t	track_point[TRACK_STUDENTS_CEILTRACK_AREA_POINT_MAX];	//跟踪区域点的x，y坐标描述,是相对于缩放之前图像所说
	motion_trigger_info_t trigger[TRACK_TRIGGER_MAX_NUM];
	motion_control_init_type_e	reset_level;	//0表示不要重新初始话,1表示需要重新初始化
}ITRACK_CEILTRACK_dynamic_params_t;


/**
* @ 存放自动跟踪的输入参数
*/
typedef struct 	_CEILITRACK_Params
{
	int32_t	size;
	ITRACK_CEILTRACK_static_param_t 	static_param;
	ITRACK_CEILTRACK_dynamic_params_t	dynamic_param;
	//ITRACK_DynamicParams dynamic_param;
}CeilITRACK_Params;


/**
* @ 存放自动跟踪的输出参数
*/
typedef struct	_CeilITRACK_OutArgs
{
	int32_t	size;
	int32_t	server_cmd;						//发送给服务器用于切换服务器是播放学生机还是老师机的命令
	int32_t	cam_position;					//控制摄像头转到某个预置位的地方
	int32_t	move_distance;					//摄像头移动距离
	int32_t	move_direction;					//摄像头移动方向
	int32_t	cmd_type;						//2为多人站立，为1表示单人站立，0表示坐下。
	int32_t	track_status;					//跟踪状态
	int32_t	trigger_num;					//触发个数
	motion_vertex_t	trigger[TRIGGER_NUM_MAX];	//触发点坐标
	out_trigger_point_info_t	trigger_point[TRIGGER_NUM_MAX];	//告诉应用做自动刷新背景用
	int32_t     IsTrigger;
	uint32_t	reserve1;
	uint32_t	reserve2;
	uint32_t	reserve3;
	uint32_t	reserve4;
}CeilITRACK_OutArgs;


/**
* @	用于内部的全局变量
*/
typedef struct _T_CEILTRACK_GLOBAL_OBJ
{
	uint8_t *Area_vertex; //表示点是否属于检测区域
	uint8_t *gray_buf;  //膨胀腐蚀之后的
	uint8_t *mid_buf;   //原始
	uint16_t *marked_buf;
	T_Rect	g_out_rect[MAX_TARGET_NUM];

	uint8_t *InBuffer; //
	uint8_t *src_rgb_buf; //存放要进行运算的源rgb数据

	uint8_t *tmp_src_rgb_buf; //存放要进行运算的源rgb数据

	uint8_t *dst_rgb_buf; //存放每次跟踪后的第一帧rgb数据

	uint8_t *pre_frame;//上一帧

	int32_t nSrcBufWidth;//采集原始图像的数据摆放buf的宽高
	int32_t nSrcBufHeight;//采集原始图像的数据摆放buf的宽高
	int16_t	turn_flag;			//为0时表示不需要重新跟踪，为1时表示捕获到目标，设置跟踪标志，为2时表示跟踪目标丢失
	int16_t	turn_time;			//定位到目标后,需要等待摄像头调用预置位的时间
	int16_t	track_start;		//表示是否跟踪上目标，0表示未跟踪上，1表示跟踪上了目标
	int32_t	first_time;			//开始进入算法的帧数,目前是前十帧不做处理
	int32_t	last_move_flag;		//方向移动标志,为0表示未移动方向,为1表示移动了方向
	int32_t	last_track_status;	//上次跟踪状态
	int32_t	skin_colour_num;	//连续检测到肤色的帧数
	uint64_t	frame_num;		//算法帧数

	T_Rect 					tInRect;

	int32_t debugValue0;
	int32_t debugValue1;
	int32_t debugValue2;

}T_CEIL_TRACK_GLOBAL_OBJ;


typedef struct _CeilTrackTrackHand {
	CeilITRACK_Params	input_param;	//输入参数
	T_CEIL_TRACK_GLOBAL_OBJ t_global_obj;
}CeilTrackHand;
typedef struct _CeilTrackSaveView
{
	unsigned int chId;
	unsigned int SaveViewFlag;
	unsigned int SaveType;
	motion_trigger_point_info_t	g_trigger_info[TRIGGER_POINT_NUM];
} CeilTrackSaveView;





extern int32_t motion_detect_init(CeilTrackHand *inst,int width,int height,int sens);
extern int32_t	motion_detect_process(CeilTrackHand *inst, int8_t *output_buf, CeilITRACK_OutArgs *output_param);
extern int32_t motion_detect_release(CeilTrackHand *inst);
#endif
