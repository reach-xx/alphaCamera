/******************************************************************************
  Some simple Hisilicon Hi3531 video encode functions.  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-2 Created******************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include "sample_comm.h"

//#define HISI3519_RTSP_FILE
#define HISI3519_RTSP
//#define COUNT_FPS

#ifdef  HISI3519_RTSP
#include <unistd.h>  
#include <sys/shm.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define TEXT_SZ (150000 * 1)  
#define MAX_INTERRUPT_TIME	10    //seconds

#define RH_FILE_NAME_LEN		128

struct shared_use_st  
{  
    int written;
	int IsStart;   // 0 ㊣赤那?赤㏒?1∩?那?㏒? 1㊣赤那??a??∩?那?
	unsigned int count;
    unsigned int fFrameSize;
	unsigned char text0[TEXT_SZ];
	unsigned char text1[TEXT_SZ];
	unsigned char text2[TEXT_SZ];
	unsigned char text3[TEXT_SZ];
	unsigned char text4[TEXT_SZ];
	unsigned char text5[TEXT_SZ];
    unsigned char text6[TEXT_SZ];
	unsigned char text7[TEXT_SZ];
	unsigned char text8[TEXT_SZ];
	unsigned char text9[TEXT_SZ];
	//unsigned char* text; 
};  

static int running = 1;  
static void *shm = NULL;  
static struct shared_use_st *shared = NULL;  
static int shmid; 

#ifdef HISI3519_RTSP_FILE 
static FILE *fp_rtsp;
#endif

#endif


const HI_U8 g_SOI[2] = {0xFF, 0xD8};
const HI_U8 g_EOI[2] = {0xFF, 0xD9};
static pthread_t gs_VencPid;
static pthread_t gs_VISVAPid;
static pthread_t gs_VISVAPid2;
static SAMPLE_VENC_GETSTREAM_PARA_S gs_stPara;
static HI_S32 gs_s32SnapCnt = 0;
HI_CHAR* DstBuf = NULL;
#define TEMP_BUF_LEN 8
#define MAX_THM_SIZE (64*1024)


#ifdef __READ_ALL_FILE__
static HI_S32 FileTrans_GetThmFromJpg(HI_CHAR* JPGPath, HI_U32* DstSize)
{
    HI_S32 s32RtnVal = 0;
    FILE* fpJpg = NULL;
    HI_CHAR tempbuf[TEMP_BUF_LEN] = {0};
    HI_S32 bufpos = 0;
    HI_CHAR startflag[2] = {0xff, 0xd8};
    HI_S32 startpos = 0;
    HI_CHAR endflag[2] = {0xff, 0xd9};
    HI_S32 endpos = 0;
    fpJpg = fopen(JPGPath, "rb");
    HI_CHAR* pszFile = NULL;
    HI_S32 fd = 0;
    HI_S32 s32I = 0;
    struct stat stStat;
    memset(&stStat, 0, sizeof(struct stat));
    if (NULL == fpJpg)
    {
        printf("file %s not exist!\n", JPGPath);
        return HI_FAILURE;
    }
    else
    {
        fd = fileno(fpJpg);
        fstat(fd, &stStat);
        pszFile = (HI_CHAR*)malloc(stStat.st_size);
        if ((NULL == pszFile) || (stStat.st_size < 6))
        {
            fclose(fpJpg);
            printf("memory malloc fail!\n");
            return HI_FAILURE;
        } 
 
        if (fread(pszFile, stStat.st_size , 1, fpJpg) <= 0)
        {
            fclose(fpJpg);
            free(pszFile);
            printf("fread jpeg src fail!\n");
            return HI_FAILURE;
        }

        fclose(fpJpg);
        HI_U16 u16THMLen = 0;
        u16THMLen = (pszFile[4] << 8) + pszFile[5];
        while (s32I < stStat.st_size)
        {
            tempbuf[bufpos] = pszFile[s32I++];
            if (bufpos > 0)
            {
                if (0 == memcmp(tempbuf + bufpos - 1, startflag, sizeof(startflag)))
                {
                    startpos = s32I - 2;
                    if (startpos < 0)
                    {
                        startpos = 0;
                    }
                }
                if (0 == memcmp(tempbuf + bufpos - 1, endflag, sizeof(endflag)))
                {
                    if (u16THMLen == s32I)
                    {
                        endpos = s32I;
                        break;
                    }
                    else
                    {
                        endpos = s32I;
                        break;
                    }
                }
            }
            bufpos++;
            if (bufpos == (TEMP_BUF_LEN - 1))
            {
                if (tempbuf[bufpos - 1] != 0xFF)
                {
                    bufpos = 0;
                }

            }
            else if (bufpos > (TEMP_BUF_LEN - 1))
            {
                bufpos = 0;
            }

        }

    }
    if (endpos - startpos <= 0)
    {
        free(pszFile);
        printf("get .thm 11 fail!\n");
        return HI_FAILURE;
    }

    if (endpos - startpos >= stStat.st_size)
    {
        free(pszFile);
        printf("NO DCF info, get .thm 22 fail!\n");
        return HI_FAILURE;
    }

    HI_CHAR* temp = pszFile + startpos;
	if(MAX_THM_SIZE < (endpos - startpos))
	{
		printf("Thm is too large than MAX_THM_SIZE, get .thm 33 fail!\n");
        return HI_FAILURE;
	}

	HI_CHAR* cDstBuf = (HI_CHAR*)malloc(endpos - startpos);
	if (NULL == cDstBuf)
	{
		printf("memory malloc fail!\n");
		return HI_FAILURE;
	}

    memcpy(cDstBuf, temp, endpos - startpos);

	DstBuf = cDstBuf;
    *DstSize = endpos - startpos;
    free(pszFile);

    return HI_SUCCESS;
}

#else
static HI_S32 FileTrans_GetThmFromJpg(HI_CHAR* JPGPath, HI_U32* DstSize)
{
    HI_CHAR tempbuf[TEMP_BUF_LEN] = {0};
    HI_S32 bufpos = 0;
    HI_CHAR startflag[2] = {0xff, 0xd8};
    HI_S32 startpos = 0;
    HI_CHAR endflag[2] = {0xff, 0xd9};
    HI_S32 endpos = 0;
	HI_BOOL bStartMatch = HI_FALSE;
	
    HI_S32 fd = 0;
    struct stat stStat;
    memset(&stStat, 0, sizeof(struct stat));

	FILE* fpJpg = NULL;
	fpJpg = fopen(JPGPath, "rb");
	if (NULL == fpJpg)
    {
        printf("file %s not exist!\n", JPGPath);
        return HI_FAILURE;
    }
    else
    {
        fd = fileno(fpJpg);
        fstat(fd, &stStat);
		
        while (!feof(fpJpg))
        {
            tempbuf[bufpos]=getc(fpJpg);
            if (bufpos > 0)
            {
                if (0 == memcmp(tempbuf + bufpos - 1, startflag, sizeof(startflag)))
                {
                    startpos = ftell(fpJpg)-2;
                    if (startpos < 0)
                    {
                        startpos = 0;
                    }
					bStartMatch = HI_TRUE;
                }
                if (0 == memcmp(tempbuf + bufpos - 1, endflag, sizeof(endflag)))
                {
                    endpos = ftell(fpJpg);
					if(HI_TRUE == bStartMatch)
					{
                    	break;
					}	
                }
            }
            bufpos++;
			
            if (bufpos == (TEMP_BUF_LEN - 1))
            {
                if (tempbuf[bufpos - 1] != 0xFF)
                {
                    bufpos = 0;
                }
            }
            else if (bufpos > (TEMP_BUF_LEN - 1))
            {
				if (tempbuf[bufpos -1] == 0xFF)
				{
					tempbuf[0] = 0xFF;
					bufpos = 1;
				}
				else
				{	
                	bufpos = 0;
				}	
            }
        }
    }
	
    if (endpos - startpos <= 0)
    {
        printf("get .thm 11 fail!\n");
		fclose(fpJpg);
        return HI_FAILURE;
    }

	if (endpos - startpos > MAX_THM_SIZE)
	{
		printf("Thm is too large than MAX_THM_SIZE, get .thm 22 fail!\n");
		fclose(fpJpg);
        return HI_FAILURE;
	}

    if (endpos - startpos >= stStat.st_size)
    {
        printf("NO DCF info, get .thm 33 fail!\n");
		fclose(fpJpg);
        return HI_FAILURE;
    }

	HI_CHAR* cDstBuf = (HI_CHAR*)malloc(endpos - startpos);
	if (NULL == cDstBuf)
	{
		printf("memory malloc fail!\n");
		fclose(fpJpg);
		return HI_FAILURE;
	}

	fseek(fpJpg, (long)startpos, SEEK_SET);
	*DstSize = fread(cDstBuf,1,endpos-startpos,fpJpg);
	if(*DstSize != (endpos - startpos))
	{
		free(cDstBuf);
		printf("fread fail!\n");
		fclose(fpJpg);
		return HI_FAILURE;
	}
	
	DstBuf = cDstBuf;
	fclose(fpJpg);

    return HI_SUCCESS;
}
#endif






/******************************************************************************
* function : Set venc memory location
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_MemConfig(HI_VOID)
{
    HI_S32 i = 0;
    HI_S32 s32Ret;
    HI_CHAR* pcMmzName;
    MPP_CHN_S stMppChnVENC;
    /* group, venc max chn is 64*/
    for (i = 0; i < 64; i++)
    {
        stMppChnVENC.enModId = HI_ID_VENC;
        stMppChnVENC.s32DevId = 0;
        stMppChnVENC.s32ChnId = i;
        pcMmzName = NULL;

        /*venc*/
        s32Ret = HI_MPI_SYS_SetMemConf(&stMppChnVENC, pcMmzName);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_SYS_SetMemConf with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

/******************************************************************************
* function : venc bind vpss
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_BindVpss(VENC_CHN VeChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = HI_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;
    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;
    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

/******************************************************************************
* function : venc unbind vpss
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_UnBindVpss(VENC_CHN VeChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = HI_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;
    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;
    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

/******************************************************************************
* function : venc bind vo
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_BindVo(VO_DEV VoDev, VO_CHN VoChn, VENC_CHN VeChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = HI_ID_VOU;
    stSrcChn.s32DevId = VoDev;
    stSrcChn.s32ChnId = VoChn;
    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;
    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

/******************************************************************************
* function : venc unbind vo
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_UnBindVo(VENC_CHN GrpChn, VO_DEV VoDev, VO_CHN VoChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = HI_ID_VOU;
    stSrcChn.s32DevId = VoDev;
    stSrcChn.s32ChnId = VoChn;
    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = GrpChn;
    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

/******************************************************************************
* function : vdec bind venc
******************************************************************************/
HI_S32 SAMPLE_COMM_VDEC_BindVenc(VDEC_CHN VdChn, VENC_CHN VeChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = HI_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;
    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;
    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

/******************************************************************************
* function : venc unbind vo
******************************************************************************/
HI_S32 SAMPLE_COMM_VDEC_UnBindVenc(VDEC_CHN VdChn, VENC_CHN VeChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = HI_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;
    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;

    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

/******************************************************************************
* funciton : get file postfix according palyload_type.
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_GetFilePostfix(PAYLOAD_TYPE_E enPayload, char* szFilePostfix)
{
    if (PT_H264 == enPayload)
    {
        strcpy(szFilePostfix, ".h264");
    }
    else if (PT_H265 == enPayload)
    {
        strcpy(szFilePostfix, ".h265");
    }
    else if (PT_JPEG == enPayload)
    {
        strcpy(szFilePostfix, ".jpg");
    }
    else if (PT_MJPEG == enPayload)
    {
        strcpy(szFilePostfix, ".mjp");
    }
    else if (PT_MP4VIDEO == enPayload)
    {
        strcpy(szFilePostfix, ".mp4");
    }
    else
    {
        SAMPLE_PRT("payload type err!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}


HI_S32 SAMPLE_COMM_VENC_GetGopAttr(VENC_GOP_MODE_E enGopMode,VENC_GOP_ATTR_S *pstGopAttr,VIDEO_NORM_E enNorm)
{
	switch(enGopMode)
	{
		case VENC_GOPMODE_NORMALP:
		pstGopAttr->enGopMode  = VENC_GOPMODE_NORMALP;
		pstGopAttr->stNormalP.s32IPQpDelta = 2;
		break;

		case VENC_GOPMODE_SMARTP:
		pstGopAttr->enGopMode  = VENC_GOPMODE_SMARTP;
		pstGopAttr->stSmartP.s32BgQpDelta = 4;
		pstGopAttr->stSmartP.s32ViQpDelta = 2;
		pstGopAttr->stSmartP.u32BgInterval = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 75 : 90;
		break;
		case VENC_GOPMODE_DUALP:
		pstGopAttr->enGopMode  = VENC_GOPMODE_DUALP;
		pstGopAttr->stDualP.s32IPQpDelta  = 4;
		pstGopAttr->stDualP.s32SPQpDelta  = 2;
		pstGopAttr->stDualP.u32SPInterval = 3;
		break;
		case VENC_GOPMODE_BIPREDB:
		pstGopAttr->enGopMode  = VENC_GOPMODE_BIPREDB;
		pstGopAttr->stBipredB.s32BQpDelta  = -2;
		pstGopAttr->stBipredB.s32IPQpDelta = 2;
		pstGopAttr->stBipredB.u32BFrmNum   = 3;
		break;
		default:
		SAMPLE_PRT("not support the gop mode !\n");
		return HI_FAILURE;
		break;
	}
	return HI_SUCCESS;
}




/******************************************************************************
* funciton : save mjpeg stream.
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_SaveMJpeg(FILE* fpMJpegFile, VENC_STREAM_S* pstStream)
{
    VENC_PACK_S*  pstData;
    HI_U32 i;
    //fwrite(g_SOI, 1, sizeof(g_SOI), fpJpegFile); //in Hi3531, user needn't write SOI!
    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        pstData = &pstStream->pstPack[i];
        fwrite(pstData->pu8Addr + pstData->u32Offset, pstData->u32Len - pstData->u32Offset, 1, fpMJpegFile);
        fflush(fpMJpegFile);
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : save jpeg stream.
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_SaveJpeg(FILE* fpJpegFile, VENC_STREAM_S* pstStream)
{
    VENC_PACK_S*  pstData;
    HI_U32 i;
    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        pstData = &pstStream->pstPack[i];
        fwrite(pstData->pu8Addr + pstData->u32Offset, pstData->u32Len - pstData->u32Offset, 1, fpJpegFile);
        fflush(fpJpegFile);
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : save H264 stream
******************************************************************************/

#ifdef COUNT_FPS
static unsigned  long diff = 0;
static int count = 0;
#endif

HI_S32 SAMPLE_COMM_VENC_SaveH264(FILE* fpH264File, VENC_STREAM_S* pstStream)
{
    HI_S32 i;

	#ifdef  HISI3519_RTSP
	unsigned int len = 0;
	unsigned int len2 = 0;
	unsigned int num = 0;

	unsigned char* temp;
	if(1 == shared->IsStart)
	{
    	temp = (unsigned char* )malloc(TEXT_SZ * sizeof(unsigned char) * 10);
	}

    //printf("IsStart = %d \n\n\n", shared->IsStart);
	//printf("count = %d \n\n\n", shared->count);

	#endif

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
		#if 0
        fwrite(pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,
               pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, fpH264File);

        fflush(fpH264File);
		#endif

		#ifdef  HISI3519_RTSP
		if(1 == shared->IsStart)
		{
			if(len <= TEXT_SZ * 6)
			{
				memcpy(temp + len, pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset, pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset); 
				len += pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset;
			}
		}
   		#endif
	}

	#ifdef COUNT_FPS
	count++;

	if(!diff)
    {
		struct  timeval start;       
        gettimeofday(&start,NULL);
		diff = 1000000*start.tv_sec + start.tv_usec;
    }
	else
	{		
        unsigned  long timep;
        struct  timeval end;
       
        gettimeofday(&end,NULL);
		timep = 1000000*end.tv_sec + end.tv_usec - diff;

		if(timep >= 1 * 1000000)
		{
        	diff = 1000000*end.tv_sec + end.tv_usec;
			
            printf("********************************************************\n");
        	printf("diff time  is %ld , count(%d) \n", timep, count);
			printf("********************************************************\n\n\n");
			count = 0;
		}
	}
	#endif



	#ifdef  HISI3519_RTSP

	while(shared->written == 1 && shared->IsStart == 1)  
    {  
		//usleep(10 * 1000); 
		//printf("Encoder Waiting...\n"); 
		shared->count++;

		if(shared->count >= MAX_INTERRUPT_TIME * 1000 / 10)
		{
			shared->IsStart = 0;
			free(temp);
			temp = NULL;
		}
    }  

	shared->count = 0;
	if(!shared->IsStart)
	{
		//printf("\n\n\nwhat's up !!!!!!!!!!!!!!! \n\n\n");
		return HI_SUCCESS;		
	}



	num = len / TEXT_SZ;

	if(0 == num)
	{
		memcpy(shared->text0, temp, len);

		#ifdef HISI3519_RTSP_FILE 
		fwrite(shared->text0, 1, len, fp_rtsp);
		#endif
	}
	else if(1 == num)
	{
		memcpy(shared->text0, temp, TEXT_SZ);
		memcpy(shared->text1, temp + TEXT_SZ, len - TEXT_SZ);

		#ifdef HISI3519_RTSP_FILE
		fwrite(shared->text0, TEXT_SZ, 1, fp_rtsp); 
		fwrite(shared->text1, len - TEXT_SZ, 1, fp_rtsp); 
		#endif
	}
	else if(2 == num)
	{
		memcpy(shared->text0, temp, TEXT_SZ);
		memcpy(shared->text1, temp + TEXT_SZ, TEXT_SZ);
		memcpy(shared->text2, temp + 2*TEXT_SZ, len - 2*TEXT_SZ);

		#ifdef HISI3519_RTSP_FILE
		fwrite(shared->text0, TEXT_SZ, 1, fp_rtsp); 
		fwrite(shared->text1, TEXT_SZ, 1, fp_rtsp); 
		fwrite(shared->text2, len - 2*TEXT_SZ, 1, fp_rtsp); 
		#endif
	}
	else if(3 == num)
	{
		memcpy(shared->text0, temp, TEXT_SZ);
		memcpy(shared->text1, temp + TEXT_SZ, TEXT_SZ);
		memcpy(shared->text2, temp + 2*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text3, temp + 3*TEXT_SZ, len - 3*TEXT_SZ);

		#ifdef HISI3519_RTSP_FILE
		fwrite(shared->text0, TEXT_SZ, 1, fp_rtsp); 
		fwrite(shared->text1, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text2, TEXT_SZ, 1, fp_rtsp);  
		fwrite(shared->text3, len - 3*TEXT_SZ, 1, fp_rtsp); 
		#endif
	}
	else if(4 == num)
	{
		memcpy(shared->text0, temp, TEXT_SZ);
		memcpy(shared->text1, temp + TEXT_SZ, TEXT_SZ);
		memcpy(shared->text2, temp + 2*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text3, temp + 3*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text4, temp + 4*TEXT_SZ, len - 4*TEXT_SZ);

		#ifdef HISI3519_RTSP_FILE
		fwrite(shared->text0, TEXT_SZ, 1, fp_rtsp); 
		fwrite(shared->text1, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text2, TEXT_SZ, 1, fp_rtsp);  
		fwrite(shared->text3, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text4, len - 4*TEXT_SZ, 1, fp_rtsp); 
		#endif
	}
	else if(5 == num)
	{
		memcpy(shared->text0, temp, TEXT_SZ);
		memcpy(shared->text1, temp + TEXT_SZ, TEXT_SZ);
		memcpy(shared->text2, temp + 2*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text3, temp + 3*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text4, temp + 4*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text5, temp + 5*TEXT_SZ, len - 5*TEXT_SZ);

		#ifdef HISI3519_RTSP_FILE
		fwrite(shared->text0, TEXT_SZ, 1, fp_rtsp); 
		fwrite(shared->text1, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text2, TEXT_SZ, 1, fp_rtsp);  
		fwrite(shared->text3, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text4, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text5, len - 5*TEXT_SZ, 1, fp_rtsp); 
		#endif
	}
	else if(6 == num)
	{
		memcpy(shared->text0, temp, TEXT_SZ);
		memcpy(shared->text1, temp + TEXT_SZ, TEXT_SZ);
		memcpy(shared->text2, temp + 2*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text3, temp + 3*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text4, temp + 4*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text5, temp + 5*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text6, temp + 6*TEXT_SZ, len - 6*TEXT_SZ);

		#ifdef HISI3519_RTSP_FILE
		fwrite(shared->text0, TEXT_SZ, 1, fp_rtsp); 
		fwrite(shared->text1, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text2, TEXT_SZ, 1, fp_rtsp);  
		fwrite(shared->text3, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text4, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text5, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text6, len - 6*TEXT_SZ, 1, fp_rtsp); 
		#endif
	}
	else if(7 == num)
	{
		memcpy(shared->text0, temp, TEXT_SZ);
		memcpy(shared->text1, temp + TEXT_SZ, TEXT_SZ);
		memcpy(shared->text2, temp + 2*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text3, temp + 3*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text4, temp + 4*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text5, temp + 5*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text6, temp + 6*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text7, temp + 7*TEXT_SZ, len - 7*TEXT_SZ);

		#ifdef HISI3519_RTSP_FILE
		fwrite(shared->text0, TEXT_SZ, 1, fp_rtsp); 
		fwrite(shared->text1, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text2, TEXT_SZ, 1, fp_rtsp);  
		fwrite(shared->text3, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text4, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text5, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text6, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text7, len - 7*TEXT_SZ, 1, fp_rtsp); 
		#endif
	}
	else if(8 == num)
	{
		memcpy(shared->text0, temp, TEXT_SZ);
		memcpy(shared->text1, temp + TEXT_SZ, TEXT_SZ);
		memcpy(shared->text2, temp + 2*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text3, temp + 3*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text4, temp + 4*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text5, temp + 5*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text6, temp + 6*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text7, temp + 7*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text8, temp + 8*TEXT_SZ, len - 8*TEXT_SZ);

		#ifdef HISI3519_RTSP_FILE
		fwrite(shared->text0, TEXT_SZ, 1, fp_rtsp); 
		fwrite(shared->text1, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text2, TEXT_SZ, 1, fp_rtsp);  
		fwrite(shared->text3, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text4, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text5, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text6, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text7, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text8, len - 8*TEXT_SZ, 1, fp_rtsp); 
		#endif
	}
	else if(9 == num)
	{
		memcpy(shared->text0, temp, TEXT_SZ);
		memcpy(shared->text1, temp + TEXT_SZ, TEXT_SZ);
		memcpy(shared->text2, temp + 2*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text3, temp + 3*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text4, temp + 4*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text5, temp + 5*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text6, temp + 6*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text7, temp + 7*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text8, temp + 8*TEXT_SZ, TEXT_SZ);
		memcpy(shared->text9, temp + 9*TEXT_SZ, len - 9*TEXT_SZ);

		#ifdef HISI3519_RTSP_FILE
		fwrite(shared->text0, TEXT_SZ, 1, fp_rtsp); 
		fwrite(shared->text1, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text2, TEXT_SZ, 1, fp_rtsp);  
		fwrite(shared->text3, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text4, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text5, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text6, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text7, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text8, TEXT_SZ, 1, fp_rtsp);
		fwrite(shared->text9, len - 9*TEXT_SZ, 1, fp_rtsp); 
		#endif
	}

    shared->written = 1; 
	shared->fFrameSize = len;
	free(temp);
	temp = NULL;
	//usleep(30 * 1000); 

	#endif
	
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : save H265 stream
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_SaveH265(FILE* fpH265File, VENC_STREAM_S* pstStream)
{
    HI_S32 i;

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        //fwrite(pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,
        //       pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, fpH265File);

        fflush(fpH265File);
    }

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : save jpeg stream
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_SaveJPEG(FILE* fpJpegFile, VENC_STREAM_S* pstStream)
{
    VENC_PACK_S*  pstData;
    HI_U32 i;

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        pstData = &pstStream->pstPack[i];
        fwrite(pstData->pu8Addr + pstData->u32Offset, pstData->u32Len - pstData->u32Offset, 1, fpJpegFile);
        fflush(fpJpegFile);
    }

    return HI_SUCCESS;
}


HI_S32 SAMPLE_COMM_VENC_Getdcfinfo(char* SrcJpgPath, char* DstThmPath)
{
    HI_S32 s32RtnVal = HI_SUCCESS;
    HI_CHAR JPGSrcPath[RH_FILE_NAME_LEN] = {0};
    HI_CHAR JPGDesPath[RH_FILE_NAME_LEN] = {0};
    HI_U32 DstSize = 0;
    snprintf(JPGSrcPath, sizeof(JPGSrcPath), "%s", SrcJpgPath);
    snprintf(JPGDesPath, sizeof(JPGDesPath), "%s", DstThmPath);

    s32RtnVal = FileTrans_GetThmFromJpg(JPGSrcPath, &DstSize);
    if ((HI_SUCCESS != s32RtnVal) || (0 == DstSize))
    {
        printf("fail to get thm\n");
        return HI_FAILURE;
    }

    FILE* fpTHM = fopen(JPGDesPath, "w");
    if (HI_NULL == fpTHM)
    {
        printf("file to create file %s\n", JPGDesPath);
        return HI_FAILURE;
    }

    HI_U32 u32WritenSize = 0;
    while (u32WritenSize < DstSize)
    {
        s32RtnVal = fwrite(DstBuf + u32WritenSize, 1, DstSize, fpTHM);
        if (s32RtnVal <= 0)
        {
            printf("fail to wirte file, rtn=%d\n", s32RtnVal);
            break;
        }

        u32WritenSize += s32RtnVal;
    }

    if (fpTHM)
    {
        fclose(fpTHM);
        fpTHM = 0;
    }

	if(NULL != DstBuf)
	{
		free(DstBuf);
		DstBuf = NULL;
	}
	
    return 0;
}


/******************************************************************************
* funciton : save snap stream
******************************************************************************/

HI_S32 SAMPLE_COMM_VENC_SaveSnap(VENC_STREAM_S* pstStream, HI_BOOL bSaveJpg, HI_BOOL bSaveThm)
{
	char acFile[RH_FILE_NAME_LEN]	= {0};
	char acFile_dcf[RH_FILE_NAME_LEN]	= {0};
	FILE* pFile;
	HI_S32 s32Ret;

	if(!bSaveJpg && !bSaveThm)
	{
		return HI_SUCCESS;	  
	}
	
	sprintf(acFile, "snap_%d.jpg", gs_s32SnapCnt);
	pFile = fopen(acFile, "wb");
	if (pFile == NULL)
	{
		SAMPLE_PRT("open file err\n");
		return HI_FAILURE;
	}
	s32Ret = SAMPLE_COMM_VENC_SaveJPEG(pFile, pstStream);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("save snap picture failed!\n");
		return HI_FAILURE;
	}
	fclose(pFile);

	if(bSaveThm)
	{
		sprintf(acFile_dcf, "snap_thm_%d.jpg", gs_s32SnapCnt);
		s32Ret = SAMPLE_COMM_VENC_Getdcfinfo(acFile, acFile_dcf);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("save thm picture failed!\n");
			return HI_FAILURE;
		}
	}
	
	gs_s32SnapCnt++;
	return HI_SUCCESS;
}


/******************************************************************************
* funciton : save stream
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_SaveStream(PAYLOAD_TYPE_E enType, FILE* pFd, VENC_STREAM_S* pstStream)
{
    HI_S32 s32Ret;
    if (PT_H264 == enType)
    {
        s32Ret = SAMPLE_COMM_VENC_SaveH264(pFd, pstStream);
    }
    else if (PT_MJPEG == enType)
    {
        s32Ret = SAMPLE_COMM_VENC_SaveMJpeg(pFd, pstStream);
    }
    else if (PT_H265 == enType)
    {
        s32Ret = SAMPLE_COMM_VENC_SaveH265(pFd, pstStream);
    }
    else
    {
        return HI_FAILURE;
    }
    return s32Ret;
}


/******************************************************************************
* funciton : the process of physical address retrace
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_SaveFile(FILE* pFd, VENC_STREAM_BUF_INFO_S *pstStreamBuf, VENC_STREAM_S* pstStream)
{
    HI_U32 i;
    HI_U32 u32SrcPhyAddr;
    HI_U32 u32Left;
    HI_S32 s32Ret = 0;
    
    for(i=0; i<pstStream->u32PackCount; i++)
    {
        if (pstStream->pstPack[i].u32PhyAddr + pstStream->pstPack[i].u32Len >=
                pstStreamBuf->u32PhyAddr + pstStreamBuf->u32BufSize)
            {
                if (pstStream->pstPack[i].u32PhyAddr + pstStream->pstPack[i].u32Offset >=
                    pstStreamBuf->u32PhyAddr + pstStreamBuf->u32BufSize)
                {
                    /* physical address retrace in offset segment */
                    u32SrcPhyAddr = pstStreamBuf->u32PhyAddr +
                                    ((pstStream->pstPack[i].u32PhyAddr + pstStream->pstPack[i].u32Offset) - 
                                    (pstStreamBuf->u32PhyAddr + pstStreamBuf->u32BufSize));

                    s32Ret = fwrite ((void *)u32SrcPhyAddr, pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);
                    if(s32Ret<0)
                    {
                        SAMPLE_PRT("fwrite err %d\n", s32Ret);
                        return s32Ret;
                    }
                }
                else
                {
                    /* physical address retrace in data segment */
                    u32Left = (pstStreamBuf->u32PhyAddr + pstStreamBuf->u32BufSize) - pstStream->pstPack[i].u32PhyAddr;

                    s32Ret = fwrite((void *)(pstStream->pstPack[i].u32PhyAddr + pstStream->pstPack[i].u32Offset),
                                 u32Left - pstStream->pstPack[i].u32Offset, 1, pFd);
                    if(s32Ret<0)
                    {
                        SAMPLE_PRT("fwrite err %d\n", s32Ret);
                        return s32Ret;
                    }
                    
                    s32Ret = fwrite((void *)pstStreamBuf->u32PhyAddr, pstStream->pstPack[i].u32Len - u32Left, 1, pFd);
                    if(s32Ret<0)
                    {
                        SAMPLE_PRT("fwrite err %d\n", s32Ret);
                        return s32Ret;
                    }
                }
            }
            else
            {
                /* physical address retrace does not happen */
                s32Ret = fwrite ((void *)(pstStream->pstPack[i].u32PhyAddr + pstStream->pstPack[i].u32Offset),
                              pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);
                if(s32Ret<0)
                {
                    SAMPLE_PRT("fwrite err %d\n", s32Ret);
                    return s32Ret;
                }
            }
			fflush(pFd);
    }
    return HI_SUCCESS;
}





/******************************************************************************
* funciton : Start venc stream mode (h264, mjpeg)
* note      : rate control parameter need adjust, according your case.
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_Start(VENC_CHN VencChn, PAYLOAD_TYPE_E enType, VIDEO_NORM_E enNorm, PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode, HI_U32  u32Profile, VENC_GOP_ATTR_S *pstGopAttr)
{
    HI_S32 s32Ret;
    VENC_CHN_ATTR_S stVencChnAttr;
    VENC_ATTR_H264_S stH264Attr;
    VENC_ATTR_H264_CBR_S    stH264Cbr;
    VENC_ATTR_H264_VBR_S    stH264Vbr;
    VENC_ATTR_H264_AVBR_S    stH264AVbr;
    VENC_ATTR_H264_FIXQP_S  stH264FixQp;
    VENC_ATTR_H265_S        stH265Attr;
    VENC_ATTR_H265_CBR_S    stH265Cbr;
    VENC_ATTR_H265_VBR_S    stH265Vbr;	
    VENC_ATTR_H265_AVBR_S    stH265AVbr;
    VENC_ATTR_H265_FIXQP_S  stH265FixQp;
    VENC_ATTR_MJPEG_S stMjpegAttr;
    VENC_ATTR_MJPEG_FIXQP_S stMjpegeFixQp;
    VENC_ATTR_JPEG_S stJpegAttr;
    SIZE_S stPicSize;

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enNorm, enSize, &stPicSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get picture size failed!\n");
        return HI_FAILURE;
    }

    /******************************************
     step 1:  Create Venc Channel
    ******************************************/
    stVencChnAttr.stVeAttr.enType = enType;
    switch (enType)
    {
        case PT_H264:
        {
            stH264Attr.u32MaxPicWidth = stPicSize.u32Width;
            stH264Attr.u32MaxPicHeight = stPicSize.u32Height;
            stH264Attr.u32PicWidth = stPicSize.u32Width;/*the picture width*/
            stH264Attr.u32PicHeight = stPicSize.u32Height;/*the picture height*/
            stH264Attr.u32BufSize  = stPicSize.u32Width * stPicSize.u32Height * 2;/*stream buffer size*/
            stH264Attr.u32Profile  = u32Profile;/*0: baseline; 1:MP; 2:HP;  3:svc_t */
            stH264Attr.bByFrame = HI_TRUE;/*get stream mode is slice mode or frame mode?*/
            //stH264Attr.u32BFrameNum = 0;/* 0: not support B frame; >=1: number of B frames */
            //stH264Attr.u32RefNum = 1;/* 0: default; number of refrence frame*/
            memcpy(&stVencChnAttr.stVeAttr.stAttrH264e, &stH264Attr, sizeof(VENC_ATTR_H264_S));
            if (SAMPLE_RC_CBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
                stH264Cbr.u32Gop            = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264Cbr.u32StatTime       = 1; /* stream rate statics time(s) */
                stH264Cbr.u32SrcFrmRate      = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30; /* input (vi) frame rate */
                stH264Cbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30; /* target frame rate */
                switch (enSize)
                {
                    case PIC_QCIF:
                        stH264Cbr.u32BitRate = 256; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stH264Cbr.u32BitRate = 512;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
					case PIC_360P:		/*640 * 360 */	
                        stH264Cbr.u32BitRate = 1024 * 2;
                        break;
                    case PIC_HD720:   /* 1280 * 720 */
                        stH264Cbr.u32BitRate = 1024 * 2;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stH264Cbr.u32BitRate = 1024 * 6.5;
                        break;
                    case PIC_5M:  /* 2592 * 1944 */
                        stH264Cbr.u32BitRate = 1024 * 6;
                        break;
					case PIC_UHD4K:
                        stH264Cbr.u32BitRate = 1024 * 30;
						break;
                    default :
                        stH264Cbr.u32BitRate = 1024 * 4;
                        break;
                }

                stH264Cbr.u32FluctuateLevel = 1; /* average bit rate */
                memcpy(&stVencChnAttr.stRcAttr.stAttrH264Cbr, &stH264Cbr, sizeof(VENC_ATTR_H264_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
                stH264FixQp.u32Gop = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25: 30;
                stH264FixQp.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264FixQp.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264FixQp.u32IQp = 20;
                stH264FixQp.u32PQp = 23;
				stH264FixQp.u32BQp = 23;
                memcpy(&stVencChnAttr.stRcAttr.stAttrH264FixQp, &stH264FixQp, sizeof(VENC_ATTR_H264_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
                stH264Vbr.u32Gop = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264Vbr.u32StatTime = 2;
                stH264Vbr.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264Vbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264Vbr.u32MinQp = 10;
				stH264Vbr.u32MinIQp = 15;
                stH264Vbr.u32MaxQp =  20;
                switch (enSize)
                {
                    case PIC_QCIF:
                        stH264Vbr.u32MaxBitRate = 256 * 3; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stH264Vbr.u32MaxBitRate = 512 * 3;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
					case PIC_360P:		/*640 * 360 */
                        stH264Vbr.u32MaxBitRate = 1024 * 2;
                        break;
					
                    case PIC_HD720:   /* 1280 * 720 */
                        stH264Vbr.u32MaxBitRate = 1024 * 3;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stH264Vbr.u32MaxBitRate = 1024 * 2;
                        break;
                    case PIC_5M:  /* 2592 * 1944 */
                        stH264Vbr.u32MaxBitRate = 1024 * 6;
                        break;
					case PIC_UHD4K:
                        stH264Vbr.u32MaxBitRate    = 1024 * 8;	
						break;
                    default :
                        stH264Vbr.u32MaxBitRate = 1024 * 4;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stAttrH264Vbr, &stH264Vbr, sizeof(VENC_ATTR_H264_VBR_S));
            }
			else if(SAMPLE_RC_AVBR == enRcMode)
			{
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;
				
                stH264AVbr.u32Gop = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264AVbr.u32StatTime = 1;
                stH264AVbr.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264AVbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
 				
                switch (enSize)
                {
                    case PIC_QCIF:
                        stH264AVbr.u32MaxBitRate = 256 * 3; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stH264AVbr.u32MaxBitRate = 512 * 3;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
					case PIC_360P:		/*640 * 360 */	
                        stH264AVbr.u32MaxBitRate = 1024 * 2;
                        break;
                    case PIC_HD720:   /* 1280 * 720 */
                        stH264AVbr.u32MaxBitRate = 1024 * 3;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stH264AVbr.u32MaxBitRate = 1024 * 4;
                        break;
                    case PIC_5M:  /* 2592 * 1944 */
                        stH264AVbr.u32MaxBitRate = 1024 * 6;
                        break;
					case PIC_UHD4K:
                        stH264AVbr.u32MaxBitRate    = 1024 * 8;	
						break;
                    default :
                        stH264AVbr.u32MaxBitRate = 1024 * 4;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stAttrH264AVbr, &stH264AVbr, sizeof(VENC_ATTR_H264_AVBR_S));
            }
            else
            {
                return HI_FAILURE;
            }
        }
        break;
        case PT_MJPEG:
        {
            stMjpegAttr.u32MaxPicWidth = stPicSize.u32Width;
            stMjpegAttr.u32MaxPicHeight = stPicSize.u32Height;
            stMjpegAttr.u32PicWidth = stPicSize.u32Width;
            stMjpegAttr.u32PicHeight = stPicSize.u32Height;
            stMjpegAttr.u32BufSize = stPicSize.u32Width * stPicSize.u32Height * 3;
            stMjpegAttr.bByFrame = HI_TRUE;  /*get stream mode is field mode  or frame mode*/
            memcpy(&stVencChnAttr.stVeAttr.stAttrMjpege, &stMjpegAttr, sizeof(VENC_ATTR_MJPEG_S));
            if (SAMPLE_RC_FIXQP == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;
                stMjpegeFixQp.u32Qfactor        = 90;
                stMjpegeFixQp.u32SrcFrmRate      = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stMjpegeFixQp.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                memcpy(&stVencChnAttr.stRcAttr.stAttrMjpegeFixQp, &stMjpegeFixQp,
                       sizeof(VENC_ATTR_MJPEG_FIXQP_S));
            }
            else if (SAMPLE_RC_CBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
                stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32StatTime       = 1;
                stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32SrcFrmRate      = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stVencChnAttr.stRcAttr.stAttrMjpegeCbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32FluctuateLevel = 1;
                switch (enSize)
                {
                    case PIC_QCIF:
                        stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32BitRate = 384 * 3; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32BitRate = 768 * 3;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
					case PIC_360P:		/*640 * 360 */	
                        stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32BitRate = 1024 * 3 * 3;
                        break;
                    case PIC_HD720:   /* 1280 * 720 */
                        stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32BitRate = 1024 * 5 * 3;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32BitRate = 1024 * 10 * 3;
                        break;
                    case PIC_5M:  /* 2592 * 1944 */
                        stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32BitRate = 1024 * 10 * 3;
                        break;
					case PIC_UHD4K:
                        stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32BitRate = 1024 * 10 * 5;	
						break;
                    default :
                        stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32BitRate = 1024 * 10 * 3;
                        break;
                }
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGVBR;
                stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32StatTime = 1;
                stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stVencChnAttr.stRcAttr.stAttrMjpegeVbr.fr32DstFrmRate = 5;
                stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MinQfactor = 50;
                stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MaxQfactor = 95;
                switch (enSize)
                {
                    case PIC_QCIF:
                        stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 256 * 3; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 512 * 3;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
					case PIC_360P:		/*640 * 360 */	
                        stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 1024 * 2 * 3;
                        break;
                    case PIC_HD720:   /* 1280 * 720 */
                        stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 1024 * 3 * 3;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 1024 * 6 * 3;
                        break;
                    case PIC_5M:  /* 2592 * 1944 */
                        stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 1024 * 12 * 3;
                        break;
					case PIC_UHD4K:
                        stH264Cbr.u32BitRate 								 = 1024 * 12 * 3;
						break;
                    default :
                        stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 1024 * 4 * 3;
                        break;
                }
            }
            else
            {
                SAMPLE_PRT("cann't support other mode in this version!\n");
                return HI_FAILURE;
            }
        }
        break;

        case PT_JPEG:
            stJpegAttr.u32PicWidth  = stPicSize.u32Width;
            stJpegAttr.u32PicHeight = stPicSize.u32Height;
            stJpegAttr.u32MaxPicWidth  = stPicSize.u32Width;
            stJpegAttr.u32MaxPicHeight = stPicSize.u32Height;
            stJpegAttr.u32BufSize   = stPicSize.u32Width * stPicSize.u32Height * 3;
            stJpegAttr.bByFrame     = HI_TRUE;/*get stream mode is field mode  or frame mode*/
            stJpegAttr.bSupportDCF  = HI_FALSE;
            memcpy(&stVencChnAttr.stVeAttr.stAttrJpege, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
            break;

        case PT_H265:
        {
            stH265Attr.u32MaxPicWidth = stPicSize.u32Width;
            stH265Attr.u32MaxPicHeight = stPicSize.u32Height;
            stH265Attr.u32PicWidth = stPicSize.u32Width;/*the picture width*/
            stH265Attr.u32PicHeight = stPicSize.u32Height;/*the picture height*/
            stH265Attr.u32BufSize  = stPicSize.u32Width * stPicSize.u32Height * 2;/*stream buffer size*/
            if (u32Profile >= 1)
            { stH265Attr.u32Profile = 0; }/*0:MP; */
            else
            { stH265Attr.u32Profile  = u32Profile; }/*0:MP*/
            stH265Attr.bByFrame = HI_TRUE;/*get stream mode is slice mode or frame mode?*/
            //stH265Attr.u32BFrameNum = 0;/* 0: not support B frame; >=1: number of B frames */
            //stH265Attr.u32RefNum = 1;/* 0: default; number of refrence frame*/
            memcpy(&stVencChnAttr.stVeAttr.stAttrH265e, &stH265Attr, sizeof(VENC_ATTR_H265_S));

            if (SAMPLE_RC_CBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
                stH265Cbr.u32Gop            = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265Cbr.u32StatTime       = 1; /* stream rate statics time(s) */
                stH265Cbr.u32SrcFrmRate      = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30; /* input (vi) frame rate */
                stH265Cbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30; /* target frame rate */
                switch (enSize)
                {
                    case PIC_QCIF:
                        stH265Cbr.u32BitRate = 256; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stH265Cbr.u32BitRate = 512;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
					case PIC_360P:		/*640 * 360 */	
                        stH265Cbr.u32BitRate = 1024 * 2;
                        break;
                    case PIC_HD720:   /* 1280 * 720 */
                        stH265Cbr.u32BitRate = 1024 * 3;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stH265Cbr.u32BitRate = 1024 * 4;
                        break;
                    case PIC_5M:  /* 2592 * 1944 */
                        stH265Cbr.u32BitRate = 1024 * 6;
                        break;
					case PIC_UHD4K:
                        stH265Cbr.u32BitRate = 1024 * 1;
                        break;
                    default :
                        stH265Cbr.u32BitRate = 1024 * 4;
                        break;
                }
                stH265Cbr.u32FluctuateLevel = 1; /* average bit rate */
                memcpy(&stVencChnAttr.stRcAttr.stAttrH265Cbr, &stH265Cbr, sizeof(VENC_ATTR_H265_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
                stH265FixQp.u32Gop = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265FixQp.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265FixQp.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265FixQp.u32IQp = 20;
                stH265FixQp.u32PQp = 23;
                stH265FixQp.u32BQp = 25;
                memcpy(&stVencChnAttr.stRcAttr.stAttrH265FixQp, &stH265FixQp, sizeof(VENC_ATTR_H265_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
                stH265Vbr.u32Gop = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265Vbr.u32StatTime = 1;
                stH265Vbr.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265Vbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265Vbr.u32MinQp  = 10;
				stH265Vbr.u32MinIQp = 10;
                stH265Vbr.u32MaxQp  = 50;
                switch (enSize)
                {
                    case PIC_QCIF:
                        stH265Vbr.u32MaxBitRate = 256 * 3; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stH265Vbr.u32MaxBitRate = 512 * 3;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
					case PIC_360P:		/*640 * 360 */	
                        stH265Vbr.u32MaxBitRate = 1024 * 2;
                        break;
                    case PIC_HD720:   /* 1280 * 720 */
                        stH265Vbr.u32MaxBitRate = 1024 * 3;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stH265Vbr.u32MaxBitRate = 1024 * 6;
                        break;
                    case PIC_5M:  /* 2592 * 1944 */
                        stH265Vbr.u32MaxBitRate = 1024 * 8;
                        break;
					case PIC_UHD4K:
                        stH265Vbr.u32MaxBitRate   = 1024 * 8;	
						break;
                    default :
                        stH265Vbr.u32MaxBitRate = 1024 * 4;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stAttrH265Vbr, &stH265Vbr, sizeof(VENC_ATTR_H265_VBR_S));
            }
			else if(SAMPLE_RC_AVBR == enRcMode)
			{
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265AVBR;
				
                stH265AVbr.u32Gop = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265AVbr.u32StatTime = 3;
                stH265AVbr.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265AVbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
 				
                switch (enSize)
                {
                    case PIC_QCIF:
                        stH265AVbr.u32MaxBitRate = 256 * 3; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stH265AVbr.u32MaxBitRate = 512 * 3;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
					case PIC_360P:		/*640 * 360 */	
                        stH265AVbr.u32MaxBitRate = 1024 * 2;
                        break;
                    case PIC_HD720:   /* 1280 * 720 */
                        stH265AVbr.u32MaxBitRate = 1024 * 3;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stH265AVbr.u32MaxBitRate = 1024 * 4;
                        break;
                    case PIC_5M:  /* 2592 * 1944 */
                        stH265AVbr.u32MaxBitRate = 1024 * 6;
                        break;
					case PIC_UHD4K:
                        stH265AVbr.u32MaxBitRate    = 1024 * 8;	
						break;
                    default :
                        stH265AVbr.u32MaxBitRate = 1024 * 4;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stAttrH265AVbr, &stH265AVbr, sizeof(VENC_ATTR_H265_AVBR_S));
            }			
            else
            {
                return HI_FAILURE;
            }
        }
        break;
        default:
            return HI_ERR_VENC_NOT_SUPPORT;
    }

	if(PT_MJPEG == enType || PT_JPEG == enType )
	{
		stVencChnAttr.stGopAttr.enGopMode  = VENC_GOPMODE_NORMALP;
		stVencChnAttr.stGopAttr.stNormalP.s32IPQpDelta = 0;
	}
	else
	{
		memcpy(&stVencChnAttr.stGopAttr,pstGopAttr,sizeof(VENC_GOP_ATTR_S));		
	}

    s32Ret = HI_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", \
                   VencChn, s32Ret);
        return s32Ret;
    }


    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    s32Ret = HI_MPI_VENC_StartRecvPic(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : Stop venc ( stream mode -- H264, MJPEG )
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_Stop(VENC_CHN VencChn)
{
    HI_S32 s32Ret;
    /******************************************
     step 1:  Stop Recv Pictures
    ******************************************/
    s32Ret = HI_MPI_VENC_StopRecvPic(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_StopRecvPic vechn[%d] failed with %#x!\n", \
                   VencChn, s32Ret);
        return HI_FAILURE;
    }    /******************************************
     step 2:  Distroy Venc Channel
    ******************************************/
    s32Ret = HI_MPI_VENC_DestroyChn(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_DestroyChn vechn[%d] failed with %#x!\n", \
                   VencChn, s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : Start snap
******************************************************************************/

HI_S32 SAMPLE_COMM_VENC_SnapStart(VENC_CHN VencChn, SIZE_S* pstSize, HI_BOOL bSupportDCF)
{
    HI_S32 s32Ret;
    VENC_CHN_ATTR_S stVencChnAttr;
    VENC_ATTR_JPEG_S stJpegAttr;
    /******************************************
     step 1:  Create Venc Channel
    ******************************************/
    stVencChnAttr.stVeAttr.enType = PT_JPEG;
    stJpegAttr.u32MaxPicWidth  = pstSize->u32Width;
    stJpegAttr.u32MaxPicHeight = pstSize->u32Height;
    stJpegAttr.u32PicWidth  = pstSize->u32Width;
    stJpegAttr.u32PicHeight = pstSize->u32Height;
    stJpegAttr.u32BufSize = pstSize->u32Width * pstSize->u32Height * 2;
    stJpegAttr.bByFrame = HI_TRUE;/*get stream mode is field mode  or frame mode*/
    stJpegAttr.bSupportDCF = bSupportDCF;
    memcpy(&stVencChnAttr.stVeAttr.stAttrJpege, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
    s32Ret = HI_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_CreateChn [%d] faild with %#x!\n", \
                   VencChn, s32Ret);
        return s32Ret;
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : Stop snap
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_SnapStop(VENC_CHN VencChn)
{
    HI_S32 s32Ret;
    s32Ret = HI_MPI_VENC_StopRecvPic(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_StopRecvPic vechn[%d] failed with %#x!\n", VencChn, s32Ret);
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_VENC_DestroyChn(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_DestroyChn vechn[%d] failed with %#x!\n", VencChn, s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : snap process
******************************************************************************/

HI_S32 SAMPLE_COMM_VENC_SnapProcess(VENC_CHN VencChn, HI_BOOL bSaveJpg, HI_BOOL bSaveThm)
{
    struct timeval TimeoutVal;
    fd_set read_fds;
    HI_S32 s32VencFd;
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S stRecvParam;

    printf("press any key to snap one pic\n");
    getchar();

    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    stRecvParam.s32RecvPicNum = 1;
    s32Ret = HI_MPI_VENC_StartRecvPicEx(VencChn, &stRecvParam);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_StartRecvPic faild with%#x!\n", s32Ret);
        return HI_FAILURE;
    }    
    /******************************************
     step 3:  recv picture
    ******************************************/
    s32VencFd = HI_MPI_VENC_GetFd(VencChn);
    if (s32VencFd < 0)
    {
        SAMPLE_PRT("HI_MPI_VENC_GetFd faild with%#x!\n", s32VencFd);
        return HI_FAILURE;
    }
    FD_ZERO(&read_fds);
    FD_SET(s32VencFd, &read_fds);
    TimeoutVal.tv_sec  = 2;
    TimeoutVal.tv_usec = 0;
    s32Ret = select(s32VencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
    if (s32Ret < 0)
    {
        SAMPLE_PRT("snap select failed!\n");
        return HI_FAILURE;
    }
    else if (0 == s32Ret)
    {
        SAMPLE_PRT("snap time out!\n");
        return HI_FAILURE;
    }
    else
    {
        if (FD_ISSET(s32VencFd, &read_fds))
        {
            s32Ret = HI_MPI_VENC_Query(VencChn, &stStat);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("HI_MPI_VENC_Query failed with %#x!\n", s32Ret);
                return HI_FAILURE;
            }						
            /*******************************************************
			suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
			 if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
			 {				SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
				return HI_SUCCESS;
			 }			
			 *******************************************************/
            if (0 == stStat.u32CurPacks)
            {
                SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                return HI_SUCCESS;
            }
            stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
            if (NULL == stStream.pstPack)
            {
                SAMPLE_PRT("malloc memory failed!\n");
                return HI_FAILURE;
            }
            stStream.u32PackCount = stStat.u32CurPacks;
            s32Ret = HI_MPI_VENC_GetStream(VencChn, &stStream, -1);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("HI_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
                free(stStream.pstPack);
                stStream.pstPack = NULL;
                return HI_FAILURE;
            }

            s32Ret = SAMPLE_COMM_VENC_SaveSnap(&stStream, bSaveJpg, bSaveThm);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("HI_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
                free(stStream.pstPack);
                stStream.pstPack = NULL;
                return HI_FAILURE;
            }
            s32Ret = HI_MPI_VENC_ReleaseStream(VencChn, &stStream);
            if (s32Ret)
            {
                SAMPLE_PRT("HI_MPI_VENC_ReleaseStream failed with %#x!\n", s32Ret);
                free(stStream.pstPack);
                stStream.pstPack = NULL;
                return HI_FAILURE;
            }
            free(stStream.pstPack);
            stStream.pstPack = NULL;
        }
    }    
    /******************************************
     step 4:  stop recv picture
    ******************************************/
    s32Ret = HI_MPI_VENC_StopRecvPic(VencChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VENC_StopRecvPic failed with %#x!\n",  s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

#define IP		 			"192.168.9.236"
#define PORT 				1259
#define MAXDATASIZE 		512


/*Define coordinate mode*/
typedef enum hiVISCA_E   
{
    VISCA_UP = 0,   
    VISCA_DOWN,  
    VISCA_LEFT,   
    VISCA_RIGHT,   
    VISCA_UPLEFT,   
    VISCA_UPRIGHT,  
    VISCA_DOWNLEFT,   
    VISCA_DOWNRIGHT,
	VISCA_ZOOMTELE,
	VISCA_ZOOMWIDE,
	VISCA_STOP,
    VISCA_EXIT,
    VISCA_INVALID
}VISCA_E;

static VISCA_E command = VISCA_INVALID;
static int 	new_width = 1920;
static int 	new_height = 1080;
static int 	zoom_width = 1920;
static int 	zoom_height = 1080;
static int 	start_X = 960;
static int 	start_Y = 540;
static int 	inc = 6;
static int 	speed_VV = 6;
static int 	speed_WW = 6;

static VPSS_CROP_INFO_S stVpssCropInfo;


static HI_S32 Set_grp()
{
	HI_S32 s32Ret;
	VPSS_GRP 				VpssGrp = 0;
	VPSS_CHN 				VpssChn = 1;

	s32Ret = HI_MPI_VPSS_SetGrpCrop(VpssGrp, &stVpssCropInfo);
    if (HI_SUCCESS != s32Ret)
    {
	        SAMPLE_PRT("HI_MPI_VPSS_SetGrpCrop, failed :0x%x\n", s32Ret);
			printf("s32X(%d), s32Y(%d), u32Width(%d), u32Height(%d) \n", 
										stVpssCropInfo.stCropRect.s32X,
										stVpssCropInfo.stCropRect.s32Y,
										stVpssCropInfo.stCropRect.u32Width,
										stVpssCropInfo.stCropRect.u32Height);
    }

	s32Ret = HI_MPI_VPSS_ResetGrp(VpssGrp);
    if (HI_SUCCESS != s32Ret)
    {
	        SAMPLE_PRT("HI_MPI_VPSS_ResetGrp, failed :0x%x\n", s32Ret);
    }

	//usleep(33 * 1000);
	
	return s32Ret;

}



static HI_S32 Set_grp2()
{
	HI_S32 s32Ret;
	VPSS_GRP 				VpssGrp = 0;
	VPSS_CHN 				VpssChn = 1;
	VPSS_CHN_MODE_S 		stVpssChnMode;
	VPSS_CHN_ATTR_S 		stVpssChnAttr = {0};
	SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);


    stVpssChnMode.enChnMode       = VPSS_CHN_MODE_USER;
    stVpssChnMode.bDouble         = HI_FALSE;
    stVpssChnMode.enPixelFormat   = SAMPLE_PIXEL_FORMAT;
    stVpssChnMode.u32Width        = new_width;
    stVpssChnMode.u32Height       = new_height;
    stVpssChnMode.enCompressMode  = COMPRESS_MODE_SEG;
    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
    stVpssChnAttr.s32SrcFrameRate = 25;
    stVpssChnAttr.s32DstFrameRate = 25;
    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
    if (HI_SUCCESS != s32Ret)
    {
	    SAMPLE_PRT("Enable vpss chn failed!\n");
    }

	Set_grp();
	return s32Ret;

}

static HI_S32 Set_grp3()
{
	HI_S32 s32Ret;
	VPSS_CROP_INFO_S 		stVpssCropInfoTemp;
	VPSS_GRP 				VpssGrp = 0;
	VPSS_CHN 				VpssChn = 1;

	//Set_grp();

	s32Ret = HI_MPI_VPSS_SetChnCrop(VpssGrp,  VpssChn, &stVpssCropInfo);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VPSS_SetChnCrop, failed :0x%x\n",s32Ret);
		return s32Ret;
    }

	s32Ret = HI_MPI_VPSS_ResetGrp(VpssGrp);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VPSS_ResetGrp, failed :0x%x\n",s32Ret);
		return s32Ret;
    }

	s32Ret = HI_MPI_VPSS_GetChnCrop(VpssGrp,  VpssChn, &stVpssCropInfoTemp);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VPSS_SetChnCrop, failed :0x%x\n",s32Ret);
		return s32Ret;
    }
    

    SAMPLE_PRT("VPSS : %d X %d \n\n", stVpssCropInfoTemp.stCropRect.u32Width, stVpssCropInfoTemp.stCropRect.u32Height);

	return s32Ret;
}



static void exit_encoder()
{
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
	VENC_CHN VencChn;
	HI_S32 s32ChnNum = 2;
	SAMPLE_VI_CONFIG_S stViConfig = {0};

    stViConfig.enViMode   = SENSOR_TYPE;
    stViConfig.enRotate   = ROTATE_NONE;
    stViConfig.enNorm     = VIDEO_ENCODING_MODE_AUTO;
    stViConfig.enViChnSet = VI_CHN_SET_NORMAL;
    stViConfig.enWDRMode  = WDR_MODE_NONE;

    SAMPLE_COMM_VENC_StopGetStream();
    
	
    VpssGrp = 0;
	switch(s32ChnNum)
	{
		case 2:
			VpssChn = 1;   
		    VencChn = 1;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
		case 1:
			VpssChn = 0;  
		    VencChn = 0;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
			break;
			
	}

    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
	

    VpssGrp = 0;
	switch(s32ChnNum)
	{
		case 2:
			VpssChn = 1;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		case 1:
			VpssChn = 0;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		break;
	
	}

  
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
    SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
    SAMPLE_COMM_VI_StopVi(&stViConfig);
    SAMPLE_COMM_SYS_Exit();
	exit(0);

}


static HI_VOID* thread_SetVISCAProc(HI_VOID* p)
{
	HI_S32 					s32Ret;

	stVpssCropInfo.bEnable = HI_TRUE;
	stVpssCropInfo.enCropCoordinate = VPSS_CROP_ABS_COOR;

	while(1)
	{		
		stVpssCropInfo.stCropRect.u32Width  = new_width;
	    stVpssCropInfo.stCropRect.u32Height = new_height;
		unsigned i = 0;

		if(command == VISCA_EXIT)
		{
			exit_encoder();
		}

		if(command == VISCA_UP)
		{
			for(;;)
			{
				usleep(40 * 1000);
				start_Y -= inc*i*speed_VV;
				
				if(start_Y <= 0)
				{
					start_Y = 0;
				}

		    	stVpssCropInfo.stCropRect.s32X = start_X;
		    	stVpssCropInfo.stCropRect.s32Y = start_Y;
				


				if(command == VISCA_STOP)
				{
					printf("s32X(%d), s32Y(%d), u32Width(%d), u32Height(%d) \n", 
										stVpssCropInfo.stCropRect.s32X,
										stVpssCropInfo.stCropRect.s32Y,
										stVpssCropInfo.stCropRect.u32Width,
										stVpssCropInfo.stCropRect.u32Height);

					break;
				}

				Set_grp();
				i++;
			}				
		}
		else if(command == VISCA_DOWN)
		{
			for(;;)
			{
				usleep(40 * 1000);
				start_Y += inc*i*speed_VV;

				if(start_Y + new_height >= 2160)
				{
					start_Y = 2160 - new_height;
				}

		    	stVpssCropInfo.stCropRect.s32X = start_X;
		    	stVpssCropInfo.stCropRect.s32Y = start_Y;				


				if(command == VISCA_STOP)
				{
					printf("s32X(%d), s32Y(%d), u32Width(%d), u32Height(%d) \n", 
										stVpssCropInfo.stCropRect.s32X,
										stVpssCropInfo.stCropRect.s32Y,
										stVpssCropInfo.stCropRect.u32Width,
										stVpssCropInfo.stCropRect.u32Height);

					break;
				}

				Set_grp();
				i++;
			}
		}
		else if(command == VISCA_LEFT)
		{
			for(;;)
			{
				usleep(40 * 1000);
				start_X -= inc*i*speed_WW;

				if(start_X <= 0)
				{
					start_X = 0;
				}

		    	stVpssCropInfo.stCropRect.s32X = start_X;
		    	stVpssCropInfo.stCropRect.s32Y = start_Y;


				if(command == VISCA_STOP)
				{
					printf("s32X(%d), s32Y(%d), u32Width(%d), u32Height(%d) \n", 
										stVpssCropInfo.stCropRect.s32X,
										stVpssCropInfo.stCropRect.s32Y,
										stVpssCropInfo.stCropRect.u32Width,
										stVpssCropInfo.stCropRect.u32Height);

					break;
				}

				Set_grp();
				i++;
			}
		}
		else if(command == VISCA_RIGHT)
		{
			for(;;)
			{
				usleep(40 * 1000);
				start_X += inc*i*speed_WW;

				if(start_X + new_width >= 3840)
				{
					start_X = 3840 - new_width;
				}

		    	stVpssCropInfo.stCropRect.s32X = start_X;
		    	stVpssCropInfo.stCropRect.s32Y = start_Y;


				if(command == VISCA_STOP)
				{
					printf("s32X(%d), s32Y(%d), u32Width(%d), u32Height(%d) \n", 
										stVpssCropInfo.stCropRect.s32X,
										stVpssCropInfo.stCropRect.s32Y,
										stVpssCropInfo.stCropRect.u32Width,
										stVpssCropInfo.stCropRect.u32Height);

					break;
				}

				Set_grp();
				i++;
			}
		}
		else if(command == VISCA_UPLEFT)
		{
			for(;;)
			{
				usleep(40 * 1000);
				start_X -= inc*i*speed_WW;
				start_Y -= inc*i*speed_VV;

				if(start_X <= 0)
				{
					start_X = 0;
				}

				if(start_Y <= 0)
				{
					start_Y = 0;
				}

		    	stVpssCropInfo.stCropRect.s32X = start_X;
		    	stVpssCropInfo.stCropRect.s32Y = start_Y;

				if(command == VISCA_STOP)
				{
					printf("s32X(%d), s32Y(%d), u32Width(%d), u32Height(%d) \n", 
										stVpssCropInfo.stCropRect.s32X,
										stVpssCropInfo.stCropRect.s32Y,
										stVpssCropInfo.stCropRect.u32Width,
										stVpssCropInfo.stCropRect.u32Height);

					break;
				}

				Set_grp();
				i++;
			}
		}
		else if(command == VISCA_UPRIGHT)
		{
			for(;;)
			{
				usleep(40 * 1000);
				start_X += inc*i*speed_WW;
				start_Y -= inc*i*speed_VV;	

				if(start_X + new_width >= 3840)
				{
					start_X = 3840 - new_width;
				}	

				if(start_Y <= 0)
				{
					start_Y = 0;
				}	

		    	stVpssCropInfo.stCropRect.s32X = start_X;
		    	stVpssCropInfo.stCropRect.s32Y = start_Y;

				if(command == VISCA_STOP)
				{
					printf("s32X(%d), s32Y(%d), u32Width(%d), u32Height(%d) \n", 
										stVpssCropInfo.stCropRect.s32X,
										stVpssCropInfo.stCropRect.s32Y,
										stVpssCropInfo.stCropRect.u32Width,
										stVpssCropInfo.stCropRect.u32Height);

					break;
				}

				Set_grp();
				i++;
			}	
		}
		else if(command == VISCA_DOWNLEFT)
		{
			for(;;)
			{
				usleep(40 * 1000);
				start_X -= inc*i*speed_WW;
				start_Y += inc*i*speed_VV;

				if(start_X <= 0)
				{
					start_X = 0;
				}

				if(start_Y + new_height >= 2160)
				{
					start_Y = 2160 - new_height;
				}

		    	stVpssCropInfo.stCropRect.s32X = start_X;
				stVpssCropInfo.stCropRect.s32Y = start_Y;

				if(command == VISCA_STOP)
				{
					printf("s32X(%d), s32Y(%d), u32Width(%d), u32Height(%d) \n", 
										stVpssCropInfo.stCropRect.s32X,
										stVpssCropInfo.stCropRect.s32Y,
										stVpssCropInfo.stCropRect.u32Width,
										stVpssCropInfo.stCropRect.u32Height);

					break;
				}

				Set_grp();
				i++;
			}
		}
		else if(command == VISCA_DOWNRIGHT)
		{
			for(;;)
			{
				usleep(40 * 1000);
				start_X += inc*i*speed_WW;
				start_Y += inc*i*speed_VV;	

				if(start_X + new_width >= 3840)
				{
					start_X = 3840 - new_width;
				}

				if(start_Y + new_height >= 2160)
				{
					start_Y = 2160 - new_height;
				}

		    	stVpssCropInfo.stCropRect.s32X = start_X;
		    	stVpssCropInfo.stCropRect.s32Y = start_Y;

				if(command == VISCA_STOP)
				{
					printf("s32X(%d), s32Y(%d), u32Width(%d), u32Height(%d) \n", 
										stVpssCropInfo.stCropRect.s32X,
										stVpssCropInfo.stCropRect.s32Y,
										stVpssCropInfo.stCropRect.u32Width,
										stVpssCropInfo.stCropRect.u32Height);

					break;
				}

				Set_grp();
				i++;
			}			
		}

		if(command == VISCA_ZOOMTELE)                 //溫湮
		{
			//for(;;)
			{
				usleep(300 * 1000);
				if(new_width == 3840 && new_height == 2160)
				{
					new_width = 1920;
					new_height = 1080;

					start_X = 960;
                    start_Y = 540;
              
					#if 0
					start_X += inc*i*speed_WW;
					start_Y += inc*i*speed_VV;	

					new_width -= inc*i*speed_WW;
					new_height -= inc*i*speed_VV;

					if(new_width < 320)
					{
						new_width = 320;
					}

					if(new_height < 240)
					{
						new_height = 240;
					}

					zoom_width = new_width;
					zoom_height = new_height;
					#endif
				}
				#if 0
				else if(new_width == 1920 && new_height == 1080)
				{
					new_width = 1280;
					new_height = 720;

					if(start_X + new_width >= 1920)
					{
						start_X = 1920 - new_width;
					}

					if(start_Y + new_height >= 1080)
					{
						start_Y = 1080 - new_height;
					}
				}
				#endif
 				

				

				stVpssCropInfo.stCropRect.s32X = start_X;
		   	 	stVpssCropInfo.stCropRect.s32Y = start_Y;
				stVpssCropInfo.stCropRect.u32Width  = new_width;
		    	stVpssCropInfo.stCropRect.u32Height = new_height;

				printf("s32X(%d), s32Y(%d), u32Width(%d), u32Height(%d) \n", 
									stVpssCropInfo.stCropRect.s32X,
									stVpssCropInfo.stCropRect.s32Y,
									stVpssCropInfo.stCropRect.u32Width,
									stVpssCropInfo.stCropRect.u32Height);


				Set_grp2();
				//i++;
			}
		}
		else if(command == VISCA_ZOOMWIDE) 			//坫苤
		{
			//for(;;)
			{
				usleep(300 * 1000);
				if(new_width == 1920 && new_height == 1080)
				{
					new_width = 3840;
                    new_height = 2160;
					start_X = 0;
                    start_Y = 0;
				}

				#if 0
				if(new_width == 1280 && new_height == 720)
				{
					new_width = 1920;
					new_height = 1080;
					
					if(start_X + new_width >= 3840)
					{
						start_X = 3840 - new_width;
					}
					
					if(start_Y + new_height >= 2160)
					{
						start_Y = 2160 - new_height;
					}
				}
				
				start_X -= inc*i*speed_WW;
				start_Y -= inc*i*speed_VV;	
				new_width += inc*i*speed_WW;
				new_height += inc*i*speed_VV;
				zoom_width = new_width;
				zoom_height = new_height;
				#endif
		
				stVpssCropInfo.stCropRect.s32X = start_X;
		   	 	stVpssCropInfo.stCropRect.s32Y = start_Y;
				stVpssCropInfo.stCropRect.u32Width  = new_width;
		    	stVpssCropInfo.stCropRect.u32Height = new_height;

				printf("s32X(%d), s32Y(%d), u32Width(%d), u32Height(%d) \n", 
									stVpssCropInfo.stCropRect.s32X,
									stVpssCropInfo.stCropRect.s32Y,
									stVpssCropInfo.stCropRect.u32Width,
									stVpssCropInfo.stCropRect.u32Height);


				Set_grp2();
				//i++;
			}
		}

	
		usleep(40 * 1000);		
	}

	return 0;
}

static HI_VOID* thread_GetVISCAProc(HI_VOID* p)
{
    printf("Welcome! This is a UDP server, I can only received message from client and reply with same message\n");
    
	HI_S32 					s32Ret;
	int 					num = 0;
    struct sockaddr_in addr;
	unsigned char buff[512] = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int sock;
    if ( (sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(1);
    }
	

    struct sockaddr_in clientAddr;
    int n;
    int len = sizeof(clientAddr);

	
    while (1)
    {
        n = recvfrom(sock, buff, 512, 0, (struct sockaddr*)&clientAddr, &len);
        if (n > 0)
        {
            buff[n] = 0;
            //printf("%s %u says: %x , length = %d \n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buff, n);

			int i = 0;
	
			#if 0
			for(i = 0; i < n; i++)
			{
				printf("%02x ", buff[i]);
			}
			#endif

		

			if(n == 9)
			{
			    if(buff[0] == 0x81 && buff[1] == 0x01 && buff[2] == 0x06 && buff[3] == 0x01 && buff[4] == 0xff && buff[5] == 0xff && buff[6] == 0x03  && buff[7] == 0x02 && buff[8] == 0xff)
				{
					printf("exit \n");
					command = VISCA_EXIT;				
				}
				else if(buff[0] == 0x81 && buff[1] == 0x01 && buff[2] == 0x06 && buff[3] == 0x01  && buff[6] == 0x03  && buff[7] == 0x01 && buff[8] == 0xff)
				{
					printf("Up \n");
					speed_WW = buff[4];
					speed_VV = buff[5];
					command = VISCA_UP;				
				}
				else if(buff[0] == 0x81 && buff[1] == 0x01 && buff[2] == 0x06 && buff[3] == 0x01  && buff[6] == 0x03  && buff[7] == 0x02 && buff[8] == 0xff)
				{
					printf("Down \n");	
					speed_WW = buff[4];
					speed_VV = buff[5];
					command = VISCA_DOWN;			
				}
				else if(buff[0] == 0x81 && buff[1] == 0x01 && buff[2] == 0x06 && buff[3] == 0x01  && buff[6] == 0x01  && buff[7] == 0x03 && buff[8] == 0xff)
				{
					printf("Left \n");
					speed_WW = buff[4];
					speed_VV = buff[5];

					command = VISCA_LEFT;				
				}
				else if(buff[0] == 0x81 && buff[1] == 0x01 && buff[2] == 0x06 && buff[3] == 0x01  && buff[6] == 0x02  && buff[7] == 0x03 && buff[8] == 0xff)
				{
					printf("right \n");
					command = VISCA_RIGHT;				
				}
				else if(buff[0] == 0x81 && buff[1] == 0x01 && buff[2] == 0x06 && buff[3] == 0x01  && buff[6] == 0x01  && buff[7] == 0x01 && buff[8] == 0xff)
				{
					printf("Up Left \n");
					speed_WW = buff[4];
					speed_VV = buff[5];

					command = VISCA_UPLEFT;				
				}
				else if(buff[0] == 0x81 && buff[1] == 0x01 && buff[2] == 0x06 && buff[3] == 0x01  && buff[6] == 0x02  && buff[7] == 0x01 && buff[8] == 0xff)
				{
					printf("Up Right \n");
					command = VISCA_UPRIGHT;				
				}
				else if(buff[0] == 0x81 && buff[1] == 0x01 && buff[2] == 0x06 && buff[3] == 0x01  && buff[6] == 0x01  && buff[7] == 0x02 && buff[8] == 0xff)
				{
					printf("Down Left \n");
					speed_WW = buff[4];
					speed_VV = buff[5];

					command = VISCA_DOWNLEFT;				
				}
				else if(buff[0] == 0x81 && buff[1] == 0x01 && buff[2] == 0x06 && buff[3] == 0x01  && buff[6] == 0x02  && buff[7] == 0x02 && buff[8] == 0xff)
				{
					printf("Down Right \n");
					speed_WW = buff[4];
					speed_VV = buff[5];

					command = VISCA_DOWNRIGHT;				
				}
				else if(buff[0] == 0x81 && buff[1] == 0x01 && buff[2] == 0x06 && buff[3] == 0x01  && buff[6] == 0x03  && buff[7] == 0x03 && buff[8] == 0xff)
				{
					printf("stop 111 \n");
					command = VISCA_STOP;				
				}
				else
				{
					command = VISCA_INVALID;
					continue;
				}

				zoom_width = 1920;
				zoom_height = 1080;


				if(speed_WW < 2)
				{
					speed_WW = 2;
				}

				if(speed_VV < 2)
				{
					speed_VV = 2;
				}
			}
			else if(n == 6)
			{
				if(buff[0] == 0x81 && buff[1] == 0x01 && buff[2] == 0x04 && buff[3] == 0x07  && buff[4] == 0x02  && buff[5] == 0xff)
				{
					printf("Zoom Tele \n");
					command = VISCA_ZOOMTELE;				
				}
				else if(buff[0] == 0x81 && buff[1] == 0x01 && buff[2] == 0x04 && buff[3] == 0x07  && buff[4] == 0x03  && buff[5] == 0xff)
				{
					printf("Zoom Wide \n");
					command = VISCA_ZOOMWIDE;				
				}
				else if(buff[0] == 0x81 && buff[1] == 0x01 && buff[2] == 0x04 && buff[3] == 0x07  && buff[4] == 0x00  && buff[5] == 0xff)
				{
					printf("stop 222 \n");
					command = VISCA_STOP;				
				}
				else
				{
					command = VISCA_INVALID;
					continue;
				}
			}	
		    
			//printf("s32X(%d), s32Y(%d) \n", stVpssCropInfo.stCropRect.s32X, stVpssCropInfo.stCropRect.s32Y);
			//printf("u32Width(%d), u32Height(%d) \n\n", stVpssCropInfo.stCropRect.u32Width, stVpssCropInfo.stCropRect.u32Height);


			//printf("start_X(%d), start_Y(%d) \n", start_X, start_Y);
			//printf("new_width(%d), new_height(%d) \n\n", new_width, new_height);


			#if 0
            n = sendto(sock, buff, n, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
            if (n < 0)
            {
                perror("sendto");
                break;
            }
			#endif
        }
        else
        {
            perror("recv");
            break;
        }
    }


	close(sock);
	exit_encoder();

    return 0;
}

/******************************************************************************
* funciton : get stream from each channels and save them
******************************************************************************/
HI_VOID* SAMPLE_COMM_VENC_GetVencStreamProc(HI_VOID* p)
{
    HI_S32 i;
    HI_S32 s32ChnTotal;
    VENC_CHN_ATTR_S stVencChnAttr;
    SAMPLE_VENC_GETSTREAM_PARA_S* pstPara;
    HI_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    HI_S32 VencFd[VENC_MAX_CHN_NUM];
    HI_CHAR aszFileName[VENC_MAX_CHN_NUM][64];
    FILE* pFile[VENC_MAX_CHN_NUM];
    char szFilePostfix[10];
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret;
    VENC_CHN VencChn;
    PAYLOAD_TYPE_E enPayLoadType[VENC_MAX_CHN_NUM];
    VENC_STREAM_BUF_INFO_S stStreamBufInfo[VENC_MAX_CHN_NUM];
	VPSS_GRP VpssGrp = 0;
	int inc = 10;
	int num = 0;
	int 					new_width = 1920;
    int 					new_height = 1080;
	int 					start_X = 64;
    int 					start_Y = 64;

    pstPara = (SAMPLE_VENC_GETSTREAM_PARA_S*)p;
    s32ChnTotal = pstPara->s32Cnt;
	printf("s32ChnTotal = %d \n", s32ChnTotal);
    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (s32ChnTotal >= VENC_MAX_CHN_NUM)
    {
        SAMPLE_PRT("input count invaild\n");
        return NULL;
    }
    for (i = 0; i < s32ChnTotal; i++)
    {
        /* decide the stream file name, and open file to save stream */
        VencChn = i;
        s32Ret = HI_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetChnAttr chn[%d] failed with %#x!\n", \
                       VencChn, s32Ret);
            return NULL;
        }
        enPayLoadType[i] = stVencChnAttr.stVeAttr.enType;
		printf("VencChn(%d),    u32BufSize(%d) \n\n", VencChn, stVencChnAttr.stVeAttr.stAttrH264e.u32BufSize);
        s32Ret = SAMPLE_COMM_VENC_GetFilePostfix(enPayLoadType[i], szFilePostfix);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VENC_GetFilePostfix [%d] failed with %#x!\n", \
                       stVencChnAttr.stVeAttr.enType, s32Ret);
            return NULL;
        }
        
        snprintf(aszFileName[i],32, "stream_chn%d%s", i, szFilePostfix);
        
        pFile[i] = fopen(aszFileName[i], "wb");
        
        if (!pFile[i])
        {
            SAMPLE_PRT("open file[%s] failed!\n",
                       aszFileName[i]);
            return NULL;
        }        /* Set Venc Fd. */
        VencFd[i] = HI_MPI_VENC_GetFd(i);
        if (VencFd[i] < 0)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetFd failed with %#x!\n",
                       VencFd[i]);
            return NULL;
        }
        if (maxfd <= VencFd[i])
        {
            maxfd = VencFd[i];
        }

        s32Ret = HI_MPI_VENC_GetStreamBufInfo (i, &stStreamBufInfo[i]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetStreamBufInfo failed with %#x!\n", s32Ret);
            return (void *)HI_FAILURE;
        }
    }


	#ifdef  HISI3519_RTSP
  
	#ifdef HISI3519_RTSP_FILE 
	fp_rtsp = fopen("send_memory.h264", "wb"); 
	#endif

    shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666|IPC_CREAT);  
    if(shmid == -1)  
    {  
        fprintf(stderr, "shmget failed\n");  
        exit(EXIT_FAILURE);  
    }  
    shm = shmat(shmid, (void*)0, 0);  
    if(shm == (void*)-1)  
    {  
        fprintf(stderr, "shmat failed\n");  
        exit(EXIT_FAILURE);  
    }  
    printf("Memory attached at %X\n", (int)shm);  
    shared = (struct shared_use_st*)shm;  
  	//shared->IsStart = 0; 
  	shared->count = 0; 

	//shared->text = (unsigned char* )malloc(TEXT_SZ * sizeof(unsigned char));

    //pthread_create(&gs_VISVAPid, 0, thread_GetVISCAProc, NULL);
	//pthread_create(&gs_VISVAPid2, 0, thread_SetVISCAProc, NULL);

	#endif
    /******************************************
     step 2:  Start to get streams of each channel.
    ******************************************/
    while (HI_TRUE == pstPara->bThreadStart)
    {
        FD_ZERO(&read_fds);
        for (i = 0; i < s32ChnTotal; i++)
        {
            FD_SET(VencFd[i], &read_fds);
        }

        TimeoutVal.tv_sec  = 0;
        TimeoutVal.tv_usec = 10 * 1000;
        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            //SAMPLE_PRT("get venc stream time out, exit thread\n");
            continue;
        }
        else
        {
            for (i = 0; i < s32ChnTotal; i++)
            {
                if (FD_ISSET(VencFd[i], &read_fds))
                {
                    /*******************************************************
                     step 2.1 : query how many packs in one-frame stream.
                    *******************************************************/
                    memset(&stStream, 0, sizeof(stStream));
                    s32Ret = HI_MPI_VENC_Query(i, &stStat);
                    if (HI_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("HI_MPI_VENC_Query chn[%d] failed with %#x!\n", i, s32Ret);
                        break;
                    }
					
					/*******************************************************
					step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
					 if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
					 {
						SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
						continue;
					 }
					*******************************************************/
					if(0 == stStat.u32CurPacks)
					{
						  SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
						  continue;
					}
                    /*******************************************************
                     step 2.3 : malloc corresponding number of pack nodes.
                    *******************************************************/
                    stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                    if (NULL == stStream.pstPack)
                    {
                        SAMPLE_PRT("malloc stream pack failed!\n");
                        break;
                    }

                    /*******************************************************
                     step 2.4 : call mpi to get one-frame stream
                    *******************************************************/

					#if 0
					if((zoom_width != 1920 || zoom_height != 1080) && (i == 1))
					{
			           VENC_CHN_ATTR_S stAttr;
					   VENC_RECV_PIC_PARAM_S stRecvParam;
					   s32Ret = HI_MPI_VENC_GetChnAttr(i, &stAttr);
			           if (HI_SUCCESS != s32Ret)
			           {
			             	SAMPLE_PRT("HI_MPI_VENC_GetChnAttr faild with%#x! \n", s32Ret);
			           		break;
			           }   

					   stAttr.stVeAttr.stAttrH264e.u32PicWidth = zoom_width;
					   stAttr.stVeAttr.stAttrH264e.u32PicHeight = zoom_height;

					   s32Ret = HI_MPI_VENC_StopRecvPic(i);
					   if (HI_SUCCESS != s32Ret)
					   {
					        SAMPLE_PRT("HI_MPI_VENC_StopRecvPic vechn[%d] failed with %#x!\n", \
					                   i, s32Ret);
					        break;
					   }   

					   s32Ret = HI_MPI_VENC_SetChnAttr(i, &stAttr);
			           if (HI_SUCCESS != s32Ret)
			           {
			             SAMPLE_PRT("HI_MPI_VENC_SetChnAttr faild with%#x! \n", s32Ret);
			             break;
			           }   

					    s32Ret = HI_MPI_VENC_StartRecvPic(i);
					    if (HI_SUCCESS != s32Ret)
					    {
					        SAMPLE_PRT("HI_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
					        break;
					    }

			           s32Ret = HI_MPI_VENC_StartRecvPicEx(i, &stRecvParam);        
			           if (HI_SUCCESS != s32Ret)
			           {
			             SAMPLE_PRT("HI_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
			             break;
			           }   
					}
					#endif
                    stStream.u32PackCount = stStat.u32CurPacks;
                    s32Ret = HI_MPI_VENC_GetStream(i, &stStream, HI_TRUE);
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("HI_MPI_VENC_GetStream failed with %#x!\n", \
                                   s32Ret);
                        break;
                    }

                    /*******************************************************
                     step 2.5 : save frame to file
                    *******************************************************/
                    #ifndef __HuaweiLite__
                    s32Ret = SAMPLE_COMM_VENC_SaveStream(enPayLoadType[i], pFile[i], &stStream);
                    #else

                    s32Ret =SAMPLE_COMM_VENC_SaveFile(pFile[i], &stStreamBufInfo[i], &stStream);
                    #endif
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("save stream failed!\n");
                        break;
                    }                    
                    /*******************************************************
                     step 2.6 : release stream                    
                     *******************************************************/
                    s32Ret = HI_MPI_VENC_ReleaseStream(i, &stStream);
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        break;
                    }                    
                    /*******************************************************
                     step 2.7 : free pack nodes
                    *******************************************************/
                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                }
            }
        }
    }    
    /*******************************************************
    * step 3 : close save-file
    *******************************************************/
    for (i = 0; i < s32ChnTotal; i++)
    {
        fclose(pFile[i]);
    }
    return NULL;
}

/******************************************************************************
* funciton : get svc_t stream from h264 channels and save them
******************************************************************************/
HI_VOID* SAMPLE_COMM_VENC_GetVencStreamProc_Svc_t(void* p)
{
    HI_S32 i = 0;
    HI_S32 s32Cnt = 0;
    HI_S32 s32ChnTotal;
    VENC_CHN_ATTR_S stVencChnAttr;
    SAMPLE_VENC_GETSTREAM_PARA_S* pstPara;
    HI_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    HI_S32 VencFd[VENC_MAX_CHN_NUM];
    HI_CHAR aszFileName[VENC_MAX_CHN_NUM][64];
    FILE* pFile[VENC_MAX_CHN_NUM];
    char szFilePostfix[10];
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret;
    VENC_CHN VencChn;
    PAYLOAD_TYPE_E enPayLoadType[VENC_MAX_CHN_NUM];
    VENC_STREAM_BUF_INFO_S stStreamBufInfo[VENC_MAX_CHN_NUM];

    pstPara = (SAMPLE_VENC_GETSTREAM_PARA_S*)p;
    s32ChnTotal = pstPara->s32Cnt;

    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (s32ChnTotal >= VENC_MAX_CHN_NUM)
    {
        SAMPLE_PRT("input count invaild\n");
        return NULL;
    }
    for (i = 0; i < s32ChnTotal; i++)
    {
        /* decide the stream file name, and open file to save stream */
        VencChn = i;
        s32Ret = HI_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetChnAttr chn[%d] failed with %#x!\n", \
                       VencChn, s32Ret);
            return NULL;
        }
        enPayLoadType[i] = stVencChnAttr.stVeAttr.enType;

        s32Ret = SAMPLE_COMM_VENC_GetFilePostfix(enPayLoadType[i], szFilePostfix);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VENC_GetFilePostfix [%d] failed with %#x!\n", \
                       stVencChnAttr.stVeAttr.enType, s32Ret);
            return NULL;
        }
        
        for (s32Cnt = 0; s32Cnt < 3; s32Cnt++)
        {
            snprintf(aszFileName[i + s32Cnt],32, "Tid%d%s", i + s32Cnt, szFilePostfix);
            
            pFile[i + s32Cnt] = fopen(aszFileName[i + s32Cnt], "wb");

            if (!pFile[i + s32Cnt])
            {
                SAMPLE_PRT("open file[%s] failed!\n",
                           aszFileName[i + s32Cnt]);
                return NULL;
            }
        }

        /* Set Venc Fd. */
        VencFd[i] = HI_MPI_VENC_GetFd(i);
        if (VencFd[i] < 0)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetFd failed with %#x!\n",
                       VencFd[i]);
            return NULL;
        }
        if (maxfd <= VencFd[i])
        {
            maxfd = VencFd[i];
        }
		s32Ret = HI_MPI_VENC_GetStreamBufInfo (i, &stStreamBufInfo[i]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetStreamBufInfo failed with %#x!\n", s32Ret);
            return NULL;
        }
    }

    /******************************************
     step 2:  Start to get streams of each channel.
    ******************************************/
    while (HI_TRUE == pstPara->bThreadStart)
    {
        FD_ZERO(&read_fds);
        for (i = 0; i < s32ChnTotal; i++)
        {
            FD_SET(VencFd[i], &read_fds);
        }
        TimeoutVal.tv_sec  = 2;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            SAMPLE_PRT("get venc stream time out, exit thread\n");
            continue;
        }
        else
        {
            for (i = 0; i < s32ChnTotal; i++)
            {
                if (FD_ISSET(VencFd[i], &read_fds))
                {
                    /*******************************************************
                    step 2.1 : query how many packs in one-frame stream.
                    *******************************************************/
                    memset(&stStream, 0, sizeof(stStream));
                    s32Ret = HI_MPI_VENC_Query(i, &stStat);
                    if (HI_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("HI_MPI_VENC_Query chn[%d] failed with %#x!\n", i, s32Ret);
                        break;
                    }										
                    /*******************************************************
					step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
					 if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
					 {						SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
						continue;
					 }					
					 *******************************************************/
                    if (0 == stStat.u32CurPacks)
                    {
                        SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                        continue;
                    }                    
                    /*******************************************************
                     step 2.3 : malloc corresponding number of pack nodes.
                    *******************************************************/
                    stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                    if (NULL == stStream.pstPack)
                    {
                        SAMPLE_PRT("malloc stream pack failed!\n");
                        break;
                    }
                    /*******************************************************
                     step 2.4 : call mpi to get one-frame stream
                    *******************************************************/
                    stStream.u32PackCount = stStat.u32CurPacks;
                    s32Ret = HI_MPI_VENC_GetStream(i, &stStream, HI_TRUE);
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("HI_MPI_VENC_GetStream failed with %#x!\n", \
                                   s32Ret);
                        break;
                    }                    
                    /*******************************************************
                     step 2.5 : save frame to file
                    *******************************************************/
#if 1
                    for (s32Cnt = 0; s32Cnt < 3; s32Cnt++)
                    {
                        switch (s32Cnt)
                        {
                            case 0:
                                if (BASE_IDRSLICE == stStream.stH264Info.enRefType ||
                                    BASE_PSLICE_REFBYBASE == stStream.stH264Info.enRefType)
                                {  
                                    #ifndef __HuaweiLite__
                                    s32Ret = SAMPLE_COMM_VENC_SaveStream(enPayLoadType[i], pFile[i+s32Cnt], &stStream);
                                    #else
									s32Ret = SAMPLE_COMM_VENC_SaveFile(pFile[i + s32Cnt], &stStreamBufInfo[i], &stStream);
                                    #endif
                                }
                                break;
                            case 1:
                                if (BASE_IDRSLICE == stStream.stH264Info.enRefType      ||
                                    BASE_PSLICE_REFBYBASE == stStream.stH264Info.enRefType ||
                                    BASE_PSLICE_REFBYENHANCE == stStream.stH264Info.enRefType)
                                {
                                    #ifndef __HuaweiLite__
                                    s32Ret = SAMPLE_COMM_VENC_SaveStream(enPayLoadType[i], pFile[i+s32Cnt], &stStream);
                                    #else
									s32Ret = SAMPLE_COMM_VENC_SaveFile(pFile[i + s32Cnt], &stStreamBufInfo[i], &stStream);
                                    #endif
                                }
                                break;
                            case 2:
                                #ifndef __HuaweiLite__
                                s32Ret = SAMPLE_COMM_VENC_SaveStream(enPayLoadType[i], pFile[i+s32Cnt], &stStream);
                                #else
								s32Ret = SAMPLE_COMM_VENC_SaveFile(pFile[i + s32Cnt], &stStreamBufInfo[i], &stStream);
                                #endif
                                break;
                        }
                        if (HI_SUCCESS != s32Ret)
                        {
                            free(stStream.pstPack);
                            stStream.pstPack = NULL;
                            SAMPLE_PRT("save stream failed!\n");
                            break;
                        }
                    }
#else
                    for (s32Cnt = 0; s32Cnt < 3; s32Cnt++)
                    {
                        if (s32Cnt == 0)
                        {
                            if (NULL != pFile[i + s32Cnt])
                            {
                                if (BASE_IDRSLICE == stStream.stH264Info.enRefType ||
                                    BASE_PSLICE_REFBYBASE == stStream.stH264Info.enRefType)
                                {
                                    s32Ret = SAMPLE_COMM_VENC_SaveStream(enPayLoadType[i], pFile[i + s32Cnt], &stStream);
                                }

                            }

                        }
                        else if (s32Cnt == 1)
                        {
                            if (NULL != pFile[i + s32Cnt])
                            {
                                if (BASE_IDRSLICE == stStream.stH264Info.enRefType         ||
                                    BASE_PSLICE_REFBYBASE == stStream.stH264Info.enRefType ||
                                    BASE_PSLICE_REFBYENHANCE == stStream.stH264Info.enRefType)
                                {
                                    s32Ret = SAMPLE_COMM_VENC_SaveStream(enPayLoadType[i], pFile[i + s32Cnt], &stStream);
                                }
                            }
                        }
                        else
                        {
                            if (NULL != pFile[i + s32Cnt])
                            {
                                s32Ret = SAMPLE_COMM_VENC_SaveStream(enPayLoadType[i], pFile[i + s32Cnt], &stStream);
                            }
                        }
                    }
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("save stream failed!\n");
                        break;
                    }
#endif
                    /*******************************************************
                    step 2.6 : release stream
                    *******************************************************/
                    s32Ret = HI_MPI_VENC_ReleaseStream(i, &stStream);
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        break;
                    }                    
                    /*******************************************************
                     step 2.7 : free pack nodes
                    *******************************************************/
                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                }
            }
        }
    }    
    /*******************************************************
     step 3 : close save-file
    *******************************************************/
    for (i = 0; i < s32ChnTotal; i++)
    {
        for (s32Cnt = 0; s32Cnt < 3; s32Cnt++)
        {
            if (pFile[i + s32Cnt])
            {
                fclose(pFile[i + s32Cnt]);
            }
        }
    }
    return NULL;
}

/******************************************************************************
* funciton : start get venc stream process thread
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_StartGetStream(HI_S32 s32Cnt)
{
    gs_stPara.bThreadStart = HI_TRUE;
    gs_stPara.s32Cnt = s32Cnt;
    return pthread_create(&gs_VencPid, 0, SAMPLE_COMM_VENC_GetVencStreamProc, (HI_VOID*)&gs_stPara);
}

/******************************************************************************
* funciton : start get venc svc-t stream process thread
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_StartGetStream_Svc_t(HI_S32 s32Cnt)
{
    gs_stPara.bThreadStart = HI_TRUE;
    gs_stPara.s32Cnt = s32Cnt;
    return pthread_create(&gs_VencPid, 0, SAMPLE_COMM_VENC_GetVencStreamProc_Svc_t, (HI_VOID*)&gs_stPara);
}

/******************************************************************************
* funciton : stop get venc stream process.
******************************************************************************/
HI_S32 SAMPLE_COMM_VENC_StopGetStream()
{
    if (HI_TRUE == gs_stPara.bThreadStart)
    {
        gs_stPara.bThreadStart = HI_FALSE;
        pthread_join(gs_VencPid, 0);
    }
    return HI_SUCCESS;
}

HI_VOID SAMPLE_COMM_VENC_ReadOneFrame( FILE* fp, HI_U8* pY, HI_U8* pU, HI_U8* pV,
                                       HI_U32 width, HI_U32 height, HI_U32 stride, HI_U32 stride2)
{
    HI_U8* pDst;
    HI_U32 u32Row;
    pDst = pY;
    for ( u32Row = 0; u32Row < height; u32Row++ )
    {
        fread( pDst, width, 1, fp );
        pDst += stride;
    }
    pDst = pU;
    for ( u32Row = 0; u32Row < height / 2; u32Row++ )
    {
        fread( pDst, width / 2, 1, fp );
        pDst += stride2;
    }
    pDst = pV;
    for ( u32Row = 0; u32Row < height / 2; u32Row++ )
    {
        fread( pDst, width / 2, 1, fp );
        pDst += stride2;
    }
}

HI_S32 SAMPLE_COMM_VENC_PlanToSemi(HI_U8* pY, HI_S32 yStride,
                                   HI_U8* pU, HI_S32 uStride,
                                   HI_U8* pV, HI_S32 vStride,
                                   HI_S32 picWidth, HI_S32 picHeight)
{
    HI_S32 i;
    HI_U8* pTmpU, *ptu;
    HI_U8* pTmpV, *ptv;
    HI_S32 s32HafW = uStride >> 1 ;
    HI_S32 s32HafH = picHeight >> 1 ;
    HI_S32 s32Size = s32HafW * s32HafH;

	pTmpU = malloc( s32Size );
	if(pTmpU == HI_NULL)
	{
        printf("malloc buf failed\n");
        return HI_FAILURE;
    }
    ptu = pTmpU;
	
    pTmpV = malloc( s32Size );
    if (pTmpV == HI_NULL)
    {
        printf("malloc buf failed\n");
		free( pTmpU );
        return HI_FAILURE;
    }	
    ptv = pTmpV;
	
    memcpy(pTmpU, pU, s32Size);
    memcpy(pTmpV, pV, s32Size);
    for (i = 0; i<s32Size >> 1; i++)
    {
        *pU++ = *pTmpV++;
        *pU++ = *pTmpU++;
    }
    for (i = 0; i<s32Size >> 1; i++)
    {
        *pV++ = *pTmpV++;
        *pV++ = *pTmpU++;
    }
    free( ptu );
    free( ptv );
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

