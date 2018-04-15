// UnInstall.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include <stdio.h>

#define SERVICE_NAME_FILE		"\\Data\\service_name.txt"

char SSDT_SERVICE_NAME[MAX_PATH] = {0};



bool UnInstallNet();
bool StopDriver();
bool DeleteSC(SC_HANDLE sc_hManager, SC_HANDLE sc_hResult);
int GetServiceName();
bool ProceeIsRun();
bool WriteToBat();

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
		if(ProceeIsRun())
		{
			MessageBox(NULL,"³ÌÐòÔÚÔËÐÐ£¬ÇëÍË³öÔÙÐ¶ÔØ£¡","",0);
			return -1;
		}	
		int flag=0;
		GetServiceName();
		WNDCLASS wc;
		wc.hIcon  = LoadIcon (hInstance,(LPCTSTR)IDI_FW); 
		bool ssdt,net;
		ssdt = StopDriver();
		
		net = UnInstallNet();
		if(ssdt==false)
		{
			MessageBox(0,"Ð¶ÔØÊ§°Ü,ÇëÉÔºóÖØÊÔ!","",0);
			flag = -1;
		}
		if(net==false)
		{
			flag = -2;
		}
		if(flag==0)
		{
			if(WriteToBat())
			{
				MessageBox(0,"Ð¶ÔØÖÐ,ÇëÉÔºó!","",0);
				WinExec("tmp.bat",SW_HIDE);
			}
			else
				MessageBox(0,"É¾³ýÊ§°Ü,ÇëÊÖ¶¯É¾³ý!","",0);
		}	
		return flag;	
}


bool WriteToBat()
{
	 DWORD dwRead = 0;
    DWORD dwWrite = 0;
	char path[MAX_PATH];
	char WriteBuf[1024]="ping 127.1 -n 5>nul\r\ncd ..\r\nrd /Q /S FireWall\r\n";
	GetCurrentDirectory(MAX_PATH,path);
	strcat(path,"\\tmp.bat");

    HANDLE hFile = CreateFile(path,  GENERIC_WRITE | GENERIC_READ, 0, 
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }
	WriteFile(hFile,WriteBuf , sizeof(WriteBuf), &dwWrite, NULL);
    CloseHandle(hFile);
    return true;
}

int GetServiceName()
{
    DWORD dwRead = 0;
    DWORD dwWrite = 0;
	char path[MAX_PATH];
	int num;
	GetCurrentDirectory(MAX_PATH,path);
	strcat(path,SERVICE_NAME_FILE);

    HANDLE hFile = CreateFile(path, GENERIC_WRITE | GENERIC_READ, 0, 
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        return -1;
    }
 
    DWORD fileSize = GetFileSize(hFile, NULL);
    ReadFile(hFile, SSDT_SERVICE_NAME, fileSize, &dwRead, NULL);
    SSDT_SERVICE_NAME[fileSize] = 0;
    CloseHandle(hFile);
    return 0;
}




bool UnInstallNet()
{
	char path[MAX_PATH];
	STARTUPINFO si = { sizeof(si) }; 
    PROCESS_INFORMATION pi;   
 
    si.dwFlags = STARTF_USESHOWWINDOW;   
    si.wShowWindow = SW_HIDE;
	GetCurrentDirectory(MAX_PATH,path);
	strcat(path,"//sys//netend.exe");
 
    bool bsucess = CreateProcess(NULL, 
        path,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi);
 
    if(bsucess)
    {
        HANDLE m_hProcess=pi.hProcess;  
        CloseHandle(pi.hThread);

        WaitForSingleObject(m_hProcess,INFINITE);
 
        DWORD dwexitcode;
        GetExitCodeProcess(m_hProcess, &dwexitcode);
		
		if(dwexitcode==1||dwexitcode==-6)
			return true;
		else
			return false;
    }
    else
    {
        return false;
    }
}

bool StopDriver()
{
	SERVICE_STATUS ssStatus;
	SERVICE_STATUS SerStat;
	SC_HANDLE sc_hManager = NULL;
	SC_HANDLE sc_hResult = NULL;
	sc_hManager = OpenSCManager (NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(NULL == sc_hManager)
	{
		return false;
	}
	sc_hResult = OpenService(sc_hManager,SSDT_SERVICE_NAME,SERVICE_ALL_ACCESS);
	if(NULL == sc_hResult)
	{
		CloseServiceHandle(sc_hResult);
		CloseServiceHandle(sc_hManager);
		return false;
	}

	if(QueryServiceStatus(sc_hResult,&ssStatus))
	{
		if(ssStatus.dwCurrentState==SERVICE_RUNNING)
		{
			if(!ControlService(sc_hResult,SERVICE_CONTROL_STOP,&SerStat))
			{
				return false;
			}else
			{
				return DeleteSC(sc_hManager,sc_hResult);
			}
		}else if(ssStatus.dwCurrentState==SERVICE_RUNNING)
		{
			return DeleteSC(sc_hManager,sc_hResult);
		}
	}else
	{
		return false;
	}
	
}

bool DeleteSC(SC_HANDLE sc_hManager, SC_HANDLE sc_hResult)
{
	if(DeleteService(sc_hResult))
	{
		CloseServiceHandle(sc_hResult);
		CloseServiceHandle(sc_hManager);
		return true;
	}
	else
	{
		CloseServiceHandle(sc_hResult);
		CloseServiceHandle(sc_hManager);
		return false;
	}
}

bool ProceeIsRun()
{
	char buf[3];
	DWORD dwRead = 0;
    DWORD dwWrite = 0;
	char path[MAX_PATH];
	int i = WinExec("cmd /c tasklist | findstr /I FireWall.exe > tmp.txt",SW_HIDE);
	
	GetCurrentDirectory(MAX_PATH,path);
	strcat(path,"\\tmp.txt");

    HANDLE hFile = CreateFile(path, GENERIC_WRITE | GENERIC_READ, 0, 
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
		CloseHandle(hFile);
        return false;
    }
	DWORD fileSize = GetFileSize(hFile, NULL);
	CloseHandle(hFile);
	if(fileSize==0)
		return false;
	else
		return true;
}
