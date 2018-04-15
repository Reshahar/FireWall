#if !defined(AFX_FILEDLG_H__21FBC723_53A0_4B60_91A1_D5EBF55AA914__INCLUDED_)
#define AFX_FILEDLG_H__21FBC723_53A0_4B60_91A1_D5EBF55AA914__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileDlg dialog

class CFileDlg : public CDialog
{
// Construction
public:
	CFileDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileDlg)
	enum { IDD = IDD_FILE_DIALOG };
	CListCtrl	m_list;
	CEdit	m_edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBtnRead();
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnDel();
	afx_msg void OnBtnUpdate();
	afx_msg void OnBtnSave();
	afx_msg void OnBtnLog();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListFile(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Init();
	int m_nRow;
	int m_nCol;
	int UpStatus;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEDLG_H__21FBC723_53A0_4B60_91A1_D5EBF55AA914__INCLUDED_)
