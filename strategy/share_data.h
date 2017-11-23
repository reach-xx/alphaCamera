#ifndef SHARE_DATA__H
#define SHARE_DATA__H

#include <stdbool.h>
#include "auto_track.h"


/**
* @ 策略模块的输入参数
*/
typedef struct _strategy_input_info
{
	/**老师机检测结果**/
	int iTeatcherNum;						//讲台区检测到目标的个数；0表示没有目标；1表示检测到一个目标；2表示检测到多个目标
	cam_preset_position_info_t cTeacherCamCoord;		//老师机云台摄像机坐标，3519该变量暂时没用，处于失效状态
	bool bIsNeedWaitTeacherCamTrans;			//是否需要等待老师特写先移动
	int iTeacherMoveState;					//老师特写镜头的移动状态，0表示静止，1表示移动
	unsigned long long iTeacherTargetRefreshTime;		//老师目标触发的刷新时间
	bool bIsNewTeacherTarget;							//是否为新的老师目标

	/**学生机检测结果**/
	int iStudentStandupNum;					//起立学生的人数；0表示未检测到学生起立；1表示检测到一个学生起立；2表示检测到多个目标起立
	cam_preset_position_info_t cStudentCamCoord;		//学生机云台摄像机坐标，3519该变量暂时没用，处于失效状态
	bool bIsHaveSendStudentCamTransMsg;			//是否发送云台摄像机转动的消息给录播服务器，3519该变量暂时没用，处于失效状态
	bool bIsNeedWaitStudentCamTrans;							//是否需要等待学生特写先移动
		
	/**板书机检测结果**/
	int	iBlackboardTriggerFlag1;			//是否在板书区域标志，1为在板书区域，0为不在板书区域
	int	iBlackboardTriggerFlag2;			//是否在板书区域标志，1为在板书区域，0为不在板书区域

}strategy_input_info_t;


void SetBlackboardDetectResult(int _iBlackboardTriggerFlag, int _iBlackboardNo);

void SetStudentDetectResult(int _iStudentStandupNum, cam_preset_position_info_t _cStudentCamCoord);
void SetTeacherDetectResult(int _iTeacherNum, cam_preset_position_info_t _cTeacherCamCoord, bool _bIsNewTeacherTarget);
void SetTeacherMoveState(int _iMoveState);





















#endif