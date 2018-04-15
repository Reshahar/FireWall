// ProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FireWall.h"
#include "ProcessDlg.h"
#include "LogListDlg.h"
#include "Global.h"
#include "CTL_CODE.h"
#include "define.h"
#include <winsvc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcessDlg dialog


CProcessDlg::CProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProcessDlg)
	//}}AFX_DATA_INIT
	m_nRow=-1;
	m_nCol=-1;
	UpStatus = -1;
}


void CProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcessDlg)
	DDX_Control(pDX, IDC_EDIT_LIST, m_edit);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProcessDlg, CDialog)
	//{{AFX_MSG_MAP(CProcessDlg)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, OnBtnDel)
	ON_BN_CLICKED(IDC_BTN_LOG, OnBtnLog)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_BN_CLICKED(IDC_BTN_UPDATE, OnBtnUpdate)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PROCESS, OnClickListProcess)
	ON_EN_KILLFOCUS(IDC_EDIT_LIST, OnKillfocusEditList)
	ON_BN_CLICKED(IDC_BTN_READ, OnBtnRead)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessDlg message handlers

void CProcessDlg::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CProcessDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
//	CDialog::OnCancel();
}

void CProcessDlg::Init()
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
	m_list.InsertColumn(1,"名单",LVCFMT_LEFT,630);
	
}

BOOL CProcessDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	// TODO: Add extra initialization here
	Init();
	Global::ReadListenName(PROCESS_LIST_FILE,m_list);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CProcessDlg::OnBtnAdd() 
{
	// TODO: Add your control notification handler code here
	CString tmp;
	int n = m_list.GetItemCount();
	tmp.Format("%d",n+1);
	m_list.InsertItem(n,tmp);
	UpStatus=2;
}

void CProcessDlg::OnBtnDel() 
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

void CProcessDlg::OnBtnLog() 
{
	// TODO: Add your control notification handler code here
	CLogListDlg processlist(NULL,PROCESS_CHOSE);
	processlist.DoModal();
}

void CProcessDlg::OnBtnSave() 
{
	// TODO: Add your control notification handler code here
	HANDLE h;
	DWORD ret;
	Global::WriteNameListToFile(PROCESS_LIST_FILE,m_list);
	h =  CreateFile("\\\\.\\FIREWALLSSDT",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(h==INVALID_HANDLE_VALUE)
	{
		AfxMessageBox("Open");
		return;
 	}
	AfxMessageBox("正在保存.....");
	DeviceIoControl(h,MY_PROCESS_RESTART_CTL_CODE,NULL,0,NULL,0,&ret,NULL);
	UpStatus = -1;
}

void CProcessDlg::OnBtnUpdate() 
{
	// TODO: Add your control notification handler code here
	UpStatus=2;
}


void CProcessDlg::OnClickListProcess(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CProcessDlg::OnKillfocusEditList() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString tmp;
	m_edit.GetWindowText(tmp);
	m_list.SetItemText(m_nRow,m_nCol,tmp);
	m_edit.ShowWindow(SW_HIDE);
}

void CProcessDlg::OnBtnRead() 
{
	// TODO: Add your control notification handler code here
	while(m_list.DeleteItem(0));
	Global::ReadListenName(PROCESS_LIST_FILE,m_list);
}

BOOL CProcessDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::DestroyWindow();
}

void CProcessDlg::UpDateProcMsg()
{
	OnBtnRead();
	//AfxMessageBox("asdasdasda");
}
