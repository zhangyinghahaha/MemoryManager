#ifndef HARDWARE_H
#define HARDWARE_H

//相关头文件声明 
#include "utils.h"
#include "process.h"
#include "page.h" 

//process.h头文件中相关类声明 
class Instruc; 
class PCB_Table;
class LinkQueue;
class Process;

//page.h头文件相关类声明 
class Block;//物理块
class Block_Table;//物理块表 
class Page;//页表项
class Page_Table;//页表 

//hardware.h相关类声明 
class Cpu;//cpu
class Memory;//内存
class Time;
class MMU;


//CPU
class Cpu
{
	public:
		//属性 
		int PC;   //程序计数器 
		int IR;   //指令寄存器 
		int PSW;  //状态寄存器
		
		//方法 
		Cpu();    //构造函数 
		int Sceneprotection(Process &e);//CPU现场保护函数
		int Scenerevover(Process e);//CPU现场恢复函数 
	protected:
		 
};

//内存(用户区)
class Memory
{
	public:
	//属性
	//内存基本信息 
	int SpaceSize;//内存空间大小 (默认100*1000B)
	int BlockNum;//物理块数量（100）
	int BlockSize;//物理块大小（1000） 
	
	//内存管理信息
	Block_Table block_table;//内存物理块表 
	int FreeSpace;//剩余空间
	int UsedSpace;//已用空间 
		
	//方法 
	Memory();
	int AllocationSpace(int size,int JobId,Page_Table &page_table);//分配资源
	int RecycleSpace(int size,int ProId);//回收资源 
	void Print(ofstream &file);//将物理块表输出到文件 
}; 


//MMU
class MMU
{
	public:
		//属性 
		Page_Table *page_table_addr;//硬件页表基址寄存器 
		
		//方法 
		MMU();    //构造函数 
		void go(ofstream &file,Page_Table &page_table,int addr,int &pageid,int &pianyi,int &paddr);
		void PageTableAddr(Page_Table &page_table);//管理页表基址寄存器 
		void BreakAddr(int addr,int &pageid,int &pianyi);//分解逻辑地址
		void VisitPageTable(ofstream &file,int pageid,int pianyi,int &paddr);//访问页表
		void MissingPage(int id);//发出异常 
		
	protected:
};

//-----------------------------------Time类---------------------------------------------------------
class Time
{
private:
	struct 
	{
		int hour;
		int min;
		int sec;
	}time_hmc;
	public:
		//获取当前时间函数
		void GetTime()
		{
			time_t timep;
			struct tm *p;
			time(&timep);  //转换成秒数
			p = localtime(&timep);  //时间年月日等结构体
			time_hmc.hour = p->tm_hour;
			time_hmc.min = p->tm_min;
			time_hmc.sec = p->tm_sec;
		}
		
		//随机数生成
		int RandNum(int start,int end)
		{
			
			return start+(rand()%(end - start + 1));
		}
		
		void test()
		{
			cout<<time_hmc.hour<<":"<<time_hmc.min<<":"<<time_hmc.sec<<"\t"<<"\n";  //多线程之中使用endl会有清空缓存区冲突
		}
};

#endif


