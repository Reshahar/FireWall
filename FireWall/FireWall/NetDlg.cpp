// NetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FireWall.h"
#include "NetDlg.h"
#include "Global.h"
#include "define.h"
#include "LogListDlg.h"
#include "CTL_CODE.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetDlg dialog


CNetDlg::CNetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNetDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nRow=-1;
	m_nCol=-1;
	UpStatus = -1;
}


void CNetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetDlg)
	DDX_Control(pDX, IDC_EDIT_LIST, m_edit);
	DDX_Control(pDX, IDC_LIST_FILE, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetDlg, CDialog)
	//{{AFX_MSG_MAP(CNetDlg)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, OnBtnDel)
	ON_BN_CLICKED(IDC_BTN_LOG, OnBtnLog)
	ON_BN_CLICKED(IDC_BTN_READ, OnBtnRead)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_BN_CLICKED(IDC_BTN_UPDATE, OnBtnUpdate)
	ON_EN_KILLFOCUS(IDC_EDIT_LIST, OnKillfocusEditList)
	ON_BN_CLICKED(IDC_BTN_UP, OnBtnUp)
	ON_BN_CLICKED(IDC_BTN_DOWN, OnBtnDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetDlg message handlers

void CNetDlg::OnBtnAdd() 
{
	// TODO: Add your control notification handler code here
	CString tmp;
	int n = m_list.GetItemCount();
	tmp.Format("%d",n+1);
	m_list.InsertItem(n,tmp);
	UpStatus=2;
}

void CNetDlg::OnBtnDel() 
{
	// TODO: Add your control notification handler code here
	int n = m_list.GetSelectedCount();
	int i,j;
	for(i=0;i<n;i++)
	{
		for(j=0;j<m_list.GetItemCount();j++)
		{
			if(m_list.GetItemState(j, LVIS_SELECTED) == LVIS_SELECTED)
			{
				m_list.DeleteItem(j);
			}
		}
	}
	AfxMessageBox("删除成功");
}

void CNetDlg::OnBtnLog() 
{
	// TODO: Add your control notification handler code here
	CLogListDlg list(NULL,NET_CHOSE);
	list.DoModal();	
}

void CNetDlg::OnBtnRead() 
{
	// TODO: Add your control notification handler code here
	while(m_list.DeleteItem(0));
	ReadFileToList();
}

void CNetDlg::OnBtnSave() 
{
	// TODO: Add your control notification handler code here

	HANDLE h;
	DWORD ret;
	WriteListToFile();
	h =  CreateFile("\\\\.\\FIREWALLNDIS",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(h==INVALID_HANDLE_VALUE)
	{
		AfxMessageBox("Open Error!");
		return;
	}	
	DeviceIoControl(h,MY_NET_RESTART_CTL_CODE,NULL,0,NULL,0,&ret,NULL);
	AfxMessageBox("正在保存.....");
	UpStatus = -1;
	
}

void CNetDlg::OnBtnUpdate() 
{
	// TODO: Add your control notification handler code here
	UpStatus=2;
}

void CNetDlg::OnKillfocusEditList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW *pNMListCtrl = (NM_LISTVIEW *)pNMHDR; 
	//int UpStatus = 2;	
	if(pNMListCtrl->iItem != -1 && pNMListCtrl->iSubItem>0&&pNMListCtrl->iSubItem < UpStatus)  
    {  

        CRect rect,dlgRect;  
        int width = m_list.GetColumnWidth(pNMListCtrl->iSubItem);  
        m_list.GetSubItemRect(pNMListCtrl->iItem,pNMListCtrl->iSubItem,LVIR_BOUNDS,rect);   
        m_nRow = pNMListCtrl->iItem;  
        m_nCol = pNMListCtrl->iSubItem;  
        m_list.GetWindowRect(&dlgRect);  
        //调整与父窗口对应   
        ScreenToClient(&dlgRect);  
        int height = rect.Height();  
        rect.top += (dlgRect.top+1);  
        rect.left += (dlgRect.left+1);  
        rect.bottom = rect.top+height+2;  
        rect.right = rect.left+width+2;  
        m_edit.MoveWindow(&rect);
		m_edit.SetWindowText(m_list.GetItemText(m_nRow,m_nCol));
        m_edit.ShowWindow(SW_SHOW);  
        m_edit.SetFocus(); 
		m_edit.SetSel(-1);
    }   
	*pResult = 0;
}

void CNetDlg::Init()
{
	m_edit.ShowWindow(SW_HIDE);
	LONG style = GetWindowLong(m_list.m_hWnd,GWL_STYLE);
	style &=~LVS_TYPEMASK;
	style |=LVS_REPORT;
	SetWindowLong(m_list.m_hWnd,GWL_STYLE,style);
	DWORD liststyle = m_list.GetExtendedStyle();
	liststyle |=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_list.SetExtendedStyle(liststyle);
	m_list.InsertColumn(0,"编号",LVCFMT_LEFT,50);
	m_list.InsertColumn(1,"IP",LVCFMT_LEFT,230);
	m_list.InsertColumn(2,"子网掩码",LVCFMT_LEFT,130);
	m_list.InsertColumn(3,"禁止/允许",LVCFMT_LEFT,230);
}

void CNetDlg::OnBtnUp() 
{
	// TODO: Add your control notification handler code here
	int j,m,i;
	CString tmp1,tmp2;
	for(j=0;j<m_list.GetItemCount();j++)
	{
		if(m_list.GetItemState(j, LVIS_SELECTED) == LVIS_SELECTED)
		{
			break;
		}
	}
	if(j!=m_list.GetItemCount()&&j!=0)
	{
		m = m_list.GetHeaderCtrl()->GetItemCount();
		for(i=1;i<m;i++)
		{
			tmp1 = m_list.GetItemText(j-1,i);
			tmp2 = m_list.GetItemText(j,i);
			m_list.SetItemText(j-1,i,tmp2);
			m_list.SetItemText(j,i,tmp1);
		}
		m_list.SetItemState(/*m_list.SetSelectionMark(j)*/j-1,LVIS_SELECTED,LVIS_SELECTED);
	}	
}

void CNetDlg::OnBtnDown() 
{
	// TODO: Add your control notification handler code here
	int j,m,i;
	CString tmp1,tmp2;
	for(j=0;j<m_list.GetItemCount();j++)
	{
		if(m_list.GetItemState(j, LVIS_SELECTED) == LVIS_SELECTED)
		{
			break;
		}
	}
	if(j!=m_list.GetItemCount()&&j!=m_list.GetItemCount()-1)
	{
		m = m_list.GetHeaderCtrl()->GetItemCount();
		for(i=1;i<m;i++)
		{
			tmp1 = m_list.GetItemText(j,i);
			tmp2 = m_list.GetItemText(j+1,i);
			m_list.SetItemText(j,i,tmp2);
			m_list.SetItemText(j+1,i,tmp1);
		}
		m_list.SetItemState(m_list.SetSelectionMark(j),0,LVIS_SELECTED);
	}
}

BOOL CNetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	Init();
	ReadFileToList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNetDlg::ReadFileToList()
{
	CStdioFile File;
	CString FilePath;
	CString ListenNameBuf,Tmp,tmp,GlobalFilePath,t;
	int i,num,n;
	FilePath.Format("%s",NET_LIST_FILE);
	char buf[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,buf);
 	t.Format("%s%s",buf,BASE_PATH);
	GlobalFilePath=t+FilePath;
	if(File.Open(GlobalFilePath,CStdioFile::modeRead))
	{
		while(File.ReadString(Tmp))
		{
			num = Global::GetSplitNum(Tmp);
			n  = m_list.GetItemCount();
			tmp.Format("%d",n+1);
			m_list.InsertItem(n,tmp);
			for(i=0;i<num;i++)
			{	
				tmp =Global::SplitName(Tmp,i);
				if(i==num-1)
				{
					if(_ttoi(tmp)==0)
					{
						tmp="禁止";
					}
					else if(_ttoi(tmp)==1)
					{
						tmp="允许";
					}
				}
				m_list.SetItemText(n,i+1,tmp);
			}	
		}
		File.Close();
	}else
	{
		AfxMessageBox(_T("文件打开错误!"));
	}
}

void CNetDlg::WriteListToFile()
{
	CStdioFile File;
	CString FilePath;
	CString ListenNameBuf,Tmp,tmp,GlobalFilePath,t;
	int i,num,n,m,j;
	CString deny;
	CString pass;
	deny.Format("%s","禁止");
	pass.Format("%s","允许");
	FilePath.Format("%s%s",BASE_PATH,NET_LIST_FILE);
	char buf[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,buf);
 	t.Format("%s",buf);
	GlobalFilePath=t+FilePath;
	if(File.Open(GlobalFilePath,CFile::modeCreate|CFile::modeWrite))
	{
		n = m_list.GetItemCount();
		for(i=0;i<n;i++)
		{
			m = m_list.GetHeaderCtrl()->GetItemCount();
			for(j=1;j<m-1;j++)
			{
				tmp = m_list.GetItemText(i,j);
				if(!tmp.IsEmpty())
					File.WriteString(tmp+"|");
			}
			tmp = m_list.GetItemText(i,j);
			if(tmp==deny)
				File.WriteString("0|");
			else if(tmp==pass)
				File.WriteString("1|");
			else
				AfxMessageBox("Input Error!");
			File.WriteString("\n");
		}
		File.Close();
	}else
	{
		AfxMessageBox(_T("文件打开错误!"));
	}
}
