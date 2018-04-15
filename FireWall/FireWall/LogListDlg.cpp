// LogListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FireWall.h"
#include "LogListDlg.h"
#include "define.h"
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogListDlg dialog


CLogListDlg::CLogListDlg(CWnd* pParent /*=NULL*/,int chose)
: CDialog(CLogListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogListDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_chose = chose;
}


void CLogListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogListDlg)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLogListDlg, CDialog)
//{{AFX_MSG_MAP(CLogListDlg)
ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_BN_CLICKED(IDC_DEL_LOG, OnDelLog)
	ON_BN_CLICKED(IDC_RE_READ, OnReRead)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogListDlg message handlers

void CLogListDlg::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW *pNMListCtrl = (NM_LISTVIEW *)pNMHDR;  
    CString tmp;
	CString head[6]={"ID   :","时间 :","父进程:","父PID :","子进程:	","行为: "};
	
	int i;
	if(pNMListCtrl->iItem != -1)  
    {  
		switch(m_chose) {
		case PROCESS_CHOSE:
			head[4]="子进程:	";
			break;
		case REG_CHOSE:
			head[4]="注册表:	";
			break;
		case FILE_CHOSE:
			head[4]="文件路径:  ";
			break;
		case NET_CHOSE:
			head[2]="源IP:    ";
			head[4]="目的IP:    ";
		}
		for(i=0;i<m_list.GetHeaderCtrl()->GetItemCount();i++)
		{
			tmp+=head[i];
			tmp+=m_list.GetItemText(pNMListCtrl->iItem,i);
			tmp+="\n";
		}
		AfxMessageBox(tmp);
    }    
	*pResult = 0;
}

void CLogListDlg::InitProcess()
{
	LONG style = GetWindowLong(m_list.m_hWnd,GWL_STYLE);
	style &=~LVS_TYPEMASK;
	style |=LVS_REPORT;
	SetWindowLong(m_list.m_hWnd,GWL_STYLE,style);
	DWORD liststyle = m_list.GetExtendedStyle();
	liststyle |=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_list.SetExtendedStyle(liststyle);
	m_list.InsertColumn(0,"编号",LVCFMT_LEFT,50);
	m_list.InsertColumn(1,"时间",LVCFMT_LEFT,180);
	m_list.InsertColumn(2,"父进程",LVCFMT_LEFT,140);
	m_list.InsertColumn(3,"父进程ID",LVCFMT_LEFT,50);
	m_list.InsertColumn(4,"子进程",LVCFMT_LEFT,100);
	m_list.InsertColumn(5,"创建/结束",LVCFMT_LEFT,80);
}

void CLogListDlg::FileToList(int CHOSE)
{
	CStdioFile File;
	CString CSPath,Tmp,tmp;
	char path[MAX_PATH];
	int i,num,f;
	GetCurrentDirectory(MAX_PATH,path);
	strcat(path,BASE_PATH);
	switch(CHOSE) {
	case PROCESS_CHOSE:
		strcat(path,PROCESS_LOG_FILE);
		break;
	case REG_CHOSE:
		strcat(path,REG_LOG_FILE);
		break;
	case FILE_CHOSE:
		strcat(path,FILE_LOG_FILE);
		break;
	case NET_CHOSE:
		strcat(path,NET_LOG_FILE);
		break;
	}
	CSPath.Format("%s",path);
	if(File.Open(CSPath,CStdioFile::modeRead))
	{
		while(File.ReadString(Tmp))
		{
			num = Global::GetSplitNum(Tmp);
			f = m_list.GetItemCount();
			tmp.Format("%d",f+1);
			m_list.InsertItem(f,tmp);
			for(i=0;i<num;i++)
			{	
				m_list.SetItemText(f,i+1,Global::SplitName(Tmp,i));
			}
			
		}	
		File.Close();
	}else
	{
		AfxMessageBox("文件打开错误!");
	}	
}

void CLogListDlg::InitReg()
{
	LONG style = GetWindowLong(m_list.m_hWnd,GWL_STYLE);
	style &=~LVS_TYPEMASK;
	style |=LVS_REPORT;
	SetWindowLong(m_list.m_hWnd,GWL_STYLE,style);
	DWORD liststyle = m_list.GetExtendedStyle();
	liststyle |=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_list.SetExtendedStyle(liststyle);
	m_list.InsertColumn(0,"编号",LVCFMT_LEFT,50);
	m_list.InsertColumn(1,"时间",LVCFMT_LEFT,180);
	m_list.InsertColumn(2,"父进程",LVCFMT_LEFT,140);
	m_list.InsertColumn(3,"父进程ID",LVCFMT_LEFT,50);
	m_list.InsertColumn(4,"注册表项",LVCFMT_LEFT,100);
	m_list.InsertColumn(5,"添加/删除",LVCFMT_LEFT,80);
}

BOOL CLogListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	switch(m_chose) {
	case PROCESS_CHOSE:
		InitProcess();
		break;
	case REG_CHOSE:
		InitReg();
		break;
	case FILE_CHOSE:
		InitFile();
		break;
	case NET_CHOSE:
		InitNet();
		break;
	}
	FileToList(m_chose);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLogListDlg::OnDelLog() 
{
	// TODO: Add your control notification handler code here
	while(m_list.DeleteItem(0));
	char buf[MAXPATH];
	CString path;
	CStdioFile file;
	GetCurrentDirectory(MAXPATH,buf);
	strcat(buf,BASE_PATH);
	switch(m_chose) {
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
	path.Format("%s",buf);
	DeleteFile(path);
	file.Open(path,CFile::modeCreate);
	file.Close();
}

void CLogListDlg::OnReRead() 
{
	// TODO: Add your control notification handler code here
	while(m_list.DeleteItem(0));
	FileToList(m_chose);
}

void CLogListDlg::InitFile()
{
	LONG style = GetWindowLong(m_list.m_hWnd,GWL_STYLE);
	style &=~LVS_TYPEMASK;
	style |=LVS_REPORT;
	SetWindowLong(m_list.m_hWnd,GWL_STYLE,style);
	DWORD liststyle = m_list.GetExtendedStyle();
	liststyle |=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_list.SetExtendedStyle(liststyle);
	m_list.InsertColumn(0,"编号",LVCFMT_LEFT,50);
	m_list.InsertColumn(1,"时间",LVCFMT_LEFT,180);
	m_list.InsertColumn(2,"父进程",LVCFMT_LEFT,140);
	m_list.InsertColumn(3,"父进程ID",LVCFMT_LEFT,50);
	m_list.InsertColumn(4,"文件路径",LVCFMT_LEFT,100);
	m_list.InsertColumn(5,"添加/删除",LVCFMT_LEFT,80);
}

void CLogListDlg::InitNet()
{
	LONG style = GetWindowLong(m_list.m_hWnd,GWL_STYLE);
	style &=~LVS_TYPEMASK;
	style |=LVS_REPORT;
	SetWindowLong(m_list.m_hWnd,GWL_STYLE,style);
	DWORD liststyle = m_list.GetExtendedStyle();
	liststyle |=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_list.SetExtendedStyle(liststyle);
	m_list.InsertColumn(0,"编号",LVCFMT_LEFT,50);
	m_list.InsertColumn(1,"时间",LVCFMT_LEFT,180);
	m_list.InsertColumn(2,"源IP",LVCFMT_LEFT,140);
	m_list.InsertColumn(3,"进程ID",LVCFMT_LEFT,50);
	m_list.InsertColumn(4,"目的IP",LVCFMT_LEFT,100);
	m_list.InsertColumn(5,"禁止/允许",LVCFMT_LEFT,80);
}
