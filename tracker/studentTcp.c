#include<stdio.h>
#include<string.h>

#include<sys/socket.h>
#include<pthread.h>
#include<netinet/in.h>
#include<netinet/tcp.h>

#include "netcom.h"
#include "studentTcp.h"
#include "auto_track.h"
#include <errno.h>

static int toTeacherSocket = 0;//与老师机连接socket
static int toStudentRightSideSocket=0;//与学生辅助机连接socket
static int toStudentRightSideTCPStatus=0;//与学生辅助机连接状态，1为有心跳，0表示没心跳
static int teacherTraceTCPStatus = 0;//老师机心跳状态,有心跳为1,没心跳为0
extern track_encode_info_t	g_track_encode_info;
extern track_class_info_t	g_class_info;
extern rightside_trigger_info_t	g_rightside_trigger_info;
extern int g_nTriggerValDelay;
extern cam_control_info_t	g_cam_info;
//extern int g_recontrol_flag;
extern StuITRACK_Params 	g_track_param;
int g_recontrol_flag;

/**
* @	和轮询相关的
*/
extern cam_preset_teacher_and_student_ack g_preset_student_req;

cam_preset_teacher_and_student_ack g_preset_student_ack;

extern call_cam_preset_teacher_and_student_req c_preset_student_req ;

track_turn_info_t	g_track_turn_info = {0};


char STUDENTRIGHTSIDE_IP[20]="192.168.0.148";

void setToTeacherSocket(int socket)
{
	toTeacherSocket = socket;
	return (void)0;
}

int getToTeacherSocket()
{
	return toTeacherSocket;
}
void setToStudentRightSideSocket(int socket)
{
	toStudentRightSideSocket = socket;
	return (void)0;
}

int getToStudentRightSideSocket()
{
	return toStudentRightSideSocket;
}

void FarCtrlByEncode(int dsp, unsigned char *data, int len)
{
 int type, speed, addr = 1;
 int fd;
 int stu_num = 0;

 //Print(data,len);
 type = (int)(data[0] | data[1] << 8 | data[2] << 16 | data[3] << 24);
 speed = (int)(data[4] | data[5] << 8 | data[6] << 16 | data[7] << 24);

 if(len < 12) {
  addr = 1;
 } else {
  addr = (int)(data[8] | data[9] << 8 | data[10] << 16 | data[11] << 24);
 }

 if(addr < 1) {
  addr = 1;
 }

 DEBUG(DL_DEBUG, "addr = %d\n",addr);
 DEBUG(DL_DEBUG, "0000type = %d\n",type);
 switch(type) {
  case 12://设置自动跟踪模块的手动和自动模式0自动1手动
   DEBUG(DL_WARNING, "1111track mode speed = %d\n",speed);
   server_set_track_type(speed);
   printf("conect   111111111111111   %d\n",speed);
   break;
  default:
   DEBUG(DL_ERROR, "FarCtrlCamera() command Error\n");
   break;
 }

}


void toTeacherTcpCmd(void *param)
{
	
	char buf[256];
	int severSocket,addr;
	severSocket = getToTeacherSocket();
	HDBMSGHEAD msg;
	HDBMSGHEAD		*msg_header 	= NULL;
	int *value = 0;
	pthread_detach(pthread_self());

	
	while(1)
	{
		int len, msgLen, cameraAddr, type, speed;
        severSocket = getToTeacherSocket();
		if(severSocket < 0)
		{
			DEBUG(DL_ERROR, "get teacher track socket failed!\n");
			break;
		}
		
		len = recv(severSocket, &msg, sizeof(HDBMSGHEAD), 0);
		if(len <1)
		{
			close(severSocket);
			DEBUG(DL_ERROR, "recv teacher data error!\n");
			setToTeacherSocket(-1);
			break;
		}
		
		switch(msg.nMsg)
        {
			case MSG_TEACHER_HEART:
				 teacherTraceTCPStatus=1;
				 DEBUG(DL_DEBUG,"recv teacher heart\n");
				 break;
        	case MSG_FARCTRL:
				len=recv(severSocket,buf,msg.nLen-sizeof(HDBMSGHEAD),0);					
               	if(len<1)
		    	{
					DEBUG(DL_ERROR,"recv teacher trace MSG_FARCTRL error\n");
					close(getToTeacherSocket());
					setToTeacherSocket(-1);
		    	}
				if(msg.nLen-sizeof(HDBMSGHEAD)<12)
				{
					addr =2;
				}
				else
				{
					addr=(int)(buf[8] | buf[9] << 8 | buf[10] << 16 | buf[11] << 24);
				}					
				FarCtrlByEncode(0,buf,msg.nLen-sizeof(HDBMSGHEAD));
				break;
			case MSG_SET_TRACK_TYPE:
				len=recv(severSocket,buf,msg.nLen-sizeof(HDBMSGHEAD),0);					
           		if(len<1)
				{
					DEBUG(DL_ERROR,"recv teacher trace MSG_SET_TRACK_TYPE error\n");
					close(getToTeacherSocket());
					setToTeacherSocket(-1);
				}

				value = (int *)buf;
				if(g_track_encode_info.is_track!=*value)
				{
					if(g_track_encode_info.nTrackSwitchType==0)
					{
						g_cam_info.cam_position_value=42;
						//__call_preset_position(TRIGGER_POSITION42);
					}
				}
				if(*value == 1)
				{
					g_track_encode_info.is_track = 1;
				}
				else if(*value == 0)
				{
					g_track_encode_info.is_track = 0;
				}
				DEBUG(DL_WARNING,"set g_track_encode_info.is_track=%d\n",g_track_encode_info.is_track);
				break;
			
			case MSG_SET_TRACK_SWITCH_TYPE:
				len=recv(severSocket,buf,msg.nLen-sizeof(HDBMSGHEAD),0);					
           		if(len<1)
				{
					DEBUG(DL_ERROR,"recv teacher MSG_SET_TRACK_SWITCH_TYPE error\n");
					close(getToTeacherSocket());
					setToTeacherSocket(-1);
				}

				value = (int *)buf;
				DEBUG(DL_WARNING,"recv teacher MSG_SET_TRACK_SWITCH_TYPE=%d\n",*value);
				if((*value==0)||(*value==1))
				{
					if(*value!=g_track_encode_info.nTrackSwitchType)
					{
						g_track_encode_info.nTrackSwitchType=*value;
						g_recontrol_flag = 1;
						g_track_param.dynamic_param.nTrackSwitchType=*value;
					}
				}
				break;
				
			case MSG_STRATEGY_NO:
				len=recv(severSocket,buf,msg.nLen-sizeof(HDBMSGHEAD),0);					
           		if(len<1)
				{
					DEBUG(DL_ERROR,"recv teacher MSG_STRATEGY_NO error\n");
					close(getToTeacherSocket());
					setToTeacherSocket(-1);
				}

				value = (int *)buf;
				DEBUG(DL_WARNING,"recv teacher MSG_STRATEGY_NO=%d\n",*value);
				if(*value!=g_track_param.dynamic_param.nStrategyNo)
				{
					g_recontrol_flag = 1;
					g_track_param.dynamic_param.nStrategyNo=*value;
				}
				break;
			case MSG_TRACK_TURN_INFO:
				len=recv(severSocket,buf,msg.nLen-sizeof(HDBMSGHEAD),0);					
           		if(len<1)
				{
					DEBUG(DL_ERROR,"recv teacher MSG_TRACK_TURN_INFO error\n");
					close(getToTeacherSocket());
					setToTeacherSocket(-1);
				}
				memcpy(&g_track_turn_info,buf,sizeof(track_turn_info_t));
				DEBUG(DL_WARNING,"set g_track_turn_info.type=%d,turn_time=%d,show_time=%d\n",g_track_turn_info.nTrackTurnType,g_track_turn_info.nTrackTurnTime,g_track_turn_info.nTrackTurnShowTime);
				if(g_track_turn_info.nTrackTurnType>1)
				{
					DEBUG(DL_ERROR,"recv g_track_turn_info.type=%d,error,set g_track_turn_info.type=0\n",g_track_turn_info.nTrackTurnType);
					g_track_turn_info.nTrackTurnType=0;
				}
				break;
			case MSG_CAM_PRESET_COORD_INFO:
				len = recv(severSocket,buf,msg.nLen-sizeof(HDBMSGHEAD),0);
				if(len<1)
				{
					DEBUG(DL_ERROR,"recv teacher MSG_CAM_PRESET_COORD_INFO error\n");
					close(getToTeacherSocket());
					setToTeacherSocket(-1);
				}
				
				memcpy(&g_preset_student_ack,buf,sizeof(cam_preset_teacher_and_student_ack));
				if(g_preset_student_ack.ePresetType >  (PRESET_NUM_MAX))
					{
						DEBUG(DL_WARNING, "preset ack value must less than 5\n");
						g_preset_student_ack.ePresetType=0;
					}

				save_position(&g_preset_student_ack);
				
				break;
			default:
				
				if(msg.nLen-sizeof(HDBMSGHEAD) > 0)
				{
				 len=recv(severSocket,buf,msg.nLen-sizeof(HDBMSGHEAD),0);	   
				 if(len<1)
					 {
				  DEBUG(DL_ERROR,"recv teacher error\n");
				  close(getToTeacherSocket());
				  setToTeacherSocket(-1);
					 }
				}
				
				break;
        }

	}
}

/**
* @	检查与老师机的心跳是否正常，如不正常，关闭socket
*/
void teacherTraceHeart(void *param)
{
   pthread_detach(pthread_self());
   while(1)
   {
   	   sleep(15);
   	   if(teacherTraceTCPStatus)
   	   	{
   	   	   teacherTraceTCPStatus=0;
   	   	   continue;
   	   	}
	   else
	   	{
	   	  if(getToTeacherSocket()>=0)
	   	  	{
			  DEBUG(DL_ERROR, "recv teacher heart status error\n");
	   	  	   shutdown(getToTeacherSocket(),SHUT_RDWR);
			   setToTeacherSocket(-1);
	   	  	}
		   continue;
	   	}
	   	 
   	}
}

void SendClassInfotoTeacher()
{
	char buf[256];
	int severSocket;
	HDBMSGHEAD		*msg_header 	= NULL;
	int *value = 0;
	int lenth;
	int len, msgLen, cameraAddr, type, speed;
    severSocket = getToTeacherSocket();
	if(severSocket <0)
	{
		//printf("no severSocket\n");
		 return;
	}
	msg_header = (HDBMSGHEAD *)buf;
	len = sizeof(HDBMSGHEAD) + sizeof(track_class_info_t);
	msg_header->nLen = htons(len);
	msg_header->nMsg =MSG_SEND_CLASSINFO;
	memcpy(buf + sizeof(HDBMSGHEAD), &g_class_info, sizeof(track_class_info_t));
	lenth=send(severSocket, buf, len, 0);
	
	if(lenth<1)
	{
		DEBUG(DL_ERROR,"send classinfo error\n");
		close(getToTeacherSocket());
		setToTeacherSocket(-1);
	}
    

}
//发送预制位信息
void SendPresetInfotoTeacher()
{
	char buf[256];
	int severSocket;
	HDBMSGHEAD		*msg_header 	= NULL;
	int *value = 0;
	int lenth;
	int len, msgLen, cameraAddr, type, speed;
    severSocket = getToTeacherSocket();
	printf("send req %d\n",g_preset_student_req.ePresetType);
	if(severSocket <0)
	{
		 printf("no severSocket\n");
		 return;
	}
	msg_header = (HDBMSGHEAD *)buf;
	len = sizeof(HDBMSGHEAD) + sizeof(cam_preset_teacher_and_student_req);
	msg_header->nLen = htons(len);
	msg_header->nMsg =MSG_CAM_PRESET_COORD_INFO;
	memcpy(buf + sizeof(HDBMSGHEAD), &g_preset_student_req, sizeof(cam_preset_teacher_and_student_req));
	lenth=send(severSocket, buf, len, 0);
	
	if(lenth<1)
	{
		DEBUG(DL_ERROR,"send presitioninfo error\n");
		close(getToTeacherSocket());
		setToTeacherSocket(-1);
	}
    

}

void SendPresetCoordTeacher()
{
	char buf[256];
	int severSocket;
	HDBMSGHEAD		*msg_header 	= NULL;
	int *value = 0;
	int lenth;
	int len, msgLen, cameraAddr, type, speed;
    severSocket = getToTeacherSocket();
	printf("send coord %d\n",c_preset_student_req.cCamCoordInfo.cur_preset_value);
	printf("coord : %x%x%x%x%x%x%x%x,zoom:%x%x%x%x\n",c_preset_student_req.cCamCoordInfo.pan_tilt[0],c_preset_student_req.cCamCoordInfo.pan_tilt[1],
		c_preset_student_req.cCamCoordInfo.pan_tilt[2],c_preset_student_req.cCamCoordInfo.pan_tilt[3],c_preset_student_req.cCamCoordInfo.pan_tilt[4],
		c_preset_student_req.cCamCoordInfo.pan_tilt[5],c_preset_student_req.cCamCoordInfo.pan_tilt[6],c_preset_student_req.cCamCoordInfo.pan_tilt[7],
		c_preset_student_req.cCamCoordInfo.zoom[0],c_preset_student_req.cCamCoordInfo.zoom[1],c_preset_student_req.cCamCoordInfo.zoom[2],c_preset_student_req.cCamCoordInfo.zoom[3]);
	if(severSocket <0)
	{
		 printf("no severSocket\n");
		 return;
	}
	msg_header = (HDBMSGHEAD *)buf;
	len = sizeof(HDBMSGHEAD) + sizeof(call_cam_preset_teacher_and_student_req);
	msg_header->nLen = htons(len);
	msg_header->nMsg =MSG_CALL_CAM_PRESET;
	memcpy(buf + sizeof(HDBMSGHEAD), &c_preset_student_req, sizeof(call_cam_preset_teacher_and_student_req));
	lenth=send(severSocket, buf, len, 0);
	
	if(lenth<1)
	{
		DEBUG(DL_ERROR,"send presitioninfo error\n");
		close(getToTeacherSocket());
		setToTeacherSocket(-1);
	}
    

}


void STUDENT_TRACETCP(void *param)
{
	pthread_t TCPCmd;
	int clientSocket;
	int listenSocket;
	int opt=1;
	struct sockaddr_in clientAddr, srvAddr;
	pthread_detach(pthread_self());
	setToTeacherSocket(-1);
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(listenSocket < 0)
	{
		DEBUG(DL_ERROR, "socket err:");
	}
	bzero(&srvAddr, sizeof(srvAddr));
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	srvAddr.sin_port = htons(STUDENT_PORT);
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if(bind(listenSocket, (struct sockaddr *)&srvAddr, sizeof(srvAddr)) < 0)
	{
		DEBUG(DL_ERROR, "bind err:");
	}
	
	if(listen(listenSocket, 10) < 0)
	{
		DEBUG(DL_ERROR, "listen err:");
	}
	
	while(1)
	{
		int result,len,nSize=0,nLen;
		len=sizeof(clientAddr);
		printf("listen1\n");
		clientSocket = accept(listenSocket, (struct sockaddr *)&clientAddr, (unsigned int *)&len);
		printf("listen2\n");
		if(getToTeacherSocket() >= 0)
		{
			DEBUG(DL_WARNING, "accept teacher socket getToTeacherSocket>=0\n");
			close(clientSocket);
		}
		else
		{
			DEBUG(DL_WARNING, "accept teacher socket getToTeacherSocket<0\n");
			if((setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR, (void *)&nSize,
					               sizeof(nSize))) == -1) 
			{
				DEBUG(DL_ERROR, "setsockopt failed\n");
			}
			
			nSize = 0;
			nLen = sizeof(nSize);
			result = getsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, &nSize , (DWORD *)&nLen);
			
			if(result==1)
			{
			   DEBUG(DL_ERROR,"getsockopt()  socket:%d	result:%d\n", clientSocket, result);
			}
			
			nSize = 1;
			
			if(setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, &nSize , sizeof(nSize))) 
			{
				DEBUG(DL_ERROR,"set IPPROTOT error\n");
			}
			setToTeacherSocket(clientSocket);
			result = pthread_create(&TCPCmd, NULL, (void *)toTeacherTcpCmd, NULL);
			if(result < 0) {
				DEBUG(DL_ERROR, "create toTeacherTcpCmd() failed\n");
			}
		
		}
		usleep(50000);
	}

}


void HEART(void *param)
{
	int serverSocket;
    HDBMSGHEAD msg;
	pthread_detach(pthread_self());
	msg.nLen=sizeof(HDBMSGHEAD);
	msg.nMsg=MSG_TEACHER_HEART;

	while(1)
	{
		int len;
		serverSocket = getToTeacherSocket();

		if(serverSocket <= 0)
		{
		    DEBUG(DL_WARNING, "HEART get teacher socket failed\n");
			sleep(4);
			continue;
		}
		else
		{
			len = send(serverSocket, &msg, sizeof(HDBMSGHEAD), 0);
			if(len < 1)
		    {
		   	   DEBUG(DL_ERROR, "send heart to teacher error\n");
		        close(serverSocket);
				setToTeacherSocket(-1);
		    }
			else
			{
				DEBUG(DL_DEBUG,"send heart to teacher track!\n");
			}
		}
		sleep(4);
	}
}

void SENDDATATOTEACHER(void *pParam)
{
	unsigned int	times=0;
	unsigned int	nTrueTimes=0;
	unsigned int	nTrueShowTimes=0;
	unsigned int	nUSleepTime=20000;
	int		nTurnFlag=0;
	int		nOnStuFlag=0;
	pthread_detach(pthread_self());

   while(1)
   	{
	   nTrueTimes=g_track_turn_info.nTrackTurnTime*1000000/nUSleepTime;
	   nTrueShowTimes=nTrueTimes+g_track_turn_info.nTrackTurnShowTime*1000000/nUSleepTime;
   	    if(0 != g_track_encode_info.send_cmd)
		{
			//DEBUG(DL_DEBUG, "g_track_encode_info.send_cmd = %d\n",g_track_encode_info.send_cmd);
			studentTracerMove(g_track_encode_info.send_cmd);
			if(SWITCH_TEATHER!=g_track_encode_info.send_cmd)
			{
				nOnStuFlag=1;
			}
			else
			{
				nOnStuFlag=0;
			}
			g_track_encode_info.send_cmd = 0;
			times=0;//发送了一次就清零
			nTurnFlag=0;
		}
		usleep(nUSleepTime);
   	}
}
/**
* @	检查与学生辅助机的心跳是否正常，如不正常，关闭socket
*/
void studentRightSideTraceHeart(void *param)
{
   pthread_detach(pthread_self());
   while(1)
   {
   	   sleep(15);
   	   if(toStudentRightSideTCPStatus)
   	   	{
   	   	   toStudentRightSideTCPStatus=0;
   	   	   continue;
   	   	}
	   else
	   	{
	   	  if(getToStudentRightSideSocket()>=0)
	   	  	{
	   	  	   shutdown(getToStudentRightSideSocket(),SHUT_RDWR);
			   setToStudentRightSideSocket(-1);
			   DEBUG(DL_WARNING, "shutdown StudentRightSideSocket\n");
	   	  	}
		   continue;
	   	}
	   	 
   	}
}

/**
* @	学生机与学生辅助机连接线程，connect学生辅助机，以及断开后重连，并接收学生辅助机发来的消息
*/
void StudentRightSideTcp(void *param)
{
	int StudentRightSideSocket;
	char recvbuf[256],sendbuf[256];
	HDBMSGHEAD msg;
	struct sockaddr_in student_addr;
	pthread_t rid;

	int		send_students_rightside = 0;
	int		recv_value		= 0;
	int recvLen, len;
	int i=0,nSumCur=0,nTimePointWritePos=0,nWriteNum=0;
	track_class_info_t tRecvClassInfo;
	unsigned long long llCurTime=0,llLastStandTime=0;
	unsigned int nInteractionPos=0;
	int result;
	
	rightside_trigger_info_t	rightside_trigger_info;
	pthread_detach(pthread_self());
	
	setToStudentRightSideSocket(-1);
    result =  ConfigGetKey("student_righttracer.ini", "student_righttracer", "ipaddr",STUDENTRIGHTSIDE_IP);

	if(result != 0) {
		DEBUG(DL_ERROR, "Get student_righttrace ipaddr failed\n");
	}

	StudentRightSideSocket = socket(AF_INET, SOCK_STREAM, 0);

	student_addr.sin_family = AF_INET;
	student_addr.sin_port = htons(STUDENT_RIGHTSIDE_PORT);
	inet_pton(AF_INET, STUDENTRIGHTSIDE_IP,&student_addr.sin_addr);

	DEBUG(DL_WARNING,"student right side tcp tread create");

	DEBUG(DL_WARNING,"@@@@@@@@%s %d\n",STUDENTRIGHTSIDE_IP,STUDENT_RIGHTSIDE_PORT);
	while(-1 == connect(StudentRightSideSocket, (struct sockaddr *)&student_addr, sizeof(struct sockaddr)))
	{
	    DEBUG(DL_ERROR,"student right side tcp error\n");
		close(StudentRightSideSocket);
		StudentRightSideSocket = socket(AF_INET, SOCK_STREAM, 0);
		sleep(5);
	   
	}
	setToStudentRightSideSocket(StudentRightSideSocket);
    pthread_create(&rid,NULL,(void *)studentRightSideTraceHeart,NULL);
	if(result < 0)
	{
		DEBUG(DL_ERROR, "create studentRightSideTraceHeart() failed\n");
	}
	

	while(1)
	{
		if(getToStudentRightSideSocket() < 0)
		{
			do
			{
			    DEBUG(DL_WARNING,"########%s %d",STUDENTRIGHTSIDE_IP,STUDENT_RIGHTSIDE_PORT);
			    DEBUG(DL_WARNING,"student right side tcp error  dododo StudentRightSideSocket=%d,errno = %d,error message:%s\n",StudentRightSideSocket,errno, strerror(errno));
				close(StudentRightSideSocket);
				StudentRightSideSocket = socket(AF_INET, SOCK_STREAM, 0);
				sleep(5);
			}
			while(-1 == connect(StudentRightSideSocket, (struct sockaddr *)&student_addr, sizeof(struct sockaddr)));

			setToStudentRightSideSocket(StudentRightSideSocket);
		}
		
		recvLen = recv(StudentRightSideSocket, &msg, sizeof(HDBMSGHEAD), 0);

		//DEBUG(DL_DEBUG,"########## student recvLen = %d\n",recvLen);
		if(recvLen < 1)
		{
			DEBUG(DL_ERROR,"recv student right side data error\n");
			close(StudentRightSideSocket);
			setToStudentRightSideSocket(-1);
		}
        switch(msg.nMsg)
        {
			case MSG_TEACHER_HEART:
				 toStudentRightSideTCPStatus=1;
				 DEBUG(DL_DEBUG, "recv student right side HEART \n");
				 //老师机向学生机返回心跳
				 len=send(StudentRightSideSocket,&msg,sizeof(HDBMSGHEAD),0);
				 if(len<1)
				 {
					DEBUG(DL_ERROR, "send HEART to student right side error\n");
					close(StudentRightSideSocket);
					setToStudentRightSideSocket(-1);
				 }
				 else
				 {
					 DEBUG(DL_DEBUG, "send HEART to student right side\n");
				 }
				break;
				//得到学生机辅助机上报的跟踪类型
			case MSG_TRIGGER_TYPE:
				//课前取景0/课中取景1
				//1表示有触发，2表示坐下(只有课前取景有用)，0不处理
				recvLen=recv(StudentRightSideSocket, recvbuf, sizeof(rightside_trigger_info_t), 0);
				if(recvLen < 1)
				{
					DEBUG(DL_ERROR,"recv student right side MSG_TRIGGER_TYPE info error\n");
					close(StudentRightSideSocket);
					setToStudentRightSideSocket(-1);
				}
				else
				{
					memcpy(&rightside_trigger_info,recvbuf,sizeof(rightside_trigger_info_t));
					DEBUG(DL_WARNING,"recv student right side trigger info nTriggerType=%d,nTriggerVal=%d\n",
						   rightside_trigger_info.nTriggerType,rightside_trigger_info.nTriggerVal);
					if(rightside_trigger_info.nTriggerType==0)
					{
						memcpy(&g_rightside_trigger_info,recvbuf,sizeof(rightside_trigger_info_t));
					}
					else if(rightside_trigger_info.nTriggerType==1) 
					{
						if(rightside_trigger_info.nTriggerVal==1)
						{
							memcpy(&g_rightside_trigger_info,recvbuf,sizeof(rightside_trigger_info_t));
							g_track_encode_info.nTriggerValDelay=0;
						}
					}
				}
				break;
			
			default :
				break;
       }
	}
}


void studentTrace(void *param)
{
	pthread_t rtmpid[3],TCPCmd;
	int result;
	pthread_detach(pthread_self());
	result = pthread_create(&rtmpid[0], NULL, (void *)STUDENT_TRACETCP, NULL);
	if(result < 0)
	{
		DEBUG(DL_ERROR, "create DSP1TCPTask() failed\n");
	}

	result = pthread_create(&rtmpid[1], NULL, (void *)HEART, NULL);


	if(result < 0)
	{
		DEBUG(DL_ERROR, "create DSP1TCPTask() failed\n");
	}

    result = pthread_create(&rtmpid[1], NULL, (void *)SENDDATATOTEACHER, NULL);


	if(result < 0)
	{
		DEBUG(DL_ERROR, "create SENDDATATOTEACHER() failed\n");
	}
	result = pthread_create(&TCPCmd, NULL, (void *)teacherTraceHeart, NULL);
	if(result < 0) {
		DEBUG(DL_ERROR, "create teacherTraceHeart() failed\n");
	}
	result = pthread_create(&rtmpid[2], NULL, (void *)StudentRightSideTcp, NULL);
	if(result < 0) {
		DEBUG(DL_ERROR, "create StudentRightSideTcp() failed\n");
	}


}


int studentTracerMove(unsigned char data)
{
    unsigned char sendData[256];
	int len,num;
	HDBMSGHEAD msg;
	msg.nLen=1+sizeof(HDBMSGHEAD);
	msg.nMsg=MSG_TRACKAUTO;
	memcpy(sendData,&msg,sizeof(HDBMSGHEAD));
	num=sizeof(HDBMSGHEAD);
	sendData[num]=data;
	if(getToTeacherSocket()<0)
		{
		  DEBUG(DL_WARNING, "studentTracerMove get teacher socket failed\n");
		  return -1;
		}
	else
		{
		   len=send(getToTeacherSocket(),sendData,1+sizeof(HDBMSGHEAD),0);
		   if(len<1)
		   	{
		   	   DEBUG(DL_ERROR, "studentTracerMove send data to teacher error\n");
		   	   close(getToTeacherSocket());
			   setToTeacherSocket(-1);
			   return -1;
		   	}
		   
		}
	return 0;
}

