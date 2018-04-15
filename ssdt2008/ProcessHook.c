#include "ProcessHook.h"
#include "Global.h"

extern ULONG  g_ntcreateprocessex;
extern ULONG  g_ntterminateprocess;
extern KEVENT Process_Create_Event,Process_Exit_Event;
extern LIST_ENTRY	NameList[3];
extern MSG_STRUCT	Process_Create_Msg,Process_Exit_Msg;
extern BOOLEAN		IsStart;

NTSTATUS ObOpenObjectByPointer(
							   __in     PVOID           Object,
							   __in     ULONG           HandleAttributes,
							   __in_opt PACCESS_STATE   PassedAccessState,
							   __in     ACCESS_MASK     DesiredAccess,
							   __in_opt POBJECT_TYPE    ObjectType,
							   __in     KPROCESSOR_MODE AccessMode,
							   __out    PHANDLE         Handle
							   );
typedef NTSTATUS (*NTCREATEPEOCESSEX)(
									  __out PHANDLE ProcessHandle,
									  __in ACCESS_MASK DesiredAccess,
									  __in POBJECT_ATTRIBUTES ObjectAttributes,
									  __in HANDLE InheritFromProcessHandle,
									  __in ULONG CreateFlags,
									  __in HANDLE SectionHandle ,
									  __in HANDLE DebugObject ,
									  __in HANDLE ExceptionPort ,
									  __in ULONG JobMemberLevel
									  );
typedef NTSTATUS (*NTTERMINATEPROCESS)(
									   HANDLE ProcessHandle,
									   NTSTATUS ExitStatus
									   );		

VOID Process_Create_Thread_Func(PIRP  pIrp)
{
	CHAR * buf;
	KdPrint(("Process Create Thread"));
	KeWaitForSingleObject(&Process_Create_Event,Executive,KernelMode,FALSE,0);
	KeResetEvent(&Process_Create_Event);                                                                                                                                          
	buf = (CHAR*)pIrp->AssociatedIrp.SystemBuffer;
	RtlCopyMemory(buf,&Process_Create_Msg,sizeof(MSG_STRUCT));
	pIrp->IoStatus.Status = STATUS_SUCCESS;   
	pIrp->IoStatus.Information = sizeof(MSG_STRUCT);

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);  
	PsTerminateSystemThread(STATUS_SUCCESS);  
}
VOID Process_Exit_Thread_Func(PIRP  pIrp)
{
	CHAR * buf;
	KdPrint(("Process Exit Thread"));
	KeWaitForSingleObject(&Process_Exit_Event,Executive,KernelMode,FALSE,0);
	KeResetEvent(&Process_Exit_Event);                                                                                                                                          
	buf = (CHAR*)pIrp->AssociatedIrp.SystemBuffer;
	RtlCopyMemory(buf,&Process_Exit_Msg,sizeof(MSG_STRUCT));
	pIrp->IoStatus.Status = STATUS_SUCCESS;   
	pIrp->IoStatus.Information = sizeof(MSG_STRUCT);

	IoCompleteRequest(pIrp, IO_NO_INCREMENT); 
	PsTerminateSystemThread(STATUS_SUCCESS);  

}



NTSTATUS MyNtCreateProcessEx(
							 __out PHANDLE ProcessHandle,
							 __in ACCESS_MASK DesiredAccess,
							 __in POBJECT_ATTRIBUTES ObjectAttributes,
							 __in HANDLE InheritFromProcessHandle,
							 __in ULONG CreateFlags,
							 __in HANDLE SectionHandle ,
							 __in HANDLE DebugObject ,
							 __in HANDLE ExceptionPort ,
							 __in ULONG JobMemberLevel
							 )
{
	NTSTATUS			status;
	ANSI_STRING			ansi_name,father_name;
	PLIST_ENTRY			plist;
	PMY_DATA			pData;
	if(IsStart)
	{
		GetFullName(SectionHandle,&ansi_name);
		plist = NameList[PROCESS_CHOSE].Flink;
		while (plist!=&NameList[PROCESS_CHOSE])
		{
			pData = CONTAINING_RECORD(plist,MY_DATA,list);
			if(RtlCompareString(&pData->CmpName,&ansi_name,TRUE)==0)
			{	
				/*消息结构体的填充*/
				RtlZeroMemory(&Process_Create_Msg,sizeof(MSG_STRUCT));
				Process_Create_Msg.flag = PROCESS_CREATE;
				RtlCopyMemory(Process_Create_Msg.OtherMsg,ansi_name.Buffer,ansi_name.Length);
				Process_Create_Msg.OtherMsglen = ansi_name.Length;
				
				RtlFreeAnsiString(&ansi_name);

				father_name.Buffer=ExAllocatePool(NonPagedPool,MAXPATH*4);
				father_name.MaximumLength=MAXPATH*4;
				GetProcessPathBySectionObject((ULONG)PsGetCurrentProcessId(),&father_name);
				RtlCopyMemory(Process_Create_Msg.ProcessName,father_name.Buffer,father_name.Length);
				Process_Create_Msg.ProcessNameLen = father_name.Length;
				RtlFreeAnsiString(&father_name);

				Process_Create_Msg.Pid = (ULONG)PsGetCurrentProcessId();
				
				KeSetEvent(&Process_Create_Event,IO_NO_INCREMENT,FALSE);
			
				return STATUS_UNSUCCESSFUL;
			}	
			plist = plist->Flink;
		}
		RtlFreeAnsiString(&ansi_name);
	}
	return ((NTCREATEPEOCESSEX)g_ntcreateprocessex)(
		ProcessHandle,DesiredAccess,ObjectAttributes,InheritFromProcessHandle,CreateFlags,SectionHandle,DebugObject,ExceptionPort,JobMemberLevel);
}

NTSTATUS  GetFullName(HANDLE  SectionHandle,PANSI_STRING pansi_fullname) 
{
	PVOID pKey=NULL,pFile=NULL;
	NTSTATUS   ns;
	UNICODE_STRING   dosName,fullname;
	if (SectionHandle==0) return STATUS_ACCESS_DENIED;
	ns=ObReferenceObjectByHandle(SectionHandle,0,NULL,KernelMode,&pKey,NULL);
	if (!NT_SUCCESS(ns)) return ns;
	fullname.Buffer = (PWCH)ExAllocatePool(PagedPool,MAXPATH);
	fullname.MaximumLength = MAXPATH;
	__try
	{
		pFile=(PVOID)*(ULONG   *)((char   *)pKey+20);
		pFile=(PVOID)*(ULONG   *)((char   *)pFile);
		pFile=(PVOID)*(ULONG   *)((char   *)pFile+36);

		ObReferenceObjectByPointer(pFile,0,NULL,KernelMode);//异常

		IoVolumeDeviceToDosName(((PFILE_OBJECT)pFile)->DeviceObject,&dosName);

		RtlCopyUnicodeString(&fullname,&dosName);


		RtlAppendUnicodeStringToString(&fullname,&((PFILE_OBJECT)pFile)->FileName);

		ObDereferenceObject(pFile);
		ObDereferenceObject(pKey);

		RtlUnicodeStringToAnsiString(pansi_fullname,&fullname,TRUE);

		ExFreePool(dosName.Buffer);
		ExFreePool(fullname.Buffer);
		return STATUS_SUCCESS;
	}
	__except(1)
	{
		if(fullname.Buffer) ExFreePool(fullname.Buffer);
		if(pKey) ObDereferenceObject(pKey);
		return   STATUS_SUCCESS;
	}
}

NTSTATUS MyNtTerminateProcess(
							  HANDLE ProcessHandle,
							  NTSTATUS ExitStatus
							  )
{
	PEPROCESS        pEProcessTarget = NULL;
	int                       i;
	HANDLE                hID;
	ANSI_STRING			 ansi_process_name,ansi_name;
	ANSI_STRING			ansi_str;
	ANSI_STRING			ansi_my;
	ANSI_STRING			ansi_child_name,father_name;
	RtlInitAnsiString(&ansi_str,"taskmgr.exe");
	RtlInitAnsiString(&ansi_my,"FireWall.exe");

	if(IsStart)
	{
		if (NT_SUCCESS(ObReferenceObjectByHandle(ProcessHandle, STANDARD_RIGHTS_READ, *PsProcessType, KernelMode, &pEProcessTarget, NULL)))
		{
			hID = PsGetProcessId(pEProcessTarget);
			RtlInitAnsiString(&ansi_child_name,PsGetProcessImageFileName(pEProcessTarget));
			
			ObDereferenceObject(pEProcessTarget);

			RtlInitAnsiString(&ansi_process_name,PsGetProcessImageFileName(PsGetCurrentProcess()));
			
			if(RtlCompareString(&ansi_process_name,&ansi_str,TRUE)!=0)
			{
				if(hID!=PsGetCurrentProcessId())
				{
					KdPrint(("Kill Other"));
					RtlZeroMemory(&Process_Exit_Msg,sizeof(MSG_STRUCT));
					Process_Exit_Msg.flag = PROCESS_EXIT;
					
					ansi_name.Buffer = ExAllocatePool(NonPagedPool,MAXPATH*2);
					ansi_name.MaximumLength = MAXPATH*2;

					GetProcessPathBySectionObject((ULONG)hID,&ansi_name);
					RtlCopyMemory(Process_Exit_Msg.OtherMsg,ansi_name.Buffer,ansi_name.Length);
					Process_Exit_Msg.OtherMsglen = ansi_name.Length;
					
					RtlFreeAnsiString(&ansi_name);

					father_name.Buffer=ExAllocatePool(NonPagedPool,MAXPATH*4);
					father_name.MaximumLength=MAXPATH*4;
					GetProcessPathBySectionObject((ULONG)PsGetCurrentProcessId(),&father_name);
					RtlCopyMemory(Process_Exit_Msg.ProcessName,father_name.Buffer,father_name.Length);
					Process_Create_Msg.ProcessNameLen = father_name.Length;
					RtlFreeAnsiString(&father_name);

					Process_Exit_Msg.Pid = (ULONG)PsGetCurrentProcessId();

					KeSetEvent(&Process_Exit_Event,IO_NO_INCREMENT,FALSE);

					return(STATUS_ACCESS_DENIED);
				}
			}else if((RtlCompareString(&ansi_child_name,&ansi_my,TRUE)==0)&&hID!=PsGetCurrentProcessId())
			{
				KdPrint(("Kill Me"));
				RtlZeroMemory(&Process_Exit_Msg,sizeof(MSG_STRUCT));
				Process_Exit_Msg.flag = PROCESS_EXIT;

				ansi_name.Buffer = ExAllocatePool(NonPagedPool,MAXPATH*2);
				ansi_name.MaximumLength = MAXPATH*2;

				GetProcessPathBySectionObject((ULONG)hID,&ansi_name);
				RtlCopyMemory(Process_Exit_Msg.OtherMsg,ansi_name.Buffer,ansi_name.Length);
				Process_Exit_Msg.OtherMsglen = ansi_name.Length;

				RtlFreeAnsiString(&ansi_name);

				father_name.Buffer=ExAllocatePool(NonPagedPool,MAXPATH*4);
				father_name.MaximumLength=MAXPATH*4;
				GetProcessPathBySectionObject((ULONG)PsGetCurrentProcessId(),&father_name);
				RtlCopyMemory(Process_Exit_Msg.ProcessName,father_name.Buffer,father_name.Length);
				Process_Create_Msg.ProcessNameLen = father_name.Length;
				RtlFreeAnsiString(&father_name);

				Process_Exit_Msg.Pid = (ULONG)PsGetCurrentProcessId();

				KeSetEvent(&Process_Exit_Event,IO_NO_INCREMENT,FALSE);
				return(STATUS_ACCESS_DENIED);
			}else
			{
				KdPrint(("Taskmgr"));
			}
				
		}
	}
	return ((NTTERMINATEPROCESS)g_ntterminateprocess)(ProcessHandle,ExitStatus);
}


NTSTATUS SetProcessSSDTHook()
{
	KdPrint(("1"));
	PageOff();
	g_ntcreateprocessex = KeServiceDescriptorTable.ServiceTableBase[48];
	KeServiceDescriptorTable.ServiceTableBase[48] = (unsigned int)MyNtCreateProcessEx;
	g_ntterminateprocess = KeServiceDescriptorTable.ServiceTableBase[257];
	KeServiceDescriptorTable.ServiceTableBase[257] = (unsigned int)MyNtTerminateProcess;
	PageOn();
	return STATUS_SUCCESS;
}
NTSTATUS UnSetProcessSSDTHook()
{
	KdPrint(("2"));
	PageOff();
	KeServiceDescriptorTable.ServiceTableBase[48] = (unsigned int)g_ntcreateprocessex;
	KeServiceDescriptorTable.ServiceTableBase[257] = (unsigned int)g_ntterminateprocess;
	PageOn();
	return STATUS_SUCCESS;
}