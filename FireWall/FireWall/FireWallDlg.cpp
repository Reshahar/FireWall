// FireWallDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FireWall.h"
#include "FireWallDlg.h"
#include "define.h"
#include "CTL_CODE.h"
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



int proc = 0;
int reg =0;
int file =0;
int net=0;
HANDLE hProcess=NULL;
HANDLE hReg=NULL;
HANDLE hFile =NULL;
HANDLE hNet=NULL;
HANDLE hProcCreateEvent=NULL;
HANDLE hProcExitEvent=NULL;
HANDLE hRegSetEvent=NULL;
HANDLE hRegDelEvent=NULL;
HANDLE hFileCreateEvent=NULL;
HANDLE hFileDeleteEvent=NULL;
HANDLE hNetPassEvent=NULL;
HANDLE hNetDenyEvent=NULL;
HANDLE proc_exit_event;
HANDLE reg_exit_event;
HANDLE file_exit_event;
HANDLE net_exit_event;
CBoxDlg * BoxList[MAX_BOX];
char SSDT_SERVICE_NAME[MAX_PATH]={0};



UINT ProcessThread(LPVOID  p);
UINT ProcessExitThread(LPVOID  p);
UINT RegThread(LPVOID  p);
UINT RegExitThread(LPVOID p);
void WriteLogToFile(PMSG_STRUCT pMsg,int CHOSE);
UINT FileThread(LPVOID  p);
UINT NetThread(LPVOID  p);
UINT NetExitThread(LPVOID  p);
int GetNULLBox();
void InitBoxList();
void MyMoveWinodow(CBoxDlg * pDlg,CFireWallDlg * pthis);

/////////////////////////////////////////////////////////////////////////////
// CFireWallDlg dialog

CFireWallDlg::CFireWallDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFireWallDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFireWallDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFireWallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFireWallDlg)
	DDX_Control(pDX, IDC_TAB, m_tab);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFireWallDlg, CDialog)
	//{{AFX_MSG_MAP(CFireWallDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_MESSAGE(WM_MYMINMENU,OnMyMin)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFireWallDlg message handlers

BOOL CFireWallDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	int ret1,ret2;
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if(!GetSSDTNAME())
	{
		if(!ReInstall())
		{
			AfxMessageBox("启动失败,请检查文件是否损坏!");
			EndDialog(0);
			return FALSE;
		}
		if(!GetSSDTNAME())
		{
			AfxMessageBox("启动失败,请重新启动!");
			EndDialog(0);
			return FALSE;
		}		
	}

	if(!OpenDriver())
	{
		AfxMessageBox("启动失败,请重启系统再试!");
		EndDialog(0);
		return FALSE;	
	}

	Global::ReadINIFile();	
	reg = 0;
	proc=0;
	file =0;
	net =0;

	ret1 = SendCurrentPath();	
	ret2 = SendNetCurrentPath();

	if(ret1==-1)
	{
		if(!ReInstall())
		{
			AfxMessageBox("请重新启动!");
			EndDialog(0);
			return FALSE;
		}
	}

	if(ret2==-1)
	{
		InstallNet();
		AfxMessageBox("请重新启动!");
		EndDialog(0);
		return FALSE;
	}
	AfxBeginThread(ProcessThread,this);
	AfxBeginThread(RegThread,this);	
	AfxBeginThread(FileThread,this);	
	AfxBeginThread(NetThread,NULL);


	//InitBoxList();

	SSDT_START();
	
	// TODO: Add extra initialization here
	Init();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFireWallDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFireWallDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFireWallDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CFireWallDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CFireWallDlg::Init()
{
	int h,w;
	CRect rect;
	GetClientRect(rect);
	TCITEM item;	
	item.mask = TCIF_TEXT;
	item.pszText = "主页";
	m_tab.InsertItem(0,&item);
	item.pszText = "进程";
	m_tab.InsertItem(1,&item);
	item.pszText = "注册表";
	m_tab.InsertItem(2,&item);
	item.pszText = "文件";
	m_tab.InsertItem(3,&item);
	item.pszText = "网络";
	m_tab.InsertItem(4,&item);
	item.pszText = "工具";
	m_tab.InsertItem(5,&item);
	
	m_index.Create(IDD_INDEX_DIALOG,&m_tab);
	m_process.Create(IDD_PROCESS_DIALOG,&m_tab);
	m_reg.Create(IDD_REG_DIALOG,&m_tab);
	m_file.Create(IDD_FILE_DIALOG,&m_tab);
	m_net.Create(IDD_NET_DIALOG,&m_tab);
	m_tool.Create(IDD_TOOL_DIALOG,&m_tab);
	h = rect.bottom-rect.top;
	w = rect.right - rect.left;
	m_index.SetWindowPos(NULL,8,25,w,h,SWP_SHOWWINDOW);
	m_process.SetWindowPos(NULL,10,20,w,h,SWP_HIDEWINDOW);
	m_reg.SetWindowPos(NULL,10,20,w,h,SWP_HIDEWINDOW);
	m_file.SetWindowPos(NULL,10,20,w,h,SWP_HIDEWINDOW);
	m_net.SetWindowPos(NULL,10,20,w,h,SWP_HIDEWINDOW);
	m_tool.SetWindowPos(NULL,10,20,w,h,SWP_HIDEWINDOW);

	m_nid.cbSize  = (DWORD)sizeof(NOTIFYICONDATA);
    m_nid.hWnd    = this->m_hWnd;
    m_nid.uID     = IDR_MAINFRAME;
    m_nid.uFlags  = NIF_ICON | NIF_MESSAGE | NIF_TIP ;
    m_nid.uCallbackMessage = WM_MYMINMENU;             
    m_nid.hIcon   = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
    strcpy(m_nid.szTip, "FireWall");               
    Shell_NotifyIcon(NIM_ADD, &m_nid);    
}

void CFireWallDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	switch(m_tab.GetCurSel())
	{
	case 0:
		m_index.ShowWindow(SW_SHOW);
		m_process.ShowWindow(SW_HIDE);
		m_reg.ShowWindow(SW_HIDE);
		m_file.ShowWindow(SW_HIDE);
		m_net.ShowWindow(SW_HIDE);
		m_tool.ShowWindow(SW_HIDE);
		break;
	case 1:
		m_process.UpDateProcMsg();
		m_process.ShowWindow(SW_SHOW);
		m_reg.ShowWindow(SW_HIDE);
		m_file.ShowWindow(SW_HIDE);
		m_index.ShowWindow(SW_HIDE);
		m_net.ShowWindow(SW_HIDE);
		m_tool.ShowWindow(SW_HIDE);
		break;
	case 2:
		m_process.ShowWindow(SW_HIDE);
		m_reg.ShowWindow(SW_SHOW);
		m_file.ShowWindow(SW_HIDE);
		m_index.ShowWindow(SW_HIDE);
		m_net.ShowWindow(SW_HIDE);
		m_tool.ShowWindow(SW_HIDE);
		break;
	case 3:
		m_file.ShowWindow(SW_SHOW);
		m_process.ShowWindow(SW_HIDE);
		m_reg.ShowWindow(SW_HIDE);
		m_index.ShowWindow(SW_HIDE);
		m_net.ShowWindow(SW_HIDE);
		m_tool.ShowWindow(SW_HIDE);
		break;
	case 4:
		m_file.ShowWindow(SW_HIDE);
		m_process.ShowWindow(SW_HIDE);
		m_reg.ShowWindow(SW_HIDE);
		m_index.ShowWindow(SW_HIDE);
		m_net.ShowWindow(SW_SHOW);
		m_tool.ShowWindow(SW_HIDE);
		break;
	case 5:
		m_file.ShowWindow(SW_HIDE);
		m_process.ShowWindow(SW_HIDE);
		m_reg.ShowWindow(SW_HIDE);
		m_index.ShowWindow(SW_HIDE);
		m_net.ShowWindow(SW_HIDE);
		m_tool.ShowWindow(SW_SHOW);
		break;
	}
	*pResult = 0;
}
UINT ProcessExitThread(LPVOID p)
{
	DWORD ret;
	proc = 1;
	DeviceIoControl(hProcess,MY_PROCESS_EXIT_CTL_CODE,NULL,0,NULL,0,&ret,NULL);
	return 0;
}
UINT ProcessThread(LPVOID  p) 
{ 
	CFireWallDlg *pthis = (CFireWallDlg *)p;
	CString str;
	int index;
	HANDLE HandleList[2];
	int chose = MY_THREAD_DEFAULT;
	char charbuf[4096]={0};
	hProcess =  CreateFile("\\\\.\\FIREWALLSSDT",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(hProcess==INVALID_HANDLE_VALUE)
	{
		hProcess =NULL;
		return -1;
	}
	hProcCreateEvent = CreateEvent(NULL,TRUE,FALSE,"");
	hProcExitEvent = CreateEvent(NULL,TRUE,FALSE,"");
	//create
	OVERLAPPED over_create;
	over_create.hEvent = hProcCreateEvent;
	over_create.Offset = 0;
	over_create.OffsetHigh = 0;
	DWORD ret_create;
	//exit
	OVERLAPPED over_exit;
	over_exit.hEvent = hProcExitEvent;
	over_exit.Offset = 0;
	over_exit.OffsetHigh = 0;
	DWORD ret_exit;


	chose = MY_PROCESS_ALL_THREAD;

	while(1)
	{	
		switch(chose) 
		{
		case MY_PROCESS_CREATE_THREAD:
			DeviceIoControl(hProcess,MY_PROCESS_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_create,&over_create);
			break;
		case MY_PROCESS_EXIT_THREAD:
			DeviceIoControl(hProcess,MY_PROCESS_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_exit,&over_exit);
			break;
		case MY_PROCESS_ALL_THREAD:
			chose = MY_PROCESS_CREATE_THREAD;
			DeviceIoControl(hProcess,MY_PROCESS_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_create,&over_create);
			Sleep(100);
			chose = MY_PROCESS_EXIT_THREAD;
			DeviceIoControl(hProcess,MY_PROCESS_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_exit,&over_exit);
			break;
		}
		HandleList[0]=hProcCreateEvent;
		HandleList[1]=hProcExitEvent;


		DWORD EventResult =  WaitForMultipleObjects(2,HandleList,FALSE,INFINITE);

		switch(EventResult) 
		{
		case WAIT_OBJECT_0+0:
		case WAIT_OBJECT_0+1:
			{
				if(proc==0)
				{
					switch(((PMSG_STRUCT)&charbuf)->flag)
					{
					case PROCESS_CREATE:
						chose = MY_PROCESS_CREATE_THREAD;
						break;
					case PROCESS_EXIT:
						chose = MY_PROCESS_EXIT_THREAD;
						break;
					}
					WriteLogToFile((PMSG_STRUCT)&charbuf,PROCESS_CHOSE);
					CBoxDlg box((PMSG_STRUCT)&charbuf);
					box.DoModal();	
					break;
				}else
				{
					ResetEvent(hProcCreateEvent);
					CancelIo(hProcess);
					CloseHandle(hProcCreateEvent);
					CloseHandle(hProcess);
					hProcess = NULL;
					SetEvent(proc_exit_event);
					return 1;
				}
			}
		case WAIT_TIMEOUT:
		CancelIo(hProcess);
		AfxMessageBox("WAIT_TIMEOUT");
			break;
		default:
			;
		}
	}
	return 0;
}
UINT RegExitThread(LPVOID p)
{
	DWORD ret;
	reg = 1;
	DeviceIoControl(hReg,MY_REG_EXIT_CTL_CODE,NULL,0,NULL,0,&ret,NULL);		
	return 0;
}
UINT RegThread(LPVOID  p) 
{ 
	CString str;
	HANDLE HandleList[2];
	int chose = MY_THREAD_DEFAULT;
	char charbuf[4096]={0};

	hReg =  CreateFile("\\\\.\\FIREWALLSSDT",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(hReg==INVALID_HANDLE_VALUE)
	{
		hReg =NULL;
	
		return -1;
	}
	hRegSetEvent = CreateEvent(NULL,TRUE,FALSE,"");
	hRegDelEvent = CreateEvent(NULL,TRUE,FALSE,"");


	OVERLAPPED over_set;
	over_set.hEvent = hRegSetEvent;
	over_set.Offset = 0;
	over_set.OffsetHigh = 0;
	DWORD ret_set;

	OVERLAPPED over_del;
	over_del.hEvent = hRegDelEvent;
	over_del.Offset = 0;
	over_del.OffsetHigh = 0;
	DWORD ret_del;
	
	chose = MY_REG_ALL_THREAD;

	while(1)
	{	
		switch(chose) 
		{
		case MY_REG_SET_THREAD:
			DeviceIoControl(hReg,MY_REG_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_set,&over_set);
			break;
		case MY_REG_DEL_THREAD:
			DeviceIoControl(hReg,MY_REG_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_del,&over_del);
			break;
		case MY_REG_ALL_THREAD:
			chose = MY_REG_SET_THREAD;
			DeviceIoControl(hReg,MY_REG_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_set,&over_set);
			Sleep(100);
			chose = MY_REG_DEL_THREAD;
			DeviceIoControl(hReg,MY_REG_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_del,&over_del);
			break;
		}
		HandleList[0]=hRegSetEvent;
		HandleList[1]=hRegDelEvent;
	
		DWORD EventResult =  WaitForMultipleObjects(2,HandleList,FALSE,INFINITE);

		switch(EventResult) 
		{
		case WAIT_OBJECT_0+0:
		case WAIT_OBJECT_0+1:
			{
				if(reg==0)
				{
					switch(((PMSG_STRUCT)&charbuf)->flag)
					{
					case REG_SET_VALUE:
						chose = MY_REG_SET_THREAD;
						break;
					case REG_DEL_VALUE:
						chose = MY_REG_DEL_THREAD;
						break;
					}
					CBoxDlg box((PMSG_STRUCT)&charbuf);
					box.DoModal();
					WriteLogToFile((PMSG_STRUCT)&charbuf,REG_CHOSE);
					break;
				}else
				{
					ResetEvent(hRegSetEvent);
					CancelIo(hReg);
					CloseHandle(hRegSetEvent);
					CloseHandle(hReg);
					hProcess = NULL;
					SetEvent(reg_exit_event);
					return 1;
				}
			}
		case WAIT_TIMEOUT:
		CancelIo(hReg);
		AfxMessageBox("WAIT_TIMEOUT");
			break;
		default:
			;
		}
	}
	return 0;
}
UINT FileExitThread(LPVOID p)
{
	DWORD ret;
	file = 1;
	DeviceIoControl(hFile,MY_FILE_EXIT_CTL_CODE,NULL,0,NULL,0,&ret,NULL);		
	return 0;
}
UINT FileThread(LPVOID  p) 
{ 
	CString str;
	HANDLE HandleList[2];
	int chose = MY_THREAD_DEFAULT;
	char charbuf[4096]={0};

	hFile =  CreateFile("\\\\.\\FIREWALLSSDT",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		hFile =NULL;
		return -1;
	}
	hFileCreateEvent = CreateEvent(NULL,TRUE,FALSE,"");
	hFileDeleteEvent = CreateEvent(NULL,TRUE,FALSE,"");


	OVERLAPPED over_create;
	over_create.hEvent = hFileCreateEvent;
	over_create.Offset = 0;
	over_create.OffsetHigh = 0;
	DWORD ret_create;

	OVERLAPPED over_del;
	over_del.hEvent = hFileDeleteEvent;
	over_del.Offset = 0;
	over_del.OffsetHigh = 0;
	DWORD ret_del;
	
	chose = MY_FILE_ALL_THREAD;

	while(1)
	{	
		switch(chose) 
		{
		case MY_FILE_CREATE_THREAD:
			DeviceIoControl(hFile,MY_FILE_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_create,&over_create);
			break;
		case MY_FILE_DELETE_THREAD:
			DeviceIoControl(hFile,MY_FILE_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_del,&over_del);
			break;
		case MY_FILE_ALL_THREAD:
			chose = MY_FILE_CREATE_THREAD;
			DeviceIoControl(hFile,MY_FILE_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_create,&over_create);
			Sleep(100);
			chose = MY_FILE_DELETE_THREAD;
			DeviceIoControl(hFile,MY_FILE_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_del,&over_del);
			break;
		}
		HandleList[0]=hFileCreateEvent;
		HandleList[1]=hFileDeleteEvent;
	
		DWORD EventResult =  WaitForMultipleObjects(2,HandleList,FALSE,INFINITE);

		switch(EventResult) 
		{
		case WAIT_OBJECT_0+0:
		case WAIT_OBJECT_0+1:
			{
				if(file==0)
				{
					switch(((PMSG_STRUCT)&charbuf)->flag)
					{
					case MYFILE_CREATE:
						chose = MY_FILE_CREATE_THREAD;
						break;
					case MYFILE_DELETE:
						chose = MY_FILE_DELETE_THREAD;
						break;
					}
					CBoxDlg box((PMSG_STRUCT)&charbuf);
					box.DoModal();
					WriteLogToFile((PMSG_STRUCT)&charbuf,FILE_CHOSE);
					break;
				}else
				{
					ResetEvent(hFileCreateEvent);
					CancelIo(hFile);
					CloseHandle(hFileCreateEvent);
					CloseHandle(hFile);
					hFile = NULL;
					SetEvent(file_exit_event);
					return 1;
				}
			}
		case WAIT_TIMEOUT:
		CancelIo(hFile);
		AfxMessageBox("WAIT_TIMEOUT");
			break;
		default:
			;
		}
	}
	return 0;
}


bool CFireWallDlg::CreateSC()
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
	strcpy(strPath,DRIVER_PATH);
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
	
			return OpenDriver();
		}
		else
		{
			CloseServiceHandle(sc_hManager);
		}
		return false;
	}else
	{
		return StartDriver(sc_hManager,sc_hResult);
	}
}

bool CFireWallDlg::StartDriver(SC_HANDLE sc_hManager,SC_HANDLE sc_hResult)
{
	if(StartService(sc_hResult,NULL,NULL))
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

bool CFireWallDlg::OpenDriver()
{
	SERVICE_STATUS ssStatus;
	SC_HANDLE sc_hManager = NULL;
	SC_HANDLE sc_hResult = NULL;
	sc_hManager = OpenSCManager (NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(NULL == sc_hManager)
	{
		return false;
	}
	sc_hResult = OpenService(sc_hManager,SSDT_SERVICE_NAME,SERVICE_ALL_ACCESS);
	
	if(sc_hResult)
	{
		if(QueryServiceStatus(sc_hResult,&ssStatus))
		{
			if(ssStatus.dwCurrentState==SERVICE_STOPPED)
			{
				if(StartDriver(sc_hManager,sc_hManager))
				{
					CloseServiceHandle(sc_hResult);
					CloseServiceHandle(sc_hManager);
					return true;
				}else
				{
					CloseServiceHandle(sc_hResult);
					CloseServiceHandle(sc_hManager);
					return false;
				}
			}else
			{
				return true;
			}
		}else
		{
			return false;
		}

	}else
	{
		return false;
	}
}


bool CFireWallDlg::StopDriver()
{
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
	SERVICE_STATUS SerStat;
	if(!ControlService(sc_hResult,SERVICE_CONTROL_STOP,&SerStat))
	{
		return false;
	}else
	{
		return DeleteSC(sc_hManager,sc_hResult);
	}
}

bool CFireWallDlg::DeleteSC(SC_HANDLE sc_hManager, SC_HANDLE sc_hResult)
{
	if(DeleteService(sc_hResult))
	{
		//AfxMessageBox("驱动卸载成功！");
		CloseServiceHandle(sc_hResult);
		CloseServiceHandle(sc_hManager);
		return true;
	}
	else
	{
	//	AfxMessageBox("驱动没有卸载成功 ！");
		CloseServiceHandle(sc_hResult);
		CloseServiceHandle(sc_hManager);
		return false;
	}
}

BOOL CFireWallDlg::DestroyWindow() 
{
	return CDialog::DestroyWindow();
}

void CFireWallDlg::OnClose() 
{
	if(Global::IsToDish)
	{
		SendMessage(WM_SIZE,SIZE_MINIMIZED);	
	}else
	{
		AfxBeginThread(ProcessExitThread,NULL);
		AfxBeginThread(RegExitThread,NULL);
		AfxBeginThread(FileExitThread,NULL);
		AfxBeginThread(NetExitThread,NULL);
		HANDLE arr[4];
		arr[0]=proc_exit_event;
		arr[1]=reg_exit_event;
		arr[2]=file_exit_event;
		arr[3]=net_exit_event;
		WaitForMultipleObjects(4,arr,TRUE,INFINITE);	
		AfxMessageBox("程序正在退出");
 		//StopDriver();
		Global::WriteINIFile();
		CDialog::OnClose();
	}
}

void WriteLogToFile(PMSG_STRUCT pMsg,int CHOSE)
{	
	CStdioFile logFile;
	char buf[MAX_PATH];
	CString ProcessFile,tmp;
	CTime t;
	GetCurrentDirectory(MAX_PATH,buf);
	strcat(buf,BASE_PATH);
	switch(CHOSE) {
	case PROCESS_CHOSE:
		strcat(buf,PROCESS_LOG_FILE);
		break;
	case REG_CHOSE:
		strcat(buf,REG_LOG_FILE);
		break;
	case FILE_CHOSE:
		strcat(buf,FILE_LOG_FILE);
		break;
	case NET_CHOSE:
		strcat(buf,NET_LOG_FILE);
	break;

	}	
	ProcessFile.Format("%s",buf);
	if(logFile.Open(ProcessFile,CFile::modeWrite))
	{
		logFile.SeekToEnd();
		t =  CTime::GetCurrentTime();
		tmp.Format("%04d年%02d月%02d日%02d:%02d:%02d",t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),t.GetSecond());
		logFile.WriteString(tmp);
		logFile.WriteString("|");
		logFile.WriteString(pMsg->ProcessName);
		logFile.WriteString("|");
		tmp.Format("%d",pMsg->Pid);
		logFile.WriteString(tmp);
		logFile.WriteString("|");
		logFile.WriteString(pMsg->OtherMsg);
		logFile.WriteString("|");
		switch(pMsg->flag) {
		case REG_SET_VALUE:
			logFile.WriteString("添加");
			logFile.WriteString("|");
			break;
		case PROCESS_CREATE:
		case MYFILE_CREATE:
			logFile.WriteString("创建");
			logFile.WriteString("|");
			break;
		case PROCESS_EXIT:
			logFile.WriteString("结束");
			logFile.WriteString("|");
			break;
		case REG_DEL_VALUE:
		case MYFILE_DELETE:
			logFile.WriteString("删除");
			logFile.WriteString("|");
			break;
		case NET_PASS:
			logFile.WriteString("通过");
			logFile.WriteString("|");
			break;
		case NET_DENY:
			logFile.WriteString("禁止");
			logFile.WriteString("|");
			break;
		}
		logFile.WriteString("\n");
		logFile.Close();
	}
}

int CFireWallDlg::SendCurrentPath()
{
	DWORD ret;
	char buf[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,buf);
	strcat(buf,BASE_PATH);
	HANDLE h =  CreateFile("\\\\.\\FIREWALLSSDT",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(h==INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox("Open Failed!!");
		return -1;
	}
	DeviceIoControl(h,MY_CURRENT_PATH_CTL_CODE,&buf,sizeof(buf),NULL,0,&ret,NULL);
	CloseHandle(h);
}

LRESULT CFireWallDlg::OnMyMin(WPARAM wParam, LPARAM lParam)
{
	if(wParam != IDR_MAINFRAME)
		  return 1;
	switch(lParam)
	{
	case WM_RBUTTONUP:                                        // 右键起来时弹出菜单
		  {
        
				 LPPOINT lpoint = new tagPOINT;
				 ::GetCursorPos(lpoint);                    // 得到鼠标位置
				 CMenu menu;
				 menu.CreatePopupMenu();                    // 声明一个弹出式菜单
				 menu.AppendMenu(MF_STRING, WM_USER+101, "关闭");
				 menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x ,lpoint->y, this);
				 HMENU hmenu = menu.Detach();
				 menu.DestroyMenu();
				 delete lpoint;
		  }
		  break;
	case WM_LBUTTONDBLCLK:                                 // 双击左键的处理
		  {
				 this->ShowWindow(SW_SHOWNORMAL);         // 显示主窗口'
		  }
		  break;
	}
	return 0;
}


void CFireWallDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(nType == SIZE_MINIMIZED)  
	{  
		  ShowWindow(SW_HIDE); 
	}  
}


void CFireWallDlg::OnDestroy() 
{
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
	CDialog::OnDestroy();
}

BOOL CFireWallDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	UINT uMsg=LOWORD(wParam);
	if(uMsg==WM_USER+101)
	{
		Global::IsToDish = FALSE;
		SendMessage(WM_CLOSE);
	}
	return CDialog::OnCommand(wParam, lParam);
}

UINT NetExitThread(LPVOID p)
{
	DWORD ret;
	net = 1;
	DeviceIoControl(hNet,MY_NET_EXIT_CTL_CODE,NULL,0,NULL,0,&ret,NULL);		
	return 0;
}

UINT NetThread(LPVOID  p) 
{ 
	CString str;
	HANDLE HandleList[2];
	int chose = MY_THREAD_DEFAULT;
	char charbuf[4096]={0};

	hNet =  CreateFile("\\\\.\\FIREWALLNDIS",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(hNet==INVALID_HANDLE_VALUE)
	{
		hNet =NULL;
		return -1;
	}
	hNetPassEvent = CreateEvent(NULL,TRUE,FALSE,"");
	hNetDenyEvent = CreateEvent(NULL,TRUE,FALSE,"");


	OVERLAPPED over_pass;
	over_pass.hEvent = hNetPassEvent;
	over_pass.Offset = 0;
	over_pass.OffsetHigh = 0;
	DWORD ret_pass;

	OVERLAPPED over_deny;
	over_deny.hEvent = hNetDenyEvent;
	over_deny.Offset = 0;
	over_deny.OffsetHigh = 0;
	DWORD ret_deny;
	DWORD ret_start;
	chose = MY_NET_ALL_THREAD;

	DeviceIoControl(hNet,MY_NET_ISSTART_CTL_CODE,NULL,0,NULL,0,&ret_start,NULL);

	while(1)
	{	
		//ZeroMemory(&charbuf,MAXPATH*4);
		switch(chose) 
		{
		case MY_NET_PASS_THREAD:
			DeviceIoControl(hNet,MY_NET_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_pass,&over_pass);
			break;
		case MY_NET_DENY_THREAD:
			DeviceIoControl(hNet,MY_NET_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_deny,&over_deny);
			break;
		case MY_NET_ALL_THREAD:
			chose = MY_NET_PASS_THREAD;
			DeviceIoControl(hNet,MY_NET_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_pass,&over_pass);
			Sleep(100);
			chose = MY_NET_DENY_THREAD;
			DeviceIoControl(hNet,MY_NET_CTL_CODE,&chose,sizeof(chose),&charbuf,4096,&ret_deny,&over_deny);
			break;
		}
		HandleList[0]=hNetPassEvent;
		HandleList[1]=hNetDenyEvent;
	
		DWORD EventResult =  WaitForMultipleObjects(2,HandleList,FALSE,INFINITE);

		switch(EventResult) 
		{
		case WAIT_OBJECT_0+0:
		case WAIT_OBJECT_0+1:
			{
				if(net==0)
				{
					switch(((PMSG_STRUCT)&charbuf)->flag)
					{
					case NET_PASS:
						chose = MY_NET_PASS_THREAD;
						break;
					case NET_DENY:
						chose = MY_NET_DENY_THREAD;
						break;
					}
					WriteLogToFile((PMSG_STRUCT)&charbuf,NET_CHOSE);
					break;
				}else
				{
					ResetEvent(hNetPassEvent);
					CancelIo(hNet);
					CloseHandle(hNetPassEvent);
					CloseHandle(hNetDenyEvent);
					CloseHandle(hNet);
					hNet = NULL;
					SetEvent(net_exit_event);
					return 1;
				}
			}
		case WAIT_TIMEOUT:
		CancelIo(hNet);
		AfxMessageBox("WAIT_TIMEOUT");
			break;
		default:
			;
		}
	}
	return 0;
}

int CFireWallDlg::SendNetCurrentPath()
{
	DWORD ret;
	char buf[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,buf);
	strcat(buf,BASE_PATH);
	HANDLE h =  CreateFile("\\\\.\\FIREWALLNDIS",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(h==INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox("Open Failed!!");
		return -1;
	}
	DeviceIoControl(h,MY_CURRENT_PATH_CTL_CODE,&buf,sizeof(buf),NULL,0,&ret,NULL);
	CloseHandle(h);
}

void CFireWallDlg::SSDT_START()
{
	DWORD ret;
	HANDLE h =  CreateFile("\\\\.\\FIREWALLSSDT",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(h==INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox("Open Failed!!");
		return;
	}
	DeviceIoControl(h,MY_SSDT_IS_START_CTL_CODE,NULL,0,NULL,0,&ret,NULL);
	CloseHandle(h);
}
void InitBoxList()
{
	int i;
	for(i=0;i<MAX_BOX;i++)
	{
		BoxList[i]=NULL;
	}
}
int GetNULLBox()
{
	int i;
	for(i=0;i<MAX_BOX;i++)
	{
		if(BoxList[i]==NULL)
		{
			break;
		}
	}
	return i;
}
void MyMoveWinodow(CBoxDlg * pDlg,CFireWallDlg * pthis)
{
	CRect rcDlgs;
	pDlg->GetWindowRect(rcDlgs);					//得到对话框的Rect 对话框的大小
	pthis->ScreenToClient(rcDlgs);					//把屏幕的值转成相应的实际的值
	int  cx  =  GetSystemMetrics(  SM_CXSCREEN  );  //获得屏幕的分辨率
	int  cy  =  GetSystemMetrics(  SM_CYSCREEN  );  
	pDlg->SetWindowPos(pthis,cx-rcDlgs.Width(),cy-rcDlgs.Height()-30,rcDlgs.Width(),rcDlgs.Height(),SWP_NOSIZE);
}

bool CFireWallDlg::InstallNet()
{
	char path[MAX_PATH];
	STARTUPINFO si = { sizeof(si) }; 
    PROCESS_INFORMATION pi;   
 
    si.dwFlags = STARTF_USESHOWWINDOW;   
    si.wShowWindow = SW_HIDE;
	GetCurrentDirectory(MAX_PATH,path);
	strcat(path,"\\sys\\netstart.exe");
 
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

bool CFireWallDlg::UnInstallNet()
{
	char path[MAX_PATH];
	STARTUPINFO si = { sizeof(si) }; 
    PROCESS_INFORMATION pi;   
 
    si.dwFlags = STARTF_USESHOWWINDOW;   
    si.wShowWindow = SW_HIDE;
	GetCurrentDirectory(MAX_PATH,path);
	strcat(path,"\\sys\\uninstall.exe");
 
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

bool CFireWallDlg::ReInstall()
{
	char path[MAX_PATH];
	STARTUPINFO si = { sizeof(si) }; 
    PROCESS_INFORMATION pi;   
 
    si.dwFlags = STARTF_USESHOWWINDOW;   
    si.wShowWindow = SW_HIDE;
	GetCurrentDirectory(MAX_PATH,path);
	strcat(path,"\\Install.exe");
 
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

bool CFireWallDlg::GetSSDTNAME()
{
	CStdioFile File;
	CString ServiceName,GlobalFilePath;
	int i,num;
	char buf[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,buf);
 	GlobalFilePath.Format("%s%s%s",buf,BASE_PATH,SERVICE_NAME_FILE);
	if(File.Open(GlobalFilePath,CStdioFile::modeRead))
	{
		if(File.GetLength()!=0)
		{
			File.ReadString(SSDT_SERVICE_NAME,MAX_PATH);
			//AfxMessageBox(SSDT_SERVICE_NAME);
			File.Close();
			return true;
		}else
		{
			File.Close();
			return false;
		}
	}else
	{
		//AfxMessageBox(_T("文件打开错误!"));
		return false;
	}
}
