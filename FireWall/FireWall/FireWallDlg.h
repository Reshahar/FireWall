// FireWallDlg.h : header file
//

#if !defined(AFX_FIREWALLDLG_H__A2436F9B_EC44_416F_805A_1FEF48D68E7B__INCLUDED_)
#define AFX_FIREWALLDLG_H__A2436F9B_EC44_416F_805A_1FEF48D68E7B__INCLUDED_

#include "ProcessDlg.h"
#include "RegDlg.h"
#include "FileDlg.h"
#include "indexDlg.h"
#include "NetDlg.h"
#include "ToolDlg.h"
#include "define.h"
#include "BoxDlg.h"
#include <winsvc.h>


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_MYMINMENU WM_USER+100 

/////////////////////////////////////////////////////////////////////////////
// CFireWallDlg dialog

class CFireWallDlg : public CDialog
{
// Construction
public:
	void Init();
	CFireWallDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFireWallDlg)
	enum { IDD = IDD_FIREWALL_DIALOG };
	CTabCtrl	m_tab;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireWallDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFireWallDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnMyMin(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	bool GetSSDTNAME();
	bool ReInstall();
	bool UnInstallNet();
	bool InstallNet();
	bool OpenDriver();
	void SSDT_START();
	bool DeleteSC(SC_HANDLE sc_hManager, SC_HANDLE sc_hResult);
	bool StopDriver();
	bool StartDriver(SC_HANDLE sc_hManager,SC_HANDLE sc_hResult);
	bool CreateSC();
	CProcessDlg m_process;
	CRegDlg		m_reg;
	CFileDlg	m_file;
	CIndexDlg	m_index;
	CNetDlg		m_net;
	CToolDlg    m_tool;
	NOTIFYICONDATA m_nid;
private:
	int SendNetCurrentPath();
	int SendCurrentPath();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIREWALLDLG_H__A2436F9B_EC44_416F_805A_1FEF48D68E7B__INCLUDED_)
