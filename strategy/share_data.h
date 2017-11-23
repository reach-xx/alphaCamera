#ifndef SHARE_DATA__H
#define SHARE_DATA__H

#include <stdbool.h>
#include "auto_track.h"


/**
* @ ����ģ����������
*/
typedef struct _strategy_input_info
{
	/**��ʦ�������**/
	int iTeatcherNum;						//��̨����⵽Ŀ��ĸ�����0��ʾû��Ŀ�ꣻ1��ʾ��⵽һ��Ŀ�ꣻ2��ʾ��⵽���Ŀ��
	cam_preset_position_info_t cTeacherCamCoord;		//��ʦ����̨��������꣬3519�ñ�����ʱû�ã�����ʧЧ״̬
	bool bIsNeedWaitTeacherCamTrans;			//�Ƿ���Ҫ�ȴ���ʦ��д���ƶ�
	int iTeacherMoveState;					//��ʦ��д��ͷ���ƶ�״̬��0��ʾ��ֹ��1��ʾ�ƶ�
	unsigned long long iTeacherTargetRefreshTime;		//��ʦĿ�괥����ˢ��ʱ��
	bool bIsNewTeacherTarget;							//�Ƿ�Ϊ�µ���ʦĿ��

	/**ѧ���������**/
	int iStudentStandupNum;					//����ѧ����������0��ʾδ��⵽ѧ��������1��ʾ��⵽һ��ѧ��������2��ʾ��⵽���Ŀ������
	cam_preset_position_info_t cStudentCamCoord;		//ѧ������̨��������꣬3519�ñ�����ʱû�ã�����ʧЧ״̬
	bool bIsHaveSendStudentCamTransMsg;			//�Ƿ�����̨�����ת������Ϣ��¼����������3519�ñ�����ʱû�ã�����ʧЧ״̬
	bool bIsNeedWaitStudentCamTrans;							//�Ƿ���Ҫ�ȴ�ѧ����д���ƶ�
		
	/**����������**/
	int	iBlackboardTriggerFlag1;			//�Ƿ��ڰ��������־��1Ϊ�ڰ�������0Ϊ���ڰ�������
	int	iBlackboardTriggerFlag2;			//�Ƿ��ڰ��������־��1Ϊ�ڰ�������0Ϊ���ڰ�������

}strategy_input_info_t;


void SetBlackboardDetectResult(int _iBlackboardTriggerFlag, int _iBlackboardNo);

void SetStudentDetectResult(int _iStudentStandupNum, cam_preset_position_info_t _cStudentCamCoord);
void SetTeacherDetectResult(int _iTeacherNum, cam_preset_position_info_t _cTeacherCamCoord, bool _bIsNewTeacherTarget);
void SetTeacherMoveState(int _iMoveState);





















#endif