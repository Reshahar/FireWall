#if !defined(AFX_INDEXDLG_H__C0BA22AE_34CF_4154_9EDD_7721D0266D2A__INCLUDED_)
#define AFX_INDEXDLG_H__C0BA22AE_34CF_4154_9EDD_7721D0266D2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IndexDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIndexDlg dialog

class CIndexDlg : public CDialog
{
// Construction
public:
	CIndexDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIndexDlg)
	enum { IDD = IDD_INDEX_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIndexDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIndexDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INDEXDLG_H__C0BA22AE_34CF_4154_9EDD_7721D0266D2A__INCLUDED_)
