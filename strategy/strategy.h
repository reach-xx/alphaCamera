#ifndef STRATEGY__H
#define STRATEGY__H

#include "share_data.h"
#include "common.h"



//镜头的切换状态
typedef enum _switch_state
{
	SWITCH_FORBID	= 1,	//禁止切换
	SWITCH_FREE	= 2,		//可以切换
	SWITCH_FORCE	= 3,	//强制切换
}switch_state_e;

//镜头的切换状态
typedef enum _cam_trans_state
{
	NO_CAM_TRANS		= 0,		//不需要做转动
	TEACHER_CAM_TRANS	= 1,	//老师机云台转动
	STUDENT_CAM_TRANS	= 2,	//学生机云台转动
	
}cam_trans_state_e;

//镜头的类型
typedef enum _cam_type
{
	TEACHER_PANORAMA								= 0,		//老师全景，并且下一个镜头未知
	
	TEACHER_PANORAMA_WITH_NEXT_TEACHER_FEATURE		= 1,		//老师全景，并且下一个镜头为老师特写, 3519该状态变量闲置
	TEACHER_PANORAMA_WITH_NEXT_STUDENT_FEATURE		= 2,		//老师全景，并且下一个镜头为学生特写, 3519该状态变量闲置
	TEACHER_PANORAMA_WITH_NEXT_STUDENT_PANORAMA		= 3,		//老师全景，并且下一个镜头为学生全景, 3519该状态变量闲置

	TEACHER_FEATURE									= 4,		//老师特写
	STUDENT_FEATURE									= 5,		//学生特写
	STUDENT_PANORAMA								= 6,		//学生全景	
	BLACK_BOARD1									= 7,		//板书镜头
	
	CAM_TYPE_NUM
}cam_type_e;

typedef enum _STRATEGY_
{
    STRATEGY_2 = 0,     //机位1 老师 + 学生
    STRATEGY_4_1 = 1,     //机位1 老师+学生+板书+学生全景
    STRATEGY_5_1 = 2,   //5机位单板书
    STRATEGY_5_2 = 3,   //5机位双板书
	STRATEGY_3_1 = 4,   //机位1 :老师+ 学生 +板书
	STRATEGY_3_2 = 5,   //机位2 :老师+ 学生 +老师全景
	STRATEGY_4_2 = 6,   //机位 :老师+学生+老师全景+学生全景

	STRATEGY_3_3 = 7,   //机位3 :老师+学生+学生全景
	STRATEGY_4_3 = 8,   //机位3 :老师+ 学生 + 板书 + 老师全景
	STRATEGY_6 = 10,   //6机位双板书  老师+学生+老师全景+学生全景+板书1+板书2
	STRATEGY_6_2STU = 11, //6机位双学生 老师+学生+老师全景+学生全景+板书1+学生2
	STRATEGY_3_4 = 12, // 3机位: 老师+老师全景+学生全景

	//以下两种不支持
	STRATEGY_4_STU_2_ROOM_1 = 50,   //机位 :老师+ 学生 1+学生2+学生全景在同一个教室
	STRATEGY_4_STU_2_ROOM_2 = 51,   //机位 :老师+ 学生 1+学生2+学生全景在不同教室


	STRATEGY_3_ONLYSWITCH = 100,     //只切不跟 :老师1 +  学生3+板书4 
	STRATEGY_4_ONLYSWITCH = 101   //只切不跟 :老师1 +  学生3(前面的学生)+板书4 +学生5(后面的学生)
 
} STRATEGY;

/**
* @	多机位策略时间信息表 
*/
typedef struct _strategy_timeinfo
{
	int iTeacherPanoramaWithNextTeacherFeatureKeepTime; 		//老师全景(并且下一个为老师特写)的保持时间, 3519该状态变量闲置
	int iTeacherPanoramaWithNextStudentFeatureKeepTime; 		//老师全景(并且下一个为学生特写)的保持时间, 3519该状态变量闲置
	int iTeacherPanoramaWithNextStudentPanoramaKeepTime; 		//老师全景(并且下一个为学生全景)的保持时间, 3519该状态变量闲置

	int iTeacherPanoramaMinKeepTime; 		//老师全景最小的保持时间
	int iTeacherFeatureMinKeepTime; 		//老师特写最小的保持时间

	int iStudentPanoramaMinKeepTime; 		//学生全景最小的保持时间
	int iStudentPanoramaMaxKeepTime; 		//学生全景最小的保持时间, 3519该状态变量闲置
	int iStudentFeatureMinKeepTime; 		//学生特写最小的保持时间
	int iStudentFeatureMaxKeepTime; 		//学生特写最大的保持时间, 3519该状态变量闲置

	int iBlackboardMinKeepTime;				//板书镜头最小保持时间

}strategy_timeinfo_t;

/**
* @	多机位策略状态信息表
*/
typedef struct _strategy_info
{
	unsigned long long iTeacherPanoramaStartTime;		//老师全景镜头开始的时间
	unsigned long long iTeacherFeatureStartTime;		//老师特写镜头开始的时间
	
	unsigned long long iStudentPanoramaStartTime;		//学生全景镜头开始的时间
	unsigned long long iStudentFeatureStartTime;		//学生特写镜头开始的时间

	unsigned long long iBlockboardStartTime;			//板书镜头开始的时间

	cam_type_e	cLastCamType;			//最后一个镜头的类型
	switch_cmd_type_e sLastCmdType;		//最后的切换镜头
	switch_cmd_type_e sOutCmdType;		//输出的待切换的镜头
	switch_state_e sSwitchState;		//当前镜头切换状态
	cam_trans_state_e cCamTransState;	//云台摄像机转动状态

}strategy_info_t;



/**
* @ 查询云台摄像机的预置位信息
*/
int query_cam_coord_info_by_teacher(int _preset_position_type);

/**
* @ 查询云台摄像机的预置位信息
*/
int query_cam_coord_info_by_student(int iStudentNo, int _preset_position_type);

/**
* @ 启动切换策略模块
*/
int strategy_start();

/**
* @ 重置切换策略模块
*/
void strategy_reset();


/**
* @ 获取老师与录播之间的链接状态
*/
int get_teacher_to_encode_connect_state();

/**
* @ 获取老师与学生之间的链接状态
*/
int get_teacher_to_student_connect_state(int index);

/**
* @ 获取老师与板书之间的链接状态
*/
int get_teacher_to_blackboard_connect_state(int index);

/**
* @ 发送机位情况给录播服务器
*/
void SendStrategyNoToEncode();

























#endif