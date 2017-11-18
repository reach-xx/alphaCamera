#ifndef BROADCAST__H
#define BROADCAST__H

typedef enum AuthorType_e
{
	AUTHOR_TYPE_STUDENT,
	AUTHOR_TYPE_TEACHER,
	AUTHOR_TYPE_NUM
}AuthorType;

int StartBroadcastServer(AuthorType aType, char *pIp);
void ResetIpInfo(char *pIp);



#endif