#ifndef PROCESS_H
#define PROCESS_H

//相关头文件声明 
#include "utils.h"
#include "hardware.h"

//hardware.h头文件中的相关类声明 
class Cpu;//cpu
class Memory;//内存
class Time;

//process.h相关类声明 
class Instruc;//指令类 
class Process;//进程类 
class PCB_Table;//进程表类 
class LinkQueue;//链队列类 

//------------------------------------------指令类-------------------------------------------------- 
class Instruc
{
	public:
		int ID;//指令ID
		int State;//指令状态标志，1为用户太操作，2为I/O操作 
		int time;//单条指令运行时间 
		int addr;//指令地址,ID*1000+Random[0,999] 
		
		Instruc();//构造函数 
		void Init(int id,Time &timeq);//构造函数 
		
		void changeID(int temp);  //修改指令ID 
		void changeState(int temp);  //修改指令状态 

	protected:		
};


//-----------------------------------------进程类--------------------------------------------------------- 
class Process
{
	public:	
		//属性 
		int ProID;//进程编号 
		int JobId;//进程对应作业ID 
		int Priority;//进程优先数 
		int InTimes;//进程创建时间 
		int ProState;//进程状态 
		int RunTimes;//进程运行时间 
		int PSW;//保存该进程当前执行的指令编号 
		int InstrucNum;//进程包含的指令数目 
		Instruc *instruc_arry;//进程包含的指令数组指针 
		int alltime;//进程周转时间 
		
		//方法 
		Process();
		Process(int id,int intime,int instrucnum,Time &timeq);
		void proinit(int m_ID,int m_priority,int m_intimes);//进程的初始化 
		int profork(PCB_Table &L,LinkQueue &ready);//进程的创建 
		int prokill(PCB_Table &L,LinkQueue &finish);//进程的终止
		int problock(PCB_Table &L,LinkQueue &finish,Cpu CPU);//进程的阻塞
		int procall(PCB_Table &L,LinkQueue &ready,LinkQueue &block);//进程的唤醒
		//int prorun(PCB_Table &L,LinkQueue &ready,LinkQueue &block);//进程的运行 
		
		//void init_instrucID(); //修正指令ID 
				
	protected:				
};

//-----------------------------------------PCB表类----------------------------------------------- 
class PCB_Table
{
	public:
		//属性 	
		Process *process; // 
		int length;   //表长 
		
		//方法 
		PCB_Table(); //构造函数 
		
		void InsertPCB(Process x,int i);//在PCB表的第i个数据元素之前插入一个进程x (PCB表的0位空出)
		void SearchProID(int id,Process &temp);//根据进程ID，查找PCB表中的某个进程 ，返回给同类的参数  
		void DeleteProID(int id);//根据进程ID，在PCB表中删除相应进程 
		void PrintTable(char *a);//打印表 
				
	protected:
};


//-----------------------------------------链队列类-------------------------------------------------
//链队列结点类型定义
typedef struct QNode  
{
	Process data;
	struct QNode *next;
}QNode,*QueuePtr;

class LinkQueue
{
	public:
		//属性		
		QueuePtr front; //头指针，data不存放元素
		QueuePtr rear;  //尾指针，data存放最后一个元素
		
		//方法 
		LinkQueue();

		void EnQueue(Process e);  //入队列
		void DeQueue(Process &e); //出队列
		void GetTop(Process &e);  //取队列头元素
		void PrintQueue(); //遍历队列
		int LenQueue();   //取队列长度
		void MinPriority(Process &e);//每次选取最小优先级的进程
		void sort();
		//void DeleteQueue(int v);
		
	protected:
};


#endif

