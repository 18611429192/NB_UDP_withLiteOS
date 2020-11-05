#include "queue.h"
#include<stdio.h>
#include<math.h>
#include <string.h> 


/*初始化一个空队列*/
int InitQueue(sqQueue *Q)
{
        Q->front=0;        //结构体指针 
        Q->rear=0;
       return 1;
}
 
/* 将队列清空 */
int ClearQueue(sqQueue *Q)
{
        Q -> front = 0;
        Q -> rear = 0;
        return 1;
} 
 
/*返回队列当前长度*/
int QueueLength(const sqQueue *Q)
{
        return ((Q->rear - Q->front+ MAXSIZE)%MAXSIZE);
       /* 返回队列中元素的个数 */
}
 
/*循环队列入队操作  e为插入的元素*/
int EnQueue(sqQueue *Q,char* e)
{	
        if((Q->rear+1)%MAXSIZE==Q->front)  /*队列已满*/
        {
                return 0;
        }
       strcpy(Q->ais_data[Q->rear],e);    //将元素e赋值给队尾 
       Q->rear=(Q->rear+1)%MAXSIZE;       //如果指向最后一位则转回到数组头 
       return 1;
}
 
/*若队列不为空,则删除Q中队头元素,用e值返回*/ 
int DeQueue(sqQueue *Q,char *e)
{
       if (Q->front==Q->rear)    /*队列为空判断*/
       {
               return 0;
       }
        strcpy(e, Q->ais_data[Q->front]);
        Q->front=(Q->front+1)%MAXSIZE;
        return 1;
}