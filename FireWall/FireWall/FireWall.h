// FireWall.h : main header file for the FIREWALL application
//

#if !defined(AFX_FIREWALL_H__C991A046_F742_4CE6_A1F6_F49E45749515__INCLUDED_)
#define AFX_FIREWALL_H__C991A046_F742_4CE6_A1F6_F49E45749515__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFireWallApp:
// See FireWall.cpp for the implementation of this class
//

class CFireWallApp : public CWinApp
{
public:
	CFireWallApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireWallApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFireWallApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIREWALL_H__C991A046_F742_4CE6_A1F6_F49E45749515__INCLUDED_)
