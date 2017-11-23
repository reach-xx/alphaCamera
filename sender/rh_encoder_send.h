#ifndef __RH_ENCODER_SEND_H
#define __RH_ENCODER_SEND_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>


/* Thread error codes */
#define THREAD_SUCCESS      (void *) 0
#define THREAD_FAILURE      (void *) -1

#define INVALID_SOCKET 			-1
#define INVALID_FD				-1

#define LOGIN_USER				0
#define LOGIN_ADMIN				1

#define     START          1
#define     STOP           0

#define MAX_CLIENT 				6
#define MAX_PACKET				14600
#define AVIIF_KEYFRAME				0x00000010
#define PORT_LISTEN_DSP_EX				3700


#define  HEAD_LEN			sizeof(MSGHEAD)
#define  HEAD110_LEN			3

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

#define MSG_QUALITYVALUE			46 	//ï¿½ï¿½ï¿½Ã¸ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ä£Ê?0:ï¿½ï¿½ 1:ï¿½ï¿½ 2:ï¿½ï¿½) 
#define MSG_UPLOADIMG 			    49	//ï¿½Ï´ï¿½logoÍ¼Æ¬
#define MSG_GET_LOGOINFO 			50	//ï¿½ï¿½È¡logoÍ¼Æ¬ï¿½ï¿½Ï¢
#define MSG_SET_LOGOINFO 			51	//ï¿½ï¿½ï¿½ï¿½logoÍ¼Æ¬ï¿½ï¿½Ï¢
#define TEACHER_HART                111
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
#define MSG_SEND_INPUT				0x93
#define MSG_PIC_DATA                0x94        //ï¿½Ô¶ï¿½pptï¿½ï¿½ï¿½ï¿½
#define MSG_LOW_SCREENDATA          0x95        //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
#define MSG_LOW_BITRATE             0x96        //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?
#define MSG_MUTE                    0x97        //ï¿½ï¿½ï¿½ï¿½ï¿?ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê¾ï¿½ï¿½Ñ¯ï¿½ï¿½ï¿½ï¿½×´Ì¬ï¿½ï¿?
#define MSG_PHOTO                   0x98        //ï¿½ï¿½ï¿½à¹¦ï¿½ï¿½
#define MSG_LOCK_SCREEN    0x99//ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ä»
#define MSG_GSCREEN_CHECK           0x9a   //ï¿½ï¿½ï¿½ï¿½Ð£ï¿½ï¿½ï¿½ï¿½ï¿½Þ²ï¿½ï¿½ï¿½
#define MSG_SETVGAADJUST            43 //ï¿½ß¿ï¿½ï¿½ï¿½ï¿?ï¿½ï¿½Ï¢Í·+pic_revise
#define MSG_CAMERACTRL_PROTOCOL     0x9b   //ï¿½ï¿½ï¿½ï¿½Í·ï¿½ï¿½ï¿½ï¿½Ð­ï¿½ï¿½ï¿½Þ¸ï¿½ ï¿½ï¿½Ï¢Í·ï¿½ï¿½ï¿½Ò»ï¿½ï¿½ï¿½Ö½Ú£ï¿?
#define MSG_TRACKAUTO				0x9f



#define 	PORT_ONE 0

#define 	ENCODE_TYPE					6	//ENC-MOD



/*if use client*/
#define ISUSED(dsp,cli)							(gDSPCliPara[dsp].cliDATA[cli].bUsed == TRUE)

/*set client used*/
#define SETCLIUSED(dsp,cli,val)					(gDSPCliPara[dsp].cliDATA[cli].bUsed=val)

/*if login client*/
#define ISLOGIN(dsp,cli)							(gDSPCliPara[dsp].cliDATA[cli].bLogIn == TRUE)

/*set client login succeed or failed*/
#define SETCLILOGIN(dsp,cli,val)					(gDSPCliPara[dsp].cliDATA[cli].bLogIn=val)

/*get socket fd*/
#define GETSOCK(dsp,cli)							(gDSPCliPara[dsp].cliDATA[cli].sSocket)

/*set socket fd*/
#define SETSOCK(dsp,cli,val)						(gDSPCliPara[dsp].cliDATA[cli].sSocket=val)

/*current socket if valid*/
#define ISSOCK(dsp,cli)							(gDSPCliPara[dsp].cliDATA[cli].sSocket != INVALID_SOCKET)


/*get dsp handle*/
#define GETDSPFD(dsp)							(gDSPCliPara[dsp].dspFD)

/*set dsp handle*/
#define SETDSPFD(dsp,val)							(gDSPCliPara[dsp].dspFD=val)

/*if handle valid*/
#define ISDSPFD(dsp)								(gDSPCliPara[dsp].dspFD != INVALID_FD)

/*client login type*/
#define SETLOGINTYPE(dsp,cli,val)					(gDSPCliPara[dsp].cliDATA[cli].nLogInType=val)

/*get login type*/
#define GETLOGINTYPE(dsp,cli)						(gDSPCliPara[dsp].cliDATA[cli].nLogInType)



/*client infomation*/
typedef struct _ClientData {
	HI_S32 bUsed;
	HI_S32 sSocket;
	HI_S32 bLogIn;
	HI_S32 nLogInType;
	HI_S32 LowRateflag;
	HI_S64 connect_time;
} ClientData;

/*DSP client param*/
typedef struct __DSPCliParam {
	ClientData cliDATA[MAX_CLIENT]; //client param infomation
} DSPCliParam;

typedef struct __MSGHEAD__ {
	HI_U16	nLen;
	HI_U16	nVer;							//ï¿½æ±¾ï¿½ï¿½(ï¿½Ý²ï¿½ï¿½ï¿½)
	HI_U8	nMsg;							//ï¿½ï¿½Ï¢ï¿½ï¿½ï¿½ï¿½
	HI_U8	szTemp[3];						//ï¿½ï¿½ï¿½ï¿½ï¿½Ö½ï¿½
} MSGHEAD;

typedef struct __MSGHEAD110__ {
	HI_U8	nVer;
	HI_U8	nLen;							//ï¿½æ±¾ï¿½ï¿½(ï¿½Ý²ï¿½ï¿½ï¿½)
	HI_U8	nMsg;							//ï¿½ï¿½Ï¢ï¿½ï¿½ï¿½ï¿½
} MSGHEAD110;


typedef struct __HDB_FRAME_HEAD {
	HI_U32 ID;								//=mmioFOURCC('4','D','S','P');
	HI_U32 nTimeTick;    					//Ê±ï¿½ï¿½ï¿?
	HI_U32 nFrameLength; 					//Ö¡ï¿½ï¿½ï¿½ï¿½
	HI_U32 nDataCodec;   					//ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	//ï¿½ï¿½ï¿½ë·½Ê½
	//ï¿½ï¿½Æµ :mmioFOURCC('H','2','6','4');
	//ï¿½ï¿½Æµ :mmioFOURCC('A','D','T','S');
	HI_U32 nFrameRate;   					//ï¿½ï¿½Æµ  :Ö¡ï¿½ï¿½
	//ï¿½ï¿½Æµ :ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ (default:44100)
	HI_U32 nWidth;       					//ï¿½ï¿½Æµ  :ï¿½ï¿½
	//ï¿½ï¿½Æµ :Í¨ï¿½ï¿½ï¿½ï¿½ (default:2)
	HI_U32 nHight;       					//ï¿½ï¿½Æµ  :ï¿½ï¿½
	//ï¿½ï¿½Æµ :ï¿½ï¿½ï¿½ï¿½Î» (default:16)
	HI_U32 nColors;      					//ï¿½ï¿½Æµ  :ï¿½ï¿½É«ï¿½ï¿½  (default: 24)
	//ï¿½ï¿½Æµ :ï¿½ï¿½Æµï¿½ï¿½ï¿½ï¿½ (default:64000)
	HI_U32 dwSegment;						//ï¿½Ö°ï¿½ï¿½ï¿½Ö¾Î»
	HI_U32 dwFlags;							//ï¿½ï¿½Æµ:  I Ö¡ï¿½ï¿½Ö¾
	//ï¿½ï¿½Æµ:  ï¿½ï¿½ï¿½ï¿½
	HI_U32 dwPacketNumber; 					//ï¿½ï¿½ï¿½ï¿½ï¿?
	HI_U32 nOthers;      					//ï¿½ï¿½ï¿½ï¿½
} FRAMEHEAD;

typedef struct _DataHeader
{
   unsigned  int    id;
	 int    biWidth;							//VGA£ºWidth		Audio£ºSampleRate
	 int    biHeight;							//VGA£ºHeight	Audio£ºBitRate
	unsigned short    biBitCount;							//VGA£ºBitCount	Audio£ºChannel
	unsigned int   fccCompressedHandler;				//VGA£ºmmioFOURCC (¡®H¡¯,¡¯2¡¯,¡¯6¡¯,¡¯4¡¯)
											    //Audio£ºmmioFOURCC(¡®A¡¯,¡¯D¡¯,¡¯T¡¯,¡¯S¡¯)
	unsigned int   fccHandler;	   						//Audio£ºSampleBit
    unsigned int   dwCompressedDataLength;
    unsigned int   dwFlags;
   unsigned  short    dwSegment;
	int    dwPacketNumber;
}DataHeader;

/*system param table*/
typedef struct __SYSPARAMS							
{
	unsigned char szMacAddr[8];				//MAC address
	unsigned int dwAddr;					//IP address
	unsigned int dwGateWay;					//getway
	unsigned int dwNetMark;					//sub network mask
	char strName[16];						//encode box name
	char strVer[10];						//Encode box version
	HI_U16 unChannel;							//channel numbers
	char bType; 							//flag  0 -------VGABOX     3-------200 4-------110 5-------120 6--------1200
	char bTemp[3]; 							//reserve
	int nTemp[7];							//reserve
}SYSPARAMS;

typedef struct _SysParamsV2 {
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
	BYTE bVmode;		//ÊÓÆµÄ£Ê½    0 ------- 4CIF   1 ------CIF   2------QCIF
	BYTE bType;
	char sTemp[26];
	//	int nTemp[7];
	////// ver 1.0
} SysParamsV2;
/*Â¼ÖÆÐÅÏ¢*/
typedef struct _RecDataInfo {
	char  IpAddr[16];		//Â¼ÖÆÊý¾ÝµÄiPµØÖ·
	int   bConnected;		//ÊÇ·ñÒÑÁ¬½Ó
	int   nDataCodec;		//±àÂë·½Ê½ JPEGÎª 0x1001
	int   nFrameRate;		//Êý¾ÝÖ¡ÂÊ
	int   nWidth;			//Êý¾ÝµÄÂ¼ÖÆ¿í¶È
	int   nHight;			//¸ß¶È
	int   nColors;			//ÑÕÉ«Êý
	int   nOthers;			//±£Áô
	char  Version[16];		//°æ±¾ÐÅÏ¢
}RecDataInfo;

typedef struct
{
	AVFormatContext *oc;
	int enableVideo;
	int enableAudio;
	int width;
	int height;
	int bitrate;
	int channels;
	int samplerate;

	int VideoStreamId;
	int AudioStreamId;
	unsigned long sTime;

	int state;
	pthread_mutex_t lock;
}RtmpHandle;



/*send encode data to every client*/
void SendDataToClient(int nLen, unsigned char *pData,
                      int nFlag, unsigned char index, int width, int height);





#endif //__RH_ENCODER_SEND_H
