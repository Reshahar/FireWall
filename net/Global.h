#ifndef GLOBAL_H
#define GLOBAL_H

#include <ntstrsafe.h>

#define  MY_CURRENT_PATH_CTL_CODE	CTL_CODE(FILE_DEVICE_UNKNOWN,0x809,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define  MY_NET_CTL_CODE			CTL_CODE(FILE_DEVICE_UNKNOWN,0x810,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define  MY_NET_EXIT_CTL_CODE		CTL_CODE(FILE_DEVICE_UNKNOWN,0x811,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define  MY_NET_RESTART_CTL_CODE	CTL_CODE(FILE_DEVICE_UNKNOWN,0x812,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define  MY_NET_ISSTART_CTL_CODE	CTL_CODE(FILE_DEVICE_UNKNOWN,0x813,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define  MY_NET_DEF_PASS_CTL_CODE	CTL_CODE(FILE_DEVICE_UNKNOWN,0x814,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define  MY_NET_NO_PING_CTL_CODE	CTL_CODE(FILE_DEVICE_UNKNOWN,0x815,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define MAXPATH 1024

#define NET_LIST_PATH  L"\\net.txt"
typedef struct _mynetstruct
{
	LIST_ENTRY		list;
	ANSI_STRING		ip;
	ULONG			sub;
	ULONG			pass;

}MY_NET_DATA,*PMY_NET_DATA;

typedef struct _my_msg_struct
{
	char 	ProcessName[MAXPATH];
	int		ProcessNameLen;
	int		Pid;
	int		flag;		
	char 	OtherMsg[MAXPATH];
	int     OtherMsglen;
}MSG_STRUCT,*PMSG_STRUCT;

typedef struct _CONTROL_AREA
{
	PVOID Segment;
	LIST_ENTRY DereferenceList;
	ULONG NumberOfSectionReferences;
	ULONG NumberOfPfnReferences;
	ULONG NumberOfMappedViews;
	ULONG NumberOfSystemCacheViews;
	ULONG NumberOfUserReferences;
	union
	{
		ULONG LongFlags;
		ULONG Flags;
	} u;
	PFILE_OBJECT FilePointer;
} CONTROL_AREA, *PCONTROL_AREA;

typedef struct _SEGMENT
{
	struct _CONTROL_AREA *ControlArea;
	ULONG TotalNumberOfPtes;
	ULONG NonExtendedPtes;
	ULONG Spare0;
} SEGMENT, *PSEGMENT;

typedef struct _SECTION_OBJECT
{
	PVOID StartingVa;
	PVOID EndingVa;
	PVOID Parent;
	PVOID LeftChild;
	PVOID RightChild;
	PSEGMENT Segment;
} SECTION_OBJECT, *PSECTION_OBJECT;


#define MY_NET_PASS_THREAD		10
#define MY_NET_DENY_THREAD		11
#define MY_NET_ALL_THREAD		12

#define NET_PASS				6
#define NET_DENY				7


void StrToIP(char * ipstr,char *hexstr);
VOID Net_Pass_Thread_Func(PIRP  pIrp);
VOID Net_Deny_Thread_Func(PIRP  pIrp);
VOID MyStart();
VOID MyEnd();
VOID MyInitNameList();
VOID MyFreeList();
int CheckIPAndSub(char * ip,int len,ULONG sub,char * packetip);
BOOLEAN GetProcessPathBySectionObject(ULONG_PTR ProcessId,PANSI_STRING pAnsi_Path);
BOOLEAN GetPathByFileObject(PFILE_OBJECT FileObject, PANSI_STRING  pAnsi_Path);

#endif