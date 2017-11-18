
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
 
/*构造一个空队列*/  
Queue *InitQueue();  

/*销毁一个队列*/  
void DestroyQueue(Queue *pqueue);  
  
/*清空一个队列*/  
void ClearQueue(Queue *pqueue);  

/*判断队列是否为空*/  
int IsEmpty(Queue *pqueue);  

/*返回队列元素个数*/  
int GetSize(Queue *pqueue);  

/*返回队头元素*/  
PNode GetHead(Queue *pqueue, Item *pitem, unsigned int *dataLen);  

/*返回队尾元素*/  
PNode GetTail(Queue *pqueue, Item *pitem, unsigned int *dataLen);  

/*将新元素压入队列尾部*/  
PNode PushQueue(Queue *pqueue, Item *item, unsigned int dataLen);  

/*弹出队列头部元素*/  
PNode PopQueue(Queue *pqueue, Item *pitem, unsigned int *dataLen);  

/*遍历队列并对各数据项调用visit函数*/  
void QueueTraverse(Queue *pqueue, void (*visit)());  



#endif










