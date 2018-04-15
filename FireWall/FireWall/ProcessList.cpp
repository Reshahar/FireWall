 // ProcessList.cpp : implementation file
//

#include "stdafx.h"
#include "FireWall.h"
#include "ProcessList.h"
#include "define.h"
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcessList dialog


CProcessList::CProcessList(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProcessList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProcessList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcessList)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProcessList, CDialog)
	//{{AFX_MSG_MAP(CProcessList)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessList message handlers

void CProcessList::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CProcessList::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CProcessList::Init()
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
	m_list.InsertColumn(4,"子进程",LVCFMT_LEFT,200);
}

void CProcessList::FileToList()
{
	
	CStdioFile ProcessFile;
	CString CSPath,Tmp,tmp;
	char path[MAX_PATH];
	int i,num,f;
	GetCurrentDirectory(MAX_PATH,path);
	strcat(path,PROCESS_LOG_FILE);
	CSPath.Format("%s",path);
	if(ProcessFile.Open(CSPath,CStdioFile::modeRead))
	{
		while(ProcessFile.ReadString(Tmp))
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
		ProcessFile.Close();
	}else
	{
		AfxMessageBox("文件打开错误!");
	}	
}

BOOL CProcessList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	Init();
	FileToList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProcessList::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW *pNMListCtrl = (NM_LISTVIEW *)pNMHDR;  
    CString tmp;
	CString head[5]={"ID   :","时间 :","父进程:","父PID :","子进程:"};
	int i;
	if(pNMListCtrl->iItem != -1)  
    {  
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
