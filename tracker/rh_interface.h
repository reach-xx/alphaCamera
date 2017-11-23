#ifndef __RH_INTERFACE_H
#define __RH_INTERFACE_H

#include "sample_comm.h"
#include "hi_comm_video.h"
#include "hi_comm_vgs.h"


#define VGS_LINE_MAX_POINT			50


typedef struct __RH_RECT_ARRAY_S
{
    HI_U16 u16Num;
	VGS_DRAW_LINE_S  stPolygon[50];  //polygon point
} RH_POLY_ARRAY_S;


typedef struct __RH_WB_ATTR_S
{
	ISP_MWB_ATTR_S stMWBattr;
}RH_WB_ATTR_S;

/*借助VDA的方法实现自动跟踪*/
HI_U32 RH_MPI_VDA_AutoTrk(VPSS_CHN VpssChn);

/******************************************************************************
* funciton : vda MD mode thread process
******************************************************************************/
HI_VOID* RH_MPI_VDA_MdGetResult(HI_VOID* pdata);
/*VDA MD 帧间差值法*/
//HI_U32 RH_MPI_MD_RefMethod(VDA_CHN VdaChn, HI_U32 u32Chn, SIZE_S* pstSize);
/******************************************************************************
* funciton : stop vda, and stop vda thread -- MD
******************************************************************************/
HI_VOID RH_MPI_VDA_MdStop(VDA_CHN VdaChn, HI_U32 u32Chn);
/******************************************************************************
* funciton : start get venc stream process thread
******************************************************************************/
HI_S32 RH_MPI_VENC_StartGetStream(HI_S32 s32Cnt);
/******************************************************************************
* funciton : stop get venc stream process.
******************************************************************************/
HI_S32 RH_MPI_VENC_StopGetStream();
/*Draw line create VGS task*/
HI_U32 RH_MPI_VGS_CreateLineTask(VPSS_GRP VpssGrp,VPSS_CHN VpssChn);
/*destroy line task*/
HI_U32 RH_MPI_VGS_DestroyLineTask();
/******************************************************************************
* funciton : ISP Run
******************************************************************************/
HI_S32 RH_MPI_ISP_Run();
/*Start VI capture*/
HI_S32 RH_MPI_VI_StartVi(SAMPLE_VI_CONFIG_S* pstViConfig);
/*stop VI capture*/
HI_S32 RH_MPI_VI_StopVi(SAMPLE_VI_CONFIG_S* pstViConfig);
/*############################################################################
##
##     ISP WB setting
##
#############################################################################*/
HI_U32 RH_MPI_ISP_SetWBMode(HI_U8 mode);
/*############################################################################
##
##     ISP exposure setting
##
#############################################################################*/
HI_U32 RH_MPI_ISP_SetAeMode(HI_U8 mode);

HI_U32 RH_MPI_ISP_SetWBGain(RH_WB_ATTR_S *prhAttr);
/*Draw Line*/
HI_U32 RH_MPI_DrawLine(RH_POLY_ARRAY_S *pstPoint);
HI_U32 RH_MPI_YUV_TrackingStop();
/*1280x720 YUV*/
HI_U32 RH_MPI_YUV_Tracking(VPSS_GRP VpssGrp,VPSS_CHN VpssChn);

/*矫正畸变图像参数*/
HI_U32 RH_MPI_ISP_SetLDCAttr(HI_U32 s32Ratio);

#endif //#ifdef __RH_INTERFACE_H
