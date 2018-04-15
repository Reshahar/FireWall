#if !defined(AFX_REGDLG_H__C90C7F18_7D83_4D28_A7A2_BE87D4AF2083__INCLUDED_)
#define AFX_REGDLG_H__C90C7F18_7D83_4D28_A7A2_BE87D4AF2083__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegDlg dialog

class CRegDlg : public CDialog
{
// Construction
public:
	CRegDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegDlg)
	enum { IDD = IDD_REG_DIALOG };
	CEdit	m_edit;
	CListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickListReg(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnRead();
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnDel();
	afx_msg void OnBtnUpdate();
	afx_msg void OnBtnSave();
	afx_msg void OnBtnLog();
	afx_msg void OnKillfocusEditList();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_nCol;
	int m_nRow;
	int UpdateStatus;
	void Init();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGDLG_H__C90C7F18_7D83_4D28_A7A2_BE87D4AF2083__INCLUDED_)
