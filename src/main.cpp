//相关头文件声明 
#include "utils.h"
#include "windows.h"  //使用系统级线程
#include "process.h"
#include "hardware.h" 
#include "job.h"


//传入线程参数的结构体 
typedef struct node{
	Memory memory;//建立内存 
	JCB_Table jcb_table;//建立作业表
	Pool_Queue pool_queue;//建立后备队列 
	Job_Scheduler job_scheduler;//建立作业调度器 
	PCB_Table pcb_table;//建立PCB表
	Page_Table page_table[11];//建立10张页表 
	LinkQueue ready;//就绪队列
	LinkQueue run;//运行队列 
	LinkQueue block;//阻塞队列
	LinkQueue finish;//完成队列 
	ofstream fmemory;//内存文件 
	ofstream fjob;//作业文件 
	ofstream frun;//运行情况保存文件 
	MMU mmu;
	Time time;//时钟（随机数生成） 
	int k; 
}DATA; 

//线程主体函数 
DWORD WINAPI ThreadFun(LPVOID pM) 
{
	DATA *data = (DATA*)pM;
	int y;
	data->k=1;
	while(1)
	{		
		if(data->k == 1)
		{
			JCB e;
			data->pool_queue.GetTop(e);
			int loop1 = e.JobId;
			while(data->pool_queue.front != data->pool_queue.rear)//等待内存有空闲资源 
			{
				cout<<"\n"<<"内存空闲，一个新的作业进入系统！"<<"\n"<<endl;
				data->frun<<"\n"<<"内存空闲，一个新的作业进入系统！"<<"\n"<<endl;
				JCB temp;
				if(data->job_scheduler.go(data->pool_queue,temp,data->memory,data->page_table[loop1],data->time,data->pcb_table,data->ready,data->jcb_table) == 0)//批处理作业的调度
				{
					data->k = 0;
					break;
				}
				cout<<endl;
				loop1++;	
			}
		} 
		
			
			
		if(data->block.front != data->block.rear)
		{
			Sleep(50);
			Process e;
			data->block.DeQueue(e);
			data->ready.EnQueue(e);
			//
			cout<<"\n"<<"一个阻塞的进程已经被唤醒"<<"\n"<<endl;
			data->frun<<"\n"<<"一个阻塞的进程已经被唤醒"<<"\n"<<endl;
		}
	
			//Sleep(50);
	}		
   	return 0; 	 
}

int main()
{
	                                                //程序初始化
	cout<<"============================================================================"<<endl; 
	cout<<"=                        虚拟页式存储管理系统                              ="<<endl;
	cout<<"=                                                  计科142   张颖          ="<<endl;  
	cout<<"============================================================================"<<endl; 
	cout<<"程序初始化.."<<endl;
	Sleep(1000);
	srand(time(0));//时间作种	
	DATA *Adata = new DATA;//传入线程的参数 
	Cpu CPU;
	Adata->k = 0;

	if(Adata == 0)
	{
		cout<<"程序初始化失败！"<<endl;
		return 1;
	}
	Adata->fmemory.open("内存.txt",ios::trunc); //ios::trunc表示在打开文件前将文件清空
	Adata->fjob.open("作业表.txt",ios::trunc); //ios::trunc表示在打开文件前将文件清空
	Adata->frun.open("运行情况.txt",ios::trunc); //ios::trunc表示在打开文件前将文件清空
	Adata->fmemory<<"作业还未进入内存时内存情况"<<"\r\n";
	Adata->fmemory<<"-----------------------------------------------------------------"<<"\r\n"; 
	Adata->memory.Print(Adata->fmemory); //打印内存状况
	cout<<"程序初始化成功！"<<endl;
	//Adata->frun<<"程序初始化成功！"<<endl;
	
	
	                                           //批处理作业的组织与管理
	/*
	1、批处理作业的建立：为每个作业建立相应的作业控制块，并将所有JCB组织成作业表 ,
		把已输入并建好的JCB作业排入后备队列，等待作业调度 
	*/ 
	for(int i = 1;i <= 10;i++)
	{
		JCB jcb;
		jcb.Init(i,(i+1)*50,Adata->time.RandNum(15,25));//为每个作业建立相应的作业控制块 
		Adata->jcb_table.InsertJCB(jcb,i);//将JCB组织成作业表 
		Adata->pool_queue.EnQueue(jcb);//把已输入并建好的JCB排入后备队列 
		cout<<endl; 
		Sleep(200);
	}
	Adata->fjob<<"作业等待调度"<<endl;//打印目前作业表 
	Adata->fjob<<"----------------------------------------------------"<<endl;
	Adata->jcb_table.PrintTable(Adata->fjob);
	cout<<"目前作业表信息已存入“作业表.txt”！"<<endl;
	cout<<"------------------------------------------------------------------"<<endl;
	cout<<endl;
	
	/*
	2、批处理作业的调度；作业调度程序需完成以下任务：
		a.选择作业。先来先服务算法
		b.分配资源。作业调度程序与存储管理程序进行通讯，为作业分配所需的资源。
		c.创建进程。每当选中作业且将其装入内存时，系统就为此作业创建应用进程，生成PCB及对应进程实体，这些进程将在低级调度控制下占用处理器运行。（可同时装入多个作业并启动运行）
		d.后续处理。作业正常结束或出错终止时，作业调度程序要做好作业撤离和完善工作，如打印输出信息、回收各种资源、撤销JCB等；同时，启动作业调度程序选择新作业进入内存；
	*/
	cout<<"开始作业调度.."<<endl;
	cout<<"输入任何东西，回车继续！"<<endl; 
	cout<<"***********************************************************"<<endl;
	char get = getchar();
	int loop1 = 1;
	while(Adata->pool_queue.front != Adata->pool_queue.rear)
	{
		JCB temp;
		if(Adata->job_scheduler.go(Adata->pool_queue,temp,Adata->memory,Adata->page_table[loop1],Adata->time,Adata->pcb_table,Adata->ready,Adata->jcb_table) == 0)//批处理作业的调度
		{
			break;
		}
		cout<<endl;
		loop1++; 
		Sleep(200);
	}
	//打印相关信息
	Adata->fjob<<"作业初次调度后"<<endl;//打印目前作业表 
	Adata->fjob<<"----------------------------------------------------"<<endl;
	Adata->jcb_table.PrintTable(Adata->fjob);
	cout<<"目前作业表信息已存入“作业表.txt”！"<<endl;
	
	Adata->pcb_table.PrintTable("进程表.txt");//打印进程表
	cout<<"目前进程表信息已存入“进程表.txt”！"<<endl;	
	
	char *Npage[] = {"页表1.txt","页表2.txt","页表3.txt","页表4.txt","页表5.txt","页表6.txt","页表7.txt","页表8.txt","页表9.txt","页表10.txt",};
	for(int i = 1;i <= 10;i++)//打印页表
	{	
		Adata->page_table[i].PrintTable(Npage[i-1]);
		cout<<"进程"<<i<<"对应页表信息已存入“页表"<<i<<".txt”！"<<endl;
	}
	Adata->fmemory<<"作业初次调度后的内存情况"<<"\r\n";
	Adata->fmemory<<"-----------------------------------------------------------------"<<"\r\n"; 
	Adata->memory.Print(Adata->fmemory); //打印内存状况
	cout<<"目前内存状态信息已存入“内存.txt”！"<<endl;	
	cout<<"------------------------------------------------------------"<<endl;
	
	
	                                                      /*进程调度（时间片轮转法）*/	
	cout<<"\r\n"<<"开始进程调度（时间片轮转法）.."<<endl;
	cout<<"输入任何东西，回车继续！"<<endl; 
	cout<<"***********************************************************"<<endl;
	getchar();
	get = getchar();

	HANDLE handle = CreateThread(NULL,0,ThreadFun,Adata,0,NULL);//线程启动函数 
	
	//时间片轮转法 
	while(Adata->ready.front != Adata->ready.rear) 
	{
		Sleep(100);
		int alltime = 0;
		int capacity = 60;//第一队列时间片 
		Process e;
		Adata->ready.DeQueue(e);//就绪队列第一个元素出队，进入运行队列 
		Adata->run.EnQueue(e);
					 
		//运行该进程第一条指令 
		Adata->pcb_table.process[e.ProID].ProState = 2;
		CPU.Scenerevover(Adata->pcb_table.process[e.ProID]);//CPU现场回复函数 
					
		cout<<"进程"<<e.ProID<<"页表地址装入页表基址寄存器！"<<endl; 
		Adata->frun<<"进程"<<e.ProID<<"页表地址装入页表基址寄存器！"<<endl; 
		cout<<"正在运行第"<<e.ProID<<"个进程"<<"，第"<<CPU.IR<<"个指令,"; 
		Adata->frun<<"正在运行第"<<e.ProID<<"个进程"<<"，第"<<CPU.IR<<"个指令,";
					
		if(Adata->pcb_table.process[e.ProID].instruc_arry[CPU.IR-1].State == 1)//当指令为用户态操作 
		{
			int addr = Adata->pcb_table.process[e.ProID].instruc_arry[CPU.IR-1].addr;
			int pageid;int pianyi;int paddr;
			cout<<"该指令为用户态计算操作"<<",逻辑地址为"<<addr<<endl; 
			Adata->frun<<"该指令为用户态计算操作"<<",逻辑地址为"<<addr<<endl; 
			Adata->mmu.go(Adata->frun,Adata->page_table[e.ProID],addr,pageid,pianyi,paddr);//MMU工作 
			cout<<endl;
			Adata->frun<<endl; 
			Sleep(Adata->pcb_table.process[e.ProID].instruc_arry[CPU.IR-1].time); 
			Adata->pcb_table.process[e.ProID].RunTimes += Adata->pcb_table.process[e.ProID].instruc_arry[CPU.IR-1].time;
			alltime += Adata->pcb_table.process[e.ProID].instruc_arry[CPU.IR-1].time;
			CPU.IR++;
			CPU.Sceneprotection(Adata->pcb_table.process[e.ProID]);
					 	
			Adata->run.DeQueue(e);
			if(CPU.IR > Adata->pcb_table.process[e.ProID].InstrucNum)//判断是否执行完成 
			{
				Adata->pcb_table.process[e.ProID].alltime = alltime;//进程的周转时间 
				Adata->finish.EnQueue(e);
				cout<<"****进程"<<e.ProID<<"已运行完成！****"<<endl; 
				Adata->frun<<"****进程"<<e.ProID<<"已运行完成！****"<<endl; 
				//回收内存 
				JCB jcb;
				Adata->jcb_table.SearchJobId(e.JobId,jcb);
				Adata->memory.RecycleSpace(jcb.MemorySize-5,jcb.JobId);
				cout<<"****作业"<<e.ProID<<"已运行完成！****"<<endl; 
				Adata->frun<<"****作业"<<e.ProID<<"已运行完成！****"<<endl;
				cout<<"****已回收"<<"作业"<<e.ProID<<"的资源！****"<<endl; 
				Adata->frun<<"****已回收"<<"作业"<<e.ProID<<"的资源！****"<<endl;
				
				
				Adata->fmemory<<"作业"<<e.ProID<<"运行完成时内存状况"<<"\r\n";
				Adata->fmemory<<"-----------------------------------------------------------------"<<"\r\n"; 
				Adata->memory.Print(Adata->fmemory); //打印内存状况
				
				Adata->k = 1; 
				Sleep(500);
			}
			else
			{
				Adata->ready.EnQueue(e);
			}
							
		}
		else//当指令为I/O操作 
		{
			int addr = Adata->pcb_table.process[e.ProID].instruc_arry[CPU.IR-1].addr;
			int pageid;int pianyi;int paddr;
			cout<<"该指令为I/O操作"<<",逻辑地址为"<<addr<<endl; 
			Adata->frun<<"该指令为I/O操作"<<",逻辑地址为"<<addr<<endl; 
			Adata->mmu.go(Adata->frun,Adata->page_table[e.ProID],addr,pageid,pianyi,paddr);//MMU工作 
			cout<<endl;
			Adata->frun<<endl; 
			Sleep(Adata->pcb_table.process[e.ProID].instruc_arry[CPU.IR-1].time); 
			Adata->pcb_table.process[e.ProID].RunTimes += Adata->pcb_table.process[e.ProID].instruc_arry[CPU.IR-1].time;
			//alltime += data1->L.process[e.ProID].instruc_arry[IR-1].time;
			CPU.IR++;
			CPU.Sceneprotection(Adata->pcb_table.process[e.ProID]);
					 	
			Adata->run.DeQueue(e);
			if(CPU.IR > Adata->pcb_table.process[e.ProID].InstrucNum)//判断是否执行完成 
			{
				Adata->pcb_table.process[e.ProID].alltime = alltime;
				Adata->finish.EnQueue(e);
				cout<<"****进程"<<e.ProID<<"已运行完成！****"<<endl; 
				Adata->frun<<"****进程"<<e.ProID<<"已运行完成！****"<<endl; 
				//data1->L.DeleteProID(e.ProID);
				//回收内存 
				JCB jcb;
				Adata->jcb_table.SearchJobId(e.JobId,jcb);
				Adata->memory.RecycleSpace(jcb.MemorySize-5,jcb.JobId);
				cout<<"****作业"<<e.ProID<<"已运行完成！****"<<endl; 
				Adata->frun<<"****作业"<<e.ProID<<"已运行完成！****"<<endl;
				cout<<"****已回收"<<"作业"<<e.ProID<<"的资源！****"<<endl; 
				Adata->frun<<"****已回收"<<"作业"<<e.ProID<<"的资源！****"<<endl; 
				 
				
				Adata->fmemory<<"作业"<<e.ProID<<"运行完成时内存状况"<<"\r\n";
				Adata->fmemory<<"-----------------------------------------------------------------"<<"\r\n"; 
				Adata->memory.Print(Adata->fmemory); //打印内存状况
				
				Adata->k = 1;
				Sleep(500);
			}
			else
			{
				Adata->block.EnQueue(e);
			}
		}
		Sleep(100);						 	 				  
	}
	cout<<"\r\n"<<"以上运行过程已存入“运行情况.txt”！"<<endl;	
	cout<<"以上运行过程的内存状态变化情况已存入“内存.txt”！"<<endl;
	cout<<"各进程页表信息已同步至相关文件！"<<endl;
	Adata->pcb_table.PrintTable("进程表(完成).txt");//打印进程表
	cout<<"目前进程表信息已存入“进程表(完成).txt”！"<<endl;	
	Adata->fjob<<"作业全部运行完成后"<<endl;//打印目前作业表 
	Adata->fjob<<"----------------------------------------------------"<<endl;
	Adata->jcb_table.PrintTable(Adata->fjob);
	cout<<"目前作业表信息已存入“作业表.txt”！"<<endl;	
	for(int i = 1;i <= 10;i++)//打印页表
	{	
		Adata->page_table[i].PrintTable(Npage[i-1]);
		//cout<<"进程"<<i<<"1对应页表信息已存入“页表"<<i<<".txt”！"<<endl;
	}
	
	//关闭所有文件 
	Adata->fmemory.close();
	Adata->fjob.close();
	Adata->frun.close();
	
	char stop = 'a';
	while(stop != 'e')
	{
		cout<<"请输入字符e退出"<<endl;
		stop = getchar();
	}
	cout<<"运行完成！"; 
	return 0;
}


