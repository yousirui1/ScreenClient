#include "queue.h"

void Init_Queue(QUEUE *pQueue,unsigned char *pucBuf,unsigned int uiMaxBufSize)
{
  pQueue->uiFront=0;
  pQueue->uiRear=0;
  pQueue->pBuf=pucBuf;
  pQueue->uiMaxBufSize=uiMaxBufSize;
  pQueue->uiBufOffset=0;
}

unsigned char En_Queue(QUEUE *pQueue,unsigned char *ucpData,unsigned int uiSize,unsigned char ucType)
{
  unsigned int uiPos;

  uiPos=((pQueue->uiRear+1)&0x7F);   //0x7F MAXQueueCnt 128
  if(pQueue->uiFront==uiPos) { //��������

    return 1;
  }
  else
  {
    if((pQueue->uiBufOffset+uiSize)>pQueue->uiMaxBufSize)
      pQueue->uiBufOffset=0;
    pQueue->stIndex[pQueue->uiRear].uiSize=uiSize;
    pQueue->stIndex[pQueue->uiRear].ucType=ucType;
    pQueue->stIndex[pQueue->uiRear].pBuf=&(pQueue->pBuf[pQueue->uiBufOffset]);
    memcpy(&pQueue->pBuf[pQueue->uiBufOffset],ucpData,uiSize);
    pQueue->uiBufOffset+=uiSize;
    pQueue->uiRear=uiPos;
    return 0;
  }
}

QUEUE_INDEX * De_Queue(QUEUE *pQueue)
{
  return &(pQueue->stIndex[pQueue->uiFront]);
}

unsigned char De_QueuePos(QUEUE *pQueue)
{
  pQueue->uiFront=((pQueue->uiFront+1)&0x7F);
  return 0;
}

unsigned char Empty_Queue(QUEUE *pQueue)
{
  if(pQueue->uiRear==pQueue->uiFront)
    return 0x1;
  else
    return 0x0;
}

unsigned char Full_Queue(QUEUE *pQueue)
{
    unsigned int  uiPos;

  uiPos=((pQueue->uiRear+1)&0x7F);   //0x1F MAXQueueCnt 32
  if(pQueue->uiFront==uiPos) //��������
    return 0;
  else
    return 1;
}
