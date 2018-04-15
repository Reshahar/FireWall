// Global.h: interface for the Global class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBAL_H__8A4F762E_6638_49DE_BB24_DC4B125BE83D__INCLUDED_)
#define AFX_GLOBAL_H__8A4F762E_6638_49DE_BB24_DC4B125BE83D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "define.h"

class Global  
{
public:
	static void WriteNameListToFile(CString FilePath,CListCtrl& m_list);
	static void ReadListenName(CString FilePath,CListCtrl& m_list);
	static int GetSplitNum(CString Name);
	static CString SplitName(CString ProcessName,int num);
	Global();
	virtual ~Global();
	static void ReadINIFile();
	static void WriteINIFile();
public:
	static bool PreStatus[STATUS_NUM];
	static bool CurStatus[STATUS_NUM];
	static bool IsToDish;
};

#endif // !defined(AFX_GLOBAL_H__8A4F762E_6638_49DE_BB24_DC4B125BE83D__INCLUDED_)
