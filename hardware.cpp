#include "hardware.h"


//--------------------------------------------------Cpu类函数------------------------------------------------------------ 

//构造函数 
Cpu::Cpu() 
{
	PC = 1;//程序计数器
	IR = 0;//指令寄存器 
	PSW = 0;//0为空闲，1为忙碌 ,//状态寄存器
}

//CPU现场保护函数
int Cpu::Sceneprotection(Process &e)
{
	e.PSW = IR;
	PSW = 0;
	return 0;
}

//CPU现场恢复函数
int Cpu::Scenerevover(Process e)
{
	PC = e.ProID;
	IR = e.PSW;
	PSW = 1;
	return 0;
}


//---------------------------------------------Memory类函数内存(用户区)------------------------------------------------------------ 

//构造函数 
Memory::Memory()
{
	//内存基本信息 
	SpaceSize = 100*1000;//单位B 
	BlockNum = 100;
	BlockSize = 1000;
	
	//内存管理信息
	Block *temp = new Block[100]; 
	for(int i = 0; i < 100;i++)//初始化内存物理块表
	{
		temp[i].InitBlock(i);
		block_table.InsertBlock(temp[i],i+1);
	} 
	FreeSpace = 100;
	UsedSpace = 0; 
}

//分配资源
/*
返回值：1 ——>分配成功
		0 ——> 分配失败 
*/ 
int Memory::AllocationSpace(int size,int JobId,Page_Table &page_table)
{
	if((size-5) <= FreeSpace)//如果空闲块数满足，则分配 
	{
		//初始化页表
		for(int k = 0;k < size;k++)
		{
			Page page;
			if(k < size -5)
			{
				page.Init(k,1);//选择作业的部分调入内存
				page_table.InsertPage(page,k+1); 
			}
			else
			{
				page.Init(k,0);
				page_table.InsertPage(page,k+1); 
			}
		} 
		
		int ii = 1;
		int j = 1;
		size = size -5;
		while(size > 0)
		{
			if(block_table.block[ii].BlockState == 0)//找出空闲的块 
			{
				block_table.block[ii].BlockState = 1;//置占用标志 
				block_table.block[ii].OwnerPro = JobId;//该块分配给的作业 
				page_table.page[j].BlockId = block_table.block[ii].BlockId;//按找到的位置块号计算对应页框号，填入进程的页表 
				FreeSpace--;//从空闲块数中减去本次占用块数 
				UsedSpace++;
				size--;
				j++;
				//相应的页对应 
			}
			ii++;
		} 
		return 1;//分配成功 
	}
	else//如果空闲块数不足则令进程等待 
	{
		//进程等待 
		return 0;//分配失败 
	} 
}

//回收资源 
int Memory::RecycleSpace(int size,int ProId)
{
	int i = 0;
	while(size > 0)
	{
		if(block_table.block[i].OwnerPro == ProId)//找出需要归还的块 
		{
			block_table.block[i].BlockState = 0;//置占用标志为0 
			block_table.block[i].OwnerPro = -1;//清空占有进程标志位 
			FreeSpace++;//从空闲块数中减去本次占用块数 
			UsedSpace--;
			size--;
		}
		i++;
	} 
}

//打印内存
void Memory::Print(ofstream &file)
{
	file<<"剩余空间（块）："<<FreeSpace<<"\r\n"<<"已用空间（块）："<<UsedSpace<<"\r\n";
	file<<"内存物理块情况（第一个数：0为空闲，1为占用;第二个数：-1表示未分配给任何进程，其他数字表示占有改块的进程ID）:"<<"\r\n";
	for(int i = 1;i <= block_table.length;i++)
	{
		file<<block_table.block[i].BlockId<<"("<<block_table.block[i].BlockState<<","<<block_table.block[i].OwnerPro<<")";
		file<<"    ";
		if(i % 10 == 0)
		{
			file<<"\r\n";
		}
	} 
	file<<"\r\n";
} 


//--------------------------------------------------MMU类函数------------------------------------------------------------ 

//构造函数 
MMU::MMU() 
{
	page_table_addr = NULL;	
}

void MMU::go(ofstream &file,Page_Table &page_table,int addr,int &pageid,int &pianyi,int &paddr)
{
	
	PageTableAddr(page_table);
	BreakAddr(addr,pageid,pianyi);	
	cout<<"逻辑地址分解完成:页号为"<<pageid<<",偏移地址为"<<pianyi<<endl; 
	file<<"逻辑地址分解完成:页号为"<<pageid<<",偏移地址为"<<pianyi<<endl; 
	VisitPageTable(file,pageid,pianyi,paddr);
}
//管理页表基址寄存器 
//(当进程被调度到CPU上运行时，操作系统自动把此进程PCB中的页表起始地址装入硬件页表基址寄存器，
//此后，进程开始运行并访问某个逻辑地址，MMU开始工作)
void MMU::PageTableAddr(Page_Table &page_table)
{
	page_table_addr = &page_table;
}

//分解逻辑地址
void MMU::BreakAddr(int addr,int &pageid,int &pianyi)
{
	pageid = addr/1000;
	pianyi = addr%1000;
}

//访问页表
void MMU::VisitPageTable(ofstream &file,int pageid,int pianyi,int &paddr)
{
	Page temp;
	page_table_addr->SearchPageId(pageid,temp);//页号为索引搜索页表
	if(temp.Dwell == 1)//如果页表命中,可送出页框号，并与页内位移拼接成物理地址
	{
		paddr = temp.BlockId * 1000 + pianyi;
		cout<<"页表命中，物理地址为"<<paddr<<endl;
		file<<"页表命中，物理地址为"<<paddr<<endl;
	}
	else
	{
		MissingPage(pageid);//缺页异常 
		cout<<"页表未命中！产生缺页中断！"<<endl;
		cout<<"替换成功！"<<endl;
		cout<<"重新运行中断指令！"<<endl;
		file<<"页表未命中！产生缺页中断！"<<endl;
		file<<"替换成功！"<<endl;
		file<<"中断指令运行成功！"<<endl;
	}
}

//发出异常
void MMU::MissingPage(int id)
{
	//page_table_addr->SearchPageId(id,temp);//根据页号，搜索外页号 
	page_table_addr->page[id].Dwell = 1;//调入内存 
	page_table_addr->page[id].BlockId = id*987%100;
}















