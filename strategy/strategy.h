#ifndef STRATEGY__H
#define STRATEGY__H

#include "share_data.h"
#include "common.h"



//��ͷ���л�״̬
typedef enum _switch_state
{
	SWITCH_FORBID	= 1,	//��ֹ�л�
	SWITCH_FREE	= 2,		//�����л�
	SWITCH_FORCE	= 3,	//ǿ���л�
}switch_state_e;

//��ͷ���л�״̬
typedef enum _cam_trans_state
{
	NO_CAM_TRANS		= 0,		//����Ҫ��ת��
	TEACHER_CAM_TRANS	= 1,	//��ʦ����̨ת��
	STUDENT_CAM_TRANS	= 2,	//ѧ������̨ת��
	
}cam_trans_state_e;

//��ͷ������
typedef enum _cam_type
{
	TEACHER_PANORAMA								= 0,		//��ʦȫ����������һ����ͷδ֪
	
	TEACHER_PANORAMA_WITH_NEXT_TEACHER_FEATURE		= 1,		//��ʦȫ����������һ����ͷΪ��ʦ��д, 3519��״̬��������
	TEACHER_PANORAMA_WITH_NEXT_STUDENT_FEATURE		= 2,		//��ʦȫ����������һ����ͷΪѧ����д, 3519��״̬��������
	TEACHER_PANORAMA_WITH_NEXT_STUDENT_PANORAMA		= 3,		//��ʦȫ����������һ����ͷΪѧ��ȫ��, 3519��״̬��������

	TEACHER_FEATURE									= 4,		//��ʦ��д
	STUDENT_FEATURE									= 5,		//ѧ����д
	STUDENT_PANORAMA								= 6,		//ѧ��ȫ��	
	BLACK_BOARD1									= 7,		//���龵ͷ
	
	CAM_TYPE_NUM
}cam_type_e;

typedef enum _STRATEGY_
{
    STRATEGY_2 = 0,     //��λ1 ��ʦ + ѧ��
    STRATEGY_4_1 = 1,     //��λ1 ��ʦ+ѧ��+����+ѧ��ȫ��
    STRATEGY_5_1 = 2,   //5��λ������
    STRATEGY_5_2 = 3,   //5��λ˫����
	STRATEGY_3_1 = 4,   //��λ1 :��ʦ+ ѧ�� +����
	STRATEGY_3_2 = 5,   //��λ2 :��ʦ+ ѧ�� +��ʦȫ��
	STRATEGY_4_2 = 6,   //��λ :��ʦ+ѧ��+��ʦȫ��+ѧ��ȫ��

	STRATEGY_3_3 = 7,   //��λ3 :��ʦ+ѧ��+ѧ��ȫ��
	STRATEGY_4_3 = 8,   //��λ3 :��ʦ+ ѧ�� + ���� + ��ʦȫ��
	STRATEGY_6 = 10,   //6��λ˫����  ��ʦ+ѧ��+��ʦȫ��+ѧ��ȫ��+����1+����2
	STRATEGY_6_2STU = 11, //6��λ˫ѧ�� ��ʦ+ѧ��+��ʦȫ��+ѧ��ȫ��+����1+ѧ��2
	STRATEGY_3_4 = 12, // 3��λ: ��ʦ+��ʦȫ��+ѧ��ȫ��

	//�������ֲ�֧��
	STRATEGY_4_STU_2_ROOM_1 = 50,   //��λ :��ʦ+ ѧ�� 1+ѧ��2+ѧ��ȫ����ͬһ������
	STRATEGY_4_STU_2_ROOM_2 = 51,   //��λ :��ʦ+ ѧ�� 1+ѧ��2+ѧ��ȫ���ڲ�ͬ����


	STRATEGY_3_ONLYSWITCH = 100,     //ֻ�в��� :��ʦ1 +  ѧ��3+����4 
	STRATEGY_4_ONLYSWITCH = 101   //ֻ�в��� :��ʦ1 +  ѧ��3(ǰ���ѧ��)+����4 +ѧ��5(�����ѧ��)
 
} STRATEGY;

/**
* @	���λ����ʱ����Ϣ�� 
*/
typedef struct _strategy_timeinfo
{
	int iTeacherPanoramaWithNextTeacherFeatureKeepTime; 		//��ʦȫ��(������һ��Ϊ��ʦ��д)�ı���ʱ��, 3519��״̬��������
	int iTeacherPanoramaWithNextStudentFeatureKeepTime; 		//��ʦȫ��(������һ��Ϊѧ����д)�ı���ʱ��, 3519��״̬��������
	int iTeacherPanoramaWithNextStudentPanoramaKeepTime; 		//��ʦȫ��(������һ��Ϊѧ��ȫ��)�ı���ʱ��, 3519��״̬��������

	int iTeacherPanoramaMinKeepTime; 		//��ʦȫ����С�ı���ʱ��
	int iTeacherFeatureMinKeepTime; 		//��ʦ��д��С�ı���ʱ��

	int iStudentPanoramaMinKeepTime; 		//ѧ��ȫ����С�ı���ʱ��
	int iStudentPanoramaMaxKeepTime; 		//ѧ��ȫ����С�ı���ʱ��, 3519��״̬��������
	int iStudentFeatureMinKeepTime; 		//ѧ����д��С�ı���ʱ��
	int iStudentFeatureMaxKeepTime; 		//ѧ����д���ı���ʱ��, 3519��״̬��������

	int iBlackboardMinKeepTime;				//���龵ͷ��С����ʱ��

}strategy_timeinfo_t;

/**
* @	���λ����״̬��Ϣ��
*/
typedef struct _strategy_info
{
	unsigned long long iTeacherPanoramaStartTime;		//��ʦȫ����ͷ��ʼ��ʱ��
	unsigned long long iTeacherFeatureStartTime;		//��ʦ��д��ͷ��ʼ��ʱ��
	
	unsigned long long iStudentPanoramaStartTime;		//ѧ��ȫ����ͷ��ʼ��ʱ��
	unsigned long long iStudentFeatureStartTime;		//ѧ����д��ͷ��ʼ��ʱ��

	unsigned long long iBlockboardStartTime;			//���龵ͷ��ʼ��ʱ��

	cam_type_e	cLastCamType;			//���һ����ͷ������
	switch_cmd_type_e sLastCmdType;		//�����л���ͷ
	switch_cmd_type_e sOutCmdType;		//����Ĵ��л��ľ�ͷ
	switch_state_e sSwitchState;		//��ǰ��ͷ�л�״̬
	cam_trans_state_e cCamTransState;	//��̨�����ת��״̬

}strategy_info_t;



/**
* @ ��ѯ��̨�������Ԥ��λ��Ϣ
*/
int query_cam_coord_info_by_teacher(int _preset_position_type);

/**
* @ ��ѯ��̨�������Ԥ��λ��Ϣ
*/
int query_cam_coord_info_by_student(int iStudentNo, int _preset_position_type);

/**
* @ �����л�����ģ��
*/
int strategy_start();

/**
* @ �����л�����ģ��
*/
void strategy_reset();


/**
* @ ��ȡ��ʦ��¼��֮�������״̬
*/
int get_teacher_to_encode_connect_state();

/**
* @ ��ȡ��ʦ��ѧ��֮�������״̬
*/
int get_teacher_to_student_connect_state(int index);

/**
* @ ��ȡ��ʦ�����֮�������״̬
*/
int get_teacher_to_blackboard_connect_state(int index);

/**
* @ ���ͻ�λ�����¼��������
*/
void SendStrategyNoToEncode();

























#endif