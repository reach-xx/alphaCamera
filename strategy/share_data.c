#include "share_data.h"
#include "common.h"


#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>


strategy_input_info_t	gStrategyInputInfo;

/*get time */
unsigned long long getCurTime(void)
{
	struct timeval tv;
	struct timezone tz;
	unsigned long long ultime;

	gettimeofday(&tv , &tz);
	ultime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return (ultime);
}

//板书机检测结果赋值
void SetBlackboardDetectResult(int _iBlackboardTriggerFlag, int _iBlackboardNo)
{
	if (_iBlackboardNo == 0)
	{
		gStrategyInputInfo.iBlackboardTriggerFlag1 = _iBlackboardTriggerFlag;
	}
	else if (_iBlackboardNo == 1)
	{
		gStrategyInputInfo.iBlackboardTriggerFlag2 = _iBlackboardTriggerFlag;
	}

	DEBUG(DL_DEBUG, "[STRATEGY]--->SetBlackboardDetectResult--->iBlackboardTriggerFlag1:%d, iBlackboardTriggerFlag2:%d, _iBlackboardTriggerFlag:%d, _iBlackboardNo:%d\n", 
		gStrategyInputInfo.iBlackboardTriggerFlag1, gStrategyInputInfo.iBlackboardTriggerFlag2, _iBlackboardTriggerFlag, _iBlackboardNo);
}



//学生机检测结果赋值
void SetStudentDetectResult(int _iStudentStandupNum, cam_preset_position_info_t _cStudentCamCoord)
{
	gStrategyInputInfo.iStudentStandupNum = _iStudentStandupNum;
	gStrategyInputInfo.cStudentCamCoord = _cStudentCamCoord;
	if (_iStudentStandupNum == 1)
	{
		gStrategyInputInfo.bIsNeedWaitStudentCamTrans = true;
	}

	DEBUG(DL_DEBUG, "[STRATEGY]--->iTeatcherNum:%d, iStudentStandupNum:%d, bIsNeedWaitTeacherCamTrans:%d, bIsNeedWaitStudentCamTrans:%d, cStudentCamCoord:%x%x%x%x%x%x%x%x%x-%x%x%x%x, cTeacherCamCoord:%x%x%x%x%x%x%x%x%x-%x%x%x%x\n", 
			gStrategyInputInfo.iTeatcherNum, 
			gStrategyInputInfo.iStudentStandupNum, 
			gStrategyInputInfo.bIsNeedWaitTeacherCamTrans, 
			gStrategyInputInfo.bIsNeedWaitStudentCamTrans,
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[0],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[1],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[2],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[3],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[4],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[5],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[6],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[7],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[8],
			gStrategyInputInfo.cStudentCamCoord.zoom[0],
			gStrategyInputInfo.cStudentCamCoord.zoom[1],
			gStrategyInputInfo.cStudentCamCoord.zoom[2],
			gStrategyInputInfo.cStudentCamCoord.zoom[3],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[0],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[1],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[2],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[3],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[4],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[5],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[6],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[7],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[8],
			gStrategyInputInfo.cTeacherCamCoord.zoom[0],
			gStrategyInputInfo.cTeacherCamCoord.zoom[1],
			gStrategyInputInfo.cTeacherCamCoord.zoom[2],
			gStrategyInputInfo.cTeacherCamCoord.zoom[3]
			);
	
}

//老师检测算法模块调用，将其检测的结果保存到全部变量gStrategyInputInfo中
void SetTeacherDetectResult(int _iTeacherNum, cam_preset_position_info_t _cTeacherCamCoord, bool _bIsNewTeacherTarget)
{
	gStrategyInputInfo.iTeatcherNum = _iTeacherNum;
	gStrategyInputInfo.cTeacherCamCoord = _cTeacherCamCoord;

	if (_iTeacherNum == 1)
	{
		if (_bIsNewTeacherTarget == true)
		{
			gStrategyInputInfo.bIsNeedWaitTeacherCamTrans = true;
			gStrategyInputInfo.iTeacherMoveState = 1;
		}
	}

	if (_iTeacherNum > 0)
	{	
		gStrategyInputInfo.iTeacherTargetRefreshTime = getCurTime();
	}

	static int iOldTeacherNum = -1;
	if (iOldTeacherNum != _iTeacherNum)
	{
		DEBUG(DL_DEBUG, "[STRATEGY]--->iTeatcherNum:%d, iStudentStandupNum:%d, bIsNeedWaitTeacherCamTrans:%d, bIsNeedWaitStudentCamTrans:%d, cStudentCamCoord:%x%x%x%x%x%x%x%x%x-%x%x%x%x, cTeacherCamCoord:%x%x%x%x%x%x%x%x%x-%x%x%x%x\n", 
			gStrategyInputInfo.iTeatcherNum, 
			gStrategyInputInfo.iStudentStandupNum, 
			gStrategyInputInfo.bIsNeedWaitTeacherCamTrans, 
			gStrategyInputInfo.bIsNeedWaitStudentCamTrans,
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[0],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[1],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[2],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[3],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[4],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[5],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[6],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[7],
			gStrategyInputInfo.cStudentCamCoord.pan_tilt[8],
			gStrategyInputInfo.cStudentCamCoord.zoom[0],
			gStrategyInputInfo.cStudentCamCoord.zoom[1],
			gStrategyInputInfo.cStudentCamCoord.zoom[2],
			gStrategyInputInfo.cStudentCamCoord.zoom[3],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[0],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[1],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[2],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[3],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[4],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[5],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[6],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[7],
			gStrategyInputInfo.cTeacherCamCoord.pan_tilt[8],
			gStrategyInputInfo.cTeacherCamCoord.zoom[0],
			gStrategyInputInfo.cTeacherCamCoord.zoom[1],
			gStrategyInputInfo.cTeacherCamCoord.zoom[2],
			gStrategyInputInfo.cTeacherCamCoord.zoom[3]
			);
	}
	iOldTeacherNum = _iTeacherNum;
}


void SetTeacherMoveState(int _iMoveState)
{
	gStrategyInputInfo.iTeacherMoveState = _iMoveState;
}





















