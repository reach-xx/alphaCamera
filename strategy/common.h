#ifndef COMMON__H
#define COMMON__H

#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include "auto_track.h"

#define  MSG_ADDCLIENT      	1
#define  MSG_DELETECLIENT   	2
#define  MSG_CONNECTSUCC    	3
#define  MSG_PASSWORD_ERR   	4
#define  MSG_MAXCLIENT_ERR  	5
#define  MSG_AUDIODATA			6
#define  MSG_SCREENDATA     	7
#define  MSG_HEARTBEAT      	8
#define  MSG_PASSWORD       	9
#define  MSG_DATAINFO       	10
#define  MSG_REQ_I          	11
#define  MSG_SET_FRAMERATE  	12
#define  MSG_PPT_INDEX  		15

#define MSG_SYSPARAMS			16
#define MSG_SETPARAMS			17
#define MSG_RESTARTSYS			18

#define MSG_UpdataFile			19
#define MSG_SAVEPARAMS			20
#define MSG_UpdataFile_FAILS		21
#define MSG_UpdataFile_SUCC			22

#define MSG_DECODE_STATUS			23
#define MSG_DECODE_DISCONNECT		24
#define MSG_DECODE_CONNECT			25

#define MSG_UPDATEFILE_DECODE 		26
#define MSG_UPDATEFILE_ROOT 		27
#define MSG_UPDATEFILE_WEB 			28

#define MSG_MODE_CODE				29
#define MSG_MODE_DECODE				30

#define MSG_ADD_TEXT				33

#define MSG_MOUT          			40
#define MSG_SENDFLAG    			41
#define MSG_FARCTRL      			42

#define MSG_VGA_ADJUST				43

#define MSG_QUALITYVALUE			46 //���ø������������ģʽ(0:�� 1:�� 2:��) 
#define MSG_FARCTRLEX      			47
#define MSG_TRACKAUTO				48 //�����Զ�����ģ����Զ��ֶ�ģʽ
#define MSG_UPLOADIMG 			    49//�ϴ�logoͼƬ
#define MSG_GET_LOGOINFO 			50//��ȡlogoͼƬ��Ϣ
#define MSG_SET_LOGOINFO 			51//����logoͼƬ��Ϣ 

#define	MSG_SET_TRACK_TYPE			110	//��ʦ���ٻ���ѧ�����ٻ�ͨ�ŵ���Ϣ����
#define MSG_TEACHER_HEART         111  //��ʦ������
/**
* @ ���͵�ǰ֡������Ϣ
*/
#define MSG_SEND_CLASSINFO		112	

/**
* @ ���͸�����������Ϣ��ѧ����
*/
#define	MSG_TRIGGER_TYPE	113

/**
* @ ���ٻ��л�����,���͸�ѧ����
*/
#define MSG_TRACK_SWITCH_TYPE		114	
/**
* @��λ 
*/
#define MSG_STRATEGY_NO		115	
/**
* @��ѯ��Ϣ 
*/
#define MSG_TRACK_TURN_INFO		116	

/**
* @��ʦ��ʧ����л�ģʽ
*/
#define MSG_LOST_MODE		117	


//��̨�ƶ�����Ϣ
#define MSG_CAM_TRANS	118

//��ѯ��̨�������Ԥ��λ������Ϣ
#define MSG_CAM_PRESET_COORD_INFO	120

//������̨Ԥ��λ
#define MSG_CALL_CAM_PRESET	121

//���õ�ǰ�л���ͷ
#define MSG_QUERY_CUR_CAM_TYPE	122

//���ý������ͣ�����������ģʽ
#define MSG_ROOM_TYPE_QUERY  123

//�����Ƿ�����ѧ��
#define MSG_STUDENT_ENABLE_QUERY  124


//ϵͳ�Ļ�λ��
#define TRACK_TRATEGY	0xa5


#define TRUE		(1)
#define FALSE		(0)

typedef bool 		BOOL;
typedef unsigned short WORD ;
typedef unsigned int   DWORD ;
typedef unsigned char  BYTE;


/*message header*/
typedef struct __HDB_MSGHEAD {
	/*
	##  length for htons change
	## include length of structure
	## and real data
	*/
	WORD	nLen;
	WORD	nVer;							//version
	BYTE	nMsg;							//message type
	BYTE	szTemp[3];						//reserve
} MSGHEAD;

typedef enum __author_type_e
{
	TEACHER = 0,
	STUDENT = 1,
	TEACHER_DEBUG = 2,
	STUDENT_DEBUG = 3,
}author_type_e;

typedef enum
{
	ROOM_MODE_START = 0,
	ROOM_STANDER= 1,        //��׼
	ROOM_MAIN = 2,             //����
	ROOM_CLIENT = 3,          //����
	ROOM_MODE_END
}ROOM_TYPE;

typedef struct _room_type
{
	int type;
}room_type;

typedef struct _student_enable
{
	int enable;  // 1 ���� 0 �ر�
}student_enable;



//�ƶ���̨λ���������Ϣ�ṹ,��¼��������������
typedef struct __cam_trans_teacher_and_encode_req
{
	author_type_e	eAuthor;			//ѧ��������Ĳ�ѯ������ʦ������Ĳ�ѯ
	cam_preset_position_info_t	cCamCoordInfo;
}cam_trans_teacher_and_encode_req;


//��ѯ��̨Ԥ��λ�������Ϣ�ṹ,��¼��������������
typedef struct __cam_preset_teacher_and_encode_req
{
	author_type_e	eAuthor;			//ѧ��������Ĳ�ѯ������ʦ������Ĳ�ѯ
	int iStudentNo;						//ѧ�������
	int ePresetType;					//Ԥ��λ����
	
}cam_preset_teacher_and_encode_req;


//��ѯ��̨Ԥ��λ����Ӧ����¼��������������
typedef struct __cam_preset_teacher_and_encode_ack
{
	author_type_e	eAuthor;
	int iStudentNo;
	int ePresetType;
	cam_preset_position_info_t	cCamCoordInfo;

}cam_preset_teacher_and_encode_ack;


//��ѯ��̨Ԥ��λ�������Ϣ�ṹ,��ѧ����������
typedef struct __cam_preset_teacher_and_student_req
{
	int ePresetType;
	
}cam_preset_teacher_and_student_req;


//��ѯ��̨Ԥ��λ����Ӧ����ѧ����������
typedef struct __cam_preset_teacher_and_student_ack
{
	int ePresetType;
	cam_preset_position_info_t	cCamCoordInfo;

}cam_preset_teacher_and_student_ack;



//������̨Ԥ��λ�������Ϣ�ṹ,��ѧ����������
typedef struct __call_cam_preset_teacher_and_student_req
{
	cam_preset_position_info_t	cCamCoordInfo;
	
}call_cam_preset_teacher_and_student_req;


#define	TEACHER_MONITOR_PORT					(5555)			//��ʦ�������˿�
#define TEACHER_CONNECT_STUDENTS_PORT 			(5556)			//��ʦ������ѧ�����Ķ˿�
#define TEACHER_CONNECT_BLACKBOARD_PORT 		(5559)			//��ʦ�����Ӱ�����Ķ˿�

/**
* @ ����ѧ�������������
*/
#define		STUCONNECT_NUM_MAX		(2)		
/**
* @ ���Ӱ�������������
*/
#define		BLACKCONNECT_NUM_MAX		(2)


//���͸�¼�����������л��������Ϣ���ͣ�����Ϣ���ͽ�ֹ�����޸ģ���ֹͨ�Ż���
typedef enum _switch_cmd_type
{
	SWITCH_TEATHER				= 1,		//�л���ʦ��������ֲ���
	SWITCH_STUDENTS				= 3,		//�л�ѧ����������ֲ���
	SWITCH_VGA					= 255,		//�л�VGA����
	SWITCH_2STUDENTS_1			= 4,		//�л�ѧ�����������ѧ����ʱ�ĵڶ�·ѧ��
	SWITCH_BLACKBOARD1			= 4,		//�л��������������
	SWITCH_TEACHER_PANORAMA_3CAMER =4,		//����λ����ʦȫ��
	SWITCH_STUDENTS_PANORAMA 	= 5,		//�л�ѧ��ȫ�������
	SWITCH_STUDENTS_2			= 5,		//�л�ѧ�������ֻ�в����ڶ�·ѧ��
	SWITCH_TEACHER_PANORAMA		= 6,		//�л���̨�����ȫ��
	SWITCH_BLACKBOARD2			= 8,//6,		//�л�����2���������
	SWITCH_NUM
}switch_cmd_type_e;

typedef struct  tracer_info_ {
 int chid;
 int layout;
} tracer_info_t;



/**
* @ ������Ϣͳ�ƽṹ��,��ѧ�������͹���
*/
typedef struct _track_class_info
{
	unsigned short	nStandupPos[4];			//ǰ����  �ң�  �������ҷֱ��Ӧ��������
	unsigned short	nUpToPlatformTimes;			//ѧ����̨�ܴ���
	unsigned short	nDownToStudentsAreaTimes;//��̨�ܴ���

	int iStudentStandupNum;					//����ѧ����������0��ʾδ��⵽ѧ��������1��ʾ��⵽һ��ѧ��������2��ʾ��⵽���Ŀ������
	cam_preset_position_info_t cStudentCamCoord;		//ѧ������̨���������
}track_class_info_t;


typedef enum {
    DL_NONE, //=0
    DL_ERROR,  //=1
    DL_WARNING,  //=2
    DL_FLOW,	//=3
    DL_DEBUG,	//=4
    DL_ALL, 	//all
} EDebugLevle;


//#define DEBUG(x,fmt,arg...) do {}while(0)
//#define DEBUG(x,fmt...) do {}while(0)
#define DEBUG(x,fmt...)   \
    do {\
		time_t timep; \
	    struct tm *p_tm;  \
	    timep = time(NULL);  \
	    p_tm = gmtime(&timep); /*��ȡGMTʱ��*/  \
        printf("[%d:%d:%d-%s]-%d: ", p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec, __FUNCTION__, __LINE__);\
        printf(fmt);\
    }while(0)

/* Global data structure */
typedef struct GlobalData {
	int             quit;                /* Global quit flag */
	int             frames;              /* Video frame counter */
	int             videoBytesProcessed; /* Video bytes processed counter */
	int             soundBytesProcessed; /* Sound bytes processed counter */
	int             samplingFrequency;   /* Sound sampling frequency */
	int             imageWidth;          /* Width of clip */
	int             imageHeight;         /* Height of clip */
	pthread_mutex_t mutex;               /* Mutex to protect the global data */
} GlobalData;

/* Global data */
GlobalData gbl;

/* Functions to protect the global data */
static inline int gblGetQuit(void)
{
	int quit;

	pthread_mutex_lock(&gbl.mutex);
	quit = gbl.quit;
	pthread_mutex_unlock(&gbl.mutex);

	return quit;
}

static inline void gblSetQuit(BOOL bQuitState)
{
	pthread_mutex_lock(&gbl.mutex);
	gbl.quit = bQuitState;
	pthread_mutex_unlock(&gbl.mutex);
}


#endif
