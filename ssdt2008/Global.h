#ifndef GLOBAL_H
#define GLOBAL_H

#include "Define.h"


//全局函数
VOID PageOn();	//打开页保护
VOID PageOff();	//关闭页保护
VOID MySleep(LONG msec);
BOOLEAN GetProcessPathBySectionObject(ULONG_PTR ProcessId,PANSI_STRING pAnsi_Path);
BOOLEAN GetPathByFileObject(PFILE_OBJECT FileObject, PANSI_STRING  pAnsi_Path);


UCHAR *PsGetProcessImageFileName(__in PEPROCESS Process); 

#endif