#ifndef	_MOTION_DETECT_H_
#define	_MOTION_DETECT_H_

#include <stdio.h>
#include <math.h>
#include <stdarg.h>

#include <stdint.h>  




#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#ifdef __cplusplus
}
#endif

//#define STANDARD_USB_CAPUTURE		//�����˾���ʹ�ñ�׼��USBЭ�������ͷ�ɼ���û�����ʹ�ù�˾�Ĳɼ��忨�ɼ�


//#define MINRECTFILLSIZE (20)
#define MINRECTWIDTH 	(30)//(10)
#define MINRECTHEIGHT 	(40)//(10)
#define	SHORT_MAX		(65535)
#define	MAXRECTNUM		(1000)		//��⵽�Ŀ���Ϊһ���ŵĸ���
#define	MAX_TARGET_NUM	(100)		//��⵽�ı仯������

#define	TRACK_TRIGGER_MAX_NUM 	(2)	
#define	TRIGGER_POINT_NUM		(30)		//һ��ˢ���������ܹ���⵽�Ĵ�������

#define SRCBUFWIDTH 1920
#define SRCBUFHEIGHT 1088

/**
* @	���ص�����
*/
typedef struct _motion_pixel_point_info
{
	int16_t	x;		//x����
	int16_t	y;		//y����
}motion_vertex_t;

/**
* @	��������
*/
typedef struct
{
	int16_t min_x;
	int16_t min_y;
	int16_t max_x;
	int16_t max_y;
} rect_t;

/**
* @ ȡ����ʽ
*/
typedef enum _track_mode_enum
{
	CLASS_VIEW_FORWARD = 0,		//��ǰȡ��
	CLASS_VIEW,					//����ȡ��
	MAX_NUM_MODE
}track_mode_e;


/**
* @	������������õ�������
*/
#define	TRACK_STUDENTS_CEILTRACK_AREA_POINT_MAX	(10)

#define TRIGGER_NUM_MAX (11)

typedef struct TRect
{
	int nTop;
	int nBottom;
	int nLeft;
	int nRight;
	int nFillSize;
	int nMarkVal;
}T_Rect;

/**
* @	��������Ϣ�Ľṹ��
*/
typedef struct _motion_trigger_info
{
	int16_t	trigger_x0;		//��ʾ����������϶��������xֵ(��Դͼ��Ϊ��׼��)
	int16_t	trigger_y0;		//��ʾ����������϶��������yֵ(��Դͼ��Ϊ��׼��)
	int16_t	trigger_x1;		//��ʾ����������¶��������xֵ(��Դͼ��Ϊ��׼��)
	int16_t	trigger_y1;		//��ʾ����������¶��������yֵ(��Դͼ��Ϊ��׼��)
}motion_trigger_info_t;

/**
* @	��������Ϣ
*/
typedef struct _out_trigger_point_info
{
	int16_t	x0;		//x����(���϶���)
	int16_t	y0;		//y����(���϶���)
	int16_t	x1;		//x����(���϶���)
	int16_t	y1;		//y����(���϶���)
}out_trigger_point_info_t;

/**
* @ ���ߵ���Ϣ
*/
typedef struct _line_info
{
	uint32_t	start_x;	//�ߵ���ʼ��x����
	uint32_t	start_y;	//�ߵ���ʼ��y����
	uint32_t	end_x;		//�ߵĽ�����x����
	uint32_t	end_y;		//�ߵĽ�����y����
}line_info_t;

/**
* @ Y,Cb,Crֵ�ṹ��
*/
typedef struct 	_yuv_value_info
{
	uint16_t	y_value;		//y��ֵ
	uint16_t	cb_value;		//cb��ֵ
	uint16_t	cr_value;		//cr��ֵ
}yuv_value_info_t;


/**
* @	����controlʱ�Ƿ���Ҫ���³�ʼ����������
*/
typedef enum _motion_control_init_type
{
	MOTION_NO_INIT = 0,		//�������control
	MOTION_RE_INIT,			//��Ҫ��ʼ�����ֲ���
	MOTION_RE_START,			//��Ҫ��ʼ�����в���
	MOTION_RE_INIT_NUM
}motion_control_init_type_e;


/**
* @ ���������Ϣ
*/
typedef struct _motion_trigger_point_info
{
	int		x0;
	int		y0;
	int		x1;
	int		y1;
	int		trigger_flag;		//����Ĵ�����־
}motion_trigger_point_info_t;

/**
* @ ����Զ����ٵľ�̬����
*/
typedef struct 	_ITRACK_CEILTRACK_static_param_t
{
	int32_t	size;
	int16_t	video_width;			//������Ƶ�Ŀ�
	int16_t	video_height;			//������Ƶ�ĸ�
	int16_t	pic_width;				//�������ź�ͼ��Ŀ�
	int16_t	pic_height;				//�������ź��ͼ��ĸ�
}ITRACK_CEILTRACK_static_param_t;

/**
* @ ����Զ����ٵĶ�̬����
*/

typedef struct 	_ITRACK_CEILTRACK_dynamic_params_t
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
	motion_vertex_t	track_point[TRACK_STUDENTS_CEILTRACK_AREA_POINT_MAX];	//����������x��y��������,�����������֮ǰͼ����˵
	motion_trigger_info_t trigger[TRACK_TRIGGER_MAX_NUM];
	motion_control_init_type_e	reset_level;	//0��ʾ��Ҫ���³�ʼ��,1��ʾ��Ҫ���³�ʼ��
}ITRACK_CEILTRACK_dynamic_params_t;


/**
* @ ����Զ����ٵ��������
*/
typedef struct 	_CEILITRACK_Params
{
	int32_t	size;
	ITRACK_CEILTRACK_static_param_t 	static_param;
	ITRACK_CEILTRACK_dynamic_params_t	dynamic_param;
	//ITRACK_DynamicParams dynamic_param;
}CeilITRACK_Params;


/**
* @ ����Զ����ٵ��������
*/
typedef struct	_CeilITRACK_OutArgs
{
	int32_t	size;
	int32_t	server_cmd;						//���͸������������л��������ǲ���ѧ����������ʦ��������
	int32_t	cam_position;					//��������ͷת��ĳ��Ԥ��λ�ĵط�
	int32_t	move_distance;					//����ͷ�ƶ�����
	int32_t	move_direction;					//����ͷ�ƶ�����
	int32_t	cmd_type;						//2Ϊ����վ����Ϊ1��ʾ����վ����0��ʾ���¡�
	int32_t	track_status;					//����״̬
	int32_t	trigger_num;					//��������
	motion_vertex_t	trigger[TRIGGER_NUM_MAX];	//����������
	out_trigger_point_info_t	trigger_point[TRIGGER_NUM_MAX];	//����Ӧ�����Զ�ˢ�±�����
	int32_t     IsTrigger;
	uint32_t	reserve1;
	uint32_t	reserve2;
	uint32_t	reserve3;
	uint32_t	reserve4;
}CeilITRACK_OutArgs;


/**
* @	�����ڲ���ȫ�ֱ���
*/
typedef struct _T_CEILTRACK_GLOBAL_OBJ
{
	uint8_t *Area_vertex; //��ʾ���Ƿ����ڼ������
	uint8_t *gray_buf;  //���͸�ʴ֮���
	uint8_t *mid_buf;   //ԭʼ
	uint16_t *marked_buf;
	T_Rect	g_out_rect[MAX_TARGET_NUM];

	uint8_t *InBuffer; //
	uint8_t *src_rgb_buf; //���Ҫ���������Դrgb����

	uint8_t *tmp_src_rgb_buf; //���Ҫ���������Դrgb����

	uint8_t *dst_rgb_buf; //���ÿ�θ��ٺ�ĵ�һ֡rgb����

	uint8_t *pre_frame;//��һ֡

	int32_t nSrcBufWidth;//�ɼ�ԭʼͼ������ݰڷ�buf�Ŀ��
	int32_t nSrcBufHeight;//�ɼ�ԭʼͼ������ݰڷ�buf�Ŀ��
	int16_t	turn_flag;			//Ϊ0ʱ��ʾ����Ҫ���¸��٣�Ϊ1ʱ��ʾ����Ŀ�꣬���ø��ٱ�־��Ϊ2ʱ��ʾ����Ŀ�궪ʧ
	int16_t	turn_time;			//��λ��Ŀ���,��Ҫ�ȴ�����ͷ����Ԥ��λ��ʱ��
	int16_t	track_start;		//��ʾ�Ƿ������Ŀ�꣬0��ʾδ�����ϣ�1��ʾ��������Ŀ��
	int32_t	first_time;			//��ʼ�����㷨��֡��,Ŀǰ��ǰʮ֡��������
	int32_t	last_move_flag;		//�����ƶ���־,Ϊ0��ʾδ�ƶ�����,Ϊ1��ʾ�ƶ��˷���
	int32_t	last_track_status;	//�ϴθ���״̬
	int32_t	skin_colour_num;	//������⵽��ɫ��֡��
	uint64_t	frame_num;		//�㷨֡��

	T_Rect 					tInRect;

	int32_t debugValue0;
	int32_t debugValue1;
	int32_t debugValue2;

}T_CEIL_TRACK_GLOBAL_OBJ;


typedef struct _CeilTrackTrackHand {
	CeilITRACK_Params	input_param;	//�������
	T_CEIL_TRACK_GLOBAL_OBJ t_global_obj;
}CeilTrackHand;
typedef struct _CeilTrackSaveView
{
	unsigned int chId;
	unsigned int SaveViewFlag;
	unsigned int SaveType;
	motion_trigger_point_info_t	g_trigger_info[TRIGGER_POINT_NUM];
} CeilTrackSaveView;





extern int32_t motion_detect_init(CeilTrackHand *inst,int width,int height,int sens);
extern int32_t	motion_detect_process(CeilTrackHand *inst, int8_t *output_buf, CeilITRACK_OutArgs *output_param);
extern int32_t motion_detect_release(CeilTrackHand *inst);
#endif
