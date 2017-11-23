/*
****************************************************
Copyright (C), 1999-1999, Reach Tech. Co., Ltd.

File name:     	audio_track.c

Description:    ��ʦ����

Date:     		2012-09-04

Author:	  		sunxl

version:  		V1.0

*****************************************************
*/
#include "auto_track.h"
#include "rwini.h"
#include "common.h"
#include "strategy.h"
#include "rh_broadcast.h"
#include <arpa/inet.h>

//extern int gRemoteFD;
int gRemoteFD = -1;

Target target[DETECTION_TARGET];

/**
* @	������Ƕȱ�
*/
model_classroom_info_t	g_model_classroom = {0};

/**
* @ �����ѧ�����͸���ѧ�����ĽǶ���Ϣ
*/
angle_info_t	g_angle_info = {0.0};


/**
* @	tcp�����
*/
#define TCP_MSG_LEN		(256)

/**
* @	�豸�����ַ������ȵĺ�	
*/
#define DEV_NAME_LEN	(32)

/**
* @	���Ϳ�������ͷ������ĳ���
*/
#define CTRL_CMD_LEN	(16)

/**
* @	��ʦ���ٵĳ�ʼ��ʱ����Ĳ���
*/
ITRACK_STUDENTS_RIGHT_SIDE_Params 	g_track_param		= {0};

//===========================��ʦ��̬������صĺ궨��=====================

/**
* @ ��ʦ���ٵľ�̬������������
*/
#define STATIC_NAME				"static"

/**
* @ ��̬����������
*/
#define VIDEO_WIDTH				"video_width"
#define	VIDEO_HEIGHT			"video_height"
#define PIC_WIDTH				"pic_width"
#define PIC_HEIGHT				"pic_height"




//===========================��ʦ��̬������صĺ궨��=====================

/**
* @ ��ʦ���ٵĶ�̬������������
*/
#define DYNAMIC_NAME			"dynamic"



/**
* @ ��̬����������
*/
#define ZOOM_DISTACE			"zoom_distance"
#define X_OFFSET 				"x_offset"
#define Y_OFFSET				"y_offset"
#define TRIGGER_SUM 			"trigger_sum"
#define TRIGGER0_X0				"trigger0_x0"
#define TRIGGER0_Y0				"trigger0_y0"
#define TRIGGER0_X1				"trigger0_x1"
#define TRIGGER0_Y1				"trigger0_y1"
#define TRIGGER1_X0				"trigger1_x0"
#define TRIGGER1_Y0				"trigger1_y0"
#define TRIGGER1_X1				"trigger1_x1"
#define TRIGGER1_Y1				"trigger1_y1"
#define CONTROL_MODE			"control_mode"
#define SENS					"sens"
#define RESET_TIME				"reset_time"
#define	MESSAGE					"message"
#define	MODEL_SUM				"model_sum"
#define TRACK_SEL				"track_sel"
#define MID_X					"mid_x"
#define	MID_Y					"mid_y"
#define	LIMIT_HEIGHT			"limit_height"
#define MODEL_MULTIPLE			"model_multiple"
#define	MODEL_LEVEL				"model_level"
#define TRACK_POINT_NUM			"track_point_num"
#define TRACK_POINTX			"track_pointx"
#define TRACK_POINTY			"track_pointy"
#define	START_TRACK_TIME		"start_track_time"
#define RESET_TIME				"reset_time"
#define TRACK_MODE				"track_mode"
#define CLASSROOM_STUDY_MODE	"classroom_study_mode"

//===========================ѧ������������=====================
/**
* @ һЩϵͳ�������趨
*/
#define SYSTEM_PARAM			"system_param"

/**
* @	�豸����Ϊ��ѧ�������Ǹ������������ 
*/
#define	HOST_TYPE				"host_type"




/**
* @	����ˢ��ʱ��
*/
#define	REFRESH_TIME				"refresh_time"



/**
* @ ���ٻ���Ӧ�ı������
*/
#define TRACK_STUDENTS_RIGHT_SIDE_ENCODE			"track_students_right_side_encode"

/**
* @	�Ƿ�����־
*/
#define TRACK_STUDENTS_RIGHT_SIDE_IS_ENCODE			"track_students_right_side_is_encode"




/**
* @ ����ͷԤ��λ������������
*/
#define PRESET_POSITION	"preset_position"

/**
* @ ��ʦ����ͷԤ��λλ�õ���������ֵ
*/
#define PRESET_POSITION_VALUE		"position_value"

/**
* @ ��ʦ����ͷԤ��λ����λ��ֵ
*/
#define PRESET_ZOOM_VALUE			"zoom_value"



//===================================���������========================
/**
* @	�������������
*/
#define	CAM_PARAM					"cam_param"


/**
* @	������ٶȲ���
*/
#define CAM_SPEED					"cam_speed"

/**
* @	��ʦ����  �л���ʽ
*/
#define LOST_MODE                   "lostmode"
/**
* @	�������ģʽ
*/
#define BLACKBOARD_SCHEME			"blackboard_scheme"
/**
* @	�Ƿ����ø���ʦ�������
*/
#define HIGH_PRESET		"highpreset"

/**
* @ ��������Ͳ���
*/
#define	CAM_TYPE_NAME				"cam_type_name"

/**
* @	��λ��������ڽ��ҳ�
*/
#define	POSITION_CAM_LENTH			"position_cam_lenth"

/**
* @	��λ��������ڽ��ҿ�
*/
#define	POSITION_CAM_WIDTH			"position_cam_width"


/**
* @	��λ��������ڵĸ߶�
*/
#define	POSITION_CAM_HEIGHT			"position_cam_height"


/**
* @	������������ڽ��ҳ�
*/
#define	TRACK_CAM_LENTH			"track_cam_lenth"

/**
* @������������ڽ��ҿ�
*/
#define	TRACK_CAM_WIDTH			"track_cam_width"


/**
* @	������������ڵĸ߶�
*/
#define	TRACK_CAM_HEIGHT			"track_cam_height"

/**
* @	���������ˮƽ�����ϵ�ƫ��ֵ
*/
#define	TRACK_CAM_OFFSET_PAN		"track_cam_offset_pan"

/**
* @�����������ֱ�����ϵ�ƫ��ֵ
*/
#define	TRACK_CAM_OFFSET_TILT		"track_cam_offset_tilt"


/**
* @	�������������ƫ��ֵ
*/
#define	TRACK_CAM_OFFSET_ZOOM		"track_cam_offset_zoom"


/**
* @	����һ����λ������ڱ�������е�x����
*/
#define	OTHER_CAM_X					"other_cam_x"

/**
* @	����һ����λ������ڱ�������е�y����
*/
#define	OTHER_CAM_Y					"other_cam_y"

/**
* @	����һ����λ������ڱ�������еĿ�
*/
#define	OTHER_CAM_WIDTH					"other_cam_width"

/**
* @	����һ����λ������ڱ�������еĸ�
*/
#define	OTHER_CAM_HEIGHT					"other_cam_height"


//--------------------------------�л��������-----------------------------
/**
* @	�л����������Ϣ
*/
#define TRACK_STRATEGY				"track_strategy"

/**
* @	������������������
*/
#define BLACKBOARD1_TRIGGER_X0	"blackboard1_trigger_x0"
#define BLACKBOARD1_TRIGGER_Y0	"blackboard1_trigger_y0"
#define BLACKBOARD1_TRIGGER_X1	"blackboard1_trigger_x1"
#define BLACKBOARD1_TRIGGER_Y1	"blackboard1_trigger_y1"
#define BLACKBOARD2_TRIGGER_X0	"blackboard2_trigger_x0"
#define BLACKBOARD2_TRIGGER_Y0	"blackboard2_trigger_y0"
#define BLACKBOARD2_TRIGGER_X1	"blackboard2_trigger_x1"
#define BLACKBOARD2_TRIGGER_Y1	"blackboard2_trigger_y1"

#define TEACHER_FEATURE_START_Y	"teacher_feature_start_y"

/**
* @ ��λѡ��
*/
#define	STRATEGY_NO			"strategy_no"

/**
* @ ѧ������������ʾ�ж��ٸ�ѧ����
*/
#define STU_NUM		"stu_num"
/**
* @ �������������ʾ�ж��ٸ������
*/
#define BLACKBOARD_NUM		"blackboard_num"

/**
* @��ѯ��Ϣ
*/
#define TRACK_TURN_INFO			"track_turn_info"

/**
* @�Ƿ���ѯ,0��ʾ����ѯ��1��ʾ��ѯ
*/
#define TRACK_TURN_TYPE			"track_turn_type"

/**
* @��ѯʱ��������λs
*/
#define TRACK_TURN_TIME			"track_turn_time"

/**
* @��ѯ�೤ʱ�����£���λs
*/
#define TRACK_TURN_SHOW_TIME			"track_turn_show_time"

/**
* @	��ʦ��ʧ�� ѧ��ȫ��������ʱ��
*/
#define LOST_MODE_STUTIME		"lostmode_stutime"

/**
* @	ѧ��ȫ����ͷ�л���ѧ����д��ͷ����ʱʱ����Ϣ
*/
#define	STUDENTS_PANORAMA_SWITCH_NEAR_TIME	"students_panorama_switch_near_time"

/**
* @	ȷ��Ϊ����1��ʱ�䣬����ʦ�����ںڰ������ʱ�䳬����ʱ�伴Ϊ����	
*/
#define	TEACHER_BLACKBOARD_TIME1			"teacher_blackboard_time1"

/**
* @	ȷ��Ϊ�뿪����1��ʱ�䣬����ʦ�������ڰ��������ʱ�䳬����ʱ�䣬��Ϊ���ڰ���
*/
#define	TEACHER_LEAVE_BLACKBOARD_TIME1			"teacher_leave_blackboard_time1"

/**
* @	ȷ��Ϊ����2��ʱ�䣬����ʦ�����ںڰ������ʱ�䳬����ʱ�伴Ϊ����	
*/
#define	TEACHER_BLACKBOARD_TIME2			"teacher_blackboard_time2"

/**
* @	ȷ��Ϊ�뿪����2��ʱ�䣬����ʦ�������ڰ��������ʱ�䳬����ʱ�䣬��Ϊ���ڰ���
*/
#define	TEACHER_LEAVE_BLACKBOARD_TIME2			"teacher_leave_blackboard_time2"


//---------------------------------��ʦȫ����ͷ������Ҫ��Ϣ------------------

/**
* @	ѧ���½�̨ʱ��������ʱ�伴�ٴν���ѧ��������Ϣ����λ����
*/
#define	STUDENTS_DOWN_TIME			"students_down_time"

/**
* @ Ҫ�л�����ʦȫ����ͷ����ʱʱ��
*/
#define	TEACHER_PANORAMA_TIME			"teacher_panorama_time"

/**
* @ Ҫ����ʦ�����ȫ����ͷ�л��ߵ���ʱʱ��
*/
#define	TEACHER_LEAVE_PANORAMA_TIME			"teacher_leave_panorama_time"


/**
* @ ��ʦȫ����ͷ�����ʱ��
*/
#define	TEACHER_KEEP_PANORAMA_TIME			"teacher_keep_panorama_time"

/**
* @ ��ʦ���ģ�������ģ�������ĳ�ʱʱ��
*/
#define	TEACHER_DETECT_HEART_TIMEOUT			"teacher_detect_heart_timeout"

/**
* @ ��ʦ�л���ѧ��֮ǰ��ͷ��ʱ
*/
#define	TEACHER_SWITCH_STUDENTS_DELAY_TIME			"teacher_switch_students_delay_time"

/**
* @ѧ��������ͷ���ֵ����ʱ��
*/
#define	STUDENTS_NEAR_KEEP_TIME			"students_near_keep_time"

/**
* @vga�׻��澵ͷ����ʱ��
*/
#define	VGA_KEEP_TIME			"vga_keep_time"
/**
* @vga��ͷ����ʱ��
*/
#define	VGA_KEEP_TIME_TOSERVER			"vga_keep_time_toserver"

/**
* @��ʦ���ˣ������ƶ�Ŀ��ʱѧ��ȫ������ʦȫ����ͷ�л���ʱ
*/
#define	MV_KEEP_TIME			"mv_keep_time"


/**
* @ ��ʦ����ͷ��λ������������
*/
#define TEACH_CAM_LIMIT_POSITION	"limit_position"

/**
* @ ��ʦ����ͷ������λ����
*/
#define LIMIT_LEFT_DOWN_NAME	"left_down"

/**
* @ ��ʦ����ͷ������λ����
*/
#define LIMIT_RIGHT_UP_NAME		"right_up"

/**
* @ ��ʦKXWELL����ͷ����λ����
*/
#define LIMIT_KXWELL_UP_NAME "kxwell_up"
/**
* @ ��ʦKXWELL����ͷ����λ����
*/
#define LIMIT_KXWELL_DOWN_NAME "kxwell_down"
/**
* @ ��ʦKXWELL����ͷ����λ����
*/
#define LIMIT_KXWELL_LEFT_NAME "kxwell_left"
/**
* @ ��ʦKXWELL����ͷ����λ����
*/
#define LIMIT_KXWELL_RIGHT_NAME "kxwell_right"


/**
* @	�������
*/
#define	NETWORK_INFO					"network_info"


/**
* @	ѧ����IPֵ
*/
#define	STUDENT_IP					"student_ip"

/**
* @	�����IPֵ
*/
#define	BLACKBOARD_IP					"blackboard_ip"


/**
* @	����IPֵ
*/
#define	LOCAL_IP					"local_ip"

/**
* @	����MASKֵ
*/
#define	LOCAL_MASK					"local_mask"

/**
* @	����GWֵ
*/
#define	LOCAL_GW					"local_gw"

/**
* @	�����Ƿ���Ҫ���趯̬������־ 
*/
int g_recontrol_flag = 0;



/**
* @	��д���������ļ����
*/
track_save_file_info_t	g_track_save_file = {0};


/**
* @	�͸����йصı����һЩȫ�ֲ���
*/
track_encode_info_t	g_track_encode_info = {0};


/**
* @ ��������ͷ��Ҫ�Ĳ���
*/
cam_control_info_t	g_cam_info = {0};

/**
* @	���ٲ��������Ϣ
*/
track_tactics_info_t g_track_tactics_info = {0};


/**
* @	 �Ͱ����йص��������λ����Ϣȫ�ֱ���
*/
track_strategy_info_t		g_track_strategy_info = {0};//�л�������ر���

track_strategy_timeinfo_t		g_track_strategy_timeinfo = {0};//���������л�ʱȫ��ͳ��ʱ��

/**
* @	 ѧ������ѵ����Ϣȫ�ֱ���
*/
stu_turn_info_t		g_stu_turn_info = {0};//�л�������ر���



/**
* @	�Զ�ˢ�±�����Ϣ 
*/
trigger_point_info_t	g_trigger_info[TRIGGER_POINT_NUM] = {0};

unsigned long long 				g_refresh_start_time 			  = 0;

extern char BLACKBOARDTRACE_IP[BLACKCONNECT_NUM_MAX][16];

int init_save_track_mutex(void)
{
	pthread_mutex_init(&g_track_save_file.save_track_m, NULL);
	
	return 0;
}

int lock_save_track_mutex(void)
{
	pthread_mutex_lock(&g_track_save_file.save_track_m);
	return 0;
}

int unlock_save_track_mutex(void)
{
	pthread_mutex_unlock(&g_track_save_file.save_track_m);
	return 0;
}

int destroy_save_track_mutex(void)
{
	pthread_mutex_destroy(&g_track_save_file.save_track_m);
}

/*send data to tty com*/
int SendDataToCom(int fd, unsigned char *data, int len)
{
	return 0;
}

int SendDataToComNoDelay(int fd, unsigned char *data, int len)
{
	return 0;
}


/**
* @	��������: set_limit_position()
* @	��������: �������Ϻ�������λ
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_limit_position(unsigned short limit_position, unsigned char *position_data)
{
	int 					fd 			= gRemoteFD; 
	int 	ret 						= -1;
	int 	size 						= 0;
	unsigned char recv_data[32]			= {0};
	unsigned char send_data[32]			= {0};

	if(SONY_BRC_Z330 == g_angle_info.cam_type)
	{
		send_data[0] = 0x81;
	   	send_data[1] = 0x01;
	   	send_data[2] = 0x06;
	   	send_data[3] = 0x07;
	   	send_data[4] = 0x00;
	   	send_data[5] = 0x00;	//��ʾ������λ����

		//������ҵ�λ����Ϣ
		send_data[6] =  position_data[0];//0x00;
		send_data[7] =  position_data[1];//0x00;
		send_data[8] = position_data[2];//0x00;
		send_data[9] = position_data[3];//0x07;
		send_data[10] = position_data[4];//0x0f;
		
		//�ϻ����µ�λ����Ϣ
		send_data[11] = position_data[5];//0x0f;
		send_data[12] = position_data[6];//0x0f;
		send_data[13] = position_data[7];//0x0e;
		send_data[14] = position_data[8];//0x0e;
		
		send_data[15] = 0xff;

		if(LIMIT_DOWN_LEFT == limit_position)
		{
			send_data[5] = 0x00;	//��ʾ������λ����
		}
		else if(LIMIT_UP_RIGHT == limit_position)
		{
		   	send_data[5] = 0x01;		//��ʾ������λ����
		}
		size = 16;
		SendDataToCom(fd, send_data, size);

	}
	else if(KXWELL_CAM == g_angle_info.cam_type)
	{
		send_data[0] = 0x81;
	   	send_data[1] = 0x01;
	   	send_data[2] = 0x06;
	   	send_data[3] = 0x07;
	   	send_data[4] = 0xAA;
	   //send_data[5] = 0x01;	//��1,��2����4����3
	   	send_data[6] = 0xff;	
		size = 7;

		switch(limit_position)
		{
			case LIMIT_KXWELL_UP:
				send_data[5] = 0x02;		//��ʾ����λ����
				DEBUG(DL_WARNING,"kxwell set up \n");
				break;
			case LIMIT_KXWELL_DOWN:
				send_data[5] = 0x01;	//��ʾ����λ����
				DEBUG(DL_WARNING,"kxwell set down \n");
				break;
			case LIMIT_KXWELL_LEFT:
				send_data[5] = 0x03;	//��ʾ����λ����
				DEBUG(DL_WARNING,"kxwell set left \n");
				break;
			case LIMIT_KXWELL_RIGHT:
				send_data[5] = 0x04;		//��ʾ����λ����
				DEBUG(DL_WARNING,"kxwell set right \n");
				break;
			default:
				break;
		}
		SendDataToCom(fd, send_data, size);

	}
	else
	{
		send_data[0] = 0x81;
	   	send_data[1] = 0x01;
	   	send_data[2] = 0x06;
	   	send_data[3] = 0x07;
	   	send_data[4] = 0x00;
	   	send_data[5] = 0x00;	//��ʾ������λ����

		//������ҵ�λ����Ϣ
	   	send_data[6] = position_data[0];
		send_data[7] = position_data[1];
		send_data[8] = position_data[2];
		send_data[9] = position_data[3];

		//�ϻ����µ�λ����Ϣ
		send_data[10] = position_data[4];
		send_data[11] = position_data[5];
		send_data[12] = position_data[6];
		send_data[13] = position_data[7];
		
		send_data[14] = 0xff;
	

		if(LIMIT_DOWN_LEFT == limit_position)
		{
			send_data[5] = 0x00;	//��ʾ������λ����
		}
		else if(LIMIT_UP_RIGHT == limit_position)
		{
		   	send_data[5] = 0x01;		//��ʾ������λ����
		}

		size = 15;
		SendDataToCom(fd, send_data, size);
	}
	return 0;
}

/**
* @	��������: clear_limit_position()
* @	��������: �������,������λ
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int clear_limit_position(void)
{
	int 			fd 				= gRemoteFD; 
	int 			ret 			= -1;
	int 			size 			= 0;
	unsigned char 	send_data[32]	= {0};

	if(SONY_BRC_Z330 == g_angle_info.cam_type)
	{
		send_data[0] = 0x81;
		send_data[1] = 0x01;
		send_data[2] = 0x06;
		send_data[3] = 0x07;
		send_data[4] = 0x01;
		send_data[5] = 0x00;		//��ʾ������λ���

		//������ҵ�λ����Ϣ
		send_data[6] = 0x07;
		send_data[7] = 0x0f;
		send_data[8] = 0x0f;
		send_data[9] = 0x0f;
		send_data[10] = 0x0f;

		//�ϻ����µ�λ����Ϣ
		send_data[11] = 0x07;
		send_data[12] = 0x0f;
		send_data[13] = 0x0f;
		send_data[14] = 0x0f;

		send_data[15] = 0xff;
		size = 16;
		SendDataToCom(fd, send_data, size);
		sleep(1);
		send_data[5] = 0x01;		//��ʾ������λ���
		SendDataToCom(fd, send_data, size);
	}
	else
	{
		send_data[0] = 0x81;
		send_data[1] = 0x01;
		send_data[2] = 0x06;
		send_data[3] = 0x07;
		send_data[4] = 0x01;
		send_data[5] = 0x00;		//��ʾ������λ���

		//������ҵ�λ����Ϣ
		send_data[6] = 0x07;
		send_data[7] = 0x0f;
		send_data[8] = 0x0f;
		send_data[9] = 0x0f;

		//�ϻ����µ�λ����Ϣ
		send_data[10] = 0x07;
		send_data[11] = 0x0f;
		send_data[12] = 0x0f;
		send_data[13] = 0x0f;

		send_data[14] = 0xff;
		size = 15;
		SendDataToCom(fd, send_data, size);

		send_data[5] = 0x01;		//��ʾ������λ���
		SendDataToCom(fd, send_data, size);
	}
	return 0;
}



/**
* @	��������: init_limit_position_set()
* @	��������: ��ʼ��λ����,��ϵͳ������������ļ���ȡ����λ��Ϣ��������
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
int init_limit_position_set(void)
{
	char 			config_file[FILE_NAME_LEN] 	= {0};
	char			param_name[FILE_NAME_LEN] 	= {0};
	char 			temp[FILE_NAME_LEN]			= {0};
	unsigned char 	temp_data[32]				= {0};
	int 			ret 						= -1;
	int				index						= 0;
	int						a 							= 0;
	int						b 							= 0;
	

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	//����������λ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, TEACH_CAM_LIMIT_POSITION, LIMIT_LEFT_DOWN_NAME, temp);
	if(ret == 0)
	{
		for(index = 0; index < CAM_PAN_TILT_LEN; index++)
		{
			if((temp[index] >= 0x30) && (temp[index] <= 0x39))
			{
				temp_data[index] = temp[index] - 0x30;
			}
			if((temp[index] >= 0x41) && (temp[index] <= 0x46))
			{
				temp_data[index] = temp[index] - 0x41 + 0xa;
			}
			if((temp[index] >= 0x61) && (temp[index] <= 0x66))
			{
				temp_data[index] = temp[index] - 0x61 + 0xa;
			}
		}

		DEBUG(DL_WARNING, "left temp_data = 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n",temp_data[0],temp_data[1],temp_data[2],temp_data[3],temp_data[4],temp_data[5],temp_data[6],temp_data[7]);
		
		if(SONY_BRC_Z330 == g_angle_info.cam_type)
		{
			b = 0;
			b = temp_data[4];
			
			a = temp_data[3];
			
			b = (a << 4) | b;
			a = temp_data[2];
			b = (a << 8) | (b);
			a = temp_data[1];
			b = (a << 12) | (b);
			a = temp_data[0];
			b = (a << 16) | (b);
			g_track_strategy_info.cam_left_limit=b & 0x000fffff;
			if(0 != strcmp(temp, "7ffff7fff"))
			{
				set_limit_position(LIMIT_DOWN_LEFT, temp_data);
			}
		}
		else
		{
			b = 0;
			
			b = temp_data[3];
			a = temp_data[2];
			
			b = (a << 4) | (b);
			a = temp_data[1];
			b = (a << 8) | (b);
			a = temp_data[0];
			b = (a << 12) | (b);
			g_track_strategy_info.cam_left_limit=b & 0x0000ffff;
			if(0 != strcmp(temp, "7fff7fff"))
			{
				set_limit_position(LIMIT_DOWN_LEFT, temp_data);
			}
		}
	}
	usleep(100000);

	//����������λ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, TEACH_CAM_LIMIT_POSITION, LIMIT_RIGHT_UP_NAME, temp);
	if(ret == 0)
	{
		for(index = 0; index < CAM_PAN_TILT_LEN; index++)
		{
			if((temp[index] >= 0x30) && (temp[index] <= 0x39))
			{
				temp_data[index] = temp[index] - 0x30;
			}
			if((temp[index] >= 0x41) && (temp[index] <= 0x46))
			{
				temp_data[index] = temp[index] - 0x41 + 0xa;
			}
			if((temp[index] >= 0x61) && (temp[index] <= 0x66))
			{
				temp_data[index] = temp[index] - 0x61 + 0xa;
			}
		}

		DEBUG(DL_WARNING, "right temp_data = 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n",temp_data[0],temp_data[1],temp_data[2],temp_data[3],temp_data[4],temp_data[5],temp_data[6],temp_data[7]);

		if(SONY_BRC_Z330 == g_angle_info.cam_type)
		{
			b = 0;
			b = temp_data[4];
			
			a = temp_data[3];
			
			b = (a << 4) | b;
			a = temp_data[2];
			b = (a << 8) | (b);
			a = temp_data[1];
			b = (a << 12) | (b);
			a = temp_data[0];
			b = (a << 16) | (b);
			g_track_strategy_info.cam_right_limit=b & 0x000fffff;
			if(0 != strcmp(temp, "7ffff7fff"))
			{
				set_limit_position(LIMIT_UP_RIGHT, temp_data);
			}
		}
		else
		{
			b = 0;
			
			b = temp_data[3];
			a = temp_data[2];
			
			b = (a << 4) | (b);
			a = temp_data[1];
			b = (a << 8) | (b);
			a = temp_data[0];
			b = (a << 12) | (b);
			g_track_strategy_info.cam_right_limit=b & 0x0000ffff;
			if(0 != strcmp(temp, "7fff7fff"))
			{
				set_limit_position(LIMIT_UP_RIGHT, temp_data);
			}
		}
	}
	return 0;
}


/**
* @ ��������: jump_appoint_position()
* @ ��������: ��������ͷת��ָ��λ��
* @ �������: cam_position -- Ԥ��λ���
* @ �������: data -- ������ͷ���͵����������
* @ ����ֵ:   ���͵����ݵĳ���
*/
int jump_appoint_position(int cam_addr, int cam_pan, int cam_tilt, int fd)
{
	int size = 0;

	int index = 0;

	int	a = 0;

	unsigned char 			data[CTRL_CMD_LEN] 		= {0};

	DEBUG(DL_DEBUG, "######cam_pan = 0x%x\n",cam_pan);
	DEBUG(DL_DEBUG, "######cam_tilt = 0x%x\n",cam_tilt);


	data[0] = cam_addr;
	data[1] = 0x01;
	data[2] = 0x06;
	data[3] = 0x02;

	data[4] = 0x18; 	//ˮƽ�����ٶ�
	data[5] = 0x14; 	//��ֱ�����ٶ�

	a = cam_pan;	
	data[6] = (a >> 12) & 0x0000000f;	//ˮƽ����λ����Ϣ
	
	a = cam_pan;
	data[7] = (a >> 8) & 0x0000000f;	//ˮƽ����λ����Ϣ
	
	a = cam_pan;
	data[8] = (a >> 4) & 0x0000000f;	//ˮƽ����λ����Ϣ
	
	a = cam_pan;
	data[9] = a & 0x0000000f;	//ˮƽ����λ����Ϣ

	a = cam_tilt;
	data[10] = (a >> 12) & 0x0000000f;;	//��ֱ����λ����Ϣ

	a = cam_tilt;
	data[11] = (a >> 8) & 0x0000000f;;	//��ֱ����λ����Ϣ

	a = cam_tilt;
	data[12] = (a >> 4) & 0x0000000f;;	//��ֱ����λ����Ϣ

	a = cam_tilt;
	data[13] = a & 0x0000000f;;	//��ֱ����λ����Ϣ
	
	data[14] = 0xff;
	
	size = 15;

	for(index = 0; index < 15; index++)
	{
		DEBUG(DL_DEBUG, "######data[%d] = 0x%x\n",index,data[index]);
	}
	
	SendDataToCom(fd, data, size);
	return size;
}



/**
* @ ��������: jump_appoint_zoom()
* @ ��������: ��������ͷת��ָ��λ��
* @ �������: cam_position -- Ԥ��λ���
* @ �������: data -- ������ͷ���͵����������
* @ ����ֵ:   ���͵����ݵĳ���
*/
int jump_appoint_zoom(int cam_addr, int zoom, int fd)
{
	int size = 0;

	int index = 0;

	int a = 0;

	unsigned char			data[CTRL_CMD_LEN]		= {0};

	DEBUG(DL_DEBUG, "######jump_appoint_zoom = 0x%x\n",zoom);


	data[0] = cam_addr;
	data[1] = 0x01;
	data[2] = 0x04;
	data[3] = 0x47;

	a = zoom;	
	data[4] = (a >> 12) & 0x0000000f;	//zoomλ����Ϣ

	a = zoom;
	data[5] = (a >> 8) & 0x0000000f; 	//��ֱ�����ٶ�

	a = zoom;	
	data[6] = (a >> 4) & 0x0000000f;	//ˮƽ����λ����Ϣ
	
	a = zoom;
	data[7] = a & 0x0000000f;	//ˮƽ����λ����Ϣ
	
	data[8] = 0xff;
	
	size = 9;


	SendDataToCom(fd, data, size);
	return size;
}



/**
* @ ��������: set_cam_addr()
* @ ��������: ��������ͷת��0x82��ַλ
*/
int set_cam_addr()
{
	int size = 0;
	unsigned char data[4];
	int fd = gRemoteFD; 

	data[0] = 0x88;
	data[1] = 0x30;
   	data[2] = 0x01;
   	data[3] = 0xff;
	size = 4;
	
	SendDataToCom(fd, data, size);
	return size;
}

/**
* @	��������: get_cam_position()
* @	��������: ��ȡ����ͷ��������λ��
* @	�������: ��
* @ �������: data,���ػ�ȡ������ͷ��λ����Ϣ
* @ ����ֵ:   ��
*/
static int get_cam_position(unsigned char 	*data)
{
	int 			fd 				= gRemoteFD; 
	unsigned char 	cmd_data[20]	= {0};
	int 			ret 			= -1;
	int 			size 			= 0;

	cmd_data[0] = g_angle_info.cam_addr;
   	cmd_data[1] = 0x09;
   	cmd_data[2] = 0x06;
   	cmd_data[3] = 0x12;
   	cmd_data[4] = 0xFF;
	size = 5;
	SendDataToCom(fd, cmd_data, size);	
	return 0;
}

/**
* @	��������: get_cam_zoom()
* @	��������: ��ȡ��ͷ���ڵĽ���λ��
* @	�������: ��
* @ �������: data,���ػ�ȡ������ͷ�Ľ�����Ϣ
* @ ����ֵ:   ��
*/
static int get_cam_zoom(unsigned char 	*data)
{
	int 			fd 				= gRemoteFD; 
	unsigned char 	cmd_data[20]	= {0};
	int 			ret 			= -1;
	int 			size 			= 0;

	cmd_data[0] = g_angle_info.cam_addr;
   	cmd_data[1] = 0x09;
   	cmd_data[2] = 0x04;
   	cmd_data[3] = 0x47;
   	cmd_data[4] = 0xFF;
	size = 5;
	SendDataToCom(fd, cmd_data, size);
	return 0;
}

int save_position_zoom_with_network_data(int ePresetType, unsigned char *data)
{
	char 					temp[FILE_NAME_LEN]			= {0};
	char 					config_file[FILE_NAME_LEN] 	= {0};
	char					param_name[FILE_NAME_LEN] 	= {0};
	char					text[FILE_NAME_LEN]			= {0};
	int 					ret = -1;
	cam_preset_position_info_t* cCamCoordInfo = (cam_preset_position_info_t*)data;

	if (ePresetType >= PRESET_NUM_MAX || ePresetType < 0)
	{
		DEBUG(DL_ERROR,"ePresetType error. ePresetType:%d\n", ePresetType);
		return -1;
	}

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	memset(text, 0, FILE_NAME_LEN);
	memset(param_name, 0, FILE_NAME_LEN);
	strcpy(text, PRESET_POSITION_VALUE);
	sprintf(param_name, "%s%d",text, ePresetType);
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%x%x%x%x%x%x%x%x%x",
		cCamCoordInfo->pan_tilt[0],
		cCamCoordInfo->pan_tilt[1],
		cCamCoordInfo->pan_tilt[2],
		cCamCoordInfo->pan_tilt[3],
		cCamCoordInfo->pan_tilt[4],
		cCamCoordInfo->pan_tilt[5],
		cCamCoordInfo->pan_tilt[6],
		cCamCoordInfo->pan_tilt[7],
		cCamCoordInfo->pan_tilt[8]);

	DEBUG(DL_DEBUG,"save_position_zoom_with_network_data->position ePresetType:%d, param_name:%s, g_track_save_file.cmd_param:%d, temp:%s\n", ePresetType, param_name, g_track_save_file.cmd_param, temp);
	
	lock_save_track_mutex();
	ret =  ConfigSetKey(config_file, PRESET_POSITION, param_name, temp);
	unlock_save_track_mutex();
	g_cam_info.cam_position[ePresetType].pan_tilt[0]= cCamCoordInfo->pan_tilt[0];
	g_cam_info.cam_position[ePresetType].pan_tilt[1]= cCamCoordInfo->pan_tilt[1];
	g_cam_info.cam_position[ePresetType].pan_tilt[2]= cCamCoordInfo->pan_tilt[2];
	g_cam_info.cam_position[ePresetType].pan_tilt[3]= cCamCoordInfo->pan_tilt[3];
	g_cam_info.cam_position[ePresetType].pan_tilt[4]= cCamCoordInfo->pan_tilt[4];
	g_cam_info.cam_position[ePresetType].pan_tilt[5]= cCamCoordInfo->pan_tilt[5];
	g_cam_info.cam_position[ePresetType].pan_tilt[6]= cCamCoordInfo->pan_tilt[6];
	g_cam_info.cam_position[ePresetType].pan_tilt[7]= cCamCoordInfo->pan_tilt[7];
	g_cam_info.cam_position[ePresetType].pan_tilt[8]= cCamCoordInfo->pan_tilt[8];


	memset(text, 0, FILE_NAME_LEN);
	memset(param_name, 0, FILE_NAME_LEN);
	strcpy(text, PRESET_ZOOM_VALUE);
	sprintf(param_name, "%s%d",text, ePresetType);
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%x%x%x%x",
		cCamCoordInfo->zoom[0],
		cCamCoordInfo->zoom[1],
		cCamCoordInfo->zoom[2],
		cCamCoordInfo->zoom[3]);

	DEBUG(DL_DEBUG,"save_position_zoom_with_network_data->zoom ePresetType:%d, param_name:%s, temp:%s\n", ePresetType, param_name, temp);
	
	lock_save_track_mutex();
	ret =  ConfigSetKey(config_file, PRESET_POSITION, param_name, temp);
	unlock_save_track_mutex();
	g_cam_info.cam_position[ePresetType].zoom[0]= cCamCoordInfo->zoom[0];
	g_cam_info.cam_position[ePresetType].zoom[1]= cCamCoordInfo->zoom[1];
	g_cam_info.cam_position[ePresetType].zoom[2]= cCamCoordInfo->zoom[2];
	g_cam_info.cam_position[ePresetType].zoom[3]= cCamCoordInfo->zoom[3];

	return ret;
}

int save_position_zoom(unsigned char *data)
{
	char 					temp[FILE_NAME_LEN]			= {0};
	char 					config_file[FILE_NAME_LEN] 	= {0};
	char					param_name[FILE_NAME_LEN] 	= {0};
	char					text[FILE_NAME_LEN]			= {0};
	int 					ret 						= -1;

	int						a 							= 0;
	int						b 							= 0;
	
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);
	//DEBUG(DL_DEBUG, "00&&&&&&&&&&g_angle_info.track_cam_cur_pan = %d\n",g_angle_info.track_cam_cur_pan);

	if(SONY_BRC_Z330 == g_angle_info.cam_type)
	{
		if((0x90 == data[0]) && (0x50 == data[1]) && (0xff == data[11]))
		{
			if(SET_PRESET_POSITION == g_track_save_file.set_cmd)
			{
				memset(text, 0, FILE_NAME_LEN);
				memset(param_name, 0, FILE_NAME_LEN);
				strcpy(text, PRESET_POSITION_VALUE);
				sprintf(param_name, "%s%d",text, g_track_save_file.cmd_param);
				memset(temp, 0, FILE_NAME_LEN);
				sprintf(temp, "%x%x%x%x%x%x%x%x%x",data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10]);
				lock_save_track_mutex();
				ret =  ConfigSetKey(config_file, PRESET_POSITION, param_name, temp);
				unlock_save_track_mutex();
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[0]= data[2];
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[1]= data[3];
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[2]= data[4];
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[3]= data[5];
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[4]= data[6];
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[5]= data[7];
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[6]= data[8];
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[7]= data[9];
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[8]= data[10];
			}
			else if(SET_LIMITE_POSITION == g_track_save_file.set_cmd)
			{
				memset(text, 0, FILE_NAME_LEN);
				memset(param_name, 0, FILE_NAME_LEN);
				memset(temp, 0, FILE_NAME_LEN);
				sprintf(temp, "%x%x%x%x%x%x%x%x",data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9]);

				switch(g_track_save_file.cmd_param)
				{
					case LIMIT_DOWN_LEFT:
						strcpy(text, LIMIT_LEFT_DOWN_NAME);
						break;
					case LIMIT_UP_RIGHT:
						strcpy(text, LIMIT_RIGHT_UP_NAME);
						break;
					case LIMIT_KXWELL_UP:
						strcpy(text, LIMIT_KXWELL_UP_NAME);
						break;
					case LIMIT_KXWELL_DOWN:
						strcpy(text, LIMIT_KXWELL_DOWN_NAME);
						break;
					case LIMIT_KXWELL_LEFT:
						strcpy(text, LIMIT_KXWELL_LEFT_NAME);
						break;
					case LIMIT_KXWELL_RIGHT:
						strcpy(text, LIMIT_KXWELL_RIGHT_NAME);
						break;
					default:
						break;
				}
				sprintf(param_name, "%s",text);
				
				lock_save_track_mutex();
				ret =  ConfigSetKey(config_file, TEACH_CAM_LIMIT_POSITION, param_name, temp);
				unlock_save_track_mutex();
				if(KXWELL_CAM == g_angle_info.cam_type)
				{
					
					//Kxwell_init_limit_position();
					//set_limit_position(g_track_save_file.cmd_param,temp_data);
				}
				else
				{
					init_limit_position_set();
				}
				DEBUG(DL_WARNING,"set cam_left_limit = 0x%x,cam_right_limit=0x%x\n",g_track_strategy_info.cam_left_limit,g_track_strategy_info.cam_right_limit);

			}
			else if(GET_CUR_POSITION == g_track_save_file.set_cmd)
			{
				b = 0;
				b = data[6];
				
				a = data[5];
				
				b = (a << 4) | b;
				a = data[4];
				b = (a << 8) | (b);
				a = data[3];
				b = (a << 12) | (b);
				a = data[2];
				b = (a << 16) | (b);

				g_angle_info.track_cam_cur_pan = b & 0x000fffff;
				g_track_strategy_info.cur_pan_tilt = g_angle_info.track_cam_cur_pan;
				
				//DEBUG(DL_WARNING,"get cur_pan_tilt = 0x%x\n",g_angle_info.track_cam_cur_pan);
			}
			else
			{
				if(data[2] > 8)
				{
					
				}
			}
		}
		else if((0x90 == data[0]) && (0x50 == data[1]) && (0xff == data[6]))
		{
			if(SET_PRESET_POSITION == g_track_save_file.set_cmd)
			{
				memset(text, 0, FILE_NAME_LEN);
				memset(param_name, 0, FILE_NAME_LEN);
				strcpy(text, PRESET_ZOOM_VALUE);
				sprintf(param_name, "%s%d",text, g_track_save_file.cmd_param);
				memset(temp, 0, FILE_NAME_LEN);
				sprintf(temp, "%x%x%x%x",data[2],data[3],data[4],data[5]);
				lock_save_track_mutex();
				ret =  ConfigSetKey(config_file, PRESET_POSITION, param_name, temp);
				unlock_save_track_mutex();
				g_cam_info.cam_position[g_track_save_file.cmd_param].zoom[0] = data[2];
				g_cam_info.cam_position[g_track_save_file.cmd_param].zoom[1] = data[3];
				g_cam_info.cam_position[g_track_save_file.cmd_param].zoom[2] = data[4];
				g_cam_info.cam_position[g_track_save_file.cmd_param].zoom[3] = data[5];
			}
		}

	}//�����ͺ������
	else
	{
		//DEBUG(DL_DEBUG, "11&&&&&&&&&&g_angle_info.track_cam_cur_pan = %d\n",g_angle_info.track_cam_cur_pan);
		if((0x90 == data[0]) && (0x50 == data[1]) && (0xff == data[10]))
		{
			//DEBUG(DL_DEBUG, "22&&&&&&&&&&g_angle_info.track_cam_cur_pan = %d\n",g_angle_info.track_cam_cur_pan);
			if(SET_PRESET_POSITION == g_track_save_file.set_cmd)
			{
				memset(text, 0, FILE_NAME_LEN);
				memset(param_name, 0, FILE_NAME_LEN);
				strcpy(text, PRESET_POSITION_VALUE);
				sprintf(param_name, "%s%d",text, g_track_save_file.cmd_param);
				memset(temp, 0, FILE_NAME_LEN);
				
				sprintf(temp, "%x%x%x%x%x%x%x%x",data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9]);

				lock_save_track_mutex();
				ret =  ConfigSetKey(config_file, PRESET_POSITION, param_name, temp);
				unlock_save_track_mutex();
				
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[0]= data[2];
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[1]= data[3];
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[2]= data[4];
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[3]= data[5];
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[4]= data[6];
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[5]= data[7];
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[6]= data[8];
				g_cam_info.cam_position[g_track_save_file.cmd_param].pan_tilt[7]= data[9];
			}
			else if(GET_CUR_POSITION == g_track_save_file.set_cmd)
			{
			
				b = 0;
				
				b = data[5];
				a = data[4];
				
				b = (a << 4) | (b);
				a = data[3];
				b = (a << 8) | (b);
				a = data[2];
				b = (a << 12) | (b);
				
				g_angle_info.track_cam_cur_pan = b & 0x0000ffff;
				g_track_strategy_info.cur_pan_tilt = g_angle_info.track_cam_cur_pan;
				//DEBUG(DL_DEBUG, "33&&&&&&&&&&g_angle_info.track_cam_cur_pan = %d\n",g_angle_info.track_cam_cur_pan);

				
			}
			else if(SET_LIMITE_POSITION == g_track_save_file.set_cmd)
			{
				memset(text, 0, FILE_NAME_LEN);
				memset(param_name, 0, FILE_NAME_LEN);
				memset(temp, 0, FILE_NAME_LEN);
				sprintf(temp, "%x%x%x%x%x%x%x%x",data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9]);

				switch(g_track_save_file.cmd_param)
				{
					case LIMIT_DOWN_LEFT:
						strcpy(text, LIMIT_LEFT_DOWN_NAME);
						break;
					case LIMIT_UP_RIGHT:
						strcpy(text, LIMIT_RIGHT_UP_NAME);
						break;
					case LIMIT_KXWELL_UP:
						strcpy(text, LIMIT_KXWELL_UP_NAME);
						break;
					case LIMIT_KXWELL_DOWN:
						strcpy(text, LIMIT_KXWELL_DOWN_NAME);
						break;
					case LIMIT_KXWELL_LEFT:
						strcpy(text, LIMIT_KXWELL_LEFT_NAME);
						break;
					case LIMIT_KXWELL_RIGHT:
						strcpy(text, LIMIT_KXWELL_RIGHT_NAME);
						break;
					default:
						break;
				}
				sprintf(param_name, "%s",text);
				
				lock_save_track_mutex();
				ret =  ConfigSetKey(config_file, TEACH_CAM_LIMIT_POSITION, param_name, temp);
				unlock_save_track_mutex();
				if(KXWELL_CAM == g_angle_info.cam_type)
				{
					
					//Kxwell_init_limit_position();
					//set_limit_position(g_track_save_file.cmd_param,temp_data);
				}
				else
				{
					init_limit_position_set();
				}
				DEBUG(DL_WARNING,"set cam_left_limit = 0x%x,cam_right_limit=0x%x\n",g_track_strategy_info.cam_left_limit,g_track_strategy_info.cam_right_limit);

			}
		}
		else if((0x90 == data[0]) && (0x50 == data[1]) && (0xff == data[6]))
		{
			if(SET_PRESET_POSITION == g_track_save_file.set_cmd)
			{
				memset(text, 0, FILE_NAME_LEN);
				memset(param_name, 0, FILE_NAME_LEN);
				strcpy(text, PRESET_ZOOM_VALUE);
				sprintf(param_name, "%s%d",text, g_track_save_file.cmd_param);
				memset(temp, 0, FILE_NAME_LEN);
				
				sprintf(temp, "%x%x%x%x",data[2],data[3],data[4],data[5]);
				
				lock_save_track_mutex();
				ret =  ConfigSetKey(config_file, PRESET_POSITION, param_name, temp);
				unlock_save_track_mutex();
				
				g_cam_info.cam_position[g_track_save_file.cmd_param].zoom[0] = data[2];
				g_cam_info.cam_position[g_track_save_file.cmd_param].zoom[1] = data[3];
				g_cam_info.cam_position[g_track_save_file.cmd_param].zoom[2] = data[4];
				g_cam_info.cam_position[g_track_save_file.cmd_param].zoom[3] = data[5];
			}
		}
	}
	return 0;
}

/**
* @ ��������: jump_zoom_position()
* @ ��������: ��������ͷת��Ԥ��λ
* @ �������: cam_position -- Ԥ��λ���
* @ �������: data -- ������ͷ���͵����������
* @ ����ֵ:   ���͵����ݵĳ���
*/
static int jump_zoom_position(unsigned char *data, int cam_position)
{
	int size = 0;

	int 			fd 						= gRemoteFD; 

	data[0] = g_angle_info.cam_addr;
	data[1] = 0x01;
   	data[2] = 0x04;
   	data[3] = 0x47;

	data[4] = g_cam_info.cam_position[cam_position].zoom[0];	//zoom��λ����Ϣ
	data[5] = g_cam_info.cam_position[cam_position].zoom[1];	//zoom��λ����Ϣ
	data[6] = g_cam_info.cam_position[cam_position].zoom[2];	//zoom��λ����Ϣ
	data[7] = g_cam_info.cam_position[cam_position].zoom[3];	//zoom��λ����Ϣ
	
	data[8] = 0xff;

	size = 9;
	return size;
}

/**
* @ ��������: jump_absolute_position()
* @ ��������: ��������ͷת��Ԥ��λ
* @ �������: cam_position -- Ԥ��λ���
* @ �������: data -- ������ͷ���͵����������
* @ ����ֵ:   ���͵����ݵĳ���
*/
static int jump_absolute_position(unsigned char *data, int cam_position)
{
	int size = 0;

	int 			fd						= gRemoteFD; 

	data[0] = g_angle_info.cam_addr;
	data[1] = 0x01;
	data[2] = 0x06;
	data[3] = 0x02;
	
	
	if(SONY_BRC_Z330 != g_angle_info.cam_type)
	{
		data[4] = 0x18; 	//ˮƽ�����ٶ�
		data[5] = 0x14; 	//��ֱ�����ٶ�
		data[6] = g_cam_info.cam_position[cam_position].pan_tilt[0];	//ˮƽ����λ����Ϣ
		data[7] = g_cam_info.cam_position[cam_position].pan_tilt[1];	//ˮƽ����λ����Ϣ
		data[8] = g_cam_info.cam_position[cam_position].pan_tilt[2];	//ˮƽ����λ����Ϣ
		data[9] = g_cam_info.cam_position[cam_position].pan_tilt[3];	//ˮƽ����λ����Ϣ
		data[10] = g_cam_info.cam_position[cam_position].pan_tilt[4];	//��ֱ����λ����Ϣ
		data[11] = g_cam_info.cam_position[cam_position].pan_tilt[5];	//��ֱ����λ����Ϣ
		data[12] = g_cam_info.cam_position[cam_position].pan_tilt[6];	//��ֱ����λ����Ϣ
		data[13] = g_cam_info.cam_position[cam_position].pan_tilt[7];	//��ֱ����λ����Ϣ
		
		data[14] = 0xff;

		size = 15;
	}
	return size;
}


/**
* @	��������: __call_preset_position()
* @	��������: ����Ԥ��λ
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
int __call_preset_position(short position)
{
	int 					fd 							= gRemoteFD; 
	unsigned char 			cmd_code[CTRL_CMD_LEN] 		= {0};
	int						size						= 0;

	unsigned char	cmd_data[20]	= {0};
	if(SONY_BRC_Z330 == g_angle_info.cam_type)
	{
		if(position > 15)
		{
			position = 0;
		}

		cmd_data[0] = g_angle_info.cam_addr;
		cmd_data[1] = 0x01;
		cmd_data[2] = 0x04;
		cmd_data[3] = 0x3F;
		cmd_data[4] = 0x02;

		cmd_data[5] = position;
		cmd_data[6] = 0xff;
		size = 7;
		SendDataToCom(fd, cmd_data, size);
	}
	else
	{
		size = jump_zoom_position(&cmd_code, position);
		if(size > 0)
		{
			SendDataToCom(fd, cmd_code, size);
		}
		usleep(50000);

		size = jump_absolute_position(&cmd_code, position);
		if(size > 0)
		{
			SendDataToCom(fd, cmd_code, size);
		}
	}
	return 0;
}


/**
* @	��������: call_preset_position()
* @	��������: ����Ԥ��λ
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int call_preset_position(unsigned char *data)
{
	preset_position_info_t 	*preset_position 			= NULL;
	int 					fd 							= gRemoteFD; 
	unsigned char 			cmd_code[CTRL_CMD_LEN] 	= {0};
	int						size						= 0;

	preset_position = (preset_position_info_t *)data;


	DEBUG(DL_DEBUG, "call_preset_position preset_position = %d\n",preset_position->preset_position);

	if(preset_position->preset_position > (PRESET_NUM_MAX - 1))
	{

		DEBUG(DL_DEBUG, "call preset value must less than 6\n");
		return 0;
	}

	cam_trans_teacher_and_encode_req cam_trans_req;
	cam_trans_req.eAuthor = TEACHER_DEBUG;
	cam_trans_req.cCamCoordInfo = g_cam_info.cam_position[preset_position->preset_position];
	int iRet = teacher_send_msg_buff_to_encode((unsigned char *)&cam_trans_req, sizeof(cam_trans_teacher_and_encode_req), MSG_CAM_TRANS);
	if (iRet< 0)
	{
		DEBUG(DL_DEBUG, "call_preset_position--->teacher_send_msg_buff_to_encode error!\n");
	}

	return 0;
}

/**
* @	��������: ctrl_cam_pan_left_rotation()
* @ ��������: ��������ͷ����ת��
* @ �������: direction -- �ƶ�����
* @			  distance -- �ƶ�����
* @ �������: data	-- ������ͷ���͵��������ݰ�
* @ ����ֵ:   ���ݰ�����
*/
static int ctrl_cam_pan_left_rotation(int fd, int speed)
{
	int size = 0;
	unsigned char data[16] = {0};

	//����ת��
	data[0] = g_angle_info.cam_addr;
	data[1] = 0x01;
	data[2] = 0x06;
	data[3] = 0x01;
	data[4] = speed;
	
	if (data[4] >0x18)
	{
		data[4] = 0x18;
	}
	
	data[5] = 0x01;
	data[6] = 0x01;	//02��ʾ����ת��01��ʾ����ת		
	data[7] = 0x03;
	data[8] = 0xff;
	size = 9;
	SendDataToCom(fd, data, size);
	return size;
}

/**
* @	��������: ctrl_cam_pan_right_rotation()
* @ ��������: ��������ͷ����ת��
* @ �������: direction -- �ƶ�����
* @			  distance -- �ƶ�����
* @ �������: data	-- ������ͷ���͵��������ݰ�
* @ ����ֵ:   ���ݰ�����
*/
static int ctrl_cam_pan_right_rotation(int fd, int speed)
{
	int size = 0;
	unsigned char data[16] = {0};

	
	//����ת��
	data[0] = g_angle_info.cam_addr;
	data[1] = 0x01;
	data[2] = 0x06;
	data[3] = 0x01;
	data[4] = speed;
	
	if (data[4] >0x18)
	{
		data[4] = 0x18;
	}
	
	data[5] = 0x01;
	data[6] = 0x02;	//02��ʾ����ת��01��ʾ����ת	
	data[7] = 0x03;
	data[8] = 0xff;
	size = 9;
	SendDataToCom(fd, data, size);
	return size;
}

/**
* @	��������: ctrl_cam_stop_rotation()
* @ ��������: ��������ͷֹͣת��
* @ �������: direction -- �ƶ�����
* @			  distance -- �ƶ�����
* @ �������: data	-- ������ͷ���͵��������ݰ�
* @ ����ֵ:   ���ݰ�����
*/
static int ctrl_cam_stop_rotation(int fd)
{
	int size = 0;
	unsigned char data[16] = {0};

	data[0] = g_angle_info.cam_addr;
	data[1] = 0x01;
	data[2] = 0x06;
	data[3] = 0x01;
	data[4] = 0x01;
	data[5] = 0x01;
	data[6] = 0x03;
	data[7] = 0x03;
	data[8] = 0xff;
	size = 9;

	SendDataToComNoDelay(fd, data, size);
	return size;
}


/**
* @	��������: get_cam_position()
* @	��������: ��ȡ����ͷ��������λ��
* @	�������: ��
* @ �������: data,���ػ�ȡ������ͷ��λ����Ϣ
* @ ����ֵ:   ��
*/
int __get_cam_position(int fd)
{
	unsigned char 	cmd_data[20]	= {0};
	int 			ret 			= -1;
	int 			size 			= 0;

	cmd_data[0] = g_angle_info.cam_addr;
   	cmd_data[1] = 0x09;
   	cmd_data[2] = 0x06;
   	cmd_data[3] = 0x12;
   	cmd_data[4] = 0xFF;
	size = 5;
	SendDataToCom(fd, cmd_data, size);	
}



/**
* @	��������: set_track_students_right_side_is_encode()
* @	��������: �����Ƿ���б���ı�־
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_is_encode(unsigned char *data)
{
	track_is_encode_info_t *encode_info 			= NULL;
	char 					temp[FILE_NAME_LEN]			= {0};
	char 					config_file[FILE_NAME_LEN] 	= {0};
	int 					ret 						= -1;

	encode_info = (track_is_encode_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	if(encode_info->isencode > 1)
	{
		encode_info->isencode = 1;
	}

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", encode_info->isencode);
	lock_save_track_mutex();
	ret =  ConfigSetKey(config_file, TRACK_STUDENTS_RIGHT_SIDE_ENCODE, TRACK_STUDENTS_RIGHT_SIDE_IS_ENCODE, temp);
	unlock_save_track_mutex();
	g_track_encode_info.is_encode = encode_info->isencode;

	DEBUG(DL_DEBUG, "encode_info->isencode = %d\n",encode_info->isencode);
	return;
}


/**
* @	��������: set_track_students_right_side_type()
* @	��������: ���ø�������,0Ϊ�Զ�����,1Ϊ�ֶ�����,�����浽�����ļ���
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_type(unsigned char *data, ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t *dynamic_param)
{
	control_type_info_t *track_info = NULL;

	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	track_info = (control_type_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	dynamic_param->control_mode = track_info->control_type;
	dynamic_param->reset_level = RE_INIT;

	DEBUG(DL_DEBUG, "track_info->control_type = %d\n",track_info->control_type);

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->control_mode);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, CONTROL_MODE, temp);
	unlock_save_track_mutex();
}



/**
* @	��������: set_track_students_right_side_param()
* @	��������: ������ٲ�����
* @	�������: data -- ��encodemanage���յ������ò�������
* @ 		  len -- ��������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_param(unsigned char *data, int len)
{
	char cmd_name[256] = {0};

	FILE *fp;
	fp = fopen("/opt/reach/track_teacher_point_cam_temp.ini","w");
	fwrite(data,len,1,fp);
	fclose(fp);
	lock_save_track_mutex();
	sprintf(cmd_name, "mv /opt/reach/track_teacher_point_cam_temp.ini /opt/reach/%s", TRACK_TEACHER_POINT_CAM_FILE);
	system(cmd_name);
	unlock_save_track_mutex();

	sleep(2);
	system("reboot -f");
}

/**
* @	��������: set_track_students_right_side_range()
* @	��������: ���ø����������
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_range(unsigned char *data, ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t *dynamic_param)
{
	track_range_info_t *track_info = NULL;
	
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	char	text[FILE_NAME_LEN]			= {0};
	int 	ret 	= -1;

	int		index 	= 0;
	

	
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	track_info = (track_range_info_t *)data;

	if(track_info->point_num > TRACK_RANGE_NUM)
	{
		track_info->point_num = TRACK_RANGE_NUM;
	}
	
	dynamic_param->track_point_num = track_info->point_num;
	dynamic_param->reset_level = RE_START;

	DEBUG(DL_DEBUG, "track_info->point_num = %d\n",track_info->point_num);

	lock_save_track_mutex();
	//����������ļ���
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->track_point_num);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRACK_POINT_NUM, temp);
	
	for(index = 0; index < track_info->point_num; index++)
	{
		dynamic_param->track_point[index].x = track_info->point[index].x;
		dynamic_param->track_point[index].y = track_info->point[index].y;
		
		DEBUG(DL_DEBUG, "track_info->point[index].x = %d,index = %d\n",track_info->point[index].x, index);
		DEBUG(DL_DEBUG, "track_info->point[index].y = %d,index = %d\n",track_info->point[index].y, index);
		
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_POINTX);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", dynamic_param->track_point[index].x);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);

		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_POINTY);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", dynamic_param->track_point[index].y);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
	}
	unlock_save_track_mutex();
	
	g_track_encode_info.is_save_class_view = 1;
}

/**
* @	��������: set_track_students_right_side_sens_value()
* @	��������: ���ü��仯��sensֵ
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_sens_value(unsigned char *data, ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t *dynamic_param)
{
	sens_info_t *sens_info = NULL;

	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	sens_info = (sens_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);


	DEBUG(DL_DEBUG, "sens_info->sens = %d\n",sens_info->sens);

	dynamic_param->sens = sens_info->sens;
	dynamic_param->reset_level = RE_INIT;

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->sens);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, SENS, temp);
	unlock_save_track_mutex();
}





/**
* @	��������: set_track_students_right_side_mode()
* @	��������: ���ø���ģʽ��0Ϊ��ԭʼ���ݱȽϣ�1Ϊ����һ֡�Ƚ��Ƿ�Ϊ�仯
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_mode(unsigned char *data, ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t *dynamic_param)
{
	track_mode_info_t *track_mode = NULL;

	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	track_mode = (track_mode_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	dynamic_param->track_mode = track_mode->track_mode;
	dynamic_param->reset_level = RE_INIT;

	DEBUG(DL_DEBUG, "dynamic_param->track_mode = %d\n",track_mode->track_mode);

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->track_mode);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRACK_MODE, temp);
	unlock_save_track_mutex();

}

/**
* @	��������: set_track_students_right_side_manual_commond()
* @	��������: �����ֶ��������
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_manual_commond(unsigned char *data)
{
	manual_commond_info_t *manual_commond_info 			= NULL;
	char 					temp[FILE_NAME_LEN]			= {0};
	char 					config_file[FILE_NAME_LEN] 	= {0};
	int 					ret 						= -1;

	manual_commond_info = (manual_commond_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	switch(manual_commond_info->type)
	{
		default:
			break;
	}

	return;
}


/**
* @	��������: set_draw_line_type()
* @	��������: ���ø�������,0Ϊ�Զ�����,1Ϊ�ֶ�����,�����浽�����ļ���
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_draw_line_type(unsigned char *data, ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t *dynamic_param)
{
	draw_line_info_t *track_line_info = NULL;

	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	track_line_info = (draw_line_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	dynamic_param->message = track_line_info->message;
	dynamic_param->reset_level = NO_INIT;

	DEBUG(DL_DEBUG, "track_line_info->message = %d\n",track_line_info->message);

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->message);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, MESSAGE, temp);
	unlock_save_track_mutex();
}

/**
* @	��������: set_track_students_right_side_trigger_sum()
* @	��������: ���ô��������,Ĭ��ֵΪ3��
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_trigger_sum(unsigned char *data, ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t *dynamic_param)
{
	trigger_num_info_t *trigger_num_info = NULL;
	

	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	trigger_num_info = (trigger_num_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	dynamic_param->trigger_sum = trigger_num_info->trigger_num;
	dynamic_param->reset_level = RE_INIT;


	DEBUG(DL_DEBUG, "trigger_num_info->trigger_num = %d\n",trigger_num_info->trigger_num);

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->trigger_sum);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER_SUM, temp);
	unlock_save_track_mutex();
}

/**
* @	��������: set_track_students_right_side_preset_position()
* @	��������: ����Ԥ��λ
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_preset_position(unsigned char *data)
{
	preset_position_info_t 	*preset_position 			= NULL;
	int 					fd 							= gRemoteFD; 
	unsigned char 			recv_data[32]				= {0};
	int 					ret 						= -1;
	int						index						= 0;

	unsigned char			cmd_data[20]				= {0};
	int 					size						= 0;

	preset_position = (preset_position_info_t *)data;


	DEBUG(DL_DEBUG, "preset_position->preset_position = %d\n",preset_position->preset_position);
	if(preset_position->preset_position >= (PRESET_NUM_MAX - 2))
	{

		DEBUG(DL_DEBUG, "preset value must less than 5\n");
		return 0;
	
	}

	query_cam_coord_info_by_teacher(preset_position->preset_position);
	return 0;
}


/**
* @ ��������: set_track_students_right_side_cam_type()
* @ ��������: ���ø������������
* @ �������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_cam_type(unsigned char *data, angle_info_t *angle_info)
{
	cam_protocol_type_info_t	*cam_type	= NULL;
	
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	cam_type = (cam_protocol_type_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	angle_info->cam_type = cam_type->cam_type;


	DEBUG(DL_DEBUG, "cam_type->cam_type = %d\n",cam_type->cam_type);

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", angle_info->cam_type);
	ret =  ConfigSetKey(config_file, CAM_PARAM, CAM_TYPE_NAME, temp);
	unlock_save_track_mutex();
	return 0;
}

/**
* @ ��������: set_track_students_right_side_position_cam_info()
* @ ��������: ���ö�λ���������Ϣ
* @ �������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_position_cam_info(unsigned char *data, angle_info_t *angle_info)
{
	position_cam_info_t	*position_cam_info	= NULL;
	
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	position_cam_info = (position_cam_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	angle_info->position_camera_lenth = position_cam_info->lenth;
	angle_info->position_camera_width = position_cam_info->width;
	angle_info->position_camera_height = position_cam_info->height;


	DEBUG(DL_DEBUG, "position_cam_info->lenth = %d\n",position_cam_info->lenth);
	DEBUG(DL_DEBUG, "position_cam_info->width = %d\n",position_cam_info->width);
	DEBUG(DL_DEBUG, "position_cam_info->height = %d\n",position_cam_info->height);

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", position_cam_info->lenth);
	ret =  ConfigSetKey(config_file, CAM_PARAM, POSITION_CAM_LENTH, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", position_cam_info->width);
	ret =  ConfigSetKey(config_file, CAM_PARAM, POSITION_CAM_WIDTH, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", position_cam_info->height);
	ret =  ConfigSetKey(config_file, CAM_PARAM, POSITION_CAM_HEIGHT, temp);
	unlock_save_track_mutex();
	return 0;
}


/**
* @ ��������: set_track_students_right_side_track_cam_info()
* @ ��������: ���ø��������������Ϣ
* @ �������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_track_cam_info(unsigned char *data, angle_info_t *angle_info)
{
	track_cam_info_t	*track_cam_info	= NULL;
	
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	track_cam_info = (track_cam_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	angle_info->track_camera_lenth = track_cam_info->lenth;
	angle_info->track_camera_width = track_cam_info->width;
	angle_info->track_camera_height = track_cam_info->height;


	DEBUG(DL_DEBUG, "track_cam_info->lenth = %d\n",track_cam_info->lenth);
	DEBUG(DL_DEBUG, "track_cam_info->width = %d\n",track_cam_info->width);
	DEBUG(DL_DEBUG, "track_cam_info->height = %d\n",track_cam_info->height);

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_cam_info->lenth);
	ret =  ConfigSetKey(config_file, CAM_PARAM, TRACK_CAM_LENTH, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_cam_info->width);
	ret =  ConfigSetKey(config_file, CAM_PARAM, TRACK_CAM_WIDTH, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_cam_info->height);
	ret =  ConfigSetKey(config_file, CAM_PARAM, TRACK_CAM_HEIGHT, temp);
	unlock_save_track_mutex();
	return 0;
}


/**
* @ ��������: set_track_students_right_side_track_cam_offset_info()
* @ ��������: ���ø���������Ƕ�ƫ��ֵ
* @ �������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_track_cam_offset_info(unsigned char *data, angle_info_t *angle_info)
{
	track_cam_offset_info_t	*track_cam_offset_info	= NULL;
	
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	track_cam_offset_info = (track_cam_offset_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	angle_info->camera_pan_offset = track_cam_offset_info->pan;
	angle_info->camera_tilt_offset = track_cam_offset_info->tilt;
	angle_info->camera_zoom_offset = track_cam_offset_info->zoom;


	DEBUG(DL_DEBUG, "track_cam_offset_info->pan = %d\n",track_cam_offset_info->pan);
	DEBUG(DL_DEBUG, "track_cam_offset_info->tilt = %d\n",track_cam_offset_info->tilt);
	DEBUG(DL_DEBUG, "track_cam_offset_info->zoom = %d\n",track_cam_offset_info->zoom);

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_cam_offset_info->pan);
	ret =  ConfigSetKey(config_file, CAM_PARAM, TRACK_CAM_OFFSET_PAN, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_cam_offset_info->tilt);
	ret =  ConfigSetKey(config_file, CAM_PARAM, TRACK_CAM_OFFSET_TILT, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_cam_offset_info->zoom);
	ret =  ConfigSetKey(config_file, CAM_PARAM, TRACK_CAM_OFFSET_ZOOM, temp);
	unlock_save_track_mutex();
	return 0;
}

/**
* @ ��������: set_track_students_right_side_student_ip_info()
* @ ��������: ����ѧ������ip��Ϣ
* @ �������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_student_ip_info(unsigned char *data, track_ip_info_t *ip_info)
{
	track_ip_info_t	*track_ip_info	= NULL;
	
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	track_ip_info = (track_ip_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	ip_info->state = track_ip_info->state;
	memcpy(ip_info->ip, track_ip_info->ip, sizeof(ip_info->ip));


	DEBUG(DL_DEBUG, "student_ip_info->ip = %s\n",ip_info->ip);

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%s", track_ip_info->ip);
	ret =  ConfigSetKey(config_file, NETWORK_INFO, STUDENT_IP, temp);
	unlock_save_track_mutex();

	sleep(2);
	system("reboot -f");
	return 0;
}

/**
* @ ��������: set_track_students_right_side_blockboard_ip_info()
* @ ��������: ����ѧ������ip��Ϣ
* @ �������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_blockboard_ip_info(unsigned char *data, track_ip_info_t *ip_info, int blackboard_no)
{
	track_ip_info_t	*track_ip_info	= NULL;
	char 	text[FILE_NAME_LEN]			= {0};
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	track_ip_info = (track_ip_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	ip_info->state = track_ip_info->state;
	memcpy(ip_info->ip, track_ip_info->ip, sizeof(ip_info->ip));


	DEBUG(DL_DEBUG, "blackboard_no = %d, blackboard_ip_info->ip = %s\n", blackboard_no, ip_info->ip);
		
	lock_save_track_mutex();
	memset(param_name, 0, FILE_NAME_LEN);
	memset(text, 0, FILE_NAME_LEN);
	strcpy(text, BLACKBOARD_IP);
	sprintf(param_name, "%s%d", text, blackboard_no);
	
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%s", track_ip_info->ip);
	ret =  ConfigSetKey(config_file, NETWORK_INFO, param_name, temp);
	unlock_save_track_mutex();

	sleep(2);
	system("reboot -f");
	return 0;
}

/**
* @ ��������: set_track_students_right_side_local_ip_info()
* @ ��������: ���ñ�����ip��Ϣ
* @ �������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_local_ip_info(unsigned char *data, track_ip_info_t *ip_info)
{
	track_ip_info_t	*track_ip_info	= NULL;
	
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	track_ip_info = (track_ip_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	ip_info->state = track_ip_info->state;
	memcpy(ip_info->ip, track_ip_info->ip, sizeof(ip_info->ip));
	memcpy(ip_info->mask, track_ip_info->mask, sizeof(ip_info->mask));
	memcpy(ip_info->route, track_ip_info->route, sizeof(ip_info->route));


	DEBUG(DL_DEBUG, "local_ip_info->ip:%s, mask:%s, route:%s\n",ip_info->ip, ip_info->mask, ip_info->route);

	if (config_sys_ip(ip_info->ip, ip_info->mask, ip_info->route) != 0)
	{//����IPʧ�ܣ��п�����IP���Ϸ����ʲ�д�������ļ�
		return -1;
	}

	ResetIpInfo(ip_info->ip);
	
	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%s", track_ip_info->ip);
	ConfigSetKey(config_file, NETWORK_INFO, LOCAL_IP, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%s", track_ip_info->mask);
	ConfigSetKey(config_file, NETWORK_INFO, LOCAL_MASK, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%s", track_ip_info->route);
	ConfigSetKey(config_file, NETWORK_INFO, LOCAL_GW, temp);
	unlock_save_track_mutex();
	return 0;
}

/**
* @ ��������: set_track_students_right_side_track_other_cam_info()
* @ ��������: ���ø���������Ƕ�ƫ��ֵ
* @ �������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_track_other_cam_info(unsigned char *data, angle_info_t *angle_info)
{
	other_position_cam_info_t	*other_cam_info	= NULL;
	
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	other_cam_info = (other_position_cam_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	angle_info->other_cam_x = other_cam_info->x;
	angle_info->other_cam_y = other_cam_info->y;
	angle_info->other_cam_width	= other_cam_info->width;
	angle_info->other_cam_height = other_cam_info->height;

	DEBUG(DL_DEBUG, "set other_cam_info->x = %d\n",other_cam_info->x);
	DEBUG(DL_DEBUG, "set other_cam_info->y = %d\n",other_cam_info->y);
	DEBUG(DL_DEBUG, "set other_cam_info->width = %d\n",other_cam_info->width);
	DEBUG(DL_DEBUG, "set other_cam_info->height = %d\n",other_cam_info->height);

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", other_cam_info->x);
	ret =  ConfigSetKey(config_file, CAM_PARAM, OTHER_CAM_X, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", other_cam_info->y);
	ret =  ConfigSetKey(config_file, CAM_PARAM, OTHER_CAM_Y, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", other_cam_info->width);
	ret =  ConfigSetKey(config_file, CAM_PARAM, OTHER_CAM_WIDTH, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", other_cam_info->height);
	ret =  ConfigSetKey(config_file, CAM_PARAM, OTHER_CAM_HEIGHT, temp);	
	unlock_save_track_mutex();
	return 0;
}

/**
* @ ��������: set_track_students_right_side_host_type()
* @ ��������: ������������
* @ �������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_track_students_right_side_host_type(unsigned char *data, track_encode_info_t *track_info)
{
	track_host_type_t	*host_type_info	= NULL;
	
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	host_type_info = (track_host_type_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	track_info->host_type = host_type_info->host_type;


	DEBUG(DL_DEBUG, "host_type_info->host_type = %d\n",host_type_info->host_type);

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", host_type_info->host_type);
	ret =  ConfigSetKey(config_file, SYSTEM_PARAM, HOST_TYPE, temp);
	unlock_save_track_mutex();

	sleep(2);
	system("reboot -f");
	return 0;
}


/**
* @	��������: set_blackboard_trigger()
* @	��������: ���úڰ崥����Χ
* @	�������: data -- ��kitutool���յ������ò�������
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int set_blackboard_trigger(unsigned char *data, track_tactics_info_t *track_tactics_info)
{
	broadcast_range_info_t *trigger_info = NULL;
	char	temp[FILE_NAME_LEN] 		= {0};
	char	config_file[FILE_NAME_LEN]	= {0};
	char	param_name[FILE_NAME_LEN]	= {0};
	int 	ret 						= -1;
	int		index 						= 0;
	
	trigger_info = (broadcast_range_info_t *)data;
	
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);


	track_tactics_info->broadcast_info[0].point[0].x = trigger_info->point[0].x;
	track_tactics_info->broadcast_info[0].point[0].y = trigger_info->point[0].y;

	track_tactics_info->broadcast_info[0].point[1].x = trigger_info->point[1].x;
	track_tactics_info->broadcast_info[0].point[1].y = trigger_info->point[1].y;

	track_tactics_info->broadcast_info[1].point[0].x = trigger_info->point[2].x;
	track_tactics_info->broadcast_info[1].point[0].y = trigger_info->point[2].y;

	track_tactics_info->broadcast_info[1].point[1].x = trigger_info->point[3].x;
	track_tactics_info->broadcast_info[1].point[1].y = trigger_info->point[3].y;

	
	
	DEBUG(DL_WARNING,"recv blackboard trigger=%d,%d,%d,%d,%d,%d,%d,%d\n",
		trigger_info->point[0].x,trigger_info->point[0].y,trigger_info->point[1].x,trigger_info->point[1].y,
		trigger_info->point[2].x,trigger_info->point[2].y,trigger_info->point[3].x,trigger_info->point[3].y);

	lock_save_track_mutex();
	//����������ļ���
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_tactics_info->broadcast_info[0].point[0].x);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD1_TRIGGER_X0, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_tactics_info->broadcast_info[0].point[0].y);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD1_TRIGGER_Y0, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_tactics_info->broadcast_info[0].point[1].x);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD1_TRIGGER_X1, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_tactics_info->broadcast_info[0].point[1].y);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD1_TRIGGER_Y1, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_tactics_info->broadcast_info[1].point[0].x);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD2_TRIGGER_X0, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_tactics_info->broadcast_info[1].point[0].y);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD2_TRIGGER_Y0, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_tactics_info->broadcast_info[1].point[1].x);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD2_TRIGGER_X1, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_tactics_info->broadcast_info[1].point[1].y);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD2_TRIGGER_Y1, temp);

	unlock_save_track_mutex();
	return 0;
}

/**
* @	��������: set_feature_view_start_y()
* @	��������: ������ʦ��д�ĳ�ʼ��Χ��������д�Ŀ�ͼ����
* @	�������: data -- ��kitutool���յ������ò�������
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int set_feature_view_start_y(unsigned char *data, track_tactics_info_t *track_tactics_info)
{
	broadcast_range_info_t *trigger_info = NULL;
	char	temp[FILE_NAME_LEN] 		= {0};
	char	config_file[FILE_NAME_LEN]	= {0};
	char	param_name[FILE_NAME_LEN]	= {0};
	int 	ret 						= -1;
	int		index 						= 0;
	
	trigger_info = (broadcast_range_info_t *)data;
	
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);


	track_tactics_info->teacher_feature_info[0].point[0].x = trigger_info->point[0].x;
	track_tactics_info->teacher_feature_info[0].point[0].y = trigger_info->point[0].y;

	track_tactics_info->teacher_feature_info[0].point[1].x = trigger_info->point[1].x;
	track_tactics_info->teacher_feature_info[0].point[1].y = trigger_info->point[1].y;

	track_tactics_info->teacher_feature_info[1].point[0].x = trigger_info->point[2].x;
	track_tactics_info->teacher_feature_info[1].point[0].y = trigger_info->point[2].y;

	track_tactics_info->teacher_feature_info[1].point[1].x = trigger_info->point[3].x;
	track_tactics_info->teacher_feature_info[1].point[1].y = trigger_info->point[3].y;

	
	
	DEBUG(DL_WARNING,"recv set_feature_view_start_y trigger=%d,%d,%d,%d,%d,%d,%d,%d\n",
		trigger_info->point[0].x,trigger_info->point[0].y,trigger_info->point[1].x,trigger_info->point[1].y,
		trigger_info->point[2].x,trigger_info->point[2].y,trigger_info->point[3].x,trigger_info->point[3].y);

	track_tactics_info->teacher_feature_info[0].point[0].y = track_tactics_info->teacher_feature_info[0].point[0].y * 3;//��720PתΪ1080P
	lock_save_track_mutex();
	//����������ļ���
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_tactics_info->teacher_feature_info[0].point[0].y);	
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, TEACHER_FEATURE_START_Y, temp);


	unlock_save_track_mutex();
	return 0;
}

/**
* @	��������: set_strategy_no()
* @	��������: ���û�λ,Ĭ��ֵΪ����λ
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int set_strategy_no(unsigned char *data,track_strategy_info_t *track_strategy_info)
{
	default_msg_info_t *strategy_no_info = NULL;

	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	int 	stu_num=1;
	int		i=0;
	int 	strategy_no=0;
	strategy_no_info = (default_msg_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	track_strategy_info->strategy_no= strategy_no_info->value;
	DEBUG(DL_WARNING, "recv strategy_no = %d\n",strategy_no_info->value);

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_strategy_info->strategy_no);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, STRATEGY_NO, temp);
	unlock_save_track_mutex();

	SendStrategyNoToEncode();
	return 0;
}


/**
* @	��������: set_track_turn_info()
* @	��������: ����ѧ������ѯ��Ϣ
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int set_track_turn_info(unsigned char *data,track_strategy_info_t *track_strategy_info, stu_turn_info_t *stu_turn_info)
{
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	char	text[FILE_NAME_LEN]			= {0};
	int 	ret 						= -1;
	int 	stu_num						= 0;
	int 	lenth						= 0;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);
	
	memcpy(stu_turn_info,data,sizeof(stu_turn_info_t));
	lenth=sizeof(track_turn_info_t);

	for(stu_num=0;stu_num<track_strategy_info->stu_num;stu_num++)
	{
		DEBUG(DL_WARNING, "send turn info to student %d ,g_stu_turn_info.type= %d,time=%d,showtime=%d\n",stu_num,stu_turn_info->tStuTrackTurnInfo[stu_num].nTrackTurnType,stu_turn_info->tStuTrackTurnInfo[stu_num].nTrackTurnTime,stu_turn_info->tStuTrackTurnInfo[stu_num].nTrackTurnShowTime);
		//teacher_send_msg_to_students(&(g_stu_turn_info.tStuTrackTurnInfo[stu_num]),lenth,MSG_TRACK_TURN_INFO,stu_num);
		//д�����ļ�
		lock_save_track_mutex();
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_TURN_TYPE);
		sprintf(param_name, "%s%d",text, stu_num);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", stu_turn_info->tStuTrackTurnInfo[stu_num].nTrackTurnType);
		ret =  ConfigSetKey(config_file, TRACK_TURN_INFO, param_name, temp);
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_TURN_TIME);
		sprintf(param_name, "%s%d",text, stu_num);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", stu_turn_info->tStuTrackTurnInfo[stu_num].nTrackTurnTime);
		ret =  ConfigSetKey(config_file, TRACK_TURN_INFO, param_name, temp);
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_TURN_SHOW_TIME);
		sprintf(param_name, "%s%d",text, stu_num);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", stu_turn_info->tStuTrackTurnInfo[stu_num].nTrackTurnShowTime);
		ret =  ConfigSetKey(config_file, TRACK_TURN_INFO, param_name, temp);
		if(ret == 0) 
		{
			DEBUG(DL_WARNING, "set student %d ,g_stu_turn_info.type= %d,time=%d,showtime=%d\n",stu_num,stu_turn_info->tStuTrackTurnInfo[stu_num].nTrackTurnType,stu_turn_info->tStuTrackTurnInfo[stu_num].nTrackTurnTime,stu_turn_info->tStuTrackTurnInfo[stu_num].nTrackTurnShowTime);
		}
		unlock_save_track_mutex();
	}
	return 0;
}

/**
* @	��������: set_students_panorama_switch_near_time()
* @	��������: ����set_students_panorama_switch_near_time 
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int set_track_strategy_time(unsigned char *data,void *key,int *pTimeVal)
{
	default_msg_info_t *time_info = NULL;
	

	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	time_info = (default_msg_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);
	*pTimeVal= time_info->value;


	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", time_info->value);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, key, temp);
	unlock_save_track_mutex();
	DEBUG(DL_WARNING, "recv set %s=%d\n",key,time_info->value);
	return 0;
}


/**
* @	��������: set_camara_speed()
* @	��������: ���ô��������,Ĭ��ֵΪ3��
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: cam_contror_info -- ����ͷ���Ʋ����ṹ��
* @ ����ֵ:   ��
*/
static int set_camara_speed(unsigned char *data, angle_info_t *angle_info)
{
	cam_speed_info_t *cam_speed_info = NULL;
	

	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	cam_speed_info = (cam_speed_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	angle_info->cam_speed = cam_speed_info->cam_speed;
	

	DEBUG(DL_WARNING, "angle_info->cam_speed = %d\n",angle_info->cam_speed);

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", angle_info->cam_speed);
	ret =  ConfigSetKey(config_file, CAM_PARAM, CAM_SPEED, temp);
	unlock_save_track_mutex();
	return 0;
}



/**
* @	��������: set_or_clear_limit_position()
* @	��������: ���ô��������,Ĭ��ֵΪ3��
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_or_clear_limit_position(unsigned char *data)
{
	limit_position_info_t 	*limit_position 			= NULL;
	int 					fd 							= gRemoteFD; 
	char 					temp[FILE_NAME_LEN]			= {0};
	char 					config_file[FILE_NAME_LEN] 	= {0};
	unsigned char 			recv_data[32]				= {0};
	unsigned char 			temp_data[32]				= {0};
	int 					ret 						= -1;
	int						index						= 0;

	limit_position = (limit_position_info_t *)data;


	DEBUG(DL_WARNING, "limit_position->limit_position = %d\n",limit_position->limit_position);

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);
	memset(temp, 0, FILE_NAME_LEN);

	if(LIMIT_CLEAR == limit_position->limit_position)
	{
		if(SONY_BRC_Z330 == g_angle_info.cam_type)
		{
			
			strcpy(temp, "7ffff7fff");
			g_track_strategy_info.cam_left_limit=0x7ffff7fff;
			g_track_strategy_info.cam_right_limit=0x7ffff7fff;
		}
		else
		{
			g_track_strategy_info.cam_left_limit=0x7fff7fff;
			g_track_strategy_info.cam_right_limit=0x7fff7fff;
			strcpy(temp, "7fff7fff");
		}
		lock_save_track_mutex();
		if(KXWELL_CAM == g_angle_info.cam_type)
		{
			ret =  ConfigSetKey(config_file, TEACH_CAM_LIMIT_POSITION, LIMIT_KXWELL_UP_NAME, temp);
			ret =  ConfigSetKey(config_file, TEACH_CAM_LIMIT_POSITION, LIMIT_KXWELL_DOWN_NAME, temp);
			ret =  ConfigSetKey(config_file, TEACH_CAM_LIMIT_POSITION, LIMIT_KXWELL_LEFT_NAME, temp);
			ret =  ConfigSetKey(config_file, TEACH_CAM_LIMIT_POSITION, LIMIT_KXWELL_RIGHT_NAME, temp);
		}
		else
		{
			ret =  ConfigSetKey(config_file, TEACH_CAM_LIMIT_POSITION, LIMIT_LEFT_DOWN_NAME, temp);
			ret =  ConfigSetKey(config_file, TEACH_CAM_LIMIT_POSITION, LIMIT_RIGHT_UP_NAME, temp);
			clear_limit_position();
		}
		unlock_save_track_mutex();
	}
	else
	{
		g_track_save_file.set_cmd = SET_LIMITE_POSITION;
		g_track_save_file.cmd_param = limit_position->limit_position;
		get_cam_position(&recv_data);
	}
	return 0;
}

/**
* @ ��������: set_refresh_background()
* @ ��������: ����ˢ�±������
* @ �������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_refresh_background(unsigned char *data, track_encode_info_t *track_info)
{
	refresh_background_info_t	*refresh_background_info	= NULL;
	
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	refresh_background_info = (refresh_background_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	//refresh_timeΪ0��ʾ��ˢ�±���
	track_info->refresh_time = refresh_background_info->refresh_time;


	DEBUG(DL_DEBUG, "track_info->refresh_time = %d\n",track_info->refresh_time);

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", refresh_background_info->refresh_time);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, REFRESH_TIME, temp);
	unlock_save_track_mutex();
	return 0;
}

/**
* @ ��������: set_lost_mode()
* @ ��������: ������ʦ��ʧ��ĸ�������
* @ �������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
static int set_lost_mode(unsigned char *data,track_strategy_info_t *track_strategy_info)
{
	nomal_info_t *lost_mode_info = NULL;

	char	temp[FILE_NAME_LEN] 		= {0};
	char	config_file[FILE_NAME_LEN]	= {0};
	char	param_name[FILE_NAME_LEN]	= {0};
	int 	ret 						= -1;
	int 	stu_num=1;
	int 	i=0;
	int 	lostmode=0;
	lost_mode_info = (nomal_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	track_strategy_info->lostmode= lost_mode_info->val0;
	DEBUG(DL_WARNING, "recv lost_mode = %d\n",lost_mode_info->val0);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", track_strategy_info->lostmode);
	lock_save_track_mutex();
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, LOST_MODE, temp);
	unlock_save_track_mutex();
	
	for(i=0;i<track_strategy_info->stu_num;i++)
	{
		lostmode=track_strategy_info->lostmode;
		teacher_send_msg_to_students((unsigned char *)&lostmode,sizeof(int), MSG_LOST_MODE,i);
		DEBUG(DL_WARNING, "yyyyyyyyyyy send to students %d  lostmode = %d\n",i,lostmode);
	}
	return 0;
}

/**
* @	��������: set_trigger_range()
* @	��������: ���ô����������
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int set_trigger_range(unsigned char *data, ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t *dynamic_param)
{
	trigger_range_info_t *trigger_info = NULL;
	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	trigger_info = (trigger_range_info_t *)data;
	
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	dynamic_param->reset_level = RE_INIT;

	dynamic_param->trigger[0].trigger_x0= trigger_info->point[0].x;
	dynamic_param->trigger[0].trigger_y0= trigger_info->point[0].y;

	dynamic_param->trigger[0].trigger_x1= trigger_info->point[1].x;
	dynamic_param->trigger[0].trigger_y1= trigger_info->point[1].y;
	
	dynamic_param->trigger[1].trigger_x0= trigger_info->point[2].x;
	dynamic_param->trigger[1].trigger_y0= trigger_info->point[2].y;

	dynamic_param->trigger[1].trigger_x1= trigger_info->point[3].x;
	dynamic_param->trigger[1].trigger_y1= trigger_info->point[3].y;
	
	DEBUG(DL_WARNING,"yyyyyyyyy recv trigger[0]=%d,%d,%d,%d,trigger[1]=%d,%d,%d,%d\n",
		trigger_info->point[0].x,trigger_info->point[0].y,trigger_info->point[1].x,trigger_info->point[1].y,
		trigger_info->point[2].x,trigger_info->point[2].y,trigger_info->point[3].x,trigger_info->point[3].y);

	lock_save_track_mutex();
	//����������ļ���
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->trigger[0].trigger_x0);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER0_X0, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->trigger[0].trigger_y0);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER0_Y0, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->trigger[0].trigger_x1);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER0_X1, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->trigger[0].trigger_y1);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER0_Y1, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->trigger[1].trigger_x0);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER1_X0, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->trigger[1].trigger_y0);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER1_Y0, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->trigger[1].trigger_x1);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER1_X1, temp);

	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", dynamic_param->trigger[1].trigger_y1);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER1_Y1, temp);

	unlock_save_track_mutex();
	
	return 0;
}


/**
* @	��������: set_highpreset()
* @	��������: �����Ƿ����ø���ʦ�������
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: cam_contror_info -- ����ͷ���Ʋ����ṹ��
* @ ����ֵ:   ��
*/
static int set_highpreset(unsigned char *data)
{
	nomal_info_t *highpreset_info = NULL;
	

	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	highpreset_info = (nomal_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);
//��ʱ��֧�ָߵ�Ԥ��λ
	g_track_encode_info.highpreset = 0;//highpreset_info->val0;
	

	DEBUG(DL_WARNING, "set g_track_encode_info.highpreset = %d\n",g_track_encode_info.highpreset);

	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", g_track_encode_info.highpreset);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, HIGH_PRESET, temp);
	unlock_save_track_mutex();
	return 0;
}

/**
* @	��������: set_blackboard_scheme()
* @	��������: ���ð��鷽��
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��nomal_info_t
*/
static int set_blackboard_scheme(unsigned char *data,track_strategy_info_t *track_strategy_info)
{
	nomal_info_t *blackboard_scheme_info = NULL;

	char	temp[FILE_NAME_LEN] 		= {0};
	char	config_file[FILE_NAME_LEN]	= {0};
	char	param_name[FILE_NAME_LEN]	= {0};
	int 	ret 						= -1;
	int 	i=0;
	int 	lostmode=0;
	blackboard_scheme_info = (nomal_info_t *)data;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);
	if(blackboard_scheme_info->val0==3)
	{//���ð���ģʽ
		track_strategy_info->blackboard_scheme= blackboard_scheme_info->val1;
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%d", track_strategy_info->blackboard_scheme);
		lock_save_track_mutex();
		ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD_SCHEME, temp);
		unlock_save_track_mutex();
		DEBUG(DL_WARNING, "recv blackboard_scheme = %d,reboot\n",blackboard_scheme_info->val1);
		system("reboot -f");
	}
	else
	{//������ʦ����ѧ����ģʽ,ֻ���װ�����
		DEBUG(DL_WARNING, "recv blackboard_scheme useless msg\n");
	}
	return 0;
}




/**
* @	��������: track_init_default_param()
* @ ��������: ��ʼ��Ĭ�ϵĸ��ٲ��������ڻָ���������
* @ �������: 
* @ �������:
* @ ����ֵ:
*/
static int track_init_default_param()
{
	int 	ret = -1;
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char 	temp[FILE_NAME_LEN]			= {0};
	char	text[FILE_NAME_LEN]			= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int		index						= 0;
	int		i 							= 0;
	char cmd_name[256] = {0};	
	int val,val_y;
	int stu_num;
	lock_save_track_mutex();	
	sprintf(cmd_name, "rm /opt/reach/%s", TRACK_TEACHER_POINT_CAM_FILE);	
	system(cmd_name);	
	DEBUG(DL_DEBUG, "track_init_default_param,delete track ini file!\n");

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);
	
	strcpy(config_file, "/opt/reach/test_track_teacher_point_cam.ini");
	//��Ƶ���
	val=704;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, STATIC_NAME, VIDEO_WIDTH, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  static_param.video_width = %d\n",val);

	
	//��Ƶ�߶�
	val=576;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, STATIC_NAME, VIDEO_HEIGHT, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  static_param.video_height = %d\n",val);

	//���ٴ���ͼ��Ŀ��
	val=160;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, STATIC_NAME, PIC_WIDTH, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  static_param.pic_width = %d\n",val);

	//���ٴ���ͼ��ĸ߶�
	val=90;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, STATIC_NAME, PIC_HEIGHT, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  static_param.pic_height = %d\n",val);

	//����ģʽ,���Զ�ģʽ�����ֶ�ģʽ
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, CONTROL_MODE, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  dynamic_param.control_mode = %d\n",val);

	//�����������
	val=4;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRACK_POINT_NUM, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  dynamic_param.track_point_num = %d\n",val);

	for(index = 0; index < 4; index++)
	{
		//��������������		
		switch(index)		
		{			
			case 0:				
				val=0;				
				val_y=200;				
				break;			
			case 1:				
				val=700;				
				val_y=200;				
				break;			
			case 2:				
				val=700;				
				val_y=260;				
				break;			
			case 3:				
				val=0;				
				val_y=260;				
				break;			
			default:				
				break;		
		}	
		memset(text, 0, FILE_NAME_LEN);
		memset(param_name, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_POINTX);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN); 	
		sprintf(temp, "%d", val);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
		DEBUG(DL_DEBUG, "track_init_default_param  dynamic_param.track_point[%d].x = %d\n",index,val);

		memset(text, 0, FILE_NAME_LEN);
		memset(param_name, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_POINTY);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN); 	
		sprintf(temp, "%d", val_y);
		ret =  ConfigSetKey(config_file, DYNAMIC_NAME, param_name, temp);
		DEBUG(DL_DEBUG, "track_init_default_param  dynamic_param.track_point[%d].y = %d\n",index,val_y);

	}

	//����������
	val=30;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER_SUM, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  dynamic_param.trigger_sum = %d\n",val);

	//sensֵ,ԽС��Եֵ�ҵ�Խ��
	val=30;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, SENS, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  dynamic_param.sens = %d\n",val);

	//��ʦ��ʧ�ĳ�ʱʱ��
	val=50;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, RESET_TIME, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  dynamic_param.RESET_TIME = %d\n",val);
		
	//����ȡ����ʽ��0�ǿ�ǰȡ����1�ǿ���ȡ��
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRACK_MODE, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  dynamic_param.track_mode = %d\n",val);

	
	//���߱�־
	val=1;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, MESSAGE, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  dynamic_param.message = %d\n",val);


	//����ͷԤ��λ��λ����Ϣ,0��λ����ʱ����,û��ʹ��
	for(index = 1; index < PRESET_NUM_MAX; index++)
	{		
		memset(text, 0, FILE_NAME_LEN);		
		memset(param_name, 0, FILE_NAME_LEN);
		strcpy(text, PRESET_POSITION_VALUE);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%x%x%x%x%x%x%x%x",0x07,0x0f,0x0f,0x0f,0x07,0x0f,0x0f,0x0f);
		ret =  ConfigSetKey(config_file, PRESET_POSITION, param_name, temp);
		DEBUG(DL_DEBUG, "track_init_default_param  ######index = %d,%s\n",index, temp);

		memset(text, 0, FILE_NAME_LEN);		
		memset(param_name, 0, FILE_NAME_LEN);
		strcpy(text, PRESET_ZOOM_VALUE);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%x%x%x%x",0x07,0x0f,0x0f,0x0f);
		ret =  ConfigSetKey(config_file, PRESET_POSITION, param_name, temp);
		
	}

	//���ٻ��Ƿ����
	val=1;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, TRACK_STUDENTS_RIGHT_SIDE_ENCODE, TRACK_STUDENTS_RIGHT_SIDE_IS_ENCODE, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_track_encode_info.is_encode = %d\n",val);

	//���������
	val=BLM_500R;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, CAM_PARAM, CAM_TYPE_NAME, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_angle_info.cam_type = %d\n",val);

	//����ͷת���ٶȻ���ֵ
	val=5;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, CAM_PARAM, CAM_SPEED, temp);
	DEBUG(DL_WARNING, "track_init_default_param  g_angle_info.cam_speed = %d\n",val);

	//��λ��������ڵĽ��ҳ���
	val=1;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, CAM_PARAM, POSITION_CAM_LENTH, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_angle_info.position_camera_lenth = %d\n",val);

	//��λ��������ڵĽ��ҿ��
	val=1;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, CAM_PARAM, POSITION_CAM_WIDTH, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_angle_info.position_camera_width = %d\n",val);

	//��λ��������ڵĸ߶�
	val=1;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, CAM_PARAM, POSITION_CAM_HEIGHT, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_angle_info.position_camera_height = %d\n",val);

	//������������ڵĳ���
	val=1;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, CAM_PARAM, TRACK_CAM_LENTH, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_angle_info.track_camera_lenth = %d\n",val);

	//������������ڵĿ��
	val=1;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, CAM_PARAM, TRACK_CAM_WIDTH, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_angle_info.track_camera_width = %d\n",val);

	
	//������������ڵĸ߶�
	val=1;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, CAM_PARAM, TRACK_CAM_HEIGHT, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_angle_info.track_camera_height = %d\n",val);

	//���������ˮƽ����ƫ��ֵ
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, CAM_PARAM, TRACK_CAM_OFFSET_PAN, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_angle_info.camera_pan_offset = %d\n",val);

	//�����������ֱ����ƫ��ֵ
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, CAM_PARAM, TRACK_CAM_OFFSET_TILT, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_angle_info.camera_tilt_offset = %d\n",val);

	//�������������ƫ��ֵ
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, CAM_PARAM, TRACK_CAM_OFFSET_ZOOM, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_angle_info.camera_zoom_offset = %d\n",val);

	//����һ��������ڱ���ͼ���е�λ��x
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, CAM_PARAM, OTHER_CAM_X, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_angle_info.other_cam_x = %d\n",val);

	//����һ��������ڱ���ͼ���е�λ��y
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, CAM_PARAM, OTHER_CAM_Y, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_angle_info.other_cam_y = %d\n",val);

	
	//����һ��������ڱ���ͼ���еĿ�
	val=1;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, CAM_PARAM, OTHER_CAM_WIDTH, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_angle_info.other_cam_width = %d\n",val);

	//����һ��������ڱ���ͼ���еĸ�
	val=1;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, CAM_PARAM, OTHER_CAM_HEIGHT, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_angle_info.other_cam_height = %d\n",val);

	//����1������ⷶΧ�����϶����xֵ
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD1_TRIGGER_X0, temp);
	DEBUG(DL_WARNING, "track_init_default_param  broadcast_info[0].point[0].x = %d\n",val);

	//����1������ⷶΧ�����϶����Yֵ
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD1_TRIGGER_Y0, temp);
	DEBUG(DL_WARNING, "track_init_default_param  broadcast_info[0].point[0].y = %d\n",val);

	//����1������ⷶΧ�����¶����xֵ
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD1_TRIGGER_X1, temp);
	DEBUG(DL_WARNING, "track_init_default_param  broadcast_info[0].point[1].x = %d\n",val);

	//����1������ⷶΧ�����¶����yֵ
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD1_TRIGGER_Y1, temp);
	DEBUG(DL_WARNING, "track_init_default_param  broadcast_info[0].point[1].y = %d\n",val);

	
	//����2������ⷶΧ�����϶����xֵ
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD2_TRIGGER_X0, temp);
	DEBUG(DL_WARNING, "track_init_default_param  broadcast_info[1].point[0].x = %d\n",val);

	//����2������ⷶΧ�����϶����Yֵ
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD2_TRIGGER_Y0, temp);
	DEBUG(DL_WARNING, "track_init_default_param  broadcast_info[1].point[0].y = %d\n",val);

	//����2������ⷶΧ�����¶����xֵ
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD2_TRIGGER_X1, temp);
	DEBUG(DL_WARNING, "track_init_default_param  broadcast_info[1].point[1].x = %d\n",val);

	//����2������ⷶΧ�����¶����yֵ
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, BLACKBOARD2_TRIGGER_Y1, temp);
	DEBUG(DL_WARNING, "track_init_default_param  broadcast_info[1].point[1].y = %d\n",val);

	//��ʦ��д����Y���ϱ߽��ֵ
	val=540;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, TEACHER_FEATURE_START_Y, temp);
	DEBUG(DL_WARNING, "track_init_default_param  teacher_feature_start_y = %d\n",val);
	
	//ѧ������ipֵ
	char student_ip[16]="172.25.100.102";	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%s", student_ip);
	ret =  ConfigSetKey(config_file, NETWORK_INFO, STUDENT_IP, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  student ip = %s\n",student_ip);

	//������ipֵ
	char local_ip[16]="172.25.100.101";	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%s", local_ip);
	ret =  ConfigSetKey(config_file, NETWORK_INFO, LOCAL_IP, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  local ip = %s\n",local_ip);

	//����MASK��ֵ
	char local_mask[16]="255.255.255.0";
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%s", local_mask);
	ret =  ConfigSetKey(config_file, NETWORK_INFO, LOCAL_MASK, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  local_mask = %s\n",local_mask);

	//����ROUTE��ֵ
	char local_route[16]="172.25.100.254";
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%s", local_route);
	ret =  ConfigSetKey(config_file, NETWORK_INFO, LOCAL_GW, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  local_route = %s\n",local_route);


	//�ñ����ض�������: ��ʦȫ��(������һ��Ϊѧ����д)�ı���ʱ��
	val=3;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, STUDENTS_PANORAMA_SWITCH_NEAR_TIME);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param students_panorama_switch_near_time = %d\n", val);

	//�ñ�����������:��ʦ��д��С�ı���ʱ��
	val=3;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, TEACHER_BLACKBOARD_TIME1);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param teacher_blackboard_time1 = %d\n",val);

	//�ñ�����������: ѧ��ȫ����С�ı���ʱ��
	val=3;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, TEACHER_LEAVE_BLACKBOARD_TIME1);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param teacher_leave_blackboard_time1 = %d\n",val);

	//�ñ�����������:ѧ��ȫ�����ı���ʱ��
	val=5;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, TEACHER_BLACKBOARD_TIME2);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param teacher_blackboard_time2 = %d\n",val);

	//�ñ�����������: ѧ����д��С�ı���ʱ��
	val=3;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, TEACHER_LEAVE_BLACKBOARD_TIME2);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param teacher_leave_blackboard_time2 = %d\n",val);

	//�ñ�����������: ��ʦȫ����С�ı���ʱ��
	val=3;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, STUDENTS_DOWN_TIME);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param students_down_time = %d\n",val);

	
	//�л���ʦȫ��Ҫ����ʱ�����л���ʦȫ��ʱ��ʱһ��ʱ�����л���ʦȫ��
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, TEACHER_PANORAMA_TIME);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param teacher_panorama_time = %d\n",val);

	//�л���ʦȫ��Ҫ����ʱ�����л���ʦȫ��ʱ��ʱһ��ʱ�����л���ʦȫ��
	val=3;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, TEACHER_LEAVE_PANORAMA_TIME);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param teacher_leave_panorama_time = %d\n",val);

	//�ñ�����������:ѧ����д���ı���ʱ��
	val=5;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, TEACHER_KEEP_PANORAMA_TIME);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param teacher_keep_panorama_time = %d\n",val);

	//�ñ�����������: ���龵ͷ��С����ʱ��
	val=3;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, TEACHER_SWITCH_STUDENTS_DELAY_TIME);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param teacher_switch_students_delay_time = %d\n",val);

	//�ñ�������: ��ʦȫ��(������һ��Ϊѧ��ȫ��)�ı���ʱ��
	val=3;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, STUDENTS_NEAR_KEEP_TIME);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param students_near_keep_time = %d\n",val);

	//�ñ�������: ��ʦȫ��(������һ��Ϊ��ʦ��д)�ı���ʱ��
	val=3;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, VGA_KEEP_TIME);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param vga_keep_time = %d\n",val);

	//��ʦ��������ĳ�ʱʱ��
	val=5;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, TEACHER_DETECT_HEART_TIMEOUT);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param teacher_detect_heart_timeout = %d\n",val);
	
	//��ʦ���ˣ������ƶ�Ŀ��ʱѧ��ȫ������ʦȫ����ͷ�л���ʱ
	val=3;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, MV_KEEP_TIME);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param mv_keep_time = %d\n",val);

	//��û�λ��Ϣ
	val=STRATEGY_6;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, STRATEGY_NO);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param  strategy_no = %d\n",val);

	//ѧ��������
	val=1;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, STU_NUM);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param  stu_num = %d\n",val);

	//���������
	val=BLACKCONNECT_NUM_MAX;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	strcpy(text, BLACKBOARD_NUM);
	sprintf(param_name, "%s",text);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, param_name, temp);
	DEBUG(DL_WARNING, "track_init_default_param  blackboard_num = %d\n", val);

	int blackboard_num = 0;
	for (blackboard_num = 0; blackboard_num < val; blackboard_num++)
	{
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, BLACKBOARD_IP);
		sprintf(param_name, "%s%d", text, blackboard_num);
		
		memset(temp, 0, FILE_NAME_LEN);
		sprintf(temp, "%s", BLACKBOARDTRACE_IP[blackboard_num]);
		
		ret =  ConfigSetKey(config_file, NETWORK_INFO, param_name, temp);
		DEBUG(DL_DEBUG, "track_init_default_param  student ip = %s\n",student_ip);
	}

	//������������϶����xֵ,������0
	val=0;	
	memset(temp, 0, FILE_NAME_LEN); 
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER0_X0, temp);
	DEBUG(DL_WARNING, "track_init_default_param trigger[0].trigger_x0 = %d\n",val);
	
	//������������϶����Yֵ,������0
	val=0;	
	memset(temp, 0, FILE_NAME_LEN); 
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER0_Y0, temp);
	DEBUG(DL_WARNING, "track_init_default_param trigger[0].trigger_y0 = %d\n",val);

	//������������¶����xֵ,������0
	val=0;	
	memset(temp, 0, FILE_NAME_LEN); 
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER0_X1, temp);
	DEBUG(DL_WARNING, "track_init_default_param trigger[0].trigger_x1 = %d\n",val);

	//������������¶����yֵ,������0
	val=0;	
	memset(temp, 0, FILE_NAME_LEN); 
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER0_Y1, temp);
	DEBUG(DL_WARNING, "track_init_default_param trigger[0].trigger_y1 = %d\n",val);

	
	//������������϶����xֵ,������1
	val=0;	
	memset(temp, 0, FILE_NAME_LEN); 
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER1_X0, temp);
	DEBUG(DL_WARNING, "track_init_default_param trigger[1].trigger_x0 = %d\n",val);
	
	//������������϶����Yֵ,������1
	val=0;	
	memset(temp, 0, FILE_NAME_LEN); 
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER1_Y0, temp);
	DEBUG(DL_WARNING, "track_init_default_param trigger[1].trigger_y0 = %d\n",val);
	
	//������������¶����xֵ,������1
	val=0;	
	memset(temp, 0, FILE_NAME_LEN); 
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER1_X1, temp);
	DEBUG(DL_WARNING, "track_init_default_param trigger[1].trigger_x1 = %d\n",val);

	//������������¶����yֵ,������1
	val=0;	
	memset(temp, 0, FILE_NAME_LEN); 
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, TRIGGER1_Y1, temp);
	DEBUG(DL_WARNING, "track_init_default_param trigger[1].trigger_y1 = %d\n",val);

	
	//��ȡ��ѵ��Ϣ
	for(stu_num=0;stu_num<2;stu_num++)
	{
		//��ȡ��ѯ����,Ĭ��Ϊ0������ѯ��1��ʾ��ѯ
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_TURN_TYPE);
		sprintf(param_name, "%s%d",text, stu_num);
		val=0;	
		memset(temp, 0, FILE_NAME_LEN); 
		sprintf(temp, "%d", val);
		ret =  ConfigSetKey(config_file, TRACK_TURN_INFO, param_name, temp);
		DEBUG(DL_WARNING, "track_init_default_param  student %d ,g_stu_turn_info.type= %d,",stu_num,val);
		
		//������ѯʱ����,Ĭ��Ϊ120��120s��ѯһ��
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_TURN_TIME);
		sprintf(param_name, "%s%d",text, stu_num);
		val=0;	
		memset(temp, 0, FILE_NAME_LEN); 
		sprintf(temp, "%d", val);
		ret =  ConfigSetKey(config_file, TRACK_TURN_INFO, param_name, temp);
		DEBUG(DL_WARNING, "time=%d,",val);

		//������ѯ�೤ʱ���л���ʦ,Ĭ��2s���л���ʦ
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_TURN_SHOW_TIME);
		sprintf(param_name, "%s%d",text, stu_num);
		val=0;	
		memset(temp, 0, FILE_NAME_LEN); 
		sprintf(temp, "%d", val);
		ret =  ConfigSetKey(config_file, TRACK_TURN_INFO, param_name, temp);
		DEBUG(DL_WARNING, "showtime=%d\n",val);
	}
	
	//ˢ�±���ʱ��
	val=300;
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, REFRESH_TIME, temp);
	DEBUG(DL_DEBUG, "track_init_default_param  g_track_encode_info.refresh_time = %d\n",val);
	//��ʦ���˵ĸ���ģʽ��Ĭ��0Ϊ��ѧ��ȫ����ѧ��������
	val=2;	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, TRACK_STRATEGY, LOST_MODE, temp);
	DEBUG(DL_WARNING, "track_init_default_param  lostmode = %d\n",val);

//����ʦ����λ����־
	val=0;	
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", val);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, HIGH_PRESET, temp);
	DEBUG(DL_WARNING, "track_init_default_param  highpreset = %d\n",val);

	//debug ip	
	memset(temp, 0, FILE_NAME_LEN);	
	sprintf(temp, "%s", "192.168.91.1");
	ret =  ConfigSetKey(config_file, DEBUG_INFO, DEBUG_IPADDR, temp);
	DEBUG(DL_ERROR, "track_init_default_param  debug ipaddr=%s\n",temp);

	unlock_save_track_mutex();
	DEBUG(DL_DEBUG, "track_init_default_param,write file over,reboot system!\n");

	sprintf(cmd_name, "mv /opt/reach/test_track_teacher_point_cam.ini /opt/reach/%s", TRACK_TEACHER_POINT_CAM_FILE);
	system(cmd_name);

	sleep(4);
	system("reboot -f");
	return 0;
	
}


/**
* @	��������: send_track_students_right_side_is_encode()
* @	��������: ���͸��پ��������Ϣ��encodemanage
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_track_students_right_side_is_encode(int socket, short is_encode)
{

	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	track_is_encode_info_t	track_is_encode 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(track_is_encode_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_TRACK_IS_ENCODE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	track_is_encode.state = 1;
	track_is_encode.isencode = is_encode;
	
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &track_is_encode, sizeof(track_is_encode_info_t));

	send(socket, send_buf, len, 0);
	return;

}


/**
* @	��������: send_track_students_right_side_type()
* @	��������: ���͸��ٿ���Ϣ��encodemanage
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_track_students_right_side_type(int socket, ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t *dynamic_param)
{

	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	control_type_info_t	control_type 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	DEBUG(DL_DEBUG,"#############dynamic_param->control_mode = %d#######\n",dynamic_param->control_mode);

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(control_type_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_TRACK_TYPE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	control_type.state = 1;
	control_type.control_type = dynamic_param->control_mode;
	
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &control_type, sizeof(control_type_info_t));

	send(socket, send_buf, len, 0);
	return ;
}


/**
* @	��������: send_track_students_right_side_param()
* @	��������: ���͸��پ��������Ϣ��encodemanage
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: ��
* @ ����ֵ:   ��
*/
static int send_track_students_right_side_param(int socket)
{
	unsigned char send_buf[8192] 	= {0};
	unsigned char temp_buf[8192] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};

	int				len_next		= 0;
	int				len				= 0;
	int				file_len		= 0;

	char cmd_name[256] = {0};
	FILE *fp;
	sprintf(cmd_name, "/opt/reach/%s",TRACK_TEACHER_POINT_CAM_FILE);
	lock_save_track_mutex();
	fp = fopen(cmd_name,"r");

	fread(temp_buf,8192,1,fp);
	
	fseek(fp,0,SEEK_END);
	file_len = ftell(fp);
	
	
	fclose(fp);
	unlock_save_track_mutex();

	DEBUG(DL_DEBUG,"cmd_name = %s\n",cmd_name);
	DEBUG(DL_DEBUG,"file_len = %d\n",file_len);

	memset(send_buf, 0, 8192);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + file_len;

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_TRACK_PARAM;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, temp_buf, file_len);

	send(socket, send_buf, len, 0);
	return;
}



/**
* @	��������: send_track_students_right_side_range()
* @	��������: ���͸��ٿ���Ϣ��encodemanage
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_track_students_right_side_range(int socket, ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t *dynamic_param)
{

	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	track_range_info_t	track_range 	= {0};
	int				len				= 0;
	int				index			= 0;
	int				len_next		= 0;

	memset(send_buf, 0, 256);
	
	 
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(track_range_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));
	
	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_TRACK_RANGE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));


	track_range.point_num = dynamic_param->track_point_num;
	track_range.state = 1;
	
	for(index = 0; index < dynamic_param->track_point_num; index++)
	{
		track_range.point[index].x = dynamic_param->track_point[index].x;
		track_range.point[index].y = dynamic_param->track_point[index].y;
	}

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &track_range, sizeof(track_range_info_t));
	
	send(socket, send_buf, len, 0);
	return ;
}

/**
* @	��������: send_track_students_right_side_sens_value()
* @	��������: ���ͼ��仯ϵ��sensֵ��encodemanage
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_track_students_right_side_sens_value(int socket, ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t *dynamic_param)
{

	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	sens_info_t	sens_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(sens_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type = GET_SENS_VALUE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	sens_info.state = 1;
	sens_info.sens = dynamic_param->sens;

	DEBUG(DL_DEBUG,"sens_info.sens  = %d\n",sens_info.sens);

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &sens_info, sizeof(sens_info_t));

	send(socket, send_buf, len, 0);
	return ;
}


/**
* @	��������: send_track_students_right_side_mode()
* @	��������: ���͸������͸�encodemanage
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_track_students_right_side_mode(int socket, ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t *dynamic_param)
{

	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	track_mode_info_t	track_mode 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	DEBUG(DL_DEBUG,"#############dynamic_param->track_mode = %d#######\n",dynamic_param->track_mode);

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(track_mode_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_TRACK_MODE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	track_mode.state = 1;
	track_mode.track_mode = dynamic_param->track_mode;
	
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &track_mode, sizeof(track_mode_info_t));

	send(socket, send_buf, len, 0);
	return ;
}

/**
* @	��������: send_track_students_right_side_draw_line_type()
* @	��������: ���͸��ٿ���Ϣ��encodemanage
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_track_students_right_side_draw_line_type(int socket, ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t *dynamic_param)
{

	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	draw_line_info_t	draw_line_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(draw_line_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_DRAW_LINE_TYPE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	draw_line_info.state = 1;
	draw_line_info.message = dynamic_param->message;

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &draw_line_info, sizeof(draw_line_info_t));

	send(socket, send_buf, len, 0);
	return;
}




/**
* @	��������: send_track_students_right_side_trigger_num()
* @	��������: ���͸��ٿ���Ϣ��encodemanage
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_track_students_right_side_trigger_num(int socket, ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t *dynamic_param)
{

	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	trigger_num_info_t	trigger_num_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(trigger_num_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - 3;
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_TRIGGER_NUM_TYPE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, 8);

	trigger_num_info.state = 1;
	trigger_num_info.trigger_num = dynamic_param->trigger_sum;
	
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &trigger_num_info, sizeof(trigger_num_info_t));

	send(socket, send_buf, len, 0);
	return;

}

/**
* @	��������: send_track_students_right_side_cam_type()
* @	��������: ���������Э������
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_track_students_right_side_cam_type(int socket, angle_info_t *angle_info)
{

	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	cam_protocol_type_info_t	cam_type_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(cam_protocol_type_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type = GET_CAM_TYPE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	cam_type_info.state = 1;
	cam_type_info.cam_type = angle_info->cam_type;

	DEBUG(DL_DEBUG,"cam_type_info.cam_type  = %d\n",cam_type_info.cam_type);

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &cam_type_info, sizeof(cam_protocol_type_info_t));

	send(socket, send_buf, len, 0);
	return ;
}

/**
* @	��������: send_track_students_right_side_position_cam_info()
* @	��������: ���Ͷ�λ��������ڽ��ҵ�λ����Ϣ
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_track_students_right_side_position_cam_info(int socket, angle_info_t *angle_info)
{

	unsigned char send_buf[256] 			= {0};
	unsigned short msg_len					= 0;
	unsigned char	msg_type				= 0;
	track_header_t		track_header 		= {0};
	position_cam_info_t	position_cam_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(position_cam_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type = GET_POSITION_CAM_INFO;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	position_cam_info.state = 1;
	position_cam_info.lenth = angle_info->position_camera_lenth;
	position_cam_info.width = angle_info->position_camera_width;
	position_cam_info.height = angle_info->position_camera_height;
	

	DEBUG(DL_DEBUG,"position_cam_info.lenth  = %d\n",position_cam_info.lenth);
	DEBUG(DL_DEBUG,"position_cam_info.width  = %d\n",position_cam_info.width);
	DEBUG(DL_DEBUG,"position_cam_info.height  = %d\n",position_cam_info.height);

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &position_cam_info, sizeof(position_cam_info_t));

	send(socket, send_buf, len, 0);
	return ;
}

/**
* @	��������: send_track_students_right_side_track_cam_info()
* @	��������: ���͸�����������ڽ��ҵ�λ����Ϣ
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_track_students_right_side_track_cam_info(int socket, angle_info_t *angle_info)
{

	unsigned char send_buf[256] 			= {0};
	unsigned short msg_len					= 0;
	unsigned char	msg_type				= 0;
	track_header_t		track_header 		= {0};
	track_cam_info_t	track_cam_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(track_cam_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type = GET_TRACK_CAM_INFO;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	track_cam_info.state = 1;
	track_cam_info.lenth = angle_info->track_camera_lenth;
	track_cam_info.width = angle_info->track_camera_width;
	track_cam_info.height = angle_info->track_camera_height;
	

	DEBUG(DL_DEBUG,"position_cam_info.lenth  = %d\n",track_cam_info.lenth);
	DEBUG(DL_DEBUG,"position_cam_info.width  = %d\n",track_cam_info.width);
	DEBUG(DL_DEBUG,"position_cam_info.height  = %d\n",track_cam_info.height);

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &track_cam_info, sizeof(track_cam_info_t));

	send(socket, send_buf, len, 0);
	return ;
}

/**
* @	��������: send_track_students_right_side_track_cam_offset_info()
* @	��������: ���͸���������Ƕ�ƫ��ֵ
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_track_students_right_side_track_cam_offset_info(int socket, angle_info_t *angle_info)
{

	unsigned char send_buf[256] 			= {0};
	unsigned short msg_len					= 0;
	unsigned char	msg_type				= 0;
	track_header_t		track_header 		= {0};
	track_cam_offset_info_t	track_cam_offset_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(track_cam_offset_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type = GET_TRACK_CAM_OFFSET;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	track_cam_offset_info.state = 1;
	track_cam_offset_info.pan = angle_info->camera_pan_offset;
	track_cam_offset_info.tilt = angle_info->camera_tilt_offset;
	track_cam_offset_info.zoom = angle_info->camera_zoom_offset;
	

	DEBUG(DL_DEBUG,"track_cam_offset_info.pan  = %d\n",track_cam_offset_info.pan);
	DEBUG(DL_DEBUG,"track_cam_offset_info.tilt  = %d\n",track_cam_offset_info.tilt);
	DEBUG(DL_DEBUG,"track_cam_offset_info.zoom  = %d\n",track_cam_offset_info.zoom);

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &track_cam_offset_info, sizeof(track_cam_offset_info_t));

	send(socket, send_buf, len, 0);
	return ;
}

static int send_track_students_right_side_ip_info(int socket, track_ip_info_t *ip_info, unsigned short sub_msg_type)
{
	unsigned char send_buf[256] 			= {0};
	unsigned short msg_len					= 0;
	unsigned char	msg_type				= 0;
	track_header_t		track_header 		= {0};
	track_ip_info_t	track_ip_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(track_ip_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type = sub_msg_type;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	track_ip_info.state = ip_info->state;
	memcpy(track_ip_info.ip, ip_info->ip, sizeof(ip_info->ip));
	memcpy(track_ip_info.mask, ip_info->mask, sizeof(ip_info->mask));
	memcpy(track_ip_info.route, ip_info->route, sizeof(ip_info->route));

	DEBUG(DL_DEBUG, "send local_ip_info->ip:%s, mask:%s, route:%s\n",track_ip_info.ip, track_ip_info.mask, track_ip_info.route);
	
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &track_ip_info, sizeof(track_ip_info_t));

	send(socket, send_buf, len, 0);
	return ;
}

/**
* @	��������: send_track_students_right_side_track_other_cam_info()
* @	��������: ���͸���������Ƕ�ƫ��ֵ
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_track_students_right_side_track_other_cam_info(int socket, angle_info_t *angle_info)
{

	unsigned char send_buf[256] 			= {0};
	unsigned short msg_len					= 0;
	unsigned char	msg_type				= 0;
	track_header_t		track_header 		= {0};
	other_position_cam_info_t	other_position_cam_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(other_position_cam_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type = GET_OTHER_CAM_POSITION;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	other_position_cam_info.state = 1;
	other_position_cam_info.x = angle_info->other_cam_x;
	other_position_cam_info.y = angle_info->other_cam_y;
	other_position_cam_info.width = angle_info->other_cam_width;
	other_position_cam_info.height = angle_info->other_cam_height;
	

	DEBUG(DL_DEBUG, "get other_position_cam_info.x  = %d\n",other_position_cam_info.x);
	DEBUG(DL_DEBUG, "get other_position_cam_info.y  = %d\n",other_position_cam_info.y);
	DEBUG(DL_DEBUG, "get other_position_cam_info.width = %d\n",other_position_cam_info.width);
	DEBUG(DL_DEBUG, "get other_position_cam_info.height = %d\n",other_position_cam_info.height);

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &other_position_cam_info, sizeof(other_position_cam_info_t));

	send(socket, send_buf, len, 0);
	return ;
}

/**
* @	��������: send_track_students_right_side_host_type()
* @	��������: ���͸���������Ƕ�ƫ��ֵ
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_track_students_right_side_host_type(int socket, track_encode_info_t *track_info)
{

	unsigned char send_buf[256] 			= {0};
	unsigned short msg_len					= 0;
	unsigned char	msg_type				= 0;
	track_header_t		track_header 		= {0};
	
	track_host_type_t	track_host_type 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(track_host_type_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type = GET_HOST_TYPE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	track_host_type.state = 1;
	track_host_type.host_type = track_info->host_type;
	

	DEBUG(DL_DEBUG,"track_host_type.host_type  = %d\n",track_host_type.host_type);

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &track_host_type, sizeof(track_host_type_t));

	send(socket, send_buf, len, 0);
	return ;
}


/**
* @	��������: send_blackboard_trigger()
* @	��������: ���͸��ٿ���Ϣ��kitetool
* @	�������: socket -- ��kitetool���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_blackboard_trigger(int socket, track_tactics_info_t *track_tactics_info)
{
	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	broadcast_range_info_t	broadcast_range 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(broadcast_range_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_BLACKBOARD_TRIGGER;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	broadcast_range.point_num = 4;
	broadcast_range.state = 1;
	
	broadcast_range.point[0].x = track_tactics_info->broadcast_info[0].point[0].x;
	broadcast_range.point[0].y = track_tactics_info->broadcast_info[0].point[0].y;
	broadcast_range.point[1].x = track_tactics_info->broadcast_info[0].point[1].x;
	broadcast_range.point[1].y = track_tactics_info->broadcast_info[0].point[1].y;
	broadcast_range.point[2].x = track_tactics_info->broadcast_info[1].point[0].x;
	broadcast_range.point[2].y = track_tactics_info->broadcast_info[1].point[0].y;
	broadcast_range.point[3].x = track_tactics_info->broadcast_info[1].point[1].x;
	broadcast_range.point[3].y = track_tactics_info->broadcast_info[1].point[1].y;

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &broadcast_range, sizeof(broadcast_range_info_t));

	send(socket, send_buf, len, 0);
	return 0;
}

/**
* @	��������: send_strategy_no()
* @	��������: ���͸��ٿ���Ϣ��encodemanage
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_strategy_no(int socket,track_strategy_info_t *track_strategy_info)
{

	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	default_msg_info_t	strategy_no_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(default_msg_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - 3;
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_STRATEGY_NO;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, 8);

	strategy_no_info.state = 1;
	strategy_no_info.value= track_strategy_info->strategy_no;
	
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &strategy_no_info, sizeof(default_msg_info_t));

	send(socket, send_buf, len, 0);
	return 0;

}

/**
* @	��������: send_track_turn_info()
* @	��������: ���͸��ٿ���Ϣ��encodemanage
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_track_turn_info(int socket,stu_turn_info_t *stu_turn_info)
{
	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(stu_turn_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - 3;
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_TRACK_TURN_INFO;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, 8);

	
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, stu_turn_info, sizeof(stu_turn_info_t));

	send(socket, send_buf, len, 0);
	return 0;

}
static int send_blackboard_scheme(int socket,track_strategy_info_t *track_strategy_info)
{

	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header	= {0};
	nomal_info_t	blackboard_scheme_info	= {0};

	int 			len_next		= 0;
	int 			len 			= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(nomal_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - 3;
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_BLACKBOARD_SCHEME;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, 8);

	blackboard_scheme_info.state = 1;
	blackboard_scheme_info.val0 = 0;
	blackboard_scheme_info.val1 = 0;
	blackboard_scheme_info.val2 = track_strategy_info->blackboard_scheme;
	blackboard_scheme_info.val3 = 0;
	
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &blackboard_scheme_info, sizeof(nomal_info_t));

	send(socket, send_buf, len, 0);
	return 0;

}



/**
* @	��������: send_track_strategy_time()
* @	��������: ���͸��ٿ���Ϣ��encodemanage
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_track_strategy_time(int socket,unsigned short nTimeMsgType,unsigned short nTimeVal)
{
	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	default_msg_info_t	time_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(default_msg_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - 3;
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= nTimeMsgType;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, 8);

	time_info.state = 1;
	time_info.value= nTimeVal;
	
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &time_info, sizeof(default_msg_info_t));

	send(socket, send_buf, len, 0);
	return 0;
}

/**
* @	��������: send_cam_speed()
* @	��������: ���͸��ٿ���Ϣ��encodemanage
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: cam_control_info -- ����ͷ���Ʋ���
* @ ����ֵ:   ��
*/
static int send_cam_speed(int socket, angle_info_t *angle_info)
{

	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	cam_speed_info_t	cam_speed_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(cam_speed_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_CAM_SPEED_TYPE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	cam_speed_info.state = 1;
	cam_speed_info.cam_speed = angle_info->cam_speed;

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &cam_speed_info, sizeof(cam_speed_info_t));

	send(socket, send_buf, len, 0);
	return 0;
	
}


/**
* @	��������: send_refresh_background()
* @	��������: �����Ƿ�ˢ�±�����Ϣ
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_refresh_background(int socket, track_encode_info_t *track_info)
{

	unsigned char send_buf[256] 			= {0};
	unsigned short msg_len					= 0;
	unsigned char	msg_type				= 0;
	track_header_t		track_header 		= {0};
	
	refresh_background_info_t	refresh_background_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(refresh_background_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type = GET_REFRESH_BACKGROUND;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	refresh_background_info.state = 1;
	if(0 == track_info->refresh_time)
	{
		refresh_background_info.is_open = 0;
	}
	else
	{
		refresh_background_info.is_open = 1;
	}
		
	
	refresh_background_info.refresh_time = track_info->refresh_time;
	

	DEBUG(DL_DEBUG,"refresh_background_info.refresh_time  = %d\n",refresh_background_info.refresh_time);

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &refresh_background_info, sizeof(refresh_background_info_t));

	send(socket, send_buf, len, 0);
	return ;
}


/**
* @	��������: send_trigger_range()
* @	��������: ���͸��ٿ���Ϣ��encodemanage
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_trigger_range(int socket, ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t *dynamic_param)
{
	unsigned char send_buf[256] 	= {0};
	unsigned short msg_len			= 0;
	unsigned char	msg_type		= 0;
	track_header_t		track_header 	= {0};
	trigger_range_info_t	trigger_range 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(trigger_range_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type	= GET_TRIGGER_RANGE;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	trigger_range.point_num = 4;
	trigger_range.state = 1;
	
	trigger_range.point[0].x = dynamic_param->trigger[0].trigger_x0;
	trigger_range.point[0].y = dynamic_param->trigger[0].trigger_y0;
	trigger_range.point[1].x = dynamic_param->trigger[0].trigger_x1;
	trigger_range.point[1].y = dynamic_param->trigger[0].trigger_y1;

	trigger_range.point[2].x = dynamic_param->trigger[1].trigger_x0;
	trigger_range.point[2].y = dynamic_param->trigger[1].trigger_y0;
	trigger_range.point[3].x = dynamic_param->trigger[1].trigger_x1;
	trigger_range.point[3].y = dynamic_param->trigger[1].trigger_y1;
	
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &trigger_range, sizeof(trigger_range_info_t));

	send(socket, send_buf, len, 0);
	return 0;
}


/**
* @	��������: send_high_teacher_flag()
* @	��������: �����Ƿ�֧����ʦ��Ԥ��λ����
* @	�������: socket -- ��encodemanage���ӵ�socket
* @ �������: dynamic_param -- ��ʦ���ٵĶ�̬����
* @ ����ֵ:   ��
*/
static int send_highpreset(int socket, track_encode_info_t *track_info)
{

	unsigned char send_buf[256] 			= {0};
	unsigned short msg_len					= 0;
	unsigned char	msg_type				= 0;
	track_header_t		track_header 		= {0};
	nomal_info_t highpreset_info			= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(nomal_info_t);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type = GET_HIGH_PRESET;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	highpreset_info.state = 1;
	highpreset_info.val0= track_info->highpreset;

	DEBUG(DL_DEBUG,"send highpreset  = %d\n",track_info->highpreset);

	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &highpreset_info, sizeof(nomal_info_t));

	send(socket, send_buf, len, 0);
	return ;
}

static int send_track_students_right_side_network_connect_info(int socket)
{
	unsigned char send_buf[256] 			= {0};
	unsigned short msg_len					= 0;
	unsigned char	msg_type				= 0;
	track_header_t		track_header 		= {0};
	ST_TEACHER_CONNECT_STATE	connect_state_info 	= {0};

	int				len_next		= 0;
	int				len				= 0;

	memset(send_buf, 0, 256);
	len = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t) + sizeof(ST_TEACHER_CONNECT_STATE);

	msg_len = htons(len);
	msg_type = MSG_SET_TRACK_PARAM;

	memcpy(send_buf, &msg_len, sizeof(unsigned short));
	len_next = sizeof(unsigned short);
	memcpy(send_buf + len_next, &msg_type, sizeof(unsigned char));

	track_header.len = len - sizeof(unsigned short) - sizeof(unsigned char);
	track_header.fixd_msg = FIXED_MSG;
	track_header.msg_type = GET_NETWORK_CONNECT_STATUS;

	len_next = sizeof(unsigned short) + sizeof(unsigned char);
	memcpy(send_buf + len_next, &track_header, sizeof(track_header_t));

	memcpy(connect_state_info.cIPStudent, g_track_strategy_info.student_ip_info.ip, sizeof(g_track_strategy_info.student_ip_info.ip));
	memcpy(connect_state_info.cIPBroad1, g_track_strategy_info.blackboard_ip_info[0].ip, sizeof(g_track_strategy_info.student_ip_info.ip));
	memcpy(connect_state_info.cIPBroad2, g_track_strategy_info.blackboard_ip_info[1].ip, sizeof(g_track_strategy_info.student_ip_info.ip));
	
	connect_state_info.iRec = get_teacher_to_encode_connect_state();
	connect_state_info.iStudent = get_teacher_to_student_connect_state(0);
	connect_state_info.iBroad1= get_teacher_to_blackboard_connect_state(0);
	connect_state_info.iBroad2= get_teacher_to_blackboard_connect_state(1);
	
	len_next = sizeof(unsigned short) + sizeof(unsigned char) + sizeof(track_header_t);
	memcpy(send_buf + len_next, &connect_state_info, sizeof(ST_TEACHER_CONNECT_STATE));

	send(socket, send_buf, len, 0);
	return ;
}

static int reboot_device()
{
	DEBUG(DL_DEBUG,"reboot device.......................\n");
	sleep(2);
	system("reboot -f");
	return 0;
}

/**
* @	��������: track_students_right_side_param()
* @	��������: ���ý�ʦ���ٲ���
* @	�������: data -- ��encodemanage���յ������ò�������
* @ �������: ��
* @ ����ֵ:   ��
*/
int track_students_right_side_param(unsigned char *data, int socket)
{
	track_header_t *track_header = (track_header_t *)data;

	DEBUG(DL_DEBUG,"track_header = 0x%x\n",track_header->msg_type);
	switch(track_header->msg_type)
	{

//-----------------------------���ò���----------------------------------------	
		case SET_TRIGGER_RANGE:
			set_trigger_range(data+sizeof(track_header_t), &g_track_param.dynamic_param);
			g_recontrol_flag = 1;
			break;

		case SET_TRACK_IS_ENCODE:
			set_track_students_right_side_is_encode(data+sizeof(track_header_t));
			break;

		case SET_TRACK_TYPE:
			//set_track_students_right_side_type(data+sizeof(track_header_t), &g_track_param.dynamic_param);
			break;
		case SET_TRACK_RANGE:
			set_track_students_right_side_range(data+sizeof(track_header_t), &g_track_param.dynamic_param);
			g_recontrol_flag = 1;
			break;
		case SET_TRACK_PARAM:
			set_track_students_right_side_param(data+sizeof(track_header_t),track_header->len - sizeof(track_header_t));
			break;

		case SET_TRIGGER_NUM_TYPE:
			set_track_students_right_side_trigger_sum(data+sizeof(track_header_t), &g_track_param.dynamic_param);
			g_recontrol_flag = 1;
			break;	

		case SET_SENS_VALUE:
			set_track_students_right_side_sens_value(data+sizeof(track_header_t),&g_track_param.dynamic_param);
			g_recontrol_flag = 1;
			break;		

		case SET_TRACK_MODE:
			set_track_students_right_side_mode(data+sizeof(track_header_t), &g_track_param.dynamic_param);
			g_recontrol_flag = 1;
			break;

		case SET_MANUAL_COMMOND:
			set_track_students_right_side_manual_commond(data+sizeof(track_header_t));
			g_recontrol_flag = 1;
			break;		

		case SET_DRAW_LINE_TYPE:
			set_track_students_right_side_draw_line_type(data+sizeof(track_header_t), &g_track_param.dynamic_param);
			g_recontrol_flag = 1;
			break;

		case SET_PRESET_POSITION_TYPE:
			set_track_students_right_side_preset_position(data+sizeof(track_header_t));
			//g_recontrol_flag = 0;
			break;	

		case CALL_PRESET_POSITION_TYPE:
			call_preset_position(data+sizeof(track_header_t));
			//g_recontrol_flag = 0;
			break;			

		case SET_CAM_TYPE:
			set_track_students_right_side_cam_type(data+sizeof(track_header_t), &g_angle_info);
			//g_recontrol_flag = 0;
			break;

		case SET_POSITION_CAM_INFO:
			set_track_students_right_side_position_cam_info(data+sizeof(track_header_t), &g_angle_info);
			//g_recontrol_flag = 0;
			break;

		case SET_TRACK_CAM_INFO:
			set_track_students_right_side_track_cam_info(data+sizeof(track_header_t), &g_angle_info);
			//g_recontrol_flag = 0;
			break;
		case SET_TRACK_CAM_OFFSET:
			set_track_students_right_side_track_cam_offset_info(data+sizeof(track_header_t), &g_angle_info);
			break;
		case SET_STUDENT_IP_INFO:
			set_track_students_right_side_student_ip_info(data+sizeof(track_header_t), &g_track_strategy_info.student_ip_info);
			break;
		case SET_BLOCKBOARD_0_IP_INFO:
			set_track_students_right_side_blockboard_ip_info(data+sizeof(track_header_t), &g_track_strategy_info.blackboard_ip_info[0], 0);
			break;
		case SET_BLOCKBOARD_1_IP_INFO:
			set_track_students_right_side_blockboard_ip_info(data+sizeof(track_header_t), &g_track_strategy_info.blackboard_ip_info[1], 1);
			break;
		case SET_LOCAL_IP_INFO:
			set_track_students_right_side_local_ip_info(data+sizeof(track_header_t), &g_track_strategy_info.local_ip_info);			
			break;
		case SET_OTHER_CAM_POSITION:
			set_track_students_right_side_track_other_cam_info(data+sizeof(track_header_t), &g_angle_info);
			break;
		case SET_HOST_TYPE:
			set_track_students_right_side_host_type(data+sizeof(track_header_t), &g_track_encode_info);
			break;

		case SET_BLACKBOARD_TRIGGER:
			set_blackboard_trigger(data+sizeof(track_header_t), &g_track_tactics_info);
			break;
		case SET_FEATURE_VIEW_Y_START:
			set_feature_view_start_y(data+sizeof(track_header_t), &g_track_tactics_info);
			break;
		case SET_STRATEGY_NO:
			set_strategy_no(data+sizeof(track_header_t), &g_track_strategy_info);
			break;			
		case SET_TRACK_TURN_INFO:
			set_track_turn_info(data+sizeof(track_header_t),&g_track_strategy_info,&g_stu_turn_info);
			break;	

		case SET_STUDENTS_PANORAMA_SWITCH_NEAR_TIME:
			set_track_strategy_time(data+sizeof(track_header_t),STUDENTS_PANORAMA_SWITCH_NEAR_TIME,&g_track_strategy_info.students_panorama_switch_near_time);
			break;	
		case SET_TEACHER_BLACKBOARD_TIME1:
			set_track_strategy_time(data+sizeof(track_header_t),TEACHER_BLACKBOARD_TIME1,&g_track_strategy_info.teacher_blackboard_time1);
			break;	
		case SET_TEACHER_LEAVE_BLACKBOARD_TIME1:
			set_track_strategy_time(data+sizeof(track_header_t),TEACHER_LEAVE_BLACKBOARD_TIME1,&g_track_strategy_info.teacher_leave_blackboard_time1);
			break;	
		case SET_STUDENTS_DOWN_TIME:
			set_track_strategy_time(data+sizeof(track_header_t),STUDENTS_DOWN_TIME,&g_track_strategy_info.students_down_time);
			break;	
		case SET_TEACHER_PANORAMA_TIME:
			set_track_strategy_time(data+sizeof(track_header_t),TEACHER_PANORAMA_TIME,&g_track_strategy_info.teacher_panorama_time);
			break;	
		case SET_TEACHER_LEAVE_PANORAMA_TIME:
			set_track_strategy_time(data+sizeof(track_header_t),TEACHER_LEAVE_PANORAMA_TIME,&g_track_strategy_info.teacher_leave_panorama_time);
			break;	
		case SET_TEACHER_KEEP_PANORAMA_TIME:
			set_track_strategy_time(data+sizeof(track_header_t),TEACHER_KEEP_PANORAMA_TIME,&g_track_strategy_info.teacher_keep_panorama_time);
			break;	
		case SET_TEACHER_SWITCH_STUDENTS_DELAY_TIME:
			set_track_strategy_time(data+sizeof(track_header_t),TEACHER_SWITCH_STUDENTS_DELAY_TIME,&g_track_strategy_info.teacher_switch_students_delay_time);
			break;	
		case SET_STUDENTS_NEAR_KEEP_TIME:
			set_track_strategy_time(data+sizeof(track_header_t),STUDENTS_NEAR_KEEP_TIME,&g_track_strategy_info.students_near_keep_time);
			break;	
		case SET_MV_KEEP_TIME:
			set_track_strategy_time(data+sizeof(track_header_t),MV_KEEP_TIME,&g_track_strategy_info.mv_keep_time);
			break;	
		case SET_VGA_KEEP_TIME:
			set_track_strategy_time(data+sizeof(track_header_t),VGA_KEEP_TIME,&g_track_strategy_info.vga_keep_time);
			set_track_strategy_time(data+sizeof(track_header_t),VGA_KEEP_TIME_TOSERVER,&g_track_strategy_info.vga_keep_time_toserver);
			//teacher_send_vgakeeptime_to_encode(g_track_strategy_info.vga_keep_time_toserver);
			break;	
		case SET_TEACHER_BLACKBOARD_TIME2:
			set_track_strategy_time(data+sizeof(track_header_t),TEACHER_BLACKBOARD_TIME2,&g_track_strategy_info.teacher_blackboard_time2);
			break;	
		case SET_TEACHER_LEAVE_BLACKBOARD_TIME2:
			set_track_strategy_time(data+sizeof(track_header_t),TEACHER_LEAVE_BLACKBOARD_TIME2,&g_track_strategy_info.teacher_leave_blackboard_time2);
			break;	
			
				
		case SET_CAM_SPEED_TYPE:
			set_camara_speed(data+sizeof(track_header_t), &g_angle_info);
			break;

		case SET_LIMIT_POSITION_TYPE:
			set_or_clear_limit_position(data+sizeof(track_header_t));
			break;

		case SET_REFRESH_BACKGROUND:
			set_refresh_background(data+sizeof(track_header_t), &g_track_encode_info);
			break;

		case SET_DEFAULT_PARAM:
			track_init_default_param();
			break;	
		case SET_HIGH_PRESET:
			set_highpreset(data+sizeof(track_header_t));
			break;
		case SET_LOST_MODE:
			set_lost_mode(data+sizeof(track_header_t), &g_track_strategy_info);
			break;			
		case SET_BLACKBOARD_SCHEME:
			set_blackboard_scheme(data+sizeof(track_header_t), &g_track_strategy_info);
			break;
//-----------------------------�ϴ�����-----------------------------------------
		case GET_TRIGGER_RANGE:
			send_trigger_range(socket,&g_track_param.dynamic_param);
			break;
		case GET_TRACK_IS_ENCODE:
			//Test...
			DEBUG(DL_DEBUG,"GET_TRACK_IS_ENCODE--->g_track_encode_info.is_encode:%d\n", g_track_encode_info.is_encode);
			g_track_encode_info.is_encode = 1;
			//Test...
			send_track_students_right_side_is_encode(socket, g_track_encode_info.is_encode);
			break;
		case GET_TRACK_TYPE:
			send_track_students_right_side_type(socket,&g_track_param.dynamic_param);
			break;

		case GET_TRACK_RANGE:
			send_track_students_right_side_range(socket,&g_track_param.dynamic_param);
			break;	
		case GET_SENS_VALUE:	
			send_track_students_right_side_sens_value(socket,&g_track_param.dynamic_param);
			break;
		case GET_TRACK_MODE:
			send_track_students_right_side_mode(socket,&g_track_param.dynamic_param);
			break;
		case GET_TRIGGER_NUM_TYPE:	
			send_track_students_right_side_trigger_num(socket,&g_track_param.dynamic_param);
			break;		
		case GET_TRACK_PARAM:
			send_track_students_right_side_param(socket);
			break;	
		case GET_DRAW_LINE_TYPE:	
			send_track_students_right_side_draw_line_type(socket,&g_track_param.dynamic_param);
			break;
		case GET_CAM_TYPE:
			send_track_students_right_side_cam_type(socket, &g_angle_info);
			break;
		case GET_POSITION_CAM_INFO:
			send_track_students_right_side_position_cam_info(socket, &g_angle_info);
			break;
		case GET_TRACK_CAM_INFO:
			send_track_students_right_side_track_cam_info(socket, &g_angle_info);
			break;
		case GET_TRACK_CAM_OFFSET:
			send_track_students_right_side_track_cam_offset_info(socket, &g_angle_info);
			break;
		case GET_OTHER_CAM_POSITION:
			send_track_students_right_side_track_other_cam_info(socket, &g_angle_info);
			break;
		case GET_LOCAL_IP_INFO:
			send_track_students_right_side_ip_info(socket, &g_track_strategy_info.local_ip_info, GET_LOCAL_IP_INFO);
			break;
		case GET_STUDENT_IP_INFO:
			send_track_students_right_side_ip_info(socket, &g_track_strategy_info.student_ip_info, GET_STUDENT_IP_INFO);
			break;
		case GET_BLOCKBOARD_0_IP_INFO:
			send_track_students_right_side_ip_info(socket, &g_track_strategy_info.blackboard_ip_info[0], GET_BLOCKBOARD_0_IP_INFO);
			break;
		case GET_BLOCKBOARD_1_IP_INFO:
			send_track_students_right_side_ip_info(socket, &g_track_strategy_info.blackboard_ip_info[1], GET_BLOCKBOARD_1_IP_INFO);
			break;
		case GET_HOST_TYPE:
			send_track_students_right_side_host_type(socket, &g_track_encode_info);
			break;

		case GET_BLACKBOARD_TRIGGER:
			send_blackboard_trigger(socket,&g_track_tactics_info);
			break;	
		case GET_STRATEGY_NO:
			send_strategy_no(socket,&g_track_strategy_info);
			break;
		case GET_TRACK_TURN_INFO:
			send_track_turn_info(socket,&g_stu_turn_info);
			break;	
		case GET_BLACKBOARD_SCHEME:
			send_blackboard_scheme(socket,&g_track_strategy_info);
			break;	
		case GET_STUDENTS_PANORAMA_SWITCH_NEAR_TIME:
			send_track_strategy_time(socket,GET_STUDENTS_PANORAMA_SWITCH_NEAR_TIME,g_track_strategy_info.students_panorama_switch_near_time);
			break;	
		case GET_TEACHER_BLACKBOARD_TIME1:
			send_track_strategy_time(socket,GET_TEACHER_BLACKBOARD_TIME1,g_track_strategy_info.teacher_blackboard_time1);
			break;	
		case GET_TEACHER_LEAVE_BLACKBOARD_TIME1:
			send_track_strategy_time(socket,GET_TEACHER_LEAVE_BLACKBOARD_TIME1,g_track_strategy_info.teacher_leave_blackboard_time1);
			break;	
		case GET_STUDENTS_DOWN_TIME:
			send_track_strategy_time(socket,GET_STUDENTS_DOWN_TIME,g_track_strategy_info.students_down_time);
			break;	
		case GET_TEACHER_PANORAMA_TIME:
			send_track_strategy_time(socket,GET_TEACHER_PANORAMA_TIME,g_track_strategy_info.teacher_panorama_time);
			break;	
		case GET_TEACHER_LEAVE_PANORAMA_TIME:
			send_track_strategy_time(socket,GET_TEACHER_LEAVE_PANORAMA_TIME,g_track_strategy_info.teacher_leave_panorama_time);
			break;	
		case GET_TEACHER_KEEP_PANORAMA_TIME:
			send_track_strategy_time(socket,GET_TEACHER_KEEP_PANORAMA_TIME,g_track_strategy_info.teacher_keep_panorama_time);
			break;	
		case GET_TEACHER_SWITCH_STUDENTS_DELAY_TIME:
			send_track_strategy_time(socket,GET_TEACHER_SWITCH_STUDENTS_DELAY_TIME,g_track_strategy_info.teacher_switch_students_delay_time);
			break;	
		case GET_STUDENTS_NEAR_KEEP_TIME:
			send_track_strategy_time(socket,GET_STUDENTS_NEAR_KEEP_TIME,g_track_strategy_info.students_near_keep_time);
			break;	
		case GET_MV_KEEP_TIME:
			send_track_strategy_time(socket,GET_MV_KEEP_TIME,g_track_strategy_info.mv_keep_time);
			break;	
		case GET_VGA_KEEP_TIME:
			send_track_strategy_time(socket,GET_VGA_KEEP_TIME,g_track_strategy_info.vga_keep_time);
			break;	
		case GET_TEACHER_BLACKBOARD_TIME2:
			send_track_strategy_time(socket,GET_TEACHER_BLACKBOARD_TIME2,g_track_strategy_info.teacher_blackboard_time2);
			break;	
		case GET_TEACHER_LEAVE_BLACKBOARD_TIME2:
			send_track_strategy_time(socket,GET_TEACHER_LEAVE_BLACKBOARD_TIME2,g_track_strategy_info.teacher_leave_blackboard_time2);
			break;	
			
		case GET_CAM_SPEED_TYPE:	
			send_cam_speed(socket,&g_angle_info);
			break;	
		case GET_REFRESH_BACKGROUND:
			send_refresh_background(socket, &g_track_encode_info);
			break;
		case GET_HIGH_PRESET:
			send_highpreset(socket, &g_track_encode_info);
			break;
		case GET_NETWORK_CONNECT_STATUS:
			send_track_students_right_side_network_connect_info(socket);
			break;
		case REBOOT_DEVICE:
			reboot_device();
			break;
		default:
			break;
	}
	return 0;
}


int server_set_track_type(short type)
{	
	g_track_param.dynamic_param.control_mode = type;
	return 0;
}


int server_set_classroom_study_mode(short mode)
{	
	g_track_param.dynamic_param.classroom_study_mode = mode;

	char 	temp[FILE_NAME_LEN]			= {0};
	char 	config_file[FILE_NAME_LEN] 	= {0};
	int 	ret 						= -1;
	
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);
	
	lock_save_track_mutex();
	memset(temp, 0, FILE_NAME_LEN);
	sprintf(temp, "%d", g_track_param.dynamic_param.classroom_study_mode);
	ret =  ConfigSetKey(config_file, DYNAMIC_NAME, CLASSROOM_STUDY_MODE, temp);
	unlock_save_track_mutex();
	
	return ret;
}

int server_set_student_enable_state(short state)
{	
	g_track_param.dynamic_param.student_enable_state = state;
	return 0;
}

/**
* @	��������:init_angle_param
* @ 	��������:��ͼ�������ص��Ӧ������ĽǶ�
* @	
* @����:�����������ɵ��ǽ��110���׵ľ�����в������������к���tan=�Ա߳����ٱ�
* @			����arctan��������봹�ߵļн�
*/
int init_angle_param(model_classroom_info_t *param, int param_len)
{
	double	a = 0.0;
	double	b = 0.0;
	double	c = 0.0;
#if 0
//�������С�ǶȵĶ�λ�����
#if 1
	c = 233;

	b = 195;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[0] = a;		//���봹�ĵ�ľ���159����


	b = 184;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[1] = a;		//���봹�ĵ�ľ���137����


	b = 167;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[2] = a;		//���봹�ĵ�ľ���121����

	b = 150;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[3] = a;		//���봹�ĵ�ľ���108����

	b = 135;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[4] = a;		//���봹�ĵ�ľ���93����

	b = 123;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[5] = a;		//���봹�ĵ�ľ���82����

	b = 108;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[6] = a;		//���봹�ĵ�ľ���73����

	b = 98;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[7] = a;		//���봹�ĵ�ľ���64����

	b = 88;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[8] = a;		//���봹�ĵ�ľ���56����

	b = 78;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[9] = a;		//���봹�ĵ�ľ���49����

	b = 67;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[10] = a;		//���봹�ĵ�ľ���41����

	b = 57;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[11] = a;		//���봹�ĵ�ľ���33����

	b = 48;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[12] = a;		//���봹�ĵ�ľ���26����

	b = 37;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[13] = a;		//���봹�ĵ�ľ���19����

	b = 28;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[14] = a;		//���봹�ĵ�ľ���13����

	b = 19;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[15] = a;		//���봹�ĵ�ľ���41����

	b = 9;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[16] = a;		//���봹�ĵ�ľ���0����

	b = 0;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[17] = a;		//���봹�ĵ�ľ���-7����

	b = -8;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[18] = a;		//���봹�ĵ�ľ���-13����

	b = -18;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[19] = a;		//���봹�ĵ�ľ���-18����

	b = -28;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[20] = a;		//���봹�ĵ�ľ���-25����

	b = -38;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[21] = a;		//���봹�ĵ�ľ���-31����

	b = -48;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[22] = a;		//���봹�ĵ�ľ���-38����

	b = -58;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[23] = a;		//���봹�ĵ�ľ���-45����


	b = -69;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[24] = a;		//���봹�ĵ�ľ���-55����

	b = -80;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[25] = a;		//���봹�ĵ�ľ���-63����

	b = -91;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[26] = a;		//���봹�ĵ�ľ���-72����

	b = -103;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[27] = a;		//���봹�ĵ�ľ���-80����

	b = -115;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[28] = a;		//���봹�ĵ�ľ���-90����

	b = -128;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[29] = a;		//���봹�ĵ�ľ���-103����

	b = -138;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[30] = a;		//���봹�ĵ�ľ���-115����

	b = -153;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[31] = a;		//���봹�ĵ�ľ���-132����

	b = -170;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[32] = a;		//���봹�ĵ�ľ���-145����

	b = -185;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[33] = a;		//���봹�ĵ�ľ���-163����

	b = -205;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[34] = a;		//���봹�ĵ�ľ���-189����

	b = -226;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[35] = a;		//���봹�ĵ�ľ���-220����

#else	//��άѡ�Ķ�λ�����
	c = 110;

	b = 188;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[0] = a;		//���봹�ĵ�ľ���159����


	b = 159;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[1] = a;		//���봹�ĵ�ľ���159����


	b = 137;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[2] = a;		//���봹�ĵ�ľ���137����


	b = 121;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[3] = a;		//���봹�ĵ�ľ���121����

	b = 108;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[4] = a;		//���봹�ĵ�ľ���108����

	b = 93;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[5] = a;		//���봹�ĵ�ľ���93����

	b = 82;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[6] = a;		//���봹�ĵ�ľ���82����

	b = 73;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[7] = a;		//���봹�ĵ�ľ���73����

	b = 64;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[8] = a;		//���봹�ĵ�ľ���64����

	b = 56;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[9] = a;		//���봹�ĵ�ľ���56����

	b = 49;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[10] = a;		//���봹�ĵ�ľ���49����

	b = 41;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[11] = a;		//���봹�ĵ�ľ���41����

	b = 33;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[12] = a;		//���봹�ĵ�ľ���33����

	b = 26;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[13] = a;		//���봹�ĵ�ľ���26����

	b = 19;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[14] = a;		//���봹�ĵ�ľ���19����

	b = 13;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[15] = a;		//���봹�ĵ�ľ���13����

	b = 5;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[16] = a;		//���봹�ĵ�ľ���41����

	b = 0;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[17] = a;		//���봹�ĵ�ľ���0����

	b = -7;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[18] = a;		//���봹�ĵ�ľ���-7����

	b = -13;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[19] = a;		//���봹�ĵ�ľ���-13����

	b = -18;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[20] = a;		//���봹�ĵ�ľ���-18����

	b = -25;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[21] = a;		//���봹�ĵ�ľ���-25����

	b = -31;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[22] = a;		//���봹�ĵ�ľ���-31����

	b = -38;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[23] = a;		//���봹�ĵ�ľ���-38����

	b = -45;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[24] = a;		//���봹�ĵ�ľ���-45����


	b = -55;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[25] = a;		//���봹�ĵ�ľ���-55����

	b = -63;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[26] = a;		//���봹�ĵ�ľ���-63����

	b = -72;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[27] = a;		//���봹�ĵ�ľ���-72����

	b = -80;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[28] = a;		//���봹�ĵ�ľ���-80����

	b = -90;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[29] = a;		//���봹�ĵ�ľ���-90����

	b = -103;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[30] = a;		//���봹�ĵ�ľ���-103����

	b = -115;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[31] = a;		//���봹�ĵ�ľ���-115����

	b = -132;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[32] = a;		//���봹�ĵ�ľ���-132����

	b = -145;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[33] = a;		//���봹�ĵ�ľ���-145����

	b = -163;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[34] = a;		//���봹�ĵ�ľ���-163����

	b = -189;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[35] = a;		//���봹�ĵ�ľ���-189����

/*
	c = 110;
	
	b = 159;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[0] = a;		//���봹�ĵ�ľ���159����

	
	b = 137;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[1] = a;		//���봹�ĵ�ľ���137����

	
	b = 121;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[2] = a;		//���봹�ĵ�ľ���121����

	b = 108;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[3] = a;		//���봹�ĵ�ľ���108����

	b = 93;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[4] = a;		//���봹�ĵ�ľ���93����

	b = 82;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[5] = a;		//���봹�ĵ�ľ���82����

	b = 73;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[6] = a;		//���봹�ĵ�ľ���73����

	b = 64;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[7] = a;		//���봹�ĵ�ľ���64����

	b = 56;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[8] = a;		//���봹�ĵ�ľ���56����

	b = 49;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[9] = a;		//���봹�ĵ�ľ���49����

	b = 41;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[10] = a;		//���봹�ĵ�ľ���41����

	b = 33;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[11] = a;		//���봹�ĵ�ľ���33����

	b = 26;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[12] = a;		//���봹�ĵ�ľ���26����

	b = 19;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[13] = a;		//���봹�ĵ�ľ���19����

	b = 13;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[14] = a;		//���봹�ĵ�ľ���13����

	b = 5;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[15] = a;		//���봹�ĵ�ľ���41����

	b = 0;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[16] = a;		//���봹�ĵ�ľ���0����

	b = -7;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[17] = a;		//���봹�ĵ�ľ���-7����

	b = -13;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[18] = a;		//���봹�ĵ�ľ���-13����

	b = -18;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[19] = a;		//���봹�ĵ�ľ���-18����

	b = -25;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[20] = a;		//���봹�ĵ�ľ���-25����

	b = -31;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[21] = a;		//���봹�ĵ�ľ���-31����

	b = -38;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[22] = a;		//���봹�ĵ�ľ���-38����

	b = -45;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[23] = a;		//���봹�ĵ�ľ���-45����

	
	b = -55;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[24] = a;		//���봹�ĵ�ľ���-55����

	b = -63;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[25] = a;		//���봹�ĵ�ľ���-63����

	b = -72;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[26] = a;		//���봹�ĵ�ľ���-72����

	b = -80;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[27] = a;		//���봹�ĵ�ľ���-80����

	b = -90;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[28] = a;		//���봹�ĵ�ľ���-90����

	b = -103;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[29] = a;		//���봹�ĵ�ľ���-103����

	b = -115;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[30] = a;		//���봹�ĵ�ľ���-115����

	b = -132;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[31] = a;		//���봹�ĵ�ľ���-132����

	b = -145;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[32] = a;		//���봹�ĵ�ľ���-145����

	b = -163;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[33] = a;		//���봹�ĵ�ľ���-163����

	b = -189;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[34] = a;		//���봹�ĵ�ľ���-189����

	b = -220;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[35] = a;		//���봹�ĵ�ľ���-220����
			*/
#endif
#endif
#if 1
	//sd200��λ�����
	c = 233;
	 
	b = 275;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[0] = a; //���봹�ĵ�ľ���159����
	 
	 
	b = 249;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[1] = a; //���봹�ĵ�ľ���137����
	 
	 
	b = 221;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[2] = a; //���봹�ĵ�ľ���121����
	 
	b = 198;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[3] = a; //���봹�ĵ�ľ���108����
	 
	b = 178;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[4] = a; //���봹�ĵ�ľ���93����
	 
	b = 160;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[5] = a; //���봹�ĵ�ľ���82����
	 
	b = 143;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[6] = a; //���봹�ĵ�ľ���73����
	 
	b = 128;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[7] = a; //���봹�ĵ�ľ���64����
	 
	b = 111;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[8] = a; //���봹�ĵ�ľ���56����
	 
	b = 97;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[9] = a; //���봹�ĵ�ľ���49����
	 
	b = 84;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[10] = a; //���봹�ĵ�ľ���41����
	 
	b = 70;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[11] = a; //���봹�ĵ�ľ���33����
	 
	b = 58;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[12] = a; //���봹�ĵ�ľ���26����
	 
	b = 46;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[13] = a; //���봹�ĵ�ľ���19����
	 
	b = 34;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[14] = a; //���봹�ĵ�ľ���13����
	 
	b = 22;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[15] = a; //���봹�ĵ�ľ���41����
	 
	b = 10;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[16] = a; //���봹�ĵ�ľ���0����
	 
	b = 0;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[17] = a; //���봹�ĵ�ľ���-7����
	 
	b = -13;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[18] = a; //���봹�ĵ�ľ���-13����
	 
	b = -25;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[19] = a; //���봹�ĵ�ľ���-18����
	 
	b = -37;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[20] = a; //���봹�ĵ�ľ���-25����
	 
	b = -48;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[21] = a; //���봹�ĵ�ľ���-31����
	 
	b = -60;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[22] = a; //���봹�ĵ�ľ���-38����
	 
	b = -73;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[23] = a; //���봹�ĵ�ľ���-45����
	 
	 
	b = -87;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[24] = a; //���봹�ĵ�ľ���-55����
	 
	b = -101;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[25] = a; //���봹�ĵ�ľ���-63����
	 
	b = -116;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[26] = a; //���봹�ĵ�ľ���-72����
	 
	b = -132;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[27] = a; //���봹�ĵ�ľ���-80����
	 
	b = -150;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[28] = a; //���봹�ĵ�ľ���-90����
	 
	b = -168;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[29] = a; //���봹�ĵ�ľ���-103����
	 
	b = -186;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[30] = a; //���봹�ĵ�ľ���-115����
	 
	b = -210;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[31] = a; //���봹�ĵ�ľ���-132����
	 
	b = -231;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[32] = a; //���봹�ĵ�ľ���-145����
	 
	b = -259;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[33] = a; //���봹�ĵ�ľ���-163����
	 
	b = -286;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[34] = a; //���봹�ĵ�ľ���-189����
	 
	b = -327;
	a = atan(b/c);
	a = a *180/3.141592653;
	param->angle[35] = a; //���봹�ĵ�ľ���-220����
#endif
}


static int set_blackbord_flag(track_strategy_info_t *g_track_strategy_info, int x, track_tactics_info_t *track_tactics_info)
{

	if(((x > track_tactics_info->broadcast_info[0].point[0].x) && (x < track_tactics_info->broadcast_info[0].point[1].x))
		|| ((x > track_tactics_info->broadcast_info[0].point[1].x) && (x < track_tactics_info->broadcast_info[0].point[0].x)))
	{
		g_track_strategy_info->blackboard_region_flag1 = 1;
	}
	else
	{
		g_track_strategy_info->blackboard_region_flag1 = 0;
	}

	if(((x > track_tactics_info->broadcast_info[1].point[0].x) && (x < track_tactics_info->broadcast_info[1].point[1].x))
		|| ((x > track_tactics_info->broadcast_info[1].point[1].x) && (x < track_tactics_info->broadcast_info[1].point[0].x)))
	{
		g_track_strategy_info->blackboard_region_flag2 = 1;
	}
	else
	{
		g_track_strategy_info->blackboard_region_flag2 = 0;
	}
}


int init_dev_type(void)
{
	int 		ret 					= 0;

	char	config_file[FILE_NAME_LEN]	= {0};
	char	temp[FILE_NAME_LEN] 		= {0};



	//ѧ�����������ã�0Ϊ��ѧ������1Ϊ��ѧ����
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, SYSTEM_PARAM, HOST_TYPE, temp);
	g_track_encode_info.host_type = atoi(temp);;
	DEBUG(DL_DEBUG, "g_track_encode_info.host_type = %d\n",g_track_encode_info.host_type);

	if(ret != 0)
	{
		g_track_encode_info.host_type = 0;
	}
}

target_change_time_info_t g_target_change_time_info = {0};

int g_last_x = -1;
#define MAX_X_PIX	(200)
int get_range_min_x(void)
{
	int index = 0;
	int x=65535;
	for(index = 0; index < g_track_param.dynamic_param.track_point_num;index++)
	{
		if(x >g_track_param.dynamic_param.track_point[index].x)
		{
			x = g_track_param.dynamic_param.track_point[index].x;
		}
	}
	return x;
}

int get_range_max_x(void)
{
	int index = 0;
	int x=0;
	for(index = 0; index < g_track_param.dynamic_param.track_point_num;index++)
	{
		if(x <g_track_param.dynamic_param.track_point[index].x)
		{
			x = g_track_param.dynamic_param.track_point[index].x;
		}
	}
	return x;
}


int g_trigger_flag = 0;
int teacher_target_validaation(int target_num, int x,int is_trigger, rect_t *rect)
{
	int cam_pan = 0,cam_pan0 = 0, cam_pan1 = 0,pan_abs = 0;
	int test = 0;
	int x0 = 0,x1 =0;
	x0 = get_range_min_x();
	x1 = get_range_max_x();
	cam_preset_position_info_t cam_info = {0};
	static rect_t rect_old = {1280,960,2879,1619};
	int x_4k = 0;
 	x_4k = 3 * x;
	static int iRealTargetNum = 0;
	/*
	g_cam_info.cam_position[0].pan_tilt[0] = 0x0f;
	g_cam_info.cam_position[0].pan_tilt[1] = 0x0e;
	g_cam_info.cam_position[0].pan_tilt[2] = 0x0b;
	g_cam_info.cam_position[0].pan_tilt[3] = 0x0f;

	g_cam_info.cam_position[0].pan_tilt[4] = 0x0f;
	g_cam_info.cam_position[0].pan_tilt[5] = 0x0f;
	g_cam_info.cam_position[0].pan_tilt[6] = 0x05;
	g_cam_info.cam_position[0].pan_tilt[7] = 0x0a;

	g_cam_info.cam_position[1].pan_tilt[0] = 0x0;
	g_cam_info.cam_position[1].pan_tilt[1] = 0x0;
	g_cam_info.cam_position[1].pan_tilt[2] = 0x0d;
	g_cam_info.cam_position[1].pan_tilt[3] = 0x01;

	g_cam_info.cam_position[1].pan_tilt[4] = 0x0f;
	g_cam_info.cam_position[1].pan_tilt[5] = 0x0f;
	g_cam_info.cam_position[1].pan_tilt[6] = 0x05;
	g_cam_info.cam_position[1].pan_tilt[7] = 0x0a;
	//printf("target_num = %d,x= %d,x0= %d,x1 = %d\n",target_num,x,x0,x1);
	*/
	if(is_trigger) 	
	{		
		target_num = 0;		
		g_trigger_flag++;		
	}
	//printf("--------------------------is_trigger:%d, g_trigger_flag:%d, target_num:%d----------------\n",is_trigger, g_trigger_flag, target_num);
	if(0 == target_num)
	{
		g_target_change_time_info.targettime0++;
		//printf("g_target_change_time_info.targettime0:%d\n", g_target_change_time_info.targettime0);
		//if(((g_trigger_flag >= 2) && (g_target_change_time_info.targettime0 >= 4))
		//	|| (g_track_param.dynamic_param.reset_time == g_target_change_time_info.targettime0))
		if( (g_trigger_flag >= 2)
			|| (g_track_param.dynamic_param.reset_time == g_target_change_time_info.targettime0))
		{
			g_trigger_flag = 0;
			g_last_x = -1;
			g_target_change_time_info.targettime1 = 0;
			g_target_change_time_info.targettime2 = 0;
			SetTeacherDetectResult(0,cam_info, false);
			iRealTargetNum = 0;

			//rect_old.min_x = 1280;
		    //rect_old.min_y = 720;
		    //rect_old.max_x = 2559;
		    //rect_old.max_y = 1439;
		}
	}
	else if(1 == target_num)
	{
		g_target_change_time_info.targettime1++;
		
		if(g_target_change_time_info.targettime1 > 5) 
		{
			g_target_change_time_info.targettime0 = 0;
			g_target_change_time_info.targettime2 = 0;
			g_trigger_flag  = 0;
			SetTeacherDetectResult(1,cam_info, false);
			if((-1 == g_last_x) || (abs(x - g_last_x) > g_angle_info.camera_pan_offset))
			{
				g_last_x = x;
				if((x_4k -(1920/2)) > 0)
			    {
			     rect_old.min_x = x_4k -(1920/2);
			    }
			    else
			    {
			     rect_old.min_x = 0;
			    }

			    if((rect_old.min_x + 1920) > 3839)
			    {
			     rect_old.max_x = 3839;
				 rect_old.min_x = 1920;
			    }
			    else
			    {
			     rect_old.max_x = rect_old.min_x +1920;
			    }
			    
			    rect_old.min_y = g_track_tactics_info.teacher_feature_info[0].point[0].y;//540;
			    if(rect_old.min_y > 1080)
			    {
					rect_old.min_y = 1080;
				}
			    rect_old.max_y = rect_old.min_y + 1080 - 1;//1619;

				SetTeacherDetectResult(1,cam_info, true);
				iRealTargetNum = 1;
			}
	
		}
	
	}
	else if(2 == target_num)
	{
		g_target_change_time_info.targettime2++;
		
		if(10 == g_target_change_time_info.targettime2);
		{
			g_trigger_flag  = 0;
			g_target_change_time_info.targettime0 = 0;
			g_target_change_time_info.targettime1 = 0;
			g_last_x = -1;
			SetTeacherDetectResult(2,cam_info, false);
			iRealTargetNum = 2;
			//rect_old.min_x = 1280;
			//rect_old.min_y = 720;
			//rect_old.max_x = 2559;
			//rect_old.max_y = 1439;
		}
		if (10 < g_target_change_time_info.targettime2)
		{
			SetTeacherDetectResult(2,cam_info, false);
		}
	
	}

	rect->max_x = rect_old.max_x;
	rect->max_y = rect_old.max_y;
	rect->min_x = rect_old.min_x;
	rect->min_y = rect_old.min_y;
	return iRealTargetNum;
}

/**
* @	�������ļ��ж�ȡ���������Ϣ
*/
int get_strategy_info_form_file(void)
{
	int 	ret = -1;
	int 	stu_num=0;
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char 	temp[FILE_NAME_LEN]			= {0};
	char	text[FILE_NAME_LEN]			= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int		index						= 0;
	int		i 							= 0;
	unsigned char data[5]				= {0};
	int track_switch_type=0;

	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	//ѧ��ȫ����ͷ�л���ѧ����д��ͷ����ʱʱ��
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, STUDENTS_PANORAMA_SWITCH_NEAR_TIME);
	sprintf(param_name, "%s",text);
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.students_panorama_switch_near_time = atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.students_panorama_switch_near_time = %d\n"
			, g_track_strategy_info.students_panorama_switch_near_time);
	}
	else
	{
		g_track_strategy_info.students_panorama_switch_near_time = 3;
	}

	//����ʦ���پ�ͷ�л������龵ͷ1ʱ����������ʦ�߽�������������л�������
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, TEACHER_BLACKBOARD_TIME1);
	sprintf(param_name, "%s",text);
	
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.teacher_blackboard_time1 = atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.teacher_blackboard_time1 = %d\n"
			, g_track_strategy_info.teacher_blackboard_time1);
	}
	else
	{
		g_track_strategy_info.teacher_blackboard_time1 = 3;
	}

	//�ɰ��龵ͷ�л�����ʦ���پ�ͷ1ʱ����������ʦ�Ӱ��������ߵ������ط��л�����ʦ���پ�ͷ��ʱ��
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, TEACHER_LEAVE_BLACKBOARD_TIME1);
	sprintf(param_name, "%s",text);
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.teacher_leave_blackboard_time1 = atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.teacher_leave_blackboard_time1 = %d\n"
			, g_track_strategy_info.teacher_leave_blackboard_time1);
	}
	else
	{
		g_track_strategy_info.teacher_leave_blackboard_time1 = 3;
	}

	//����ʦ���پ�ͷ�л������龵ͷ2ʱ����������ʦ�߽�������������л�������
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, TEACHER_BLACKBOARD_TIME2);
	sprintf(param_name, "%s",text);
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.teacher_blackboard_time2 = atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.teacher_blackboard_time2 = %d\n"
			, g_track_strategy_info.teacher_blackboard_time2);
	}
	else
	{
		g_track_strategy_info.teacher_blackboard_time2 = 5;
	}

	//�ɰ��龵ͷ�л�����ʦ���پ�ͷ2ʱ����������ʦ�Ӱ��������ߵ������ط��л�����ʦ���پ�ͷ��ʱ��
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, TEACHER_LEAVE_BLACKBOARD_TIME2);
	sprintf(param_name, "%s",text);
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.teacher_leave_blackboard_time2 = atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.teacher_leave_blackboard_time2 = %d\n"
			, g_track_strategy_info.teacher_leave_blackboard_time2);
	}
	else
	{
		g_track_strategy_info.teacher_leave_blackboard_time2 = 3;
	}

	//ѧ���½�̨ʱ��������ʱ�伴�ٴν���ѧ��������Ϣ����λ����
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, STUDENTS_DOWN_TIME);
	sprintf(param_name, "%s",text);
	
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.students_down_time= atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.students_down_time = %d\n"
			, g_track_strategy_info.students_down_time);
	}
	else
	{
		g_track_strategy_info.students_down_time = 3;
	}

	
	
	//�л���ʦȫ��Ҫ����ʱ�����л���ʦȫ��ʱ��ʱһ��ʱ�����л���ʦȫ��
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, TEACHER_PANORAMA_TIME);
	sprintf(param_name, "%s",text);
	
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.teacher_panorama_time = atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.teacher_panorama_time = %d\n"
					, g_track_strategy_info.teacher_panorama_time);
	}
	else
	{
		g_track_strategy_info.teacher_panorama_time = 0;
	}

	//�л���ʦȫ��Ҫ����ʱ�����л���ʦȫ��ʱ��ʱһ��ʱ�����л���ʦȫ��
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, TEACHER_LEAVE_PANORAMA_TIME);
	sprintf(param_name, "%s",text);
	
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.teacher_leave_panorama_time = atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.teacher_leave_panorama_time = %d\n"
						, g_track_strategy_info.teacher_leave_panorama_time);
	}
	else
	{
		g_track_strategy_info.teacher_leave_panorama_time = 3;
	}

	//��ʦȫ�������ʱ��
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, TEACHER_KEEP_PANORAMA_TIME);
	sprintf(param_name, "%s",text);
	
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.teacher_keep_panorama_time = atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.teacher_keep_panorama_time = %d\n"
						, g_track_strategy_info.teacher_keep_panorama_time);
	}
	else
	{
		g_track_strategy_info.teacher_keep_panorama_time = 5;
	}

	//��ʦ��������ĳ�ʱʱ��
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, TEACHER_DETECT_HEART_TIMEOUT);
	sprintf(param_name, "%s",text);
	
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.teacher_detect_heart_timeout = atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.teacher_detect_heart_timeout = %d\n"
						, g_track_strategy_info.teacher_detect_heart_timeout);
	}
	else
	{
		g_track_strategy_info.teacher_detect_heart_timeout = 5;
	}

	//ѧ������ʱ������ʦ�л���ѧ����ʱ
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, TEACHER_SWITCH_STUDENTS_DELAY_TIME);
	sprintf(param_name, "%s",text);
	
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.teacher_switch_students_delay_time= atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.teacher_switch_students_delay_time = %d\n"
						, g_track_strategy_info.teacher_switch_students_delay_time);
	}
	else
	{
		g_track_strategy_info.teacher_switch_students_delay_time= 3;
	}


	//ѧ��������ͷ���ֵ����ʱ��
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, STUDENTS_NEAR_KEEP_TIME);
	sprintf(param_name, "%s",text);
	
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.students_near_keep_time= atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.students_near_keep_time = %d\n"
						, g_track_strategy_info.students_near_keep_time);
	}
	else
	{
		g_track_strategy_info.students_near_keep_time= 3;
	}

	//ѧ��������ͷ���ֵ����ʱ��
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, VGA_KEEP_TIME);
	sprintf(param_name, "%s",text);
	
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.vga_keep_time= atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.vga_keep_time = %d\n"
						, g_track_strategy_info.vga_keep_time);
	}
	else
	{
		g_track_strategy_info.vga_keep_time= 3;
	}

	g_track_strategy_info.vga_keep_time_toserver=g_track_strategy_info.vga_keep_time;

	//��ʦ���ˣ������ƶ�Ŀ��ʱѧ��ȫ������ʦȫ����ͷ�л���ʱ
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, MV_KEEP_TIME);
	sprintf(param_name, "%s",text);
	
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.mv_keep_time= atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.mv_keep_time = %d\n"
						, g_track_strategy_info.mv_keep_time);
	}
	else
	{
		g_track_strategy_info.mv_keep_time= 3;
	}

	//��ʦ���ˣ�ѧ��ȫ������ʱ��
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, LOST_MODE_STUTIME);
	sprintf(param_name, "%s",text);
	
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.lostmodestutime= atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.lostmodestutime = %d\n"
						, g_track_strategy_info.lostmodestutime);
	}
	else
	{
		g_track_strategy_info.lostmodestutime= 3;
	}


	//��û�λ��Ϣ
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, STRATEGY_NO);
	sprintf(param_name, "%s",text);
	
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.strategy_no= atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.strategy_no = %d\n"
						, g_track_strategy_info.strategy_no);
	}
	else
	{
		g_track_strategy_info.strategy_no= STRATEGY_6;
	}
	memset(&g_track_strategy_timeinfo,0,sizeof(track_strategy_timeinfo_t));
	g_track_strategy_timeinfo.last_strategy_no=g_track_strategy_info.strategy_no;
#if 0
	//���ø�������,ֻ�в���Ϊ1,����Ϊ0,Ĭ��Ϊ0
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, TRACK_SWITCH_TYPE, temp);
	if(ret != 0)
	{
		g_track_encode_info.nTrackSwitchType= 0;
	}
	else
	{
		g_track_encode_info.nTrackSwitchType= atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_encode_info.nTrackSwitchType = %d\n",g_track_encode_info.nTrackSwitchType);
	}
#endif	
	//���ѧ��������
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, STU_NUM);
	sprintf(param_name, "%s",text);
	
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.stu_num= atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.stu_num = %d\n"
						, g_track_strategy_info.stu_num);
	}
	else
	{
		g_track_strategy_info.stu_num= 1;
	}

	//ѧ������IPֵ��Ŀǰѧ�����ĸ����̶�Ϊ1������ѧ������ip��Ϣû��ʹ������
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, NETWORK_INFO, STUDENT_IP, temp);
	g_track_strategy_info.student_ip_info.state = 1;
	memcpy(g_track_strategy_info.student_ip_info.ip, temp, sizeof(g_track_strategy_info.student_ip_info.ip) );
	DEBUG(DL_DEBUG, "g_track_strategy_info.student_ip_info.ip = %s\n",g_track_strategy_info.student_ip_info.ip);
	if(ret != 0)
	{
		char student_ip[16]="172.25.100.102";
		memcpy(g_track_strategy_info.student_ip_info.ip, student_ip, sizeof(g_track_strategy_info.student_ip_info.ip) );
	}

	//������IPֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, NETWORK_INFO, LOCAL_IP, temp);
	g_track_strategy_info.local_ip_info.state = 1;
	memcpy(g_track_strategy_info.local_ip_info.ip, temp, sizeof(g_track_strategy_info.local_ip_info.ip) );
	DEBUG(DL_DEBUG, "g_track_strategy_info.local_ip_info.ip = %s\n",g_track_strategy_info.local_ip_info.ip);
	if(ret != 0)
	{
		char local_ip[16]="172.25.100.101";
		memcpy(g_track_strategy_info.local_ip_info.ip, local_ip, sizeof(g_track_strategy_info.local_ip_info.ip) );
	}

	//������MASKֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, NETWORK_INFO, LOCAL_MASK, temp);
	memcpy(g_track_strategy_info.local_ip_info.mask, temp, sizeof(g_track_strategy_info.local_ip_info.mask) );
	DEBUG(DL_DEBUG, "g_track_strategy_info.local_ip_info.mask = %s\n",g_track_strategy_info.local_ip_info.mask);
	if(ret != 0)
	{
		char local_mask[16]="255.255.255.0";
		memcpy(g_track_strategy_info.local_ip_info.mask, local_mask, sizeof(g_track_strategy_info.local_ip_info.mask) );
	}

	//������routeֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, NETWORK_INFO, LOCAL_GW, temp);
	memcpy(g_track_strategy_info.local_ip_info.route, temp, sizeof(g_track_strategy_info.local_ip_info.route) );
	DEBUG(DL_DEBUG, "g_track_strategy_info.local_ip_info.route = %s\n",g_track_strategy_info.local_ip_info.route);
	if(ret != 0)
	{
		char local_route[16]="172.25.100.254";
		memcpy(g_track_strategy_info.local_ip_info.route, local_route, sizeof(g_track_strategy_info.local_ip_info.route) );
	}
	//���ñ�����ϵͳIP
	config_sys_ip(g_track_strategy_info.local_ip_info.ip, g_track_strategy_info.local_ip_info.mask, g_track_strategy_info.local_ip_info.route);
	
	//��ð������ģʽ
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, BLACKBOARD_SCHEME);
	sprintf(param_name, "%s",text);
	
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.blackboard_scheme= atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.blackboard_scheme = %d\n"
						, g_track_strategy_info.blackboard_scheme);
	}
	else
	{
		g_track_strategy_info.blackboard_scheme= 0;
	}
	
	//��ð������
	memset(temp, 0, FILE_NAME_LEN);
	strcpy(text, BLACKBOARD_NUM);
	sprintf(param_name, "%s",text);
	
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, param_name, temp);
	if(ret == 0)
	{
		g_track_strategy_info.blackboard_num= atoi(temp);
		DEBUG(DL_WARNING, "GetStrategyInfo g_track_strategy_info.blackboard_num = %d\n"
						, g_track_strategy_info.blackboard_num);
	}
	else
	{
		g_track_strategy_info.blackboard_num= 2;
	}

	int blackboard_num = 0;
	for (blackboard_num = 0; blackboard_num < g_track_strategy_info.blackboard_num; blackboard_num++)
	{
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, BLACKBOARD_IP);
		sprintf(param_name, "%s%d",text, blackboard_num);
		memset(temp, 0, FILE_NAME_LEN);
		ret =  ConfigGetKey(config_file, NETWORK_INFO, param_name, temp);
		if (ret == 0)
		{
			g_track_strategy_info.blackboard_ip_info[blackboard_num].state = 1;
			memcpy(g_track_strategy_info.blackboard_ip_info[blackboard_num].ip, temp, sizeof(g_track_strategy_info.blackboard_ip_info[blackboard_num].ip) );
			DEBUG(DL_DEBUG, "g_track_strategy_info.blackboard_ip_info[%d].ip = %s\n",blackboard_num, g_track_strategy_info.blackboard_ip_info[blackboard_num].ip);
		}
		else
		{
			memcpy(g_track_strategy_info.blackboard_ip_info[blackboard_num].ip, BLACKBOARDTRACE_IP[blackboard_num], sizeof(g_track_strategy_info.blackboard_ip_info[blackboard_num].ip) );
		}
	}

	
	//��ȡ��ѵ��Ϣ
	for(stu_num=0;stu_num<g_track_strategy_info.stu_num;stu_num++)
	{
		//��ȡ��ѯ����,Ĭ��Ϊ0������ѯ��1��ʾ��ѯ
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_TURN_TYPE);
		sprintf(param_name, "%s%d",text, stu_num);
		memset(temp, 0, FILE_NAME_LEN);
		ret =  ConfigGetKey(config_file, TRACK_TURN_INFO, param_name, temp);
		if(ret != 0)
		{
			g_stu_turn_info.tStuTrackTurnInfo[stu_num].nTrackTurnType = 0;
		}
		else
		{
			g_stu_turn_info.tStuTrackTurnInfo[stu_num].nTrackTurnType = atoi(temp);
		}
		
		//������ѯʱ����,Ĭ��Ϊ120��120s��ѯһ��
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_TURN_TIME);
		sprintf(param_name, "%s%d",text, stu_num);
		memset(temp, 0, FILE_NAME_LEN);
		ret =  ConfigGetKey(config_file, TRACK_TURN_INFO, param_name, temp);
		if(ret != 0)
		{
			g_stu_turn_info.tStuTrackTurnInfo[stu_num].nTrackTurnTime = 0;
		}
		else
		{
			g_stu_turn_info.tStuTrackTurnInfo[stu_num].nTrackTurnTime = atoi(temp);
		}
		
		//������ѯ�೤ʱ���л���ʦ,Ĭ��2s���л���ʦ
		memset(param_name, 0, FILE_NAME_LEN);
		memset(text, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_TURN_SHOW_TIME);
		sprintf(param_name, "%s%d",text, stu_num);
		memset(temp, 0, FILE_NAME_LEN);
		ret =  ConfigGetKey(config_file, TRACK_TURN_INFO, param_name, temp);
		if(ret != 0)
		{
			g_stu_turn_info.tStuTrackTurnInfo[stu_num].nTrackTurnShowTime = 0;
		}
		else
		{
			g_stu_turn_info.tStuTrackTurnInfo[stu_num].nTrackTurnShowTime = atoi(temp);
		}
		DEBUG(DL_WARNING, "GetStrategyInfo student %d ,g_stu_turn_info.type= %d,time=%d,showtime=%d\n",stu_num,g_stu_turn_info.tStuTrackTurnInfo[stu_num].nTrackTurnType,g_stu_turn_info.tStuTrackTurnInfo[stu_num].nTrackTurnTime,g_stu_turn_info.tStuTrackTurnInfo[stu_num].nTrackTurnShowTime);
	}
	
	//��ʦ���˵ĸ���ģʽ��Ĭ��0Ϊ��ѧ��ȫ����ѧ��������
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, LOST_MODE, temp);
	g_track_strategy_info.lostmode = atoi(temp);

	DEBUG(DL_WARNING, "g_track_strategy_info.lostmode = %d\n",g_track_strategy_info.lostmode);
	if(ret != 0)
	{
		g_track_strategy_info.lostmode = 2;
	}	
	return 0;
}



/**
* @	��������: track_init()
* @	��������: ��ʼ�����ٲ���
* @ �������: 
* @ �������:
* @ ����ֵ:
*/
int track_init(ITRACK_STUDENTS_RIGHT_SIDE_Params *track_param)
{
	int 	ret = -1;
	char 	config_file[FILE_NAME_LEN] 	= {0};
	char 	temp[FILE_NAME_LEN]			= {0};
	char	text[FILE_NAME_LEN]			= {0};
	char	param_name[FILE_NAME_LEN] 	= {0};
	int		index						= 0;
	int		i 							= 0;

	memset(track_param,0,sizeof(ITRACK_STUDENTS_RIGHT_SIDE_Params));
	track_param->size = sizeof(ITRACK_STUDENTS_RIGHT_SIDE_Params);

	init_angle_param(&g_model_classroom,CAM_ANGLE_NUM);
	
	memset(config_file, 0, FILE_NAME_LEN);
	strcpy(config_file, TRACK_TEACHER_POINT_CAM_FILE);

	//���������
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_PARAM, CAM_TYPE_NAME, temp);
	g_angle_info.cam_type = atoi(temp);

	DEBUG(DL_DEBUG, "g_angle_info.cam_type = %d\n",g_angle_info.cam_type);

	if(ret != 0)
	{
		g_angle_info.cam_type = BLM_500R;
	}

	
	//��Ƶ���
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, STATIC_NAME, VIDEO_WIDTH, temp);
	track_param->static_param.video_width = atoi(temp);

	DEBUG(DL_DEBUG, "track_param->static_param.video_width = %d\n",track_param->static_param.video_width);

	if(ret != 0)
	{
		track_param->static_param.video_width = 704;
	}

	
	//��Ƶ�߶�
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, STATIC_NAME, VIDEO_HEIGHT, temp);
	track_param->static_param.video_height = atoi(temp);

	DEBUG(DL_DEBUG, "track_param->static_param.video_height = %d\n",track_param->static_param.video_height);

	if(ret != 0)
	{
		track_param->static_param.video_height = 576;
	}

	//���ٴ���ͼ��Ŀ��
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, STATIC_NAME, PIC_WIDTH, temp);
	track_param->static_param.pic_width = atoi(temp);

	DEBUG(DL_DEBUG, "track_param->static_param.pic_width = %d\n",track_param->static_param.pic_width);

	if(ret != 0)
	{
		track_param->static_param.pic_width = 160;
	}

	//���ٴ���ͼ��ĸ߶�
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, STATIC_NAME, PIC_HEIGHT, temp);
	track_param->static_param.pic_height = atoi(temp);

	DEBUG(DL_DEBUG, "track_param->static_param.pic_height = %d\n",track_param->static_param.pic_height);

	if(ret != 0)
	{
		track_param->static_param.pic_height = 90;
	}
	

	//���ٻ��Ƿ����
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, TRACK_STUDENTS_RIGHT_SIDE_ENCODE, TRACK_STUDENTS_RIGHT_SIDE_IS_ENCODE, temp);
	g_track_encode_info.is_encode= atoi(temp);

	DEBUG(DL_DEBUG, "g_track_encode_info.is_encode = %d\n",g_track_encode_info.is_encode);
	if(ret != 0)
	{
		g_track_encode_info.is_encode = 1;
	}	

	//����ģʽ,���Զ�ģʽ�����ֶ�ģʽ
	//Ĭ��Ϊ�Զ�ģʽ�����ٶ�д�����ļ���������̨���ó��ֶ�ģʽʱ���Ż�����ڴ��״̬Ϊ�ֶ�ģʽ�����Բ�д�����ļ�
	track_param->dynamic_param.control_mode = 0;
	/*
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, CONTROL_MODE, temp);
	track_param->dynamic_param.control_mode = atoi(temp);
	
	DEBUG(DL_DEBUG, "track_param->dynamic_param.control_mode = %d\n",track_param->dynamic_param.control_mode);

	if(ret != 0)
	{
		track_param->dynamic_param.control_mode = 0;
	}
	*/

	//����ѧϰģʽ��Ĭ��Ϊ0������ģʽ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, CLASSROOM_STUDY_MODE, temp);
	track_param->dynamic_param.classroom_study_mode = atoi(temp);
	
	DEBUG(DL_DEBUG, "track_param->dynamic_param.classroom_study_mode = %d\n",track_param->dynamic_param.classroom_study_mode);

	if(ret != 0)
	{
		track_param->dynamic_param.classroom_study_mode = ROOM_STANDER;
	}

	//�Ƿ���ѧ������
	g_track_param.dynamic_param.student_enable_state = 1;
	 
	//�����������
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, TRACK_POINT_NUM, temp);
	track_param->dynamic_param.track_point_num = atoi(temp);

	DEBUG(DL_DEBUG, "track_param->dynamic_param.track_point_num = %d\n",track_param->dynamic_param.track_point_num);
	if(ret != 0)
	{
		track_param->dynamic_param.track_point_num = 4;
	}
	
	
	//������������϶����xֵ,������0
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, TRIGGER0_X0, temp);
	track_param->dynamic_param.trigger[0].trigger_x0 = atoi(temp);
#ifdef ENC_1200_DEBUG
	PRINTF("track_param->dynamic_param.trigger[0].trigger_x0 = %d\n",track_param->dynamic_param.trigger[0].trigger_x0);
#else
	DEBUG(DL_WARNING, "track_param->dynamic_param.trigger[0].trigger_x0 = %d\n",track_param->dynamic_param.trigger[0].trigger_x0);
#endif
	if(ret != 0)
	{
		track_param->dynamic_param.trigger[0].trigger_x0 = 0;
	}
	
	//������������϶����Yֵ,������0
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, TRIGGER0_Y0, temp);
	track_param->dynamic_param.trigger[0].trigger_y0 = atoi(temp);
#ifdef ENC_1200_DEBUG
	PRINTF("track_param->dynamic_param.trigger[0].trigger_y0 = %d\n",track_param->dynamic_param.trigger[0].trigger_y0);
#else
	DEBUG(DL_WARNING, "track_param->dynamic_param.trigger[0].trigger_y0 = %d\n",track_param->dynamic_param.trigger[0].trigger_y0);
#endif
	if(ret != 0)
	{
		track_param->dynamic_param.trigger[0].trigger_y0 = 0;
	}

	//������������¶����xֵ,������0
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, TRIGGER0_X1, temp);
	track_param->dynamic_param.trigger[0].trigger_x1 = atoi(temp);
#ifdef ENC_1200_DEBUG
	PRINTF("track_param->dynamic_param.trigger[0].trigger_x1 = %d\n",track_param->dynamic_param.trigger[0].trigger_x1);
#else
		DEBUG(DL_WARNING, "track_param->dynamic_param.trigger[0].trigger_x1 = %d\n",track_param->dynamic_param.trigger[0].trigger_x1);
#endif
	if(ret != 0)
	{
		track_param->dynamic_param.trigger[0].trigger_x1 = 0;
	}

	//������������¶����yֵ,������0
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, TRIGGER0_Y1, temp);
	track_param->dynamic_param.trigger[0].trigger_y1 = atoi(temp);
#ifdef ENC_1200_DEBUG
	PRINTF("track_param->dynamic_param.trigger[0].trigger_y1 = %d\n",track_param->dynamic_param.trigger[0].trigger_y1);
#else
	DEBUG(DL_WARNING, "track_param->dynamic_param.trigger[0].trigger_y1 = %d\n",track_param->dynamic_param.trigger[0].trigger_y1);
#endif
	if(ret != 0)
	{
		track_param->dynamic_param.trigger[0].trigger_y1 = 0;
	}


	//������������϶����xֵ,������1
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, TRIGGER1_X0, temp);
	track_param->dynamic_param.trigger[1].trigger_x0 = atoi(temp);
#ifdef ENC_1200_DEBUG
	PRINTF("track_param->dynamic_param.trigger[1].trigger_x0 = %d\n",track_param->dynamic_param.trigger[1].trigger_x0);
#else
	DEBUG(DL_WARNING, "track_param->dynamic_param.trigger[1].trigger_x0 = %d\n",track_param->dynamic_param.trigger[1].trigger_x0);
#endif
	if(ret != 0)
	{
		track_param->dynamic_param.trigger[1].trigger_x0 = 0;
	}
	
	//������������϶����Yֵ,������1
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, TRIGGER1_Y0, temp);
	track_param->dynamic_param.trigger[1].trigger_y0 = atoi(temp);
#ifdef ENC_1200_DEBUG
	PRINTF("track_param->dynamic_param.trigger[1].trigger_y0 = %d\n",track_param->dynamic_param.trigger[1].trigger_y0);
#else
	DEBUG(DL_WARNING, "track_param->dynamic_param.trigger[1].trigger_y0 = %d\n",track_param->dynamic_param.trigger[1].trigger_y0);
#endif
	if(ret != 0)
	{
		track_param->dynamic_param.trigger[1].trigger_y0 = 0;
	}

	//������������¶����xֵ,������1
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, TRIGGER1_X1, temp);
	track_param->dynamic_param.trigger[1].trigger_x1 = atoi(temp);
#ifdef ENC_1200_DEBUG
	PRINTF("track_param->dynamic_param.trigger[1].trigger_x1 = %d\n",track_param->dynamic_param.trigger[1].trigger_x1);
#else
	DEBUG(DL_WARNING, "track_param->dynamic_param.trigger[1].trigger_x1 = %d\n",track_param->dynamic_param.trigger[1].trigger_x1);
#endif
	if(ret != 0)
	{
		track_param->dynamic_param.trigger[1].trigger_x1 = 0;
	}

	//������������¶����yֵ,������1
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, TRIGGER1_Y1, temp);
	track_param->dynamic_param.trigger[1].trigger_y1 = atoi(temp);
#ifdef ENC_1200_DEBUG
	PRINTF("track_param->dynamic_param.trigger[1].trigger_y1 = %d\n",track_param->dynamic_param.trigger[1].trigger_y1);
#else
	DEBUG(DL_WARNING, "track_param->dynamic_param.trigger[1].trigger_y1 = %d\n",track_param->dynamic_param.trigger[1].trigger_y1);
#endif
	if(ret != 0)
	{
		track_param->dynamic_param.trigger[1].trigger_y1 = 0;
	}

	for(index = 0; index < track_param->dynamic_param.track_point_num; index++)
	{
		//��������������
		memset(text, 0, FILE_NAME_LEN);
		memset(param_name, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_POINTX);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		ret =  ConfigGetKey(config_file, DYNAMIC_NAME, param_name, temp);
		track_param->dynamic_param.track_point[index].x = atoi(temp);

		memset(text, 0, FILE_NAME_LEN);
		memset(param_name, 0, FILE_NAME_LEN);
		strcpy(text, TRACK_POINTY);
		sprintf(param_name, "%s%d",text, index);
		memset(temp, 0, FILE_NAME_LEN);
		ret =  ConfigGetKey(config_file, DYNAMIC_NAME, param_name, temp);
		track_param->dynamic_param.track_point[index].y = atoi(temp);

		DEBUG(DL_DEBUG, "track_param->dynamic_param.track_point[%d].x = %d\n",index,track_param->dynamic_param.track_point[index].x);
		DEBUG(DL_DEBUG, "track_param->dynamic_param.track_point[%d].y = %d\n",index,track_param->dynamic_param.track_point[index].y);

		if(ret != 0)
		{
			track_param->dynamic_param.track_point[0].x	= 0;
			track_param->dynamic_param.track_point[0].y	= 200;
			track_param->dynamic_param.track_point[1].x	= 700;
			track_param->dynamic_param.track_point[1].y	= 200;
			track_param->dynamic_param.track_point[2].x	= 700;
			track_param->dynamic_param.track_point[2].y	= 260;
			track_param->dynamic_param.track_point[3].x	= 0;
			track_param->dynamic_param.track_point[3].y	= 260;
			break;
		}
	}

	//����������
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, TRIGGER_SUM, temp);
	track_param->dynamic_param.trigger_sum = atoi(temp);

	DEBUG(DL_DEBUG, "track_param->dynamic_param.trigger_sum = %d\n",track_param->dynamic_param.trigger_sum);

	if(ret != 0)
	{
		track_param->dynamic_param.trigger_sum = 30;
	}

	//sensֵ,ԽС��Եֵ�ҵ�Խ��
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, SENS, temp);
	track_param->dynamic_param.sens = atoi(temp);

	DEBUG(DL_DEBUG, "track_param->dynamic_param.sens = %d\n",track_param->dynamic_param.sens);

	if(ret != 0)
	{
		track_param->dynamic_param.sens = 30;
	}

	//��ʦ��ʧ�ĳ�ʱʱ��
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, RESET_TIME, temp);
	track_param->dynamic_param.reset_time = atoi(temp);

	DEBUG(DL_DEBUG, "track_param->dynamic_param.reset_time = %d\n",track_param->dynamic_param.reset_time);

	if(ret != 0)
	{
		track_param->dynamic_param.reset_time = 50;
	}

	//����ȡ����ʽ��0�ǿ�ǰȡ����1�ǿ���ȡ��
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, TRACK_MODE, temp);
	track_param->dynamic_param.track_mode = atoi(temp);

	DEBUG(DL_DEBUG, "track_param->dynamic_param.track_mode = %d\n",track_param->dynamic_param.track_mode);

	if(ret != 0)
	{
		track_param->dynamic_param.track_mode = 0;
	}

	
	//���߱�־
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, MESSAGE, temp);
	track_param->dynamic_param.message = atoi(temp);

	DEBUG(DL_DEBUG, "track_param->dynamic_param.message = %d\n",track_param->dynamic_param.message);

	if(ret != 0)
	{
		track_param->dynamic_param.message = 1;
	}


	//����ͷԤ��λ��λ����Ϣ,0��λ����ʱ����,û��ʹ��
	for(index = 1; index < PRESET_NUM_MAX; index++)
	{
		memset(temp, 0, FILE_NAME_LEN);
		strcpy(text, PRESET_POSITION_VALUE);
		sprintf(param_name, "%s%d",text, index);
		
		ret =  ConfigGetKey(config_file, PRESET_POSITION, param_name, temp);
		if(ret == 0)
		{
			for(i = 0; i < CAM_PAN_TILT_LEN; i++)
			{
				if((temp[i] >= 0x30) && (temp[i] <= 0x39))
				{
					g_cam_info.cam_position[index].pan_tilt[i] = temp[i] - 0x30;
				}
				if((temp[i] >= 0x41) && (temp[i] <= 0x46))
				{
					g_cam_info.cam_position[index].pan_tilt[i] = temp[i] - 0x41 + 0xa;
				}
				if((temp[i] >= 0x61) && (temp[i] <= 0x66))
				{
					g_cam_info.cam_position[index].pan_tilt[i] = temp[i] - 0x61 + 0xa;
				}
			}
		}	
		else
		{
			if(SONY_BRC_Z330 == g_angle_info.cam_type)
			{
				
				g_cam_info.cam_position[index].pan_tilt[0] = 0x07;
				g_cam_info.cam_position[index].pan_tilt[1] = 0x0f;
				g_cam_info.cam_position[index].pan_tilt[2] = 0x0f;
				g_cam_info.cam_position[index].pan_tilt[3] = 0x0f;
				g_cam_info.cam_position[index].pan_tilt[4] = 0x0f;
				g_cam_info.cam_position[index].pan_tilt[5] = 0x07;
				g_cam_info.cam_position[index].pan_tilt[6] = 0x0f;
				g_cam_info.cam_position[index].pan_tilt[7] = 0x0f;
				g_cam_info.cam_position[index].pan_tilt[8] = 0x0f;

			}
			else
			{
				g_cam_info.cam_position[index].pan_tilt[0] = 0x07;
				g_cam_info.cam_position[index].pan_tilt[1] = 0x0f;
				g_cam_info.cam_position[index].pan_tilt[2] = 0x0f;
				g_cam_info.cam_position[index].pan_tilt[3] = 0x0f;
				g_cam_info.cam_position[index].pan_tilt[4] = 0x07;
				g_cam_info.cam_position[index].pan_tilt[5] = 0x0f;
				g_cam_info.cam_position[index].pan_tilt[6] = 0x0f;
				g_cam_info.cam_position[index].pan_tilt[7] = 0x0f;
			}
		}
		


		DEBUG(DL_DEBUG, "###########index = %d,%s\n",index, temp);

		memset(temp, 0, FILE_NAME_LEN);
		strcpy(text, PRESET_ZOOM_VALUE);
		sprintf(param_name, "%s%d",text, index);
		
		ret =  ConfigGetKey(config_file, PRESET_POSITION, param_name, temp);
		if(ret == 0)
		{
			for(i = 0; i < 4; i++)
			{
				if((temp[i] >= 0x30) && (temp[i] <= 0x39))
				{
					g_cam_info.cam_position[index].zoom[i] = temp[i] - 0x30;
				}
				if((temp[i] >= 0x41) && (temp[i] <= 0x46))
				{
					g_cam_info.cam_position[index].zoom[i] = temp[i] - 0x41 + 0xa;
				}
				if((temp[i] >= 0x61) && (temp[i] <= 0x66))
				{
					g_cam_info.cam_position[index].zoom[i] = temp[i] - 0x61 + 0xa;
				}
			}
		}	
		else
		{
			g_cam_info.cam_position[index].zoom[0] = 0x07;
			g_cam_info.cam_position[index].zoom[1] = 0x0f;
			g_cam_info.cam_position[index].zoom[2] = 0x0f;
			g_cam_info.cam_position[index].zoom[3] = 0x0f;
		}
		
	}

	//��λ��������ڵĽ��ҳ���
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_PARAM, POSITION_CAM_LENTH, temp);
	g_angle_info.position_camera_lenth = atoi(temp);

	DEBUG(DL_DEBUG, "g_angle_info.position_camera_lenth = %d\n",g_angle_info.position_camera_lenth);

	if(ret != 0)
	{
		g_angle_info.position_camera_lenth = 1;
	}

	//��λ��������ڵĽ��ҿ��
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_PARAM, POSITION_CAM_WIDTH, temp);
	g_angle_info.position_camera_width = atoi(temp);

	DEBUG(DL_DEBUG, "g_angle_info.position_camera_width = %d\n",g_angle_info.position_camera_width);

	if(ret != 0)
	{
		g_angle_info.position_camera_width = 1;
	}

	//��λ��������ڵĸ߶�
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_PARAM, POSITION_CAM_HEIGHT, temp);
	g_angle_info.position_camera_height = atoi(temp);

	DEBUG(DL_DEBUG, "g_angle_info.position_camera_height = %d\n",g_angle_info.position_camera_height);

	if(ret != 0)
	{
		g_angle_info.position_camera_height = 1;
	}

	//������������ڵĳ���
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_PARAM, TRACK_CAM_LENTH, temp);
	g_angle_info.track_camera_lenth = atoi(temp);

	DEBUG(DL_DEBUG, "g_angle_info.track_camera_lenth = %d\n",g_angle_info.track_camera_lenth);

	if(ret != 0)
	{
		g_angle_info.track_camera_lenth = 1;
	}

	//������������ڵĿ��
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_PARAM, TRACK_CAM_WIDTH, temp);
	g_angle_info.track_camera_width = atoi(temp);

	DEBUG(DL_DEBUG, "g_angle_info.track_camera_width = %d\n",g_angle_info.track_camera_width);

	if(ret != 0)
	{
		g_angle_info.track_camera_width = 1;
	}

	
	//������������ڵĸ߶�
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_PARAM, TRACK_CAM_HEIGHT, temp);
	g_angle_info.track_camera_height = atoi(temp);

	DEBUG(DL_DEBUG, "g_angle_info.track_camera_height = %d\n",g_angle_info.track_camera_height);

	if(ret != 0)
	{
		g_angle_info.track_camera_height = 1;
	}

	//���������ˮƽ����ƫ��ֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_PARAM, TRACK_CAM_OFFSET_PAN, temp);
	g_angle_info.camera_pan_offset = atoi(temp);

	DEBUG(DL_DEBUG, "g_angle_info.camera_pan_offset = %d\n",g_angle_info.camera_pan_offset);

	if(ret != 0)
	{
		g_angle_info.camera_pan_offset = 0;
	}	

	//�����������ֱ����ƫ��ֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_PARAM, TRACK_CAM_OFFSET_TILT, temp);
	g_angle_info.camera_tilt_offset = atoi(temp);

	DEBUG(DL_DEBUG, "g_angle_info.camera_tilt_offset = %d\n",g_angle_info.camera_tilt_offset);

	if(ret != 0)
	{
		g_angle_info.camera_tilt_offset = 0;
	}	

	//�������������ƫ��ֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_PARAM, TRACK_CAM_OFFSET_ZOOM, temp);
	g_angle_info.camera_zoom_offset = atoi(temp);

	DEBUG(DL_DEBUG, "g_angle_info.camera_zoom_offset = %d\n",g_angle_info.camera_zoom_offset);

	if(ret != 0)
	{
		g_angle_info.camera_zoom_offset = 0;
	}	

	//����һ��������ڱ���ͼ���е�λ��x
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_PARAM, OTHER_CAM_X, temp);
	g_angle_info.other_cam_x = atoi(temp);

	DEBUG(DL_DEBUG, "g_angle_info.other_cam_x = %d\n",g_angle_info.other_cam_x);

	if(ret != 0)
	{
		g_angle_info.other_cam_x = 0;
	}	
	

	//����һ��������ڱ���ͼ���е�λ��y
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_PARAM, OTHER_CAM_Y, temp);
	g_angle_info.other_cam_y = atoi(temp);

	DEBUG(DL_DEBUG, "g_angle_info.other_cam_y = %d\n",g_angle_info.other_cam_y);

	if(ret != 0)
	{
		g_angle_info.other_cam_y = 0;
	}	

	
	//����һ��������ڱ���ͼ���еĿ�
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_PARAM, OTHER_CAM_WIDTH, temp);
	g_angle_info.other_cam_width = atoi(temp);

	DEBUG(DL_DEBUG, "g_angle_info.other_cam_width = %d\n",g_angle_info.other_cam_width);

	if(ret != 0)
	{
		g_angle_info.other_cam_width = 1;
	}	

	//����һ��������ڱ���ͼ���еĸ�
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_PARAM, OTHER_CAM_HEIGHT, temp);
	g_angle_info.other_cam_height = atoi(temp);

	DEBUG(DL_DEBUG, "g_angle_info.other_cam_height = %d\n",g_angle_info.other_cam_height);

	if(ret != 0)
	{
		g_angle_info.other_cam_height = 1;
	}	

	//����1������ⷶΧ�����϶����xֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, BLACKBOARD1_TRIGGER_X0, temp);
	g_track_tactics_info.broadcast_info[0].point[0].x = atoi(temp);

	DEBUG(DL_WARNING, "g_track_tactics_info.broadcast_info[0].point[0].x = %d\n",g_track_tactics_info.broadcast_info[0].point[0].x);
	if(ret != 0)
	{
		g_track_tactics_info.broadcast_info[0].point[0].x = 0;
	}

	//����1������ⷶΧ�����϶����Yֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, BLACKBOARD1_TRIGGER_Y0, temp);
	g_track_tactics_info.broadcast_info[0].point[0].y = atoi(temp);

	DEBUG(DL_WARNING, "g_track_tactics_info.broadcast_info[0].point[0].y = %d\n",g_track_tactics_info.broadcast_info[0].point[0].y);
	if(ret != 0)
	{
		g_track_tactics_info.broadcast_info[0].point[0].y = 0;
	}

	//����1������ⷶΧ�����¶����xֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, BLACKBOARD1_TRIGGER_X1, temp);
	g_track_tactics_info.broadcast_info[0].point[1].x = atoi(temp);

	DEBUG(DL_WARNING, "g_track_tactics_info.broadcast_info[0].point[1].x = %d\n",g_track_tactics_info.broadcast_info[0].point[1].x);
	if(ret != 0)
	{
		g_track_tactics_info.broadcast_info[0].point[1].x = 0;
	}

	//����1������ⷶΧ�����¶����yֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, BLACKBOARD1_TRIGGER_Y1, temp);
	g_track_tactics_info.broadcast_info[0].point[1].y = atoi(temp);

	DEBUG(DL_WARNING, "g_track_tactics_info.broadcast_info[0].point[1].y = %d\n",g_track_tactics_info.broadcast_info[0].point[1].y);
	if(ret != 0)
	{
		g_track_tactics_info.broadcast_info[0].point[1].y = 0;
	}

	
	//����2������ⷶΧ�����϶����xֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, BLACKBOARD2_TRIGGER_X0, temp);
	g_track_tactics_info.broadcast_info[1].point[0].x = atoi(temp);

	DEBUG(DL_WARNING, "g_track_tactics_info.broadcast_info[1].point[0].x = %d\n",g_track_tactics_info.broadcast_info[1].point[0].x);
	if(ret != 0)
	{
		g_track_tactics_info.broadcast_info[1].point[0].x = 0;
	}

	//����2������ⷶΧ�����϶����Yֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, BLACKBOARD2_TRIGGER_Y0, temp);
	g_track_tactics_info.broadcast_info[1].point[0].y = atoi(temp);

	DEBUG(DL_WARNING, "g_track_tactics_info.broadcast_info[1].point[0].y = %d\n",g_track_tactics_info.broadcast_info[1].point[0].y);
	if(ret != 0)
	{
		g_track_tactics_info.broadcast_info[1].point[0].y = 0;
	}

	//����2������ⷶΧ�����¶����xֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, BLACKBOARD2_TRIGGER_X1, temp);
	g_track_tactics_info.broadcast_info[1].point[1].x = atoi(temp);

	DEBUG(DL_WARNING, "g_track_tactics_info.broadcast_info[1].point[1].x = %d\n",g_track_tactics_info.broadcast_info[1].point[1].x);
	if(ret != 0)
	{
		g_track_tactics_info.broadcast_info[1].point[1].x = 0;
	}

	//����2������ⷶΧ�����¶����yֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, BLACKBOARD2_TRIGGER_Y1, temp);
	g_track_tactics_info.broadcast_info[1].point[1].y = atoi(temp);

	DEBUG(DL_WARNING, "g_track_tactics_info.broadcast_info[1].point[1].y = %d\n",g_track_tactics_info.broadcast_info[1].point[1].y);
	if(ret != 0)
	{
		g_track_tactics_info.broadcast_info[1].point[1].y = 0;
	}

	//��ʦ��д����Y���ϱ߽��ֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, TRACK_STRATEGY, TEACHER_FEATURE_START_Y, temp);
	g_track_tactics_info.teacher_feature_info[0].point[0].y = atoi(temp);

	DEBUG(DL_WARNING, "g_track_tactics_info.teacher_feature_info[0].point[0].y = %d\n", g_track_tactics_info.teacher_feature_info[0].point[0].y);
	if(ret != 0)
	{
		g_track_tactics_info.teacher_feature_info[0].point[0].y = 540;
	}

	//����ͷת���ٶȻ���ֵ
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, CAM_PARAM, CAM_SPEED, temp);
	g_angle_info.cam_speed = atoi(temp);

	DEBUG(DL_WARNING, "g_angle_info.cam_speed = %d\n",g_angle_info.cam_speed);
	if(ret != 0)
	{
		g_angle_info.cam_speed = 5;
	}	

	

	//ˢ�±���ʱ��
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, REFRESH_TIME, temp);
	g_track_encode_info.refresh_time = atoi(temp);
	DEBUG(DL_DEBUG, "g_track_encode_info.refresh_time = %d\n",g_track_encode_info.refresh_time);
	if(ret != 0)
	{
		g_track_encode_info.refresh_time = 300;
	}
	if(g_track_encode_info.refresh_time > 600)
	{
		g_track_encode_info.refresh_time = 600;
	}
	if(g_track_encode_info.refresh_time < 0)
	{
		g_track_encode_info.refresh_time = 0;
	}

	//��ȡ�Ƿ����ø���ʦ���
	memset(temp, 0, FILE_NAME_LEN);
	ret =  ConfigGetKey(config_file, DYNAMIC_NAME, HIGH_PRESET, temp);
//��ʱ��֧�ָߵ�λ
	g_track_encode_info.highpreset = 0;//atoi(temp);
	DEBUG(DL_DEBUG, "g_track_encode_info.highpreset = %d\n",g_track_encode_info.highpreset);
	if(ret != 0)
	{
		g_track_encode_info.highpreset = 0;
	}

	g_angle_info.aux_base_angle 		= g_model_classroom.angle[12];
	g_angle_info.host_base_angle 		= g_model_classroom.angle[22];


	g_angle_info.cam_addr				= 0x81;
	g_track_encode_info.is_track		= 1;
	g_track_encode_info.call_position_no=LOW_TEACHER_PRESET;

	//�������ļ���ʼ���ڴ����
	get_strategy_info_form_file();

	//����Ϊ�Զ�����ģʽ
	server_set_track_type(0);
	return 0;
	
}

/*check IP addr */
int CheckIP(int ipaddr, int netmask)
{
	int mask, ip, gateip;
	mask = netmask;
	mask = htonl(mask);
	ip = ipaddr;
	ip = htonl(ip);


	if((((ip & 0xFF000000) >> 24) > 223) || ((((ip & 0xFF000000) >> 24 == 127)))) {
		return 0;
	}

	if(((ip & 0xFF000000) >> 24) < 1) {
		return 0;
	}

	if((ip & mask) == 0) {
		return 0;
	}

	if((ip & (~mask)) == 0) {
		return 0;
	}

	if((~(ip | mask)) == 0) {
		return 0;
	}

	while(mask != 0) {
		if(mask > 0) {
			return 0;
		}

		mask <<= 1;
	}

	return 1;
}

/*check IP addr and Netmask */
int CheckIPNetmask(int ipaddr, int netmask, int gw)
{
 int mask, ip, gateip;
 mask = netmask;
 mask = htonl(mask);
 ip = ipaddr;
 ip = htonl(ip);
 gateip = gw;
 gateip = htonl(gateip);

 if((((ip & 0xFF000000) >> 24) > 223) || ((((ip & 0xFF000000) >> 24 == 127)))) {
  return 0;
 }

 if(((ip & 0xFF000000) >> 24) < 1) {
  return 0;
 }

 if((((gateip & 0xFF000000) >> 24) > 223) || (((gateip & 0xFF000000) >> 24 == 127))) {
  return 0;
 }

 if(((gateip & 0xFF000000) >> 24) < 1) {
  return 0;
 }

 if((ip & mask) == 0) {
  return 0;
 }

 if((ip & (~mask)) == 0) {
  return 0;
 }

 if((~(ip | mask)) == 0) {
  return 0;
 }

 while(mask != 0) {
  if(mask > 0) {
   return 0;
  }

  mask <<= 1;
 }

 return 1;
}

int config_sys_ip(unsigned char *pIp, unsigned char *pMask, unsigned char *pRoute)
{
	char  temp_ip[FILE_NAME_LEN]   = {0};
	char  temp_route[FILE_NAME_LEN]   = {0};
	int  ret       = -1;
	char text[FILE_NAME_LEN]   = {0};
	
	struct in_addr addr_ip, addr_mask, addr_route;
	int ip_num = 0;
	int netmask_num = 0;
	int route_num = 0;

	memset(temp_ip, 0, FILE_NAME_LEN);
	memset(temp_route, 0, FILE_NAME_LEN);
	memset(text, 0, FILE_NAME_LEN);
	
	strcat(temp_ip, "ifconfig eth0 ");
	ip_num = inet_addr(pIp); 
	netmask_num = inet_addr(pMask); 
	route_num = inet_addr(pRoute); 
	memcpy(&addr_ip, &ip_num, 4);
	memcpy(&addr_mask, &netmask_num, 4);
	memcpy(&addr_route, &route_num, 4);
	
	strcat(temp_ip, inet_ntoa(addr_ip));
	strcat(temp_ip, " netmask ");
	strcat(temp_ip, inet_ntoa(addr_mask));

	DEBUG(DL_DEBUG, "ifconfig = %s\n", temp_ip);

	strcat(temp_route, "route add default gw ");
	strcat(temp_route, inet_ntoa(addr_route));
	DEBUG(DL_DEBUG, "route = %s\n", temp_route);
	
	if(CheckIPNetmask(ip_num, netmask_num, route_num) == 0)
	{
		DEBUG(DL_ERROR, "ERROR ip:%s, mask:%s, route:%s\n", inet_ntoa(addr_ip), inet_ntoa(addr_mask), inet_ntoa(addr_route));
		return -1;
	}

	ret = system(temp_ip); 
	if (ret <0)
	{
		DEBUG(DL_ERROR, "ERROR ip:%s, mask:%s, route:%s\n", inet_ntoa(addr_ip), inet_ntoa(addr_mask), inet_ntoa(addr_route));
		return -1;
	}

	ret = system(temp_route); 
	if (ret <0)
	{
		DEBUG(DL_ERROR, "ERROR ip:%s, mask:%s, route:%s\n", inet_ntoa(addr_ip), inet_ntoa(addr_mask), inet_ntoa(addr_route));
		return -1;
	}
	
	
	return 0;
}
