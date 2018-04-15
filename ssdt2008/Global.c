#include "Global.h"

//全局变量

//Address
ULONG		g_ntcreateprocessex;
ULONG		g_ntterminateprocess;
ULONG		g_ntsetvaluekey;
ULONG		g_ntdeletevaluekey;
ULONG		g_ntcreatefile;
ULONG		g_ntdeletefile;
ULONG		g_ntsetfile;
ULONG		g_ntloaddriver;
ULONG       g_ntsetsystemtime;
ULONG		g_ntquerysysteminfo;
//other

LIST_ENTRY	NameList[3];
MSG_STRUCT	Process_Create_Msg;
MSG_STRUCT	Process_Exit_Msg;
MSG_STRUCT	Reg_Set_Msg;
MSG_STRUCT	Reg_Del_Msg;
MSG_STRUCT	File_Create_Msg;
MSG_STRUCT	File_Delete_Msg;
KEVENT		Process_Create_Event;
KEVENT		Reg_Set_Event;
KEVENT      File_Create_Event;
KEVENT		Process_Exit_Event;
KEVENT		Reg_Del_Event;
KEVENT      File_Delete_Event;
BOOLEAN		IsStart = FALSE;
BOOLEAN     IsSetTime = TRUE;
BOOLEAN		IsLoadDriver = TRUE;
BOOLEAN		IsHide=FALSE;
KTIMER		Timer;
KDPC		 Dpc;


LIST_ENTRY  ProcessNameList,RegNameList,FileNameList;


NTSTATUS PsLookupProcessByProcessId(
									HANDLE    ProcessId,
									PEPROCESS *Process
									);

NTSTATUS IoQueryFileDosDeviceName(
								   PFILE_OBJECT             FileObject,
								   POBJECT_NAME_INFORMATION *ObjectNameInformation
								  );


VOID PageOn()
{
	__asm
	{
		mov eax,cr0
			or eax,10000h
			mov cr0,eax
			sti
	}
}
VOID PageOff()
{
	__asm
	{
		cli
			mov eax,cr0
			or eax,not 10000h
			mov cr0,eax
	}
}

VOID MySleep(LONG msec)
{
	LARGE_INTEGER my_interval;
	my_interval.QuadPart = DELAY_ONE_MILLISECOND;
	my_interval.QuadPart *= msec;
	KeDelayExecutionThread(KernelMode,0,&my_interval);
}
BOOLEAN GetProcessPathBySectionObject(ULONG_PTR ProcessId, PANSI_STRING pAnsi_strPath)
{
	PEPROCESS         eprocess = NULL;
	PSECTION_OBJECT   sectionObject = NULL;
	PSEGMENT        segment = NULL;
	PCONTROL_AREA   ControlArea = NULL;
	PFILE_OBJECT    fileObject = NULL;
	BOOLEAN         bGetPath = FALSE;
	NTSTATUS status;

	if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)ProcessId, &eprocess)))
	{
		if (MmIsAddressValid((PVOID)((ULONG_PTR)eprocess + 0x138)))
		{
			sectionObject = *(PSECTION_OBJECT*)((ULONG_PTR)eprocess + 0x138);
			if (sectionObject && MmIsAddressValid(sectionObject))
			{
				segment = (PSEGMENT)sectionObject->Segment;
				if (segment && MmIsAddressValid(segment))
				{
					ControlArea = segment->ControlArea;
					if (ControlArea && MmIsAddressValid(ControlArea))
					{
						fileObject = ControlArea->FilePointer;
						if (fileObject&&MmIsAddressValid(fileObject))
						{
							bGetPath = GetPathByFileObject(fileObject, pAnsi_strPath);
						}
					}
				}
			}
		}
	}
	return bGetPath;
}

BOOLEAN GetPathByFileObject(PFILE_OBJECT FileObject, PANSI_STRING pAnsi_strPath)
{
	BOOLEAN bGetPath = FALSE;
	UNICODE_STRING u_strPath;
	NTSTATUS status;
	POBJECT_NAME_INFORMATION ObjectNameInformation = NULL;

	__try
	{
		if (FileObject && MmIsAddressValid(FileObject) && pAnsi_strPath)
		{
			if (NT_SUCCESS(IoQueryFileDosDeviceName(FileObject, &ObjectNameInformation))) 
			{
				status = IoQueryFileDosDeviceName(FileObject, &ObjectNameInformation);
				u_strPath.Buffer = ExAllocatePool(NonPagedPool, 2048);
				u_strPath.MaximumLength = 2048;
				RtlCopyUnicodeString(&u_strPath, &(ObjectNameInformation->Name));
				RtlUnicodeStringToAnsiString(pAnsi_strPath, &u_strPath, FALSE);

				bGetPath = TRUE;
				ExFreePool(u_strPath.Buffer);
				ExFreePool(ObjectNameInformation);
			}
		}
	}
	__except(1)
	{
		KdPrint(("[GetPathByFileObject]NULL"));
		bGetPath = FALSE;
	}

	return bGetPath;
}
