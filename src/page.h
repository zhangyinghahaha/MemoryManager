#ifndef PAGE_H
#define PAGE_H

//相关头文件声明 
#include "utils.h"

//page.h相关类声明 
class Block;//物理块
class Block_Table;//物理块表 
class Page;//页表项
class Page_Table;//页表 

//-----------------------------------------页表项类--------------------------------------------------------- 
class Page
{
	public:
		//属性 
		int PageId;//页号,从0开始依次编号 
		int BlockId;//内存块号，与页面大小相等，从0依此开始编号 
		int Dwell;//驻留标志位，0表示不在内存，1表示在内存
		int Yinyong;//引用位
		int Modifi;//修改位，0表示未被修改，1表示被修改 
		
		//方法 
		Page();
		void Init(int id,int dwell);//初始化函数 
		
				
	protected:				
};

//-----------------------------------------页表类--------------------------------------------------------- 
class Page_Table
{
	public:
		//属性 
		Page *page; //基址 
		int length;   //表长 
		
		//方法 
		Page_Table();
		
		void InsertPage(Page x,int i);//在Page_Table表的第i个数据元素之前插入一个页表项x (页表的0位空出)
		void SearchPageId(int id,Page &temp);//根据页号，查找页表中的某个页表项 ，返回给同类的参数  
		void DeletePageId(int id);//根据页号，在P页表中删除相应页表项 
		void PrintTable(char *a);//打印表 
						
	protected:				
};

//-----------------------------------------物理块类--------------------------------------------------------- 
class Block
{
	public:
		//属性 
		int BlockId;//物理块ID
		int BlockState;//物理块分配状态（0表示空闲，1表示占有）
		int OwnerPro;//分配给的进程ID 
		
		//方法 
		Block();
		
		void InitBlock(int id);//物理块初始化 
				
	protected:				
};

//-----------------------------------------物理块表类----------------------------------------------- 
class Block_Table
{
	public:
		//属性 
		
		Block *block; // 基址 
		int length;   //表长 
		
		//方法 
		Block_Table(); //构造函数 
		
		void InsertBlock(Block x,int i);//在Block_Table表的第i个数据元素之前插入一个Block x (Block_Table表的0位空出)
		void SearchBlockID(int id,Block &temp);//根据物理块ID，查找物理块表中的某个物理块，返回给同类的参数  
		void DeleteBlockID(int id);//根据物理块ID，在物理块表中删除相应物理块 
		
				
	protected:
};


#endif
