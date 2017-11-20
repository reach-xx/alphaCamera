#ifndef __RH_ENCODER_SEND_H
#define __RH_ENCODER_SEND_H

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


#define  HEAD_LEN			sizeof(MSGHEAD)
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

#define MSG_QUALITYVALUE			46 	//���ø������������ģʽ(0:�� 1:�� 2:��) 
#define MSG_UPLOADIMG 			    49	//�ϴ�logoͼƬ
#define MSG_GET_LOGOINFO 			50	//��ȡlogoͼƬ��Ϣ
#define MSG_SET_LOGOINFO 			51	//����logoͼƬ��Ϣ
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
#define MSG_PIC_DATA                0x94        //�Զ�ppt����
#define MSG_LOW_SCREENDATA          0x95        //����������
#define MSG_LOW_BITRATE             0x96        //���������
#define MSG_MUTE                    0x97        //�����1�������0�������������������ʾ��ѯ����״̬��
#define MSG_PHOTO                   0x98        //���๦��
#define MSG_LOCK_SCREEN    0x99//������Ļ
#define MSG_GSCREEN_CHECK           0x9a   //����У�����޲���
#define MSG_SETVGAADJUST            43 //�߿����,��Ϣͷ+pic_revise
#define MSG_CAMERACTRL_PROTOCOL     0x9b   //����ͷ����Э���޸� ��Ϣͷ���һ���ֽڣ�
#define MSG_TRACKAUTO				0x9f



#define 	PORT_ONE 0
#define 	ENCODE_TYPE					6	//ENC-MOD



/*if use client*/
#define ISUSED(dsp,cli)				(gDSPCliPara[dsp].cliDATA[cli].bUsed == HI_TRUE)
/*set client used*/
#define SETCLIUSED(dsp,cli,val)		(gDSPCliPara[dsp].cliDATA[cli].bUsed=val)
/*if login client*/
#define ISLOGIN(dsp,cli)			(gDSPCliPara[dsp].cliDATA[cli].bLogIn == HI_TRUE)
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
/*set resize flag*/
#define SETLOWRATEFLAG(dsp,cli,val)  	(gDSPCliPara[dsp].cliDATA[cli].LowRateflag=val)
/*get resize flag*/
#define GETLOWRATEFLAG(dsp,cli)  		(gDSPCliPara[dsp].cliDATA[cli].LowRateflag)
/*get connect time*/
#define GETCONNECTTIME(dsp,cli)  		(gDSPCliPara[dsp].cliDATA[cli].connect_time)


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
	HI_U16	nVer;							//�汾��(�ݲ���)
	HI_U8	nMsg;							//��Ϣ����
	HI_U8	szTemp[3];						//�����ֽ�
} MSGHEAD;

typedef struct __HDB_FRAME_HEAD {
	HI_U32 ID;								//=mmioFOURCC('4','D','S','P');
	HI_U32 nTimeTick;    					//ʱ���
	HI_U32 nFrameLength; 					//֡����
	HI_U32 nDataCodec;   					//��������
	//���뷽ʽ
	//��Ƶ :mmioFOURCC('H','2','6','4');
	//��Ƶ :mmioFOURCC('A','D','T','S');
	HI_U32 nFrameRate;   					//��Ƶ  :֡��
	//��Ƶ :������ (default:44100)
	HI_U32 nWidth;       					//��Ƶ  :��
	//��Ƶ :ͨ���� (default:2)
	HI_U32 nHight;       					//��Ƶ  :��
	//��Ƶ :����λ (default:16)
	HI_U32 nColors;      					//��Ƶ  :��ɫ��  (default: 24)
	//��Ƶ :��Ƶ���� (default:64000)
	HI_U32 dwSegment;						//�ְ���־λ
	HI_U32 dwFlags;							//��Ƶ:  I ֡��־
	//��Ƶ:  ����
	HI_U32 dwPacketNumber; 					//�����
	HI_U32 nOthers;      					//����
} FRAMEHEAD;

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

/*send encode data to every client*/
void SendDataToClient(int nLen, unsigned char *pData,
                      int nFlag, unsigned char index, int width, int height);





#endif //__RH_ENCODER_SEND_H
