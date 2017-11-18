/* 
 * Copyright (c) 2009, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/*
 *  ======== itrack.h ========
 *  The track_students interface.
 */

#ifndef codecs_scale_ITRACK_STUDENTS_H_
#define codecs_scale_ITRACK_STUDENTS_H_

#ifdef __cplusplus
extern "C" {
#endif
	/*
#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/visa.h>
#include <ti/sdo/ce/skel.h>
#include <ti/xdais/xdas.h>

#include <ti/xdais/xdas.h>
#include <ti/xdais/ialg.h>
	*/
#include <stdlib.h>
#include<stdint.h>

typedef unsigned char uint8_t;
typedef float		float_t;
typedef unsigned short uint16_t;
typedef unsigned int	uint32_t;
typedef unsigned long long	uint64_t;


#define ITRACK_EOK      0
#define ITRACK_EFAIL    -1
#define ITRACK_ERUNTIME -2




/**
* @ 取点的角度的个数，如90度，180度等的取点
*/
#define	POINT_VIEW_NUM			(4)

/**
* @	跟踪区域点设置的最大点数
*/
#define	TRACK_AREA_POINT_MAX	(4)

/**
* @ 学生跟踪触发框的最大值
*/
#define STUDENTS_TRIGGER_NUM	(35)

/**
* @ 学生跟踪屏蔽框的最大值
*/
#define STUDENTS_SHIELD_NUM	(20)
/**
* @ 学生跟踪多目标上台检测框的最大值
*/
#define STUDENTS_MULTITARGET_NUM	(10)

/**
* @ 学生跟踪预置位的最大值,由于要留一个预置位给全景,所以要加1
*/
#define STUDENTS_PRESET_NUM	(STUDENTS_TRIGGER_NUM + 1)

/**
 *  @brief      This must be the first field of all ITRACK instance objects.
 */
 
 #define STUDENTS_MAX_FRAME 66
 
typedef struct ITRACK_Obj 
{
    struct ITRACK_STUDENTS_Fxns *fxns;
}ITRACK_Obj;

/**
 *  @brief      Opaque handle to an ITRACK object.
 */
typedef struct ITRACK_Obj *ITRACK_Handle;

//typedef IALG_Cmd ITRACK_Cmd;
#define STU_MAX_CHECKNUM 20
/**
* @	像素点描述
*/
typedef struct _pixel_point_info
{	
	int16_t	x;		//x坐标
	int16_t	y;		//y坐标
}vertex_t;

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
* @	触发框信息的结构体
*/
typedef struct _student_trigger_info
{
	int16_t	x;
	int16_t	y;	
	int16_t	width;
	int16_t	height;
}student_trigger_info_t;

/**
* @ 存放自动跟踪的静态参数
*/
typedef struct 	_stutrack_static_param
{
	int32_t	size;
	int16_t	video_width;			//输入视频的宽
	int16_t	video_height;			//输入视频的高
	int16_t	pic_width;				//输入缩放后图像的宽
	int16_t	pic_height;				//输入缩放后的图像的高
}stutrack_static_param_t;
typedef struct Line_ab
{
	float a;
	float b;
}Line_ab;
/**
* @ 存放自动跟踪的动态参数
*/

typedef struct CamPos
{
	unsigned char x[4];
	unsigned char y[4];
	unsigned char z[4];
}stCamPos;
typedef struct 	_stutrack_dynamic_param
{
	int32_t	size;
	stCamPos CamPrePos[4];
	int16_t	x_offset;		//图像的x坐标缩放倍数,是IMG_X_offset_0
	int16_t	y_offset;		//图像的y坐标缩放倍数,是IMG_Y_offset_0
	short	cam_x_offset;		//摄像机横向偏移值
	short	cam_y_offset;		//摄像机纵向偏移值
	int16_t	reset_time;		//学生触发后多长时间没有坐下的触发就认为是坐下了的时间
	int16_t	trigger_num;	//表示触发框个数
	int16_t track_num_row;   //设定一行学生数量
	student_trigger_info_t	*trigger_info;	//学生机跟踪的触发框信息
	student_trigger_info_t	*shield_info;	//学生机屏蔽框信息
	student_trigger_info_t	*multitarget_info;	//学生机屏蔽框信息
	int16_t	control_mode;	//控制类型,0为自动,1为手动
	int16_t frame_onesecond;  //一秒钟获取原始数据的帧数
	int16_t video_type;    
	int16_t	sens;			//
	int16_t	message;		//主要用来设定画那些线，比如是否画人得轮廓线等,1:显示所有边缘点状态；
							//2:符合45度，90度，135度，180度的线画出来；3:画模版;4:画跟踪框;5:画跟踪框内的肤色点
	int16_t process_type;	//为0时进行算法处理,为1时只画触发区域,跟踪区域和跟踪框
	int16_t	track_point_num;//跟踪框的设置，最大有几个点，(可由这些点来连接成框，组成了跟踪区域)
	vertex_t	track_point[TRACK_AREA_POINT_MAX];	//跟踪区域点的x，y坐标描述,是相对于缩放之前图像所说
	control_init_type_e	reset_level;	//0表示不要重新初始话,1表示需要重新初始化
	int16_t	nTrackSwitchType;			//跟踪切换类型,0表示跟踪,1表示只切换
	int16_t	nStrategyNo;			//机位信息,用于只切不跟的方式选择
	
	Line_ab line_left;
	Line_ab line_right;
}StuITRACK_DynamicParams;

/**
* @ 存放自动跟踪的输入参数
*/
typedef struct 	_StuITRACK_Params
{
	int32_t	size;
	stutrack_static_param_t 	static_param;
	StuITRACK_DynamicParams	dynamic_param;
}StuITRACK_Params;

typedef struct ITRACK_InArgs 
{
    int32_t inBufSize;
	int32_t outBufSize;
	int32_t inBufValidBytes;
} ITRACK_InArgs;

/**
* @ 存放自动跟踪的输出参数
*/
typedef struct Rect
{
	int x;
	int y;
	int width;
	int height;
}Rect;


typedef struct	_stutrack_output_param
{
							
	int32_t up_num;							//为0表示没有任何学生站起,为1表示有1个或多个人站起
	Rect Up_List[STU_MAX_CHECKNUM];
	int64_t	time[STU_MAX_CHECKNUM];

}StuITRACK_OutArgs;


#ifdef __cplusplus
}
#endif


/*@}*/  /* ingroup */

#endif
/*
 *  @(#) ti.sdo.ce.examples.codecs.track_students; 1, 0, 0,204; 7-25-2009 21:30:29; /db/atree/library/trees/ce/ce-n11x/src/
 */

