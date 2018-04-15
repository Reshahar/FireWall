#if !defined(AFX_BOXDLG_H__8CF9AD5D_21BB_458E_9156_3E7569709FD5__INCLUDED_)
#define AFX_BOXDLG_H__8CF9AD5D_21BB_458E_9156_3E7569709FD5__INCLUDED_

#include "define.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BoxDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBoxDlg dialog

class CBoxDlg : public CDialog
{
// Construction
public:
	CBoxDlg(PMSG_STRUCT pMsg,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBoxDlg)
	enum { IDD = IDD_BOX_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBoxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBoxDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnMyClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSee();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString t;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOXDLG_H__8CF9AD5D_21BB_458E_9156_3E7569709FD5__INCLUDED_)
