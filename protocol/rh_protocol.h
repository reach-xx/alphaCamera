#ifndef __RH_PROCOTOL_H
#define __RH_PROCOTOL_H

#include "sample_comm.h"

/*Э������*/
#define VISCA_PROCO			0x01
#define PELCO_D_PROCO		0x02
#define PELCO_P_PROCO		0x03
/*�������󳤶�*/
#define MAX_CMD_LENGTH  	32

/*������������*/
#define MAX_CMD_NUMS		20

/*Ycat ViscaЭ��˿�*/
#define PORT 				1259


/*ʵ������ֱ���*/
#define OUTPUT_WIDTH		1920
#define OUTPUT_HEIGHT		1080

/*Sensor ����ֱ���*/
#define SENSOR_OUTPUT_W		3840
#define SENSOR_OUTPUT_H		2160

/*������̨��С�ֱ���*/
#define PTZ_MIN_WIDTH       960
#define PTZ_MIN_HEIGHT		540

/*��������*/


/*Define coordinate mode*/
typedef enum hiVISCA_E   
{
    VISCA_UP = 0,   
    VISCA_DOWN,  
    VISCA_LEFT,   
    VISCA_RIGHT,   
    VISCA_UPLEFT,   
    VISCA_UPRIGHT,  
    VISCA_DOWNLEFT,   
    VISCA_DOWNRIGHT,
	VISCA_ZOOMTELE,
	VISCA_ZOOMWIDE,
	VISCA_STOP,
    VISCA_EXIT,
    VISCA_INVALID
}VISCA_E;

/*Э�鶨��*/
typedef struct __RH_CMD_CODE__
{
	int cmd_mode;   //���������  �������ҵ�	
	int code_len;	//�����ʽ����
	unsigned char code[MAX_CMD_LENGTH];	  //�����ʽ
}CMD_code;

typedef struct __RH_PROCOTOL_H__ 
{
	int type;   //Э������       VISCA��PELCO-D��PELCO-P��
	int cmd_nums; //��������
	CMD_code stCMD[MAX_CMD_NUMS];
}RH_Procotol;

/* ---- ------ ----- ----- ----- ----- ----- ----- ----- ----- ----- -----*/
typedef struct __RH_COORD_
{
	HI_BOOL  bAuto;      /*�ֶ�ģʽ--FALSE      �Զ�ģʽ��-TRUE*/
	HI_BOOL  bEnable;    /*����*/
	int      cmd_mode;   //���������  �������ҵ�
	int 	 vSpeed;     //V�ٶ�
	int      wSpeed;     //W�ٶ�	
	RECT_S   rect;       //���㵱ǰʵʱ����
	RECT_S   end_rect;   //�Զ�ģʽ�£��ƶ�����λ��
}RH_Coord;

#endif   //__RH_PROCOTOL_H
