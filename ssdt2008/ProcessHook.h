#ifndef	PROCESS_H 
#define PROCESS_H

#include <ntddk.h>

//新的NtCreateProcessEx
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
							 );
//新的NtTerminateProcess
NTSTATUS MyNtTerminateProcess(
							 HANDLE ProcessHandle,
							 NTSTATUS ExitStatus
							);
VOID		Process_Create_Thread_Func(PIRP  pIrp);
VOID		Process_Exit_Thread_Func(PIRP  pIrp);
NTSTATUS	GetFullName(HANDLE  SectionHandle,PANSI_STRING pansi_fullname);
NTSTATUS	SetProcessSSDTHook();
NTSTATUS	UnSetProcessSSDTHook();
#endif