#include "job.h"

/*
-----------------------------------JCB类-----------------------------------------
*/

//构造函数
JCB::JCB()
{
	strcpy(UserName,"zhangying");//用户名,默认为zhangying 
	JobId = 0;//作业号
	
	//资源需求 
	RunTime = 0;//CPU运行估计时间
	DeadLine = 0;//截止时间
	MemorySize = 0;//所需内存容量
	
	//资源使用情况 
	InTime = 0;//进入系统时间
	StarTime = 0;//开始运行时间
	MemoryAddress = 0;//内存地址 
	
	ProId = 0;//作业对应进程 
	
}

//初始化函数 
void JCB::Init(int jobid,int intime,int memorysize)
{
	JobId = jobid;//作业号
	
	//资源需求 
	MemorySize = memorysize;//所需内存容量
	
	//资源使用情况 
	InTime = intime;//进入系统时间
	//MemoryAddress = memoryaddress;//内存地址
	cout<<"有新的作业进入系统.."<<endl; 
	cout<<"已为新作业建立作业控制块，作业ID为："<<jobid<<endl; 
} 

/*
-----------------------------------JCB_Table类-----------------------------------------
*/

//构造函数
JCB_Table::JCB_Table()
{
	jcb = new JCB[20];   //分配初始化作业表空间 
	length = 0;   // JCB表中作业个数初始化为0
} 

void JCB_Table::InsertJCB(JCB x,int i)
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
			jcb[k+1] = jcb[k];
		}	
		jcb[k+1] = x;
	}
	cout<<"作业ID为"<<x.JobId<<"的作业已添入作业表！"<<endl; 
}

//根据作业号，查找JCB表中的某个作业，返回给同类的参数  
void JCB_Table::SearchJobId(int id,JCB &temp)
{
	int i;
	int flag = 0;
	for(i = 1;i < length + 1;i++)
	{
		if(jcb[i].JobId == id)
		{
			temp = jcb[i];
			flag = 1;
			break;
		}
	}
	if(flag == 0)
	{
		cout<<"JCB表中没有该ID的进程"<<endl;
	}
}

//根据作业号，在JCB表中删除相应JCB
void JCB_Table::DeleteJobId(int id)
{
	int i;
	int flag=0;
	for(i = 1;i < length + 1;i++)
	{
		if(jcb[i].JobId == id)
		{
			int k = i;
			for(k=k+1;k < length + 1;k++)
			{
				jcb[k-1] = jcb[k];
			}
			length--;
			flag = 1;
			break;
		}
	}
	if(flag == 0)
	{
		cout<<"JCB表中没有该ID的作业"<<endl;
	}
}

//打印作业表 
void JCB_Table::PrintTable(ofstream &file)
{
	//ofstream file;
	//file.open(a,ios::trunc); //ios::trunc表示在打开文件前将文件清空
	//char UserName[10];//用户名,默认为zhangying //资源需求 //int RunTime;//CPU运行估计时间//int DeadLine;//截止时间
	//int MemorySize;//所需内存容量(以页为单位，10——20页)//资源使用情况 //int InTime;//进入系统时间	//int StarTime;//开始运行时间
	//int MemoryAddress;//内存地址 //int ProId;//作业对应进程 
	file<<"作业号"<<setw(15)<<"用户名"<<setw(20)<<"进程号"<<setw(15)<<"估计时间"<<setw(20)<<"截止时间"<<setw(20)<<"所需内存"
			<<setw(20)<<"进入系统时间"<<setw(20)<<"开始运行时间"<<setw(20)<<"内存地址"<<endl;
	for(int i = 1;i <= length;i++)
	{
		file<<jcb[i].JobId<<setw(15)<<jcb[i].UserName<<setw(20)<<jcb[i].ProId<<setw(15)<<jcb[i].RunTime<<setw(20)<<jcb[i].DeadLine<<setw(20)<<jcb[i].MemorySize
			<<setw(20)<<jcb[i].InTime<<setw(20)<<jcb[i].StarTime<<setw(20)<<jcb[i].MemoryAddress<<endl;
	}
	file<<"\r\n";
//	file.close();
} 

/*
-----------------------------------Pool_Queue(后备队列)类-----------------------------------------
*/
  
//构造函数
Pool_Queue::Pool_Queue()
{
	rear = new QNode_J;  //为队列的头尾结点分配空间
	front = rear;
	if (!front )  return ;
	front->next = NULL;
}

//入队列
void Pool_Queue::EnQueue(JCB e)
{
	QueuePtr_J p;
	p = new QNode_J;
	if (!p) return;
	p->data = e; 
	p->next = NULL;
	rear->next = p;
	rear = p;
	cout<<"作业ID为"<<e.JobId<<"的作业"<<"已移入后备队列！"<<endl;
}

//出队列
void Pool_Queue::DeQueue(JCB &e)
{
	if(front == rear)
	{
		//cout<<"该队列为空队列"<<endl;
		return ;
	}
	QueuePtr_J p;
	p = front ->next ;
	e = p->data ;
	//printf("出队列完成，被删除的元素为%d\n",e);
	front ->next = p->next ;
	if (rear == p ) rear = front ; //如果链队列原本只有一个元素，要同时修改队尾指针
	delete p;
}

//取队列头元素
void Pool_Queue::GetTop(JCB &e)
{
	if (rear ==  front ) 
	{
		//cout<<"该队列为空队列"<<endl;
		return ;
	}
	QueuePtr_J p;
	p = front ->next ;
	e = p->data;
}

//遍历队列
void Pool_Queue::PrintQueue()
{
	if (front == rear)
	{
		//cout<<"该队列为空队列"<<endl;
		return;
	}
	
	QueuePtr_J p;
	p = front->next ;
	cout<<"该队列中的作业ID如下：";
	while(p)
	{
		cout<<p->data.JobId<<",";
		p = p->next ;
	}
	cout<<endl;
}

//取队列长度
int Pool_Queue::LenQueue()
{
	if (front == rear )
	{
		//cout<<"该队列为空"<<endl;
		return 0; 
	}
	int i=0;
	QueuePtr_J p;
	p = front ->next ;
	while(p) 
	{
		i++;
		p = p->next ;
	}
	return i;
}
	
/*
-----------------------------------Job_Scheduler(作业调度程序）类-----------------------------------------
*/
//构造函数	
Job_Scheduler::Job_Scheduler()
{
	
}
/*
2、批处理作业的调度；作业调度程序需完成以下任务：
	a.选择作业。先来先服务算法
	b.分配资源。作业调度程序与存储管理程序进行通讯，为作业分配所需的资源。
	c.创建进程。每当选中作业且将其装入内存时，系统就为此作业创建应用进程，生成PCB及对应进程实体，这些进程将在低级调度控制下占用处理器运行。（可同时装入多个作业并启动运行）
	d.后续处理。作业正常结束或出错终止时，作业调度程序要做好作业撤离和完善工作，如打印输出信息、回收各种资源、撤销JCB等；同时，启动作业调度程序选择新作业进入内存；
*/


int Job_Scheduler::go(Pool_Queue &pool_queue,JCB &temp,Memory &memory,Page_Table &page_table,Time &timeq,PCB_Table &pcb_table,LinkQueue &ready,JCB_Table &jcb_table)
{
	ChooseJob(pool_queue,temp);//a.选择作业。
	
	int i = AllocatinResource(memory,temp,page_table,pool_queue,jcb_table);//	b.分配资源。
	if(i == 1)
	{
		Job_Scheduler::CreateProcees(temp,timeq,pcb_table,ready);//c.创建进程。
		return 1;
	}
	else
	{
		return 0;
	}
	
	
}

//选择作业(先来先服务算法),从后备队列中取出一个JCB给temp 
void Job_Scheduler::ChooseJob(Pool_Queue &pool_queue,JCB &temp)
{
	if(pool_queue.front == pool_queue.rear)
	{
		cout<<"后备队列为空队列！"<<endl;
	}
	else
	{
		pool_queue.GetTop(temp);//从后备队列中选择一个作业调入内存
		cout<<"作业号为："<<temp.JobId<<"的作业已调入内存！"<<endl; 
	}
	
}

//分配资源（作业调度程序与存储管理程序进行通讯，为作业分配所需的资源。） 
int Job_Scheduler::AllocatinResource(Memory &memory,JCB jcb,Page_Table &page_table,Pool_Queue &pool_queue,JCB_Table &jcb_table)
{
	int i = memory.AllocationSpace(jcb.MemorySize,jcb.JobId,page_table);
	if(i == 1)
	{
		cout<<"已为作业"<<jcb.JobId<<"分配所需内存资源!"<<endl;
		pool_queue.DeQueue(jcb);
		jcb_table.jcb[jcb.JobId].ProId = jcb.JobId;//改变JCB表中相关信息 
		jcb_table.jcb[jcb.JobId].StarTime = jcb_table.jcb[jcb.JobId].InTime + 10;
		return 1;
	}
	else
	{
		cout<<"作业"<<jcb.JobId<<"正在回滚!"<<endl;
		cout<<"内存分配已满，请等待！"<<endl;
		cout<<endl;
		return 0;
	}

	
}

//创建进程(每当选中作业且将其装入内存时，系统就为此作业创建应用进程，生成PCB及对应进程实体，这些进程将在低级调度控制下占用处理器运行。)
int Job_Scheduler::CreateProcees(JCB temp,Time &timeq,PCB_Table &pcb_table,LinkQueue &ready)
{
	cout<<"已为作业"<<temp.JobId<<"创建对应进程"<<temp.JobId<<"以及相应页表!"<<endl;
	Process process(temp.JobId,temp.StarTime+10,temp.MemorySize,timeq);//为此作业创建应用进程，生成PCB 
	process.profork(pcb_table,ready);//进程移入就绪队列和PCB表等待低级调度 
	return 0;
}

//后续处理
int Job_Scheduler::FollowWork()
{
	return 0;
} 	
	
	
	
	
	

