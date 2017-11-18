#include "mpi_vpss.h"


#ifndef DATAQUEUE2_H
#define DATAQUEUE2_H

#define MAX_DATA_NUM2 100

typedef VIDEO_FRAME_INFO_S Item2;  
typedef struct node2 * PNode2;  

typedef struct node2  
{  
	Item2 data;  
	unsigned int dataLen;
	PNode2 next;  
}Node2;  

typedef struct  
{  
	PNode2 head;  
	PNode2 tail;  
	int size;  
}Queue2;  
 
/*����һ���ն���*/  
Queue2 *InitQueue2();  

/*����һ������*/  
void DestroyQueue2(Queue2 *pqueue);  
  
/*���һ������*/  
void ClearQueue2(Queue2 *pqueue);  

/*�ж϶����Ƿ�Ϊ��*/  
int IsEmpty2(Queue2 *pqueue);  

/*���ض���Ԫ�ظ���*/  
int GetSize2(Queue2 *pqueue);  

/*���ض�ͷԪ��*/  
PNode2 GetHead2(Queue2 *pqueue, Item2 *pitem, unsigned int *dataLen);  

/*���ض�βԪ��*/  
PNode2 GetTail2(Queue2 *pqueue, Item2 *pitem, unsigned int *dataLen);  

/*����Ԫ��ѹ�����β��*/  
PNode2 PushQueue2(Queue2 *pqueue, Item2 *item, unsigned int dataLen);  

/*��������ͷ��Ԫ��*/  
PNode2 PopQueue2(Queue2 *pqueue, Item2 *pitem, unsigned int *dataLen);  

/*�������в��Ը����������visit����*/  
void QueueTraverse2(Queue2 *pqueue, void (*visit)());  



#endif










