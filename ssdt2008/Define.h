#ifndef		DEFINE_H
#define		DEFINE_H

#include <ntddk.h>

//定义常量
#define MAXPATH 1024
#define LISTCOUNT	3
//选择
#define PROCESS_CHOSE	0
#define REG_CHOSE		1
#define FILE_CHOSE		2

//Define some times  
#define DELAY_ONE_MICROSECOND (-10)
#define DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND*1000)


#pragma pack(1)
typedef struct ServiceDescriptorEntry
{
	unsigned int *ServiceTableBase;
	unsigned int *ServiceCounterTableBase;
	unsigned int NumberOfServices;
	unsigned char *ParamTableBase;  
}SSDT_Entry;
#pragma pack()

__declspec(dllimport) SSDT_Entry KeServiceDescriptorTable;

//声明结构体
typedef struct _mystruct
{
	LIST_ENTRY		list;
	ANSI_STRING		CmpName;	

}MY_DATA,*PMY_DATA;
//通信消息结构体
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



#define PROCESS_CREATE	0
#define PROCESS_EXIT	1
#define REG_SET_VALUE	2
#define REG_DEL_VALUE	3
#define MYFILE_CREATE   4
#define MYFILE_DELETE   5


#define MY_PROCESS_CREATE_THREAD	1
#define MY_PROCESS_EXIT_THREAD		2
#define MY_PROCESS_ALL_THREAD		3

#define MY_REG_SET_THREAD			4
#define MY_REG_DEL_THREAD			5
#define MY_REG_ALL_THREAD			6

#define MY_FILE_CREATE_THREAD		7
#define MY_FILE_DELETE_THREAD		8
#define MY_FILE_ALL_THREAD			9


#define FILE_LIST_PATH	L"\\file.txt"
#define PROCESS_LIST_PATH L"\\process.txt"
#define REG_LIST_PATH	L"\\reg.txt"

#endif