#if !defined(AFX_TOOLDLG_H__D480EEDA_4262_4EBD_B17B_867C0E156F29__INCLUDED_)
#define AFX_TOOLDLG_H__D480EEDA_4262_4EBD_B17B_867C0E156F29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolDlg dialog

class CToolDlg : public CDialog
{
// Construction
public:
	void MyExitToDish(int chose);
	void MyNoHide(int nohide);
	void MyNoLoadDriver(int chose);
	void MyNoSetTime(int chose);
	void DelToAutoStart();
	void AddToAutoStart();
	void MySomeProcess(int chose);
	void MyNoSomeProcess(int chose);
	void MyNoPing(int chose);
	void MyNoNet(int chose);
	CToolDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CToolDlg)
	enum { IDD = IDD_TOOL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CToolDlg)
	afx_msg void OnSave();
	afx_msg void OnReset();
	afx_msg void OnSaveSystem();
	afx_msg void OnResetSystem();
	afx_msg void OnNoCmd();
	afx_msg void OnExitToDish();
	afx_msg void OnNoAddUser();
	afx_msg void OnNoChangeTime();
	afx_msg void OnNoDelUser();
	afx_msg void OnNoHideThread();
	afx_msg void OnNoLoadDrvier();
	afx_msg void OnNoNet();
	afx_msg void OnNoPing();
	afx_msg void OnNoReg();
	afx_msg void OnRunStart();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void Init();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLDLG_H__D480EEDA_4262_4EBD_B17B_867C0E156F29__INCLUDED_)
