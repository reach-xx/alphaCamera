#ifndef __RH_PROCOTOL_H
#define __RH_PROCOTOL_H

#include "sample_comm.h"

/*协议类型*/
#define VISCA_PROCO			0x01
#define PELCO_D_PROCO		0x02
#define PELCO_P_PROCO		0x03
/*命令的最大长度*/
#define MAX_CMD_LENGTH  	32

/*控制命令条数*/
#define MAX_CMD_NUMS		20

/*Ycat Visca协议端口*/
#define PORT 				1259


/*实际输出分辨率*/
#define OUTPUT_WIDTH		1920
#define OUTPUT_HEIGHT		1080

/*Sensor 输出分辨率*/
#define SENSOR_OUTPUT_W		3840
#define SENSOR_OUTPUT_H		2160

/*数字云台最小分辨率*/
#define PTZ_MIN_WIDTH       960
#define PTZ_MIN_HEIGHT		540

/*计算坐标*/


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

/*协议定义*/
typedef struct __RH_CMD_CODE__
{
	int cmd_mode;   //命令的类型  上下左右等	
	int code_len;	//命令格式长度
	unsigned char code[MAX_CMD_LENGTH];	  //命令格式
}CMD_code;

typedef struct __RH_PROCOTOL_H__ 
{
	int type;   //协议类型       VISCA，PELCO-D，PELCO-P等
	int cmd_nums; //命令数量
	CMD_code stCMD[MAX_CMD_NUMS];
}RH_Procotol;

/* ---- ------ ----- ----- ----- ----- ----- ----- ----- ----- ----- -----*/
typedef struct __RH_COORD_
{
	HI_BOOL  bAuto;      /*手动模式--FALSE      自动模式—-TRUE*/
	HI_BOOL  bEnable;    /*启动*/
	int      cmd_mode;   //命令的类型  上下左右等
	int 	 vSpeed;     //V速度
	int      wSpeed;     //W速度	
	RECT_S   rect;       //计算当前实时坐标
	RECT_S   end_rect;   //自动模式下，移动结束位置
}RH_Coord;

#endif   //__RH_PROCOTOL_H
