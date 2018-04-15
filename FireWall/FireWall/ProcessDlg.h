#if !defined(AFX_PROCESSDLG_H__937C9AAC_585B_4579_8682_835CC28DE5F1__INCLUDED_)
#define AFX_PROCESSDLG_H__937C9AAC_585B_4579_8682_835CC28DE5F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcessDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProcessDlg dialog

class CProcessDlg : public CDialog
{
// Construction
public:
	CProcessDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProcessDlg)
	enum { IDD = IDD_PROCESS_DIALOG };
	CEdit	m_edit;
	CListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProcessDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnDel();
	afx_msg void OnBtnLog();
	afx_msg void OnBtnSave();
	afx_msg void OnBtnUpdate();
	afx_msg void OnClickListProcess(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditList();
	afx_msg void OnBtnRead();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Init();
	int m_nRow;
	int m_nCol;
	int UpStatus;
public:
	void UpDateProcMsg();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSDLG_H__937C9AAC_585B_4579_8682_835CC28DE5F1__INCLUDED_)
