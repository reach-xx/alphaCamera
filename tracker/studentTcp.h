#ifndef _STUDENTTCP_
#define _STUDENTTCP_

#define STUDENT_PORT 5556
#define STUDENT_RIGHTSIDE_PORT 5558
typedef enum _switch_cmd_type
{
	SWITCH_TEATHER				= 1,		//�л���ʦ������
	SWITCH_STUDENTS				= 3,		//�л�ѧ��������
	SWITCH_VGA					= 255,		//�л�VGA����
	SWITCH_BLACKBOARD			= 4,		//�л��������������
	SWITCH_STUDENTS_PANORAMA 	= 5,		//�л�ѧ��ȫ�������
	SWITCH_STUDENTS_2			= 5,		//�л�ѧ��������
	SWITCH_TEACHER_PANORAMA		= 6,		//�л���̨�����ȫ��
	SWITCH_NUM
}switch_cmd_type_e;

extern void studentTrace(void *param);
extern int studentTracerMove(unsigned char data);
extern void SENDDATATOTEACHER(void *pParam);
#endif
