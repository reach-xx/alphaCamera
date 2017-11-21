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


static pthread_t gs_ProtoPid;
static pthread_t gs_ProcessPid;

static HI_BOOL gs_ThreadStart = HI_TRUE;
/*Զң����ʵʱ����ֵ*/
static RH_Coord gs_Coord = {0};
static int 	gs_inc = 6;
/*VISCAЭ���ʽ����*/
static RH_Procotol procoVisca = {
	/*����VISCAЭ��*/
	.type	  =   VISCA_PROCO, 
	.cmd_nums =   12,  /*��̨������*/
	.stCMD    = 
	{
		{
			VISCA_UP, /*��*/
			9,        /*�����*/
			{0x81,0x01,0x06,0x01,0x00,0x00,0x03,0x01,0xFF},
		},
		{
			VISCA_DOWN, /*��*/
			9,		    /*�����*/
			{0x81,0x01,0x06,0x01,0x00,0x00,0x03,0x02,0xFF},
		},	
		{
			VISCA_LEFT, /*��*/
			9,		    /*�����*/
			{0x81,0x01,0x06,0x01,0x00,0x00,0x01,0x03,0xFF},
		},	
		{
			VISCA_RIGHT, /*��*/
			9,		     /*�����*/
			{0x81,0x01,0x06,0x01,0x00,0x00,0x02,0x03,0xFF},
		},	
		{
			VISCA_STOP, /*ֹͣ*/
			9,		    /*�����*/
			{0x81,0x01,0x06,0x01,0x00,0x00,0x03,0x03,0xFF},
		},	
		{
			VISCA_UPLEFT, /*����*/
			9,		    /*�����*/
			{0x81,0x01,0x06,0x01,0x00,0x00,0x01,0x01,0xFF},
		},	
		{
			VISCA_UPRIGHT, /*����*/
			9,			/*�����*/
			{0x81,0x01,0x06,0x01,0x00,0x00,0x02,0x01,0xFF},
		},
		{
			VISCA_DOWNLEFT, /*����*/
			9,			/*�����*/
			{0x81,0x01,0x06,0x01,0x00,0x00,0x01,0x02,0xFF},
		},			
		{
			VISCA_DOWNRIGHT, /*����*/
			9,				/*�����*/
			{0x81,0x01,0x06,0x01,0x00,0x00,0x02,0x02,0xFF},
		},
		{
			VISCA_ZOOMTELE, /*�䱶��*/
			6,				/*�����*/
			{0x81,0x01,0x04,0x07,0x02,0xFF},
		},		
		{
			VISCA_ZOOMWIDE, /*�䱶С*/
			6,				/*�����*/
			{0x81,0x01,0x04,0x07,0x03,0xFF},
		},	
		{
			VISCA_STOP, 	/*�䱶ͣ*/
			6,				/*�����*/
			{0x81,0x01,0x04,0x07,0x03,0xFF},
		},		
	},
};

//���÷�����  
static void SetNonBlocking(int sockfd) 
{  
    int flag = fcntl(sockfd, F_GETFL, 0);  
    if (flag < 0) 
    {  
        perror("fcntl F_GETFL fail");  
        return;  
    } 
 
    if (fcntl(sockfd, F_SETFL, flag | O_NONBLOCK) < 0) 
    {  
        perror("fcntl F_SETFL fail");  
    }  
}  


/*�Ƚ������Ƿ����*/
HI_BOOL CompareCmd(unsigned char *src ,unsigned char *dst, int len)
{
	int cnt = 0 ;
	
	for (cnt = 0 ;cnt < len ; cnt++)   {
		if (*src++ != *dst++) {
			return HI_FALSE;
		}
	}
	return HI_TRUE;
}

/*��̨����*/
HI_S32 RH_CoordSet(const RH_Coord *pCoord)
{
	HI_S32 S32Ret;
	VPSS_CROP_INFO_S stVpssCropInfo = {0};
	
	stVpssCropInfo.bEnable = HI_TRUE;
	stVpssCropInfo.enCropCoordinate = VPSS_CROP_ABS_COOR;
	stVpssCropInfo.stCropRect.u32Width  = OUTPUT_WIDTH;
	stVpssCropInfo.stCropRect.u32Height = OUTPUT_HEIGHT;

	return S32Ret;
}


/*VISCAЭ�麯��*/
HI_S32 RH_CoordCalc(RH_Coord *pCoord)
{
	HI_S32 s32Ret = HI_SUCCESS;	
	
	switch (pCoord->cmd_mode)
	{
		case VISCA_UP:   //UP
			pCoord->rect.s32Y -= gs_inc*pCoord->vSpeed;
			if(pCoord->rect.s32Y <= 0)	{
				pCoord->rect.s32Y = 0;
			}	
			break;
		case VISCA_DOWN:
			
			break;
		case VISCA_LEFT:
			
			break;
		case VISCA_RIGHT:
			
			break;
		case VISCA_UPLEFT:
			
			break;
		case VISCA_DOWNLEFT:
			
			break;
		case VISCA_UPRIGHT:
			
			break;
		case VISCA_DOWNRIGHT:
			
			break;
		case VISCA_STOP:
			
			break;
		case VISCA_ZOOMTELE:   //�䱶��
		
			break;
		case VISCA_ZOOMWIDE:   //�䱶С
			
			break;
		default:
			SAMPLE_PRT("visca command error!!! mode =%d\n", pCoord->cmd_mode);
			break;	
	}
	return s32Ret;
}


/*VISCA�����������*/
int ViscaProcoParse(unsigned char *pCmd, int length)
{
	RH_Procotol *pVisca = &procoVisca;
	RH_Coord  *pCoord = &gs_Coord;
	int cnt = 0;
	HI_BOOL is_equal = HI_TRUE;

	if (pVisca->cmd_nums < MAX_CMD_NUMS) {
		SAMPLE_PRT("max command numbers:%d\n",pVisca->cmd_nums);
		return HI_FAILURE;
	}

	for (cnt = 0; cnt < pVisca->cmd_nums; cnt++)  {		
		is_equal = CompareCmd(pCmd, pVisca->stCMD[cnt].code,length);
		if(HI_TRUE == is_equal) {  /*�ҵ���Ӧ����*/
			pCoord->bEnable = HI_TRUE;
			pCoord->cmd_mode = pVisca->stCMD[cnt].cmd_mode;
			if(length == 9)  {
				pCoord->vSpeed = pVisca->stCMD[cnt].code[4];
				pCoord->wSpeed = pVisca->stCMD[cnt].code[5];
				if(pCoord->vSpeed < 2)			
					pCoord->vSpeed = 2;
				if(pCoord->wSpeed < 2)
					pCoord->wSpeed  = 2;
			}
		}
	}
	
	return HI_SUCCESS;
}

/*Э����ս����߳�*/
HI_VOID* RH_UDP_Recv_Parse(HI_VOID* pdata)
{
    struct sockaddr_in addr;
	unsigned char buff[512] = {0};
	int sock;
	
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);	
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)    {
        perror("socket");
        return ;
    }
	SetNonBlocking(sock);	
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)   {
        perror("bind");
        return ;
    }
	
    struct sockaddr_in clientAddr;
    int iSize = 0;
    int length = sizeof(clientAddr);

	while(HI_TRUE == gs_ThreadStart)
	{
        iSize = recvfrom(sock,buff,sizeof(buff), 0, (struct sockaddr*)&clientAddr, &length);
        if (iSize > 0)  {
			/*TODO��Ŀǰֻ����VISCAЭ��*/
        	ViscaProcoParse(buff,length);
        }
		usleep(2000);
	}
	return ;
}


/*Э�鴦���߳�*/
HI_VOID* RH_ProtoProcess(HI_VOID* pdata)
{
	RH_Coord *pCoord = &gs_Coord;	

	while(HI_TRUE == gs_ThreadStart)
	{
		RH_CoordCalc(pCoord);
		RH_CoordSet(pCoord);
		usleep(33*1000);
	}
	return ;
}

/*Ycat ����ʹ��UDP����ViscaЭ��*/
HI_S32 RH_UDP_Proto_Start(HI_VOID)
{
	HI_S32 s32Ret = HI_SUCCESS;
	int i = 0;

	s32Ret = pthread_create(&gs_ProtoPid, 0, RH_UDP_Recv_Parse, (HI_VOID*)NULL);
	s32Ret = pthread_create(&gs_ProcessPid, 0, RH_ProtoProcess, (HI_VOID*)NULL);	
	return s32Ret;
}


/*ֹͣ*/
HI_U32 RH_UDP_Proto_Stop(HI_VOID)
{
    /* join thread */
    if (HI_TRUE == gs_ThreadStart)
    {
        gs_ThreadStart = HI_FALSE;
        pthread_join(gs_ProtoPid, 0);
		pthread_join(gs_ProcessPid, 0);
    }
	return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

