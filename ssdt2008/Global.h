#ifndef GLOBAL_H
#define GLOBAL_H

#include "Define.h"


//ȫ�ֺ���
VOID PageOn();	//��ҳ����
VOID PageOff();	//�ر�ҳ����
VOID MySleep(LONG msec);
BOOLEAN GetProcessPathBySectionObject(ULONG_PTR ProcessId,PANSI_STRING pAnsi_Path);
BOOLEAN GetPathByFileObject(PFILE_OBJECT FileObject, PANSI_STRING  pAnsi_Path);


UCHAR *PsGetProcessImageFileName(__in PEPROCESS Process); 

#endif