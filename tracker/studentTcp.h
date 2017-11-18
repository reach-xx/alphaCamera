#ifndef _STUDENTTCP_
#define _STUDENTTCP_

#define STUDENT_PORT 5556
#define STUDENT_RIGHTSIDE_PORT 5558
typedef enum _switch_cmd_type
{
	SWITCH_TEATHER				= 1,		//切换老师机命令
	SWITCH_STUDENTS				= 3,		//切换学生机命令
	SWITCH_VGA					= 255,		//切换VGA命令
	SWITCH_BLACKBOARD			= 4,		//切换板书摄像机命令
	SWITCH_STUDENTS_PANORAMA 	= 5,		//切换学生全景摄像机
	SWITCH_STUDENTS_2			= 5,		//切换学生机命令
	SWITCH_TEACHER_PANORAMA		= 6,		//切换讲台摄像机全景
	SWITCH_NUM
}switch_cmd_type_e;

extern void studentTrace(void *param);
extern int studentTracerMove(unsigned char data);
extern void SENDDATATOTEACHER(void *pParam);
#endif
