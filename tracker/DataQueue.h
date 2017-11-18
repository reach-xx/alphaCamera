
#ifndef DATAQUEUE_H
#define DATAQUEUE_H

#define MAX_DATA_NUM 100

#define TEXT_SZ (150000 * 1)  
typedef unsigned char Item;  
typedef struct node * PNode;  

typedef struct node  
{  
	Item data[TEXT_SZ * 10];  
	unsigned int dataLen;
	PNode next;  
}Node;  

typedef struct  
{  
	PNode head;  
	PNode tail;  
	int size;  
}Queue;  
 
/*����һ���ն���*/  
Queue *InitQueue();  

/*����һ������*/  
void DestroyQueue(Queue *pqueue);  
  
/*���һ������*/  
void ClearQueue(Queue *pqueue);  

/*�ж϶����Ƿ�Ϊ��*/  
int IsEmpty(Queue *pqueue);  

/*���ض���Ԫ�ظ���*/  
int GetSize(Queue *pqueue);  

/*���ض�ͷԪ��*/  
PNode GetHead(Queue *pqueue, Item *pitem, unsigned int *dataLen);  

/*���ض�βԪ��*/  
PNode GetTail(Queue *pqueue, Item *pitem, unsigned int *dataLen);  

/*����Ԫ��ѹ�����β��*/  
PNode PushQueue(Queue *pqueue, Item *item, unsigned int dataLen);  

/*��������ͷ��Ԫ��*/  
PNode PopQueue(Queue *pqueue, Item *pitem, unsigned int *dataLen);  

/*�������в��Ը����������visit����*/  
void QueueTraverse(Queue *pqueue, void (*visit)());  



#endif










