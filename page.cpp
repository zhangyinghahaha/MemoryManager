#include "page.h" 

//-----------------------------------------页表项类----------------------------------------------------------
//构造函数
Page::Page()
{
	PageId = -1;//页号,从0开始依次编号 
	BlockId = -1;//内存块号，与页面大小相等，从0依此开始编号 
	Dwell = 0;//驻留标志位，0表示不在内存，1表示在内存
	Yinyong = 0;//引用位
	Modifi = 0;//修改位，0表示未被修改，1表示被修改 
} 

void Page::Init(int id,int dwell)
{
	PageId = id;
	Dwell = dwell;
}
//------------------------------------------页表类函数实现模块--------------------------------------------------------------------------
//构造函数 
Page_Table::Page_Table()
{
	page = new Page[25];   //初始化页表空间(初始25) 
	length = 0;   // PCB中进程个数初始化为0
}

//在Page_Table表的第i个数据元素之前插入一个页表项x (页表的0位空出)
void Page_Table::InsertPage(Page x,int i)
{
	int k;
	if(i < 1 || i > length + 1)  //判断插入位置是否合法，不合法输出错误提示
	{
		cout<<"插入位置错误！！"<<endl;
	}
	else
	{
		length++;
		for(k = length -1;k > i - 1;k--)
		{
			page[k+1] = page[k];
		}	
		page[k+1] = x;
	}
} 

//根据页号，在P页表中删除相应页表项 
void Page_Table::DeletePageId(int id)
{
	int i;
	int flag=0;
	for(i = 1;i < length + 1;i++)
	{
		if(page[i].PageId == id)
		{
			int k = i;
			for(k=k+1;k < length + 1;k++)
			{
				page[k-1] = page[k];
			}
			length--;
			flag = 1;
			break;
		}
	}
	if(flag == 0)
	{
		cout<<"页表中没有该页号"<<endl;
	}
} 

//根据页号，查找页表中的某个页表项 ，返回给同类的参数 
void Page_Table::SearchPageId(int id,Page &temp)
{
	int i;
	int flag = 0;
	for(i = 1;i < length + 1;i++)
	{
		if(page[i].PageId == id)
		{
			temp = page[i];
			flag = 1;
			break;
		}
	}
	if(flag == 0)
	{
		cout<<"页表中没有页号"<<endl;
	}
} 

void Page_Table::PrintTable(char *a)
{
	ofstream file;
	file.open(a,ios::trunc); //ios::trunc表示在打开文件前将文件清空
		//int PageId;//页号,从0开始依次编号 
	//	int BlockId;//内存块号，与页面大小相等，从0依此开始编号 
	//	int Dwell;//驻留标志位，0表示不在内存，1表示在内存
	//	int Yinyong;//引用位
	//	int Modifi;//修改位，0表示未被修改，1表示被修改 
	file<<"PageId"<<setw(15)<<"BlockId"<<setw(15)<<"Dwell"<<setw(15)<<"Yinyong"<<setw(15)<<"Modifi";
	for(int i = 1;i <= length;i++)
	{
		file<<"\r\n"<<page[i].PageId<<setw(15)<<page[i].BlockId<<setw(15)<<page[i].Dwell<<setw(15)
		<<page[i].Yinyong<<setw(15)<<page[i].Modifi<<setw(15);
	}
	file.close();
}

//-----------------------------------------物理块类---------------------------------------------------------
//构造函数
Block::Block()
{
	BlockId = -1;//物理块ID
	BlockState = 0;//物理块分配状态（0表示空闲，1表示占有）
	OwnerPro = -1;//分配给的进程ID 
} 

//物理块初始化 
void Block::InitBlock(int id)
{
	BlockId = id;
} 


//------------------------------------------Block_Table类函数实现模块--------------------------------------------------------------------------
//构造函数 
Block_Table::Block_Table()
{
	block = new Block[200];   //初始化Block_Tabel空间(初始200块) 
	length = 0;   // PCB中进程个数初始化为0
}

//在Block_Table表的第i个数据元素之前插入一个Block x (Block_Table表的0位空出)
void Block_Table::InsertBlock(Block x,int i)
{
	int k;
	if(i < 1 || i > length + 1)  //判断插入位置是否合法，不合法输出错误提示
	{
		cout<<"插入位置错误！！"<<endl;
	}
	else
	{
		length++;
		for(k = length -1;k > i - 1;k--)
		{
			block[k+1] = block[k];
		}	
		block[k+1] = x;
	}
} 

//根据物理块ID，在物理块表中删除相应物理块 
void Block_Table::DeleteBlockID(int id)
{
	int i;
	int flag=0;
	for(i = 1;i < length + 1;i++)
	{
		if(block[i].BlockId == id)
		{
			int k = i;
			for(k=k+1;k < length + 1;k++)
			{
				block[k-1] = block[k];
			}
			length--;
			flag = 1;
			break;
		}
	}
	if(flag == 0)
	{
		cout<<"Block_Table表中没有该ID的物理块"<<endl;
	}
} 

//根据物理块ID，查找物理块表中的某个物理块，返回给同类的参数
void Block_Table::SearchBlockID(int id,Block &temp)
{
	int i;
	int flag = 0;
	for(i = 1;i < length + 1;i++)
	{
		if(block[i].BlockId == id)
		{
			temp = block[i];
			flag = 1;
			break;
		}
	}
	if(flag == 0)
	{
		cout<<"Block_Table表中没有该ID的物理块"<<endl;
	}
} 


