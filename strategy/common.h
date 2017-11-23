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

#define MSG_QUALITYVALUE			46 //设置高清编码器码流模式(0:低 1:中 2:高) 
#define MSG_FARCTRLEX      			47
#define MSG_TRACKAUTO				48 //设置自动跟踪模块的自动手动模式
#define MSG_UPLOADIMG 			    49//上传logo图片
#define MSG_GET_LOGOINFO 			50//获取logo图片信息
#define MSG_SET_LOGOINFO 			51//设置logo图片信息 

#define	MSG_SET_TRACK_TYPE			110	//老师跟踪机和学生跟踪机通信的消息类型
#define MSG_TEACHER_HEART         111  //教师机心跳
/**
* @ 发送当前帧课堂信息
*/
#define MSG_SEND_CLASSINFO		112	

/**
* @ 发送辅助机触发信息给学生机
*/
#define	MSG_TRIGGER_TYPE	113

/**
* @ 跟踪或切换类型,发送给学生机
*/
#define MSG_TRACK_SWITCH_TYPE		114	
/**
* @机位 
*/
#define MSG_STRATEGY_NO		115	
/**
* @轮询信息 
*/
#define MSG_TRACK_TURN_INFO		116	

/**
* @老师丢失后的切换模式
*/
#define MSG_LOST_MODE		117	


//云台移动的消息
#define MSG_CAM_TRANS	118

//查询云台摄像机的预置位坐标信息
#define MSG_CAM_PRESET_COORD_INFO	120

//调用云台预置位
#define MSG_CALL_CAM_PRESET	121

//调用当前切换镜头
#define MSG_QUERY_CUR_CAM_TYPE	122

//设置教室类型，主讲或听话模式
#define MSG_ROOM_TYPE_QUERY  123

//设置是否屏蔽学生
#define MSG_STUDENT_ENABLE_QUERY  124


//系统的机位数
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
	ROOM_STANDER= 1,        //标准
	ROOM_MAIN = 2,             //主讲
	ROOM_CLIENT = 3,          //听讲
	ROOM_MODE_END
}ROOM_TYPE;

typedef struct _room_type
{
	int type;
}room_type;

typedef struct _student_enable
{
	int enable;  // 1 开启 0 关闭
}student_enable;



//移动云台位置请求的消息结构,与录播服务器交互用
typedef struct __cam_trans_teacher_and_encode_req
{
	author_type_e	eAuthor;			//学生机发起的查询还是老师机发起的查询
	cam_preset_position_info_t	cCamCoordInfo;
}cam_trans_teacher_and_encode_req;


//查询云台预置位请求的消息结构,与录播服务器交互用
typedef struct __cam_preset_teacher_and_encode_req
{
	author_type_e	eAuthor;			//学生机发起的查询还是老师机发起的查询
	int iStudentNo;						//学生机编号
	int ePresetType;					//预置位类型
	
}cam_preset_teacher_and_encode_req;


//查询云台预置位的响应，与录播服务器交互用
typedef struct __cam_preset_teacher_and_encode_ack
{
	author_type_e	eAuthor;
	int iStudentNo;
	int ePresetType;
	cam_preset_position_info_t	cCamCoordInfo;

}cam_preset_teacher_and_encode_ack;


//查询云台预置位请求的消息结构,与学生机交互用
typedef struct __cam_preset_teacher_and_student_req
{
	int ePresetType;
	
}cam_preset_teacher_and_student_req;


//查询云台预置位的响应，与学生机交互用
typedef struct __cam_preset_teacher_and_student_ack
{
	int ePresetType;
	cam_preset_position_info_t	cCamCoordInfo;

}cam_preset_teacher_and_student_ack;



//调用云台预置位请求的消息结构,与学生机交互用
typedef struct __call_cam_preset_teacher_and_student_req
{
	cam_preset_position_info_t	cCamCoordInfo;
	
}call_cam_preset_teacher_and_student_req;


#define	TEACHER_MONITOR_PORT					(5555)			//老师机监听端口
#define TEACHER_CONNECT_STUDENTS_PORT 			(5556)			//老师机连接学生机的端口
#define TEACHER_CONNECT_BLACKBOARD_PORT 		(5559)			//老师机连接板书机的端口

/**
* @ 链接学生的最大连接数
*/
#define		STUCONNECT_NUM_MAX		(2)		
/**
* @ 链接板书的最大连接数
*/
#define		BLACKCONNECT_NUM_MAX		(2)


//发送给录播服务器的切换命令的消息类型，此消息类型禁止随意修改，防止通信混乱
typedef enum _switch_cmd_type
{
	SWITCH_TEATHER				= 1,		//切换老师机命令，保持不动
	SWITCH_STUDENTS				= 3,		//切换学生机命令，保持不动
	SWITCH_VGA					= 255,		//切换VGA命令
	SWITCH_2STUDENTS_1			= 4,		//切换学生机命令，两个学生机时的第二路学生
	SWITCH_BLACKBOARD1			= 4,		//切换板书摄像机命令
	SWITCH_TEACHER_PANORAMA_3CAMER =4,		//三机位的老师全景
	SWITCH_STUDENTS_PANORAMA 	= 5,		//切换学生全景摄像机
	SWITCH_STUDENTS_2			= 5,		//切换学生机命令，只切不跟第二路学生
	SWITCH_TEACHER_PANORAMA		= 6,		//切换讲台摄像机全景
	SWITCH_BLACKBOARD2			= 8,//6,		//切换板书2摄像机命令
	SWITCH_NUM
}switch_cmd_type_e;

typedef struct  tracer_info_ {
 int chid;
 int layout;
} tracer_info_t;



/**
* @ 课堂信息统计结构体,由学生机发送过来
*/
typedef struct _track_class_info
{
	unsigned short	nStandupPos[4];			//前面左  右，  后面左右分别对应起立次数
	unsigned short	nUpToPlatformTimes;			//学生上台总次数
	unsigned short	nDownToStudentsAreaTimes;//下台总次数

	int iStudentStandupNum;					//起立学生的人数；0表示未检测到学生起立；1表示检测到一个学生起立；2表示检测到多个目标起立
	cam_preset_position_info_t cStudentCamCoord;		//学生机云台摄像机坐标
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
	    p_tm = gmtime(&timep); /*获取GMT时间*/  \
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
