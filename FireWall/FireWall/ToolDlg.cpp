// ToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FireWall.h"
#include "ToolDlg.h"
#include "define.h"
#include "CTL_CODE.h"
#include "Global.h"
#include "FireWallDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CToolDlg dialog


CToolDlg::CToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CToolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolDlg, CDialog)
	//{{AFX_MSG_MAP(CToolDlg)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_SAVE_SYSTEM, OnSaveSystem)
	ON_BN_CLICKED(IDC_RESET_SYSTEM, OnResetSystem)
	ON_BN_CLICKED(IDC_NO_CMD, OnNoCmd)
	ON_BN_CLICKED(IDC_EXIT_TO_DISH, OnExitToDish)
	ON_BN_CLICKED(IDC_NO_ADD_USER, OnNoAddUser)
	ON_BN_CLICKED(IDC_NO_CHANGE_TIME, OnNoChangeTime)
	ON_BN_CLICKED(IDC_NO_DEL_USER, OnNoDelUser)
	ON_BN_CLICKED(IDC_NO_HIDE_THREAD, OnNoHideThread)
	ON_BN_CLICKED(IDC_NO_LOAD_DRVIER, OnNoLoadDrvier)
	ON_BN_CLICKED(IDC_NO_NET, OnNoNet)
	ON_BN_CLICKED(IDC_NO_PING, OnNoPing)
	ON_BN_CLICKED(IDC_NO_REG, OnNoReg)
	ON_BN_CLICKED(IDC_RUN_START, OnRunStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolDlg message handlers

void CToolDlg::OnSave() 
{
	// TODO: Add your control notification handler code here
	//AfxMessageBox("保存成功");
	if(Global::CurStatus[MY_NO_NET]!=Global::PreStatus[MY_NO_NET])
	{
		if(Global::CurStatus[MY_NO_NET])
			MyNoNet(0);
		else
			MyNoNet(1);
		Global::PreStatus[MY_NO_NET] = Global::CurStatus[MY_NO_NET];
	}
	if(Global::CurStatus[MY_NO_PING]!=Global::PreStatus[MY_NO_PING])
	{
		if(Global::CurStatus[MY_NO_PING])
			MyNoPing(0);
		else
			MyNoPing(1);
		Global::PreStatus[MY_NO_PING] = Global::CurStatus[MY_NO_PING];
	}
	if(Global::CurStatus[MY_NO_CHANGE_TIME]!=Global::PreStatus[MY_NO_CHANGE_TIME])
	{
		if(Global::CurStatus[MY_NO_CHANGE_TIME])
			MyNoSetTime(1);
		else
			MyNoSetTime(0);
		Global::PreStatus[MY_NO_CHANGE_TIME] = Global::CurStatus[MY_NO_CHANGE_TIME];
	}
	if(Global::CurStatus[MY_NO_DRIVER]!=Global::PreStatus[MY_NO_DRIVER])
	{
		if(Global::CurStatus[MY_NO_DRIVER])
			MyNoLoadDriver(1);
		else
			MyNoLoadDriver(0);
		Global::PreStatus[MY_NO_DRIVER] = Global::CurStatus[MY_NO_DRIVER];
	}

	if(Global::CurStatus[MY_NO_CMD]!=Global::PreStatus[MY_NO_CMD])
	{
		if(Global::CurStatus[MY_NO_CMD])
			MyNoSomeProcess(MY_NO_CMD);
		else
			MySomeProcess(MY_NO_CMD);
		Global::PreStatus[MY_NO_CMD] = Global::CurStatus[MY_NO_CMD];
	}
	if(Global::CurStatus[MY_NO_REG]!=Global::PreStatus[MY_NO_REG])
	{
		if(Global::CurStatus[MY_NO_REG])
			MyNoSomeProcess(MY_NO_REG);
		else
			MySomeProcess(MY_NO_REG);
		Global::PreStatus[MY_NO_REG] = Global::CurStatus[MY_NO_REG];
	}
	if(Global::CurStatus[MY_NO_ADD_USER]!=Global::PreStatus[MY_NO_ADD_USER])
	{
		if(Global::CurStatus[MY_NO_ADD_USER])
			MyNoSomeProcess(MY_NO_ADD_USER);
		else
			MySomeProcess(MY_NO_ADD_USER);
		Global::PreStatus[MY_NO_ADD_USER] = Global::CurStatus[MY_NO_ADD_USER];
	}
	if(Global::CurStatus[MY_NO_DEL_USER]!=Global::PreStatus[MY_NO_DEL_USER])
	{
		if(Global::CurStatus[MY_NO_DEL_USER])
			MyNoSomeProcess(MY_NO_DEL_USER);
		else
			MySomeProcess(MY_NO_DEL_USER);
		Global::PreStatus[MY_NO_DEL_USER] = Global::CurStatus[MY_NO_DEL_USER];
	}
	if(Global::CurStatus[MY_NO_HIDE]!=Global::PreStatus[MY_NO_HIDE])
	{
		if(Global::CurStatus[MY_NO_HIDE])
			MyNoHide(1);
		else
			MyNoHide(0);
		Global::PreStatus[MY_NO_HIDE] = Global::CurStatus[MY_NO_HIDE];
	}
	(GetDlgItem(IDC_SAVE))->EnableWindow(0);
	AfxMessageBox("设置成功");
}

void CToolDlg::OnReset() 
{
	// TODO: Add your control notification handler code here
	if(Global::CurStatus[MY_NO_CMD])
	{
		((CButton*)GetDlgItem(IDC_NO_CMD))->SetCheck(0);
		Global::CurStatus[MY_NO_CMD] =false;		
	}
	if(Global::CurStatus[MY_NO_REG])
	{
		((CButton*)GetDlgItem(IDC_NO_REG))->SetCheck(0);
		Global::CurStatus[MY_NO_REG] =false;		
	}
	if(Global::CurStatus[MY_NO_HIDE])
	{
		((CButton*)GetDlgItem(IDC_NO_HIDE_THREAD))->SetCheck(0);
		Global::CurStatus[MY_NO_HIDE] =false;		
	}
	if(Global::CurStatus[MY_NO_PING])
	{
		((CButton*)GetDlgItem(IDC_NO_PING))->SetCheck(0);
		Global::CurStatus[MY_NO_PING] =false;		
	}
	if(Global::CurStatus[MY_NO_NET])
	{
		((CButton*)GetDlgItem(IDC_NO_NET))->SetCheck(0);
		Global::CurStatus[MY_NO_NET] =false;		
	}
	if(Global::CurStatus[MY_NO_DRIVER])
	{
		((CButton*)GetDlgItem(IDC_NO_LOAD_DRVIER))->SetCheck(0);
		Global::CurStatus[MY_NO_DRIVER] =false;		
	}
	if(Global::CurStatus[MY_NO_ADD_USER])
	{
		((CButton*)GetDlgItem(IDC_NO_ADD_USER))->SetCheck(0);
		Global::CurStatus[MY_NO_ADD_USER] =false;		
	}
	if(Global::CurStatus[MY_NO_DEL_USER])
	{
		((CButton*)GetDlgItem(IDC_NO_DEL_USER))->SetCheck(0);
		Global::CurStatus[MY_NO_DEL_USER] =false;		
	}
	if(Global::CurStatus[MY_NO_CHANGE_TIME])
	{
		((CButton*)GetDlgItem(IDC_NO_CHANGE_TIME))->SetCheck(0);
		Global::CurStatus[MY_NO_CHANGE_TIME] =false;		
	}
	(GetDlgItem(IDC_SAVE))->EnableWindow(1);
}

void CToolDlg::OnSaveSystem() 
{
	// TODO: Add your control notification handler code here
	if(Global::CurStatus[MY_NO_START]!=Global::PreStatus[MY_NO_START])
	{
		if(Global::CurStatus[MY_NO_START])
			AddToAutoStart();
		else
			DelToAutoStart();
		Global::PreStatus[MY_NO_START] = Global::CurStatus[MY_NO_START];
	}
	if(Global::CurStatus[MY_NO_TO_DISH]!=Global::PreStatus[MY_NO_TO_DISH])
	{
		if(Global::CurStatus[MY_NO_TO_DISH])
			MyExitToDish(1);
		else
			MyExitToDish(0);
		Global::PreStatus[MY_NO_TO_DISH] = Global::CurStatus[MY_NO_TO_DISH];
	}
	(GetDlgItem(IDC_SAVE_SYSTEM))->EnableWindow(0);
	AfxMessageBox("设置成功");
}

void CToolDlg::OnResetSystem() 
{
	// TODO: Add your control notification handler code here
	if(Global::CurStatus[MY_NO_START])
	{
		((CButton*)GetDlgItem(IDC_RUN_START))->SetCheck(0);
		Global::CurStatus[MY_NO_START] =false;		
	}
	if(Global::CurStatus[MY_NO_TO_DISH])
	{
		((CButton*)GetDlgItem(IDC_EXIT_TO_DISH))->SetCheck(0);
		Global::CurStatus[MY_NO_TO_DISH] =false;
	}
	(GetDlgItem(IDC_SAVE_SYSTEM))->EnableWindow(1);
}

void CToolDlg::OnNoAddUser() 
{
	// TODO: Add your control notification handler code here
	Global::CurStatus[MY_NO_ADD_USER]=!Global::CurStatus[MY_NO_ADD_USER];
	(GetDlgItem(IDC_SAVE))->EnableWindow(1);
}

void CToolDlg::OnNoChangeTime() 
{
	// TODO: Add your control notification handler code here
	Global::CurStatus[MY_NO_CHANGE_TIME]=!Global::CurStatus[MY_NO_CHANGE_TIME];
	(GetDlgItem(IDC_SAVE))->EnableWindow(1);
}

void CToolDlg::OnNoCmd() 
{
	// TODO: Add your control notification handler code here
	Global::CurStatus[MY_NO_CMD]=!Global::CurStatus[MY_NO_CMD];
	(GetDlgItem(IDC_SAVE))->EnableWindow(1);
}

void CToolDlg::OnNoDelUser() 
{
	// TODO: Add your control notification handler code here
	Global::CurStatus[MY_NO_DEL_USER]=!Global::CurStatus[MY_NO_DEL_USER];
	(GetDlgItem(IDC_SAVE))->EnableWindow(1);
}

void CToolDlg::OnNoHideThread() 
{
	// TODO: Add your control notification handler code here
	Global::CurStatus[MY_NO_HIDE]=!Global::CurStatus[MY_NO_HIDE];
	(GetDlgItem(IDC_SAVE))->EnableWindow(1);
}

void CToolDlg::OnNoLoadDrvier() 
{
	// TODO: Add your control notification handler code here
	Global::CurStatus[MY_NO_DRIVER]=!Global::CurStatus[MY_NO_DRIVER];
	(GetDlgItem(IDC_SAVE))->EnableWindow(1);
}

void CToolDlg::OnNoNet() 
{
	// TODO: Add your control notification handler code here
	Global::CurStatus[MY_NO_NET]=!Global::CurStatus[MY_NO_NET];
	(GetDlgItem(IDC_SAVE))->EnableWindow(1);
}

void CToolDlg::OnNoPing() 
{
	// TODO: Add your control notification handler code here
	Global::CurStatus[MY_NO_PING]=!Global::CurStatus[MY_NO_PING];
	(GetDlgItem(IDC_SAVE))->EnableWindow(1);
}

void CToolDlg::OnNoReg() 
{
	// TODO: Add your control notification handler code here
	Global::CurStatus[MY_NO_REG]=!Global::CurStatus[MY_NO_REG];
	(GetDlgItem(IDC_SAVE))->EnableWindow(1);
}

void CToolDlg::OnRunStart() 
{
	// TODO: Add your control notification handler code here
	Global::CurStatus[MY_NO_START]=!Global::CurStatus[MY_NO_START];
	(GetDlgItem(IDC_SAVE_SYSTEM))->EnableWindow(1);
}
void CToolDlg::OnExitToDish()
{
	Global::CurStatus[MY_NO_TO_DISH]=!Global::CurStatus[MY_NO_TO_DISH];
	(GetDlgItem(IDC_SAVE_SYSTEM))->EnableWindow(1);
}

void CToolDlg::MyNoNet(int chose)
{
	DWORD ret;
	HANDLE h =  CreateFile("\\\\.\\FIREWALLNDIS",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(h==INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox("Open Failed!!");
		return;
	}
	DeviceIoControl(h,MY_NET_DEF_PASS_CTL_CODE,&chose,sizeof(int),NULL,0,&ret,NULL);
	CloseHandle(h);
}

void CToolDlg::MyNoPing(int chose)
{
	DWORD ret;
	HANDLE h =  CreateFile("\\\\.\\FIREWALLNDIS",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(h==INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox("Open Failed!!");
		return;
	}
	DeviceIoControl(h,MY_NET_NO_PING_CTL_CODE,&chose,sizeof(int),NULL,0,&ret,NULL);
	CloseHandle(h);	
}

void CToolDlg::Init()
{
	if(Global::CurStatus[MY_NO_CMD])
	{
		((CButton*)GetDlgItem(IDC_NO_CMD))->SetCheck(1);		
	}
	if(Global::CurStatus[MY_NO_REG])
	{
		((CButton*)GetDlgItem(IDC_NO_REG))->SetCheck(1);
	}
	if(Global::CurStatus[MY_NO_HIDE])
	{
		MyNoHide(1);
		((CButton*)GetDlgItem(IDC_NO_HIDE_THREAD))->SetCheck(1);	
	}
	if(Global::CurStatus[MY_NO_PING])
	{
		MyNoPing(0);
		((CButton*)GetDlgItem(IDC_NO_PING))->SetCheck(1);	
	}
	if(Global::CurStatus[MY_NO_NET])
	{
		MyNoNet(0);
		((CButton*)GetDlgItem(IDC_NO_NET))->SetCheck(1);		
	}
	if(Global::CurStatus[MY_NO_DRIVER])
	{
		MyNoLoadDriver(1);
		((CButton*)GetDlgItem(IDC_NO_LOAD_DRVIER))->SetCheck(1);	
	}
	if(Global::CurStatus[MY_NO_ADD_USER])
	{
		((CButton*)GetDlgItem(IDC_NO_ADD_USER))->SetCheck(1);	
	}
	if(Global::CurStatus[MY_NO_DEL_USER])
	{
		((CButton*)GetDlgItem(IDC_NO_DEL_USER))->SetCheck(1);	
	}
	if(Global::CurStatus[MY_NO_CHANGE_TIME])
	{
		MyNoSetTime(1);
		((CButton*)GetDlgItem(IDC_NO_CHANGE_TIME))->SetCheck(1);	
	}
	if(Global::CurStatus[MY_NO_START])
	{
		((CButton*)GetDlgItem(IDC_RUN_START))->SetCheck(1);	
	}
	if(Global::CurStatus[MY_NO_TO_DISH])
	{
		MyExitToDish(1);
		((CButton*)GetDlgItem(IDC_EXIT_TO_DISH))->SetCheck(1);	
	}
}


BOOL CToolDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	Init();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CToolDlg::MyNoSomeProcess(int chose)
{
	CString str,filepath;
	DWORD ret;
	CStdioFile File;
	char path[MAX_PATH];
	switch(chose) {
	case MY_NO_CMD:
		str.Format("%s","C:\\windows\\system32\\cmd.exe|");
		break;
	case MY_NO_REG:
		str.Format("%s","C:\\windows\\system32\\reg.exe|");
		break;
	case MY_NO_ADD_USER:
	case MY_NO_DEL_USER:
		str.Format("%s","C:\\windows\\system32\\net.exe|");
		break;
	}
	GetCurrentDirectory(MAX_PATH,path);
	filepath.Format("%s%s%s",path,BASE_PATH,PROCESS_LIST_FILE);
	if(File.Open(filepath,CFile::modeWrite))
	{
		File.SeekToEnd();
		File.WriteString(str);
		File.Close();
	}else
	{
		AfxMessageBox("File Error!");
		return ;
	}
	HANDLE h =  CreateFile("\\\\.\\FIREWALLSSDT",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(h==INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox("Open Failed!!");
		return;
	}
	DeviceIoControl(h,MY_PROCESS_RESTART_CTL_CODE,NULL,0,NULL,0,&ret,NULL);
 	CloseHandle(h);
}

void CToolDlg::MySomeProcess(int chose)
{
	CString str,filepath,tmp,newtmp;
	int f;
	DWORD ret;
	CStdioFile File;
	char path[MAX_PATH];
	switch(chose) {
	case MY_NO_CMD:
		str.Format("%s","C:\\windows\\system32\\cmd.exe|");
		break;
	case MY_NO_REG:
		str.Format("%s","C:\\windows\\system32\\reg.exe|");
		break;
	case MY_NO_ADD_USER:
	case MY_NO_DEL_USER:
		str.Format("%s","C:\\windows\\system32\\net.exe|");
		break;
	}
	GetCurrentDirectory(MAX_PATH,path);
	filepath.Format("%s%s%s",path,BASE_PATH,PROCESS_LIST_FILE);
	if(File.Open(filepath,CFile::modeReadWrite))
	{
		File.ReadString(tmp);
		f = tmp.Find(str);
		newtmp = tmp.Left(f)+tmp.Right(tmp.GetLength()-f-str.GetLength());
		//AfxMessageBox(newtmp);
		File.SetLength(0);
		File.WriteString(newtmp);
		File.Close();
	}else
	{
		AfxMessageBox("File Error!");
		return ;
	}
	HANDLE h =  CreateFile("\\\\.\\FIREWALLSSDT",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(h==INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox("Open Failed!!");
		return;
	}
	DeviceIoControl(h,MY_PROCESS_RESTART_CTL_CODE,NULL,0,NULL,0,&ret,NULL);
 	CloseHandle(h);	
}

void CToolDlg::AddToAutoStart()
{
	HKEY hKey;
	DWORD dwtype;
	LONG result1;
	LONG result2;
	LONG result3;
	char path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,path);
	strcat(path,"\\FireWall.exe");

	result1 = RegOpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey);

	if (!result1)
	{
		result2 = RegSetValueEx(hKey, "FireWall",0,REG_SZ,(BYTE*)path, strlen(path));
	}
	else
	{
		return ;
	}
}

void CToolDlg::DelToAutoStart()
{
	HKEY hKey;
	DWORD dwtype;
	LONG result1;
	LONG result2;
	LONG result3;

	result1 = RegOpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey);

	if (!result1)
	{
		result3 = RegDeleteValue(hKey,"FireWall");
	}
	else
	{
		return ;//MessageBox(_T("打开失败"));
	}
}

void CToolDlg::MyNoSetTime(int chose)
{
	DWORD ret;
	HANDLE h =  CreateFile("\\\\.\\FIREWALLSSDT",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(h==INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox("Open Failed!!");
		return;
	}
	DeviceIoControl(h,MY_SSDT_IS_SET_TIME_CTL_CODE,&chose,sizeof(int),NULL,0,&ret,NULL);
	CloseHandle(h);	
}

void CToolDlg::MyNoLoadDriver(int chose)
{
	DWORD ret;
	HANDLE h =  CreateFile("\\\\.\\FIREWALLSSDT",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(h==INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox("Open Failed!!");
		return;
	}
	DeviceIoControl(h,MY_SSDT_IS_LOAD_DRIVER_CTL_CODE,&chose,sizeof(int),NULL,0,&ret,NULL);
	CloseHandle(h);	
}

void CToolDlg::MyNoHide(int nohide)
{
	DWORD ret;
	HANDLE h =  CreateFile("\\\\.\\FIREWALLSSDT",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(h==INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox("Open Failed!!");
		return;
	}
	DeviceIoControl(h,MY_SSDT_IS_HIDE_CTL_CODE,&nohide,sizeof(int),NULL,0,&ret,NULL);
	CloseHandle(h);
}

void CToolDlg::MyExitToDish(int chose)
{
	if(chose)
		Global::IsToDish = TRUE;
	else
		Global::IsToDish = FALSE;
}
