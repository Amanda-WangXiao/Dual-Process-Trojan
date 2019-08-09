//��3��ľ�����������
#include <stdio.h>
#include <string.h>
#include <Winsock2.h>
#pragma comment(lib, "ws2_32.lib")
 
SOCKET sockClient;     // �ͻ��� Scoket
 
DWORD WINAPI ThreadProc (LPVOID lpThreadParameter)//Ϊһ���̵߳���ʼ��ַ,�ڵ���CreateThread����ʱ��ָ���õ�ַ
	                                              //�����̴߳��ݸ�����ʹ�õ�CreateThread����lpParameter��������
{
    char recvBuf[1024] = {0};    //������
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
    int err; // ������Ϣ
    int len = sizeof(SOCKADDR);
    DWORD Tid; // �߳� ID
 
    char  cmdStr[100] = {0};
    char sendBuf[100] = {0}; // �������ͻ��˵��ַ���
    char recvBuf[1024] = {0}; // ���ܿͻ��˷��ص��ַ���
    char * ip;
 
    SOCKET sockServer;     // ����� Socket
    SOCKADDR_IN addrServer;// ����˵�ַ    
    SOCKADDR_IN addrClient;// �ͻ��˵�ַ
 
    WSADATA wsaData;       // winsock �ṹ��
    WORD wVersionRequested;// winsock �İ汾
 
    
    wVersionRequested = MAKEWORD( 2, 2 );	// ���� Windows Socket�汾
 
    
    err = WSAStartup( wVersionRequested, &wsaData );	// ��ʼ�� Windows Socket
 
    if ( err != 0 )		// �������󣬳������
    {
        
        return;
    }
 
    if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )		// �������󣬳������
    {
        
        WSACleanup(); // ��ֹWinsock 2 DLL (Ws2_32.dll) ��ʹ��
        return;
    }
 
    
    sockServer = socket(AF_INET, SOCK_STREAM, 0);		// �����������socket

    //--------------------���÷���� socket--------------------
    addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.28.128"); // ����IP
    addrServer.sin_family = AF_INET;                   // Э��������INET
    addrServer.sin_port = htons(6000);                 // �󶨶˿�6000
	//--------------------���÷���� socket end--------------------

	
    bind(sockServer, (SOCKADDR *)&addrServer, sizeof(SOCKADDR));// ����������socket���ڱ��ض˿�
    
    listen(sockServer, 5);		// Listen �����˿�,5 Ϊ�ȴ�������Ŀ
 
    printf("������������:\n������...\n");
 
    
    sockClient = accept(sockServer, (SOCKADDR *)&addrClient, &len);	// �ȴ��ͻ�������
    
    ip = inet_ntoa(addrClient.sin_addr);	// ��ȡip��ַ
    
    printf("-- IP %s ���ӵ������\n", ip);	// ���������ʾ
 
    
    CreateThread(	// ����������Ϣ
        NULL,       // ���ܱ��ӽ��̼̳�
        0,          // Ĭ�϶�ջ��С
        ThreadProc, // �̵߳��ú�������
        NULL,       // ���ݲ���
        0,          // ����������ִ��
        &Tid        // ���洴������߳�ID
    );
  
    while (1)
    {
        printf(">>");
        gets(cmdStr);
             
        
        if (strcmp(cmdStr, "exit") == 0)	// ��������Ĳ�ͬ������ִ�в�ͬ�Ĳ���
        {
            send(sockClient, cmdStr, strlen(cmdStr) + 1, 0);	 // ����û����� exit �����ѭ��
            break;
        }
        
        send(sockClient, cmdStr, strlen(cmdStr) + 1, 0);	// ��������͵��ͻ���
    }
    closesocket(sockClient);
    WSACleanup();
}

