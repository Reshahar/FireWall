// Global.cpp: implementation of the Global class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FireWall.h"
#include "Global.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Global::Global()
{
}

Global::~Global()
{

}
bool Global::PreStatus[STATUS_NUM]={false};
bool Global::CurStatus[STATUS_NUM]={false};
bool Global::IsToDish = FALSE;
CString Global::SplitName(CString Name, int num)
{
		
	int f=0,i,c=0;
	CString buf;
	for (i=0;i<Name.GetLength();i++)
	{
		if(Name[i]=='|')
		{	
			if(c==num)
			{
				buf=Name.Mid(f,i-f);
				return buf;
			}
			c++;
			f = i+1;
		}
	}
}

int Global::GetSplitNum(CString Name)
{
	int i,j=0;
	for(i=0;i<Name.GetLength();i++)
	{
		if(Name[i]=='|')
		{
			j++;
		}
	}
	return j;
}

void Global::ReadListenName(CString FilePath, CListCtrl& m_list)
{
	CStdioFile File;
	CString ListenNameBuf,Tmp,tmp,GlobalFilePath;
	int i,num;
	char buf[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,buf);
 	GlobalFilePath.Format("%s%s%s",buf,BASE_PATH,FilePath);
	if(File.Open(GlobalFilePath,CStdioFile::modeRead))
	{
		while(File.ReadString(Tmp))
		{
			ListenNameBuf+=Tmp;
		}
		num = Global::GetSplitNum(ListenNameBuf);
		for(i=0;i<num;i++)
		{
			int n = m_list.GetItemCount();
			tmp.Format("%d",n+1);
			m_list.InsertItem(n,tmp);
			m_list.SetItemText(n,1,SplitName(ListenNameBuf,i));
		}
		File.Close();
	}else
	{
		AfxMessageBox(_T("文件打开错误!"));
	}
}

void Global::WriteNameListToFile(CString FilePath, CListCtrl &m_list)
{
	CString GlobalFilePath,t;
	CStdioFile File;
	int n,i;
	CString tmp;
	char buf[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,buf);
 	GlobalFilePath.Format("%s%s%s",buf,BASE_PATH,FilePath);
	DeleteFile(GlobalFilePath);
	if(File.Open(GlobalFilePath,CFile::modeCreate|CFile::modeWrite))
	{
		n = m_list.GetItemCount();
		for(i=0;i<n;i++)
		{
			tmp = m_list.GetItemText(i,1);
			if(!tmp.IsEmpty())
				File.WriteString(tmp+"|");
		}
		File.Close();
	}else
	{
		MessageBox(NULL,"Write Fialed!",NULL,NULL);
	}

}

void Global::ReadINIFile()
{
	CString path,data;
	CStdioFile File;
	char buf[MAX_PATH];
	int i;
	GetCurrentDirectory(MAX_PATH,buf);
	path.Format("%s%s%s",buf,BASE_PATH,INI_FILE_NAME);
	if(File.Open(path,CFile::modeRead))
	{
		if(File.ReadString(data))
		{
			if(data.GetLength()!=STATUS_NUM)
			{
				AfxMessageBox("File Error!");
			}else
			{
				for(i=0;i<STATUS_NUM;i++)
				{
					if(data[i]=='1')
					{
						PreStatus[i]=true;
						CurStatus[i]=true;
					}else
					{
						PreStatus[i]=false;
						CurStatus[i]=false;
					}
				}
			}
		}
		File.Close();
	}else
	{
		MessageBox(NULL,"Read Fialed!",NULL,NULL);
	}

	
}
void Global::WriteINIFile()
{
	CString path,data;
	CStdioFile File;
	char buf[MAX_PATH];
	int i;
	GetCurrentDirectory(MAX_PATH,buf);
	path.Format("%s%s%s",buf,BASE_PATH,INI_FILE_NAME);
	if(File.Open(path,CFile::modeCreate|CFile::modeWrite))
	{		
		for(i=0;i<STATUS_NUM;i++)
		{
			if(CurStatus[i]==true)
			{	
				data+="1";
			}else
			{
				data+="0";
			}
		}
		File.WriteString(data);
		File.Close();
	}else
	{
		MessageBox(NULL,"Read Fialed!",NULL,NULL);
	}

}
