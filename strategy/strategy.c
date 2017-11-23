#include "strategy.h"

#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

#include<stdio.h>
#include<string.h>
#include <unistd.h> 


#include<sys/socket.h>
#include<pthread.h>
#include<netinet/in.h>
#include<sys/types.h>
#include <arpa/inet.h> 
#include<linux/tcp.h>
#include <errno.h>
#include "auto_track.h"


strategy_timeinfo_t gStrategyTimeInfo;
strategy_info_t	gStrategyInfo;

extern strategy_input_info_t	gStrategyInputInfo;
extern track_strategy_info_t		g_track_strategy_info;//�л�������ر���,�ñ�����Ϊһȫ�ֱ���ֱ����kitetool���߻������ļ��������°汾���л�������Ҫ�Ĳ���ֱ�ӴӸñ�����ȡֵ
extern ITRACK_STUDENTS_RIGHT_SIDE_Params 	g_track_param;

#define IPADDR	"ipaddr"
char BLACKBOARDTRACE_IP[BLACKCONNECT_NUM_MAX][16]={"172.25.100.104","172.25.100.105"};
//char STUDENTTRACE_IP[STUCONNECT_NUM_MAX][16]={"192.168.0.148","10.11.0.2"};

static int	teacher_to_encode_socket						= -1;	//��ʦ��������ʦ��������socket
static int	teacher_to_students_socket[STUCONNECT_NUM_MAX]	= {-1};	//��ʦ������ѧ������socket
static int	teacher_to_blackboard_socket[BLACKCONNECT_NUM_MAX]	= {-1};	//��ʦ�����Ӱ������socket

static int teacer_to_encode_connect_status 	= 0;	//��ʦ������ʦ������,������Ϊ1,û����Ϊ0
static int teacer_to_students_connect_status[STUCONNECT_NUM_MAX]={0};	//��ʦ����ѧ����������Ϊ1,û����Ϊ0
static int teacer_to_blackboard_connect_status[BLACKCONNECT_NUM_MAX]={0};	//��ʦ���Ͱ����������Ϊ1,û����Ϊ0

/**
*--------------------------------------------------------------------------------------------
* ������Ϊ���豸��Ϊ��ʦ������ѧ�����Ĳ���
* 
*
*--------------------------------------------------------------------------------------------
*/

/**
* @	��������: set_socket_teacher_to_students_students()
* @  ��������: ������ʦ������ѧ������socket
* @  �������: socket -- Ҫ�����socket
* @  �������: ��
* @  ����ֵ:	   0��ʾ�ɹ�
*/
static void set_socket_teacher_to_students_students(int socket,int index)
{
	teacher_to_students_socket[index] = socket;
	return (void)0;
}

/**
* @	��������: get_socket_teacher_to_students_students()
* @  ��������: ��ȡ��ʦ������ѧ������socket
* @  �������: ��
* @  �������: ��
* @  ����ֵ:	   ������ʦ������ѧ�������ӵ�socket
*/

static int get_socket_teacher_to_students_students(int index)
{
	return teacher_to_students_socket[index];
}

/**
*--------------------------------------------------------------------------------------------
* ������Ϊ���豸��Ϊ��ʦ�����Ӱ������ѧ�����Ĳ���
* 
*
*--------------------------------------------------------------------------------------------
*/

/**
* @	��������: set_socket_teacher_to_blackboard()
* @  ��������: ������ʦ�����Ӱ������socket
* @  �������: socket -- Ҫ�����socket
* @  �������: ��
* @  ����ֵ:	   0��ʾ�ɹ�
*/
static void set_socket_teacher_to_blackboard(int socket,int index)
{
	teacher_to_blackboard_socket[index] = socket;
	return (void)0;
}

/**
* @	��������: get_socket_teacher_to_blackboard()
* @  ��������: ��ȡ��ʦ�����Ӱ������socket
* @  �������: ��
* @  �������: ��
* @  ����ֵ:	   ������ʦ�����Ӱ�������ӵ�socket
*/

static int get_socket_teacher_to_blackboard(int index)
{
	return teacher_to_blackboard_socket[index];
}

/**
* @	��ѧ�������������
*/
int teacher_send_msg_to_students(unsigned char *data,int len,int cmd,int sock_num)
{
	unsigned char sendData[256] = {0};
	int lenth 					= 0;
	MSGHEAD msg;
	msg.nLen = len + sizeof(MSGHEAD);
	msg.nMsg = cmd;
	
	memcpy(sendData,&msg,sizeof(MSGHEAD));
	memcpy(sendData+sizeof(MSGHEAD),data,len);
	
	if(get_socket_teacher_to_students_students(sock_num) <0 )
	{
		DEBUG(DL_ERROR,"teacher_send_msg_to_students get student %d socket error\n",sock_num);
		return -1;
	}
	else
	{
		lenth=send(get_socket_teacher_to_students_students(sock_num),sendData,len+sizeof(MSGHEAD),0);
		if(lenth < 1)
		{
			DEBUG(DL_ERROR,"teacher_send_msg_to_students send data to student %d error\n",sock_num);
			close(get_socket_teacher_to_students_students(sock_num));
			set_socket_teacher_to_students_students(-1,sock_num);
			return -1;
		}
	}
	return 0;
}

/**
* @	�����ѧ�����������Ƿ��������粻�������ر�socket
*/
static void check_teacher_and_students_heart_thread(void *param)
{
	int sock_num =	*((int*)param);
	
   	pthread_detach(pthread_self());
	
   	while(!gblGetQuit())
   	{
		sleep(15);
		
		if(teacer_to_students_connect_status[sock_num])
		{
			teacer_to_students_connect_status[sock_num]=0;
			continue;
		}
		else
		{
			if(get_socket_teacher_to_students_students(sock_num) >= 0)
			{
				DEBUG(DL_DEBUG, "check_teacher_and_students_heart_thread socket < 0! heart failed!! shutdown...\n");
				shutdown(get_socket_teacher_to_students_students(sock_num),SHUT_RDWR);
				set_socket_teacher_to_students_students(-1,sock_num);
			}
			continue;
		}

	}
}

/**
* @	����������������Ƿ��������粻�������ر�socket
*/
static void check_teacher_and_blackboard_heart_thread(void *param)
{
	int sock_num =	*((int*)param);
	
   	pthread_detach(pthread_self());
	
   	while(!gblGetQuit())
   	{
		sleep(15);
		
		if(teacer_to_blackboard_connect_status[sock_num])
		{
			teacer_to_blackboard_connect_status[sock_num]=0;
			continue;
		}
		else
		{
			if(get_socket_teacher_to_blackboard(sock_num) >= 0)
			{
				DEBUG(DL_DEBUG, "check_teacher_and_blackboard_heart_thread socket < 0!\n");
				shutdown(get_socket_teacher_to_blackboard(sock_num),SHUT_RDWR);
				set_socket_teacher_to_blackboard(-1,sock_num);
			}
			continue;
		}

	}
}



/**
* @ ��������: teacher_connect_students_thread()
* @  ��������: ��ʦ����ѧ���������̣߳�connectѧ�������Լ��Ͽ���������������ѧ������������Ϣ
* @  �������: ��
* @  �������: ��
* @  ����ֵ:	   ����0�ɹ���-1ʧ��
*/
static void teacher_connect_students_thread(void *param)
{
	int 	stusock_num= *((int*)param);
	int 	student_socket 	= -1;
	char 	recvbuf[256] 	= {0};
	MSGHEAD msg				= {0};

	int 	recvLen			= 0;
	int		len				= 0;
	
	struct sockaddr_in student_addr;
	
	pthread_t rid;

	track_class_info_t 		recv_class_info;
	
	
	pthread_detach(pthread_self());
	
	set_socket_teacher_to_students_students(-1,stusock_num);
	
	student_socket = socket(AF_INET, SOCK_STREAM, 0);

	student_addr.sin_family 	= AF_INET;
	student_addr.sin_port 		= htons(TEACHER_CONNECT_STUDENTS_PORT);
	
	inet_pton(AF_INET, g_track_strategy_info.student_ip_info.ip,&student_addr.sin_addr);

	DEBUG(DL_WARNING,"teacher_connect_students_thread %d tcp tread create\n",stusock_num);

	DEBUG(DL_WARNING,"@@@@@@@@%s %d\n",g_track_strategy_info.student_ip_info.ip,TEACHER_CONNECT_STUDENTS_PORT);
	
	while(-1 == connect(student_socket, (struct sockaddr *)&student_addr, sizeof(struct sockaddr)))
	{
	    DEBUG(DL_ERROR,"teacher_connect_students_thread %d (ip:%s) tcp error\n",stusock_num, g_track_strategy_info.student_ip_info.ip);
		close(student_socket);
		
		student_socket = socket(AF_INET, SOCK_STREAM, 0);
		sleep(5);
	}
	
    pthread_create(&rid,NULL,(void *)check_teacher_and_students_heart_thread,(void *)&stusock_num);
	
	set_socket_teacher_to_students_students(student_socket,stusock_num);
	
	while(!gblGetQuit())
	{
		if(get_socket_teacher_to_students_students(stusock_num) < 0)
		{
			//��⵽��ѧ���������ӶϿ��󣬸�λ����ֵΪ0����ֹ��һֱ���ڴ���״̬
			SetStudentDetectResult(0, recv_class_info.cStudentCamCoord);
			DEBUG(DL_WARNING,"disreconnet student %d, reset the StudentDetectResult with 0!!!\n",stusock_num);
			do
			{
				close(student_socket);
				student_socket = socket(AF_INET, SOCK_STREAM, 0);
				DEBUG(DL_WARNING,"reconnect students %s %d, student_socket:%d\n",g_track_strategy_info.student_ip_info.ip,TEACHER_CONNECT_STUDENTS_PORT,student_socket);
				sleep(5);
			}
			while(-1 == connect(student_socket, (struct sockaddr *)&student_addr, sizeof(struct sockaddr)));
			
			set_socket_teacher_to_students_students(student_socket,stusock_num);

			//��������ѧ��������ѧ�������������Ϣ
			DEBUG(DL_WARNING,"reconnet student %d,send track info\n",stusock_num);
		}
		
		recvLen = recv(student_socket, &msg, sizeof(MSGHEAD), 0);
		if(recvLen < 1)
		{
			DEBUG(DL_ERROR,"teacher recv student %d data error\n",stusock_num);
			close(student_socket);
			set_socket_teacher_to_students_students(-1,stusock_num);
		}
		
        switch(msg.nMsg)
        {
			case MSG_TEACHER_HEART:
				 teacer_to_students_connect_status[stusock_num] = 1;
				 
				 //��ʦ����ѧ������������
				 len = send(student_socket,&msg,sizeof(MSGHEAD),0);
				 if(len < 1)
				 {
					DEBUG(DL_ERROR, "send HEART to student%d error\n",stusock_num);
					close(student_socket);
					set_socket_teacher_to_students_students(-1,stusock_num);
				 }
				 else
				 {
					 DEBUG(DL_DEBUG, "recv & send HEART to student%d!\n",stusock_num);
				 }
				break;
				//�õ�ѧ�����ϱ��Ŀ�����Ϣ
			case MSG_SEND_CLASSINFO:
				recvLen=recv(student_socket, recvbuf, ntohs(msg.nLen)-sizeof(MSGHEAD), 0);
				if(recvLen < 0)
				{
					DEBUG(DL_ERROR,"recv student MSG_SEND_CLASSINFO %d data error\n",stusock_num);
					close(student_socket);
					set_socket_teacher_to_students_students(-1,stusock_num);
					break;
				}
				memcpy(&recv_class_info,recvbuf,sizeof(track_class_info_t));
				//ֻ�����Զ�ģʽ�£��½��ܼ��Ľ����Ϣ
				if (g_track_param.dynamic_param.control_mode != 1 && g_track_param.dynamic_param.student_enable_state == 1)
				{
					SetStudentDetectResult(recv_class_info.iStudentStandupNum, recv_class_info.cStudentCamCoord);
				}
				break;
			case MSG_CAM_PRESET_COORD_INFO:
				recvLen=recv(student_socket, recvbuf, ntohs(msg.nLen)-sizeof(MSGHEAD), 0);
				if(recvLen < 0)
				{
					DEBUG(DL_ERROR,"recv student MSG_CAM_PRESET_COORD_INFO %d data error\n",stusock_num);
					close(student_socket);
					set_socket_teacher_to_students_students(-1,stusock_num);
					break;
				}
				cam_preset_teacher_and_student_req query_cam_preset_req;
				memcpy(&query_cam_preset_req, recvbuf, sizeof(cam_preset_teacher_and_student_req));
				DEBUG(DL_DEBUG, "rec student msg:%d, query_cam_preset_req.ePresetType:%d\n", msg.nMsg, query_cam_preset_req.ePresetType);

				query_cam_coord_info_by_student(stusock_num, query_cam_preset_req.ePresetType);
				break;
			case MSG_CALL_CAM_PRESET:
				recvLen=recv(student_socket, recvbuf, ntohs(msg.nLen)-sizeof(MSGHEAD), 0);
				if(recvLen < 0)
				{
					DEBUG(DL_ERROR,"recv student MSG_CAM_PRESET_COORD_INFO %d data error\n",stusock_num);
					close(student_socket);
					set_socket_teacher_to_students_students(-1,stusock_num);
					break;
				}
				call_cam_preset_teacher_and_student_req call_cam_preset_by_student_req;
				memcpy(&call_cam_preset_by_student_req, recvbuf, sizeof(call_cam_preset_teacher_and_student_req));
				DEBUG(DL_DEBUG, "rec student msg:%d.[MSG_CALL_CAM_PRESET]\n", msg.nMsg);

				cam_trans_teacher_and_encode_req cam_trans_req;
				cam_trans_req.eAuthor = STUDENT_DEBUG;
				cam_trans_req.cCamCoordInfo = call_cam_preset_by_student_req.cCamCoordInfo;
				int iRet = teacher_send_msg_buff_to_encode((unsigned char *)&cam_trans_req, sizeof(cam_trans_teacher_and_encode_req), MSG_CAM_TRANS);
				if (iRet< 0)
				{
					DEBUG(DL_DEBUG, "student call_preset_position--->teacher_send_msg_buff_to_encode error!\n");
				}
				break;
			default :
				//������Ϣ��ֱ�Ӷ���
				DEBUG(DL_ERROR,"recv student %d useless data,msg.nMsg=%d\n",stusock_num,msg.nMsg);
				if(ntohs(msg.nLen)-sizeof(MSGHEAD)>0)
				{
					recvLen=recv(student_socket, recvbuf, ntohs(msg.nLen)-sizeof(MSGHEAD), 0);
					if(recvLen < 0)
					{
						DEBUG(DL_ERROR,"recv student %d useless data error\n",stusock_num);
						close(student_socket);
						set_socket_teacher_to_students_students(-1,stusock_num);
						break;
					}
				}
				break;
       }
	}
}

/**
* @ ��������: teacher_connect_blackboard_thread()
* @  ��������: ��ʦ�������������̣߳�connect��������Լ��Ͽ��������������հ������������Ϣ
* @  �������: ��
* @  �������: ��
* @  ����ֵ:	   ����0�ɹ���-1ʧ��
*/
static int teacher_connect_blackboard_thread(void *pParam)
{
	int 	blackboardsock_num= *((int*)pParam);
	int 	blackboard_socket	= -1;
	char	recvbuf[256]	= {0};

	MSGHEAD msg 			= {0};

	int 	recv_value		= 0;
	int 	recvLen 		= 0;
	int 	len 			= 0;
	
	struct sockaddr_in blackboard_addr;
	
	pthread_t rid;
	
	pthread_detach(pthread_self());
	
	set_socket_teacher_to_blackboard(-1,blackboardsock_num);
	
	blackboard_socket = socket(AF_INET, SOCK_STREAM, 0);

	blackboard_addr.sin_family 	= AF_INET;
	blackboard_addr.sin_port	= htons(TEACHER_CONNECT_BLACKBOARD_PORT);
	
	inet_pton(AF_INET, g_track_strategy_info.blackboard_ip_info[blackboardsock_num].ip, &blackboard_addr.sin_addr);

	DEBUG(DL_WARNING,"teacher_connect_blackboard_thread %d tcp thread create",blackboardsock_num);

	DEBUG(DL_WARNING,"@@@@@@@@%s %d\n",g_track_strategy_info.blackboard_ip_info[blackboardsock_num].ip, TEACHER_CONNECT_BLACKBOARD_PORT);
	
	while(-1 == connect(blackboard_socket, (struct sockaddr *)&blackboard_addr, sizeof(struct sockaddr)))
	{
		DEBUG(DL_ERROR,"teacher_connect_blackboard_thread  %d tcp (%s) error\n",blackboardsock_num, g_track_strategy_info.blackboard_ip_info[blackboardsock_num].ip);
		close(blackboard_socket);
		
		blackboard_socket = socket(AF_INET, SOCK_STREAM, 0);
		sleep(5);
	}
	
	DEBUG(DL_WARNING,"teacher_connect_blackboard_thread after connect!\n");

	pthread_create(&rid,NULL,(void *)check_teacher_and_blackboard_heart_thread,(void *)&blackboardsock_num);
	
	set_socket_teacher_to_blackboard(blackboard_socket,blackboardsock_num);
	
	while(!gblGetQuit())
	{
		if(get_socket_teacher_to_blackboard(blackboardsock_num) < 0)
		{
			//��⵽�����������ӶϿ��󣬸�λ����ֵΪ0����ֹ��һֱ���ڴ���״̬
			SetBlackboardDetectResult(0, blackboardsock_num);
			DEBUG(DL_ERROR,"Disconnect with blackboard %d, reset BlackboardDetectResult with 0!!!! \n",blackboardsock_num);
			do
			{
				DEBUG(DL_WARNING,"reconnect blackboard %s %d\n", g_track_strategy_info.blackboard_ip_info[blackboardsock_num].ip, TEACHER_CONNECT_BLACKBOARD_PORT);
				close(blackboard_socket);
				blackboard_socket = socket(AF_INET, SOCK_STREAM, 0);
				sleep(5);
			}
			while(-1 == connect(blackboard_socket, (struct sockaddr *)&blackboard_addr, sizeof(struct sockaddr)));
			
			set_socket_teacher_to_blackboard(blackboard_socket,blackboardsock_num);
		}
		
		recvLen = recv(blackboard_socket, &msg, sizeof(MSGHEAD), 0);
		if(recvLen < 1)
		{
			DEBUG(DL_ERROR,"teacher recv blackboard %d data error\n",blackboardsock_num);
			close(blackboard_socket);
			set_socket_teacher_to_blackboard(-1,blackboardsock_num);
		}
		
		switch(msg.nMsg)
		{
			case MSG_TEACHER_HEART:
				 teacer_to_blackboard_connect_status[blackboardsock_num] = 1;
				 
				 //��ʦ����������������
				 len = send(blackboard_socket,&msg,sizeof(MSGHEAD),0);
				 if(len < 1)
				 {
					DEBUG(DL_ERROR, "send HEART to blackboard %d error\n",blackboardsock_num);
					close(blackboard_socket);
					set_socket_teacher_to_blackboard(-1,blackboardsock_num);
				 }
				 else
				 {
					 DEBUG(DL_ERROR, "recv & send HEART to blackboard %d!\n",blackboardsock_num);
				 }
				break;
				//�õ�������ϱ��İ�����Ϣ
			case MSG_TRACKAUTO:
			{
				recvLen = recv(blackboard_socket, recvbuf, ntohs(msg.nLen)-sizeof(MSGHEAD), 0);
				if(recvLen < 0)
				{
					DEBUG(DL_ERROR,"recv blackboard %d data error\n",blackboardsock_num);
					close(blackboard_socket);
					set_socket_teacher_to_blackboard(-1,blackboardsock_num);
					break;
				}
				else
				{
					recv_value = (int)recvbuf[0];
					DEBUG(DL_WARNING,"recv blackboard %d MSG_TRACKAUTO data,val=%d\n",blackboardsock_num, recv_value);
					//ֻ�����Զ�ģʽ�£��½��ܼ��Ľ����Ϣ
					if (g_track_param.dynamic_param.control_mode != 1)
					{
						SetBlackboardDetectResult(recv_value, blackboardsock_num);
					}
				}
				break;
			}		
			default :
				recvLen=recv(blackboard_socket, recvbuf, ntohs(msg.nLen)-sizeof(MSGHEAD), 0);
				if(recvLen < 0)
				{
					DEBUG(DL_ERROR,"recv blackboard %d data error\n",blackboardsock_num);
					close(blackboard_socket);
					set_socket_teacher_to_blackboard(-1,blackboardsock_num);
					break;
				}
				break;
	   }
	}
	return 0;
}




/**
*--------------------------------------------------------------------------------------------
* ������Ϊ���豸��Ϊ��ʦ�����ӱ������Ĳ���
* 
*
*--------------------------------------------------------------------------------------------
*/

/**
* @	��������: set_socket_teacher_to_encode_students()
* @  ��������: ������ʦ��������ʦ��������socket
* @  �������: socket -- Ҫ�����socket
* @  �������: ��
* @  ����ֵ:	   0��ʾ�ɹ�
*/
static void set_socket_teacher_to_encode_students(int socket)
{
	teacher_to_encode_socket = socket;
	return (void)0;
}

/**
* @	��������: get_socket_teacher_to_encode_students()
* @  ��������: ��ȡ��ʦ��������ʦ��������socket
* @  �������: ��
* @  �������: ��
* @  ����ֵ:	   ������ʦ�����ӱ��������ӵ�socket
*/
static int get_socket_teacher_to_encode_students()
{
	return teacher_to_encode_socket;
}

/**
* @	����ʦ����Ӧ�ı����������л�����	
*/
int teacher_send_msg_to_encode(unsigned char data)
{
	unsigned char sendData[256];
	int len,num;
	MSGHEAD msg;
	msg.nLen=1+sizeof(MSGHEAD);
	msg.nMsg=48;
	memcpy(sendData,&msg,sizeof(MSGHEAD));
	num=sizeof(MSGHEAD);
	sendData[num]=data;
	if(get_socket_teacher_to_encode_students()<0)
	{
		return -1;
	}
	else
	{
		DEBUG(DL_DEBUG,"teacher_send_msg_to_encode data = %d\n",data);
		len=send(get_socket_teacher_to_encode_students(),sendData,1+sizeof(MSGHEAD),0);
		if(len<1)
		{
			close(get_socket_teacher_to_encode_students());
			set_socket_teacher_to_encode_students(-1);
			return -1;
		}

	}
	return 0;
}

/**
* @	����ʦ����Ӧ�ı�����������̨�ƶ�λ��	
*/
int teacher_send_msg_buff_to_encode(unsigned char *data,int len,int cmd)
{
	unsigned char sendData[512] = {0};
	MSGHEAD msg;
	msg.nLen = len + sizeof(MSGHEAD);
	msg.nMsg = cmd;
	
	memcpy(sendData,&msg,sizeof(MSGHEAD));
	memcpy(sendData+sizeof(MSGHEAD),data,len);

	if(get_socket_teacher_to_encode_students()<0)
	{
		return -1;
	}
	else
	{
		DEBUG(DL_DEBUG,"teacher_send_msg_to_encode data = %d\n",data);
		len=send(get_socket_teacher_to_encode_students(),sendData,len+sizeof(MSGHEAD),0);
		if(len<1)
		{
			close(get_socket_teacher_to_encode_students());
			set_socket_teacher_to_encode_students(-1);
			return -1;
		}

	}
	return 0;
}



/**
* @  ��������: check_teacher_and_encode_heart_thread()
* @  ��������: 	�����������������Ƿ��������粻�������ر�socket
* @  �������: ��
* @  �������: ��
* @  ����ֵ:	   ������ʦ�����ӱ��������ӵ�socket
*/
static void check_teacher_and_encode_heart_thread(void *param)
{
	pthread_detach(pthread_self());

	while(!gblGetQuit())
	{
		sleep(15);
		if(teacer_to_encode_connect_status)
		{
			teacer_to_encode_connect_status=0;
			continue;
		}
		else
		{
			if(get_socket_teacher_to_encode_students() >= 0)
			{
				DEBUG(DL_DEBUG,"check_teacher_and_encode_heart_thread no heart!\n");
				shutdown(get_socket_teacher_to_encode_students(),SHUT_RDWR);
				set_socket_teacher_to_encode_students(-1);
			}
			continue;
		}

	}
}


/**
* @	��������: teacher_send_heart_to_encode_thread()
* @  ��������: ��ʦ������ʦ���������͵�����
* @  �������: ��
* @  �������: ��
* @  ����ֵ:	   ������ʦ�����ӱ��������ӵ�socket
*/
static void teacher_send_heart_to_encode_thread(void *param)
{
	int 	socket 	= -1;
	int 	len				= 0;
	MSGHEAD msg;
	
	pthread_detach(pthread_self());
	
	msg.nLen=sizeof(MSGHEAD);
	msg.nMsg=MSG_TEACHER_HEART;

	while(!gblGetQuit())
	{
		socket = get_socket_teacher_to_encode_students();
		if(socket < 0)
		{
			DEBUG(DL_ERROR,"socket to server not connect\n");
		}
		else
		{
			len = send(socket, &msg, sizeof(MSGHEAD), 0);
			if(len < 1)
			{
			    close(get_socket_teacher_to_encode_students());
				set_socket_teacher_to_encode_students(-1);
				DEBUG(DL_ERROR,"teacher_send_heart_to_encode_thread send heart error\n");
			}
			else
			{
				//DEBUG(DL_DEBUG, "teacher_send_heart_to_encode_thread send heart succ!\n");
			}
		}			
		sleep(4);
	}

}

void FarCtrlByEncode(int dsp, unsigned char *data, int len)
{
	int type, speed, addr = 1;
	int fd;
	int	stu_num = 0;

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
		case 12://�����Զ�����ģ����ֶ����Զ�ģʽ0�Զ�1�ֶ�
			DEBUG(DL_WARNING, "1111track mode speed = %d\n",speed);
			server_set_track_type(speed);
			/*
			data[8] = 2;
			for(stu_num=0;stu_num<g_track_strategy_info.stu_num;stu_num++)
			{
				teacher_send_msg_to_students(data,len,MSG_FARCTRL,stu_num);
				DEBUG(DL_WARNING, "server FarCtrlCamera send  MSG_FARCTRL to students = %d\n",stu_num);
			}
			*/
			break;
		default:
			DEBUG(DL_ERROR, "FarCtrlCamera() command Error\n");
			break;
	}

}

/**
* @	����ʦ����Ӧ�ı���������λ
*/
void SendStrategyNoToEncode()
{
	int iStrategyNo = g_track_strategy_info.strategy_no;
	int iRet = teacher_send_msg_buff_to_encode((unsigned char *)&iStrategyNo, sizeof(int), TRACK_TRATEGY);
	if (iRet < 0)
		DEBUG(DL_ERROR, "SendStrategyNoToEncode error!--->iStrategyNo:%d\n", iStrategyNo);
	else
		DEBUG(DL_ERROR, "SendStrategyNoToEncode success!--->iStrategyNo:%d\n", iStrategyNo);
	
	return;
}

/**
* @ ��������: teacher_recv_encode_msg_thread()
* @  ��������: ������ʦ���������͹����������̡߳�
* @  �������: ��
* @  �������: ��
* @  ����ֵ:	   0�ɹ�-1��ʧ��
*/
static void teacher_recv_encode_msg_thread(void *Param)
{
	unsigned char 	buf[256] 		= {0};
	int 			cliSocket 		= 0;
	MSGHEAD 		msg;
	int 			len 			= 0;
	int				addr 			= 0;
	int 			cmd_type		= 0;

    pthread_detach(pthread_self());

	SendStrategyNoToEncode();
	while(!gblGetQuit())
	{
		cliSocket = get_socket_teacher_to_encode_students();
		if(cliSocket < 0)
		{
		   DEBUG(DL_ERROR,"get_socket_teacher_to_encode_students < 0\n");
		   break;
		}
		
		len = recv(cliSocket,&msg,sizeof(MSGHEAD),0);
		if(len < 1)
		{
			DEBUG(DL_ERROR,"teacher_recv_encode_msg_thread recv len = %d\n",len);
			close(cliSocket);
			set_socket_teacher_to_encode_students(-1);
			break;
		}
		//DEBUG(DL_WARNING,"teacher_recv_encode msg type=%d\n",msg.nMsg);
        switch(msg.nMsg)
		{
			case MSG_TEACHER_HEART:
				teacer_to_encode_connect_status = 1;
				//DEBUG(DL_DEBUG, "teacher_recv_encode_msg_thread recv encode heart succ!\n");
				break;
			case MSG_CAM_PRESET_COORD_INFO:
				len=recv(cliSocket,buf,msg.nLen-sizeof(MSGHEAD),0);					
				if(len < 1)
				{
					DEBUG(DL_ERROR,"teacher_recv_encode_msg_thread MSG_CAM_PRESET_COORD_INFO recv len=%d\n",len);
					close(get_socket_teacher_to_encode_students());
					set_socket_teacher_to_encode_students(-1);
				}
				cam_preset_teacher_and_encode_ack query_cam_preset_ack;
				memcpy(&query_cam_preset_ack,buf,sizeof(cam_preset_teacher_and_encode_ack));
				DEBUG(DL_DEBUG,"query_cam_preset_ack.eAuthor:%d, ePresetType:%d, iStudentNo:%d\n",query_cam_preset_ack.eAuthor, query_cam_preset_ack.ePresetType, query_cam_preset_ack.iStudentNo);
				if (query_cam_preset_ack.eAuthor == STUDENT)
				{
					cam_preset_teacher_and_student_ack cam_preset_to_student_ack;
					cam_preset_to_student_ack.ePresetType = query_cam_preset_ack.ePresetType;
					cam_preset_to_student_ack.cCamCoordInfo = query_cam_preset_ack.cCamCoordInfo;
					
					teacher_send_msg_to_students((unsigned char *)&cam_preset_to_student_ack, sizeof(cam_preset_teacher_and_student_ack), MSG_CAM_PRESET_COORD_INFO, query_cam_preset_ack.iStudentNo);
				}
				else if (query_cam_preset_ack.eAuthor == TEACHER)
				{
					save_position_zoom_with_network_data(query_cam_preset_ack.ePresetType, (unsigned char *)&query_cam_preset_ack.cCamCoordInfo);
				}
				else
				{
					DEBUG(DL_WARNING,"teacher_recv_encode MSG_CAM_PRESET_COORD_INFO eAuthor unknown eAuthor=%d\n", query_cam_preset_ack.eAuthor);
				}
				
				break;
			case MSG_ROOM_TYPE_QUERY:
				len=recv(cliSocket,buf,msg.nLen-sizeof(MSGHEAD),0);					
				if(len < 1)
				{
					DEBUG(DL_ERROR,"teacher_recv_encode_msg_thread MSG_ROOM_TYPE_QUERY recv len=%d\n",len);
					close(get_socket_teacher_to_encode_students());
					set_socket_teacher_to_encode_students(-1);
				}
				room_type classroom_type;
				memcpy(&classroom_type,buf,sizeof(room_type));
				DEBUG(DL_DEBUG, "########recv MSG_ROOM_TYPE_QUERY msg. classroom_type.type = %d\n",classroom_type.type);
				server_set_classroom_study_mode(classroom_type.type);
				break;
			case MSG_STUDENT_ENABLE_QUERY:
				len=recv(cliSocket,buf,msg.nLen-sizeof(MSGHEAD),0);					
				if(len < 1)
				{
					DEBUG(DL_ERROR,"teacher_recv_encode_msg_thread MSG_STUDENT_ENABLE_QUERY recv len=%d\n",len);
					close(get_socket_teacher_to_encode_students());
					set_socket_teacher_to_encode_students(-1);
				}
				student_enable student_enable_state;
				memcpy(&student_enable_state, buf, sizeof(student_enable));
				DEBUG(DL_DEBUG, "########recv MSG_STUDENT_ENABLE_QUERY msg. student_enable_state.enable = %d\n", student_enable_state.enable);
				server_set_student_enable_state(student_enable_state.enable);
				break;
			case MSG_FARCTRL:
				len=recv(cliSocket,buf,msg.nLen-sizeof(MSGHEAD),0);					
				if(len < 1)
				{
					DEBUG(DL_ERROR,"teacher_recv_encode_msg_thread MSG_FARCTRL recv len=%d\n",len);
					close(get_socket_teacher_to_encode_students());
					set_socket_teacher_to_encode_students(-1);
				}
				
				if(msg.nLen-sizeof(MSGHEAD) < 12)
				{
					addr =1;
				}
				else
				{
					addr=(int)(buf[8] | buf[9] << 8 | buf[10] << 16 | buf[11] << 24);
				}

				DEBUG(DL_WARNING, "########recv MSG_FARCTRL addr = %d\n",addr);
				if((addr == 1) || (addr == 0))
				{
					FarCtrlByEncode(0,buf,msg.nLen-sizeof(MSGHEAD));
				}				
				break;
			case MSG_QUERY_CUR_CAM_TYPE:
				//��ѯ��ǰ���л���ͷ
				cmd_type = gStrategyInfo.sLastCmdType;
				if (gStrategyInfo.sLastCmdType == SWITCH_TEATHER)
				{
					cmd_type = SWITCH_TEACHER_PANORAMA;
				}

				if (gStrategyInfo.sLastCmdType == SWITCH_STUDENTS)
				{
					cmd_type = SWITCH_STUDENTS_PANORAMA;
				}
				
				tracer_info_t trace_info;
				trace_info.chid = 0;
				trace_info.layout = cmd_type;
				teacher_send_msg_buff_to_encode((unsigned char *)&trace_info, sizeof(tracer_info_t), MSG_TRACKAUTO);
				break;
			default:
				DEBUG(DL_WARNING,"teacher_recv_encode useless msg type=%d\n",msg.nMsg);
				if(msg.nLen-sizeof(MSGHEAD)>0)
				{
					len=recv(cliSocket,buf,msg.nLen-sizeof(MSGHEAD),0);					
					if(len < 0)
					{
						DEBUG(DL_ERROR,"teacher_recv_encode useless msg len=%d\n",len);
						close(get_socket_teacher_to_encode_students());
						set_socket_teacher_to_encode_students(-1);
					}
				}
				break;
		}
        
	}
}



/**
* @ 	��������: teacher_accept_encode_connect_thread()
* @  ��������: ����ʦ����Ӧ�������ļ����ˣ������Ƿ��б����������ӣ��еĻ��򴴽�
* @  �������: data -- Ҫ���͵�����
* @  �������: ��
* @  ����ֵ:	   0Ϊ�ɹ�; -1Ϊʧ��
*/
static void teacher_accept_encode_connect_thread(void *param)
{
	pthread_t tcp_cmd;
	
	int clientSocket = -1;
	int listenSocket = -1;

	int result = 0;
	int	len = 0;
	int	nSize = 1;
	int	nLen = 0;
	
	struct sockaddr_in clientAddr, srvAddr;
	
	int opt		=	1;
	
	pthread_detach(pthread_self());
	
	set_socket_teacher_to_encode_students(-1);
	
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bzero(&srvAddr, sizeof(srvAddr));
	srvAddr.sin_family 		= AF_INET;
	srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	srvAddr.sin_port 		= htons(TEACHER_MONITOR_PORT);
	
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if(bind(listenSocket, (struct sockaddr *)&srvAddr, sizeof(srvAddr)) < 0)
	{
		DEBUG(DL_ERROR,"bind:");
	}

	if(listen(listenSocket, 10) < 0)
	{
		DEBUG(DL_ERROR,"listen:");
	}
	
	while(!gblGetQuit())
	{
		
		len = sizeof(clientAddr);
		clientSocket = accept(listenSocket, (struct sockaddr *)&clientAddr,(unsigned int *)&len);
		
		if(get_socket_teacher_to_encode_students() >= 0)
		{
			DEBUG(DL_WARNING,"@@@@@@@@@@@@@@@@@encode connet you %d,get_socket_teacher_to_encode_students>=0\n",clientSocket);
			close(clientSocket);
		}
		else
		{
			DEBUG(DL_WARNING,"@@@@@@@@@@@@@@@@@encode connet you %d,get_socket_teacher_to_encode_students<0\n",clientSocket);
			set_socket_teacher_to_encode_students(clientSocket);
			if((setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR, (void *)&nSize,
				sizeof(nSize))) == -1) 
			{
				DEBUG(DL_ERROR,"setsockopt failed");
			}
			
			nSize 	= 0;
			nLen 	= sizeof(nSize);
			
			result 	= getsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, &nSize , (DWORD *)&nLen);
	       	if(result==1)
			{
			  	DEBUG(DL_WARNING,"getsockopt()  socket:%d  result:%d\n", clientSocket, result);
			}

	       	nSize = 1;
			if(setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, &nSize , sizeof(nSize))) 
			{
				DEBUG(DL_ERROR,"set IPPROTOT error\n");
			}
			result = pthread_create(&tcp_cmd, NULL, (void *)teacher_recv_encode_msg_thread, NULL);
		}
	}

}




void InitStrategyInfo()
{
	//��ʦȫ��(������һ��Ϊ��ʦ��д)�ı���ʱ��
	gStrategyTimeInfo.iTeacherPanoramaWithNextTeacherFeatureKeepTime = g_track_strategy_info.vga_keep_time; 		
	//��ʦȫ��(������һ��Ϊѧ����д)�ı���ʱ��
	gStrategyTimeInfo.iTeacherPanoramaWithNextStudentFeatureKeepTime = g_track_strategy_info.students_panorama_switch_near_time;  		
	//��ʦȫ��(������һ��Ϊѧ��ȫ��)�ı���ʱ��
	gStrategyTimeInfo.iTeacherPanoramaWithNextStudentPanoramaKeepTime = g_track_strategy_info.students_near_keep_time;		

	//��ʦȫ����С�ı���ʱ��
	gStrategyTimeInfo.iTeacherPanoramaMinKeepTime = g_track_strategy_info.students_down_time; 		
	//��ʦ��д��С�ı���ʱ��
	gStrategyTimeInfo.iTeacherFeatureMinKeepTime = g_track_strategy_info.teacher_blackboard_time1; 		

	//ѧ��ȫ����С�ı���ʱ��
	gStrategyTimeInfo.iStudentPanoramaMinKeepTime = g_track_strategy_info.teacher_leave_blackboard_time1; 	
	//ѧ��ȫ�����ı���ʱ��
	gStrategyTimeInfo.iStudentPanoramaMaxKeepTime = g_track_strategy_info.teacher_blackboard_time2; 		
	//ѧ����д��С�ı���ʱ��
	gStrategyTimeInfo.iStudentFeatureMinKeepTime = g_track_strategy_info.teacher_leave_blackboard_time2; 		
	//ѧ����д���ı���ʱ��
	gStrategyTimeInfo.iStudentFeatureMaxKeepTime = g_track_strategy_info.teacher_keep_panorama_time; 		
	//���龵ͷ��С����ʱ��
	gStrategyTimeInfo.iBlackboardMinKeepTime = g_track_strategy_info.teacher_switch_students_delay_time; 

	static int iIndex = 0;
	if (iIndex % 100 == 0)
	{
		iIndex++;
		DEBUG(DL_DEBUG, "[STRATEGY]--->TeacherPanorama:(%d, %d, %d), Teacher:(%d, %d), Student:(%d, %d, %d, %d), Blackboard:%d\n", 
			gStrategyTimeInfo.iTeacherPanoramaWithNextTeacherFeatureKeepTime, 
			gStrategyTimeInfo.iTeacherPanoramaWithNextStudentFeatureKeepTime, 
			gStrategyTimeInfo.iTeacherPanoramaWithNextStudentPanoramaKeepTime,
			gStrategyTimeInfo.iTeacherPanoramaMinKeepTime,
			gStrategyTimeInfo.iTeacherFeatureMinKeepTime,
			gStrategyTimeInfo.iStudentPanoramaMinKeepTime,
			gStrategyTimeInfo.iStudentPanoramaMaxKeepTime,
			gStrategyTimeInfo.iStudentFeatureMinKeepTime,
			gStrategyTimeInfo.iStudentFeatureMaxKeepTime,
			gStrategyTimeInfo.iBlackboardMinKeepTime
			);
	}
		
	
}

/*get time */
unsigned long long getCurrentTime(void)
{
	struct timeval tv;
	struct timezone tz;
	unsigned long long ultime;

	gettimeofday(&tv , &tz);
	ultime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return (ultime);
}


/**
* @ ��ȡʱ����λΪ��
*/
unsigned long long GetDuringTime(unsigned long long lStartTime)
{
	return getCurrentTime() - lStartTime;
}


/**
* @ ��ȡ��ʦĿ��ĸ������������ж���ʦ�Ƿ��½�̨����ѧ���ľ�ͷ�лؽ�̨��ͷ�����
*/
int GetTeacherNum()
{
	if (gStrategyInputInfo.iTeatcherNum == 0)
	{
		return 0;
	}

	if (gStrategyInfo.sLastCmdType == SWITCH_STUDENTS || gStrategyInfo.sLastCmdType == SWITCH_STUDENTS_PANORAMA)
	{//��һ����ͷ��ѧ����д��ѧ��ȫ��
		if (GetDuringTime(gStrategyInputInfo.iTeacherTargetRefreshTime) > g_track_strategy_info.teacher_detect_heart_timeout)
		{//������ʱ����ʱ��Ϊ��ʦ�Ѿ��½�̨������0
			return 0;
		}
	}
	
	return gStrategyInputInfo.iTeatcherNum;
}

/**
* @��⵱ǰ��ͷ�Ƿ񱻸����ȼ���Ŀ�괥����ϣ����鴥�� > ��ѧ������ > ��ѧ������ > ����ʦ���� > ����ʦ�����������ȼ��ɱ������ȼ����
*/
bool IsLastCamBreakedByHeighTarget()
{
	bool bRet = false;
	switch(gStrategyInfo.cLastCamType)
	{
		case TEACHER_FEATURE:
			if(gStrategyInputInfo.iTeatcherNum >= 2 //����ʦ����
				|| gStrategyInputInfo.iTeatcherNum == 0 //��ʦ��ʧ
				|| gStrategyInputInfo.iStudentStandupNum > 0	//��ѧ�����ѧ������
				|| gStrategyInputInfo.iBlackboardTriggerFlag1 != 0 //���鴥��
				|| gStrategyInputInfo.iBlackboardTriggerFlag2 != 0)
			{
				bRet = true;
			}
			break;
		case TEACHER_PANORAMA:
			if(gStrategyInputInfo.iTeatcherNum == 0 //��ʦ��ʧ
				|| gStrategyInputInfo.iStudentStandupNum > 0	//��ѧ�����ѧ������
				|| gStrategyInputInfo.iBlackboardTriggerFlag1 != 0 //���鴥��
				|| gStrategyInputInfo.iBlackboardTriggerFlag2 != 0)
			{
				bRet = true;
			}
			break;
		case STUDENT_FEATURE:
			if(gStrategyInputInfo.iStudentStandupNum == 0 //ѧ����ʧ
				|| gStrategyInputInfo.iStudentStandupNum > 1	//��ѧ������
				|| gStrategyInputInfo.iBlackboardTriggerFlag1 != 0 //���鴥��
				|| gStrategyInputInfo.iBlackboardTriggerFlag2 != 0)
			{
				bRet = true;
			}
			break;
		case STUDENT_PANORAMA:
			if(gStrategyInputInfo.iStudentStandupNum == 0 //ѧ����ʧ
				|| gStrategyInputInfo.iBlackboardTriggerFlag1 != 0 //���鴥��
				|| gStrategyInputInfo.iBlackboardTriggerFlag2 != 0)
			{
				bRet = true;
			}
			break;
		case BLACK_BOARD1:
			//���鲻�ܱ����
			break;
		default:
			break;
	}
	return bRet;
}


/**
* @��⵱ǰ��ͷ�Ŀ��л�״̬,��ͷ�л������ȼ������鴥�� > ��ѧ������ > ��ѧ������ > ����ʦ���� > ����ʦ�����������ȼ��ɱ������ȼ����
*/
void CheckSwitchState()
{
	switch(gStrategyInfo.cLastCamType)
	{
		case TEACHER_FEATURE:
			if (GetDuringTime(gStrategyInfo.iTeacherFeatureStartTime) >= gStrategyTimeInfo.iTeacherFeatureMinKeepTime * 1000
				|| IsLastCamBreakedByHeighTarget() == true)
			{
				gStrategyInfo.sSwitchState = SWITCH_FREE;
			}
			else
			{
				gStrategyInfo.sSwitchState = SWITCH_FORBID;
			}
			break;
		case STUDENT_FEATURE:
			if (GetDuringTime(gStrategyInfo.iStudentFeatureStartTime) > gStrategyTimeInfo.iStudentFeatureMinKeepTime * 1000
				|| IsLastCamBreakedByHeighTarget() == true)
			{
				gStrategyInfo.sSwitchState = SWITCH_FREE;
			}
			else
			{
				gStrategyInfo.sSwitchState = SWITCH_FORBID;
			}
			break;
		case BLACK_BOARD1:
			if (GetDuringTime(gStrategyInfo.iBlockboardStartTime) >= gStrategyTimeInfo.iBlackboardMinKeepTime * 1000
				|| IsLastCamBreakedByHeighTarget() == true)
			{
				gStrategyInfo.sSwitchState = SWITCH_FREE;
			}
			else
			{
				gStrategyInfo.sSwitchState = SWITCH_FORBID;
			}
			break;
		case STUDENT_PANORAMA:
			if (GetDuringTime(gStrategyInfo.iStudentPanoramaStartTime) > gStrategyTimeInfo.iStudentPanoramaMinKeepTime * 1000
				|| IsLastCamBreakedByHeighTarget() == true)
			{
				gStrategyInfo.sSwitchState = SWITCH_FREE;
			}
			else
			{
				gStrategyInfo.sSwitchState = SWITCH_FORBID;
			}
			break;
		case TEACHER_PANORAMA:
			if (GetDuringTime(gStrategyInfo.iTeacherPanoramaStartTime) >= gStrategyTimeInfo.iTeacherPanoramaMinKeepTime * 1000
				|| IsLastCamBreakedByHeighTarget() == true)
			{
				gStrategyInfo.sSwitchState = SWITCH_FREE;
			}
			else
			{
				gStrategyInfo.sSwitchState = SWITCH_FORBID;
			}
			break;
		default:
			gStrategyInfo.sSwitchState = SWITCH_FREE;
			break;
	}
}

void ResetStartTime()
{
	switch(gStrategyInfo.cLastCamType)
	{
		case TEACHER_FEATURE:
			gStrategyInfo.iTeacherFeatureStartTime = getCurrentTime();
			break;
		case STUDENT_FEATURE:
			gStrategyInfo.iStudentFeatureStartTime = getCurrentTime();
			break;
		case BLACK_BOARD1:
			gStrategyInfo.iBlockboardStartTime= getCurrentTime();
			break;
		case STUDENT_PANORAMA:
			gStrategyInfo.iStudentPanoramaStartTime= getCurrentTime();
			break;
		case TEACHER_PANORAMA:
			gStrategyInfo.iTeacherPanoramaStartTime= getCurrentTime();
			break;
		default:
			break;
	}
}

void DebugText()
{
	char sOutCmdTYpeArray[256] = {0};
	char sLastCmdTYpeArray[256] = {0};
	char cCamTransStateArray[256] = {0};
	char cSwitchStateArray[256] = {0};
	switch(gStrategyInfo.sOutCmdType)
	{
		case SWITCH_TEATHER:
			sprintf(sOutCmdTYpeArray, "TEATHER");
			break;
		case SWITCH_STUDENTS:
			sprintf(sOutCmdTYpeArray, "STUDENTS");
			break;
		case SWITCH_STUDENTS_PANORAMA:
			sprintf(sOutCmdTYpeArray, "STUDENTS_PANORAMA");
			break;
		case SWITCH_TEACHER_PANORAMA:
			sprintf(sOutCmdTYpeArray, "TEACHER_PANORAMA");
			break;
		case SWITCH_BLACKBOARD1:
			sprintf(sOutCmdTYpeArray, "BLACKBOARD1");
			break;
		case SWITCH_BLACKBOARD2:
			sprintf(sOutCmdTYpeArray, "BLACKBOARD2");
			break;
		default:
			sprintf(sOutCmdTYpeArray, "N/A");
			break;
	}

	switch(gStrategyInfo.sLastCmdType)
	{
		case SWITCH_TEATHER:
			sprintf(sLastCmdTYpeArray, "TEATHER");
			break;
		case SWITCH_STUDENTS:
			sprintf(sLastCmdTYpeArray, "STUDENTS");
			break;
		case SWITCH_STUDENTS_PANORAMA:
			sprintf(sLastCmdTYpeArray, "STUDENTS_PANORAMA");
			break;
		case SWITCH_TEACHER_PANORAMA:
			sprintf(sLastCmdTYpeArray, "TEACHER_PANORAMA");
			break;
		case SWITCH_BLACKBOARD1:
			sprintf(sLastCmdTYpeArray, "BLACKBOARD1");
			break;
		case SWITCH_BLACKBOARD2:
			sprintf(sLastCmdTYpeArray, "BLACKBOARD2");
			break;
		default:
			sprintf(sLastCmdTYpeArray, "N/A");
			break;
	}

	switch(gStrategyInfo.cCamTransState)
	{
		case TEACHER_CAM_TRANS:
			sprintf(cCamTransStateArray, "TEACHER_CAM_TRANS");
			break;
		case STUDENT_CAM_TRANS:
			sprintf(cCamTransStateArray, "STUDENT_CAM_TRANS");
			break;
		case NO_CAM_TRANS:
			sprintf(cCamTransStateArray, "NO_CAM_TRANS");
			break;
		default:
			break;
	}

	switch(gStrategyInfo.sSwitchState)
	{
		case SWITCH_FORBID:
			sprintf(cSwitchStateArray, "SWITCH_FORBID");
			break;
		case SWITCH_FREE:
			sprintf(cSwitchStateArray, "SWITCH_FREE");
			break;
		case SWITCH_FORCE:
			sprintf(cSwitchStateArray, "SWITCH_FORCE");
			break;
		default:
			break;
	}

	
	DEBUG(DL_DEBUG, "[#####################STRATEGY#################]--->sOutCmdType:%s, cCamTransState:%s, sLastCmdType:%s, SwitchState:%s  ##################\n", 
		sOutCmdTYpeArray, cCamTransStateArray, sLastCmdTYpeArray, cSwitchStateArray);
	DEBUG(DL_DEBUG, "[##########################################################]\n");
	DEBUG(DL_DEBUG, "[###                                                    ###]\n");
	DEBUG(DL_DEBUG, "[###    %s-----------%s----[StrategyNo:%d, StudyMode:%d]             \n", sOutCmdTYpeArray, cCamTransStateArray, g_track_strategy_info.strategy_no, g_track_param.dynamic_param.classroom_study_mode);
	DEBUG(DL_DEBUG, "[###                                                    ###]\n");
	DEBUG(DL_DEBUG, "[##########################################################]\n");
}

void HandleOutCmdTypeWithListenMode()
{
	if (gStrategyInfo.sOutCmdType == gStrategyInfo.sLastCmdType && gStrategyInfo.cCamTransState == NO_CAM_TRANS)
	{//����Ҫ���л�
		return;
	}

	DebugText();
	tracer_info_t trace_info;
	trace_info.chid = 0;
	trace_info.layout = gStrategyInfo.sOutCmdType;
	
	teacher_send_msg_buff_to_encode((unsigned char *)&trace_info, sizeof(tracer_info_t), MSG_TRACKAUTO);
	
	//teacher_send_msg_to_encode(gStrategyInfo.sOutCmdType);
	gStrategyInfo.sLastCmdType = gStrategyInfo.sOutCmdType;
	ResetStartTime();

	if (gStrategyInfo.cCamTransState == STUDENT_CAM_TRANS)
	{
		gStrategyInputInfo.bIsNeedWaitStudentCamTrans = false;
	}
}

void HandleOutCmdType()
{
	if (g_track_param.dynamic_param.student_enable_state == 0)
	{//����ѧ������
		if (gStrategyInfo.sOutCmdType == SWITCH_STUDENTS || gStrategyInfo.sOutCmdType == SWITCH_STUDENTS_PANORAMA)
		{
			gStrategyInfo.sOutCmdType = SWITCH_TEACHER_PANORAMA;
			gStrategyInfo.cLastCamType = TEACHER_PANORAMA;
		}
	}
	
	if (gStrategyInfo.sOutCmdType == gStrategyInfo.sLastCmdType && gStrategyInfo.cCamTransState == NO_CAM_TRANS)
	{//����Ҫ���л�
		return;
	}

	DebugText();
	tracer_info_t trace_info;
	trace_info.chid = 0;
	trace_info.layout = gStrategyInfo.sOutCmdType;
	
	//�������λ�ĳ��������⴦�����е�ѧ����д�ľ�ͷ�ĳ�ѧ��ȫ���ģ���Ŀ��Ʋ��Ա�����5��λһ��
	if (g_track_strategy_info.strategy_no == STRATEGY_3_4)
	{
		if (gStrategyInfo.sOutCmdType == SWITCH_STUDENTS)
		{
			trace_info.layout = SWITCH_STUDENTS_PANORAMA;
		}
	}

	teacher_send_msg_buff_to_encode((unsigned char *)&trace_info, sizeof(tracer_info_t), MSG_TRACKAUTO);
	
	gStrategyInfo.sLastCmdType = gStrategyInfo.sOutCmdType;
	ResetStartTime();

	//�ȴ�ѧ�����ƶ������
	if (gStrategyInfo.cCamTransState == STUDENT_CAM_TRANS)
	{
		gStrategyInputInfo.bIsNeedWaitStudentCamTrans = false;
	}
	
	if (gStrategyInfo.cCamTransState == TEACHER_CAM_TRANS)
	{
		gStrategyInputInfo.bIsNeedWaitTeacherCamTrans = false;
	}
}

//����ģʽ�µ��л����ԣ�ֻ�а��顢ѧ��ȫ����ѧ����д���־�ͷ
void StrategyWithListenMode(void *pParam)
{
	InitStrategyInfo();
	gStrategyInfo.cCamTransState = NO_CAM_TRANS;
	//���жϵ�ǰ��ͷ�Ƿ������л�
	CheckSwitchState();
	if (gStrategyInfo.sSwitchState == SWITCH_FORBID)
	{//��ֹ�л�
		return;
	}
	else
	{//���������л�
		//�ȼ������ź�
		if (gStrategyInputInfo.iBlackboardTriggerFlag1 != 0 || gStrategyInputInfo.iBlackboardTriggerFlag2 != 0)
		{
			gStrategyInfo.cLastCamType = BLACK_BOARD1;
			if (gStrategyInputInfo.iBlackboardTriggerFlag1 != 0 && gStrategyInputInfo.iBlackboardTriggerFlag2 == 0)
			{//����1����������2δ����
				gStrategyInfo.sOutCmdType = SWITCH_BLACKBOARD1;
			}

			if (gStrategyInputInfo.iBlackboardTriggerFlag1 == 0 && gStrategyInputInfo.iBlackboardTriggerFlag2 != 0)
			{//����2����������1δ����
				gStrategyInfo.sOutCmdType = SWITCH_BLACKBOARD2;
			}

			if (gStrategyInputInfo.iBlackboardTriggerFlag1 != 0 && gStrategyInputInfo.iBlackboardTriggerFlag2 != 0)
			{//����1������2������
				if (gStrategyInfo.sLastCmdType == SWITCH_BLACKBOARD1 || gStrategyInfo.sLastCmdType == SWITCH_BLACKBOARD2)
				{
					gStrategyInfo.sOutCmdType = gStrategyInfo.sLastCmdType;
				}
				else
				{
					gStrategyInfo.sOutCmdType = SWITCH_BLACKBOARD1;
				}
			}
		}
		else
		{
			//�鿴ѧ�����Ƿ񴥷�
			if (gStrategyInputInfo.iStudentStandupNum > 0)
			{
				if (gStrategyInputInfo.iStudentStandupNum == 1)
				{//�������������
					//����ѧ��ȫ����Ϊ���ɣ�����̨�����������ѧ����д
					if ((gStrategyInfo.sLastCmdType == SWITCH_STUDENTS_PANORAMA && gStrategyInputInfo.bIsNeedWaitStudentCamTrans == false)
						|| (gStrategyInfo.sLastCmdType == SWITCH_STUDENTS && gStrategyInputInfo.bIsNeedWaitStudentCamTrans == false))
					{//��ǰ�ǹ��ɾ�ͷ����̨�����Ҳ�Ѿ��ƶ���λ�˻�ǰ�Ѿ���ѧ����д��ѧ��Ŀ��λ��δ�����仯
						gStrategyInfo.sOutCmdType = SWITCH_STUDENTS;
						gStrategyInfo.cLastCamType = STUDENT_FEATURE;
					}
					else
					{
						gStrategyInfo.sOutCmdType = SWITCH_STUDENTS_PANORAMA;
						gStrategyInfo.cCamTransState = STUDENT_CAM_TRANS;
						gStrategyInfo.cLastCamType = STUDENT_PANORAMA;
					}
				}
				else
				{//�������������
					//��ѧ��ȫ��
					gStrategyInfo.sOutCmdType = SWITCH_STUDENTS_PANORAMA;
					gStrategyInfo.cLastCamType = STUDENT_PANORAMA;
				}
				
			}
			else
			{//ѧ����δ��⵽ѧ��վ��
				gStrategyInfo.sOutCmdType = SWITCH_STUDENTS_PANORAMA;
				gStrategyInfo.cLastCamType = STUDENT_PANORAMA;
			}
		}
	}

	HandleOutCmdTypeWithListenMode();
	
}

void StrategyWithCams_5(void *pParam)
{
	gStrategyInfo.cCamTransState = NO_CAM_TRANS;
	InitStrategyInfo();

	//��ͷ�л������ȼ������鴥�� > ��ѧ������ > ��ѧ������ > ����ʦ���� > ����ʦ����
	//�����ȼ����Ա������ȼ���ϣ������ʱ�����Ǳ���Ͼ�ͷ����ͱ���ʱ��
	
	//���жϵ�ǰ��ͷ�Ƿ������л�
	CheckSwitchState();
	//DEBUG(DL_DEBUG, "[STRATEGY]--->gStrategyInfo.sLastCmdType:%d, gStrategyInfo.sSwitchState:%d\n", gStrategyInfo.sLastCmdType, gStrategyInfo.sSwitchState);
	if (gStrategyInfo.sSwitchState == SWITCH_FORBID)
	{//��ֹ�л�
		return;
	}
	else
	{//���������л�
		//�ȼ������ź�
		if (gStrategyInputInfo.iBlackboardTriggerFlag1 != 0 || gStrategyInputInfo.iBlackboardTriggerFlag2 != 0)
		{
			gStrategyInfo.cLastCamType = BLACK_BOARD1;
			if (gStrategyInputInfo.iBlackboardTriggerFlag1 != 0 && gStrategyInputInfo.iBlackboardTriggerFlag2 == 0)
			{//����1����������2δ����
				gStrategyInfo.sOutCmdType = SWITCH_BLACKBOARD1;
			}

			if (gStrategyInputInfo.iBlackboardTriggerFlag1 == 0 && gStrategyInputInfo.iBlackboardTriggerFlag2 != 0)
			{//����2����������1δ����
				gStrategyInfo.sOutCmdType = SWITCH_BLACKBOARD2;
			}

			if (gStrategyInputInfo.iBlackboardTriggerFlag1 != 0 && gStrategyInputInfo.iBlackboardTriggerFlag2 != 0)
			{//����1������2������
				if (gStrategyInfo.sLastCmdType == SWITCH_BLACKBOARD1 || gStrategyInfo.sLastCmdType == SWITCH_BLACKBOARD2)
				{
					gStrategyInfo.sOutCmdType = gStrategyInfo.sLastCmdType;
				}
				else
				{
					gStrategyInfo.sOutCmdType = SWITCH_BLACKBOARD1;
				}
			}
		}
		else
		{//������δ�������鿴ѧ�����Ƿ񴥷�
			if (gStrategyInputInfo.iStudentStandupNum > 0 && GetTeacherNum() > 0)
			{
				if (gStrategyInputInfo.iStudentStandupNum == 1)
				{//�������������
					//����ѧ��ȫ����Ϊ���ɣ�����ѧ����д
					if ((gStrategyInfo.sLastCmdType == SWITCH_STUDENTS_PANORAMA && gStrategyInputInfo.bIsNeedWaitStudentCamTrans == false)
						|| (gStrategyInfo.sLastCmdType == SWITCH_STUDENTS && gStrategyInputInfo.bIsNeedWaitStudentCamTrans == false))
					{//��ǰ�ǹ��ɾ�ͷ��ѧ��Ŀ��λ��δ�����仯 �� ��ǰ�Ѿ���ѧ����д��ѧ��Ŀ��λ��δ�����仯
						gStrategyInfo.sOutCmdType = SWITCH_STUDENTS;
						gStrategyInfo.cLastCamType = STUDENT_FEATURE;
					}
					else
					{//�й��ɾ�ͷ
						gStrategyInfo.sOutCmdType = SWITCH_STUDENTS_PANORAMA;
						gStrategyInfo.cLastCamType = STUDENT_PANORAMA;
						gStrategyInfo.cCamTransState = STUDENT_CAM_TRANS;
					}
				}
				else
				{//�������������
					//��ѧ��ȫ��
					gStrategyInfo.sOutCmdType = SWITCH_STUDENTS_PANORAMA;
					gStrategyInfo.cLastCamType = STUDENT_PANORAMA;
				}
			}
			else
			{//ѧ����δ��⵽ѧ��վ�����鿴��ʦ����״̬
				if (GetTeacherNum() > 0)
				{//��̨��Ŀ��
					if (GetTeacherNum() == 1)
					{//���˳���
						if (gStrategyInputInfo.bIsNeedWaitTeacherCamTrans == false && gStrategyInputInfo.iTeacherMoveState == 0
							&& (gStrategyInfo.sLastCmdType == SWITCH_TEACHER_PANORAMA || gStrategyInfo.sLastCmdType == SWITCH_TEATHER))
						{
							gStrategyInfo.sOutCmdType = SWITCH_TEATHER;
							gStrategyInfo.cLastCamType = TEACHER_FEATURE;
						}
						else
						{
							gStrategyInfo.sOutCmdType = SWITCH_TEACHER_PANORAMA;
							gStrategyInfo.cLastCamType = TEACHER_PANORAMA;
							gStrategyInfo.cCamTransState = TEACHER_CAM_TRANS;
						}
					}
					else
					{//���˳���
						gStrategyInfo.sOutCmdType = SWITCH_TEACHER_PANORAMA;
						gStrategyInfo.cLastCamType = TEACHER_PANORAMA;
					}
				}
				else
				{//��̨û��Ŀ�꣬��ѧ��ȫ��
					gStrategyInfo.sOutCmdType = SWITCH_STUDENTS_PANORAMA;
					gStrategyInfo.cLastCamType = STUDENT_PANORAMA;
				}
			}
			
		}
	}

	HandleOutCmdType();
}

/**
* @  ��������: strategy_thread()
* @  ��������: �л����ԣ���Ҫ�Ǵ����ж�Ӧ������ʦ����Ӧ�����������л��Ǹ���λ������
* @  �������: pParam -- �̴߳�������
* @  �������: ��
* @  ����ֵ:	   0�ɹ�-1��ʧ��
*/
static int strategy_thread(void *pParam)
{
	pthread_detach(pthread_self());
	bool bIsListenMode = false;

    while(!gblGetQuit())
	{
		if (g_track_param.dynamic_param.classroom_study_mode == ROOM_CLIENT)
    	{//����ģʽ
    		if (bIsListenMode == false)
    		{
				gStrategyInfo.cLastCamType = CAM_TYPE_NUM;
				gStrategyInfo.sLastCmdType = SWITCH_NUM;
				gStrategyInfo.cCamTransState = NO_CAM_TRANS;
			}
			bIsListenMode = true;
			StrategyWithListenMode(pParam);
		}
		else
		{//����ģʽ
			if (bIsListenMode == true)
    		{
				gStrategyInfo.cLastCamType = CAM_TYPE_NUM;
				gStrategyInfo.sLastCmdType = SWITCH_NUM;
				gStrategyInfo.cCamTransState = NO_CAM_TRANS;
			}
			
			bIsListenMode = false;
			StrategyWithCams_5(pParam);
		}
		
		usleep(20000);//��λ΢�룬20����
    }
	return 0;
}


/**
* @ 	��������: teacher_net_create_thread()
* @  ��������: ������ʦ���������ѧ����ͨѶ���߳�
* @  �������: data -- Ҫ���͵�����
* @  �������: ��
* @  ����ֵ:	   0Ϊ�ɹ�; -1Ϊʧ��
*/
void teacher_net_create_thread(void *pParam)
{
	int stu_id[STUCONNECT_NUM_MAX];
	int black_id[BLACKCONNECT_NUM_MAX];
	pthread_t rtmpid[5] ;
	int result;
	int sock_num=0;
	//char	text[FILE_NAME_LEN]			= {0};
	//char	param_name[FILE_NAME_LEN] 	= {0};
	pthread_t rtmpid_stu[STUCONNECT_NUM_MAX] ;
	pthread_t rtmpid_black[BLACKCONNECT_NUM_MAX] ;
	
	pthread_detach(pthread_self());

	//��ȡ���Ե�������Ϣ
	get_strategy_info_form_file();

	result = pthread_create(&rtmpid[1], NULL, (void *)teacher_accept_encode_connect_thread, (void *)NULL);
	if(result < 0)
	{ 
		DEBUG(DL_ERROR, "create TEACHER_TRACETCP() failed\n");
	}


	
	result = pthread_create(&rtmpid[2], NULL, (void *)teacher_send_heart_to_encode_thread, (void *)NULL);
	if(result < 0)
	{
		DEBUG(DL_ERROR, "create teacher_send_heart_to_encode_thread() failed\n");
	}
	

	result = pthread_create(&rtmpid[3], NULL, (void *)check_teacher_and_encode_heart_thread, (void *)NULL);
	if(result < 0)
	{
		DEBUG(DL_ERROR, "create serverTraceHeart() failed\n");
	}
			
	//�������ѧ����
	//ip��ȡȫ������֤web�ϱ���ȷ
	/*
	for(sock_num=0;sock_num<STUCONNECT_NUM_MAX;sock_num++)
	{
		strcpy(text, IPADDR);
		sprintf(param_name, "%s%d",text, sock_num);
		
	    result =  ConfigGetKey("studenttracer.ini", "studenttracer", param_name, STUDENTTRACE_IP[sock_num]);
		if(result != 0) 
		{
			DEBUG(DL_ERROR, "Get student%d ipaddr failed\n",sock_num);
		}
		else
		{
			DEBUG(DL_WARNING, "Get student%d ipaddr = %s\n",sock_num,STUDENTTRACE_IP[sock_num]);
		}
	}
	*/
	for(sock_num=0;sock_num<g_track_strategy_info.stu_num;sock_num++)
	{
		stu_id[sock_num]=sock_num;
		
		result = pthread_create(&rtmpid_stu[sock_num], NULL, (void *)teacher_connect_students_thread, (void *)&(stu_id[sock_num]));
		if(result < 0)
		{
			DEBUG(DL_ERROR, "create STUDENT_TRACETCP[%d] failed!\n",sock_num);
		}
	}
	//���������߳�
	//ip��ȡȫ������֤web�ϱ���ȷ
	/*
	for(sock_num=0;sock_num<BLACKCONNECT_NUM_MAX;sock_num++)
	{
		strcpy(text, IPADDR);
		sprintf(param_name, "%s%d",text, sock_num);
		
	    result =  ConfigGetKey("blackboradtracer.ini", "blackboradtracer", param_name, BLACKBOARDTRACE_IP[sock_num]);
		if(result != 0) 
		{
			DEBUG(DL_ERROR, "Get blackboradtracer%d ipaddr failed\n",sock_num);
		}
		else
		{
			DEBUG(DL_WARNING, "Get blackboradtracer%d ipaddr = %s\n",sock_num,BLACKBOARDTRACE_IP[sock_num]);
		}
	}
	*/
	
	//Ĭ�Ͽ������·��
	for(sock_num=0; sock_num<g_track_strategy_info.blackboard_num; sock_num++)
	{
		black_id[sock_num]=sock_num;
		
		result = pthread_create(&rtmpid_black[sock_num], NULL, (void *)teacher_connect_blackboard_thread, (void *)&(black_id[sock_num]));
		if(result < 0)
		{
			DEBUG(DL_ERROR, "create BLACKBOARD_TRACETCP[%d] failed!\n",sock_num);
		}
	}
	
	result = pthread_create(&rtmpid[4], NULL, (void *)strategy_thread, (void *)NULL);
	if(result < 0)
	{
		DEBUG(DL_ERROR, "create strategy_thread() failed\n");
	}	
	
	while(!gblGetQuit())
	{
		sleep(5);
	}
}

int query_cam_coord_info_by_teacher(int _preset_position_type)
{
	cam_preset_teacher_and_encode_req query_req;
	query_req.eAuthor = TEACHER;
	query_req.iStudentNo = 0;
	query_req.ePresetType = _preset_position_type;
	DEBUG(DL_ERROR, "query_cam_coord_info_by_teacher-->query_req.eAuthor:%d, query_req.ePresetType:%d, query_req.iStudentNo:%d\n", query_req.eAuthor, query_req.ePresetType, query_req.iStudentNo);
	
	return teacher_send_msg_buff_to_encode((unsigned char *)&query_req, sizeof(cam_preset_teacher_and_encode_req), MSG_CAM_PRESET_COORD_INFO);
}

int query_cam_coord_info_by_student(int iStudentNo, int _preset_position_type)
{
	cam_preset_teacher_and_encode_req query_req;
	query_req.eAuthor = STUDENT;
	query_req.iStudentNo = iStudentNo;
	query_req.ePresetType = _preset_position_type;
	DEBUG(DL_ERROR, "query_cam_coord_info_by_teacher-->query_req.eAuthor:%d, query_req.ePresetType:%d, query_req.iStudentNo:%d\n", query_req.eAuthor, query_req.ePresetType, query_req.iStudentNo);
	return teacher_send_msg_buff_to_encode((unsigned char *)&query_req, sizeof(cam_preset_teacher_and_encode_req), MSG_CAM_PRESET_COORD_INFO);
}

/**
* @ ��ȡ��ʦ��¼��֮�������״̬
*/
int get_teacher_to_encode_connect_state()
{
	return get_socket_teacher_to_encode_students() < 0 ? 0 : 1;
}

/**
* @ ��ȡ��ʦ��ѧ��֮�������״̬
*/
int get_teacher_to_student_connect_state(int index)
{
	return get_socket_teacher_to_students_students(index) < 0 ? 0 : 1;
}

/**
* @ ��ȡ��ʦ�����֮�������״̬
*/
int get_teacher_to_blackboard_connect_state(int index)
{
	return get_socket_teacher_to_blackboard(index) < 0 ? 0 : 1;
}

int strategy_start()
{
	if (!gblGetQuit())
	{
		DEBUG(DL_ERROR, "strategy has been started!\n");
		return 0;
	}

	gblSetQuit(FALSE);
	pthread_t p_id;
	int result = pthread_create(&p_id, NULL, (void *)teacher_net_create_thread, (void *)NULL);
	
	if(result < 0) {
	   DEBUG(DL_ERROR, "create studentTrace() failed\n");
	   gblSetQuit(TRUE);
	   return -1;
	}
	return 0;
}


void strategy_reset()
{
	gStrategyInfo.cLastCamType = CAM_TYPE_NUM;
	gStrategyInfo.sLastCmdType = SWITCH_NUM;
	gStrategyInfo.cCamTransState = NO_CAM_TRANS;
	
	gblSetQuit(TRUE);
}



