#include<malloc.h>   
#include<stdio.h>   
#include "DataQueue2.h"

/*构造一个空队列*/  
Queue2 *InitQueue2()  
{  
	Queue2 *pqueue = (Queue2 *)malloc(sizeof(Queue2));  
	if(pqueue!=NULL)
	{  
		 pqueue->head = NULL; 
		 pqueue->tail = NULL;  
		 pqueue->size = 0;  
	}  
	
	return pqueue;  
}  


/*销毁一个队列*/  
void DestroyQueue2(Queue2 *pqueue)  
{  
	if(IsEmpty2(pqueue) != 1) 
		ClearQueue2(pqueue);  
		
	free(pqueue);
	pqueue = NULL;  
}  


/*清空一个队列*/  
void ClearQueue2(Queue2 *pqueue)  
{  
	while(IsEmpty2(pqueue) != 1)  
	{  
		int *n;
		PopQueue2(pqueue, NULL, n);  
	}  	
}  


/*判断队列是否为空*/  
int IsEmpty2(Queue2 *pqueue)  
{  
	if((pqueue->head == NULL) && (pqueue->tail == NULL) &&	(pqueue->size == 0))
		return 1;  
	else  
		return 0;  
}  

/*返回队列元素个数*/  
int GetSize2(Queue2 *pqueue)  
{  
	 return pqueue->size;  
}  


/*返回队头元素*/  
PNode2 GetHead2(Queue2 *pqueue, Item2 *pitem, unsigned int *dataLen)  
{  
	if((IsEmpty2(pqueue) != 1) && (pitem != NULL))  
	{  
		//*pitem = pqueue->head->data; 
		memcpy((VIDEO_FRAME_INFO_S*)&pitem, (VIDEO_FRAME_INFO_S*)&pqueue->head->data, pqueue->head->dataLen);
		*dataLen = pqueue->head->dataLen;		
	}  	

	return pqueue->head;  
}  

/*返回队尾元素*/  
PNode2 GetTail2(Queue2 *pqueue, Item2 *pitem, unsigned int *dataLen)  
{  
	if((IsEmpty2(pqueue) !=1 ) && (pitem != NULL))  
	{  
		//*pitem = pqueue->tail->data;  
		memcpy((VIDEO_FRAME_INFO_S*)&pitem, (VIDEO_FRAME_INFO_S*)&pqueue->tail->data, pqueue->tail->dataLen);
		*dataLen = pqueue->tail->dataLen;
	}  

	return pqueue->tail;  
}

/*将新元素入队*/  
PNode2 PushQueue2(Queue2 *pqueue, Item2 *item, unsigned int dataLen)  
{ 
	PNode2 pnode = (PNode2)malloc(sizeof(Node2));  
	if(pnode != NULL) 
	{  
		//pnode->data = item; 
		memcpy((VIDEO_FRAME_INFO_S*)&pnode->data, (VIDEO_FRAME_INFO_S*)item, dataLen);
		printf("dataLen=%d\n",dataLen);
		pnode->dataLen = dataLen;
		pnode->next = NULL;  
		
		if(IsEmpty2(pqueue))
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
PNode2 PopQueue2(Queue2 *pqueue, Item2 *pitem, unsigned int *dataLen)  
{  
	PNode2 pnode = pqueue->head;  
	if((IsEmpty2(pqueue) != 1) && (pnode != NULL))
	{  
		
		if(pitem!=NULL)  
		{
			//*pitem = pnode->data;
			memcpy((VIDEO_FRAME_INFO_S*)pitem, (VIDEO_FRAME_INFO_S*)&pnode->data, pnode->dataLen);   
			*dataLen = pnode->dataLen;
			printf("dataLen=%d\n",pnode->dataLen);
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


/*队头元素出队列*/  
PNode2 PopTailQueue2(Queue2 *pqueue, Item2 *pitem, unsigned int *dataLen)  
{  
	PNode2 pnode = pqueue->tail;  
	if((IsEmpty2(pqueue) != 1) && (pnode != NULL))
	{  
		
		if(pitem!=NULL)  
		{
			//*pitem = pnode->data;
			memcpy((VIDEO_FRAME_INFO_S*)pitem, (VIDEO_FRAME_INFO_S*)&pnode->data, pnode->dataLen);   
			*dataLen = pnode->dataLen;

		}
		
		//pqueue->size--; 
		//pqueue->head = pnode->next;  
		ClearQueue2(pqueue);
		
		if(pqueue->size == 0)  
			pqueue->tail = NULL;  
	}  
	
	return pqueue->head;  
}   


/*遍历队列并对各数据项调用visit函数*/  
void QueueTraverse2(Queue2 *pqueue, void (*visit)())  
{  
	PNode2 pnode = pqueue->head;  
	int i = pqueue->size;  
	while(i--)  
	{  
		visit(pnode->data);
		pnode = pnode->next;  
	}  	       
}  

