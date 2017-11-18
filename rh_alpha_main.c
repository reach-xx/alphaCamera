#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


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

#include "sample_comm.h"


/*4K 缩放1080P   分辨率*/
#define HI_ZOOM_WIDTH		1920
#define HI_ZOOM_HEIGHT		1080
/*4K 裁剪1080P   分辨率*/
#define HI_CROP_WIDTH		1920
#define HI_CROP_HEIGHT		1080

/*跟踪分辨率*/
#define HI_TRACK_WIDTH		640
#define HI_TRACK_HEIGHT		360



/******************************************************************************
* function : to process abnormal case
******************************************************************************/
void RH_MPI_CAM_HandleSig(HI_S32 signo)
{
	if (SIGINT == signo || SIGTERM == signo)
	{
		SAMPLE_COMM_ISP_Stop();
		SAMPLE_COMM_SYS_Exit();
		printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
	}
	exit(-1);
}

/******************************************************************************
* function : to process abnormal case - the case of stream venc
******************************************************************************/
void RH_MPI_CAM_StreamHandleSig(HI_S32 signo)
{

	if (SIGINT == signo || SIGTERM == signo)
	{
		SAMPLE_COMM_SYS_Exit();
		printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
	}

	exit(0);
}


/*绑定VI------VPSS组*/
HI_S32 RH_MPI_Bind_VI_VPSS(VI_CHN 		   ViChn, VPSS_GRP VpssGrp)
{
	HI_S32    s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
	
    stSrcChn.enModId  = HI_ID_VIU;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = ViChn;

    stDestChn.enModId  = HI_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT(" failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }	
	return s32Ret;
}



/*裁剪VPSS通道*/
HI_S32 RH_MPI_VPSS_SetChnCrop(VPSS_CHN VpssChn , RECT_S *pCropRect)
{

	VPSS_CROP_INFO_S stVpssCropInfo = {0};
	VPSS_GRP  	VpssGrp =  0;	
	HI_S32 		s32Ret = HI_SUCCESS ;
	
    stVpssCropInfo.bEnable 				= HI_TRUE;
    stVpssCropInfo.enCropCoordinate 	= VPSS_CROP_ABS_COOR;
    stVpssCropInfo.stCropRect.s32X 		= pCropRect->s32X;
    stVpssCropInfo.stCropRect.s32Y 		= pCropRect->s32Y;
    stVpssCropInfo.stCropRect.u32Width  = pCropRect->u32Width;
    stVpssCropInfo.stCropRect.u32Height = pCropRect->u32Height;
    s32Ret = HI_MPI_VPSS_SetChnCrop(VpssGrp, VpssChn, &stVpssCropInfo);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("set VPSS group crop Failed! 0x%x\n", s32Ret);
        return s32Ret;
    }

	return s32Ret;

}


/*主进程例程*/
HI_U32 RH_AlphaCAM_Routine(HI_VOID)
{
	HI_S32				s32Ret = HI_SUCCESS;
	PIC_SIZE_E			enSize = PIC_UHD4K;
	VIDEO_NORM_E		enNorm = VIDEO_ENCODING_MODE_NTSC; //VIDEO_ENCODING_MODE_PAL;
	HI_S32				pixel_format = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	HI_U32 				u32Profile = 2;		/*0: baseline; 1:MP; 2:HP;  3:svc_t */
	SAMPLE_VI_CONFIG_S	stViConfig =	{0};
	VPSS_GRP_ATTR_S 	stGrpVpssAttr = {0};
	VPSS_CHN_ATTR_S 	stChnAttr = 	{0};
	VPSS_GRP			VpssGrp =  0;	 
	VPSS_CHN			VpssChn =  1;
	VPSS_CHN			VpssChn2 = 2;
	VPSS_CHN			VpssChn3 = 3;
	VENC_CHN			VencChn = 0;
	VPSS_CHN_MODE_S 	stVpssMode = {0};
	VB_CONF_S			stVbConf;
	HI_U32				u32BlkSize;
	VI_DEV				ViDev		= 0;
	HI_S32				u32Depth	= 2;
	VI_CHN				ViChn		= 0;
	SIZE_S				stSize = {0};
	VENC_GOP_ATTR_S 	stGopAttr;
	SAMPLE_RC_E 		enRcMode;	 //SAMPLE_RC_CBR,VBR,AVBR,FIXQP
	HI_S32 				i , S32EncChnNum = 3;   //编码三路
	
	memset(&stVbConf,0,sizeof(VB_CONF_S));	
	SAMPLE_COMM_VI_GetSizeBySensor(&enSize);	

	stVbConf.u32MaxPoolCnt = 32;
	u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(enNorm, PIC_HD1080, pixel_format, SAMPLE_SYS_ALIGN_WIDTH);
	stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt = 20;

	u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(enNorm, PIC_UHD4K, pixel_format, SAMPLE_SYS_ALIGN_WIDTH);
	stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
	stVbConf.astCommPool[1].u32BlkCnt = 10;

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enNorm, PIC_UHD4K, &stSize);
	if (HI_SUCCESS != s32Ret)	  {
		SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed! s32Ret:0x%x \n",s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (HI_SUCCESS != s32Ret)	{
		SAMPLE_PRT("system init failed with s32Ret:0x%x!\n", s32Ret);
		goto END_1080P_1;
	}

	stViConfig.enViMode   = SENSOR_TYPE;
	s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
	if (HI_SUCCESS != s32Ret)	{
		SAMPLE_PRT("start vi failed!  s32Ret:0x%x\n",s32Ret); 
		return s32Ret;
	}

	s32Ret = HI_MPI_VI_SetFrameDepth(ViChn, u32Depth);
	if(HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("set VI frame depth error, s32Ret:0x%x\n",s32Ret);
	}	

	stGrpVpssAttr.u32MaxW = stSize.u32Width;
	stGrpVpssAttr.u32MaxH = stSize.u32Height;
	stGrpVpssAttr.bNrEn = HI_TRUE;
	stGrpVpssAttr.enDieMode = VPSS_DIE_MODE_NODIE;
	stGrpVpssAttr.stNrAttr.enNrType = VPSS_NR_TYPE_VIDEO;
	stGrpVpssAttr.stNrAttr.stNrVideoAttr.enNrRefSource = VPSS_NR_REF_FROM_RFR;
	stGrpVpssAttr.stNrAttr.u32RefFrameNum = 2;
	stGrpVpssAttr.enPixFmt =   pixel_format;
	s32Ret = HI_MPI_VPSS_CreateGrp(VpssGrp, &stGrpVpssAttr);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("HI_MPI_VPSS_CreateGrp failed with %#x!\n", s32Ret);
		goto END_1080P_2;
	}

	s32Ret = HI_MPI_VPSS_StartGrp(VpssGrp);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("HI_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
		goto END_1080P_3;
	}	

	/*通道1为 --------- 4K 缩放1080P*/
	memset(&stChnAttr, 0, sizeof(stChnAttr));
	stChnAttr.s32SrcFrameRate = 30;
	stChnAttr.s32DstFrameRate = 30;
	s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stChnAttr);		
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("HI_MPI_VPSS_SetChnAttr failed with %#x!\n", s32Ret);
		goto END_1080P_4;
	}
	
	stVpssMode.enChnMode		= VPSS_CHN_MODE_USER;
	stVpssMode.bDouble			= HI_FALSE;
	stVpssMode.enPixelFormat	= pixel_format;
	stVpssMode.u32Width 		= HI_ZOOM_WIDTH;			   
	stVpssMode.u32Height		= HI_ZOOM_HEIGHT;			  
	stVpssMode.enCompressMode	= COMPRESS_MODE_NONE;
	s32Ret =  HI_MPI_VPSS_SetChnMode(VpssGrp, VpssChn, &stVpssMode);
	if (s32Ret != HI_SUCCESS)	 {
		SAMPLE_PRT("HI_MPI_VPSS_SetChnMode failed with %#x!\n", s32Ret);
		goto END_1080P_4;
	}

	s32Ret =  HI_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
	if (s32Ret != HI_SUCCESS)	 {
		SAMPLE_PRT("HI_MPI_VPSS_EnableChn failed with %#x!\n", s32Ret);
		goto END_1080P_4;
	}


	/*通道2 -----------裁剪1080P*/
	RECT_S crop_rect = {200,200,HI_CROP_WIDTH,HI_CROP_HEIGHT};
	RH_MPI_VPSS_SetChnCrop(VpssChn2,&crop_rect);
	
	memset(&stChnAttr, 0, sizeof(stChnAttr));
	stChnAttr.s32SrcFrameRate = 30;
	stChnAttr.s32DstFrameRate = 30;
	s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn2, &stChnAttr); 	
	if (s32Ret != HI_SUCCESS)	 {
		SAMPLE_PRT("HI_MPI_VPSS_SetChnAttr failed with %#x!\n", s32Ret);
		goto END_1080P_5;
	}
	stVpssMode.enChnMode	  = VPSS_CHN_MODE_USER;
	stVpssMode.bDouble		  = HI_FALSE;
	stVpssMode.enPixelFormat  = pixel_format;
	stVpssMode.u32Width 	  = stSize.u32Width;			   
	stVpssMode.u32Height	  = stSize.u32Height;			  
	stVpssMode.enCompressMode = COMPRESS_MODE_NONE;
	s32Ret =  HI_MPI_VPSS_SetChnMode(VpssGrp, VpssChn2, &stVpssMode);
	if (s32Ret != HI_SUCCESS)	 {
		SAMPLE_PRT("HI_MPI_VPSS_SetChnMode failed with %#x!\n", s32Ret);
		goto END_1080P_5;
	}

	s32Ret =  HI_MPI_VPSS_EnableChn(VpssGrp, VpssChn2);
	if (s32Ret != HI_SUCCESS)	 {
		SAMPLE_PRT("HI_MPI_VPSS_EnableChn failed with %#x!\n", s32Ret);
		goto END_1080P_5;
	}	

	/*通道三 ，用于跟踪*/
	memset(&stChnAttr, 0, sizeof(stChnAttr));
	stChnAttr.s32SrcFrameRate = 30;
	stChnAttr.s32DstFrameRate = 4;
	s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn3, &stChnAttr); 	
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("HI_MPI_VPSS_SetChnAttr failed with %#x!\n", s32Ret);
		goto END_1080P_5;
	}
	stVpssMode.enChnMode		= VPSS_CHN_MODE_USER;
	stVpssMode.bDouble			= HI_FALSE;
	stVpssMode.enPixelFormat	= PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	stVpssMode.u32Width 		= HI_TRACK_WIDTH;				
	stVpssMode.u32Height		= HI_TRACK_HEIGHT;			   
	stVpssMode.enCompressMode	= COMPRESS_MODE_NONE;
	s32Ret =  HI_MPI_VPSS_SetChnMode(VpssGrp, VpssChn3, &stVpssMode);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("HI_MPI_VPSS_SetChnMode failed with %#x!\n", s32Ret);
		goto END_1080P_5;
	}
	
	s32Ret =  HI_MPI_VPSS_SetDepth(VpssGrp, VpssChn3, u32Depth);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("HI_MPI_VPSS_SetDepth failed with %#x!\n", s32Ret);
		goto END_1080P_5;
	}

	s32Ret =  HI_MPI_VPSS_EnableChn(VpssGrp, VpssChn3);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("HI_MPI_VPSS_EnableChn failed with %#x!\n", s32Ret);
		goto END_1080P_5;
	}

	s32Ret = RH_MPI_Bind_VI_VPSS(ViChn, VpssGrp);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		goto END_1080P_5;
	}
	
	VPSS_CHN curVpssChn = 0;
	
	for (i = 0; i < S32EncChnNum; i++) {
		
		switch(i)
		{
			case 0:     //第一路编码
				VencChn = 0;   //4K缩放1080P编码
				curVpssChn = VpssChn;
				enSize 	   = PIC_HD1080;  		//编码1080P			
				break;
			case 1:
				VencChn = 1;    //crop的1080p编码
				curVpssChn = VpssChn2;
				enSize	   = PIC_HD1080;			//编码1080P
				break;
			case 2:
				VencChn = 2;    //跟踪分析编码
				curVpssChn = VpssChn3;
				enSize 	   = PIC_360P;			
				break;
			default:
				SAMPLE_PRT("Venc failed!  EncChnNum=%d\n",i);
				break;
		}
		
		SAMPLE_COMM_VENC_GetGopAttr(VENC_GOPMODE_NORMALP,&stGopAttr,enNorm);
		enRcMode = SAMPLE_RC_FIXQP;     //固定QP值编码

		/*编码参数设置*/
		s32Ret = SAMPLE_COMM_VENC_Start(VencChn, PT_H264,
							enNorm, enSize, enRcMode,u32Profile,&stGopAttr);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Start Venc start failed!\n");
			goto END_1080P_6;
		}		
		
	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, curVpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc  bind vpss failed!\n");
	        goto END_1080P_6;
	    }		
		printf("----------------------------------Venc:%d\n",VencChn);
	}
	
	/*启动编码接收数据*/
	s32Ret = RH_MPI_VENC_StartGetStream(S32EncChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        //goto END_1080P_6;
    }	

	while(1) {
		sleep(10);
	}

END_1080P_1:
	

END_1080P_2:
	

END_1080P_3:


END_1080P_4:
	

END_1080P_5:

END_1080P_6:

	return s32Ret;
}


/*-----------------------MAIN-----------------------------------------*/
int main(int argc, char* argv[])
{
	HI_S32	s32Ret = HI_SUCCESS;

    signal(SIGINT, RH_MPI_CAM_HandleSig);
    signal(SIGTERM, RH_MPI_CAM_HandleSig);
	
	s32Ret = RH_AlphaCAM_Routine();
	if (HI_SUCCESS == s32Ret)
	{ 
		printf("program exit normally!\n"); 
	}
	else
	{ 
		printf("program exit abnormally!\n"); 
	}
	
	return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

