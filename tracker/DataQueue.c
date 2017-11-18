#include<malloc.h>   
#include<stdio.h>   
#include "DataQueue.h"

/*构造一个空队列*/  
Queue *InitQueue()  
{  
	Queue *pqueue = (Queue *)malloc(sizeof(Queue));  
	if(pqueue!=NULL)
	{  
		 pqueue->head = NULL; 
		 pqueue->tail = NULL;  
		 pqueue->size = 0;  
	}  
	
	return pqueue;  
}  


/*销毁一个队列*/  
void DestroyQueue(Queue *pqueue)  
{  
	if(IsEmpty(pqueue) != 1) 
		ClearQueue(pqueue);  
		
	free(pqueue);
	pqueue = NULL;  
}  


/*清空一个队列*/  
void ClearQueue(Queue *pqueue)  
{  
	while(IsEmpty(pqueue) != 1)  
	{  
		int *n;
		PopQueue(pqueue, NULL, n);  
	}  	
}  


/*判断队列是否为空*/  
int IsEmpty(Queue *pqueue)  
{  
	if((pqueue->head == NULL) && (pqueue->tail == NULL) &&	(pqueue->size == 0))
		return 1;  
	else  
		return 0;  
}  

/*返回队列元素个数*/  
int GetSize(Queue *pqueue)  
{  
	 return pqueue->size;  
}  


/*返回队头元素*/  
PNode GetHead(Queue *pqueue, Item *pitem, unsigned int *dataLen)  
{  
	if((IsEmpty(pqueue) != 1) && (pitem != NULL))  
	{  
		//*pitem = pqueue->head->data; 
		memcpy(pitem, pqueue->head->data, pqueue->head->dataLen);
		*dataLen = pqueue->head->dataLen;		
	}  	

	return pqueue->head;  
}  

/*返回队尾元素*/  
PNode GetTail(Queue *pqueue, Item *pitem, unsigned int *dataLen)  
{  
	if((IsEmpty(pqueue) !=1 ) && (pitem != NULL))  
	{  
		//*pitem = pqueue->tail->data;  
		memcpy(pitem, pqueue->tail->data, pqueue->tail->dataLen);
		*dataLen = pqueue->tail->dataLen;
	}  

	return pqueue->tail;  
}

/*将新元素入队*/  
PNode PushQueue(Queue *pqueue, Item *item, unsigned int dataLen)  
{ 
	PNode pnode = (PNode)malloc(sizeof(Node));  
	if(pnode != NULL) 
	{  
		//pnode->data = item; 
		memcpy(pnode->data, item, dataLen); 
		pnode->dataLen = dataLen;
		pnode->next = NULL;  
		
		if(IsEmpty(pqueue))
		{  
			pqueue->head = pnode;  
		} 
		else 
		{  
			pqueue->tail->next = pnode; 
		}  

		pqueue->tail = pnode;  
		pqueue->size++;  
	}  

	return pnode;  
} 
 


/*队头元素出队列*/  
PNode PopQueue(Queue *pqueue, Item *pitem, unsigned int *dataLen)  
{  
	PNode pnode = pqueue->head;  
	if((IsEmpty(pqueue) != 1) && (pnode != NULL))
	{  
		if(pitem!=NULL)  
		{
			//*pitem = pnode->data;
			memcpy(pitem, pnode->data, pnode->dataLen);   
			*dataLen = pnode->dataLen;
		}
		
		pqueue->size--; 
		pqueue->head = pnode->next;  
		free(pnode);
		pnode = NULL;  
		
		if(pqueue->size == 0)  
			pqueue->tail = NULL;  
	}  
	
	return pqueue->head;  
}     


/*遍历队列并对各数据项调用visit函数*/  
void QueueTraverse(Queue *pqueue, void (*visit)())  
{  
	PNode pnode = pqueue->head;  
	int i = pqueue->size;  
	while(i--)  
	{  
		visit(pnode->data);
		pnode = pnode->next;  
	}  	       
}  

