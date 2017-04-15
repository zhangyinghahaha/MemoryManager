#ifndef JOB_H
#define JOB_H

//相关头文件声明 
#include "utils.h"
#include "process.h"
#include "hardware.h"

//process.h相关类声明 
class Instruc;//指令类 
class Process;//进程类 
class PCB_Table;//进程表类 
class LinkQueue;//链队列类 

//job.h相关类声明 
class JCB;//JCB(作业控制块)
class JCB_Table;//JCB表
class Pool_Queue;//后备队列
class Job_Scheduler;//作业调度程序 

//hardware.h相关类声明 
class Cpu;//cpu
class Memory;//内存
class Time;
class MMU;


//JCB
class JCB
{
	public:	
		//属性: 
		//作业情况 
		char UserName[10];//用户名,默认为zhangying 
		int JobId;//作业号
		int ProId;//作业对应进程
		//资源需求 
		int RunTime;//CPU运行估计时间
		int DeadLine;//截止时间
		int MemorySize;//所需内存容量(以页为单位，10——20页)
		//资源使用情况 
		int InTime;//进入系统时间
		int StarTime;//开始运行时间
		int MemoryAddress;//内存地址 
		 	
		//方法: 
		JCB(); //构造函数
		void Init(int jobid,int intime,int memorysize);//初始化函数 
				
	protected:				
};

//JCB_Table
class JCB_Table
{
	public:	
		//属性: 
		JCB *jcb; // 存储空间基址 
		int length;   //表长 
		
		//方法:
		JCB_Table();
		
		void InsertJCB(JCB x,int i);//在JCB表的第i个数据元素之前插入一个JCBx (JCB表的0位空出)
		void SearchJobId(int id,JCB &temp);//根据作业号，查找JCB表中的某个作业，返回给同类的参数  
		void DeleteJobId(int id);//根据作业号，在JCB表中删除相应JCB
		void PrintTable(ofstream &file);//打印作业表 
		
				
	protected:				
};

//Pool_Queue(后备队列)
//链队列结点类型定义
typedef struct QNode_J  
{
	JCB data;
	struct QNode_J *next;
}QNode_J,*QueuePtr_J;

class Pool_Queue
{
	public:	
		//属性 
		QueuePtr_J front; //头指针，data不存放元素
		QueuePtr_J rear;  //尾指针，data存放最后一个元素
		
		//方法 
		Pool_Queue();

		void EnQueue(JCB e);  //入队列
		void DeQueue(JCB &e); //出队列
		void GetTop(JCB &e);  //取队列头元素
		void PrintQueue(); //遍历队列
		int LenQueue();   //取队列长度
		void MinPriority(JCB &e);//每次选取最小优先级的进程
		void sort();
		//void DeleteQueue(int v);
		
		
				
	protected:				
};

/*
2、批处理作业的调度；作业调度程序需完成以下任务：
	a.选择作业。先来先服务算法
	b.分配资源。作业调度程序与存储管理程序进行通讯，为作业分配所需的资源。
	c.创建进程。每当选中作业且将其装入内存时，系统就为此作业创建应用进程，生成PCB及对应进程实体，这些进程将在低级调度控制下占用处理器运行。（可同时装入多个作业并启动运行）
	d.后续处理。作业正常结束或出错终止时，作业调度程序要做好作业撤离和完善工作，如打印输出信息、回收各种资源、撤销JCB等；同时，启动作业调度程序选择新作业进入内存；
*/
//Job_Scheduler(作业调度程序） 
class Job_Scheduler
{
	public:	
		//属性 
		
		
		//方法 
		Job_Scheduler();
		int go(Pool_Queue &pool_queue,JCB &temp,Memory &memory,Page_Table &page_table,Time &timeq,PCB_Table &pcb_table,LinkQueue &ready,JCB_Table &jcb_table);
		void ChooseJob(Pool_Queue &pool_queue,JCB &temp);//选择作业(先来先服务算法),从后备队列中取出一个JCB给temp 
		int AllocatinResource(Memory &memory,JCB jcb,Page_Table &page_table,Pool_Queue &pool_queue,JCB_Table &jcb_table);//分配资源 
		int CreateProcees(JCB temp,Time &timeq,PCB_Table &pcb_table,LinkQueue &ready);//创建进程
		int FollowWork();//后续处理 
				
	protected:				
};

#endif


