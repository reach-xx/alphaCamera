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
 
/*构造一个空队列*/  
Queue2 *InitQueue2();  

/*销毁一个队列*/  
void DestroyQueue2(Queue2 *pqueue);  
  
/*清空一个队列*/  
void ClearQueue2(Queue2 *pqueue);  

/*判断队列是否为空*/  
int IsEmpty2(Queue2 *pqueue);  

/*返回队列元素个数*/  
int GetSize2(Queue2 *pqueue);  

/*返回队头元素*/  
PNode2 GetHead2(Queue2 *pqueue, Item2 *pitem, unsigned int *dataLen);  

/*返回队尾元素*/  
PNode2 GetTail2(Queue2 *pqueue, Item2 *pitem, unsigned int *dataLen);  

/*将新元素压入队列尾部*/  
PNode2 PushQueue2(Queue2 *pqueue, Item2 *item, unsigned int dataLen);  

/*弹出队列头部元素*/  
PNode2 PopQueue2(Queue2 *pqueue, Item2 *pitem, unsigned int *dataLen);  

/*遍历队列并对各数据项调用visit函数*/  
void QueueTraverse2(Queue2 *pqueue, void (*visit)());  



#endif










