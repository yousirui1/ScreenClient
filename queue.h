#ifndef QUEUE_H
#define QUEUE_H
#include <stdio.h>
#include <string.h>

#pragma pack(4)

#define MAX_QUEUESIZE 128


typedef struct {
  unsigned int uiSize;
  unsigned char *pBuf;
  unsigned char ucType;
}QUEUE_INDEX;


typedef struct {
  unsigned int uiFront;
  unsigned int uiRear;
  unsigned int uiMaxBufSize;
  unsigned int uiBufOffset;
  unsigned char *pBuf;
  QUEUE_INDEX stIndex[MAX_QUEUESIZE];
}QUEUE;

void Init_Queue(QUEUE *pQueue,unsigned char *pucBuf,unsigned int uiMaxBufSize);
QUEUE_INDEX * De_Queue(QUEUE *pQueue);
unsigned char De_QueuePos(QUEUE *pQueue);
unsigned char En_Queue(QUEUE *pQueue,unsigned char *ucpData,unsigned int uiSize,unsigned char ucType);
unsigned char Empty_Queue(QUEUE *pQueue);
unsigned char Full_Queue(QUEUE *pQueue);
class queue
{
public:
    queue();
};

#endif // QUEUE_H
