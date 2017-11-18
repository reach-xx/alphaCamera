#include "rh_broadcast.h"

#include <stdio.h>  
#include <unistd.h> 

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h> 
#include <linux/tcp.h>



#define	MAGIC_MAX_NUM	(16)
#define	HOST_MAX_NUM	(127)
#define	IP_MAX_NUM	(16)
#define	SERIAL_NO_MAX_NUM	(127)
#define MAGIC_TEXT	"reach123"

typedef enum BroadcastMsgType_e
{
	BROADCAST_MSG_TYPE_GET_IP_REQ,
	BROADCAST_MSG_TYPE_GET_IP_ACK,
	BROADCAST_MSG_TYPE_NUM
}BroadcastMsgType;


//��ȡIP����Ϣ�㲥
typedef struct BroadcastMsg_s
{
	BroadcastMsgType bMsgType;	//��Ϣ����
	char cMagic[MAGIC_MAX_NUM]; //ħ����
	char cHost[HOST_MAX_NUM];
	char cIp[IP_MAX_NUM];
	char cSerialNo[SERIAL_NO_MAX_NUM];
}BroadcastMsg_t;


AuthorType g_AuthorType;
char g_Ip[IP_MAX_NUM];

static void BroadcastServerThread(void *param)
{
	printf("BroadcastServerThread....g_AuthorType:%d, g_Ip:%s\n", g_AuthorType, g_Ip);
	pthread_detach(pthread_self());

	int sock;
	struct sockaddr_in from, a;

	int optval = 1;

	//Ȼ��ֵ��������ַ��һ���������׽��֣�һ�������������ϵĹ㲥��ַ������Ϣ��  
	a.sin_family = AF_INET;
	a.sin_addr.s_addr = htonl(INADDR_ANY);
	a.sin_port = htons(9000);

	from.sin_family = AF_INET;
	from.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	from.sin_port = htons(9090);

	int fromlength = sizeof(struct sockaddr_in);
	//��UDP��ʼ���׽���  
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	// ���ø��׽���Ϊ�㲥���ͣ�  
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
	// �Ѹ��׽��ְ���һ������ĵ�ַ��  
	bind(sock, (struct sockaddr *)&a, sizeof(a));

	BroadcastMsg_t msg_req;
	BroadcastMsg_t msg_ack;
	int iRet = 0;
	while (1)
	{//�ӹ㲥��ַ������Ϣ��ע�������󶨵ĵ�ַ�ͽ�����Ϣ�ĵ�ַ�ǲ�һ���� 
		memset(&msg_req, 0, sizeof(BroadcastMsg_t));
		memset(&msg_ack, 0, sizeof(BroadcastMsg_t));
		
		iRet = recvfrom(sock, (char *)&msg_req, sizeof(BroadcastMsg_t), 0, (struct sockaddr *)&from, (socklen_t *)&fromlength);
		if (-1 != iRet)
		{
			printf("magic:%s; msg_type:%d\n", msg_req.cMagic, msg_req.bMsgType);
			//У��MAGIC
			iRet = strncmp(msg_req.cMagic, MAGIC_TEXT, MAGIC_MAX_NUM);
			if (iRet != 0)
			{
				continue;
			}

			if (msg_req.bMsgType == BROADCAST_MSG_TYPE_GET_IP_REQ)
			{
				msg_ack.bMsgType = BROADCAST_MSG_TYPE_GET_IP_ACK;
				snprintf(msg_ack.cMagic, MAGIC_MAX_NUM, "%s", MAGIC_TEXT);
				if (g_AuthorType == AUTHOR_TYPE_STUDENT)
				{
					snprintf(msg_ack.cHost, HOST_MAX_NUM, "%s", "Student");
				}
				else
				{
					snprintf(msg_ack.cHost, HOST_MAX_NUM, "%s", "Teacher");
				}
				
				snprintf(msg_ack.cIp, IP_MAX_NUM, "%s", g_Ip);

				//ͨ���㲥�ķ�ʽ������Ӧ��Ϣ
				from.sin_family = AF_INET;
				from.sin_addr.s_addr = INADDR_BROADCAST;
				from.sin_port = htons(9090);
				fromlength = sizeof(from);
				sendto(sock, (const char *)&msg_ack, sizeof(BroadcastMsg_t), 0, (struct sockaddr *)&from, fromlength);
			}
		}
	}

}

int StartBroadcastServer(AuthorType aType, char *pIp)
{
	g_AuthorType = aType;
	memcpy(g_Ip, pIp, IP_MAX_NUM);
	
	pthread_t rtmpid = 0;
	int result = pthread_create(&rtmpid, NULL, (void *)BroadcastServerThread, (void *)NULL);
	if (result < 0)
	{
		return -1;
	}
	return 0;
}

void ResetIpInfo(char *pIp)
{
	memcpy(g_Ip, pIp, IP_MAX_NUM);
}


