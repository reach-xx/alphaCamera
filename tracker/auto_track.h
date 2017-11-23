
/*
 * auto_track.h
 *
 * ============================================================================
 * Copyright (c) Texas Instruments Inc 2012
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied or provided.
 * ============================================================================
 */

#ifndef _AUTO_TRACK_H
#define _AUTO_TRACK_H

#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include "rh_motion_detect.h"

/**
* @	ÿ��ͳ��ʱ��εĳ��ȣ����ӱ�ʾ
*/
#define CLASSINFO_PER_TIME              (15)

/**
* @	����Ծʱ�����
*/
#define MAX_INTERACTION_NUM					(20)

/**
* @	�����������
*/
#define MAX_STANDUP_NUM					(50)



/**
* @	���������������
*/
#define TRACK_BROADCAST_NUM				(2)

/**
* @	��������һ����������ĵ���
*/
#define ONE_BROADCAST_POINT_NUM			(2)

/**
* @	������������ĵ������ǰ��������2��
*/
#define TRACK_BROADCAST_POINT_NUM				(TRACK_BROADCAST_NUM * ONE_BROADCAST_POINT_NUM)

/**
* @ ����ѧ�������������
*/
#define		STUCONNECT_NUM_MAX		(2)		
/**
* @ ���Ӱ�������������
*/
#define		BLACKCONNECT_NUM_MAX		(2)		




/**
* @ ������Ƕȸ���
*/
#define CAM_ANGLE_NUM	(36)

/**
* @	�Ƕȶ�Ӧ����С��λ�����ص�
*/
#define ANGLE_POINT_NUM	(20)




/**
* @	�ļ����Ƶĳ���
*/
#define FILE_NAME_LEN		(64)

/**
* @ ��ʦ���ٲ��������ļ�
*/
#define TRACK_TEACHER_POINT_CAM_FILE		"track_teacher_point_cam.ini"

#define DEBUG_INFO "debug_info"
#define DEBUG_IPADDR "debug_ipaddr"

/**
* @	�����ǰȡ����ͼƬ·��
*/
#define	CLASS_VIEW_JPG	"/opt/reach/class_view.yuv"

/**
* @	ȷ�����ٿ��ĸ���
*/
#define TRACK_RANGE_NUM					(10)

/**
* @	ȷ���������ĸ���
*/
#define TRACK_TRIGGER_NUM				(4)


/**
* @	���������λ����ռ���ֽ�����ͨVISCAռ��9���ֽڣ�����BRC-Z330ռ��8���ֽ�
*/
#define	CAM_PAN_TILT_LEN	(9)

/**
* @	��������Խ�����ռ�ֽ���Ϊ4���ֽ�
*/
#define	CAM_ZOOM_LEN		(4)




/**
* @	Ԥ��λ��������
*/
#define PRESET_NUM_MAX					(7)


/**
* @	�̶���Ϣ��
*/
#define FIXED_MSG						(0xFFF0)


//==============================���ò���============================
/**
* @	�趨���ٷ�Χ����Ϣ��
*/
#define SET_TRACK_RANGE					(0x0001)

/**
* @	�趨����������Ϣ��
*/
#define SET_TRIGGER_RANGE				(0x0002)

/**
* @	������ͷ��ĸ����ߵĸ�				
*/
#define	SET_TRACK_LIMIT_HEIGHT			(0x0003)

/**
* @	����������Ϣ�� 
*/
#define SET_TRACK_TYPE					(0x0004)


/**
* @	���û�������,������,����ٿ���,����������,�켣�ߵ�
*/
#define SET_DRAW_LINE_TYPE				(0x0005)

/**
* @	��������ͷ�����ٶȵĻ�����Ϣ�� 
*/
#define SET_CAM_SPEED_TYPE				(0x0006)

/**
* @	���ô�����������Ϣ�� 
*/
#define SET_TRIGGER_NUM_TYPE			(0x0007)

/**
* @	����Ԥ��λ��Ϣ�� 
*/
#define SET_PRESET_POSITION_TYPE		(0x0008)

/**
* @	��������ͷ����������λ��Ϣ�� 
*/
#define SET_LIMIT_POSITION_TYPE			(0x0009)

/**
* @	����Ԥ��λ��Ϣ��
*/
#define CALL_PRESET_POSITION_TYPE		(0x000a)

/**
* @	���ø��پ���
*/
#define SET_TRACK_DISTANCE_TYPE			(0x000b)

/**
* @	���ø��ٻ��Ƿ����Ŀ���
*/
#define SET_TRACK_IS_ENCODE				(0x000c)

/**
* @ ������ٲ�����Ϣ��
*/
#define SET_TRACK_PARAM					(0x000d)

/**
* @ ����Ԥ����ʱ��
*/
#define SET_START_TRACK_TIME			(0x000e)

/**
* @ ���ø��ٸ�λʱ��
*/
#define SET_RESET_TIME					(0x000f)


/**
* @ ���ü��仯ϵ��sensֵ
*/
#define SET_SENS_VALUE					(0x0010)

/**
* @ �����ֶ�����
*/
#define SET_MANUAL_COMMOND				(0x0012)

/**
* @ �������ø���ģʽ
*/
#define SET_TRACK_MODE					(0x0013)

/**
* @ ���û�λ
*/
#define SET_STRATEGY_NO					(0x0018)



/**
* @ ����ѧ��ȫ������ʱ��
*/
#define SET_STUDENTS_PANORAMA_SWITCH_NEAR_TIME				(0x001a)

/**
* @ ������ʦ�ߵ�����1�����а����ʱ��
*/
#define SET_TEACHER_BLACKBOARD_TIME1				(0x001b)

/**
* @ �����뿪����1��������ʦ��ʱ��
*/
#define SET_TEACHER_LEAVE_BLACKBOARD_TIME1				(0x001c)

/**
* @ ����ѧ���½�̨�����ø��ٵ�ʱ��
*/
#define SET_STUDENTS_DOWN_TIME				(0x001d)

/**
* @ ������ʦ��������ͷ�ƶ��л���ʦȫ����ʱ��
*/
#define SET_TEACHER_PANORAMA_TIME				(0x001e)

/**
* @ ������ʦ����ͷֹͣ�ƶ��л���ʦ������ʱ��
*/
#define SET_TEACHER_LEAVE_PANORAMA_TIME				(0x001f)

/**
* @ ����ѧ���Ͻ�̨�󱣳���ʦȫ�����ʱ��
*/
#define SET_TEACHER_KEEP_PANORAMA_TIME				(0x0020)

/**
* @ ������ʦ�л�Ϊѧ��ǰ����ʦ��ͷ����ʱ��
*/
#define SET_TEACHER_SWITCH_STUDENTS_DELAY_TIME				(0x0021)

/**
* @ ����ѧ���������ֵ�ʱ��
*/
#define SET_STUDENTS_NEAR_KEEP_TIME				(0x0022)

/**
* @ ������ʦ����ʱ��ʦȫ����ѧ��ȫ���л���ʱ
*/
#define SET_MV_KEEP_TIME				(0x0023)

/**
* @ ����vga����ʱ��
*/
#define SET_VGA_KEEP_TIME				(0x0024)

/**
* @ ������ʦ�ߵ�����2�����а����ʱ��
*/
#define SET_TEACHER_BLACKBOARD_TIME2				(0x0025)

/**
* @ �����뿪����2��������ʦ��ʱ��
*/
#define SET_TEACHER_LEAVE_BLACKBOARD_TIME2				(0x0026)




/**
* @ �ڰ����򴥷���Χ
*/
#define SET_BLACKBOARD_TRIGGER				(0x0027)



/**
* @������������ͣ�����ͬ�ͺ��������ѡ��
*/
#define	SET_CAM_TYPE					(0x0028)

/**
* @ ����ѧ����ѯ��Ϣ
*/
#define SET_TRACK_TURN_INFO				(0x002a)

/**
* @ ���ð��鷽��
*/
#define SET_BLACKBOARD_SCHEME				(0x002b)


/**
* @	���ö�λ���������λ�ý��ҵĳ����
*/
#define	SET_POSITION_CAM_INFO			(0x0040)


/**
* @	���ø������������λ�õĳ����
*/
#define	SET_TRACK_CAM_INFO				(0x0041)


/**
* @	���ø��������ˮƽ����ֱ�������ƫ��ֵ
*/
#define	SET_TRACK_CAM_OFFSET			(0x0042)

/**
* @	���ñ�����IP��Ϣ
*/
#define	SET_LOCAL_IP_INFO					(0x4D)

/**
* @	����ѧ������IP��Ϣ
*/
#define	SET_STUDENT_IP_INFO					(0x4E)

/**
* @	���ð����0��IP��Ϣ
*/
#define	SET_BLOCKBOARD_0_IP_INFO					(0x4F)

/**
* @	���ð����1��IP��Ϣ
*/
#define	SET_BLOCKBOARD_1_IP_INFO					(0x50)

/**
* @	���ö��涨λ�������λ��
*/
#define	SET_OTHER_CAM_POSITION			(0x0043)


/**
* @	���ø��ٻ�����(�������ٻ����Ǹ�ѧ����)
*/
#define	SET_HOST_TYPE					(0x0044)


/**
* @	���ñ���ˢ���Ƿ�ˢ�£��Լ�ˢ��ʱ��
*/
#define	SET_REFRESH_BACKGROUND			(0x0045)

/**
* @	�ָ���������
*/
#define	SET_DEFAULT_PARAM					(0x0046)

/**
* @	�����Ƿ�ʼ����ʦԤ��λ����
*/
#define	SET_HIGH_PRESET					(0x0047)

/**
* @	���ö�ʧģʽ����ʦ��ʧ�����ģʽ
*/
#define	SET_LOST_MODE					(0x0048)

/**
* @ ������ʦ��д������ϱ��ص�Y����
*/
#define SET_FEATURE_VIEW_Y_START				(0x0049)



//=========================��ȡ����130��ʼ====================
/**
* @	��ȡ���ٷ�Χ����Ϣ��
*/
#define GET_TRACK_RANGE					(0x0100)

/**
* @	��ȡ����������Ϣ��
*/
#define GET_TRIGGER_RANGE				(0x0101)

/**
* @	��ȡ��ͷ��ĸ����ߵĸ�				
*/
#define	GET_TRACK_LIMIT_HEIGHT			(0x0102)

/**
* @	��ȡ��������				
*/
#define	GET_TRACK_TYPE					(0x0103)

/**
* @	��ȡ��������				
*/
#define	GET_DRAW_LINE_TYPE				(0x0104)

/**
* @	��ȡ����ͷ�ٶȻ���			
*/
#define	GET_CAM_SPEED_TYPE				(0x0105)

/**
* @	��ȡ����������			
*/
#define	GET_TRIGGER_NUM_TYPE			(0x0106)

/**
* @	��ȡ���پ���			
*/
#define	GET_TRACK_DISTANCE_TYPE			(0x0107)

/**
* @	��ȡ���ٻ��Ƿ����Ŀ���
*/
#define GET_TRACK_IS_ENCODE				(0x0108)

/**
* @ �������ٲ�����Ϣ��
*/
#define GET_TRACK_PARAM					(0x0109)

/**
* @ ��ȡԤ����ʱ��
*/
#define GET_START_TRACK_TIME			(0x010a)

/**
* @ ��ȡ���ٸ�λʱ��
*/
#define GET_RESET_TIME					(0x010b)

/**
* @ ��ȡ���仯ϵ��sensֵ
*/
#define GET_SENS_VALUE					(0x010c)

/**
* @��ȡ����ģʽ
*/
#define GET_TRACK_MODE					(0x010d)

/**
* @��ȡ��λ
*/
#define GET_STRATEGY_NO					(0x0112)



/**
* @ ��ȡѧ��ȫ������ʱ��
*/
#define GET_STUDENTS_PANORAMA_SWITCH_NEAR_TIME				(0x0114)

/**
* @��ȡ��ʦ�ߵ�����1�����а����ʱ��
*/
#define GET_TEACHER_BLACKBOARD_TIME1				(0x0115)

/**
* @ ��ȡ�뿪����1��������ʦ��ʱ��
*/
#define GET_TEACHER_LEAVE_BLACKBOARD_TIME1				(0x0116)

/**
* @ ��ȡѧ���½�̨�����ø��ٵ�ʱ��
*/
#define GET_STUDENTS_DOWN_TIME				(0x0117)

/**
* @ ��ȡ��ʦ��������ͷ�ƶ��л���ʦȫ����ʱ��
*/
#define GET_TEACHER_PANORAMA_TIME				(0x0118)

/**
* @ ��ȡ��ʦ����ͷֹͣ�ƶ��л���ʦ������ʱ��
*/
#define GET_TEACHER_LEAVE_PANORAMA_TIME				(0x0119)

/**
* @ ��ȡѧ���Ͻ�̨�󱣳���ʦȫ�����ʱ��
*/
#define GET_TEACHER_KEEP_PANORAMA_TIME				(0x011A)

/**
* @��ȡ��ʦ�л�Ϊѧ��ǰ����ʦ��ͷ����ʱ��
*/
#define GET_TEACHER_SWITCH_STUDENTS_DELAY_TIME				(0x011B)

/**
* @ ��ȡѧ���������ֵ�ʱ��
*/
#define GET_STUDENTS_NEAR_KEEP_TIME				(0x011C)

/**
* @ ��ȡ��ʦ����ʱ��ʦȫ����ѧ��ȫ���л���ʱ
*/
#define GET_MV_KEEP_TIME				(0x011D)

/**
* @��ȡvga����ʱ��
*/
#define GET_VGA_KEEP_TIME				(0x011E)


/**
* @��ȡ��ʦ�ߵ�����2�����а����ʱ��
*/
#define GET_TEACHER_BLACKBOARD_TIME2				(0x011f)

/**
* @ ��ȡ�뿪����2��������ʦ��ʱ��
*/
#define GET_TEACHER_LEAVE_BLACKBOARD_TIME2				(0x0120)

/**
* @ ��ȡ�ڰ崥����Χ
*/
#define GET_BLACKBOARD_TRIGGER			(0x0121)


/**
* @��ȡ��������ͣ�����ͬ�ͺ��������ѡ��
*/
#define	GET_CAM_TYPE					(0x0122)

/**
* @ ��ȡѧ����ѯ��Ϣ
*/
#define GET_TRACK_TURN_INFO				(0x0124)

/**
* @ ��ȡ���鷽��
*/
#define GET_BLACKBOARD_SCHEME				(0x0125)

/**
* @	��ȡ��λ���������λ�ý��ҵĳ����
*/
#define	GET_POSITION_CAM_INFO			(0x0130)


/**
* @	��ȡ�������������λ�õĳ����
*/
#define	GET_TRACK_CAM_INFO				(0x0131)


/**
* @	��ȡ���������ˮƽ����ֱ�������ƫ��ֵ
*/
#define	GET_TRACK_CAM_OFFSET			(0x0132)


/**
* @	��ȡ���涨λ�������λ��
*/
#define	GET_OTHER_CAM_POSITION			(0x0133)



/**
* @	��ȡ���ٻ�����(�������ٻ����Ǹ�ѧ����)
*/
#define	GET_HOST_TYPE					(0x0134)

/**
* @	��ȡ����ˢ���Ƿ�ˢ�£��Լ�ˢ��ʱ��
*/
#define	GET_REFRESH_BACKGROUND			(0x0135)

/**
* @	��ȡ�Ƿ�ʼ����ʦԤ��λ����
*/
#define	GET_HIGH_PRESET			(0x0136)

/**
* @	��ȡ������IP��Ϣ
*/
#define	GET_LOCAL_IP_INFO					(0x0137)

/**
* @	��ȡѧ������IP��Ϣ
*/
#define	GET_STUDENT_IP_INFO					(0x0138)

/**
* @	��ȡ�����0��IP��Ϣ
*/
#define	GET_BLOCKBOARD_0_IP_INFO					(0x0139)

/**
* @	��ȡ�����1��IP��Ϣ
*/
#define	GET_BLOCKBOARD_1_IP_INFO					(0x013A)


/**
* @	��ȡϵͳ����������Ϣ
*/
#define	GET_NETWORK_CONNECT_STATUS					(0x013C)

/**
* @	�����豸
*/
#define	REBOOT_DEVICE					(0x013D)


#define	MSG_SET_TRACK_PARAM	0xA3	//��ʦ���ٲ�������

typedef enum
{
	PANORAMA_PRESET	=1,	//ȫ��Ԥ��λ	
	LOW_TEACHER_PRESET = 2,//����ʦԤ��λ
	HIGH_TEACHER_PRESET = 3, //����ʦԤ��λ
	BIG_PANORAMA_PRESET = 6, //��ȫ��Ԥ��λ
}PRESET_POSITION;

/**
* @	��ʦ���˺��л�ģʽ��
0��ʾ�����٣���ʦ��������ʦȫ��(ȫ������)/ѧ��ȫ��(��ȫ������)
1��ʾ��������ѧ������ʦ������ѧ��ȫ��
2��ʾ��������ѧ������ʦ��������ʦȫ��
*/

typedef enum
{
	LOST_NOTRACK = 0,
	LOST_TRACK = 1,
	LOST_TRACK_TEACH = 2, 
}LOSTMODE;


/**
* @	�����ת����������
*/
typedef enum _cam_direct_type
{
	CAM_DIRECT_STOP = 0,
	CAM_DIRECT_LEFT,
	CAM_DIRECT_RIGHT,
	CAM_DIRECT_NUM
}cam_direct_type_e;


typedef struct _stTeacherConnectState    //��ʦ������״̬   1����   0�Ͽ�
{
	int iStudent;                        //��ѧ������
	int iBroad1;                         //�����1����
	int iBroad2;                         //�����2����
	int iRec;                            //��¼������
	char cIPStudent[32];	
	char cIPBroad1[32];	
	char cIPBroad2[32];	
}ST_TEACHER_CONNECT_STATE;



/**
* @ ENC110����������Ϣͷ�ṹ
*/
typedef struct _msg_header_110
{
	unsigned char	nMsg;					//��Ϣ����
	unsigned short	nLen;					//ͨ��htonsת����ֵ,�����ṹ�屾�������
	
} msg_header_110_t;

/**
* @	���������� 
*/
typedef struct _point_info
{
	unsigned short x;	//x����
	unsigned short y;	//y����
}point_info_t;


/**
* @ ������Ϣͷ�ṹ
*/
typedef struct _track_header
{
	unsigned short len;
	unsigned short fixd_msg;		//�̶���Ϣ��
	unsigned short msg_type;		//��Ϣ����
	unsigned short reserve;			//Ԥ��
}track_header_t;

/**
* @ ��������ͷ��ʽ
*/
typedef enum _control_cam_mode
{
	AUTO_CONTROL = 0,		//�Զ�����
	MANUAL_CONTROL,			//�ֶ�����
	MAX_NUM_CONTROL
}control_cam_mode_e;



/**
* @ ���ƻ��ߵ�����
*/
typedef enum _draw_line_type
{
	DRAW_NO = 0,			//������
	DRAW_LINE = 1,			//����
	MAX_NUM_DRAW
}draw_line_type_e;




/**
* @	�Զ����ֶ�����
*/
typedef struct _control_type_info
{
	unsigned char 	state;			//���û�������
	control_cam_mode_e 	control_type;	//��������0���Զ�����,1Ϊ�ֶ�����
}control_type_info_t;


/**
* @	���ٿ���Ϣ
*/
typedef struct _track_range_info
{
	unsigned char state;						//���û�������
	unsigned char point_num;					//ȷ����������ĵ���
	unsigned short video_width;					//��Ƶ�Ŀ�
	unsigned short video_height;				//��Ƶ�ĸ�
	point_info_t point[TRACK_RANGE_NUM];		//���ٿ�
}track_range_info_t;

/**
* @	��������Ϣ
*/
typedef struct _trigger_range_info
{
	unsigned char state;						//���û�������
	unsigned char point_num;					//ȷ����������ĵ���
	unsigned short video_width;					//��Ƶ�Ŀ�
	unsigned short video_height;				//��Ƶ�ĸ�
	point_info_t point[TRACK_TRIGGER_NUM];		//������
}trigger_range_info_t;

/**
* @	���ø��ٻ��Ƿ����(Э�����õĽṹ��)
*/
typedef struct _track_is_encode_info
{
	unsigned char 	state;			//���û�������
	unsigned short 	isencode;		//���ٻ��Ƿ����ı�־,0��ʾ������,1��ʾ����
}track_is_encode_info_t;


/**
* @ �ͱ����йص�һЩȫ�ֱ���(����ȫ�ֱ������õ��Ľṹ��)
*/
typedef struct _track_encode_info
{
	short	is_encode;		//Ϊ0��ʾ������,Ϊ1��ʾ����
	int		is_track;			//�Ƿ����1��ʾ����,0��ʾ������
	int		is_save_class_view;	//�����ǰȡ��ͼƬ��1��Ҫ���棬0����Ҫ����
	int		students_status;	//ѧ��վ��״̬��1Ϊվ����2Ϊ����վ����0Ϊ����
	int		last_students_status;	//�ϴ�ѧ��վ��״̬��1Ϊվ����2Ϊ���£�0Ϊ�����κβ���
	int		host_type;				//�������ͣ�0Ϊ��ѧ������1Ϊ��ѧ����
	int		send_cmd;				//�������ͣ����л���ʦ����ѧ��
	int		nTrackSwitchType;		//�������ͣ�0��ʾ���٣�1��ʾֻ�л�������
	int		track_status;			//���ٻ�״̬0��ʾδ������,1��ʾ������
	int		refresh_time;			//����ˢ��ʱ�䵥λ���룬0��ʾ��ˢ�±���
	int		record_flag;//�Ѿ���ʼ¼�Ʊ�־
	int		call_position_no;//ÿ��Ҫ���õ�Ԥ��λ��
	int		detect_high_teach;//��⵽����ʦ��־����ʾ�㷨�Ƿ��⵽�˸���ʦ����
	int		highpreset;//����ʦ��־����ʾ�Ƿ����ø���ʦ���
}track_encode_info_t;




/**
* @	���ü��仯��ϵ��sensֵ
*/
typedef struct _sens_info
{
	unsigned char 	state;				//���û�������
	unsigned short 	sens;				//sensֵ
}sens_info_t;


/**
* @	����ģʽ���ã��Ǻ�ԭʼͼ��Ƚϻ��Ǻ���֡ͼ��Ƚ�
*/
typedef struct _track_mode_info
{
	unsigned char 	state;			//���û�������
	int16_t 	track_mode;	//��������0���Զ�����,1Ϊ�ֶ�����
}track_mode_info_t;

/**
* @	�����ֶ��������
*/
typedef struct _manual_commond_info
{
	unsigned char 	state;			//���û�������
	unsigned short 	type;			//�ֶ������е������ͣ���СΪ0-65535
	unsigned short 	value;			//�ֶ������е�ֵ����СΪ0-65535
}manual_commond_info_t;


/**
* @	�������ߵĿ���
*/
typedef struct _draw_line_info
{
	unsigned char 		state;		//���û�������
	draw_line_type_e 	message;	//message����
}draw_line_info_t;

/**
* @	���ô���������
*/
typedef struct _trigger_num_info
{
	unsigned char 		state;		//���û�������
	unsigned short 	trigger_num;	//����������,�����������⵽����ô����㴥������Ϊ�Ǵ���
}trigger_num_info_t;


/**
* @ ģ�������Ϣ 
*/
typedef struct _model_classroom_info
{
	int 	height;					//�������ģ��Ĵ�ֱ����
	int 	width[CAM_ANGLE_NUM];	//ģ��ȡ���봹�ĵľ���
	double	angle[CAM_ANGLE_NUM];	//������봹�ߵļн�
}model_classroom_info_t;


/**
* @	���ò�������
*/
typedef enum	_set_cmd_type
{
	SET_PRESET_POSITION = 1,
	SET_LIMITE_POSITION = 2,
	GET_CUR_POSITION
}set_cmd_type_e;


/**
* @	����Ԥ��λ
*/
typedef struct _preset_position_info
{
	unsigned char 		state;		//���û�������
	unsigned short 		preset_position;//���õ�Ԥ��λ,��Χ��1��255
}preset_position_info_t;

/**
* @ ������������ϴ�����������Ϣ
*/
typedef struct _track_save_file_info
{
	pthread_mutex_t save_track_m;	//�����޸ĸ��ٻ������ļ�ʱ�ӵ���
	set_cmd_type_e		set_cmd;	//���������������
	int		cmd_param;				//�������ϸֵ
}track_save_file_info_t;




/**
* @	������ͺ�����
*/
typedef enum _cam_type_info
{
	BLM_500R		= 0,		//��׼VISCAЭ�飬��ʤ�Ǵ�������
	SONY_BRC_Z330	= 1,		//SONY��BRC-Z330Э�������Э��
	HITACHI_CAM 	= 2,		//�����������֧��
	KXWELL_CAM		= 3,		//���������������֧��
	SONY_D100		= 4,		//����D100
	VHD_V100N		= 5,		//ӭ�紫ѶVHD-V100N�����
	SONY_H100S		= 6,		//SONY EVI_H100S�����
	HUAWEI		= 7,		//HUAWEI�����
	VHD_PTZ		= 8,		//    VHD_PTZ
	SONY_HD1    = 9,
	MAX_CAMNUM,
	CAM_NUM
}cam_type_info_e;



/**
* @ �������Ԥ��λ����Ϣ
*/
typedef struct _cam_preset_position_info
{
	
	unsigned char pan_tilt[CAM_PAN_TILT_LEN];	//ǰ�ĸ��ֽ�Ϊ����ͷˮƽλ��,���ĸ��ֽ�Ϊ����ͷ��ֱλ��
	unsigned char zoom[CAM_ZOOM_LEN];		//����ͷzoomλ��,�����ĸ��ֽ�����
	int cur_preset_value;		//�ϴ�ִ�е�Ԥ��λֵ
}cam_preset_position_info_t;


/**
* @	�����йص�ȫ�ֲ���
*/
typedef struct _cam_control_info
{
	//int		cam_position_value;				//Ԥ��λ��
	cam_preset_position_info_t	cam_position[PRESET_NUM_MAX];
	//unsigned char	cam_addr;						//�������ַ
}cam_control_info_t;


/**
* @	���û��ȡ���������Э��
*/
typedef struct _cam_protocol_type_info
{
	unsigned char		state;		//���û�������
	unsigned short		cam_type;	//�����Э������
}cam_protocol_type_info_t;


/**
* @	���û��ȡ��λ��������ڽ��ҵĳ����(�߱�ʾ��λ������ĸ�)
*/
typedef struct _position_cam_info
{
	unsigned char		state;		//���û�������
	unsigned short		lenth;		//���Ҷ�λ������������ҵĳ���
	unsigned short		width;		//��������λ������ľ���
	unsigned short		height;		//���Ҷ�λ������ĸ߶�
}position_cam_info_t;


/**
* @	���û��ȡ������������ڽ��ҵĳ����(�߱�ʾ��λ������ĸ�)
*/
typedef struct _track_cam_info
{
	unsigned char		state;		//���û�������
	unsigned short		lenth;		//���Ҷ�λ������������ҵĳ���
	unsigned short		width;		//��������λ������ľ���
	unsigned short		height;		//���Ҷ�λ������ĸ߶�
}track_cam_info_t;


/**
* @	���û��ȡ���������ˮƽ����ֱ���򣬽����ƫ��ֵ
*/
typedef struct _track_cam_offset_info
{
	unsigned char		state;		//���û�������
	short		pan;		//���������ˮƽ�����ϵĽǶ�ƫ��ֵ
	short		tilt;		//�����������ֱ�����ϵĽǶ�ƫ��ֵ
	short		zoom;		//�������������ƫ��ֵ
}track_cam_offset_info_t;


/**
* @	���û��ȡѧ������IP��Ϣ
*/
typedef struct _track_ip_info
{
	unsigned char		state;		///1:����; 0:��������
	char 				ip[16];		//IP��ֵ
	char 				mask[16];		//mast��ֵ
	char 				route[16];		//route��ֵ
}track_ip_info_t;


/**
* @	��������һ����λ�����λ����Ϣ
*/
typedef struct _other_position_cam_info
{
	unsigned char		state;	//���û�������
	unsigned short		x;		//����һ����λ������ڱ�������е�x����
	unsigned short		y;		//����һ����λ������ڱ�������е�y����
	unsigned short		width;	//����һ����λ������ڱ�������еĿ�
	unsigned short		height;	//����һ����λ������ڱ�������еĸ�
}other_position_cam_info_t;


/**
* @	������������
*/
typedef struct _track_host_type
{
	unsigned char		state;	//���û�������
	unsigned short		host_type;		//��������0Ϊ��ѧ������1Ϊ��ѧ����
}track_host_type_t;


/**
* @ �����ٶȼ���
*/
typedef enum _cam_speed_level
{
	SPEED_LEVEL_0 = 0,
	SPEED_LEVEL_1 = 1,
	SPEED_LEVEL_2 = 2,
	SPEED_LEVEL_3 = 3,
	SPEED_LEVEL_4 = 4,
	SPEED_LEVEL_5 = 5,
	SPEED_LEVEL_NUM
}cam_speed_level_t;

/**
* @	����������λ�ĵķ���
*/
typedef enum _limit_position_type
{
	LIMIT_DOWN_LEFT = 0,
	LIMIT_UP_RIGHT,	// 1
	LIMIT_CLEAR, // 2
	LIMIT_KXWELL_UP,  // 3
	LIMIT_KXWELL_DOWN,// 4
	LIMIT_KXWELL_LEFT,// 5
	LIMIT_KXWELL_RIGHT //6
}limit_position_type_e;



/**
* @	��ż���ǶȽ������Ϣ,���п�ȣ��߶ȣ����ȣ�����������λ������ڵ���
* @  �ڵ����ϵ�ӳ��Ϊ0���
*/
typedef struct _angle_info
{
	int		cam_addr;				//����������ĵ�ַλ
	cam_type_info_e		cam_type;	//���������
	unsigned short cam_speed;		//����ͷת���ٶ�
	int		last_cam_cmd;			//�ϴ�����������͵�����
	int		last_cam_speed;			//�ϴ�����������͵��ٶ�
	int		track_cam_cur_pan;		//�����������ǰˮƽ����λ��ֵ
	int		other_cam_x;			//����һ�����������ͼ���е�xֵ
	int		other_cam_y;			//����һ�����������ͼ���е�yֵ
	int		other_cam_width;		//����һ�����������ͼ���еĿ�
	int		other_cam_height;		//����һ�����������ͼ���еĸ�
	int		host_status;			//��ѧ����״̬
	int		aux_status;				//��ѧ����״̬
	int		last_host_status;		//��ѧ�����ϴ�״̬
	int		last_aux_status;		//��ѧ�����ϴ�״̬
	int		position_camera_width;	//���ҿ��(��x����)
	int		position_camera_lenth;	//���ҳ���(��y����)
	int		position_camera_height;	//��λ������߶�(��z����)
	int		track_camera_width;		//����������Ŀ��(��x����)
	int		track_camera_lenth;		//����������ĳ���(��y����)
	int		track_camera_height;	//����������߶�(��z����)
	double	camera_pan_offset;		//�����ˮƽ�����ƫ�ƽǶ�
	double	camera_tilt_offset;		//�������ֱ�����ƫ��
	int		camera_zoom_offset;		//��������෽���ƫ��
	double 	host_angle;			//��ѧ�����Ƕ�
	double	host_base_angle;	//����������������ͼ���е�λ�������������������������ĽǶ�
	double  aux_angle;			//��ѧ�����Ƕ�
	double	aux_base_angle;		//��������ڸ������ͼ���е�λ���븨�������������������ĽǶ�
}angle_info_t;

/**
* @	��kiteͨѶ��������������Ϣ
*/
typedef struct _broadcast_range_info
{
	unsigned char state;						//���û�������
	unsigned char point_num;					//ȷ����������ĵ���
	unsigned short video_width;					//��Ƶ�Ŀ�
	unsigned short video_height;				//��Ƶ�ĸ�
	point_info_t point[TRACK_BROADCAST_POINT_NUM];		//������
}broadcast_range_info_t;

/**
* @	��������Ϣ
*/
typedef struct _broadcast_info
{
	point_info_t point[ONE_BROADCAST_POINT_NUM];				
}broadcast_info_t;

/**
* @	����͸��ٲ����йصĲ���
*/
typedef struct _track_tactics_info
{
	broadcast_info_t	broadcast_info[TRACK_BROADCAST_NUM];
	broadcast_info_t	teacher_feature_info[TRACK_BROADCAST_NUM];
}track_tactics_info_t;


/**
* @	�������úڰ������Լ���ȡʱ��ֵ��ͨ�ýṹ��
*/
typedef struct _default_msg_info
{
	unsigned char 	state;		//���û�������
	unsigned short 	value;	
}default_msg_info_t;


/**
* @ ������Ϣͳ�ƽṹ��.�ϱ���web����Ϣ�ṹ��
*/
typedef struct _send_class_info
{
	unsigned short  nClassType;//�ڿ�����
	unsigned short  nUpTimesSum;//�����ܴ���
	unsigned short	nStandupPos[4];			//ǰ����  �ң�  �������ҷֱ��Ӧ��������
	unsigned short	nUpToPlatformTimes;			//ѧ����̨�ܴ���
	unsigned short	nTeacherToStudentsAreaTimes;//��ʦ��̨�ܴ���
	unsigned short  nPerTimeLen;//ÿ��ʱ��γ���
	unsigned short 	nAllTimesNum;//�ܹ���ʱ�����
	unsigned short  nInteractionNum[MAX_INTERACTION_NUM];//�ֱ��Ǹ���ʱ��εĻ�Ծ����
	unsigned short 	nStandupTimePoint[MAX_STANDUP_NUM];//�ֱ��Ǹ�����������������ʱ���
}send_class_info_t;

/**
* @ ������Ϣͳ�ƽṹ��.�ϱ���web����Ϣ�ṹ��
*/
typedef struct _final_class_info
{
	send_class_info_t tSendClassInfo;
	unsigned int nClassInfoFlag;
	unsigned long long nClassStartTime;
	unsigned long long nClassEndTime;
}final_class_info_t;


/**
* @	��������ͷ�ٶȻ���
*/
typedef struct _cam_speed_info
{
	unsigned char 		state;		//���û�������
	unsigned short 	cam_speed;		//����ͷ�ٶȻ���,����Ҫ�ƶ��ľ�����Դ�ֵ,��Ϊ����ͷ�ٶ�
}cam_speed_info_t;


/**
* @	������λ����
*/
typedef struct _limit_position_info
{
	unsigned char 			state;			//���û�������
	//limit_position_type_e 	limit_position;	//��������ͷ���������ҵ�����ƶ�λ��
	unsigned short 	limit_position;	//��������ͷ���������ҵ�����ƶ�λ��
}limit_position_info_t;



/**
* @	�л������������
*/
typedef enum _switch_cmd_author_info
{//����ѧ����������޸ģ�ֻ�ܴӺ����ۼ�ѧ������ֵ��Ҫ�����ۼӣ����ProcessMsgTrackAuto �����÷�
	AUTHOR_TEACHER	= 1,	//�л���������ʦ�����
	AUTHOR_STUDENTS	= 2,	//��ѧ�������͵��л�����
	AUTHOR_NUM
}switch_cmd_author_info_e;


/**
* @	���λ����������Ϣ��
*/
typedef struct _track_strategy_info
{
	int	left_pan_tilt1;	//����1���	//ǰ�ĸ�����ǰ����ֽ�Ϊ����ͷˮƽλ��,���ĸ��ֽ�Ϊ����ͷ��ֱλ�ã���ʾ�ڰ�����߶�Ӧ�����λ��
	int	right_pan_tilt1;//����1�ұ�		//ǰ�ĸ�����ǰ����ֽ�Ϊ����ͷˮƽλ��,���ĸ��ֽ�Ϊ����ͷ��ֱλ�ã���ʾ�ڰ����ұ߶�Ӧ�����λ��
	int	left_pan_tilt2;	//����2���	//ǰ�ĸ�����ǰ����ֽ�Ϊ����ͷˮƽλ��,���ĸ��ֽ�Ϊ����ͷ��ֱλ�ã���ʾ�ڰ�����߶�Ӧ�����λ��
	int	right_pan_tilt2;//����2�ұ�		//ǰ�ĸ�����ǰ����ֽ�Ϊ����ͷˮƽλ��,���ĸ��ֽ�Ϊ����ͷ��ֱλ�ã���ʾ�ڰ����ұ߶�Ӧ�����λ��
	int cur_pan_tilt;		//ǰ�ĸ�����ǰ����ֽ�Ϊ����ͷˮƽλ��,���ĸ��ֽ�Ϊ����ͷ��ֱλ�ã���ʾ�������ǰ������λ��
	int	blackboard_region_flag1;//�Ƿ��ڰ��������־��1Ϊ�ڰ�������0Ϊ���ڰ�������
	int	blackboard_region_flag2;//�Ƿ��ڰ��������־��1Ϊ�ڰ�������0Ϊ���ڰ�������
	int	students_panorama_switch_near_time;		//ѧ��վ��������ѧ��ȫ�������������ѧ����д�������ʱ�䣬��λ����
	int	teacher_blackboard_time1;				//��ʦͣ���ںڰ������ʱ��������ʱ�伴��Ϊ����ʦ�ںڰ����򣬵�λ����
	int	teacher_leave_blackboard_time1;			//��ʦ�뿪�ڰ������ʱ��������ʱ�伴��Ϊ����ʦ���ںڰ����򣬵�λ����
	int	teacher_blackboard_time2;				//��ʦͣ���ںڰ������ʱ��������ʱ�伴��Ϊ����ʦ�ںڰ����򣬵�λ����
	int	teacher_leave_blackboard_time2;			//��ʦ�뿪�ڰ������ʱ��������ʱ�伴��Ϊ����ʦ���ںڰ����򣬵�λ����
	int	teacher_panorama_time;					//��ʦ�ƶ���೤ʱ���л���ʦȫ������λ����
	int	teacher_leave_panorama_time;			//��ʦͣ������ú��л�����ʦ��д����λ����
	int	teacher_keep_panorama_time;				//��ʦȫ����ͷ�����ʱ�䣬��λ����
	int	move_flag;								//�ƶ���־,��ʦ���ٻ�����ͷ�ƶ���־
	int	send_cmd;		//��ʦ����ǰҪ���͵�����
	int	last_send_cmd;		//��һ����ʦ�����͵�����
	switch_cmd_author_info_e	switch_cmd_author;			//�л������������ʦ������ѧ����
	int teacher_panorama_flag;//��ʦȫ��,��ʾѧ���Ͻ�̨��,��Ŀ����ʱ��
	int	students_down_time;			//ѧ���½�̨ʱ��������ʱ�伴�ٴν���ѧ��������Ϣ����λ����
	int	students_track_flag;			//�Ƿ����ѧ�������Լ���̨��Ϣ��־����λ����
	int switch_vga_flag;//�Ƿ��л�vga
	int teacher_switch_students_delay_time;//��ʦ��ͷ�л���ѧ����ͷ��ʱ
	int	students_near_keep_time;			//ѧ��������ͷ����ʱ��
	int	vga_keep_time;//vga �׻��汣��ʱ��,�������ʱ��,����vga��ͷ�л�����ľ�ͷ
	int cam_left_limit;//����λ
	int cam_right_limit;//����λ
	int position1_mv_flag;//������ʦ���˺����ƶ�Ŀ��
	int mv_keep_time;//��ʦ���ˣ������ƶ�Ŀ��ʱѧ��ȫ������ʦȫ����ͷ�л���ʱ
	int lostmodestutime;//��ʦ���ˣ�ѧ��ȫ������ʱ��
	int strategy_no;//��λ����Ŀǰ�����࣬3��λ��4\5\6��λ
	int cam_jump_flag;// 1��ʾ����Ԥ��λ1 (��ʦȫ��) ,2��ʾ��������ʦ������0��ʾû���κε��ã��������δ���ʱ��ʦ��������ͷ��ֱ�ӵ���ʦ�������Ժ�������ʦ
	int stu_num;//���ӵ�ѧ��������
	int lostmode;//��ʦ��ʧ�����ģʽ
	int	vga_keep_time_toserver;//vga ����ʱ��,���͸�server��
	int blackboard_scheme;//������ٷ���
	int blackboard_num;//���������������0��ʾ�ް����(��ʦ�����������)��1��ʾ�а����
	track_ip_info_t student_ip_info;	//ѧ������ip��Ϣ
	track_ip_info_t local_ip_info;	//��ʦ��������ip��Ϣ
	track_ip_info_t blackboard_ip_info[BLACKCONNECT_NUM_MAX];	//�������ip��Ϣ

	int	teacher_detect_heart_timeout;		//��ʦ���ģ�������ģ�������ĳ�ʱʱ��
}track_strategy_info_t;

/**
* @	���λ����������Ϣ��,����ͳ�� 
*/
typedef struct _track_strategy_timeinfo
{
	int	last_strategy_no;//��һ�εĻ�λ��,�������ǰ��һ��������س�ʼ����������
	int	students_panorama_switch_near_time;//ѧ��ȫ���л�ѧ��ǰ���ۻ�ʱ��,���ʱ������������Ե���ʱ����ѧ������
	int	blackboard_time1;//��ʦ�����������1��ʱ��,���ֻ��һ����������,���������1��Ч
	int	leave_blackboard_time1;//��ʦ�뿪��������1��ʱ��,���ֻ��һ����������,���������1��Ч
	int	blackboard_time2;//��ʦ�������2����ʱ��
	int	leave_blackboard_time2;//��ʦ�뿪����2����ʱ��
	int	panorama_time;//5��λ,��ʦȫ��ʱ��
	int	leave_panorama_time;//5��λ,�뿪��ʦȫ��ʱ��
	
	int	teacher_panorama_switch_near_time;//��ʦȫ���л�����ʦ����֮ǰ��ʱ��,ʱ��ͳ�Ƶ��˲������õ�ʱ����л���ʦ����
	int teacher_panorama_flag;//��ǰ��ͷ�Ƿ�����ʦȫ��
	int	students_down_time;//ѧ���½�̨��ʱ��ͳ��
	int	students_to_platform_time;//ѧ���Ͻ�̨��ʱ��ͳ��
	int	switch_flag;// 1�����л�,0 ��ʱδ�� �������л�
	int	switch_delaytime;//��׼��ʱʱ��,3s
	int	delaytime;//ÿ���л�����ʱʱ��,���ݲ�ͬ�л���ͷ,�õ�����ʱʱ��Ҳ��һ��
	int	on_teacher_flag;//��ǰ��ͷ�Ƿ�����ʦ��,������ʦ�Ͱ������ʦȫ��,������ʦ��ѧ��ʱ����ʱ����
	int	on_teacher_delaytime;//��ʦ��ͷ�ı���ʱ��
	int	vga_delaytime;//vga��ʱ��
	int	mv_time;//��ʦ��ʧʱ,�����˶�Ŀ���ʱ��
	int	stu_time;//��ʦ��ʧʱ,�л���ѧ��ȫ����ʱ��
	int stu_state[STUCONNECT_NUM_MAX];//���ѧ�����ĵ�ǰ״̬ ���ж��ѧ��ʱ�����õ�
	unsigned long long stu_uptime[STUCONNECT_NUM_MAX];//���ѧ������⵽����ʱ�䣬���һ������ʱ��
	int nStuStandFlag;//ѧ�������ˣ�������ʦ��ʧ���ѧ������

}track_strategy_timeinfo_t;


/**
* @ ��ѯ��ؽṹ��
*/
typedef struct _track_turn_info
{
	unsigned short 	nTrackTurnType;
	unsigned short 	nTrackTurnTime;
	unsigned short 	nTrackTurnShowTime;
}track_turn_info_t;


/**
* @	ѧ������ѯ��Ϣ
*/
typedef struct _stu_turn_info
{
	unsigned char 	state;		//���û�������
	track_turn_info_t 	tStuTrackTurnInfo[STUCONNECT_NUM_MAX];	
}stu_turn_info_t;


/**
* @	Ŀ������仯ȷ��ʱ����Ϣ
*/
typedef struct _target_change_time_info
{
	unsigned int	targettime0;		//Ŀ�궪ʧʱ�䣬��λ��֡
	unsigned int	targettime1;		//��һ��Ŀ���ʱ�䣬��λ��֡
	unsigned int	targettime2;		//�ж��Ŀ���ʱ�䣬��λ��֡
}target_change_time_info_t;


//#define		TRIGGER_NUM				(150)		//һ����������ڰ��������ڷֵĶ���
#define		TRIGGER_POINT_NUM		(30)		//һ��ˢ���������ܹ���⵽�Ĵ�������
//#define		DETECTION_SECTION_NUM	(50)		//����֡Ϊһ������
#define     DETECTION_TARGET    (100)        //���ٸ�Ŀ��
#define 	DETECTION_FRAMES_SECTION (20)       //ÿ�ζ���֡
#define 	DETECTION_SECTION_NUM (400)
typedef struct _target
{
	int x0;
	int y0;
	int x1;
	int y1;
	int trigger_flag[DETECTION_SECTION_NUM];
 
}Target;



/**
* @	����ˢ�±�����Ϣ
*/
typedef struct _nomal_info
{
	
	unsigned char 			state;			//���û�������
	unsigned short 			val0;		//0��ʾ�رգ�1��ʾ����
	unsigned short			val1;	//ˢ��ʱ�䣬��λ����
	unsigned short			val2;			//����
	unsigned short			val3;			//����
}nomal_info_t;

/**
* @	����ˢ�±�����Ϣ
*/
typedef struct _refresh_background_info
{
	
	unsigned char 			state;			//���û�������
	unsigned short 			is_open;		//0��ʾ�رգ�1��ʾ����
	unsigned short			refresh_time;	//ˢ��ʱ�䣬��λ����
	unsigned short			base1;			//����
	unsigned short			base2;			//����
}refresh_background_info_t;

/**
* @ ���������Ϣ
*/
typedef struct _trigger_point_info
{
	int		x0;
	int		y0;
	int		x1;
	int		y1;
	int		trigger_flag;		//����Ĵ�����־
}trigger_point_info_t;

/**
* @	���ص�����
*/
typedef struct _pixel_point_info
{	
	int16_t	x;		//x����
	int16_t	y;		//y����
}vertex_t;

/**
* @	��������Ϣ�Ľṹ��
*/
typedef struct _trigger_info
{
	int16_t	trigger_x0;		//��ʾ����������϶��������xֵ(��Դͼ��Ϊ��׼��)
	int16_t	trigger_y0;		//��ʾ����������϶��������yֵ(��Դͼ��Ϊ��׼��)
	int16_t	trigger_x1;		//��ʾ����������¶��������xֵ(��Դͼ��Ϊ��׼��)
	int16_t	trigger_y1;		//��ʾ����������¶��������yֵ(��Դͼ��Ϊ��׼��)
}trigger_info_t;


/**
* @	����controlʱ�Ƿ���Ҫ���³�ʼ����������
*/
typedef enum _control_init_type
{
	NO_INIT = 0,		//�������control
	RE_INIT,			//��Ҫ��ʼ�����ֲ���
	RE_START,			//��Ҫ��ʼ�����в���
	RE_INIT_NUM
}control_init_type_e;

/**
* @ ����Զ����ٵľ�̬����
*/
typedef struct 	_track_students_right_side_static_param
{
	int32_t	size;
	int16_t	video_width;			//������Ƶ�Ŀ�
	int16_t	video_height;			//������Ƶ�ĸ�
	int16_t	pic_width;				//�������ź�ͼ��Ŀ�
	int16_t	pic_height;				//�������ź��ͼ��ĸ�
}ITRACK_STUDENTS_RIGHT_SIDE_static_param_t;

/**
* @	������������õ�������
*/
#define	TRACK_STUDENTS_RIGHT_SIDE_AREA_POINT_MAX	(10)


/**
* @	���ٿ�������
*/
#define	TRACK_STUDENTS_RIGHT_SIDE_TRIGGER_MAX_NUM 	(2)	

/**
* @ ����Զ����ٵĶ�̬����
*/
typedef struct 	_track_students_right_side_dynamic_param
{
	int32_t	size;
	int16_t	trigger_sum;		//����������
	int16_t	track_mode;			//����ģʽ��0��ʾ�Ͳɼ��õ�ԭʼ֡�Ƚϣ�1��ʾ����֡�ȽϿ��Ƿ��б仯
	int16_t	start_track_time;	//��ֵʱ�䣬������೤ʱ�俪ʼ����,��λ��֡
	int16_t	reset_time;			//���ٶ�ʧ���´ο�ʼ������ʱ��
	int16_t	control_mode;	//��������,0Ϊ�Զ�,1Ϊ�ֶ�
	int16_t	sens;			//
	int16_t	message;		//��Ҫ�����趨����Щ�ߣ������Ƿ��˵������ߵ�,1:��ʾ���б�Ե��״̬��
							//2:����45�ȣ�90�ȣ�135�ȣ�180�ȵ��߻�������3:��ģ��;4:�����ٿ�;5:�����ٿ��ڵķ�ɫ��
	int16_t	track_point_num;//���ٿ�����ã�����м����㣬(������Щ�������ӳɿ�����˸�������)
	vertex_t	track_point[TRACK_STUDENTS_RIGHT_SIDE_AREA_POINT_MAX];	//����������x��y��������,�����������֮ǰͼ����˵
	trigger_info_t trigger[TRACK_STUDENTS_RIGHT_SIDE_TRIGGER_MAX_NUM];
	control_init_type_e	reset_level;	//0��ʾ��Ҫ���³�ʼ��,1��ʾ��Ҫ���³�ʼ��
	int16_t classroom_study_mode;	//����ѧϰģʽ��0��ʾ����ģʽ��1��ʾ����ģʽ
	int16_t student_enable_state;	//ѧ�����α�־��1 ���� 0 �ر�
}ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t;

/**
* @ ����Զ����ٵ��������
*/
typedef struct 	ITRACK_STUDENTS_RIGHT_SIDE_Params
{
	int32_t	size;
	ITRACK_STUDENTS_RIGHT_SIDE_static_param_t 	static_param;
	ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t	dynamic_param;
}ITRACK_STUDENTS_RIGHT_SIDE_Params;

int track_students_right_side_param(unsigned char *data, int socket);


extern trigger_point_info_t	g_trigger_info[TRIGGER_POINT_NUM];
extern unsigned long long 			g_refresh_start_time;



extern int init_dev_type(void);
//extern int classroom_location_calculation(ITRACK_STUDENTS_RIGHT_SIDE_OutArgs *output_param);
extern int jump_appoint_position(int cam_addr, int cam_pan, int cam_tilt, int fd);
extern int jump_appoint_zoom(int cam_addr, int zoom, int fd);
extern int set_cam_addr();
extern int save_position_zoom(unsigned char *data);
extern int save_position_zoom_with_network_data(int ePresetType, unsigned char *data);
extern int __get_cam_position(int fd);
extern int get_strategy_info_form_file(void);
extern int init_limit_position_set(void);
extern int teacher_target_validaation(int target_num, int x,int is_trigger,rect_t *rect);
extern int server_set_track_type(short type);
extern int server_set_classroom_study_mode(short mode);
#endif

