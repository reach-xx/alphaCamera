
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
#include "itrack_students.h"
/**
* @	�ļ����Ƶĳ���
*/
#define FILE_NAME_LEN		(64)
/**
* @ ѧ�����ٲ��������ļ�
*/
#define STUDENTS_TRACK_FILE		"students_track.ini"

#define DEBUG_INFO "debug_info"
#define DEBUG_IPADDR "debug_ipaddr"
#define DEBUG_LEVEL_VAL	"debug_level_val"

/**
* @ ��ʦ���ٵĶ�̬������������
*/
#define DYNAMIC_NAME			"dynamic"

/**
* @  ֧������BRCϵ�е��������VISCAЭ��
*/
//#define SUPORT_SONY_BRC_CAM


/**
* @	Ԥ��λ��������
*/
#define PRESET_NUM_MAX					(4)


/**
* @	�̶���Ϣ��
*/
#define FIXED_MSG						(0xFFF0)

//--------------------------�趨������Ϣ��------------------------

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


#define SET_CAM_ADDR                    (0x004d)

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
* @ ������������
*/
#define SET_SHIELD_RANGE				(0x0011)

/**
* @ �����ֶ�����
*/
#define SET_MANUAL_COMMOND				(0x0012)

/**
* @ ���ö�Ŀ����̨����
*/
#define SET_MULTITARGET_RANGE				(0x0013)


/**
* @ ��������ͷЭ������
*/
#define SET_CAM_TYPE				(0x0028)

/**
* @	���ô�����������Ŀ��			
*/
#define SET_TRACK_CAM_OFFSET   (0x0042)

/**
* @	�ָ���������
*/
#define	SET_DEFAULT_PARAM					(0x0046)


//-----------------------------���ز�����Ϣ��-------------------------

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
* @ ��ȡ��������
*/
#define GET_SHIELD_RANGE				(0x010d)
/**
* @ ��ȡ��Ŀ����̨����
*/
#define GET_MULTITARGET_RANGE				(0x010e)


/**
* @ ��ȡ�����Э��
*/
#define GET_CAM_TYPE					(0x0122)

/**
* @	��ȡ��λ���������λ�ý��ҵĳ����
*/
#define	GET_POSITION_CAM_INFO			(0x0130)


/**
* @	��ȡ���������ˮƽ����ֱ�������ƫ��ֵ
*/
#define	GET_TRACK_CAM_OFFSET			(0x0132)


//======================================================


/**
* @ �����豸
*/
#define REBOOT_DEVICE     (0x013D)



//-------------------�����ֶ������������---------------------
typedef enum manualMANUAL_COMMAND_ENUM
{
/**
* @	�����Ƿ�����ѧ���������������
*/
	SET_IS_CONTROL_CAM	=	0x01,


/**
* @	����Ԥ��λʱ������ת�����Խ���;���ˮƽ����ֱλ��ʱ��ʱ��������
* @  ��������brc-z330������ڵ���һ����������ȴ�ִ����ɺ󣬲��ܷ��ڶ������
* @  ����ڶ����������ִ�С�
*/
	SET_ZOOM_PAN_DELAY	=	0x02,

/**
* @	�������úڰ����������ұ�������������ֵΪ0��ʾ��ߣ�Ϊ1��ʾ�ұ� 
*/
	SET_BLACKBOARD_REGION	= 0x03,
	
/**
* @	����debug level������ֵΪ��Ӧ��level ֵ
*/
	SET_DEBUG_LEVEL	= 0x04
	
}manualMANUAL_COMMAND_e;


#define	PUSH_CLOSE_RANGE		(1)		//�ƽ���
#define NOT_PUSH_CLOSE_RANGE	(0)		//���ƽ���

/**
* @	ȷ�����ٿ��ĸ���
*/
#define TRACK_RANGE_NUM					(10)

/**
* @	ȷ���������ĸ���
*/
#define TRACK_TRIGGER_NUM				(50)

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
* @ ���û��ȡ���������ˮƽ����ֱ���򣬽����ƫ��ֵ
*/
typedef struct _track_cam_offset_info
{
 unsigned char  state;  //���û�������
 short  x_offset;  //���������ˮƽ�����ϵĽǶ�ƫ��ֵ
 short  y_offset;  //�����������ֱ�����ϵĽǶ�ƫ��ֵ
 short  row_width;  //���ҿ��
}track_cam_offset_info_t;



/**
* @ ���ƻ��ߵ�����
*/
typedef enum _draw_line_type
{
	DRAW_NO = 0,			//������
	DRAW_TRIGGER_TRACK,		//��ʾ�����ٿ�ʹ�����,�޸���,�͸��������
	DRAW_SLANT_LINE,		//��б�߱�ʾ��45��,135��,90��,180����
	DRAW_MODEL,				//��ģ��
	DRAW_SKIN_COLOR,		//����������ͷ��ķ�ɫ����
	DRAW_TRACK_TRAJECTORY,	//�����ٹ켣��
	DRAW_TRACK_DIRECT,		//�����ٷ�����
	DRAW_TRACK_SPEED,		//�������ٶ���
	MAX_NUM_DRAW
}draw_line_type_e;

/**
* @	����������λ�ĵķ���
*/
typedef enum _limit_position_type
{
	LIMIT_DOWN_LEFT = 0,
	LIMIT_UP_RIGHT,	
	LIMIT_CLEAR
}limit_position_type_e;


/**
* @ ����������Ϣͷ�ṹ
*/
typedef struct _msg_header
{
	unsigned short	nLen;					//ͨ��htonsת����ֵ,�����ṹ�屾�������
	unsigned short	nVer;					//�汾��(�ݲ���)
	unsigned char	nMsg;					//��Ϣ����
	unsigned char	szTemp[3];				//�����ֽ�
} msg_header_t;

/**
* @ ENC110����������Ϣͷ�ṹ
*/
typedef struct _msg_header_110
{
	unsigned char	nMsg;					//��Ϣ����
	unsigned short	nLen;					//ͨ��htonsת����ֵ,�����ṹ�屾�������
	
} msg_header_110_t;



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
* @	���������� 
*/
typedef struct _point_info
{
	unsigned short x;	//x����
	unsigned short y;	//y����
}point_info_t;

/**
* @	������� 
*/
typedef struct _rectangle_info
{
	unsigned short x;	//x����
	unsigned short y;	//y����
	unsigned short width;	//��ĸ�
	unsigned short height;	//��Ŀ�
}rectangle_info_t;

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
	unsigned char state;							//���û�������
	unsigned char point_num;						//ȷ����������ĵ���
	unsigned short video_width;						//��Ƶ�Ŀ�
	unsigned short video_height;					//��Ƶ�ĸ�
	rectangle_info_t rectangle[TRACK_TRIGGER_NUM];		//���ٿ�
}trigger_range_info_t;

/**
* @	�޸���Ϣ,��Ŀ���yֵ���ڴ�ֵʱ���ڸ���(��Ŀ��̫��)
*/
typedef struct _limit_height_info
{
	unsigned char 	state;			//���û�������
	unsigned short 	limit_height;	//�޸߸߶�
}limit_height_info_t;

/**
* @	�Զ����ֶ�����
*/
typedef struct _control_type_info
{
	unsigned char 	state;			//���û�������
	control_cam_mode_e 	control_type;	//��������0���Զ�����,1Ϊ�ֶ�����
}control_type_info_t;

/**
* @	�������ߵĿ���
*/
typedef struct _draw_line_info
{
	unsigned char 		state;		//���û�������
	draw_line_type_e 	message;	//message����
}draw_line_info_t;

/**
* @	��������ͷ�ٶȻ���
*/
typedef struct _cam_speed_info
{
	unsigned char 		state;		//���û�������
	unsigned short 	cam_speed;		//����ͷ�ٶȻ���,����Ҫ�ƶ��ľ�����Դ�ֵ,��Ϊ����ͷ�ٶ�
}cam_speed_info_t;

/**
* @	���ô���������
*/
typedef struct _trigger_num_info
{
	unsigned char 		state;		//���û�������
	unsigned short 	trigger_num;	//����������,�����������⵽����ô����㴥������Ϊ�Ǵ���
}trigger_num_info_t;

/**
* @	����Ԥ��λ
*/
typedef struct _preset_position_info
{
	unsigned char 		state;		//���û�������
	unsigned short 		preset_position;//���õ�Ԥ��λ,��Χ��1��255
	unsigned short 		position_time;//���õ�Ԥ��λ,��Χ��1��255
}preset_position_info_t;

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
* @	���ø��پ���
*/
typedef struct _track_distance_info
{
	unsigned char 			state;			//���û�������
	unsigned short 	zoom_distance;	//�����Ľ���Զ��ѡ��,ֵԽ��,��������ԽԶ,��0Ϊ���������,1ΪԶ�������
}track_distance_info_t;


/**
* @	���ø��ٻ��Ƿ����(Э�����õĽṹ��)
*/
typedef struct _track_is_encode_info
{
	unsigned char 	state;			//���û�������
	unsigned short 	isencode;		//���ٻ��Ƿ����ı�־,0��ʾ������,1��ʾ����
}track_is_encode_info_t;

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
* @ �������Ԥ��λ����Ϣ
*/
/**
* @ ���������λ����ռ���ֽ�����ͨVISCAռ��9���ֽڣ�����BRC-Z330ռ��8���ֽ�
*/
#define CAM_PAN_TILT_LEN (9)
	
/**
* @ ��������Խ�����ռ�ֽ���Ϊ4���ֽ�
*/
#define CAM_ZOOM_LEN  (4)

typedef struct _cam_preset_position_info
{
	
	unsigned char pan_tilt[CAM_PAN_TILT_LEN];	//ǰ�ĸ��ֽ�Ϊ����ͷˮƽλ��,���ĸ��ֽ�Ϊ����ͷ��ֱλ��
	unsigned char zoom[CAM_ZOOM_LEN];		//����ͷzoomλ��,�����ĸ��ֽ�����
	int cur_preset_value;		//�ϴ�ִ�е�Ԥ��λֵ
	//unsigned short 		position_time;//���õ�Ԥ��λ,��Χ��1��255
}cam_preset_position_info_t;


//������̨Ԥ��λ�������Ϣ�ṹ,��ѧ����������
typedef struct __call_cam_preset_teacher_and_student_req
{
 cam_preset_position_info_t cCamCoordInfo;
 
}call_cam_preset_teacher_and_student_req;


/**
* @	���ô�����ʼ����ʱ��
*/
typedef struct _start_track_time_info
{
	unsigned char 	state;				//���û�������
	unsigned short 	start_track_time;	//��ʼ����ʱ��
}start_track_time_info_t;

/**
* @	�������ö�ʧĿ������¿�ʼ������ʱ��
*/
typedef struct _reset_time_info
{
	unsigned char 	state;				//���û�������
	unsigned short 	reset_time;			//��λʱ��
}reset_time_info_t;

/**
* @	���ü��仯��ϵ��sensֵ
*/
typedef struct _sens_info
{
	unsigned char 	state;				//���û�������
	unsigned short 	sens;				//sensֵ
}sens_info_t;

/**
* @	���ü��仯��ϵ��sensֵ
*/
typedef struct _row_info
{
	unsigned char 	state;				//���û�������
	unsigned short 	row;				//rowֵ
}row_info_t;

/**
* @	����cam type
*/
typedef struct _cam_type
{
	unsigned char 	state;				//���û�������
	unsigned short 	type;				//sensֵ
}cam_type_info_t;
/**
* @	����track_switch_type
*/
typedef struct _track_switch_type
{
	unsigned char 	state;				//���û�������
	unsigned short 	type;				//sensֵ
}track_switch_type_info_t;

/**
* @	���ο���Ϣ
*/
typedef struct _shield_range_info
{
	unsigned char state;							//���û�������
	unsigned char point_num;						//ȷ����������ĵ���
	unsigned short video_width;						//��Ƶ�Ŀ�
	unsigned short video_height;					//��Ƶ�ĸ�
	rectangle_info_t rectangle[STUDENTS_SHIELD_NUM];		//���ٿ�
}shield_range_info_t;
/**
* @	��Ŀ����̨������Ϣ
*/
typedef struct _multitarget_range_info
{
	unsigned char state;							//���û�������
	unsigned char point_num;						//ȷ����������ĵ���
	unsigned short video_width;						//��Ƶ�Ŀ�
	unsigned short video_height;					//��Ƶ�ĸ�
	rectangle_info_t rectangle[STUDENTS_MULTITARGET_NUM];		//���ٿ�
}multitarget_range_info_t;
/*
typedef struct _cam_coord_info
{
	int iX;     //��������
	int iY;     //��������
	int iFocalDistance;  //����
}cam_coord_info_t;
*/
typedef struct _track_class_info
{
	unsigned short nStandupPos[4];   //ǰ����  �ң�  �������ҷֱ��Ӧ��������
	unsigned short nUpToPlatformTimes;   //ѧ����̨�ܴ���
	unsigned short nDownToStudentsAreaTimes;//��̨�ܴ���

	int iStudentStandupNum;     //����ѧ����������0��ʾδ��⵽ѧ��������1��ʾ��⵽һ��ѧ��������2��ʾ��⵽���Ŀ������
	cam_preset_position_info_t cStudentCamCoord;  //ѧ������̨���������
}track_class_info_t;


/**
* @	�ƶ�����ͷ����
*/
typedef enum _move_cam_type
{
	MOVE_NOT = 0,		//���ƶ�
	MOVE_ZOOM,			//�ƶ�����
	MOVE_PAN_TILT,		//�ƶ�λ��
	MOVE_NUM
}move_cam_type_e;

/**
* @	�����йص�ȫ�ֲ���
*/
typedef struct _cam_control_info
{
	int		cam_last_speed;					//����ͷ�ϴε��ƶ��ٶ�
	//camera_move_direction_e		cam_last_direction;	//����ͷ�ϴ��ƶ�����
	unsigned short cam_speed;				//����ͷת���ٶȻ���
	int		cam_position_value;				//Ԥ��λ��
	move_cam_type_e control_flag;			//��Ҫ�ƶ�zoomΪ1,��Ҫ�ƶ�λ��Ϊ2,����ҪΪ3
	cam_preset_position_info_t	cam_position[PRESET_NUM_MAX];
	unsigned char	cam_addr;						//�������ַ
	int		cam_position_lastvalue;				//Ԥ��λ��
}cam_control_info_t;

//��ѯ��̨Ԥ��λ�������Ϣ�ṹ,��ѧ����������
typedef struct __cam_preset_teacher_and_student_req
{
 int ePresetType;
 
}cam_preset_teacher_and_student_req;


//��ѯ��̨Ԥ��λ����Ӧ����ѧ����������
typedef struct __cam_preset_teacher_and_student_ack
{
 int ePresetType;
 cam_preset_position_info_t cCamCoordInfo;

}cam_preset_teacher_and_student_ack;


/**
* @ �ͱ����йص�һЩȫ�ֱ���(����ȫ�ֱ������õ��Ľṹ��)
*/
typedef struct _track_encode_info
{
	short	is_encode;			//Ϊ0��ʾ������,Ϊ1��ʾ����
	short	server_cmd;			//����������͵�����,��ʾ�������л�ѧ��������ʦ��
	int		track_status;		//���ٻ�״̬0��ʾδ������,1��ʾ������
	int		send_cmd;			//��ʦ��Ҫ���͵�����
	short	last_position_no;	//�ϴ�Ԥ��λ��
	int		is_track;			//�Ƿ����1��ʾ����,0��ʾ������
	unsigned short is_control_cam;//�Ƿ����������ƽ�����1Ϊ�ƽ�����0Ϊ���ƽ���
	int		zoom_pan_delay;	//��ת������λ��ʱ��zoom�͵���������λ��ʱ�м�ļ����λ��ms
	int		last_send_cmd;			//��ʦ����һ�η��͵�����
	int 	nTriggerValDelay;
	int 	nOnlyRightSideUpDelay;
	int 	nLastTriggerVal;
	int		nStandUpPos;
	int		nTrackSwitchType;
}track_encode_info_t;

/**
* @ ���û��ȡѧ������IP��Ϣ
*/
typedef struct _track_ip_info
{
 unsigned char  state;  ///1:����; 0:��������
 char     ip[16];  //IP��ֵ
 char     mask[16];
 char    route[16];
}track_ip_info_t;


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
* @	������ͺ�����
*/
typedef enum _cam_type_info
{
	VISCA_STANDARD 	= 0,		//��׼VISCAЭ�飬������ʤ�Ǵ�������
	SONY_BRC_Z330  	= 1, 		//SONY��BRC-Z330Э�������Э��
	HITACHI_CAM 	= 2,		//�����������֧��
	KXWELL_CAM  	= 3,		//���������������֧��
	CAM_NUM
}cam_type_info_e;

/**
* @ ������ͺ�ѡ�� 
*/
typedef struct _track_cam_model_info
{
	cam_type_info_e		cam_type;
	
}track_cam_model_info_t;

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
* @	���ò�������
*/
typedef enum	_set_cmd_type
{
	SET_PRESET_POSITION = 1,
	SET_LIMITE_POSITION = 2	
}set_cmd_type_e;

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
* @ ѧ���������ϱ�����Ϣ�ṹ��
*/
typedef struct _rightside_trigger_info
{
	unsigned int	nTriggerType;//��ǰȡ��0/����ȡ��1			
	unsigned int	nTriggerVal;//1��ʾ�д�����2��ʾ����(ֻ�п�ǰȡ������)��0������
}rightside_trigger_info_t;




extern int set_cam_addr();
extern int track_init(StuITRACK_Params *track_param);
extern int cam_ctrl_cmd(StuITRACK_OutArgs *output_param,unsigned char* posCmdOutput);
extern int cam_ctrl_cmd_rightside(StuITRACK_OutArgs *output_param);
extern int set_students_track_param(unsigned char *data, int socket);
extern int write_track_static_file(stutrack_static_param_t *static_param);
extern int server_set_track_type(short type);
extern int save_position_zoom(unsigned char *data);
extern int init_save_track_mutex(void);
extern int destroy_save_track_mutex(void);
extern int __call_preset_position(short position);
extern void CamCallPosition(void *pParam);
extern int student_init_params();


static int8_t active = 0;//�Ƿ���




#endif

