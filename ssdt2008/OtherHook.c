#include "Global.h"
#include "OtherHook.h"


extern ULONG	   g_ntloaddriver;
extern ULONG       g_ntsetsystemtime;
extern ULONG		g_ntquerysysteminfo;

extern BOOLEAN     IsSetTime ;
extern BOOLEAN	   IsLoadDriver;
extern KTIMER		Timer;
extern KDPC			Dpc;
extern BOOLEAN		IsHide;
extern BOOLEAN		IsStart;

NTSTATUS MyNtLoadDriver(PUNICODE_STRING DriverServerName);
NTSTATUS MyNtSetSystemTime(PLARGE_INTEGER NewTime,PLARGE_INTEGER OldTime);


typedef NTSTATUS (*NTLOADDRIVER)(
								 PUNICODE_STRING DriverServerName
								 );

typedef NTSTATUS (*NTSETTIME)(
							 PLARGE_INTEGER NewTime,PLARGE_INTEGER OldTime
								 );


NTSTATUS GetQuertSystemAddress()
{
	PageOff();
	g_ntquerysysteminfo = KeServiceDescriptorTable.ServiceTableBase[173];
	PageOn();
	return STATUS_SUCCESS;
}

NTSTATUS SetQuertSystemAddress()
{
	PageOff();
	KeServiceDescriptorTable.ServiceTableBase[173] = (unsigned int)g_ntquerysysteminfo;
	PageOn();
	return STATUS_SUCCESS;
}



NTSTATUS SetOtherSSDTHook()
{
	PageOff();
	g_ntloaddriver = KeServiceDescriptorTable.ServiceTableBase[97];
	KeServiceDescriptorTable.ServiceTableBase[97] = (unsigned int)MyNtLoadDriver;
	g_ntsetsystemtime = KeServiceDescriptorTable.ServiceTableBase[242];
	KeServiceDescriptorTable.ServiceTableBase[242] = (unsigned int)MyNtSetSystemTime;
	PageOn();
	return STATUS_SUCCESS;
}
NTSTATUS UnSetOtherSSDTHook()
{
	PageOff();
	KeServiceDescriptorTable.ServiceTableBase[97] = (unsigned int)g_ntloaddriver;
	KeServiceDescriptorTable.ServiceTableBase[242] = (unsigned int)g_ntsetsystemtime;
	PageOn();
	return STATUS_SUCCESS;
}

NTSTATUS MyNtLoadDriver(PUNICODE_STRING DriverServerName)
{

	if(IsStart)
	{
		if(!IsLoadDriver)
		{
			return STATUS_ACCESS_DENIED;
		}
	}
	return ((NTLOADDRIVER)g_ntloaddriver)(
		DriverServerName
		);
}

NTSTATUS MyNtSetSystemTime(PLARGE_INTEGER NewTime,PLARGE_INTEGER OldTime)
{

	if(IsStart)
	{
		if(!IsSetTime)
		{
			return STATUS_ACCESS_DENIED;
		}
	}
	return ((NTSETTIME)g_ntsetsystemtime)(
		NewTime,
		OldTime
		);
}
VOID Thread_Func(PVOID Context)
{
	IsHide = TRUE;
    while(IsHide&&IsStart)
	{
		SetQuertSystemAddress();
		MySleep(60000);
	}
	PsTerminateSystemThread(STATUS_SUCCESS);  
}

VOID MyStartTimer()
{
	HANDLE hHIde;
	if(IsStart)
	{
		PsCreateSystemThread(&hHIde,THREAD_ALL_ACCESS , NULL,NULL,NULL,(PKSTART_ROUTINE)Thread_Func, NULL);
	}
	
}
VOID MyEndTimer()
{
	if(IsStart)
	{
		IsHide =FALSE;
	}
}