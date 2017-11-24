#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif   /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "sample_comm.h"
#include "math_process.h"


typedef struct rhTRACK_PARAM_S
{
    HI_BOOL bThreadStart;
    VPSS_CHN VpssChn;
} TRACK_PARAM_S;

static TRACK_PARAM_S gs_stTrkParam;
static pthread_t gs_TrkPid;


#define DISTANCE_WITH_WIDTH_RATIO_TRIGGER_STOP				(0.04)		//移动距离占总宽度的百分比，当小于该值时，触发低速移动，大于该值时，高速移动
#define DISTANCE_WITH_WIDTH_RATIO_TRIGGER_MOVE				(0.5)		//移动距离占总宽度的百分比，当大于该值时，触发移动，小于该值时，不移动

/*坐标移动策略*/
#define MOVE_DIRECTION_HOLD									(-1)
#define MOVE_DIRECTION_LEFT									(0)
#define MOVE_DIRECTION_RIGHT								(1)
int g_iMoveDirection = MOVE_DIRECTION_HOLD;
int g_iLastSpeedX = 0;

static HI_U8 g_bHoldState = HI_FALSE;	//静止状态
static int g_iTriggerNum = 0;
#define ARRAY_NUM	(3)
static int g_iHistPosArray[ARRAY_NUM] = { -1 };

void JudgeTargeIsHold(RECT_S *rDst)
{
	static int iIndex = 0;
	iIndex = iIndex % ARRAY_NUM;
	g_iHistPosArray[iIndex] = rDst->s32X;
	iIndex++;

	int iDstXMin = 99999;
	int iDstXMax = 0;
	int iTempVar;
	int i;
	for (i = 0; i < ARRAY_NUM; i++)
	{
		iTempVar = g_iHistPosArray[i];
		if (iTempVar == -1)
		{//位置还没填满，直接认为其没有静止
			g_bHoldState = HI_FALSE;
			return;
		}

		if (iDstXMin > iTempVar)
		{
			iDstXMin = iTempVar;
		}

		if (iDstXMax < iTempVar)
		{
			iDstXMax = iTempVar;
		}
	}

	if (abs(iDstXMax - iDstXMin) < DISTANCE_WITH_WIDTH_RATIO_TRIGGER_STOP * rDst->u32Width)
	{
		g_bHoldState = HI_TRUE;
	}
	else
	{
		g_bHoldState = HI_FALSE;
	}
}

void GetNextRect(RECT_S *rCur, RECT_S *rDst, RECT_S *rOut)
{
	int iWidth  = rCur->u32Width;
	int iHeight = rCur->u32Height;
	int iCurX   = rCur->s32X;
	int iCurY   = rCur->s32Y;
	int iDstX   = rDst->s32X;
	int iDstY   = rDst->s32Y;
	int iTotalDiffX = abs(iCurX - iDstX);
	JudgeTargeIsHold(rDst);
	if (iTotalDiffX < DISTANCE_WITH_WIDTH_RATIO_TRIGGER_MOVE * iWidth
		&& g_iLastSpeedX == 0)//小于触发矩离，直接返回当前位置,不做移动
	{
		g_iLastSpeedX = 0;
		rOut->s32X      = rCur->s32X;
		rOut->s32Y      = rCur->s32Y;
		rOut->u32Width  = rDst->u32Width;
		rOut->u32Height = rDst->u32Height;
		return;
	}

	if (iTotalDiffX >= DISTANCE_WITH_WIDTH_RATIO_TRIGGER_MOVE * iWidth)//当距离够大的时候，释放方向锁
	{
		g_iMoveDirection = MOVE_DIRECTION_HOLD;
	}

	//计算X坐标移动的速度
	int iSpeedX = 1;
	int iMaxSpeedX = 1;
	if (iTotalDiffX < DISTANCE_WITH_WIDTH_RATIO_TRIGGER_STOP * iWidth
		&& (g_iLastSpeedX != 0))//位置移动到目标位置附近的时候，即可触发停止，防止在接近目标时，目标又小范围摆动，导致视角窗口左右晃动
	{
		g_iTriggerNum++;
		if (g_iTriggerNum > 3 || g_bHoldState)//连续5帧位置均没有大范围的移动，则认为目标已停止
		{
			g_iMoveDirection = MOVE_DIRECTION_HOLD;
			//printf("----g_iTriggerNum:%d, g_iMoveDirection:%d\n", g_iTriggerNum, g_iMoveDirection);
			g_iLastSpeedX = 0;
		}
		rOut->s32X      = rCur->s32X;
		rOut->s32Y      = rCur->s32Y;
		rOut->u32Width  = rDst->u32Width;
		rOut->u32Height = rDst->u32Height;
		return;
	}
	else
	{
		//printf("g_iTriggerNum retrive\n");
		g_iTriggerNum = 0;
	}

	iMaxSpeedX = iTotalDiffX / 24;//6
	if (iMaxSpeedX < 3)
	{
		iMaxSpeedX = 3;
	}
	
	if (g_iLastSpeedX < iMaxSpeedX)
	{
		int iDiffX = (iMaxSpeedX - g_iLastSpeedX) / 100;//25
		if (iDiffX < 2)
		{
			iDiffX = 1;
		}

		iSpeedX = g_iLastSpeedX + iDiffX;
		g_iLastSpeedX = iSpeedX;
	}
	else
	{
		iSpeedX = iMaxSpeedX;
		g_iLastSpeedX = iSpeedX;
	}
	
	rOut->s32X      = rCur->s32X;
	rOut->s32Y      = rCur->s32Y;
	rOut->u32Width  = rDst->u32Width;
	rOut->u32Height = rDst->u32Height;
	if (iCurX < iDstX)
	{
		if (g_iMoveDirection != MOVE_DIRECTION_LEFT)
		{
			rOut->s32X = rOut->s32X + iSpeedX > iDstX ? iDstX : rOut->s32X + iSpeedX;
			g_iMoveDirection = MOVE_DIRECTION_RIGHT;
		}
	}
	else
	{
		if (g_iMoveDirection != MOVE_DIRECTION_RIGHT)
		{
			rOut->s32X = rOut->s32X - iSpeedX < iDstX ? iDstX : rOut->s32X - iSpeedX;
			g_iMoveDirection = MOVE_DIRECTION_LEFT;
		}
	}

	if (rOut->s32X < 0)
	{
		rOut->s32X = 0;
	}

	//printf("GetNextRect-->iSpeedX:%d,  iMaxSpeedX:%d; \n", iSpeedX, iMaxSpeedX);
	return;
}


/*Analysis of Algorithms */
HI_VOID *RH_MPI_Alg_Analysis(VIDEO_FRAME_INFO_S *pFrmInfo, HI_U8 *pBuff, HI_U32 bufferlen)
{
	HI_U32 u32Size = pFrmInfo->stVFrame.u32Width*pFrmInfo->stVFrame.u32Height*3/2;
	int Is_Realcoordinate = 0;	
	HI_U8* pVirAddr;
	rect_t rect = {0}; 
	
	pVirAddr = (HI_U8*) HI_MPI_SYS_Mmap(pFrmInfo->stVFrame.u32PhyAddr[0], u32Size);

	/*TODO：使用COPY方式，原因跟踪运行太慢可能导致buffer未更新	*/
	memset(pBuff,0,bufferlen);
	memcpy(pBuff,pVirAddr,bufferlen);
	HI_MPI_SYS_Munmap((HI_VOID*)pVirAddr,u32Size);

	Is_Realcoordinate = Reach_Track_run(pBuff,&rect);
	if(Is_Realcoordinate == 1)
	{
		RH_CheckMoveWay(rect.min_x, rect.min_y);
	}
	else
	{
		//SAMPLE_PRT("!!!!!!!!!!!!!!!!!!!!!!!!!Send720PDataFunc  Is_Realcoordinate:%d ,overall view!!!!!!!!!!!!!!!!!!!!!!!!!\n", Is_Realcoordinate);
	}
	return ;
}


/*RH_MPI_YUV_Analysis */
HI_VOID* RH_MPI_YUV_Analysis(HI_VOID* pdata)
 {
 	TRACK_PARAM_S *pgs_stTrkParam;
	VPSS_CHN VpssChn = 0;
	VPSS_GRP VpssGrp = 0;
	HI_U32 s32Ret;
    HI_S32 s32GetFrameMilliSec = 20000;
	VIDEO_FRAME_INFO_S stFrameInfo;
	HI_U32 bufferLen = TRACK_WIDTH*TRACK_HEIGHT*3/2;
	HI_U8 *pBuff = (unsigned char*)malloc(bufferLen);

	

    pgs_stTrkParam = (TRACK_PARAM_S*)pdata;
	VpssChn = pgs_stTrkParam->VpssChn;

	while(HI_TRUE == pgs_stTrkParam->bThreadStart)
	{
		s32Ret = HI_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrameInfo, s32GetFrameMilliSec);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("HI_MPI_VPSS_GetChnFrame fail,VpssChn(%d),Error(%#x)\n", VpssChn, s32Ret);
			usleep(100*1000);
			continue ;
		}
		/*Analysis of Algorithms */
		RH_MPI_Alg_Analysis(&stFrameInfo, pBuff, bufferLen);	
		
		s32Ret = HI_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrameInfo);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("HI_MPI_VPSS_ReleaseChnFrame fail,VpssChn(%d),Error(%#x)\n", VpssChn, s32Ret);
			usleep(100*1000);
			continue;
		}
	}
	if (pBuff != NULL)
		free(pBuff);
	pBuff = NULL;

	return ;
}

/*跟踪算法启动*/
HI_U32 RH_Track_Algorithm_Start(VPSS_CHN VpssChn)
{
	HI_U32 u32Depth = 4;
	HI_U32 s32Ret = HI_SUCCESS;
	VPSS_GRP VpssGrp = 0;

	Reach_Track_start();
	s32Ret = HI_MPI_VPSS_SetDepth(VpssGrp, VpssChn, u32Depth);	
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("HI_MPI_VPSS_SetDepth failed with %#x!\n",s32Ret);
		return s32Ret;
	}	
	gs_stTrkParam.bThreadStart = HI_TRUE;
	gs_stTrkParam.VpssChn   = VpssChn;	

	s32Ret = pthread_create(&gs_TrkPid, 0, RH_MPI_YUV_Analysis, (HI_VOID*)&gs_stTrkParam);
	return s32Ret;
}

/*Stop Tracking YUV*/
HI_U32 RH_Track_Algorithm_Stop(HI_VOID)
{
    /* join thread */
    if (HI_TRUE == gs_stTrkParam.bThreadStart)
    {
        gs_stTrkParam.bThreadStart = HI_FALSE;
        pthread_join(gs_TrkPid, 0);
    }
	return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
