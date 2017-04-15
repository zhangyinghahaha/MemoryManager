#include "process.h"


//-----------------------------------------指令类函数实现模块-----------------------------------------------------------------------
//构造函数 
Instruc::Instruc()
{
	ID = 0;       //ID都初始化为0 
	State = 1;       //Instruc_State初始化为1 
	time = 0;   //当Instruc_State=0或1时, Instruc_Times=产生[10-40]之间的随机10ms倍数的整数
	addr = 0;
}

void Instruc::Init(int id,Time &timeq)
{
	ID = id;       //ID从1开始 
	State = timeq.RandNum(1,2);       //Instruc_State初始化为1或2
	if(State == 1)//当Instruc_State=0或1时, Instruc_Times=产生[10-40]之间的随机10ms倍数的整数；
	{
		time = timeq.RandNum(1,4)*10;
	}
	else//当 Instruc_State=2 时, Instruc_Times=50，用户进程发生 I/O 阻塞请求，并假设完成 I/O 数据通信时间为 50ms，之后可唤醒
	{
		time = 50;
	}
	addr = ID*1000 +	timeq.RandNum(0,999); //生成指令地址 
}

//修改指令ID 
void Instruc::changeID(int temp)
{
	ID = temp;
}

//修改指令状态 
void Instruc::changeState(int temp)
{
	State = temp;
}


//--------------------------------------------进程类函数实现模块---------------------------------------------------------------------
//构造函数 
Process::Process()
{
	ProID = 0;      //进程编号（ProID）,如果没有输入ID，默认置为0 
	
	Priority = 5;	//进程优先数（Priority）,如果没有输入,默认置为5
	
	InTimes =0;   //进程创建时间（InTimes）：程序运行从0开始计时，在2s(即2000ms)内随机生成时间递增序列。[与进程ID相关]
	
	RunTimes = 0;  //进程运行时间（RunTime）:统计记录进程当前已运行了多少时间，此字段开始时为空，进程运行过程中不断保存和记录。
	
	InstrucNum = 20; //进程包含的指令数目（InstrucNum）：用[5-20]以内的随机整数产生；
	
	PSW = 1;  //PSW中保存该进程当前执行的指令编号。例如，1表示正在执行第1条指令 
	
	ProState = 1;  //进程状态（ProState）:0为阻塞态，1为就绪态，2为运行态。默认置为就绪态 
	
	instruc_arry = new Instruc[InstrucNum];  //随机产生N条指令 
	
	alltime = 0; 
}

Process::Process(int id,int intime,int instrucnum,Time &timeq)
{
	ProID = id;      //进程编号（ProID）,如果没有输入ID，默认置为0 
	
	JobId = id;
	
	Priority = 5;	//进程优先数（Priority）,如果没有输入,默认置为5
	
	InTimes = intime;   //进程创建时间（InTimes）：程序运行从0开始计时，在2s(即2000ms)内随机生成时间递增序列。[与进程ID相关]
	
	RunTimes = 0;  //进程运行时间（RunTime）:统计记录进程当前已运行了多少时间，此字段开始时为空，进程运行过程中不断保存和记录。
	
	InstrucNum = instrucnum; //进程包含的指令数目（InstrucNum）：用[5-20]以内的随机整数产生；
	
	PSW = 1;  //PSW中保存该进程当前执行的指令编号。例如，1表示正在执行第1条指令 
	
	ProState = 1;  //进程状态（ProState）:0为阻塞态，1为就绪态，2为运行态。默认置为就绪态 
	
	instruc_arry = new Instruc[InstrucNum];  //产生N条指令 
	
	for(int j = 0;j < InstrucNum;j++)//初始化各个指令 
	{
		instruc_arry[j].Init(j+1,timeq);
	}
	
	alltime = 0; 
}

//进程的初始化 
void Process::proinit(int m_ID,int m_priority,int m_intimes)
{
	ProID = m_ID;   
	Priority = m_priority;
	InTimes = m_intimes; 
} 

//进程的创建
int Process::profork(PCB_Table &L,LinkQueue &ready)
{
	Process temp;
	temp = *this;	
	ready.EnQueue(temp);//移入就绪队列
	cout<<"进程"<<temp.ProID<<"已移入就绪队列！"<<endl;	
	//通知某些模块，监测程序	
	L.InsertPCB(temp,ProID);//移入PCB表 
	cout<<"进程"<<temp.ProID<<"已移入PCB表！"<<endl;		
	return 0;	
}

//进程的终止
int Process::prokill(PCB_Table &L,LinkQueue &finish)
{
	Process temp;
	//从相应的队列中移除
	finish.DeQueue(temp);
	//移出PCB表
	L.DeleteProID(ProID);
	//转向系统调度工作 
	return ProID;
}

//进程的阻塞
int Process::problock(PCB_Table &L,LinkQueue &block,Cpu CPU)
{
	//保护现场信息
	CPU.Sceneprotection(L.process[ProID]);
	//修改进程状态
	L.process[ProID].ProState = 0;
	//移入相关队列 
	block.EnQueue(L.process[ProID]);
	return 0;
}

//进程的唤醒 
int Process::procall(PCB_Table &L,LinkQueue &ready,LinkQueue &block)
{
	//从阻塞队列移到就绪队列
	Process e;
	block.DeQueue(e);
	ready.EnQueue(e); 
	//修改进程状态 
	L.process[ProID].ProState = 1;
	return 0;
}

//------------------------------------------PCB类函数实现模块--------------------------------------------------------------------------
//构造函数 
PCB_Table::PCB_Table()
{
	process = new Process[20];   //分配初始化进程表空间 
	length = 0;   // PCB中进程个数初始化为0
}

//在PCB表的第i个数据元素之前插入一个进程x (PCB表的0位空出)
void PCB_Table::InsertPCB(Process x,int i)
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
			process[k+1] = process[k];
		}	
		process[k+1] = x;
	}
} 

//根据进程ID，在PCB表中删除相应进程 
void PCB_Table::DeleteProID(int id)
{
	int i;
	int flag=0;
	for(i = 1;i < length + 1;i++)
	{
		if(process[i].ProID == id)
		{
			int k = i;
			for(k=k+1;k < length + 1;k++)
			{
				process[k-1] = process[k];
			}
			length--;
			flag = 1;
			break;
		}
	}
	if(flag == 0)
	{
		cout<<"PCB表中没有该ID的进程"<<endl;
	}
} 

//根据进程ID，查找PCB表中的某个进程 ,返回给同类的参数 
void PCB_Table::SearchProID(int id,Process &temp)
{
	int i;
	int flag = 0;
	for(i = 1;i < length + 1;i++)
	{
		if(process[i].ProID == id)
		{
			temp = process[i];
			flag = 1;
			break;
		}
	}
	if(flag == 0)
	{
		cout<<"PCB表中没有该ID的进程"<<endl;
	}
} 

//打印PCB表 
void PCB_Table::PrintTable(char *a)
{
	ofstream file;
	file.open(a,ios::trunc); //ios::trunc表示在打开文件前将文件清空
	//int ProID;//进程编号 
	//	int JobId;//进程对应作业ID 
	//	int Priority;//进程优先数 
	//	int InTimes;//进程创建时间 
	//	int ProState;//进程状态 
	//	int RunTimes;//进程运行时间 
	//	int InstrucNum;//进程包含的指令数目 
	//	Instruc *instruc_arry;//进程包含的指令数组指针 
	//	int alltime;//进程周转时间 
	file<<"ProID"<<setw(15)<<"JobId"<<setw(15)<<"InTimes"<<setw(15)<<"ProState"<<setw(15)<<"RunTime"<<setw(15)<<"InstrucNum"
			<<setw(15)<<"PSW"<<setw(15)<<"Instruc_ID"<<setw(20)<<"Instruc_State"<<setw(20)<<"Instruc_Times(ms)"<<setw(20)<<"Instruc_Addr";
	for(int i = 1;i <= length;i++)
	{
		file<<"\r\n"<<process[i].ProID<<setw(15)<<process[i].JobId<<setw(15)<<process[i].InTimes<<setw(15)<<
			process[i].ProState<<setw(15)<<process[i].RunTimes<<setw(15)<<process[i].InstrucNum<<setw(18)<<process[i].PSW<<setw(15);
		//输出指令的信息
		for(int j=0;j<process[i].InstrucNum;j++)
		{
			file<<process[i].instruc_arry[j].ID<<setw(15)<<process[i].instruc_arry[j].State<<setw(15)
				<<process[i].instruc_arry[j].time<<setw(25)<<process[i].instruc_arry[j].addr;
			if(j<process[i].InstrucNum-1)//输出前面的空格
			{
				file<<"\r\n"<<setw(109);
			}	
		}
	}
	file.close();
} 

//-----------------------------------------链队列类实现函数-------------------------------------------------------------------------
LinkQueue::LinkQueue()
{
	rear = new QNode;  //为队列的头尾结点分配空间
	front = rear;
	if (!front )  return ;
	front->next = NULL;
}

void LinkQueue::EnQueue(Process e)  //入队列
{
	QueuePtr p;
	p = new QNode;
	if (!p) return;
	p->data = e; 
	p->next = NULL;
	rear ->next = p;
	rear = p;
}

void LinkQueue::DeQueue(Process &e) //出队列
{
	if(front == rear)
	{
		cout<<"该队列为空队列chu"<<endl;
	}
	QueuePtr p;
	p = front ->next ;
	e = p->data ;
	//printf("出队列完成，被删除的元素为%d\n",e);
	front ->next = p->next ;
	if (rear == p ) rear = front ; //如果链队列原本只有一个元素，要同时修改队尾指针
	delete p;
}

void LinkQueue::GetTop(Process &e)  //取队列头元素
{
	if (rear ==  front ) 
	{
		cout<<"该队列为空队列"<<endl;
	}
	QueuePtr p;
	p = front ->next ;
	e = p->data;
}

void LinkQueue::PrintQueue() //遍历队列
{
	if (front == rear)
	{
		cout<<"该队列为空队列"<<endl;
		return;
	}
	
	QueuePtr p;
	p = front->next ;
	cout<<"该队列中的ProID如下：";
	while(p)
	{
		cout<<p->data.ProID<<",";
		p = p->next ;
	}
	cout<<endl;
}

int LinkQueue::LenQueue()  //取队列长度
{
	if (front == rear )
	{
		//cout<<"该队列为空"<<endl;
		return 0; 
	}
	int i=0;
	QueuePtr p;
	p = front ->next ;
	while(p) 
	{
		i++;
		p = p->next ;
	}
	return i;
}

void LinkQueue::MinPriority(Process &e)
{
	QueuePtr p,min,f;
	if(front == rear)
	{
		cout<<"队列为空!"<<"\n";
	}
	else
	{
		min = front->next;//第一个节点 
		f = front;//p节点的前一个节点 
		p = min;//第二个节点 
		while(p->next)
		{
			f = p;	
			p = p->next;
			if(p->data.Priority < min->data.Priority)	
			{
				min = p;
			}		
		}
		f->next = min->next;
		e=min->data;
		delete min;
	}	
	
}

void LinkQueue::sort()
{
	
	QueuePtr p,r;

	p =front ;r=p->next->next;
	if(front==rear)
		cout<<"队列为空!"<<"\n";
	for(int i=0;i<6;i++)
	while(p->next->next)
	{
		if(p->next->data.Priority>r->data.Priority)
		{
			QueuePtr q=new QNode;
			q->data=r->data;
			//p->next=q->next;
			p->next=q;	
			q->next=p->next;
			q->next->next=r->next;
			delete r;
		}
		p=p->next;
		r=p->next->next;
	}
	
	
}

/*void LinkQueue::DeleteQueue(int v)
{
	QueuePtr p,q;
	p=front;
	if(front == rear)
		cout<<"队列为空！"<<"\n";
	while(p->next)
	{
		if(p->next->data.ProID==v)
		{
			q=p->next;
			p->next=q->next;
			delete q;
			break;
		}
		else
			p=p->next;
	}

	
}*/




