/* 
 * Copyright (c) 2009, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/*
 *  ======== itrack.h ========
 *  The track_students interface.
 */

#ifndef codecs_scale_ITRACK_STUDENTS_H_
#define codecs_scale_ITRACK_STUDENTS_H_

#ifdef __cplusplus
extern "C" {
#endif
	/*
#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/visa.h>
#include <ti/sdo/ce/skel.h>
#include <ti/xdais/xdas.h>

#include <ti/xdais/xdas.h>
#include <ti/xdais/ialg.h>
	*/
#include <stdlib.h>
#include<stdint.h>

typedef unsigned char uint8_t;
typedef float		float_t;
typedef unsigned short uint16_t;
typedef unsigned int	uint32_t;
typedef unsigned long long	uint64_t;


#define ITRACK_EOK      0
#define ITRACK_EFAIL    -1
#define ITRACK_ERUNTIME -2




/**
* @ ȡ��ĽǶȵĸ�������90�ȣ�180�ȵȵ�ȡ��
*/
#define	POINT_VIEW_NUM			(4)

/**
* @	������������õ�������
*/
#define	TRACK_AREA_POINT_MAX	(4)

/**
* @ ѧ�����ٴ���������ֵ
*/
#define STUDENTS_TRIGGER_NUM	(35)

/**
* @ ѧ���������ο�����ֵ
*/
#define STUDENTS_SHIELD_NUM	(20)
/**
* @ ѧ�����ٶ�Ŀ����̨��������ֵ
*/
#define STUDENTS_MULTITARGET_NUM	(10)

/**
* @ ѧ������Ԥ��λ�����ֵ,����Ҫ��һ��Ԥ��λ��ȫ��,����Ҫ��1
*/
#define STUDENTS_PRESET_NUM	(STUDENTS_TRIGGER_NUM + 1)

/**
 *  @brief      This must be the first field of all ITRACK instance objects.
 */
 
 #define STUDENTS_MAX_FRAME 66
 
typedef struct ITRACK_Obj 
{
    struct ITRACK_STUDENTS_Fxns *fxns;
}ITRACK_Obj;

/**
 *  @brief      Opaque handle to an ITRACK object.
 */
typedef struct ITRACK_Obj *ITRACK_Handle;

//typedef IALG_Cmd ITRACK_Cmd;
#define STU_MAX_CHECKNUM 20
/**
* @	���ص�����
*/
typedef struct _pixel_point_info
{	
	int16_t	x;		//x����
	int16_t	y;		//y����
}vertex_t;

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
* @	��������Ϣ�Ľṹ��
*/
typedef struct _student_trigger_info
{
	int16_t	x;
	int16_t	y;	
	int16_t	width;
	int16_t	height;
}student_trigger_info_t;

/**
* @ ����Զ����ٵľ�̬����
*/
typedef struct 	_stutrack_static_param
{
	int32_t	size;
	int16_t	video_width;			//������Ƶ�Ŀ�
	int16_t	video_height;			//������Ƶ�ĸ�
	int16_t	pic_width;				//�������ź�ͼ��Ŀ�
	int16_t	pic_height;				//�������ź��ͼ��ĸ�
}stutrack_static_param_t;
typedef struct Line_ab
{
	float a;
	float b;
}Line_ab;
/**
* @ ����Զ����ٵĶ�̬����
*/

typedef struct CamPos
{
	unsigned char x[4];
	unsigned char y[4];
	unsigned char z[4];
}stCamPos;
typedef struct 	_stutrack_dynamic_param
{
	int32_t	size;
	stCamPos CamPrePos[4];
	int16_t	x_offset;		//ͼ���x�������ű���,��IMG_X_offset_0
	int16_t	y_offset;		//ͼ���y�������ű���,��IMG_Y_offset_0
	short	cam_x_offset;		//���������ƫ��ֵ
	short	cam_y_offset;		//���������ƫ��ֵ
	int16_t	reset_time;		//ѧ��������೤ʱ��û�����µĴ�������Ϊ�������˵�ʱ��
	int16_t	trigger_num;	//��ʾ���������
	int16_t track_num_row;   //�趨һ��ѧ������
	student_trigger_info_t	*trigger_info;	//ѧ�������ٵĴ�������Ϣ
	student_trigger_info_t	*shield_info;	//ѧ�������ο���Ϣ
	student_trigger_info_t	*multitarget_info;	//ѧ�������ο���Ϣ
	int16_t	control_mode;	//��������,0Ϊ�Զ�,1Ϊ�ֶ�
	int16_t frame_onesecond;  //һ���ӻ�ȡԭʼ���ݵ�֡��
	int16_t video_type;    
	int16_t	sens;			//
	int16_t	message;		//��Ҫ�����趨����Щ�ߣ������Ƿ��˵������ߵ�,1:��ʾ���б�Ե��״̬��
							//2:����45�ȣ�90�ȣ�135�ȣ�180�ȵ��߻�������3:��ģ��;4:�����ٿ�;5:�����ٿ��ڵķ�ɫ��
	int16_t process_type;	//Ϊ0ʱ�����㷨����,Ϊ1ʱֻ����������,��������͸��ٿ�
	int16_t	track_point_num;//���ٿ�����ã�����м����㣬(������Щ�������ӳɿ�����˸�������)
	vertex_t	track_point[TRACK_AREA_POINT_MAX];	//����������x��y��������,�����������֮ǰͼ����˵
	control_init_type_e	reset_level;	//0��ʾ��Ҫ���³�ʼ��,1��ʾ��Ҫ���³�ʼ��
	int16_t	nTrackSwitchType;			//�����л�����,0��ʾ����,1��ʾֻ�л�
	int16_t	nStrategyNo;			//��λ��Ϣ,����ֻ�в����ķ�ʽѡ��
	
	Line_ab line_left;
	Line_ab line_right;
}StuITRACK_DynamicParams;

/**
* @ ����Զ����ٵ��������
*/
typedef struct 	_StuITRACK_Params
{
	int32_t	size;
	stutrack_static_param_t 	static_param;
	StuITRACK_DynamicParams	dynamic_param;
}StuITRACK_Params;

typedef struct ITRACK_InArgs 
{
    int32_t inBufSize;
	int32_t outBufSize;
	int32_t inBufValidBytes;
} ITRACK_InArgs;

/**
* @ ����Զ����ٵ��������
*/
typedef struct Rect
{
	int x;
	int y;
	int width;
	int height;
}Rect;


typedef struct	_stutrack_output_param
{
							
	int32_t up_num;							//Ϊ0��ʾû���κ�ѧ��վ��,Ϊ1��ʾ��1��������վ��
	Rect Up_List[STU_MAX_CHECKNUM];
	int64_t	time[STU_MAX_CHECKNUM];

}StuITRACK_OutArgs;


#ifdef __cplusplus
}
#endif


/*@}*/  /* ingroup */

#endif
/*
 *  @(#) ti.sdo.ce.examples.codecs.track_students; 1, 0, 0,204; 7-25-2009 21:30:29; /db/atree/library/trees/ce/ce-n11x/src/
 */

