#if !defined(AFX_NETDLG_H__B76C2F5F_97E8_4162_B786_9173D80FE0C7__INCLUDED_)
#define AFX_NETDLG_H__B76C2F5F_97E8_4162_B786_9173D80FE0C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNetDlg dialog

class CNetDlg : public CDialog
{
// Construction
public:
	CNetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNetDlg)
	enum { IDD = IDD_NET_DIALOG };
	CEdit	m_edit;
	CListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNetDlg)
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnDel();
	afx_msg void OnBtnLog();
	afx_msg void OnBtnRead();
	afx_msg void OnBtnSave();
	afx_msg void OnBtnUpdate();
	afx_msg void OnKillfocusEditList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnUp();
	afx_msg void OnBtnDown();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void WriteListToFile();
	void ReadFileToList();
	void Init();
	int m_nRow;
	int m_nCol;
	int UpStatus;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETDLG_H__B76C2F5F_97E8_4162_B786_9173D80FE0C7__INCLUDED_)
