// BoxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FireWall.h"
#include "BoxDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int count;
/////////////////////////////////////////////////////////////////////////////
// CBoxDlg dialog


CBoxDlg::CBoxDlg(PMSG_STRUCT pMsg,CWnd* pParent /*=NULL*/)
	: CDialog(CBoxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBoxDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	count=4;
	switch(pMsg->flag)
	{
	case PROCESS_CREATE:
		t.Format("父进程   :%s\r\n父进程PID:%d\r\n子进程   :%s\r\n行为   :%s\r\n",pMsg->ProcessName,pMsg->Pid,pMsg->OtherMsg,"进程创建");
		break;
	case PROCESS_EXIT:
		t.Format("父进程   :%s\r\n父进程PID:%d\r\n子进程   :%s\r\n行为   :%s\r\n",pMsg->ProcessName,pMsg->Pid,pMsg->OtherMsg,"进程结束");
		break;
	case REG_SET_VALUE:
		t.Format("进程     :%s\r\n进程PID:%d\r\n注册表   :%s\r\n行为   :%s\r\n",pMsg->ProcessName,pMsg->Pid,pMsg->OtherMsg,"添加注册表");
		break;
	case REG_DEL_VALUE:
		t.Format("进程     :%s\r\n进程PID:%d\r\n注册表   :%s\r\n行为   :%s\r\n",pMsg->ProcessName,pMsg->Pid,pMsg->OtherMsg,"删除注册表");
		break;
	case MYFILE_CREATE:
		t.Format("进程   :%s\r\n进程PID:%d\r\n文件路径 :%s\r\n行为   :%s\r\n",pMsg->ProcessName,pMsg->Pid,pMsg->OtherMsg,"创建文件");
		break;
	case MYFILE_DELETE:
		t.Format("进程   :%s\r\n进程PID:%d\r\n文件路径 :%s\r\n行为   :%s\r\n",pMsg->ProcessName,pMsg->Pid,pMsg->OtherMsg,"删除文件");
		break;
	case NET_DENY:
		t.Format("进程   :%s\r\n进程PID:%d\r\nIP地址   :%s\r\n行为   :%s\r\n",pMsg->ProcessName,pMsg->Pid,pMsg->OtherMsg,"禁止");
		break;
	case NET_PASS:
		t.Format("进程   :%s\r\n进程PID:%d\r\nIP地址   :%s\r\n行为   :%s\r\n",pMsg->ProcessName,pMsg->Pid,pMsg->OtherMsg,"允许");
		break;
	}
}


void CBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBoxDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBoxDlg, CDialog)
	//{{AFX_MSG_MAP(CBoxDlg)
	ON_BN_CLICKED(IDC_MY_CLOSE, OnMyClose)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SEE, OnSee)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBoxDlg message handlers

void CBoxDlg::OnOK() 
{
	// TODO: Add extra validation here
	DestroyWindow();
	CDialog::OnOK();
}

void CBoxDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CBoxDlg::OnMyClose() 
{
	// TODO: Add your control notification handler code here
	EndDialog(0);
}

BOOL CBoxDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rcDlgs;
	this->GetWindowRect(rcDlgs);					//得到对话框的Rect 对话框的大小
	GetParent()->ScreenToClient(rcDlgs);					//把屏幕的值转成相应的实际的值
	int  cx  =  GetSystemMetrics(  SM_CXSCREEN  );  //获得屏幕的分辨率
	int  cy  =  GetSystemMetrics(  SM_CYSCREEN  );  
	this->SetWindowPos(GetParent(),cx-rcDlgs.Width(),cy-rcDlgs.Height()-30,rcDlgs.Width(),rcDlgs.Height(),SWP_NOSIZE);

	// TODO: Add extra initialization here
	SetDlgItemText(IDC_TEXT,t);
	SetTimer(1,1000,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBoxDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CString str;
	str.Format("关闭(%d)",count);
	SetDlgItemText(IDC_MY_CLOSE,str);
	if(count==0)
		EndDialog(0);
	count--;
	CDialog::OnTimer(nIDEvent);
}

void CBoxDlg::OnSee() 
{
	// TODO: Add your control notification handler code here
	GetParent()->ShowWindow(SW_SHOW);
	EndDialog(0);
}

