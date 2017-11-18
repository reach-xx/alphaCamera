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

#include "rh_interface.h"

static pthread_t s_draw_line ;
static SAMPLE_VENC_GETSTREAM_PARA_S gs_stVgs;
static RH_POLY_ARRAY_S  gs_stPolygon ={0};

HI_U32 RH_MPI_DrawLine(RH_POLY_ARRAY_S *pstPoint)
{
	HI_U8 i;

	gs_stPolygon.u16Num = pstPoint->u16Num;
	if(gs_stPolygon.u16Num > VGS_LINE_MAX_POINT) {
		SAMPLE_PRT("<RH_MPI_DrawLine> max Point for Polygon\n");
		return HI_FAILURE;
	}
	/*The thick and color for draw line*/
	for(i = 0 ; i < gs_stPolygon.u16Num ;i++) {
		gs_stPolygon.stPolygon[i].u32Thick = pstPoint->stPolygon[i].u32Thick;
		gs_stPolygon.stPolygon[i].u32Color = pstPoint->stPolygon[i].u32Color;
		gs_stPolygon.stPolygon[i].stStartPoint.s32X =pstPoint->stPolygon[i].stStartPoint.s32X;
		gs_stPolygon.stPolygon[i].stStartPoint.s32Y = pstPoint->stPolygon[i].stStartPoint.s32Y;
		gs_stPolygon.stPolygon[i].stEndPoint.s32X   = pstPoint->stPolygon[i].stEndPoint.s32X;
		gs_stPolygon.stPolygon[i].stEndPoint.s32Y   = pstPoint->stPolygon[i].stEndPoint.s32Y;				
	}	
	
	return HI_SUCCESS;

}

/*draw line*/
HI_S32 RH_MPI_VGS_FillRect(VIDEO_FRAME_INFO_S* pstFrmInfo, RH_POLY_ARRAY_S* pstRect, HI_U32 u32Color)
{
    VGS_HANDLE VgsHandle;
    HI_S32 s32Ret = HI_SUCCESS;
    VGS_TASK_ATTR_S stVgsTask;
	VGS_DRAW_LINE_S stVgsAddLine[VGS_LINE_MAX_POINT];
	HI_S32 stPointCnt = 0;

    if (0 == pstRect->u16Num || pstRect->u16Num > VGS_LINE_MAX_POINT)
    {
        return s32Ret;
    }
    s32Ret = HI_MPI_VGS_BeginJob(&VgsHandle);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Vgs begin job fail,Error(%#x)\n", s32Ret);
        return s32Ret;
    }

    memcpy(&stVgsTask.stImgIn, pstFrmInfo, sizeof(VIDEO_FRAME_INFO_S));
    memcpy(&stVgsTask.stImgOut, pstFrmInfo, sizeof(VIDEO_FRAME_INFO_S));

	/*draw Line */
	stPointCnt = pstRect->u16Num;
	memcpy(stVgsAddLine,pstRect->stPolygon,sizeof(VGS_DRAW_LINE_S)*stPointCnt);		
	s32Ret = HI_MPI_VGS_AddDrawLineTaskArray(VgsHandle, &stVgsTask,stVgsAddLine,stPointCnt);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VGS_AddDrawLineTaskArray fail,Error(%#x)\n", s32Ret);
        HI_MPI_VGS_CancelJob(VgsHandle);
        return s32Ret;
    }

    s32Ret = HI_MPI_VGS_EndJob(VgsHandle);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VGS_EndJob fail,Error(%#x)\n", s32Ret);
        HI_MPI_VGS_CancelJob(VgsHandle);
        return s32Ret;
    }
    return s32Ret;

}


/*Draw Line Task start*/
HI_VOID* RH_MPI_VGS_StartTask(HI_VOID* p)
{
    VPSS_CHN VpssChn;
	VPSS_GRP VpssGrp = 0;
	VENC_CHN vencChn = 0;
	HI_U32 s32Ret;
	SAMPLE_VENC_GETSTREAM_PARA_S* pstPara;
	VIDEO_FRAME_INFO_S stFrameInfo;
    HI_S32 s32GetFrameMilliSec = 2000;
    HI_S32 s32SetFrameMilliSec = 2000;
	RH_POLY_ARRAY_S  Polygon;

    pstPara = (SAMPLE_VENC_GETSTREAM_PARA_S*)p;
    VpssChn = pstPara->s32Cnt;
	
	while(HI_TRUE == pstPara->bThreadStart) {	
		memcpy(&Polygon,&gs_stPolygon,sizeof(RH_POLY_ARRAY_S));
		s32Ret = HI_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrameInfo, s32GetFrameMilliSec);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("HI_MPI_VPSS_GetChnFrame fail,VpssChn(%d),Error(%#x)\n", VpssChn, s32Ret);
			continue;
		}

        s32Ret = RH_MPI_VGS_FillRect(&stFrameInfo, &Polygon, 0x0000FF00);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("RH_MPI_VGS_FillRect fail,Error(%#x)\n", s32Ret);
            (HI_VOID)HI_MPI_VI_ReleaseFrame(VpssChn, &stFrameInfo);
            continue;
        }	
		
		s32Ret = HI_MPI_VENC_SendFrame(vencChn, &stFrameInfo, s32SetFrameMilliSec);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("HI_MPI_VENC_SendFrame fail,Error(%#x)\n", s32Ret);
		}	
		
		s32Ret = HI_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrameInfo);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("HI_MPI_VPSS_ReleaseChnFrame fail,VpssChn(%d),Error(%#x)\n", VpssChn, s32Ret);
			continue;
		}

	}

	return ;
}


/*Draw line create VGS task*/
HI_U32 RH_MPI_VGS_CreateLineTask(VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{
	HI_U32 s32Ret = HI_SUCCESS;
	HI_U32 u32Depth = 4;

	s32Ret = HI_MPI_VPSS_SetDepth(VpssGrp, VpssChn, u32Depth);	
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("HI_MPI_VPSS_SetDepth failed with %#x!\n",s32Ret);
		return s32Ret;
	}
	memset(&gs_stPolygon,0x0,sizeof(RH_POLY_ARRAY_S));
    gs_stVgs.bThreadStart = HI_TRUE;
    gs_stVgs.s32Cnt = VpssChn;
	
	s32Ret = pthread_create(&s_draw_line, NULL, (void *)RH_MPI_VGS_StartTask,  (HI_VOID*)&gs_stVgs);
	if(s32Ret < 0) {
		SAMPLE_PRT( "create RH_MPI_VGS_StartTask() failed\n");
	}
	
	return s32Ret;
}

/*destroy line task*/
HI_U32 RH_MPI_VGS_DestroyLineTask()
{
    if (HI_TRUE == gs_stVgs.bThreadStart)
    {
        gs_stVgs.bThreadStart = HI_FALSE;
		
        pthread_join(s_draw_line, 0);
		SAMPLE_PRT("<RH_MPI_VGS_DestroyLineTask>   pthread Draw Line Pid\n");
    }
    return HI_SUCCESS;		
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif   /* End of #ifdef __cplusplus */

