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
#include "student_process.h"

typedef struct rhTRACK_PARAM_S
{
    HI_BOOL bThreadStart;
    VPSS_CHN VpssChn;
} TRACK_PARAM_S;

static TRACK_PARAM_S gs_stTrkParam;
static pthread_t gs_TrkPid;

/*Analysis of Algorithms */
HI_VOID *RH_MPI_Alg_Analysis(VIDEO_FRAME_INFO_S *pFrmInfo)
{
	HI_U32 u32Size = pFrmInfo->stVFrame.u32Width*pFrmInfo->stVFrame.u32Height*3/2;
	int Is_Realcoordinate = 0;	
	HI_U8* pVirAddr;
	rect_t rect = {0}; 
	
	pVirAddr = (HI_U8*) HI_MPI_SYS_Mmap(pFrmInfo->stVFrame.u32PhyAddr[0], u32Size);
	usleep(200000);
	//算法运行接口
	Is_Realcoordinate = Reach_Track_run(pVirAddr,&rect);	
	if(Is_Realcoordinate == 2)	{
		SAMPLE_PRT("current data !!!!!!!!!!!! ");
		SAMPLE_PRT("rect min_x=%d,min_y=%d,max_x=%d,max_y=%d\n",rect.min_x,rect.min_y,rect.max_x,rect.max_y);
	}

	HI_MPI_SYS_Munmap((HI_VOID*)pVirAddr,u32Size);
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

    pgs_stTrkParam = (TRACK_PARAM_S*)pdata;
	VpssChn = pgs_stTrkParam->VpssChn;

	while(HI_TRUE == pgs_stTrkParam->bThreadStart)
	{
		s32Ret = HI_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrameInfo, s32GetFrameMilliSec);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("HI_MPI_VPSS_GetChnFrame fail,VpssChn(%d),Error(%#x)\n", VpssChn, s32Ret);
			continue;
		}
		/*Analysis of Algorithms */
		RH_MPI_Alg_Analysis(&stFrameInfo);	
		
		s32Ret = HI_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrameInfo);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("HI_MPI_VPSS_ReleaseChnFrame fail,VpssChn(%d),Error(%#x)\n", VpssChn, s32Ret);
			continue;
		}
	}
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
