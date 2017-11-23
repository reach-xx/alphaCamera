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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "rh_protocol.h"

static HI_BOOL gs_PTZStart = HI_TRUE;
static pthread_t gs_PTZPid;
static int 	gs_step = 1;
extern RH_Coord gs_Coord;


/*云台设置*/
HI_S32 RH_CoordSet(const RH_Coord *pCoord)
{
	HI_S32 S32Ret;
	VPSS_CHN  vpssChn = VPSS_CROP_EXTCHN ;

	SAMPLE_PRT("Crop: X=%d   Y =%d   width: %d   height:%d  \n",pCoord->rect.s32X,
					pCoord->rect.s32Y,pCoord->rect.u32Width,pCoord->rect.u32Height);
	RH_MPI_VPSS_SetChnCrop(vpssChn, &pCoord->rect);
	return S32Ret;
	
}

/*检查移动路径*/
HI_VOID* RH_CheckMoveWay(int x,int y)
{
	RH_Coord *pCoord = &gs_Coord;
	HI_S32 s32Ret = HI_SUCCESS;
	VPSS_CHN VpssChn = VPSS_CROP_EXTCHN;

	s32Ret = RH_MPI_VPSS_GetChnCrop(VpssChn,&pCoord->rect); 
	if(HI_SUCCESS !=s32Ret)  {
		SAMPLE_PRT("GET VPSS Crop Failed 0x%x\n",s32Ret);
	}

	x = x % 2 == 0 ? x : x - 1;
	x = x < 0 ? 0 : x;
	
	y = y % 2 == 0 ? y : y - 1;
	y = y < 0 ? 0 : y;
	
	pCoord->end_rect.s32X  = x;
	pCoord->end_rect.s32Y  = y;
	SAMPLE_PRT("ChickMoveWay (x,y)=(%d, %d) \n", x, y);
	
	if (pCoord->rect.s32X > x && pCoord->rect.s32Y == y)	{
		pCoord->cmd_mode = VISCA_LEFT;
	}
	else if(pCoord->rect.s32X < x && pCoord->rect.s32Y == y)
	{
		pCoord->cmd_mode = VISCA_RIGHT;
	}
	else if(pCoord->rect.s32X  == x && pCoord->rect.s32Y > y)
	{
		pCoord->cmd_mode = VISCA_UP;
	}
	else if(pCoord->rect.s32X  == x && pCoord->rect.s32Y < y)
	{
		pCoord->cmd_mode = VISCA_DOWN;
	}
	else if(pCoord->rect.s32X  < x && pCoord->rect.s32Y < y)
	{
		pCoord->cmd_mode = VISCA_DOWNRIGHT;
	}
	else if(pCoord->rect.s32X  < x && pCoord->rect.s32Y > y)
	{
		pCoord->cmd_mode = VISCA_UPRIGHT;
	}
	else if(pCoord->rect.s32X  > x && pCoord->rect.s32Y < y)
	{
		pCoord->cmd_mode = VISCA_DOWNLEFT;
	}
	else if(pCoord->rect.s32X  > x && pCoord->rect.s32Y > y)
	{
		pCoord->cmd_mode = VISCA_UPLEFT;
	}	
}


/*自动云台模式*/
HI_S32 RH_AUTOCoordCalc(RH_Coord *pCoord)
{
	HI_S32 s32Ret = HI_SUCCESS;	


	
	return s32Ret;
}


/*函数计算坐标  ---- 手动模式*/
HI_S32 RH_ManualCoordCalc(RH_Coord *pCoord)
{
	HI_S32 s32Ret = HI_SUCCESS;	
	VPSS_CHN VpssChn = VPSS_CROP_EXTCHN;
	int iSpeed = 2;

	s32Ret = RH_MPI_VPSS_GetChnCrop(VpssChn,&pCoord->rect); 
	if(HI_SUCCESS !=s32Ret)  {
		SAMPLE_PRT("GET VPSS Crop Failed 0x%x\n",s32Ret);
	}
	
	switch (pCoord->cmd_mode)
	{
		case VISCA_UP:   //UP
			pCoord->rect.s32Y -= gs_step*pCoord->vSpeed;
			if(pCoord->rect.s32Y <= 0)	{
				pCoord->bEnable = HI_FALSE;
				pCoord->rect.s32Y = 0;
			}	
			break;
		case VISCA_DOWN:
			pCoord->rect.s32Y += gs_step*pCoord->vSpeed;
			if(pCoord->rect.s32Y + pCoord->rect.u32Height  >= SENSOR_OUTPUT_H)	{
				pCoord->rect.s32Y = SENSOR_OUTPUT_H -  pCoord->rect.u32Height;
				pCoord->bEnable = HI_FALSE;
			}		
			break;
		case VISCA_LEFT:
			pCoord->rect.s32X -= gs_step*pCoord->wSpeed;
			if (pCoord->rect.s32X <= 0) {
				pCoord->bEnable = HI_FALSE;
				pCoord->rect.s32X = 0 ;				
			}
			break;
		case VISCA_RIGHT:
			pCoord->rect.s32X += gs_step*pCoord->wSpeed;
			if (pCoord->rect.s32X + pCoord->rect.u32Width >= SENSOR_OUTPUT_W) {
				pCoord->bEnable = HI_FALSE;
				pCoord->rect.s32X = SENSOR_OUTPUT_W - pCoord->rect.u32Width;
			 }
			break;
		case VISCA_UPLEFT:
			if(pCoord->rect.s32X == 0 || pCoord->rect.s32Y == 0) {
				iSpeed = 2;
			}else {
				iSpeed = pCoord->rect.s32Y * pCoord->wSpeed/pCoord->rect.s32X;
			}			
			pCoord->rect.s32X -= gs_step*pCoord->wSpeed;
			pCoord->rect.s32Y -= gs_step*iSpeed;
			if(pCoord->rect.s32X <= 0) {
				pCoord->rect.s32X = 0;
				pCoord->bEnable = HI_FALSE;
			}
			if(pCoord->rect.s32Y <= 0) {
				pCoord->rect.s32Y = 0;
				pCoord->bEnable = HI_FALSE;
			}			
			break;
		case VISCA_DOWNLEFT:
			if(pCoord->rect.s32X == 0 || pCoord->rect.s32Y == 0) {
				iSpeed = 2;
			}else {
				iSpeed = pCoord->rect.s32Y * pCoord->wSpeed/pCoord->rect.s32X;
			}			
			pCoord->rect.s32X -= gs_step*pCoord->wSpeed;
			pCoord->rect.s32Y += gs_step*iSpeed;
			if(pCoord->rect.s32X <= 0) {
				pCoord->rect.s32X = 0;
				pCoord->bEnable = HI_FALSE;
			}
			if(pCoord->rect.s32Y + pCoord->rect.u32Height >= SENSOR_OUTPUT_H) {
				pCoord->rect.s32Y = SENSOR_OUTPUT_H - pCoord->rect.u32Height;
				pCoord->bEnable = HI_FALSE;
			}		
			break;
		case VISCA_UPRIGHT:
			if(pCoord->rect.s32X == 0 || pCoord->rect.s32Y == 0) {
				iSpeed = 2;
			}else {
				iSpeed = pCoord->rect.s32Y * pCoord->wSpeed/pCoord->rect.s32X;
			}			
			pCoord->rect.s32X += gs_step*pCoord->wSpeed;
			pCoord->rect.s32Y -= gs_step*iSpeed;

			if(pCoord->rect.s32X + pCoord->rect.u32Width >= SENSOR_OUTPUT_W)  {
				pCoord->rect.s32X = SENSOR_OUTPUT_W - pCoord->rect.u32Width;
				pCoord->bEnable = HI_FALSE;
			}
			if(pCoord->rect.s32Y <= 0) {
				pCoord->rect.s32Y = 0;
				pCoord->bEnable = HI_FALSE;
			}			
			break;
		case VISCA_DOWNRIGHT:
			if(pCoord->rect.s32X == 0 || pCoord->rect.s32Y == 0) {
				iSpeed = 2;
			}else {
				iSpeed = pCoord->rect.s32Y * pCoord->wSpeed/pCoord->rect.s32X;
			}			
			pCoord->rect.s32X += gs_step*pCoord->wSpeed;
			pCoord->rect.s32Y += gs_step*iSpeed;

			if(pCoord->rect.s32X + pCoord->rect.u32Width >= SENSOR_OUTPUT_W)  {
				pCoord->rect.s32X = SENSOR_OUTPUT_W - pCoord->rect.u32Width;
				pCoord->bEnable = HI_FALSE;
			}
			if(pCoord->rect.s32Y + pCoord->rect.u32Height >= SENSOR_OUTPUT_H) {
				pCoord->rect.s32Y = SENSOR_OUTPUT_H - pCoord->rect.u32Height;
				pCoord->bEnable = HI_FALSE;
			}	
			SAMPLE_PRT("VISCA_DOWNRIGHT: X=%d	 Y =%d	 width: %d	 height:%d	\n",pCoord->rect.s32X,
							pCoord->rect.s32Y,pCoord->rect.u32Width,pCoord->rect.u32Height);
			break;
		case VISCA_STOP:
			pCoord->bEnable = HI_FALSE;
			break;
		
		case VISCA_ZOOMTELE:   //变倍大	  TODO: 暂未调好
			SAMPLE_PRT("<VISCA_ZOOMTELE> Crop: X=%d	 Y =%d	 width: %d	 height:%d	\n",pCoord->rect.s32X,
							pCoord->rect.s32Y,pCoord->rect.u32Width,pCoord->rect.u32Height);
			if(pCoord->rect.s32X == 0 || pCoord->rect.s32Y == 0) {
				iSpeed = 2;
			}
			else  {
				iSpeed = pCoord->rect.s32Y * pCoord->wSpeed/pCoord->rect.s32X;		
			}
			if(pCoord->vSpeed < 2) {
				pCoord->vSpeed = 2;
			}
			pCoord->rect.s32X += gs_step*pCoord->wSpeed;
			pCoord->rect.s32Y += gs_step*iSpeed;
			int centerW = (SENSOR_OUTPUT_W - PTZ_MIN_WIDTH)/2;
			int centerH = (SENSOR_OUTPUT_H - PTZ_MIN_HEIGHT)/2;
			if(pCoord->rect.s32X >= centerW) {
				pCoord->bEnable = HI_FALSE;
				pCoord->rect.s32X = centerW;
			}
			if(pCoord->rect.s32Y >= centerH) {
				pCoord->bEnable = HI_FALSE;
				pCoord->rect.s32Y = centerH;
			}			
			pCoord->rect.u32Width -= gs_step*pCoord->wSpeed *2;
			pCoord->rect.u32Height -= gs_step*iSpeed *2;
			if(pCoord->rect.u32Width <= PTZ_MIN_WIDTH) {
				pCoord->bEnable = HI_FALSE;
				pCoord->rect.u32Width = PTZ_MIN_WIDTH;
			}
			if(pCoord->rect.u32Height <= PTZ_MIN_HEIGHT) {
				pCoord->bEnable = HI_FALSE;
				pCoord->rect.u32Height = PTZ_MIN_HEIGHT;	
			}
			break;
			
		case VISCA_ZOOMWIDE:   //变倍小     TODO: 暂未调好
			SAMPLE_PRT("<VISCA_ZOOMWIDE> Crop: X=%d	 Y =%d	 width: %d	 height:%d	\n",pCoord->rect.s32X,
							pCoord->rect.s32Y,pCoord->rect.u32Width,pCoord->rect.u32Height);
			pCoord->vSpeed = pCoord->rect.s32Y * pCoord->wSpeed/pCoord->rect.s32X;		
			pCoord->rect.s32X -= gs_step*pCoord->wSpeed;
			pCoord->rect.s32Y -= gs_step*pCoord->vSpeed;
			if(pCoord->rect.s32X <= 0) {
				pCoord->rect.s32X = 0;
			}
			if(pCoord->rect.s32Y <= 0) {
				pCoord->rect.s32Y = 0;
			}			
			pCoord->rect.u32Width += gs_step*pCoord->wSpeed *2;
			pCoord->rect.u32Height += gs_step*pCoord->vSpeed *2;
			if(pCoord->rect.u32Width >= SENSOR_OUTPUT_W) {				
				pCoord->rect.u32Width = SENSOR_OUTPUT_W;
				pCoord->bEnable = HI_FALSE;
			}
			if(pCoord->rect.u32Height >= SENSOR_OUTPUT_H) {
				pCoord->rect.u32Height = SENSOR_OUTPUT_H;
				pCoord->bEnable = HI_FALSE;
			}
			break;
		default:
			SAMPLE_PRT("visca command error!!! mode =%d\n", pCoord->cmd_mode);
			break;	
	}
	return s32Ret;
}

/*协议处理线程*/
HI_VOID* RH_PTZControl(HI_VOID* pdata)
{
	RH_Coord *pCoord = &gs_Coord;	

	while(HI_TRUE == gs_PTZStart)
	{
		while(HI_TRUE == pCoord->bEnable)  {
			if(HI_TRUE == pCoord->bAuto) {
				RH_AUTOCoordCalc(pCoord);
			}
			else {
				RH_ManualCoordCalc(pCoord);
			}
			RH_CoordSet(pCoord);
			usleep(33*1000);
		}
		usleep(100*1000);
	}
	return ;
}

/*PTZ 云台控制开启*/
HI_S32 RH_PTZ_Contrl_Start(HI_VOID)
{
	HI_S32 s32Ret = HI_SUCCESS;
	int i = 0;
	s32Ret = pthread_create(&gs_PTZPid, 0, RH_PTZControl, (HI_VOID*)NULL);
	return s32Ret;
}

/*停止*/
HI_U32 RH_PTZ_Control_Stop(HI_VOID)
{
    /* join thread */
    if (HI_TRUE == gs_PTZStart)
    {
        gs_PTZStart = HI_FALSE;
		pthread_join(gs_PTZPid, 0);
    }
	return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


