#if !defined(AFX_LOGLISTDLG_H__AC159E97_7161_4222_BA19_98151A6E382D__INCLUDED_)
#define AFX_LOGLISTDLG_H__AC159E97_7161_4222_BA19_98151A6E382D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLogListDlg dialog

class CLogListDlg : public CDialog
{
// Construction
public:
	CLogListDlg(CWnd* pParent = NULL,int chose=-1);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLogListDlg)
	enum { IDD = IDD_LOG_LIST };
	CListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLogListDlg)
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnDelLog();
	afx_msg void OnReRead();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void InitNet();
	void InitFile();
	int m_chose;
	void InitReg();
	void FileToList(int CHOSE);
	void InitProcess();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGLISTDLG_H__AC159E97_7161_4222_BA19_98151A6E382D__INCLUDED_)
