// FileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FireWall.h"
#include "FileDlg.h"
#include "define.h"
#include "Global.h"
#include "CTL_CODE.h"
#include "LogListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileDlg dialog


CFileDlg::CFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nRow=-1;
	m_nCol=-1;
	UpStatus = -1;
}


void CFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileDlg)
	DDX_Control(pDX, IDC_LIST_FILE, m_list);
	DDX_Control(pDX, IDC_EDIT_LIST, m_edit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileDlg, CDialog)
	//{{AFX_MSG_MAP(CFileDlg)
	ON_BN_CLICKED(IDC_BTN_READ, OnBtnRead)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, OnBtnDel)
	ON_BN_CLICKED(IDC_BTN_UPDATE, OnBtnUpdate)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_BN_CLICKED(IDC_BTN_LOG, OnBtnLog)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FILE, OnClickListFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileDlg message handlers

void CFileDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CFileDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CFileDlg::OnBtnRead() 
{
	// TODO: Add your control notification handler code here
	
}

void CFileDlg::OnBtnAdd() 
{
	// TODO: Add your control notification handler code here
	
}

void CFileDlg::OnBtnDel() 
{
	// TODO: Add your control notification handler code here
	
}

void CFileDlg::OnBtnUpdate() 
{
	// TODO: Add your control notification handler code here
	
}

void CFileDlg::OnBtnSave() 
{
	// TODO: Add your control notification handler code here
	HANDLE h;
	DWORD ret;
	Global::WriteNameListToFile(FILE_LIST_FILE,m_list);
	h =  CreateFile("\\\\.\\FIREWALLSSDT",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
	if(h==INVALID_HANDLE_VALUE)
	{
		AfxMessageBox("Open");
		return;
	}
	AfxMessageBox("正在保存.....");
	DeviceIoControl(h,MY_FILE_RESTART_CTL_CODE,NULL,0,NULL,0,&ret,NULL);
	
}

void CFileDlg::OnBtnLog() 
{
	// TODO: Add your control notification handler code here
	CLogListDlg processlist(NULL,FILE_CHOSE);
	processlist.DoModal();
}

void CFileDlg::Init()
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
	m_list.InsertColumn(1,"文件路径",LVCFMT_LEFT,630);
}

BOOL CFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	Init();
	Global::ReadListenName(FILE_LIST_FILE,m_list);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileDlg::OnClickListFile(NMHDR* pNMHDR, LRESULT* pResult) 
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
