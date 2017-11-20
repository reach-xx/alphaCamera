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


//#include "sample_comm.h"
#include "rh_interface.h"
#include "rh_encoder_send.h"

static pthread_t gs_VencPid;
static SAMPLE_VENC_GETSTREAM_PARA_S gs_stPara;
static 	sem_t s_sem_con_lock;
/*client param infomation*/
DSPCliParam gDSPCliPara[4];
static pthread_t client_threadid[MAX_CLIENT] = {0};
static pthread_t s_TcpServerPid ;
static HI_U32 gnSentCount = 0;
static unsigned long g_video_time = 0;
/*send data packet*/
static unsigned char gszSendBuf[MAX_PACKET];



/*Initial client infomation*/
void InitClientData(unsigned char port_num)
{
	int cli;

	for(cli = 0; cli < MAX_CLIENT; cli++) 	{
		SETCLIUSED(port_num, cli, HI_FALSE);
		SETSOCK(port_num, cli, INVALID_SOCKET);
		SETCLILOGIN(port_num, cli, HI_FALSE);
		SETLOGINTYPE(port_num, cli, LOGIN_USER);
		SETLOWRATEFLAG(port_num, cli, STOP);
	}

	return;
}

void ClearLostClient(unsigned char dsp)
{
	int cli ;
	unsigned long currenttime = 0;

	for(cli = 0; cli < MAX_CLIENT; cli++) {
		if(!ISUSED(dsp, cli) && ISSOCK(dsp, cli)) {
			SAMPLE_PRT( "[ClearLostClient] Socket = %d \n", GETSOCK(dsp, cli));
			shutdown(GETSOCK(dsp, cli), 2);
			close(GETSOCK(dsp, cli));
			SETSOCK(dsp, cli, INVALID_SOCKET);
			SETCLILOGIN(dsp, cli, HI_FALSE) ;
		}

		if(ISUSED(dsp, cli) && (!ISLOGIN(dsp, cli))) {
			if((currenttime - GETCONNECTTIME(dsp, cli)) > 300) {
				SAMPLE_PRT( "\n\n\n\n ClearLostClient Socket = %d,client %d \n\n\n\n", GETSOCK(dsp, cli), cli);
				shutdown(GETSOCK(dsp, cli), 2);
				close(GETSOCK(dsp, cli));
				SETCLIUSED(dsp, cli, HI_FALSE);
				SETSOCK(dsp, cli, INVALID_SOCKET);
				SETCLILOGIN(dsp, cli, HI_FALSE) ;
			}
		}

	}
}


/*get null client index*/
static int GetNullClientData(unsigned char port_num)
{
	int cli ;

	for(cli = 0; cli < MAX_CLIENT; cli++) {
		if((!ISUSED(port_num, cli)) && (!ISLOGIN(port_num, cli))) {
			SAMPLE_PRT( "[add Client] number = %d \n", cli);
			return cli;
		}
	}

	return -1;
}

void PackHeaderMSG(HI_U8 *data,
                   HI_U8 type, HI_U16 len)
{
	MSGHEAD  *p;
	p = (MSGHEAD *)data;
	memset(p, 0, HEAD_LEN);
	p->nLen = htons(len);
	p->nMsg = type;
	return ;
}


static void InitStandardParam(SYSPARAMS *std)
{
	std->dwAddr = 	inet_addr("192.168.4.88");
	std->dwGateWay =  inet_addr("192.168.4.254");
	std->dwNetMark = inet_addr("255.255.255.0");

	std->szMacAddr[0] = 0x00;
	std->szMacAddr[1] = 0x09;
	std->szMacAddr[2] = 0x30;
	std->szMacAddr[3] = 0x28;
	std->szMacAddr[4] = 0x12;
	std->szMacAddr[5] = 0x22;
	strcpy(std->strName, "DSS-ENC-MOD");
	strcpy(std->strVer, "6.4.8");
	std->unChannel = 1;
	std->bType = ENCODE_TYPE;  //0 -------VGABOX  3-------200 4-------110 5-------120 6--------1200	8 --ENC-1100
	bzero(std->nTemp, sizeof(std->nTemp));
}

				   
/*set send timeout*/
int SetSendTimeOut(HI_S32 sSocket, unsigned long time)
{
	struct timeval timeout ;
	int ret = 0;

	timeout.tv_sec = time ; //3
	timeout.tv_usec = 0;

	ret = setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

	if(ret == -1) {
	   SAMPLE_PRT( "setsockopt() Set Send Time Failed\n");
	}

	return ret;
}

int mid_recv(HI_S32 s, char *buff, int len, int flags)
{
	int toplen = 0;
	int readlen = 0;

	while(len - toplen > 0) {
		readlen =  recv(s, buff + toplen, len - toplen, flags);

		if(readlen <= 0) {
			SAMPLE_PRT("ERROR\n");
			return -1;
		}

		if(readlen != len - toplen) {
			SAMPLE_PRT("WARNNING,i read the buff len = %d,i need len = %d\n", readlen, len);
		}

		toplen += readlen;
	}

	return toplen;
}


/*PORT_ONE TCP communication message*/
void OneTcpComMsg(void *pParams)
{
   char szData[256] = {0}, szPass[] = "123";
   int nLen;
   HI_S32 sSocket;
   int nPos;
   HI_U16 length;
   MSGHEAD header, *phead;
   int *pnPos = (int *)pParams;
   SAMPLE_PRT( "enter OneTcpComMsg() function!!\n");
   nPos = *pnPos;

   free(pParams);  //free memory
   pParams = NULL;
   sSocket = GETSOCK(0, nPos);

   /*set timeout 3s*/
   SetSendTimeOut(sSocket, 3);
   /*set recv time out*/

   bzero(szData, 256);
   phead = &header;
   memset(&header, 0, sizeof(MSGHEAD));
   sem_post(&s_sem_con_lock);
   SAMPLE_PRT( "OneTcpComMsg()	   sem_post!!!!\n");

   while(HI_TRUE == gs_stPara.bThreadStart) {
	   memset(szData, 0, sizeof(szData));

	   if(sSocket <= 0) {
		   goto ExitClientMsg;
	   }

	   nLen = recv(sSocket, szData, HEAD_LEN, 0);

	   SAMPLE_PRT( "nlen = %d \n", nLen);
	   memcpy(phead, szData, HEAD_LEN);

	   if(nLen < HEAD_LEN || nLen == -1) {
		   SAMPLE_PRT( "nLen< %d !\n", HEAD_LEN);
		   goto ExitClientMsg;
	   }

	   SAMPLE_PRT( "receive length:%d,HEAD_LEN:%d\n", nLen, HEAD_LEN);
	   //message length
	   phead->nLen = ntohs(phead->nLen);

	   if(phead->nLen - HEAD_LEN > 0) {
		   nLen = 0;
		   nLen = mid_recv(sSocket, szData + HEAD_LEN, phead->nLen - HEAD_LEN, 0);
		   SAMPLE_PRT( "nMsgLen = %d,len:%d!\n", phead->nLen, nLen);

		   if(nLen == -1 || nLen < phead->nLen - HEAD_LEN) {
			   SAMPLE_PRT( "nLen < nMsgLen -HEAD_LEN\n");

			   goto ExitClientMsg;
		   }
	   }

	   switch(phead->nMsg) {
		   case MSG_GET_AUDIOPARAM:
			   SAMPLE_PRT( "PORT_ONE Get AudioParam \n");
			   break;

		   case MSG_GET_VIDEOPARAM:
			   SAMPLE_PRT( "PORT_ONE Get VideoParam \n");
			   break;

		   case MSG_SET_AUDIOPARAM:
			   SAMPLE_PRT( "PORT_ONE Set AudioParam \n");
			   break;

		   case MSG_SET_VIDEOPARAM:
			   SAMPLE_PRT( "PORT_ONE Set VideoParam \n");
			   break;
		   case MSG_SET_DEFPARAM:
			   SAMPLE_PRT( "DSP:%d Set Default Param !!!!!!!!\n", PORT_ONE);
			   break;

		   case MSG_ADD_TEXT:
			   SAMPLE_PRT( "enter into MSG_ADD_TEXT \n");
			   break;

		   case MSG_FARCTRL:
			   SAMPLE_PRT( "PORT_ONE Far Control\n");
			   break;

		   case MSG_SET_SYSTIME: 
				break;

		   case MSG_PASSWORD: {
		   		SYSPARAMS sysPara;
			   if(!(strcmp("sawyer", szData + HEAD_LEN))) {
				   SETLOGINTYPE(0, nPos, LOGIN_ADMIN);
			   } else if(szData[HEAD_LEN] == 'A' && !strcmp(szPass, szData + HEAD_LEN + 1)) {
				   SETLOGINTYPE(0, nPos, LOGIN_ADMIN);
				   SAMPLE_PRT( "logo Admin!\n");
			   } else if(szData[HEAD_LEN] == 'U' && !strcmp(szPass, szData + HEAD_LEN + 1)) {
				   SETLOGINTYPE(0, nPos, LOGIN_USER);
				   SAMPLE_PRT( "logo User!\n");
			   } else {
				   PackHeaderMSG((HI_U8 *)szData, MSG_PASSWORD_ERR, HEAD_LEN);
				   send(sSocket, szData, HEAD_LEN, 0);
				   SAMPLE_PRT( "logo error!\n");
				   SETLOGINTYPE(0, nPos, LOGIN_ADMIN);
				   goto ExitClientMsg;	 //
			   }

			   PackHeaderMSG((HI_U8 *)szData, MSG_CONNECTSUCC, HEAD_LEN);
			   send(sSocket, szData, HEAD_LEN, 0);
			   SAMPLE_PRT( "send MSG_CONNECTSUCC");
			   InitStandardParam(&sysPara);
			   /*length*/
			   length = HEAD_LEN + sizeof(sysPara);
			   PackHeaderMSG((HI_U8 *)szData, MSG_SYSPARAMS, length);

			   memcpy(szData + HEAD_LEN, &sysPara, sizeof(sysPara));
			   send(sSocket, szData, length, 0);
			   SAMPLE_PRT( "user log!  DSP:%d  client:%d \n", PORT_ONE, nPos);
			   /*set client login succeed*/
			   SETCLIUSED(PORT_ONE, nPos, HI_TRUE);
			   SETCLILOGIN(PORT_ONE, nPos, HI_TRUE);
			   SAMPLE_PRT( "DSP:%d	socket:%d\n", PORT_ONE, GETSOCK(PORT_ONE, nPos));
			   SAMPLE_PRT( "ISUSED=%d, ISLOGIN=%d\n", ISUSED(PORT_ONE, nPos), ISLOGIN(PORT_ONE, nPos));
			   break;
		   }

		   case MSG_SYSPARAMS: 
			   SAMPLE_PRT( "Get Sys Params ..........................\n");
			   break;
		   break;

		   case MSG_SETPARAMS:
			   SAMPLE_PRT( "Set Params! gSysParams Bytes\n");
			   break;

		   case MSG_SAVEPARAMS:
			   SAMPLE_PRT( "Save Params!\n");
			   break;

		   case MSG_QUALITYVALUE:
			   SAMPLE_PRT( "Set quality value !!\n");
			   break;

		   case MSG_RESTARTSYS:
				system("sync");
				sleep(4);
				SAMPLE_PRT( "Restart sys\n");
				system("reboot -f");
				break;

			case MSG_UpdataFile: 
				break;

		   case MSG_UPDATEFILE_ROOT:
			   SAMPLE_PRT( "Updata Root file\n");
			   break;

		   case MSG_REQ_I:
			   SAMPLE_PRT( "Request I Frame!\n");
			   break;

		   case MSG_SEND_INPUT: 
				SAMPLE_PRT( "send the input type message \n");
				break;

		   case MSG_LOW_BITRATE: 
		   		break;

		   case MSG_MUTE: 
		   		break;

		   case MSG_SETVGAADJUST: 
		   		break;

		   case MSG_GSCREEN_CHECK: 
		   		break;

		   case MSG_CAMERACTRL_PROTOCOL: 
		   		break;

		   case MSG_LOCK_SCREEN:
			   break;

		   case MSG_GET_LOGOINFO: 
		   		break;

		   case MSG_SET_LOGOINFO:
		   		break;

		   default:
		   		SAMPLE_PRT( "Error Command  End!\n");
			   	break;
	   }

	   SAMPLE_PRT( "Switch End!\n");
   }

ExitClientMsg:
   client_threadid[nPos] = 0;

   if(sSocket == GETSOCK(PORT_ONE, nPos)) {
	   SAMPLE_PRT("Exit Client Msg DSP:%d nPos = %d sSocket = %d\n", PORT_ONE, nPos, sSocket);
	   SETCLIUSED(PORT_ONE, nPos, HI_FALSE);
	   SETSOCK(PORT_ONE, nPos, INVALID_SOCKET);
	   SETLOGINTYPE(PORT_ONE, nPos, LOGIN_USER);
	   SETCLILOGIN(PORT_ONE, nPos, HI_FALSE);
	   SETLOWRATEFLAG(PORT_ONE, nPos, STOP);
	   close(sSocket);
   } else {

	   int cli ;
	   int dsp = 0;

	   for(cli = 0; cli < MAX_CLIENT; cli++) {
		   SAMPLE_PRT("[%s %d],socket = %d,,blogin=%d,bused=%d,nlogintype=%d\n", __FUNCTION__, __LINE__, gDSPCliPara[dsp].cliDATA[cli].sSocket,
				  gDSPCliPara[dsp].cliDATA[cli].bLogIn,
				  gDSPCliPara[dsp].cliDATA[cli].bUsed,
				  gDSPCliPara[dsp].cliDATA[cli].nLogInType);
	   }

	   SAMPLE_PRT("Warnning,this client is also cleared socket =%d,pos=%d,s=%d\n", sSocket, nPos, GETSOCK(PORT_ONE, nPos));
   }

   pthread_detach(pthread_self());
   pthread_exit(NULL);
}


/*EncodeManage Send   TCP task mode*/
HI_U32 TCPServerTask(void *pParam)
{

	HI_S32 sClientSocket;
	HI_U32 s32Ret = HI_SUCCESS;
	struct sockaddr_in SrvAddr, ClientAddr;
	HI_S32			ServSock = 0;
	int 			nLen;
	char 			newipconnect[20];
	short 			sPort = 3100;
	int 			clientsocket = 0;
	void 			*ret = 0;
	int 			ipos = 0;
	int 			fileflags;
	int 			opt = 1;

	
	printf( "TCPServerTask()........GetPid():%d\n", getpid());
	sem_init((sem_t *)&s_sem_con_lock, 0, 0);
	InitClientData(0);
DSP1STARTRUN:

	printf( "TCPServerTask Task start...\n");
	bzero(&SrvAddr, sizeof(struct sockaddr_in));
	SrvAddr.sin_family = AF_INET;
	SrvAddr.sin_port = htons(sPort);
	SrvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ServSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(ServSock < 0)  {
		SAMPLE_PRT( "ListenTask create error:%d,error msg: = %s\n", errno, strerror(errno));
		return HI_FAILURE;
	}

	setsockopt(ServSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if(bind(ServSock, (struct sockaddr *)&SrvAddr, sizeof(SrvAddr)) < 0)  {
		SAMPLE_PRT( "bind terror:%d,error msg: = %s\n", errno, strerror(errno));
		return HI_FAILURE;
	}

	if(listen(ServSock, 10) < 0) {
		SAMPLE_PRT( "listen error:%d,error msg: = %s", errno, strerror(errno));
		return HI_FAILURE;
	}

	if((fileflags = fcntl(ServSock, F_GETFL, 0)) == -1) {
		SAMPLE_PRT( "fcntl F_GETFL error:%d,error msg: = %s\n", errno, strerror(errno));
		return HI_FAILURE;
	}

	if(fcntl(ServSock, F_SETFL, fileflags | O_NONBLOCK) == -1) {
		SAMPLE_PRT( "fcntl F_SETFL error:%d,error msg: = %s\n", errno, strerror(errno));
		return HI_FAILURE;
	}

	nLen = sizeof(struct sockaddr_in);
	while(HI_TRUE == gs_stPara.bThreadStart)  {
		memset(&ClientAddr, 0, sizeof(struct sockaddr_in));
		nLen = sizeof(struct sockaddr_in);
		sClientSocket = accept(ServSock, (void *)&ClientAddr, (HI_U32 *)&nLen);

		if(sClientSocket > 0) {
			//打印客户端ip//
			inet_ntop(AF_INET, (void *) & (ClientAddr.sin_addr), newipconnect, 16);
			int nPos = 0;
			char chTemp[16], *pchTemp;

			pchTemp = &chTemp[0];
			ClearLostClient(0);
			nPos = GetNullClientData(0);

			if(-1 == nPos) 	{
				char chData[20];
				PackHeaderMSG((HI_U8 *)chData, MSG_MAXCLIENT_ERR, HEAD_LEN);
				SAMPLE_PRT( "ERROR: max client error\n");
				send(sClientSocket, chData, HEAD_LEN, 0);
				close(sClientSocket);
			} else {
				int nSize = 0;
				int result;
				int *pnPos = malloc(sizeof(int));

				/* set client used */
				SETCLIUSED(0, nPos, HI_TRUE);
				SETSOCK(0, nPos, sClientSocket);
				nSize = 1;

				if((setsockopt(sClientSocket, SOL_SOCKET, SO_REUSEADDR, (void *)&nSize,
				               sizeof(nSize))) == -1) {
					SAMPLE_PRT("setsockopt failed\n");
				}

				nSize = 0;
				nLen = sizeof(nLen);
				result = getsockopt(sClientSocket, SOL_SOCKET, SO_SNDBUF, &nSize , (HI_U32 *)&nLen);

				if(result) {
					SAMPLE_PRT( "getsockopt() errno:%d socket:%d  result:%d\n", errno, sClientSocket, result);
				}

				nSize = 1;

				if(setsockopt(sClientSocket, IPPROTO_TCP, TCP_NODELAY, &nSize , sizeof(nSize))) {
					SAMPLE_PRT( "Setsockopt error%d\n", errno);
				}

				memset(chTemp, 0, 16);
				pchTemp = inet_ntoa(ClientAddr.sin_addr);
				SAMPLE_PRT( "Clent:%s connected,nPos:%d socket:%d!\n", chTemp, nPos, sClientSocket);

				*pnPos = nPos;
				//Create ClientMsg task!
				result = pthread_create(&client_threadid[nPos], NULL, (void *)OneTcpComMsg, (void *)pnPos);
				usleep(400000);

				if(result) {
					close(sClientSocket);   //
					SAMPLE_PRT( "creat pthread ClientMsg error  = %d!\n" , errno);
					continue;
				}

				sem_wait(&s_sem_con_lock);
				SAMPLE_PRT( "sem_wait() semphone inval!!!  result = %d\n", result);
			}
		} else {
			if(errno == ECONNABORTED || errno == EAGAIN) { //软件原因中断
				//				ERR_PRN("errno =%d program again start!!!\n",errno);
				usleep(100000);
				continue;
			}

			if(ServSock > 0)	{
				close(ServSock);
			}

			goto DSP1STARTRUN;
		}

	}

	for(ipos = 0; ipos < MAX_CLIENT; ipos++) {
		if(client_threadid[ipos]) {
			clientsocket = GETSOCK(PORT_ONE, ipos);

			if(clientsocket != INVALID_SOCKET) {
				close(clientsocket);
				SETSOCK(PORT_ONE, ipos, INVALID_SOCKET);
			}

			if(pthread_join(client_threadid[ipos], &ret) == 0) {
				if(ret == THREAD_FAILURE) {
					SAMPLE_PRT( "client_threadid ret == THREAD_FAILURE \n");
				}
			}
		}
	}

	if(ServSock > 0)	{
		close(ServSock);
	}

	ServSock = 0;
	return s32Ret;
}

static unsigned long getCurrentTime2(void)
{
	struct timeval tv;
	struct timezone tz;
	unsigned long ultime;

	gettimeofday(&tv , &tz);
	ultime = tv.tv_sec * 1000 + ((tv.tv_usec) / 1000);
	return (ultime);
}

/*
##
## send date to socket runtime
##
##
*/
int WriteData(int s, void *pBuf, int nSize)
{
	int nWriteLen = 0;
	int nRet = 0;
	int nCount = 0;

	while(nWriteLen < nSize) {

		nRet = send(s, (char *)pBuf + nWriteLen, nSize - nWriteLen, 0);

		if(nRet < 0) {
			if((errno == ENOBUFS) && (nCount < 10)) {
				SAMPLE_PRT( "network buffer have been full!\n");
				usleep(10000);
				nCount++;
				continue;
			}
			return nRet;
		} else if(nRet == 0) {
			SAMPLE_PRT( "Send Net Data Error nRet= %d\n", nRet);
			continue;
		}

		nWriteLen += nRet;
		nCount = 0;
	}

	return nWriteLen;
}


/*send encode data to every client*/
void SendDataToClient(int nLen, unsigned char *pData,
                      int nFlag, unsigned char index, int width, int height)
{
	int nRet, nSent, nSendLen, nPacketCount, nMaxDataLen;
	FRAMEHEAD  DataFrame;
	int cnt = 0;
	MSGHEAD	*p;
	HI_S32  sendsocket = 0;

	bzero(&DataFrame, sizeof(FRAMEHEAD));
	nSent = 0;
	nSendLen = 0;
	nPacketCount = 0;
	nMaxDataLen = MAX_PACKET - sizeof(FRAMEHEAD) - HEAD_LEN;


	DataFrame.ID = 0x56534434;				// "4DSP"
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


	if(nFlag == 1)	{	//if I frame
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


		for(cnt = 0; cnt < MAX_CLIENT; cnt++) {
			if(ISUSED(index, cnt) && ISLOGIN(index, cnt) && (GETLOWRATEFLAG(index, cnt) == STOP)) {
				sendsocket = GETSOCK(index, cnt);

				if(sendsocket > 0)  {
					nRet = WriteData(sendsocket, gszSendBuf, nSendLen + sizeof(FRAMEHEAD) + HEAD_LEN);

					if(nRet < 0) {
						SETCLIUSED(index, cnt, HI_FALSE);
						SETCLILOGIN(index, cnt, HI_FALSE);
						SAMPLE_PRT( "Error: SOCK = %d count = %d  errno = %d  ret = %d\n", sendsocket, cnt, errno, nRet);
					}
				}
			}
		}

		nSent += nSendLen;
	}

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
    	SendDataToClient(pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 
			pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,1, 0,1920, 1080);
    }
	
	return s32Ret;
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
                    /*******************************************************
                     step 2.5 : send frame to EncoderManage
                    *******************************************************/
                    if(i == 1)  //
                    {
						s32Ret = RH_MPI_VENC_SendData(&stStream);
	                    if (HI_SUCCESS != s32Ret)
	                    {
	                        free(stStream.pstPack);
	                        stStream.pstPack = NULL;
	                        SAMPLE_PRT("send stream failed!\n");
	                        break;
	                    }
                    }
//					printf("---------------------------------------:%d\n",i);					
				
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
	
	result = pthread_create(&s_TcpServerPid, NULL, (void *)TCPServerTask, (void *)NULL);
	if(result < 0) {
		SAMPLE_PRT( "create TCPServerTask() failed\n");
	}
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
		pthread_join(s_TcpServerPid, 0);
		SAMPLE_PRT("<RH_MPI_VENC_StopGetStream>   pthread TcpServer Pid\n");
    }
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif   /* End of #ifdef __cplusplus */


