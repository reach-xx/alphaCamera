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
 *  ======== track_ti_priv.h ========
 *  Internal vendor specific (TI) interface header for TRACK_STUDENTS
 *  algorithm. Only the implementation source files include
 *  this header; this header is not shipped as part of the
 *  algorithm.
 *
 *  This header contains declarations that are specific to
 *  this implementation and which do not need to be exposed
 *  in order for an application to use the TRACK_STUDENTS algorithm.
 */
#ifndef TRACK_TI_PRIV_
#define TRACK_TI_PRIV_


//#include <ti/xdais/xdas.h>

//#include <stdlib.h>
//#include <xdc/std.h>
#include <string.h>
#include "itrack_students.h"


/**
* @ Y,Cb,Crֵ�ṹ��
*/
typedef struct 	_students_yuv_value_info
{
	int16_t	y_value;		//y��ֵ
	int16_t	cb_value;		//cb��ֵ
	int16_t	cr_value;		//cr��ֵ
}students_yuv_value_info_t;

/**
* @ ���ߵ���Ϣ
*/
typedef struct _line_info
{
	int32_t	start_x;	//�ߵ���ʼ��x����
	int32_t	start_y;	//�ߵ���ʼ��y����
	int32_t	end_x;		//�ߵĽ�����x����
	int32_t	end_y;		//�ߵĽ�����y����
}line_info_t;

typedef struct
{
  int16_t min_x;
  int16_t min_y;
  int16_t max_x;
  int16_t max_y;
} rect_t;
typedef struct
{
  int16_t head_x;
  int16_t head_y;

} Head_t;

typedef struct
{
rect_t* rect;
Head_t* head;
int16_t  rectNum;


}Rect_Head;
//#define STU_MAX_CHECKNUM 20
#define STU_CHECK_TIME 200
typedef struct tstucheck_rect
{
	int16_t nCheckActiveTime;   //���������Ч����
	int16_t	nCheckFlag;
	int16_t	nCheckTime;
	int16_t	nCheckX[STU_CHECK_TIME];
	int16_t	nCheckY[STU_CHECK_TIME];
	int16_t nCheckW;
	int16_t nCheckH;
	int32_t nCheckNum;
	//int16_t nCheckMoveH[STU_CHECK_TIME];
	//int16_t nCheckMoveW[STU_CHECK_TIME];
}TStuCheckRect;
/**
* @ �ƶ������Ϣ�ṹ��
*/


typedef struct _T_STU_GLOBAL_OBJ {
	
	uint8_t *students_src_y_buf[2];   //������ź�Ybuffer����,ÿ10֡���һ��ԭʼ���� 
	
	uint8_t *students_dst_y_buf[3] ;		 //�����һ֡Ybuffer����
	
	uint8_t *students_time_count[9];//0��ŵ�ǰ֡��n�����ǰ��n֡
	uint8_t *students_dst_count[9];//ʵ�ʲ����ڴ�ָ�룬��students_time_count���ʹ�ã�Ϊ�˼����ڴ濽��
	uint8_t * students_active_count[9];
	uint8_t *Students_Area_vertex; //��ʾ���Ƿ����ڼ������
	uint8_t* students_active_at9count;
	int32_t nSrcBufWidth;//�ɼ�ԭʼͼ������ݰڷ�buf�Ŀ��
	int32_t nSrcBufHeight;//�ɼ�ԭʼͼ������ݰڷ�buf�Ŀ��
	int32_t nFrameNum;
	int32_t nCopyPos;
	int32_t nCopy10FrameNum;
	int32_t nCopyFramNum;
	Rect_Head* student_Head[9];
	TStuCheckRect t_check_point[STU_MAX_CHECKNUM];
}T_STU_GLOBAL_OBJ;

typedef struct _StuTrackHand {
    StuITRACK_Params	input_param;	//�������
    T_STU_GLOBAL_OBJ t_stu_global_obj;
	StuITRACK_OutArgs output_param;
}StuTrackHand;
#endif
/*
 *  @(#) ti.sdo.ce.examples.codecs.track_students; 1, 0, 0,204; 7-25-2009 21:30:30; /db/atree/library/trees/ce/ce-n11x/src/
 */

