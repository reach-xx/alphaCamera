#include<malloc.h>   
#include<stdio.h>   
#include "DataQueue2.h"

/*����һ���ն���*/  
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


/*����һ������*/  
void DestroyQueue2(Queue2 *pqueue)  
{  
	if(IsEmpty2(pqueue) != 1) 
		ClearQueue2(pqueue);  
		
	free(pqueue);
	pqueue = NULL;  
}  


/*���һ������*/  
void ClearQueue2(Queue2 *pqueue)  
{  
	while(IsEmpty2(pqueue) != 1)  
	{  
		int *n;
		PopQueue2(pqueue, NULL, n);  
	}  	
}  


/*�ж϶����Ƿ�Ϊ��*/  
int IsEmpty2(Queue2 *pqueue)  
{  
	if((pqueue->head == NULL) && (pqueue->tail == NULL) &&	(pqueue->size == 0))
		return 1;  
	else  
		return 0;  
}  

/*���ض���Ԫ�ظ���*/  
int GetSize2(Queue2 *pqueue)  
{  
	 return pqueue->size;  
}  


/*���ض�ͷԪ��*/  
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

/*���ض�βԪ��*/  
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

/*����Ԫ�����*/  
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


/*��ͷԪ�س�����*/  
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


/*��ͷԪ�س�����*/  
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


/*�������в��Ը����������visit����*/  
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

