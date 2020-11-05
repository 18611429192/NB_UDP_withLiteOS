#ifndef QUEUE
#define QUEUE
#define MAXSIZE 20

/*循环队列对的顺序存储结构*/
typedef struct
{
        char ais_data[MAXSIZE][100];  //队列中的二维数组元素 
        int front;    //头指针
        int rear;    //尾指针
}sqQueue;

/*初始化一个空队列*/
extern int InitQueue(sqQueue *Q);

/* 将队列清空 */
extern int ClearQueue(sqQueue *Q);

/*返回队列当前长度*/
extern int QueueLength(const sqQueue *Q);

/*循环队列入队操作  e为插入的元素*/
extern int EnQueue(sqQueue *Q,char* e);

/*若队列不为空,则删除Q中队头元素,用e值返回*/ 
extern int DeQueue(sqQueue *Q,char *e);

#endif