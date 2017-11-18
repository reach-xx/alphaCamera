#ifndef __NETCOM_HEADER__
#define __NETCOM_HEADER__

#include <netinet/in.h>
#include "common.h"

#define DSP_NUM        	4

#ifndef SDCARD
//#define BOX_VER					"V2.5.3"//ENC110版本
#define BOX_VER					"2.0.6"//ENC110版本

#else
#define BOX_VER					"V2.5.0"//sdcard版本

#endif


#define KERNEL_VER				"1.1.0"

#define CONFIG_NAME				"config.dat"
#define CONFIG_NAME2			"config.dat_1"
#define PROTOCOL_NAME			"append.ini"
#define LOGOFILENAME			"logo.ini"
#define REMOTE_NAME				"remote.ini"

#define HV_TABLE_D_NAME			"digital.ini"
#define HV_TABLE_A_NAME			"analog.ini"
#define DTYPECONFIG_NAME		"dtypeconfig.ini"
#define TXTOSDFILE				"txtosd.dat"
#define OSDTRANSFILE			"osdtrans.ini"
#define DSP1			0

#define TRUE		1
#define FALSE		0

#define PORT_COM1		0
#define PORT_COM2		1
#define PORT_COM3		2


#define FPGA_UPDATE				0x55AA
#define FPGA_NO_UPDATE			0xAA55

#define IIS_FPGA_UPDATE()		(FPGA_UPDATE == g_FPGA_update)
#define SET_FPGA_UPDATE(update)		(g_FPGA_update = update)
#define INITWIDTH		720
#define INITHEIGHT		480

#define PORT_LISTEN_DSP1		3100
//#define PORT_LISTEN_DSP2		3110
//#define PORT_LISTEN_DSP3		3120
//#define PORT_LISTEN_DSP4		3130
#define PORT_LISTEN_MULTICAST   4000


#define H264_HEADER_LEN 		0x40
#define H264_IFRAME_SKIPNUM 	0x1E

/*
#############LCD order  start#################
*/
#define FLAG_CLR			0x01
#define FLAG_RESET			0x02
#define FLAG_WAYLEFT		0x05
#define FLAG_WAYRIGHT		0x06
#define FLAG_HIDECURSOR		0x0C
#define FLAG_SHOWCURSOR		0x0F
#define FLAG_MOVELEFT		0x10
#define FLAG_MOVERIGHT		0x14
#define FLAG_SCREENLEFT		0x18
#define FLAG_SCREENRIGHT	0x1C
#define FLAG_SETCURSOR		0x80
#define FLAG_LIGHTSHOW		0x28
#define FLAG_LIGHTHIDE		0x2C

#define FLAG_UP				0xB8
#define FLAG_DOWN			0xE8
#define FLAG_ENTER			0xD8
#define FLAG_ESCAPE			0x78

#define FLAG_NULL			0x1
#define FLAG_CENTER			0x2
#define FLAG_LEFT			0x4
#define FLAG_RIGHT			0x8
#define FLAG_NEWLINE		0x10

#define ONE_LINE_POS		0x00
#define TWO_LINE_POS		0x40

#define MAX_DISPLAY_CHAR	16

/*
#############LCD order  end#################
*/


#define ADTS 					0x53544441
#define AAC						0x43414130
#define PCM						0x4d435030

#define MIC_INPUT				1
#define LINE_INPUT				0

#define MAX_CLIENT				6
#define MAX_PACKET				14600

#define MAX_FAR_CTRL_NUM		5

#define LOGIN_USER				0
#define LOGIN_ADMIN				1

#define INVALID_SOCKET 			-1

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

//查询云台摄像机的预置位坐标信息
#define MSG_CAM_PRESET_COORD_INFO 120

#define MSG_SET_TRACK_SWITCH_TYPE 114 //老师机发送过来的跟踪和切换类型
/**
* @机位 
*/
#define MSG_STRATEGY_NO		115	

/**
* @轮询信息 
*/
#define MSG_TRACK_TURN_INFO		116	
/**
* @ 学生辅助机上报的跟踪类型
*/
#define MSG_TRIGGER_TYPE		113		

//调用云台预置位
#define MSG_CALL_CAM_PRESET 121

//#ifdef SUPPORT_TRACK
#define	MSG_SET_TRACK_PARAM	0xA3	//教师跟踪参数设置
//#endif

#define MSG_GET_VIDEOPARAM			0x70
#define MSG_SET_VIDEOPARAM			0x71
#define MSG_GET_AUDIOPARAM			0x72
#define MSG_SET_AUDIOPARAM			0x73
#define MSG_REQ_AUDIO				0x74
#define MSG_CHG_PRODUCT				0x75

#define MSG_SET_SYSTIME				0x77
#define MSG_SET_DEFPARAM			0x79

#define MSG_SET_PICPARAM			0x90
#define MSG_GET_PICPARAM			0x91


#define MSG_CHANGE_INPUT			0x92
#define MSG_LOW_BITRATE 			0x96 //低码率
#define MSG_MUTE        			0x97
#define MSG_LOCK_SCREEN    			0x99//锁定屏幕 enc110，enc120，enc1100，enc1200

#define MSG_SEND_INPUT				0x93
#define MSG_CAMERACTRL_PROTOCOL     0x9b   //摄像头控制协议修改 消息头后跟一个字节，0：visca，1：p-d，2：p-p

#define MSG_LOCK_FARCTRL            0x9c //请求独占远遥，消息头+1/0(BYTE),1:表示独占，0：表示取消独占。
#define MSG_FARCTRL_LOCKED 			0x9d //远遥已被锁定

#define MSG_FIX_RESOLUTION 			0x9e//导播锁定分辨率


//-------------------------------------------组播----------------------------------------------------------------------------------------------------------------------
#define MSG_MUL_START		        0x80            //开启组播 HDB_MSGHEAD
#define MSG_MUL_STOP		        0x81            //停止组播 HDB_MSGHEAD
#define MSG_MUL_VIDEOSIZE           0x82            //设置组播视频尺寸，HDB_MSGHEAD +flag(BYTE) flag: 0：原始尺寸，1：缩小
#define MSG_MUL_ADDR_PORT           0x83            //设置组播地址和端口, HDB_MSGHEAD+MulAddr
#define MSG_MUL_STREAM	            0x84            //设置组播流，HDB_MSGHEAD +flag(BYTE) flag: 0：所有流，1：音频，2：视频
#define MSG_MUL_STATE               0x85            // 获取组播状态，HDB_MSGHEAD+MulAddr+sizeflag（BYTE）+streamFlag（BYTE）+state(BYTE)(开启/停止).
#define MSG_MUL_SET_AUDIO_ADDR      0x86            //设置音频组播地址和端口, HDB_MSGHEAD+MulAddr
#define MSG_MUL_SET_VIDEO_ADDR      0x87            //设置视频组播地址和端口, HDB_MSGHEAD+MulAddr
#define MSG_MUL_GET_AUDIO_ADDR      0x88            //获取音频组播地址和端口, HDB_MSGHEAD+MulAddr
#define MSG_MUL_GET_VIDEO_ADDR      0x89            //获取视频组播地址和端口, HDB_MSGHEAD+MulAddr
#define MSG_MUL_GET_ADDR            0x90            //获取组播地址和端口, HDB_MSGHEAD+MulAddr
#define MSG_MUL_SET_FRAMEBITRATE	0x91
#define MSG_MUL_GET_FRAMEBITRATE	0x92
#define MSG_HEART					0x93			//心跳
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#ifdef SUPPORT_TRACK
#define	MSG_SET_STUDENTS_TRACK_PARAM	0xA1	//教师跟踪参数设置
#endif

#define AVIIF_KEYFRAME				0x00000010

#define INVALID_FD					-1

#define	A(bit)		rcvd_tbl[(bit)>>3]	/* identify byte in array */
#define	B(bit)		(1 << ((bit) & 0x07))	/* identify bit in byte */
#define	SET(bit)	(A(bit) |= B(bit))
#define	CLR(bit)	(A(bit) &= (~(B(bit))))
#define	TST(bit)	(A(bit) & B(bit))


#define SETLOWSTREAM(dsp,cli,val)			(gDSPCliPara[dsp].cliDATA[cli].LowStream = val)
#define GETLOWSTREAM(dsp,cli)			(gDSPCliPara[dsp].cliDATA[cli].LowStream)
/*if use client*/
#define ISUSED(dsp,cli)				(gDSPCliPara[dsp].cliDATA[cli].bUsed == TRUE)

/*set client used*/
#define SETCLIUSED(dsp,cli,val)		(gDSPCliPara[dsp].cliDATA[cli].bUsed=val)

/*if login client*/
#define ISLOGIN(dsp,cli)			(gDSPCliPara[dsp].cliDATA[cli].bLogIn == TRUE)

/*set client login succeed or failed*/
#define SETCLILOGIN(dsp,cli,val)	(gDSPCliPara[dsp].cliDATA[cli].bLogIn=val)

/*get socket fd*/
#define GETSOCK(dsp,cli)			(gDSPCliPara[dsp].cliDATA[cli].sSocket)

/*set socket fd*/
#define SETSOCK(dsp,cli,val)		(gDSPCliPara[dsp].cliDATA[cli].sSocket=val)

/*current socket if valid*/
#define ISSOCK(dsp,cli)				(gDSPCliPara[dsp].cliDATA[cli].sSocket != INVALID_SOCKET)

/*get dsp handle*/
#define GETDSPFD(dsp)				(gDSPCliPara[dsp].dspFD)

/*set dsp handle*/
#define SETDSPFD(dsp,val)			(gDSPCliPara[dsp].dspFD=val)

/*if handle valid*/
#define ISDSPFD(dsp)				(gDSPCliPara[dsp].dspFD != INVALID_FD)

/*client login type*/
#define SETLOGINTYPE(dsp,cli,val)  	(gDSPCliPara[dsp].cliDATA[cli].nLogInType=val)

/*get login type*/
#define GETLOGINTYPE(dsp,cli)  		(gDSPCliPara[dsp].cliDATA[cli].nLogInType)

/*设置远遥锁定标志位*/
#define SETFARCTRLLOCK(dsp,cli,flag) 				(gDSPCliPara[dsp].cliDATA[cli].lkfarctrlflag=flag)
/*获取远遥锁定标志位*/
#define GETFARCTRLLOCK(dsp,cli) 					(gDSPCliPara[dsp].cliDATA[cli].lkfarctrlflag)

#define MAX_FRAMES_RECREATE		1300000
typedef int SOCKET;
typedef int DSPFD;
typedef unsigned short WORD ;
typedef unsigned int   DWORD ;
typedef unsigned char  BYTE;

/*client infomation*/
typedef struct _DSPClientData {
	int bUsed;
	SOCKET sSocket;
	int bLogIn;
	int nLogInType;
	BYTE lkfarctrlflag ;			//远遥锁定标志位
	int LowStream;//鉴别低码流
} DSPClientData;

/*DSP client param*/
typedef struct __DSPClientParam {
	DSPFD dspFD;     	//DSP handle
	DSPClientData cliDATA[MAX_CLIENT]; //client param infomation
} DSPClientParam;
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
} HDBMSGHEAD;
#define HEAD_LEN2			sizeof(HDBMSGHEAD)
#define HEAD_LEN			3
typedef struct _SysDSPParamsV2 {
	unsigned char szMacAddr[8];
	unsigned int dwAddr;
	unsigned int dwGateWay;
	unsigned int dwNetMark;
	char strName[16];
	int nFrame;
	int nColors;
	int nQuality;
	char strVer[10];
	char strAdminPassword[16];
	char strUserPassword[16];
	short sLeftRight;
	short sUpDown;
	////// ver 2.0//////////////////
	short sCbr;  //
	short sBitrate; //
	short sCodeType;//  0---RCH0     0f -----H264
	char  strCodeVer[8];    //
	BYTE bVmode;		//视频模式    0 ------- 4CIF   1 ------CIF   2------QCIF
	BYTE bType;
	char sTemp[26];
	//	int nTemp[7];
	////// ver 1.0
} SysDSPParamsV2;


typedef struct _DSPDataHeader {
	DWORD   id;
	long    biWidth;							//VGA：Width		Audio：SampleRate
	long    biHeight;							//VGA：Height	Audio：BitRate
	WORD    biBitCount;							//VGA：BitCount	Audio：Channel
	DWORD   fccCompressedHandler;				//VGA：mmioFOURCC (‘H’,’2’,’6’,’4’)
	//Audio：mmioFOURCC(‘A’,’D’,’T’,’S’)
	DWORD   fccHandler;	   						//Audio：SampleBit
	DWORD   dwCompressedDataLength;
	DWORD   dwFlags;
	WORD    dwSegment;
	long    dwPacketNumber;
} DSPDataHeader;
/*视频参数*/
typedef struct __VIDEOPARAM {
	DWORD nDataCodec;   					//编码方式
	//视频：mmioFOURCC('H','2','6','4');
	DWORD nFrameRate;   					//视频：数据帧率
	DWORD nWidth;       					//视频：宽度
	DWORD nHight;       					//视频：高度
	DWORD nColors;      						//视频：颜色数
	DWORD nQuality;							//视频：质量系数
	WORD sCbr;								//视频：定码率/定质量
	WORD sBitrate;							//视频：带宽
} VideoParam;
/*audio param*/
typedef struct __AUDIOPARAM {
	DWORD Codec;  							//Encode Type
	DWORD SampleRate; 						//SampleRate
	DWORD BitRate;  						//BitRate
	DWORD Channel;  						//channel numbers
	DWORD SampleBit;  						//SampleBit
	BYTE  LVolume;							//left volume       0 --------31
	BYTE  RVolume;							//right volume      0---------31
	WORD  InputMode;                        //1------ MIC input 0------Line Input
} AudioParam;

/*system param save*/
typedef struct __SAVEPARAM {

	SysDSPParamsV2 sysPara;                 				//standard param table
	VideoParam videoPara[DSP_NUM];				 	//video param table
	AudioParam audioPara[DSP_NUM];					//audio param table
} SavePTable;
/*system param save*/
typedef struct __SAVEPARAM2 {

	VideoParam lowvideoPara[DSP_NUM]; 					//audio param table
} SavePTable2;
//low video param table
typedef struct _WhoIsMsg {
	unsigned char szFlag[4];	//must 0x7e7e7e7e
	unsigned char szMacAddr[8];
	unsigned int dwAddr;
	unsigned int dwGateWay;
	unsigned int dwNetMark;
	char strName[16];
	int nFrame;
	int nColors;
	int nQuality;
	char strVer[10];
	int nType;			//6 ---- ENC1200
} WhoIsMsg;
/*录制信息*/
typedef struct _DSPRecDataInfo {
	char  IpAddr[16];   //录制数据的iP地址
	int   bConnected;   //是否已连接
	int   nDataCodec;   //编码方式 JPEG为 0x1001
	int   nFrameRate;   //数据帧率
	int   nWidth;       //数据的录制宽度
	int   nHight;       //高度
	int   nColors;      //颜色数
	int   nOthers;      //保留
	char  Version[16];  //版本信息
} DSPRecDataInfo;

#define IIS_RTMP_CONNECT()          (1 == g_rtmp_connect_flag)
//#define IIS_AUDEOMULT_SEND()        (1 == gCur_MulCast.stream_flag)

typedef struct __HDB_MULTIADDR {
	char chIP[16];
	int nPort;
} MulAddr;

typedef struct __HDB_SIZETYPE {
	int nflag; // =0 原始大小 =1 缩小到指定尺寸
	int width;
	int height;
} SizeType;



typedef struct frame_bitrate {
	int nFrame;
	int nBitrate;
} frame_bitrate;
/*protocol struct*/
typedef struct __PROTOCOL__ {
	/*protocol count*/
	unsigned char count ;
	/*protocol open status  1 -- open */
	/*0--- close all */
	unsigned int status: 1 ;
	/*TS stream*/
	unsigned int TS: 1;
	/*RTP*/
	unsigned int RTP: 1;
	/*RTSP*/
	unsigned int RTSP: 1;
	/*RTMP*/
	unsigned int RTMP: 1;
	/*reserve*/
	unsigned char temp[7];
} Protocol;

typedef enum __ENUM_PROTOCOL__ {
    TS_STREAM = 0,
    RTP_STREAM,
    RTSP_STREAM,
    RTMP_STREAM,
    MAX_PROTOCOL
} Enum_Protocol;

#define MULT_STOP		(0)
#define MULT_START		(1)

/*mult addr*/
typedef struct __MULT_ADDR_PORT {
	char chIP[16];
	unsigned short nPort;
	short nStatus;
} MultAddr;

#define WEB_SET_MULT_START(protocol)		(s_MultAddr[protocol].nStatus = MULT_START)
#define WEB_SET_MULT_STOP(protocol)		(s_MultAddr[protocol].nStatus = MULT_STOP)

#define H264_ENC_PARAM_FLAG			0x55AA55AA

typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;

/*set video encode param table*/
typedef struct __VENC_PARAMS_ {
	UINT32 uflag;  		/*video encode params available flag
						   flag:0x55AA55AA for param availability
						   flag:0x0  for param not availability
						 */
	UINT32 req_I;   		/* 强制I 帧 AVIIF_KEYFRAME 为I 帧*/
	UINT32 framerate;	/*control frame rate*/
	UINT32 quality;		/*quality*/
	UINT32 width;		/*encoder lib width*/
	UINT32 height;		/*encoder lib height*/
	UINT16 bitrate;		/*bit rate*/
	UINT16 cbr;	  		/*1:bitrate / 0:quality*/
} VENC_params;

/*set audio encode param table*/
typedef struct __AENC_PARAMS__ {
	UINT32 uflag;  		/* audio encode params available flag
						   flag:0x55AA55AA for param availability
						   flag:0x0  for param not availability
						 */
	UINT32 samplerate;		/*audio encode sample rate*/
	UINT32 bitrate;			/*audio encode bit rate*/
	UINT32 channel;			/*audio channels*/
	UINT32 samplebit;		/*sample bit*/
	UINT8  lvolume;   		/*left volume*/
	UINT8  rvolume;			/*right volume*/
	UINT16 inputmode;		/*0: mic input 1:line input*/
} AENC_params;

/*revise VGA picture*/
typedef struct pic_revise {
	short hporch;    //col
	short vporch;    //row
	int temp[4];
} pic_revise ;

/*set encode params table*/
typedef struct __VAENC_PARAM__ {
	VENC_params venc;
	AENC_params aenc;
} VAENC_params, *pVAENC_Params;

typedef struct gpio_data {
	unsigned int data;
	unsigned int nums;
} gpio_op;



#define MAX_MODE_VALUE			50
#define D_1280x1024X50			17


typedef struct __HV__ {
	short	hporch;
	short	vporch;
} HV;

typedef struct __HV_Table__ {
	HV	analog[MAX_MODE_VALUE];
	HV	digital[MAX_MODE_VALUE];
} HVTable;

/*Resize Param table*/
typedef struct __THREAD_PARAM__ {
	int                 nPos;
	int                 client_socket;
	struct sockaddr_in  client_addr;
} Thread_Param;
typedef struct __PthreadLock__ {
	pthread_mutex_t net_send_m;
	pthread_mutex_t sys_m;
	pthread_mutex_t save_flush_m;
	pthread_mutex_t send_m;
	pthread_mutex_t send_audio_m;
	pthread_mutex_t audio_video_m;
	pthread_mutex_t save_sys_m;
	pthread_mutex_t rtmp_send_m;
	pthread_mutex_t rtmp_audio_video;
	pthread_mutex_t rtp_send_m;
	pthread_mutex_t rtsp_send_m;
	pthread_mutex_t ts_send_m;
} PLock;
/*加字幕结构标题*/
typedef struct __RecAVTitle {
	int  x;                     //x pos
	int  y;            //y pos
	int len;   //Text实际长度
	char Text[128];//text
} RecAVTitle;

/*read Hportch Vportch Table*/
extern int ReadHVTable(const HVTable *pp, int digital);
extern int SaveHVTable(HVTable *pOld, HVTable *pNew, int digital);
/*compare beyond HV table*/
extern int BeyondHVTable(int mode, int digital, short hporch, short vporch, HVTable *pp);

/*initial hporch and vporch value*/
extern  int InitHVTable(HVTable *pp);
/*define*/
#define RESIZE_OK		(0xAA55AA55)
#define RESIZE_INVALID		(-1)

#define RECREATE_OK				(0x55AA55AA)
#define RECREATE_INVALID		(-1)
#define ZOOMIN		(0xAAAAAA55)
#define ZOOMOUT		(0xAA55AA55)
/*get resize param*/
#define ISZOOMOUT		(ZOOMOUT == gResizeP.flag)//
#define ISZOOMIN		(ZOOMIN == gResizeP.flag)//放大
#define NORESIZE		(RESIZE_INVALID == gResizeP.flag)
#define INIT_RESIZE_PARAM	{RESIZE_INVALID,RECREATE_INVALID,5,0,0,800,600,800,600};
/*Resize Param table*/
typedef struct __RESIZE_PARAM__ {
	unsigned int flag ;
	unsigned int re_enc ; // receate encode lbi
	int black;
	int x;
	int y;
	int req_w;			//source width
	int req_h;			//source height
	int dst_w;			//dest  width
	int dst_h;			//dest height
} ResizeParam;
typedef struct low_bitrate {
	int nWidth;		//宽
	int nHeight;		//高
	int nFrame;		//帧率
	int nBitrate;		//带宽
	int nTemp;		//保留
} low_bitrate;

/**/
#define MIN_CLIENT_PORT_BIT 9
#define TCP_BUFF_LEN 1400
#define RTP_PAYLOAD_LEN 1350

typedef signed char INT8;
typedef signed short INT16;
typedef signed int INT32;

extern int g_Logserver;
extern struct sockaddr_in               g_serveraddr;
extern pthread_mutex_t sendlog_m;
extern char logbuf[1024];

/*Initial system/video/audio param table*/
extern void InitSysParams();
/*Initial pthread mutex*/
extern int initMutexPthread();
/*Initial client infomation*/
extern void InitClientData(unsigned char dsp);
/*set send timeout*/
extern int SetSendTimeOut(SOCKET sSocket, unsigned long time);
/*set recv timeout*/
extern int SetRecvTimeOut(SOCKET sSocket, unsigned long time);
/*send date to socket runtime*/
extern int WriteData(int s, void *pBuf, int nSize);
/*set param mutex*/
extern int initSetParam();
/*TCP task mode*/
extern void DSP1TCPTask(void *pParam);
extern void MultiCastTask(void *pParam);
extern int SendRTPAudiotoClient(int nLen, unsigned char *pData, int nFlag, unsigned char index);
extern int SendMultCastDataToVlc(char *pData, int length);
/*send encode data to every client*/
//extern void SendDataToClient(int nLen, unsigned char *pData, int nFlag, int width, int height, unsigned char index);
/*send encode audio data to every client*/
extern void SendAudioToClient(int nLen, unsigned char *pData, int nFlag, unsigned char index, unsigned int samplerate);
/*parse ADTS packets*/
extern void SendADTSAudio(unsigned char *pData, unsigned int dwSize, int nUseLen, int index);
/*set h264 width*/
extern int SetVGAH264Width(int width);
/*set h264 height*/
extern int SetVGAH264Height(int height);
/*get encode height*/
extern unsigned int VGAH264Width(void);
/*get encode width*/
extern unsigned int VGAH264Height(void);
/*Initial default audio encoder params*/
extern int InitAudioEncParams(AudioParam *aparam);
/*Initial default video encoder params*/
extern int InitVideoEncParams(SavePTable *vparam);
/*set ethernet ip address*/
extern void SetEthConfigIP(unsigned int ipaddr, unsigned netmask);
/*set ethernet gatewall*/
extern void SetEthConfigGW(unsigned int gw);
/*revise pictrue*/
extern int revise_picture(short h, short v);
/*set Mac Address*/
extern int SetMacAddr(int fd);

/*
change index to samplerate value
	///0---8KHz
	///2---44.1KHz default----44100Hz
	///1---32KHz
	///3---48KHz
	///other-----96KHz
*/
extern int ChangeSampleIndex(int index);

/**********************************************************************
* Function Name:  ConfigSetKey
* Function scriptor:  Set Key Value
* Access table:   NULL
* Modify Table:	  NULL
* Input param：   void *CFG_file File   void *section   void *key
					void *buf
* Output Param:   void *buf  key Value
* Return:  		  0 -- OK  no 0-- FAIL
***********************************************************************/
extern int  ConfigSetKey(void *CFG_file, void *section, void *key, void *buf);
/**********************************************************************
* Function Name:  ConfigGetKey
* Function scriptor:  Get Key Value
* Access table:   NULL
* Modify Table:	  NULL
* Input param：   void *CFG_file File   void *section   void *key
* Output Param:   void *buf  key Value
* Return:  		  0 -- OK  no 0-- FAIL
***********************************************************************/
extern int  ConfigGetKey(void *CFG_file, void *section, void *key, void *buf);
/*create sys Param File*/
extern void CreateSysFile(const char *config_file);
/*Create append Param File*/
extern void CreateAppendParam(const char *config_file);
/*Read Remote Protocol*/
extern int ReadRemoteCtrlIndex(const char *config_file, int *index);
/*save remote control index*/
extern int SaveRemoteCtrlIndex(void);
/*read param table from file*/
extern int ReadEncodeParamTable(char *config_file, SavePTable *tblParam);
/*save params to flash "config.dat"*/
extern int SaveParamsToFlash(int dsp);
/*update */
extern int ProductUpdate(int fd, char *p, unsigned long len);
extern void RtmpTask(void *pParam);
//extern void RtspTask(void *pParam);
/*find encode box*/
extern void DSP1UDPTask(void *pParam);
/*send data to tty com*/
extern int SendDataToCom(int fd, unsigned char *data, int len);
/*open lcd module communication ttyS*/
extern void OpenLCDCom();
/*Thread LCD*/
extern void LCDModuleTask(void *param);
/*get control frame rate value*/
extern unsigned int GetCtrlFrame();
/*print current num*/
extern void PrintClientNum();
extern int open_gpio_port();

extern int set_gpio_bit(int bit);

extern int clear_gpio_bit(int bit);

extern int get_gpio_bit();

/*Send ICMP message to router*/
extern void SendICMPmessage(void);

extern int cleanICMP(void);

extern void InitICMP(void);

/*is resize*/
extern int ISresize();

/*update FPGA program*/
extern int updateFpgaProgram(const char *fpgafile);


/*ES to TS MultCast Send*/
/*get Multcast status*/
extern int GetMultStatus(int protocol);

/*Initial protocol param*/
extern int InitProtocolParam(Protocol *pp);
/*print protocol*/
extern int PrintProtocol(Protocol *pp);
/*check IP addr and Netmask */
extern int CheckIPNetmask(int ipaddr, int netmask, int gw);
extern unsigned int GetIPaddr(char *interface_name);
extern unsigned int GetNetmask(char *interface_name);
extern unsigned int GetBroadcast(char *interface_name);
extern void SendClassInfotoTeacher();


#endif //__NETCOM_HEADER__
