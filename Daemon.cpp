#include <windows.h>
#include<stdio.h>
DWORD WINAPI ThreadCheckProc(LPVOID lParam){
    STARTUPINFO si={sizeof(si)};
    PROCESS_INFORMATION pi={0};
    HANDLE hMutex;
    char *pName=(char *)lParam;
    while(true){
        hMutex=OpenMutex(MUTEX_ALL_ACCESS,FALSE,pName);
        if(!hMutex){
            CreateProcess(pName,
            NULL,
            NULL,
            NULL, 
            FALSE, 
            CREATE_NEW_CONSOLE, 
            NULL, 
            NULL, 
            &si, 
            &pi ); 
            WaitForSingleObject( pi.hProcess, INFINITE );
            CloseHandle( pi.hProcess );
            CloseHandle( pi.hThread );
        }
        else{
            CloseHandle(hMutex);
        }
        Sleep(500);
    }
}
int main()
{
    DWORD ThreadId;
    CreateMutex(NULL,TRUE,"test2.exe");
    CreateThread(NULL,0,ThreadCheckProc,(LPVOID *)"test1.exe",0,&ThreadId);
    printf("Protected\n");
    while(1){
        Sleep(500);
    }
    return 0;
}
-------------------------------------------------
#include <windows.h>
#include <stdio.h>
int main()
{
    HANDLE event1;
    HANDLE event2;
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    event2 = CreateEvent(NULL, FALSE, TRUE, "Local\\p2");
    while (true)
    {
        if (!(event1 = OpenEvent(EVENT_MODIFY_STATE, FALSE, "Local\\p1")))
        {
            if (!CreateProcess("protect1.exe",
                NULL,
                NULL,
                NULL,
                NULL,
                CREATE_NEW_CONSOLE,
                NULL,
                NULL,
                &si,
                &pi
                ))
            {
                printf("打开守护进程失败！\n");
            }

            printf("打开守护进程成功！PID:%d \n", pi.dwProcessId);
            WaitForSingleObject(pi.hProcess, INFINITE);
            printf("守护进程被关闭！\n");
            CloseHandle(pi.hProcess);
        }
        CloseHandle(event1);
        Sleep(1000);
    }
    return 0;
}

