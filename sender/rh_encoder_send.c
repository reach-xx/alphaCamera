

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif   /* End of #ifdef __cplusplus */
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "ReachLive555Port.h"

//#include "sample_comm.h"
#include "rh_interface.h"
#include "rh_encoder_send.h"





static pthread_t gs_VencPid;
static SAMPLE_VENC_GETSTREAM_PARA_S gs_stPara;


/*client param infomation*/



int 	gRunStatus = 1;
int 	gRunStatus_ex[3] = {1, 1, 1};
static unsigned long g_video_time = 0;
static unsigned int gnAudioCount = 0;
sem_t	g_sem;
sem_t	g_sem_ex[3];
AudioParam		gAudioParam;
SYSPARAMS		gSysParsm;
DSPCliParam		gDSPCliPara[DSP_NUM];
static unsigned int				gnSentCount					= 0;
static unsigned int				gnSentCount110					= 0;

static unsigned char			gszSendBuf[MAX_PACKET];
static unsigned char            gszAudioBuf[4800];
VideoParam gvideoPara;

/*send data packet*/
static unsigned char gszSendBuf110[MAX_PACKET];
pthread_mutex_t g_send_m;
pthread_mutex_t g_send110_m;


#define ENC_MAX_NUM 10


#define MAX_STREAM_NUM 4
int ClientType[MAX_STREAM_NUM]  = {-1}; 
int studentFlag = 0; 

static unsigned long g_audio_time = 0;


static void *rtspSend[3] = {NULL,NULL,NULL};
static RtmpHandle handle[3];



typedef struct EncoderIndex_ {
	int32_t eindex;
	int32_t pos;
} EncoderIndex_t;


static unsigned long getCurrentTime2(void)
{
	struct timeval tv;
	struct timezone tz;
	unsigned long ultime;

	gettimeofday(&tv , &tz);
	ultime = tv.tv_sec * 1000 + ((tv.tv_usec) / 1000);
	return (ultime);
}


int SetSendTimeOut(int sSocket, unsigned long time)
{
	struct timeval timeout ;
	int ret = 0;

	timeout.tv_sec = time ; //3
	timeout.tv_usec = 0;

	ret = setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

	if(ret == -1) {
		printf("setsockopt() Set Send Time Failed\n");
	}

	return ret;
}
void PackHeaderMSG110(unsigned char *data,
                   int type, int len)
{
	MSGHEAD110  *p;
	p = (MSGHEAD110 *)data;
	memset(p, 0, HEAD110_LEN);
	p->nLen = htons(len);
	p->nMsg = type;
	return ;
}

void PackHeaderMSG(unsigned char *data,
                   int type, int len)
{
	MSGHEAD  *p;
	p = (MSGHEAD *)data;
	memset(p, 0, HEAD_LEN);
	p->nLen = htons(len);
	p->nMsg = type;
	return ;
}


static int GetNullClientData(unsigned char dsp)
{
	int cli ;

	for(cli = 0; cli < MAX_CLIENT; cli++) {
		//		printf("\n\n");
		//		printf("cnt: %d		used: %d\n", cli, ISUSED(dsp, cli));
		//		printf("cnt: %d		login: %d\n", cli, ISLOGIN(dsp, cli));
		//		printf("\n\n\n");
		if((!ISUSED(dsp, cli)) && (!ISLOGIN(dsp, cli))) {
			return cli;
		}
	}

	return -1;
}

void InitClient(unsigned char dsp)
{
	int cli ;

	for(cli = 0; cli < MAX_CLIENT; cli++) {

		SETSOCK(dsp, cli, INVALID_SOCKET);
		SETCLILOGIN(dsp, cli, FALSE);
		SETCLIUSED(dsp, cli, FALSE);
	}
}

void ClearLostClient(unsigned char dsp)
{
	int cli ;

	for(cli = 0; cli < MAX_CLIENT; cli++) {
		if(!ISUSED(dsp, cli) && ISSOCK(dsp, cli)) {
			close(GETSOCK(dsp, cli));
			SETSOCK(dsp, cli, INVALID_SOCKET);
			SETCLILOGIN(dsp, cli, FALSE);

		}
	}
}


/*set audio header infomation*/
static void SetAudioHeader(FRAMEHEAD *pAudio, AudioParam *pSys)
{
	pAudio->nDataCodec = 0x53544441;					//"ADTS"
	pAudio->nFrameRate = pSys->SampleRate; 			//sample rate 1-----44.1KHz
	pAudio->nWidth = pSys->Channel;					//channel (default: 2)
	pAudio->nHight = pSys->SampleBit;				//sample bit (default: 16)
	pAudio->nColors = pSys->BitRate;					//bitrate  (default:64000)
}

int SetVideoParam(int dsp, unsigned char *data, int len)
{
	unsigned char temp[200];
	VideoParam *Newp, *Oldp;
	memcpy(temp, data, len);
	Oldp = &gvideoPara;
	Newp = (VideoParam *)&temp[0];
	printf("set video param  old nFrameRate : [%d], new nFrameRate : [%d] bitrate : [%d]\n\n",
	       Oldp->nFrameRate, Newp->nFrameRate, Newp->sBitrate);

	memcpy(Oldp, Newp, sizeof(VideoParam));
	
	return 0;
}


void GetAudioParam(int socket, int dsp, unsigned char *data, int len)
{
	unsigned char temp[200];
	int length = 0, ret;

	length = HEAD_LEN + sizeof(gAudioParam);
	PackHeaderMSG(temp, MSG_GET_AUDIOPARAM, length);

	memcpy(temp + HEAD_LEN, &gAudioParam, sizeof(gAudioParam));
	ret = WriteData(socket, temp, length);

	if(ret < 0) {
		printf("Send GetAudioParam()return:%d socket:%d\n", ret, socket);
	}


	return ;
}


int WriteData(int s, void *pBuf, int nSize)
{
	int nWriteLen = 0;
	int nRet = 0;
	int nCount = 0;

	//	pthread_mutex_lock(&gSetP_m.send_m);
	while(nWriteLen < nSize) {
		nRet = send(s, (char *)pBuf + nWriteLen, nSize - nWriteLen, 0);

		if(nRet < 0) {
			printf("WriteData ret =%d,sendto=%d,errno=%d,s=%d\n", nRet, nSize - nWriteLen, s, errno);

			if((errno == ENOBUFS) && (nCount < 10)) {
				printf("network buffer have been full!\n");
				usleep(10000);
				nCount++;
				continue;
			}

			//			pthread_mutex_unlock(&gSetP_m.send_m);
			return nRet;
		} else if(nRet == 0) {
			printf("WriteData ret =%d,sendto=%d,errno=%d,s=%d\n", nRet, nSize - nWriteLen, s, errno);
			printf("Send Net Data Error nRet= %d\n", nRet);
			continue;
		}

		nWriteLen += nRet;
		nCount = 0;
	}

	//	pthread_mutex_unlock(&gSetP_m.send_m);
	return nWriteLen;
} 

/*send encode audio data to every client*/
void SendAudioToClientEx(int nLen, unsigned char *pData,
                       int nFlag, unsigned char index, unsigned int samplerate)
{

	pthread_mutex_lock(&g_send_m);
	int nRet, nSent, nSendLen, nPacketCount, nMaxDataLen;
	FRAMEHEAD  DataFrame;
	int cnt = 0;
	MSGHEAD  *p;

	//	ParsePackageLock();
	bzero(&DataFrame, sizeof(FRAMEHEAD));
	DataFrame.dwPacketNumber = gnAudioCount++;

	nSent = 0;
	nSendLen = 0;
	nPacketCount = 0;
	nMaxDataLen = MAX_PACKET - sizeof(FRAMEHEAD) - HEAD_LEN;

	DataFrame.ID = 0x56534434;		// " 4DSP"
	DataFrame.nFrameLength = nLen;


	unsigned int timeTick = 0;

	if(g_audio_time == 0) {
		timeTick = 0 ;
//		g_audio_time = getCurrentTime2() ;
	} else {
	//	timeTick = getCurrentTime2() - g_audio_time;
	}

	DataFrame.nTimeTick = timeTick;
	SetAudioHeader(&DataFrame, &gAudioParam);
	//	DataFrame.nWidth = 2;					//sound counts (default:2)
	//DataFrame.nHight = samplerate;					//samplebit (default:16)

	if(nFlag == 1) {
		DataFrame.dwFlags = AVIIF_KEYFRAME;
	} else {
		DataFrame.dwFlags = 0;
	}

	while(nSent < nLen) {

		if(nLen - nSent > nMaxDataLen) {
			nSendLen = nMaxDataLen;

			if(nPacketCount == 0) {
				DataFrame.dwSegment = 2;
			} else {
				DataFrame.dwSegment = 0;
			}

			nPacketCount++;
		} else {
			nSendLen = nLen - nSent;

			if(nPacketCount == 0) {
				DataFrame.dwSegment = 3;
			} else {
				DataFrame.dwSegment = 1;
			}

			nPacketCount++;
		}

		memcpy(gszAudioBuf + HEAD_LEN, &DataFrame, sizeof(FRAMEHEAD));
		memcpy(gszAudioBuf + sizeof(FRAMEHEAD) + HEAD_LEN, pData + nSent, nSendLen);
		//PackHeaderMSG(gszAudioBuf,MSG_AUDIODATA,(nSendLen+sizeof(FRAMEHEAD)+HEAD_LEN));
		p = (MSGHEAD *)gszAudioBuf;
		memset(p, 0, HEAD_LEN);
		p->nLen = htons((nSendLen + sizeof(FRAMEHEAD) + HEAD_LEN));
		p->nMsg = MSG_AUDIODATA;


		for(cnt = 0; cnt < MAX_CLIENT; cnt++) {
			if(ISUSED(index, cnt) && ISLOGIN(index, cnt)) {
				nRet = WriteData(GETSOCK(index, cnt), gszAudioBuf, nSendLen + sizeof(FRAMEHEAD) + HEAD_LEN);

				if(nRet < 0) {
					SETCLIUSED(index, cnt, FALSE);
					SETCLILOGIN(index, cnt, FALSE);
					printf("Audio Send error index:%d, cnt:%d %d nSendLen:%d  ,nRet:%d\n", index, cnt, errno, nSendLen, nRet);
				}
			}
		}

		nSent += nSendLen;
	}

	pthread_mutex_unlock(&g_send_m);
	//	ParsePackageunLock();
}



void SendDataToClientEx(int nLen, unsigned char *pData,
                      int nFlag, unsigned char index, int width, int height)
{

	pthread_mutex_lock(&g_send_m);

	int nRet, nSent, nSendLen, nPacketCount, nMaxDataLen;
	FRAMEHEAD	DataFrame;
	int cnt = 0;
	SOCKET	sendsocket = 0;
	MSGHEAD	*p;
	//pthread_mutex_lock(&g_send_m);
	bzero(&DataFrame, sizeof(FRAMEHEAD));
	nSent = 0;
	nSendLen = 0;
	nPacketCount = 0;
	nMaxDataLen = MAX_PACKET - sizeof(FRAMEHEAD) - HEAD_LEN;


	DataFrame.ID = 0x56534434;		// "4DSP"
	DataFrame.nFrameLength = nLen;
	DataFrame.nDataCodec = 0x34363248;		//"H264"

	unsigned int timeTick = 0;

	if(g_video_time == 0) {
		timeTick = 0 ;
		g_video_time = getCurrentTime2();
	} else {

		timeTick = getCurrentTime2() - g_video_time;

	}

	DataFrame.nTimeTick = timeTick;

	DataFrame.nWidth = width;		//video width
	DataFrame.nHight = height;		//video height

	if(DataFrame.nHight == 1088) {
		DataFrame.nHight = 1080;
	}


	if(nFlag == 1)
		//if I frame
	{
		DataFrame.dwFlags = AVIIF_KEYFRAME;
	} else {
		DataFrame.dwFlags = 0;
	}


	while(nSent < nLen) {
		if(nLen - nSent > nMaxDataLen) {
			nSendLen = nMaxDataLen;

			if(nPacketCount == 0) {
				DataFrame.dwSegment = 2;    //start frame
			} else {
				DataFrame.dwSegment = 0;    //middle frame
			}

			nPacketCount++;
		} else {
			nSendLen = nLen - nSent;

			if(nPacketCount == 0) {
				DataFrame.dwSegment = 3;    //first frame and last frame
			} else {
				DataFrame.dwSegment = 1;    //last frame
			}

			nPacketCount++;
		}


		DataFrame.dwPacketNumber = gnSentCount++;
		memcpy(gszSendBuf + HEAD_LEN, &DataFrame, sizeof(FRAMEHEAD));
		memcpy(gszSendBuf + sizeof(FRAMEHEAD) + HEAD_LEN, pData + nSent, nSendLen);
		p = (MSGHEAD *)gszSendBuf;
		memset(p, 0, HEAD_LEN);
		p->nLen = htons((nSendLen + sizeof(FRAMEHEAD) + HEAD_LEN));
		p->nMsg = MSG_SCREENDATA;

		//send multi client
		for(cnt = 0; cnt < MAX_CLIENT; cnt++) {
			if(ISUSED(0, cnt) && ISLOGIN(0, cnt)) {
				sendsocket = GETSOCK(0, cnt);
				int sendflag = 0;
				if(index == ClientType[cnt])
				{
					sendflag = 1;
				}
				if(sendsocket > 0 && sendflag) {
					nRet = WriteData(sendsocket, gszSendBuf, nSendLen + sizeof(FRAMEHEAD) + HEAD_LEN);

					if(nRet < 0) {
						SETCLIUSED(0, cnt, FALSE);
						SETCLILOGIN(0, cnt, FALSE);
				
						printf("Error: SOCK = %d count = %d  errno = %d  ret = %d\n", sendsocket, cnt, errno, nRet);
					}
				}
			}

		}

		nSent += nSendLen;
	}

	pthread_mutex_unlock(&g_send_m);
}


void SendDataToClient110(int nLen, unsigned char *pData,
                         int nFlag, unsigned char index , int width, int height)
{
	int nRet, nSent, nSendLen, nPacketCount, nMaxDataLen;
	DataHeader  DataFrame;
	int cnt = 0;
	SOCKET  sendsocket = 0;

	pthread_mutex_lock(&g_send110_m);
	bzero(&DataFrame, sizeof(DataHeader));
	//	static uint32_t time = 0;
	//	static uint32_t test = 0;
	nSent = 0;
	nSendLen = 0;
	nPacketCount = 0;
	nMaxDataLen = MAX_PACKET - sizeof(DataHeader) - HEAD110_LEN;
	DataFrame.id = 0x1001;

	DataFrame.dwCompressedDataLength = nLen;

	DataFrame.biBitCount = 24; 
	DataFrame.biWidth = width;		
	DataFrame.biHeight = height;
	if(nFlag == 1)	{	//if I frame
		DataFrame.dwFlags = AVIIF_KEYFRAME;
	} else {
		DataFrame.dwFlags = 0;
	}

	DataFrame.fccCompressedHandler = 0x34363248;//0x30484352; H264
	DataFrame.fccHandler = 0x34363248;//0x30484352;  H264


	//char *sendbuf = r_malloc(MAX_PACKET);

	while(nSent < nLen) {
		if(nLen - nSent > nMaxDataLen) {
			nSendLen = nMaxDataLen;

			if(nPacketCount == 0) {
				DataFrame.dwSegment = 2;    //start frame
			} else {
				DataFrame.dwSegment = 0;    //middle frame
			}

			nPacketCount++;
		} else {
			nSendLen = nLen - nSent;

			if(nPacketCount == 0) {
				DataFrame.dwSegment = 3;    //first frame and last frame
			} else {
				DataFrame.dwSegment = 1;    //last frame
			}

			nPacketCount++;
		}

		DataFrame.dwPacketNumber = gnSentCount110++;
		memcpy(gszSendBuf110 + HEAD110_LEN, &DataFrame, sizeof(DataHeader));
		memcpy(gszSendBuf110 + sizeof(DataHeader) + HEAD110_LEN, pData + nSent, nSendLen);
		gszSendBuf110[0] = (nSendLen + sizeof(DataHeader) + HEAD110_LEN) >> 8;
		gszSendBuf110[1] = (nSendLen + sizeof(DataHeader) + HEAD110_LEN) & 0xff;
		gszSendBuf110[2] = MSG_SCREENDATA;


		//send multi client
		for(cnt = 0; cnt < MAX_CLIENT; cnt++) {
			if(ISUSED(DSP2, cnt) && ISLOGIN(DSP2, cnt)) {
				sendsocket = GETSOCK(DSP2, cnt);

		
				if((sendsocket > 0))  {
					nRet = WriteData(sendsocket, gszSendBuf110, nSendLen + sizeof(DataHeader) + HEAD110_LEN);

					if(nRet < 0) {
						SETCLIUSED(DSP2, cnt, FALSE);
						SETCLILOGIN(DSP2, cnt, FALSE);

						printf("Send to socket Error: SOCK = %d count = %d  errno = %d  ret = %d\n", sendsocket, cnt, errno, nRet);
					}
				}
			}
		}

		nSent += nSendLen;
	}

#if 0

	if(sendbuf) {
		r_free(sendbuf);
	}

#endif
	pthread_mutex_unlock(&g_send110_m);
}


/*get video param*/
void GetVideoParam(int socket, int dsp, unsigned char *data, int len)
{
	unsigned char temp[200];
	int length = 0, ret;
	length = HEAD_LEN + sizeof(gvideoPara);
	PackHeaderMSG(temp, MSG_GET_VIDEOPARAM, length);

	memcpy(temp + HEAD_LEN, &gvideoPara, sizeof(gvideoPara));
	ret = WriteData(socket, temp, length);

	if(ret < 0) {
		printf("Send GetAudioParam()return:%d socket:%d\n", ret, socket);
	}

	return ;
}



/*Initial audio param table*/
static void InitAudioParam(AudioParam *audio)
{

	//å…¨åˆå§‹åŒ–ä¸ºé›¶
	bzero(audio, sizeof(AudioParam));
	audio->Codec = ADTS;		//"ADTS"
	///0---8KHz
	///2---44.1KHz éŸ³é¢‘é‡‡æ ·çŽ?(é»˜è®¤ä¸?4100)
	///1---32KHz
	///3---48KHz
	///å…¶ä»–-----96KHz
	audio->SampleRate = 3;
	audio->Channel = 2;					//å£°é“æ•?(é»˜è®¤ä¸?)
	audio->SampleBit = 16;				//é‡‡æ ·ä½?(é»˜è®¤ä¸?6)
	audio->BitRate = 12800;				//ç¼–ç å¸¦å®½ (é»˜è®¤ä¸?4000)
	audio->InputMode = LINE_INPUT;		//çº¿æ€§è¾“å…?	audio->RVolume = 31;					//å·¦å³å£°é“éŸ³é‡
	audio->LVolume = 31;
}

/*modify standard system param*/
static void InitStandardParam(SYSPARAMS *psys)
{
	memset(psys, 0, sizeof(*psys));
	psys->dwAddr = 0;
	psys->dwGateWay =  0;
	psys->dwNetMark = 0;

	psys->szMacAddr[0] = 0x00;
	psys->szMacAddr[1] = 0x09;
	psys->szMacAddr[2] = 0x30;
	psys->szMacAddr[3] = 0x28;
	psys->szMacAddr[4] = 0x12;
	psys->szMacAddr[5] = 0x22;

	strcpy(psys->strName, "EduKit-SD");
	strcpy(psys->strVer, "V1.0.0");
	psys->unChannel = 1;
	psys->bType = 6;  //0 -------VGABOX  3-------200 4-------110 5-------120 6--------1200  8 --ENC-1100
	bzero(psys->nTemp, sizeof(psys->nTemp));

}

static void InitVideoParam(VideoParam *video)
{
	bzero(video, sizeof(VideoParam));
	video->nDataCodec = 0x34363248;	//"H264"
	video->nWidth = 1920;			//video width
	video->nHight = 1080;			//video height
	video->nQuality = 45;				//quality (5---90)
	video->sCbr = 0;					// 0---quality  1---bitrate
	video->nFrameRate = 25;			//current framerate
	video->sBitrate = 4096;				//bitrate (128k---4096k)
	video->nColors = 24;			//24

}
static int Pwd2ClientTypePro(char *pPwd, int nPos)
{
	if (nPos < 0 || nPos > MAX_STREAM_NUM)
	{
		printf("nPos = %d, [%d ~ %d]!!!!!!\n", nPos, 0, MAX_STREAM_NUM);
		return 0;
	}

	int pwd = 0;

	pwd = atoi(pPwd);
	if (pwd < 0 || pwd > MAX_STREAM_NUM)
	{
		printf("atoi(pPwd) = %d, [%d ~ %d]!!!!!!\n", nPos, 0, MAX_STREAM_NUM);
		return 0;
	}
	else
	{
		printf("nPos = [%d], pwd = [%d]!!!!\n",  nPos, pwd);
	}

	ClientType[nPos] = pwd;

	return 1;
}


#define	MAX_EM_MSG_LEN	   255
static void *EncoderProcess(void *pParams)
{
	int					sSocket = -1;
	//WORD					length;

	//char			szPass[] = "reach123";
	char *szData = NULL;

	int 					nPos					= 0;
	int 					nMsgLen 				= 0;
	int 					*pnPos					= (int *)pParams;
	int 					nLen					= 0;
	MSGHEAD header, *phead;

	nPos = *pnPos;
	free(pParams);
	sSocket = GETSOCK(DSP1, nPos);
	//SysParamsV2 sys2;
//	SysParamsV2 sys2;
	/*????TCP?????? 3s*/
	SetSendTimeOut(sSocket, 3);

	sem_post(&g_sem);

	phead = &header;
	memset(&header, 0, sizeof(MSGHEAD));
	szData = (char *)malloc(MAX_EM_MSG_LEN + 1);

	if(NULL == szData) {
		printf( "NULL == szData\n");
		return NULL;
	}

	memset(szData, 0, MAX_EM_MSG_LEN + 1);

	while(gRunStatus) {
		memset(szData, 0, MAX_EM_MSG_LEN);

		if(sSocket <= 0) {
			printf( "sSocket<0 !\n");
			goto ExitClientMsg;
		}

		nLen = recv(sSocket, szData, HEAD_LEN, 0);
		printf( "receive length:%d,errno=%d\n", nLen, errno);

		if(nLen < HEAD_LEN || nLen == -1) {
			printf( "recv nLen < %d || nLen == -1  goto ExitClientMsg\n ", HEAD_LEN);
			goto ExitClientMsg;
		}

		memcpy(phead, szData, HEAD_LEN);

		nMsgLen = ntohs(*((short *)szData));
		phead->nLen = nMsgLen;
		
		if(MAX_EM_MSG_LEN < nMsgLen) {
			printf( "sSocket = %d, msg = 0x%02x ver:[%d] nMsgLen = %d\n", sSocket, phead->nMsg, phead->nVer, nMsgLen);
			goto ExitClientMsg;
		}
		nLen = recv(sSocket, szData + HEAD_LEN, nMsgLen - HEAD_LEN, 0);

		if(nLen == -1 || nLen < nMsgLen - HEAD_LEN) {
			printf( "sSocket = %d, msg = 0x%02x ver:[%d] nMsgLen = %d\n", sSocket, phead->nMsg, phead->nVer, nMsgLen);
			goto ExitClientMsg;
		}

		printf( "sSocket = %d, msg = 0x%02x, nLen = %d, nMsgLen = %d\n", sSocket, phead->nMsg, nLen, nMsgLen);

		/*???????????*/
		switch(phead->nMsg) {
			case MSG_REQ_AUDIO:
				printf( "DSP1 request Audio Data \n");
				break;

			case MSG_GET_AUDIOPARAM:
				printf( "DSP1 Get AudioParam \n");

				GetAudioParam(sSocket, PORT_ONE, (BYTE *)&szData[HEAD_LEN], nMsgLen - HEAD_LEN);
				break;

			case MSG_SET_AUDIOPARAM:
				printf( "DSP1 Set AudioParam \n");
				break;

			case MSG_GET_VIDEOPARAM:
				GetVideoParam(sSocket, PORT_ONE, (BYTE *)&szData[HEAD_LEN], nMsgLen - HEAD_LEN);

				printf( "DSP1 Get VideoParam \n");
				break;

			case MSG_SET_VIDEOPARAM:
				//SetVideoParam(PORT_ONE, (BYTE *)&szData[HEAD_LEN], nMsgLen - HEAD_LEN);
				printf( "DSP1 Set VideoParam \n");
				break;

			case MSG_SETVGAADJUST:
				printf( "DSP1 ReviseVGAPic vga picture!\n");
				//			ReviseVGAPic(DSP1,(unsigned char *)&szData[3],nMsgLen-3);
				break;

			case MSG_GSCREEN_CHECK:
				//			GreenScreenAjust();
				break;

			case MSG_SET_DEFPARAM:
				break;

			case MSG_ADD_TEXT:	//???
				printf( "enter into MSG_ADD_TEXT \n");
				break;

			case MSG_FARCTRL:
//				printf( "MSG_FARCTRL:sdi[]", ClientType[nPos]/2);
				//FarCtrlCamera(ClientType[nPos]/2, (unsigned char *)&szData[HEAD_LEN], nMsgLen - HEAD_LEN);
				break;

			case MSG_SET_SYSTIME:
				printf( "DSP1 Set system Time\n");
				break;

			case MSG_PASSWORD: { //???????
				printf( "MSG_PASSWORD:[%d]\n", phead->nMsg);

				if(!(strcmp("sawyer", szData + HEAD_LEN))) {
					SETLOGINTYPE(PORT_ONE, nPos, LOGIN_ADMIN);

				} else if((szData[HEAD_LEN] == 'A') && (!strcmp("reach123", szData + HEAD_LEN + 1)  || !strcmp("123", szData + HEAD_LEN + 1))) {
					SETLOGINTYPE(PORT_ONE, nPos, LOGIN_ADMIN);
					printf(  "logo Admin [%s]!\n", szData + HEAD_LEN + 1);


					if(!strcmp("123", szData + HEAD_LEN + 1)) {


					} else {

					}
				} else if((szData[HEAD_LEN] == 'U') && ((!strcmp("reach123", szData + HEAD_LEN + 1)) || (!strcmp("123", szData + HEAD_LEN + 1)))) {

					SETLOGINTYPE(PORT_ONE, nPos, LOGIN_USER);
					ClientType[nPos] = 1;
					printf(  "logo User!\n");
				} else if((szData[HEAD_LEN] == 'A') && Pwd2ClientTypePro(szData + HEAD_LEN + 1, nPos)) {

					printf("MSG_PASSWORD:[%s]\n", szData + HEAD_LEN + 1);

				}
				else {
					PackHeaderMSG((BYTE *)szData, MSG_PASSWORD_ERR, HEAD_LEN);
					send(sSocket, szData, HEAD_LEN, 0);
					printf( "MSG_PASSWORD [%s] error!\n", szData + HEAD_LEN + 1);
					SETLOGINTYPE(PORT_ONE, nPos, LOGIN_ADMIN);

					ClientType[nPos] = -1;
					goto ExitClientMsg;   //
				}

				PackHeaderMSG((BYTE *)szData, MSG_CONNECTSUCC, HEAD_LEN);
				send(sSocket, szData, HEAD_LEN, 0);
				printf( "send MSG_CONNECTSUCC\n");


				PackHeaderMSG((BYTE *)szData, MSG_SYSPARAMS, HEAD_LEN+sizeof(gSysParsm));
				memcpy(szData + HEAD_LEN, &gSysParsm, sizeof(gSysParsm));
				send(sSocket, szData, HEAD_LEN + sizeof(gSysParsm), 0);
			

				printf( "user log!  DSP:%d  client:%d \n", DSP1, nPos);
				SETCLIUSED(DSP1, nPos, TRUE);
				SETCLILOGIN(DSP1, nPos, TRUE);
				printf( "DSP:%d	 socket:%d\n", DSP1, GETSOCK(DSP1, nPos));
				printf( "ISUSED=%d, ISLOGIN=%d\n", ISUSED(DSP1, nPos), ISLOGIN(DSP1, nPos));
				break;
			}

			case MSG_SYSPARAMS: { //?????????
#if 0
				length = 3 + sizeof(gSysParsm);
				PackHeaderMSG((BYTE *)szData, MSG_SYSPARAMS, length);
				memcpy(szData + 3, &gSysParsm, sizeof(gSysParsm));
				send(sSocket, szData, length, 0);
#endif
				printf( "Get Sys Params ..........................\n");
			}
			break;

			case MSG_SETPARAMS: {	//??????????

				//printf( "Set Params! gSysParams %d Bytes\n", sizeof(SysParamsV2));
			}
			break;

			case MSG_SAVEPARAMS:		//?????????flash
				printf( "Save Params!\n");
				break;

			case MSG_RESTARTSYS:
				printf( "Restart sys\n");
				system("reboot -f");
				break;

			case MSG_UpdataFile: { //????????
			}
			break;

			case MSG_UPDATEFILE_ROOT:
				printf( "Updata Root file\n");
				break;

			case MSG_REQ_I:
				printf( "Request I Frame!\n");
				break;

			case MSG_PIC_DATA:
				printf( "SetPPTIndex\n");
				break;

			case MSG_MUTE:
				printf( "NoSound!\n");
				break;

			case MSG_LOCK_SCREEN:
				break;

			case MSG_LOW_BITRATE:
				break;

			case MSG_FIX_RESOLUTION:
				break;

			case MSG_SEND_INPUT:
				break;

			default:
				printf( "Cmd=%d!\n", phead->nMsg);
				break;
		}

	}


ExitClientMsg:
	printf( " ExitClientMsg socket = %d %d\n", GETSOCK(DSP1, nPos), sSocket);

	SETCLIUSED(DSP1, nPos, FALSE);
	SETSOCK(DSP1, nPos, INVALID_SOCKET);
	SETLOGINTYPE(DSP1, nPos, LOGIN_USER);
	SETCLILOGIN(DSP1, nPos, FALSE);

	close(sSocket);
	shutdown(sSocket, SHUT_RDWR);

	ClientType[nPos] = -1;

	if(szData != NULL) {
		free(szData);
		szData = NULL;
	}

	pthread_detach(pthread_self());
	pthread_exit(0);
	return NULL;
}

static void *EncoderProcess110(void *pParams)
{
	int					sSocket = -1;
	//WORD					length;

	//char			szPass[] = "reach123";
	char *szData = NULL;

	int 					nPos					= 0;
	int 					nMsgLen 				= 0;
	int 					*pnPos					= (int *)pParams;
	int 					nLen					= 0;
	MSGHEAD header, *phead;

	nPos = *pnPos;
	free(pParams);
	sSocket = GETSOCK(DSP2, nPos);
	//SysParamsV2 sys2;
//	SysParamsV2 sys2;
	/*????TCP?????? 3s*/
	SetSendTimeOut(sSocket, 3);

	sem_post(&g_sem);

	phead = &header;
	memset(&header, 0, sizeof(MSGHEAD));
	szData = (char *)malloc(MAX_EM_MSG_LEN + 1);

	if(NULL == szData) {
		printf( "NULL == szData\n");
		return NULL;
	}

	memset(szData, 0, MAX_EM_MSG_LEN + 1);

	while(gRunStatus) {
			memset(szData, 0, sizeof(szData));

		if(sSocket <= 0) {
			printf("sSocket<0 !\n");
			goto ExitClientMsg;
		}

		nLen = recv(sSocket, szData, 2, 0);
		printf("receive length:%d,errno=%d\n", nLen, errno);

		if(nLen < 2 || nLen == -1) {
			printf( "recv nLen < 2 || nLen == -1  goto ExitClientMsg\n ");
			goto ExitClientMsg;
		}

		nMsgLen = ntohs(*((short *)szData));
		nLen = recv_long_tcp_data(sSocket, szData + 2, nMsgLen - 2);
		printf("recv nLen = %d\n", nLen);
		printf("nMsgLen = %d,szData[2]=%d!\n", nMsgLen, szData[2]);
		printf("szData[3],szData[4]=%d,%d,%d,%d!\n", szData[3], szData[4], szData[5], szData[6]);
		printf("szData[7],szData[8}=%d,%d,%d,%d!\n", szData[7], szData[8], szData[9], szData[10]);

		if(nLen < nMsgLen - 2) {
			printf("nLen < nMsgLen -2  goto ExitClientMsg\n ");
			goto ExitClientMsg;
		}
		switch(szData[2]) {
			case MSG_REQ_AUDIO:
				printf( "DSP1 request Audio Data \n");
				break;

			case MSG_GET_AUDIOPARAM:
				printf( "DSP1 Get AudioParam \n");

			//	GetAudioParam(sSocket, PORT_ONE, (BYTE *)&szData[HEAD110_LEN], nMsgLen - HEAD110_LEN);
				break;

			case MSG_SET_AUDIOPARAM:
				printf( "DSP1 Set AudioParam \n");
				break;

			case MSG_GET_VIDEOPARAM:
				GetVideoParam(sSocket, DSP2, (BYTE *)&szData[HEAD110_LEN], nMsgLen - HEAD110_LEN);

				printf( "DSP1 Get VideoParam \n");
				break;

			case MSG_SET_VIDEOPARAM:
				//SetVideoParam(PORT_ONE, (BYTE *)&szData[HEAD_LEN], nMsgLen - HEAD_LEN);
				printf( "DSP1 Set VideoParam \n");
				break;

			case MSG_SETVGAADJUST:
				printf( "DSP1 ReviseVGAPic vga picture!\n");
				//			ReviseVGAPic(DSP1,(unsigned char *)&szData[3],nMsgLen-3);
				break;

			case MSG_GSCREEN_CHECK:
				//			GreenScreenAjust();
				break;

			case MSG_SET_DEFPARAM:
				break;

			case MSG_ADD_TEXT:	//???
				printf( "enter into MSG_ADD_TEXT \n");
				break;

			case MSG_FARCTRL:
//				printf( "MSG_FARCTRL:sdi[]", ClientType[nPos]/2);
				//FarCtrlCamera(ClientType[nPos]/2, (unsigned char *)&szData[HEAD_LEN], nMsgLen - HEAD_LEN);
				break;

			case MSG_SET_SYSTIME:
				printf( "DSP1 Set system Time\n");
				break;

			case MSG_PASSWORD: { //???????
				printf( "MSG_PASSWORD:[%d]\n", phead->nMsg);

				if(!(strcmp("sawyer", szData + HEAD110_LEN))) {
					SETLOGINTYPE(DSP2, nPos, LOGIN_ADMIN);

				} else if((szData[HEAD110_LEN] == 'A') && (!strcmp("reach123", szData + HEAD110_LEN + 1)  || !strcmp("123", szData + HEAD110_LEN + 1))) {
					SETLOGINTYPE(DSP2, nPos, LOGIN_ADMIN);
					printf(  "logo Admin [%s]!\n", szData + HEAD110_LEN + 1);


					if(!strcmp("123", szData + HEAD110_LEN + 1)) {


					} else {

					}
				} else if((szData[HEAD110_LEN] == 'U') && ((!strcmp("reach123", szData + HEAD110_LEN + 1)) || (!strcmp("123", szData + HEAD110_LEN + 1)))) {

					SETLOGINTYPE(DSP2, nPos, LOGIN_USER);
					printf(  "logo User!\n");
				} 
				else {
					PackHeaderMSG110((BYTE *)szData, MSG_PASSWORD_ERR, HEAD110_LEN);
					send(sSocket, szData, HEAD110_LEN, 0);
					printf( "MSG_PASSWORD [%s] error!\n", szData + HEAD110_LEN + 1);
					SETLOGINTYPE(DSP2, nPos, LOGIN_ADMIN);

					//ClientType[nPos] = -1;
					goto ExitClientMsg;   //
				}


				RecDataInfo theDataInfo = {{0}, 0, 0, 0, 0, 0, 0, 0, {0}};
				int len = 0;
				szData[0] = 0;
				szData[1] = HEAD110_LEN;
				szData[2] = MSG_CONNECTSUCC;
				len = send(sSocket, szData, HEAD110_LEN, 0);
				if(len != 3)
				{
					printf("send error \n");
				}

				#if 1
				printf("send MSG_CONNECTSUCC");
				theDataInfo.nColors = 16;
				theDataInfo.nDataCodec = 0x1001;//JPEG?a 0x1001
				theDataInfo.nHight = 1920;
				theDataInfo.nWidth = 1088;
				theDataInfo.nFrameRate = 30;
				strcpy(theDataInfo.Version, "VGABOX");
				memcpy(szData + HEAD110_LEN, &theDataInfo, sizeof(RecDataInfo));
				szData[0] = 0;
				szData[1] = (char)(HEAD110_LEN + sizeof(theDataInfo));
				szData[2] = MSG_DATAINFO;
				len = send(sSocket, szData, HEAD110_LEN + sizeof(theDataInfo), 0);
				if(len != (HEAD110_LEN + sizeof(theDataInfo)))
				{
					printf("send error \n");
				}
				#endif

				szData[0] = 0;
				szData[1] = (char)(HEAD110_LEN + sizeof(SysParamsV2));
				szData[2] = MSG_SYSPARAMS;

				SysParamsV2 sys2 = {{0}, 0};
				
			
				memcpy(sys2.szMacAddr, gSysParsm.szMacAddr, 8);
				sys2.dwAddr = gSysParsm.dwAddr;
				sys2.dwGateWay = gSysParsm.dwGateWay;
				sys2.dwNetMark = gSysParsm.dwNetMark;
				strcpy(sys2.strName, gSysParsm.strName);
				sys2.nFrame = gvideoPara.nFrameRate;
				sys2.nQuality = gvideoPara.nQuality;
				sys2.sBitrate = gvideoPara.sBitrate;
				sys2.nColors = gvideoPara.nColors;
				sys2.sCbr = gvideoPara.sCbr;
				strcpy(sys2.strVer, gSysParsm.strVer);
				memcpy(&szData[HEAD110_LEN], &sys2, sizeof(SysParamsV2));
				
				len = send(sSocket, szData, HEAD110_LEN + sizeof(SysParamsV2), 0);
				if(len != (HEAD110_LEN + sizeof(SysParamsV2)))
				{
					printf("send error \n");
				}
			

				printf( "user log!  DSP:%d  client:%d \n", DSP2, nPos);
				SETCLIUSED(DSP2, nPos, TRUE);
				SETCLILOGIN(DSP2, nPos, TRUE);
				printf( "DSP:%d	 socket:%d\n", DSP2, GETSOCK(DSP2, nPos));
				printf( "ISUSED=%d, ISLOGIN=%d\n", ISUSED(DSP2, nPos), ISLOGIN(DSP2, nPos));
				break;
			}

			case MSG_SYSPARAMS: { //?????????
#if 0
				length = 3 + sizeof(gSysParsm);
				PackHeaderMSG((BYTE *)szData, MSG_SYSPARAMS, length);
				memcpy(szData + 3, &gSysParsm, sizeof(gSysParsm));
				send(sSocket, szData, length, 0);
#endif
				printf( "Get Sys Params ..........................\n");
			}
			break;

			case MSG_SETPARAMS: {	//??????????

				//printf( "Set Params! gSysParams %d Bytes\n", sizeof(SysParamsV2));
			}
			break;

			case MSG_SAVEPARAMS:		//?????????flash
				printf( "Save Params!\n");
				break;

			case MSG_RESTARTSYS:
				printf( "Restart sys\n");
				system("reboot -f");
				break;

			case MSG_UpdataFile: { //????????
			}
			break;

			case MSG_UPDATEFILE_ROOT:
				printf( "Updata Root file\n");
				break;

			case MSG_REQ_I:
				printf( "Request I Frame!\n");
				break;

			case MSG_PIC_DATA:
				printf( "SetPPTIndex\n");
				break;

			case MSG_MUTE:
				printf( "NoSound!\n");
				break;

			case MSG_LOCK_SCREEN:
				break;

			case MSG_LOW_BITRATE:
				break;

			case MSG_FIX_RESOLUTION:
				break;

			case MSG_SEND_INPUT:
				break;

			default:
				printf( "Cmd=%d!\n", phead->nMsg);
				break;
		}

	}


ExitClientMsg:
	printf( " ExitClientMsg socket = %d %d\n", GETSOCK(DSP2, nPos), sSocket);

	SETCLIUSED(DSP2, nPos, FALSE);
	SETSOCK(DSP2, nPos, INVALID_SOCKET);
	SETLOGINTYPE(DSP2, nPos, LOGIN_USER);
	SETCLILOGIN(DSP2, nPos, FALSE);

	close(sSocket);
	shutdown(sSocket, SHUT_RDWR);

	if(szData != NULL) {
		free(szData);
		szData = NULL;
	}

	pthread_detach(pthread_self());
	pthread_exit(0);
	return NULL;
}



static void* EncoderServerThread(void *arg)
{
	struct sockaddr_in		SrvAddr, ClientAddr;
	int					sClientSocket = -1;
	int					ServSock = -1;

	pthread_t				client_threadid[MAX_CLIENT] = {0};

	short					sPort					= PORT_LISTEN_DSP1;
	void					*ret 					= 0;

	int 					clientsocket				= 0;
	int 					nLen					= 0;;
	int 					ipos						= 0;
	int 					fileflags					= 0;
	int 					opt 					= 1;

	//prctl(PR_SET_NAME, "EncoderServerThread", NULL, NULL, NULL);

	InitClient(DSP1);
	sem_init(&g_sem, 0, 0);

SERVERSTARTRUN:

	bzero(&SrvAddr, sizeof(struct sockaddr_in));
	SrvAddr.sin_family = AF_INET;
	SrvAddr.sin_port = htons(sPort);
	SrvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	ServSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(ServSock < 0) {
		printf( "ListenTask create error:%d,error msg: = %s\n", errno, strerror(errno));
		gRunStatus = 0;
		pthread_detach(pthread_self());
		pthread_exit(0);
		return NULL;
	}

	setsockopt(ServSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if(bind(ServSock, (struct sockaddr *)&SrvAddr, sizeof(SrvAddr)) < 0) {
		printf(  "bind terror:%d,error msg: = %s\n", errno, strerror(errno));
		gRunStatus = 0;
		pthread_detach(pthread_self());
		pthread_exit(0);
		return NULL;
	}

	if(listen(ServSock, 10) < 0) {
		printf( "listen error:%d,error msg: = %s", errno, strerror(errno));
		gRunStatus = 0;
		pthread_detach(pthread_self());
		pthread_exit(0);
		return NULL;
	}

	if((fileflags = fcntl(ServSock, F_GETFL, 0)) == -1) {
		printf(  "fcntl F_GETFL error:%d,error msg: = %s\n", errno, strerror(errno));
		gRunStatus = 0;
		pthread_detach(pthread_self());
		pthread_exit(0);
		return NULL;
	}

	memset(&ClientType[0], -1, sizeof(ClientType[0])*ENC_MAX_NUM);
	nLen = sizeof(struct sockaddr_in);

	while(gRunStatus) {
		memset(&ClientAddr, 0, sizeof(struct sockaddr_in));

		nLen = sizeof(struct sockaddr_in);
		sClientSocket = accept(ServSock, (struct sockaddr *)&ClientAddr, (DWORD *)&nLen);

		printf( "naccept a valid connect!!!!-----------------, socket = %d\n\n\n\n", sClientSocket);

		if((sClientSocket > 0) /*&& (gEduKit->Start == 1)*/) {

			int nPos = 0;
			ClearLostClient(DSP1);
			nPos = GetNullClientData(DSP1);

			if(-1 == nPos) {
				char chData[20];
				chData[0] = 0;
				chData[1] = 3;
				chData[2] = MSG_MAXCLIENT_ERR;
				send(sClientSocket, chData, 3, 0);
				close(sClientSocket);
				printf(  "MAXCLIENT ERR!\n");
			} else {
				int nSize = 0;
				int result;
				int *pnPos = (int *)malloc(sizeof(int));

				/* set client used */
				SETCLIUSED(DSP1, nPos, TRUE);
				SETSOCK(DSP1, nPos, sClientSocket) ;

				nSize = 1;

				if((setsockopt(sClientSocket, SOL_SOCKET, SO_REUSEADDR, (void *)&nSize,
				               sizeof(nSize))) == -1) {
					printf(  "setsockopt failed");
				}

				nSize = 0;
				nLen = sizeof(nLen);
				result = getsockopt(sClientSocket, SOL_SOCKET, SO_SNDBUF, &nSize , (DWORD *)&nLen);

				if(result) {
					printf(  "getsockopt() errno:%d socket:%d  result:%d\n", errno, sClientSocket, result);
				}

				nSize = 1;

				if(setsockopt(sClientSocket, IPPROTO_TCP, TCP_NODELAY, &nSize , sizeof(nSize))) {
					printf( "Setsockopt error%d\n", errno);
				}

				printf(  "Clent:%s connected,nPos:%d socket:%d!\n", inet_ntoa(ClientAddr.sin_addr), nPos, sClientSocket);
				*pnPos = nPos;

//#define ENC110

				
				result = pthread_create(&client_threadid[nPos], NULL, EncoderProcess, (void *)pnPos);

				if(result) {
					close(sClientSocket);	//
					SETCLIUSED(DSP1, nPos, FALSE);
					SETSOCK(DSP1, nPos, INVALID_SOCKET);
					SETLOGINTYPE(DSP1, nPos, LOGIN_USER);
					SETCLILOGIN(DSP1, nPos, FALSE);
			

					printf(  "creat pthread ClientMsg error  = %d!\n" , errno);
					continue;
				}

				sem_wait(&g_sem);
				printf( "sem_wait() semphone inval!!!  result = %d\n", result);
			}
		} else {

			if(sClientSocket > 0) {
				close(sClientSocket);
				printf( "close sClientSocket socket!!! %d\n", sClientSocket);
				sClientSocket = -1;
			}

			if(errno == ECONNABORTED || errno == EAGAIN)
				//Èí¼þÔ­ÒòÖÐ¶Ï
			{
				usleep(100000);
				continue;
			}

			if(ServSock > 0) {
				printf( "close enclive socket!!! %d\n", ServSock);
				close(ServSock);
				ServSock = -1;
				sleep(1);
			}

			goto SERVERSTARTRUN;
		}

	}

	printf( "begin exit the DSP1TCPTask \n");

	printf(  "exit the drawtimethread \n");

	for(ipos = 0; ipos < MAX_CLIENT; ipos++) {
		if(client_threadid[ipos]) {
			clientsocket = GETSOCK(DSP1, ipos);

			if(clientsocket != INVALID_SOCKET) {
				close(clientsocket);
				SETSOCK(DSP1, ipos, INVALID_SOCKET);
			}

			if(pthread_join(client_threadid[ipos], &ret) == 0) {
			}
		}
	}

	printf(  "close the encoder server socket \n");

	if(ServSock > 0) {
		printf(  "close gserv socket \n");
		close(ServSock);
	}

	ServSock = 0;
	printf( "End.\n");
	pthread_detach(pthread_self());
	pthread_exit(0);
	return NULL;
}



static void* EncoderServerThread110(void *arg)
{
	struct sockaddr_in		SrvAddr, ClientAddr;
	int					sClientSocket = -1;
	int					ServSock = -1;

	pthread_t				client_threadid[MAX_CLIENT] = {0};

	short					sPort					= PORT_LISTEN_DSP2;
	void					*ret 					= 0;

	int 					clientsocket				= 0;
	int 					nLen					= 0;;
	int 					ipos						= 0;
	int 					fileflags					= 0;
	int 					opt 					= 1;

	//prctl(PR_SET_NAME, "EncoderServerThread", NULL, NULL, NULL);

	InitClient(DSP2);
	sem_init(&g_sem, 0, 0);

SERVERSTARTRUN:

	bzero(&SrvAddr, sizeof(struct sockaddr_in));
	SrvAddr.sin_family = AF_INET;
	SrvAddr.sin_port = htons(sPort);
	SrvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	ServSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(ServSock < 0) {
		printf( "ListenTask create error:%d,error msg: = %s\n", errno, strerror(errno));
		gRunStatus = 0;
		pthread_detach(pthread_self());
		pthread_exit(0);
		return NULL;
	}

	setsockopt(ServSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if(bind(ServSock, (struct sockaddr *)&SrvAddr, sizeof(SrvAddr)) < 0) {
		printf(  "bind terror:%d,error msg: = %s\n", errno, strerror(errno));
		gRunStatus = 0;
		pthread_detach(pthread_self());
		pthread_exit(0);
		return NULL;
	}

	if(listen(ServSock, 10) < 0) {
		printf( "listen error:%d,error msg: = %s", errno, strerror(errno));
		gRunStatus = 0;
		pthread_detach(pthread_self());
		pthread_exit(0);
		return NULL;
	}

	if((fileflags = fcntl(ServSock, F_GETFL, 0)) == -1) {
		printf(  "fcntl F_GETFL error:%d,error msg: = %s\n", errno, strerror(errno));
		gRunStatus = 0;
		pthread_detach(pthread_self());
		pthread_exit(0);
		return NULL;
	}

	//memset(&ClientType[0], -1, sizeof(ClientType[0])*ENC_MAX_NUM);
	nLen = sizeof(struct sockaddr_in);

	while(gRunStatus) {
		memset(&ClientAddr, 0, sizeof(struct sockaddr_in));

		nLen = sizeof(struct sockaddr_in);
		sClientSocket = accept(ServSock, (struct sockaddr *)&ClientAddr, (DWORD *)&nLen);

		printf( "naccept a valid connect!!!!-----------------, socket = %d\n\n\n\n", sClientSocket);

		if((sClientSocket > 0) /*&& (gEduKit->Start == 1)*/) {

			int nPos = 0;
			ClearLostClient(DSP2);
			nPos = GetNullClientData(DSP2);

			if(-1 == nPos) {
				char chData[20];
				chData[0] = 0;
				chData[1] = 3;
				chData[2] = MSG_MAXCLIENT_ERR;
				send(sClientSocket, chData, 3, 0);
				close(sClientSocket);
				printf(  "MAXCLIENT ERR!\n");
			} else {
				int nSize = 0;
				int result;
				int *pnPos = (int *)malloc(sizeof(int));

				/* set client used */
				SETCLIUSED(DSP2, nPos, TRUE);
				SETSOCK(DSP2, nPos, sClientSocket) ;

				nSize = 1;

				if((setsockopt(sClientSocket, SOL_SOCKET, SO_REUSEADDR, (void *)&nSize,
				               sizeof(nSize))) == -1) {
					printf(  "setsockopt failed");
				}

				nSize = 0;
				nLen = sizeof(nLen);
				result = getsockopt(sClientSocket, SOL_SOCKET, SO_SNDBUF, &nSize , (DWORD *)&nLen);

				if(result) {
					printf(  "getsockopt() errno:%d socket:%d  result:%d\n", errno, sClientSocket, result);
				}

				nSize = 1;

				if(setsockopt(sClientSocket, IPPROTO_TCP, TCP_NODELAY, &nSize , sizeof(nSize))) {
					printf( "Setsockopt error%d\n", errno);
				}

				printf(  "Clent:%s connected,nPos:%d socket:%d!\n", inet_ntoa(ClientAddr.sin_addr), nPos, sClientSocket);
				*pnPos = nPos;


				printf("r_pthread_create------------------[%s:%s:%d]\n ", __FILE__, __FUNCTION__, __LINE__);
				
				result = pthread_create(&client_threadid[nPos], NULL, EncoderProcess110, (void *)pnPos);


				if(result) {
					close(sClientSocket);	//
					SETCLIUSED(DSP2, nPos, FALSE);
					SETSOCK(DSP2, nPos, INVALID_SOCKET);
					SETLOGINTYPE(DSP2, nPos, LOGIN_USER);
					SETCLILOGIN(DSP2, nPos, FALSE);
			

					printf(  "creat pthread ClientMsg error  = %d!\n" , errno);
					continue;
				}

				sem_wait(&g_sem);
				printf( "sem_wait() semphone inval!!!  result = %d\n", result);
			}
		} else {

			if(sClientSocket > 0) {
				close(sClientSocket);
				printf( "close sClientSocket socket!!! %d\n", sClientSocket);
				sClientSocket = -1;
			}

			if(errno == ECONNABORTED || errno == EAGAIN)
				//Èí¼þÔ­ÒòÖÐ¶Ï
			{
				usleep(100000);
				continue;
			}

			if(ServSock > 0) {
				printf( "close enclive socket!!! %d\n", ServSock);
				close(ServSock);
				ServSock = -1;
				sleep(1);
			}

			goto SERVERSTARTRUN;
		}

	}

	printf( "begin exit the DSP1TCPTask \n");

	printf(  "exit the drawtimethread \n");

	for(ipos = 0; ipos < MAX_CLIENT; ipos++) {
		if(client_threadid[ipos]) {
			clientsocket = GETSOCK(DSP2, ipos);

			if(clientsocket != INVALID_SOCKET) {
				close(clientsocket);
				SETSOCK(DSP2, ipos, INVALID_SOCKET);
			}

			if(pthread_join(client_threadid[ipos], &ret) == 0) {
			}
		}
	}

	printf(  "close the encoder server socket \n");

	if(ServSock > 0) {
		printf(  "close gserv socket \n");
		close(ServSock);
	}

	ServSock = 0;
	printf( "End.\n");
	pthread_detach(pthread_self());
	pthread_exit(0);
	return NULL;
}


int StartEncoderMangerServer()
{
	int result;
	pthread_t	serverThid;
	//int32_t *index = (int32_t *)malloc(sizeof(int32_t));
	//*index = eindex;
	pthread_mutex_init(&g_send_m, NULL);
	pthread_mutex_init(&g_send110_m, NULL);

	InitStandardParam(&gSysParsm);
	InitAudioParam(&gAudioParam);
	InitVideoParam(&gvideoPara);
	printf("r_pthread_create------------------[%s:%s:%d] \n", __FILE__, __FUNCTION__, __LINE__);

#if 1
	result = pthread_create(&serverThid, NULL, EncoderServerThread, (void *)NULL);

	if(result < 0) {
		printf("create EncoderServerThread() failed\n");
		return -1;
	}
#endif
	result = pthread_create(&serverThid, NULL, EncoderServerThread110, (void *)NULL);

	if(result < 0) {
		printf("create EncoderServerThread() failed\n");
		return -1;
	}

	return 0;
}


/*############################################################################*/

					   

/******************************************************************************
* funciton : get file postfix according palyload_type.
******************************************************************************/
HI_S32 RH_MPI_VENC_GetFilePostfix(PAYLOAD_TYPE_E enPayload, char* szFilePostfix)
{
    if (PT_H264 == enPayload)
    {
//        strcpy(szFilePostfix, ".h264");
					strcpy(szFilePostfix, ".264");
    }
    else if (PT_H265 == enPayload)
    {
//        strcpy(szFilePostfix, ".h265");
        strcpy(szFilePostfix, ".265");
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



/*Send H264 Data EncodeManager*/
HI_U32 RH_MPI_VENC_SendData( VENC_STREAM_S* pstStream)
{
	HI_U32 s32Ret = HI_SUCCESS;
	HI_U32 i;

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
    	SendDataToClientEx(pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 
			pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,1, 0,1920, 1080);
    }
	
	return s32Ret;
}

int RtspStopLiveStream(int chid)
{
	if(chid < 0 || chid > 3)
	{
		return -1;
	}
	
	RtspLiveServerDelteSeesion(rtspSend[chid]);
	rtspSend[chid] = NULL;

	return 0;
}

int RtspStartLiveStream(int chid)
{	
	if(chid < 0 || chid > 3)
	{
		return -1;
	}

	char name[50]; 
	sprintf(name, "live%d",chid);
	RtspLiveServerCreateSeesion(&rtspSend[chid], name,          1 , 0);

	return 0;
}






int RtmpInit(RtmpHandle *handle)
{
	if(!handle)
	{
		return -1;
	}
	
	memset(handle, 0x0, sizeof(RtmpHandle));
	pthread_mutex_init(&handle->lock, NULL);

	av_register_all();
	avformat_network_init();
	av_log_set_level(AV_LOG_VERBOSE);
	avformat_alloc_output_context2(&handle->oc, NULL, "flv", NULL);
	return 0;
}



int RtmpAddAudio(RtmpHandle *handle, int channels, int samplerate)
{
	AVOutputFormat *fmt;
	AVFormatContext *oc;
	int ret;
	RtmpHandle *phandle = (RtmpHandle *)handle;

	if(!handle|| !handle->oc)
	{
		return -1;
	}
	pthread_mutex_lock(&phandle->lock);	

	oc = handle->oc;
	fmt = oc->oformat;
	fmt->audio_codec = AV_CODEC_ID_AAC;
	
	if (fmt->audio_codec != AV_CODEC_ID_NONE) 
	{
		AVStream *st = avformat_new_stream(oc, NULL);
		if (!st) 
		{
		    fprintf(stderr, "Could not allocate stream\n");
			pthread_mutex_unlock(&phandle->lock);
			return -1;
		}
		st->id = oc->nb_streams-1;
		AVCodecParameters *par = st->codecpar;
		par->codec_type = AVMEDIA_TYPE_AUDIO;
		par->codec_id	= fmt->audio_codec;

		par->format			= AV_SAMPLE_FMT_S16;
		par->channel_layout	= AV_CH_LAYOUT_STEREO;
		par->channels 		= channels;
		par->sample_rate		= samplerate;
	 	st->time_base = (AVRational){ 1, samplerate};
		handle->AudioStreamId =    st->id;
		handle->enableAudio = 1;
	}

	
	pthread_mutex_unlock(&phandle->lock);
	
	return 0;
}


int RtmpAddVideo(RtmpHandle *handle, int width, int height, int bitrate)
{
	AVOutputFormat *fmt;
	AVFormatContext *oc;
	int ret;
	RtmpHandle *phandle = (RtmpHandle *)handle;

	if(!handle|| !handle->oc)
	{
		return -1;
	}

	pthread_mutex_lock(&phandle->lock);

	oc = handle->oc;
	fmt = oc->oformat;
	fmt->video_codec = AV_CODEC_ID_H264;
	

	if (fmt->video_codec != AV_CODEC_ID_NONE) 
	{
		AVStream *st = avformat_new_stream(oc, NULL);
		if (!st) 
		{
			fprintf(stderr, "Could not allocate stream\n");
			return -1;
		}
		st->id = oc->nb_streams-1;
		AVCodecParameters *par = st->codecpar;
		par->codec_type = AVMEDIA_TYPE_VIDEO;
		par->codec_id 		   = fmt->video_codec;
		par->bit_rate 		   = bitrate;
		par->format			   = AV_PIX_FMT_YUV420P;
		par->width			   = width;
		par->height			   = height;
		st->time_base = (AVRational){ 1, 1000};
		handle->VideoStreamId =    st->id;
		handle->enableVideo = 1;
	}
	
	pthread_mutex_unlock(&phandle->lock);

	return 0;
}

int RtmpOpenUrl(RtmpHandle *handle, char *url)
{
	AVFormatContext *oc;
	AVOutputFormat *fmt;
	int ret;
	RtmpHandle *phandle = (RtmpHandle *)handle;

	if(!handle|| !handle->oc)
	{
		return -1;
	}

	pthread_mutex_lock(&phandle->lock);

	oc = handle->oc;
	fmt = oc->oformat;


	if (!(fmt->flags & AVFMT_NOFILE) && (NULL == oc->pb)) 
	{
		ret = avio_open(&oc->pb, url, AVIO_FLAG_WRITE);
		if (ret < 0) 
		{
		    fprintf(stderr, "Could not open '%s': %s\n", url,
		            av_err2str(ret));
			pthread_mutex_unlock(&phandle->lock);
		    return -1;
		}
		else
		{
			  fprintf(stderr, " open '%s': %s\n", url,
		            av_err2str(ret));
		}
		
		ret = avformat_write_header(oc, NULL);
		if (ret < 0) 
		{
			fprintf(stderr, "Error occurred when opening output file: %s\n",
			av_err2str(ret));
			pthread_mutex_unlock(&phandle->lock);
			return -1;
		}
	}

	pthread_mutex_unlock(&phandle->lock);

	return 0;
}


int RtmpCloseUrl(RtmpHandle *handle)
{
	AVFormatContext *oc;
	AVOutputFormat *fmt;
	int ret;
	RtmpHandle *phandle = (RtmpHandle *)handle;

	if(!handle|| !handle->oc)
	{
		return -1;
	}

	pthread_mutex_lock(&phandle->lock);

	printf("RtmpCloseUrl\n");
	oc = handle->oc;
	fmt = oc->oformat;

	if (!(fmt->flags & AVFMT_NOFILE) && (NULL != oc->pb))
	{
        avio_closep(&oc->pb);
	}

	phandle->state = 0;
	pthread_mutex_unlock(&phandle->lock);

	return 0;
}


int Rtmpuninit(RtmpHandle *handle)
{
	AVFormatContext *oc;
	AVOutputFormat *fmt;
	int ret;
	RtmpHandle *phandle = (RtmpHandle *)handle;

	if(!handle)
	{
		return -1;
	}

	pthread_mutex_lock(&phandle->lock);
	oc = handle->oc;
	if(oc)	
	{
		if(oc->pb)
		{
			avio_closep(&oc->pb);
		}
		avformat_free_context(oc);
	
	}
	handle->enableVideo = 0;
	handle->enableAudio = 0;
	pthread_mutex_unlock(&phandle->lock);
	pthread_mutex_destroy(&phandle->lock);
}


//Î´²âÊÔ
int RtmpSendAudio(RtmpHandle *handle, char *ptr, int len)
{
	if(!handle)
	{
		return -1;
	}

	pthread_mutex_lock(&handle->lock);

	if((!handle->enableAudio)     || (NULL == handle->oc)  || (NULL == handle->oc->pb))
	{
		pthread_mutex_unlock(&handle->lock);
		return 0;
	}
	
	AVPacket pkt;
	av_init_packet(&pkt);

	if(!handle->sTime)
	{
		handle->sTime = getCurrentTime2();
	}
	
	pkt.data = ptr;
	pkt.size = len;
	pkt.stream_index = handle->AudioStreamId;
	pkt.pts = pkt.dts    = getCurrentTime2() - handle->sTime;
	pkt.flags  = 1;

	
	av_interleaved_write_frame(handle->oc, &pkt);
	pthread_mutex_unlock(&handle->lock);

	return 0;
}

int RtmpSendVido(RtmpHandle *handle, char *ptr, int len,int isIframe)
{
	if(!handle)
	{
		return -1;
	}

	pthread_mutex_lock(&handle->lock);
	if((!handle->enableVideo)     || (NULL == handle->oc)  || (NULL == handle->oc->pb))
	{
		pthread_mutex_unlock(&handle->lock);
		return 0;
	}
	
	AVPacket pkt;
	av_init_packet(&pkt);

	if(!handle->sTime)
	{
		handle->sTime = getCurrentTime2();
	}
	
	pkt.data = ptr;
	pkt.size = len;
	pkt.stream_index = handle->VideoStreamId;
	pkt.pts = pkt.dts    = getCurrentTime2() - handle->sTime;
	pkt.flags  = isIframe;
	
	av_interleaved_write_frame(handle->oc, &pkt);
	pthread_mutex_unlock(&handle->lock);

	return 0;
}

/******************************************************************************
* funciton : get stream from each channels and save them
******************************************************************************/
HI_VOID* RH_MPI_VENC_GetVencStreamProc(HI_VOID* p)
{
    HI_S32 i;
    HI_S32 s32ChnTotal;
    VENC_CHN_ATTR_S stVencChnAttr;
    SAMPLE_VENC_GETSTREAM_PARA_S* pstPara;
    HI_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    HI_S32 VencFd[VENC_MAX_CHN_NUM];
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret;
    VENC_CHN VencChn;
	HI_S8 *pdata = NULL;

    pstPara = (SAMPLE_VENC_GETSTREAM_PARA_S*)p;
    s32ChnTotal = pstPara->s32Cnt;


	RtspLiveServerInit(554, "guest", "guest");

	unsigned long RtmpStime = 0;


	int StreamNum = 3;


	for(i = 0; i < StreamNum; i ++)
	{
		RtmpInit(&handle[i]);
		RtmpAddVideo(&handle[i], 1920, 1080, 2000000);
		char url[256] = {0};
		sprintf(url,"rtmp://192.168.4.83/live/%d",i);		
		RtmpOpenUrl(&handle[i], url);
		RtspStartLiveStream(i);
	}


	RtspLiveServerStart();

	


    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (s32ChnTotal >= VENC_MAX_CHN_NUM)
    {
        SAMPLE_PRT("input count invaild\n");
        return NULL;
    }

	/*ºÏ²¢Á÷*/
	pdata = (HI_S8 *)malloc(1920*1080*2);
	if(NULL == pdata)
	{
		SAMPLE_PRT("malloc stream buff failed!\n");
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

					int j = 0;
					int dataLen = 0;
					int isIframe = 0;


					#if 1
					for (j = 0; j < stStream.u32PackCount; j++)
					{
						if((dataLen + (stStream.pstPack[j].u32Len-stStream.pstPack[j].u32Offset)) > 1920*1080*2)
						{
							SAMPLE_PRT("ch[%d] data len is too big!\n",j);
							break;
						}
						else
						{
							memcpy(pdata + dataLen,stStream.pstPack[j].pu8Addr+stStream.pstPack[j].u32Offset,stStream.pstPack[j].u32Len-stStream.pstPack[j].u32Offset);
							dataLen += (stStream.pstPack[j].u32Len-stStream.pstPack[j].u32Offset);
						}

						if((stStream.pstPack[j].DataType.enH264EType == H264E_NALU_ISLICE) || (stStream.pstPack[j].DataType.enH264EType == H264E_NALU_IDRSLICE))
						{
							isIframe = 1;
						}
						
					}
					#endif
                    /*******************************************************
                     step 2.5 : send frame to EncoderManage
                    *******************************************************/
               		if(i < 3)
                    {

						VENC_CHN_ATTR_S chAttr;
						int width,height;
						s32Ret = HI_MPI_VENC_GetChnAttr(i,&chAttr);
						if (HI_SUCCESS != s32Ret)
						{
							printf("HI_MPI_VENC_GetChnAttr failed with %#x!\n", \
							    s32Ret);
							break;
						}
						
						width = chAttr.stVeAttr.stAttrH264e.u32PicWidth;
						height = chAttr.stVeAttr.stAttrH264e.u32PicHeight;
					
						
						//s32Ret = RH_MPI_VENC_SendData(&stStream);
						SendDataToClientEx(dataLen, pdata, isIframe , i, width, height);

						if(i == 2)
						{
							SendDataToClient110(dataLen, pdata, isIframe , i, width, height);
						}

						if(rtspSend[i])
						{
							RtspLiveServerSendVideoFrame(rtspSend[i], pdata,   dataLen);
						}
	

						if(handle[i].enableVideo)
						{
							RtmpSendVido(&handle[i], pdata, dataLen, isIframe);
						}
					
	         
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

    return NULL;
}

/******************************************************************************
* funciton : start get venc stream process thread
******************************************************************************/
HI_S32 RH_MPI_VENC_StartGetStream(HI_S32 s32Cnt)
{
    gs_stPara.bThreadStart = HI_TRUE;
    gs_stPara.s32Cnt = s32Cnt;
	HI_U32 result;

	StartEncoderMangerServer();
	result = pthread_create(&gs_VencPid, 0, RH_MPI_VENC_GetVencStreamProc, (HI_VOID*)&gs_stPara);
    return result;
}

/******************************************************************************
* funciton : stop get venc stream process.
******************************************************************************/
HI_S32 RH_MPI_VENC_StopGetStream()
{
    if (HI_TRUE == gs_stPara.bThreadStart)
    {
        gs_stPara.bThreadStart = HI_FALSE;
		
        pthread_join(gs_VencPid, 0);
		SAMPLE_PRT("<RH_MPI_VENC_StopGetStream>   pthread Venc Pid\n");

    }
    return HI_SUCCESS;
}






#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif   /* End of #ifdef __cplusplus */


