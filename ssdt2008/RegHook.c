#include "RegHook.h"
#include "Global.h"

extern LIST_ENTRY NameList[3];
extern ULONG  g_ntsetvaluekey;
extern ULONG  g_ntdeletevaluekey;
extern KEVENT Reg_Del_Event;
extern KEVENT Reg_Set_Event;
extern MSG_STRUCT Reg_Set_Msg,Reg_Del_Msg;
extern BOOLEAN		IsStart;



NTSTATUS ObQueryNameString(
						   PVOID                    Object,
						   POBJECT_NAME_INFORMATION ObjectNameInfo,
						   ULONG                    Length,
						   PULONG                   ReturnLength
						   );
/*******************定义函数指针*******************/
//NtSetValueKey
typedef NTSTATUS (*NTSETVALUEKEY) (
								   IN HANDLE KeyHandle,
								   IN PUNICODE_STRING ValueName,
								   IN ULONG TitleIndex,
								   IN ULONG Type,
								   IN PVOID Data,
								   IN ULONG DataSize);

//NtDeleteValueKey
typedef NTSTATUS(*NTDELETEVALUEKEY) (
									 IN HANDLE KeyHandle,
									 IN PUNICODE_STRING ValueName);
NTSTATUS NewNtSetValueKey(
						  IN HANDLE KeyHandle,
						  IN PUNICODE_STRING ValueName,
						  IN ULONG TitleIndex,
						  IN ULONG Type,
						  IN PVOID Data,
						  IN ULONG DataSize)
{
	NTSTATUS status;
	UNICODE_STRING RegPath;
	ANSI_STRING Ansi_strName;
	ANSI_STRING Ansi_ProcName,Ansi_RegPath;
	PLIST_ENTRY			plist;
	PMY_DATA			pData;
	
	if(IsStart)
	{
		//获取注册表路径
		RegPath.Buffer = ExAllocatePool(NonPagedPool,MAXPATH*2);
		RegPath.MaximumLength = MAXPATH*2;
		GetRegistryPath(KeyHandle, &RegPath);
		RtlUnicodeStringToAnsiString(&Ansi_RegPath,&RegPath,TRUE);
		Ansi_RegPath.Length = RegPath.Length/2;
		ExFreePool(RegPath.Buffer);

		plist = NameList[REG_CHOSE].Flink;
		while (plist!=&NameList[REG_CHOSE])
		{
			pData = CONTAINING_RECORD(plist,MY_DATA,list);
			//KdPrint(("%Z %d %d",&pData->CmpName,pData->CmpName.Length,Ansi_RegPath.Length));
			if(strcmp((pData->CmpName.Buffer),Ansi_RegPath.Buffer)==0)
			{		
				//获取进程名
				KdPrint(("Set"));
				Ansi_strName.Buffer = ExAllocatePool(NonPagedPool,MAXPATH*2);
				Ansi_strName.MaximumLength = MAXPATH*2;

				if (!GetProcessPathBySectionObject((ULONG)PsGetCurrentProcessId(), &Ansi_strName))
				{
					KdPrint(("GetProcessFailed!"));
				}
				Ansi_ProcName.Buffer=ExAllocatePool(NonPagedPool,MAXPATH*2);
				Ansi_ProcName.MaximumLength=MAXPATH*2;
				RtlCopyString(&Ansi_ProcName,&Ansi_strName);
				ExFreePool(Ansi_strName.Buffer);
				
				if(strstr(Ansi_ProcName.Buffer,"FireWall.exe")!=NULL)
					break;

				RtlZeroMemory(&Reg_Set_Msg,sizeof(MSG_STRUCT));
				Reg_Set_Msg.flag = REG_SET_VALUE;
				RtlCopyMemory(Reg_Set_Msg.OtherMsg,Ansi_RegPath.Buffer,Ansi_RegPath.Length);
				Reg_Set_Msg.OtherMsglen = Ansi_RegPath.Length;

				RtlCopyMemory(Reg_Set_Msg.ProcessName,Ansi_ProcName.Buffer,Ansi_ProcName.Length);
				Reg_Set_Msg.ProcessNameLen = Ansi_ProcName.Length;
				ExFreePool(Ansi_ProcName.Buffer);

				Reg_Set_Msg.Pid = (ULONG)PsGetCurrentProcessId();
				
				KeSetEvent(&Reg_Set_Event,IO_NO_INCREMENT,FALSE);
				RtlFreeAnsiString(&Ansi_RegPath);
				return STATUS_UNSUCCESSFUL;
			}	
			plist = plist->Flink;
		}
		RtlFreeAnsiString(&Ansi_RegPath);
	}
	//返回原函数
	return ((NTSETVALUEKEY)g_ntsetvaluekey)(
		KeyHandle,
		ValueName,
		TitleIndex,
		Type,
		Data,
		DataSize);
}
//新NtDeleteValueKey函数
NTSTATUS NewNtDeleteValueKey(
							 IN HANDLE KeyHandle,
							 IN PUNICODE_STRING ValueName)
{
	NTSTATUS status;
	UNICODE_STRING RegPath;
	ANSI_STRING Ansi_strName;
	ANSI_STRING Ansi_ProcName,Ansi_RegPath;
	PLIST_ENTRY			plist;
	PMY_DATA			pData;

	if(IsStart)
	{
		//获取注册表路径
		RegPath.Buffer = ExAllocatePool(NonPagedPool,MAXPATH*2);
		RegPath.MaximumLength = 2048;
		GetRegistryPath(KeyHandle, &RegPath);
		RtlUnicodeStringToAnsiString(&Ansi_RegPath,&RegPath,TRUE);
		Ansi_RegPath.Length = RegPath.Length/2;
		ExFreePool(RegPath.Buffer);
		
		plist = NameList[REG_CHOSE].Flink;
		while (plist!=&NameList[REG_CHOSE])
		{
			pData = CONTAINING_RECORD(plist,MY_DATA,list);
			if(strcmp((pData->CmpName.Buffer),Ansi_RegPath.Buffer)==0)
			{	

				KdPrint(("%Z",&Ansi_RegPath));
				//获取进程名
				KdPrint(("Del"));

				Ansi_strName.Buffer = ExAllocatePool(NonPagedPool,MAXPATH*2);
				Ansi_strName.MaximumLength = MAXPATH*2;

				if (!GetProcessPathBySectionObject((ULONG)PsGetCurrentProcessId(), &Ansi_strName))
				{
					KdPrint(("GetProcessFailed!"));
				}
				Ansi_ProcName.Buffer=ExAllocatePool(NonPagedPool,MAXPATH*2);
				Ansi_ProcName.Length=0;
				Ansi_ProcName.MaximumLength=MAXPATH*2;
				RtlCopyString(&Ansi_ProcName,&Ansi_strName);
				ExFreePool(Ansi_strName.Buffer);
				
				if(strstr(Ansi_ProcName.Buffer,"FireWall.exe")!=NULL)
					break;

				RtlZeroMemory(&Reg_Del_Msg,sizeof(MSG_STRUCT));
				Reg_Del_Msg.flag = REG_DEL_VALUE;
				RtlCopyMemory(Reg_Del_Msg.OtherMsg,Ansi_RegPath.Buffer,Ansi_RegPath.Length);
				Reg_Del_Msg.OtherMsglen = Ansi_RegPath.Length;

				RtlCopyMemory(Reg_Del_Msg.ProcessName,Ansi_ProcName.Buffer,Ansi_ProcName.Length);
				Reg_Del_Msg.ProcessNameLen = Ansi_ProcName.Length;
				ExFreePool(Ansi_ProcName.Buffer);

				Reg_Del_Msg.Pid = (ULONG)PsGetCurrentProcessId();

				KeSetEvent(&Reg_Del_Event,IO_NO_INCREMENT,FALSE);
				RtlFreeAnsiString(&Ansi_RegPath);
				return STATUS_UNSUCCESSFUL;
			}	
			plist = plist->Flink;
		}
		RtlFreeAnsiString(&Ansi_RegPath);
	}
	//返回原函数
	return ((NTDELETEVALUEKEY)g_ntdeletevaluekey)(
		KeyHandle,
		ValueName);
}
//获取注册表完整路径
void GetRegistryPath(HANDLE KeyHandle, PUNICODE_STRING pRegPath)
{
	WCHAR str1[] = L"\\REGISTRY\\ROOT";
	WCHAR str2[] = L"\\REGISTRY\\USER";
	WCHAR str3[] = L"\\REGISTRY\\MACHINE";
	WCHAR str4[] = L"\\REGISTRY\\USERS";
	WCHAR str5[] = L"\\REGISTRY\\CONFIG";

	UNICODE_STRING reg1;
	UNICODE_STRING reg2;
	UNICODE_STRING reg3;
	UNICODE_STRING reg4;
	UNICODE_STRING reg5;

	NTSTATUS status;
	PVOID pKey;
	ULONG BufLen;
	POBJECT_NAME_INFORMATION pStr;

	UNICODE_STRING u_reg;

	RtlInitUnicodeString(&reg1, L"HKEY_CLASSES_ROOT");
	RtlInitUnicodeString(&reg2, L"HKEY_CURRENT_USER");
	RtlInitUnicodeString(&reg3, L"HKEY_LOCAL_MACHINE");
	RtlInitUnicodeString(&reg4, L"HKEY_USERS");
	RtlInitUnicodeString(&reg5, L"HKEY_CURRENT_CONFIG");

	ObReferenceObjectByHandle(KeyHandle, 0, NULL, KernelMode, &pKey, NULL);

	BufLen = pRegPath->MaximumLength + sizeof(UNICODE_STRING);

	pStr = ExAllocatePoolWithTag(NonPagedPool, BufLen, 'Reg1');
	if (pStr == NULL) 
	{
		ObDereferenceObject(pKey);
		ExFreePoolWithTag(pStr, 'Reg1');
	}
	else
	{
		memset(pStr, 0, BufLen);
		status = ObQueryNameString(pKey, pStr, BufLen, &BufLen);

		if ( NT_SUCCESS(status) && (pStr->Name.Buffer != NULL) )
		{
			u_reg.Buffer = ExAllocatePoolWithTag(NonPagedPool, 2048, 'con1');
			u_reg.MaximumLength = 2048;

			RtlCopyUnicodeString(&u_reg, &pStr->Name);

			if (!_wcsnicmp(u_reg.Buffer, str1, wcslen(str1)))
			{
				RtlCopyUnicodeString(pRegPath, &reg1);
				wcsncat(pRegPath->Buffer, pStr->Name.Buffer + wcslen(str1), pStr->Name.Length - (wcslen(str1) + 1));
				pRegPath->Length=pStr->Name.Length - (wcslen(str1) + 1)+reg1.Length;
			}
			else if (!_wcsnicmp(u_reg.Buffer, str2, wcslen(str2)))
			{
				RtlCopyUnicodeString(pRegPath, &reg2);
				wcsncat(pRegPath->Buffer, pStr->Name.Buffer + wcslen(str2), pStr->Name.Length - (wcslen(str2) + 1));
				pRegPath->Length=pStr->Name.Length - (wcslen(str2) + 1)+reg2.Length;
			}
			else if (!_wcsnicmp(u_reg.Buffer, str3, wcslen(str3)))
			{
				RtlCopyUnicodeString(pRegPath, &reg3);
				wcsncat(pRegPath->Buffer, pStr->Name.Buffer + wcslen(str3), pStr->Name.Length - (wcslen(str3) + 1));
				pRegPath->Length=pStr->Name.Length - (wcslen(str3) + 1)+reg3.Length;
			}
			else if (!_wcsnicmp(u_reg.Buffer, str4, wcslen(str4)))
			{
				RtlCopyUnicodeString(pRegPath, &reg4);
				wcsncat(pRegPath->Buffer, pStr->Name.Buffer + wcslen(str4), pStr->Name.Length - (wcslen(str4) + 1));
				pRegPath->Length=pStr->Name.Length - (wcslen(str4) + 1)+reg4.Length;
			}
			else if (!_wcsnicmp(u_reg.Buffer, str5, wcslen(str5)))
			{
				RtlCopyUnicodeString(pRegPath, &reg5);
				wcsncat(pRegPath->Buffer, pStr->Name.Buffer + wcslen(str5), pStr->Name.Length - (wcslen(str5) + 1));
				pRegPath->Length=pStr->Name.Length - (wcslen(str5) + 1)+reg5.Length;
			}

			ExFreePoolWithTag(u_reg.Buffer, 'con1');
		}
		ObDereferenceObject(pKey);
		ExFreePoolWithTag(pStr, 'Reg1');
	}
}	


NTSTATUS SetRegSSDTHook()
{
	PageOff();
	g_ntsetvaluekey = KeServiceDescriptorTable.ServiceTableBase[247];
	KeServiceDescriptorTable.ServiceTableBase[247] = (unsigned int)NewNtSetValueKey;
	g_ntdeletevaluekey = KeServiceDescriptorTable.ServiceTableBase[65];
	KeServiceDescriptorTable.ServiceTableBase[65] = (unsigned int)NewNtDeleteValueKey;
	PageOn();
	return STATUS_SUCCESS;
}
NTSTATUS UnSetRegSSDTHook()
{
	PageOff();
	KeServiceDescriptorTable.ServiceTableBase[247] = (unsigned int)g_ntsetvaluekey;
	KeServiceDescriptorTable.ServiceTableBase[65] = (unsigned int)g_ntdeletevaluekey;
	PageOn();
	return STATUS_SUCCESS;
}

VOID Reg_Set_Thread_Func(PIRP  pIrp)
{
	CHAR * buf;
	KdPrint(("Reg Set Thread"));
	KeWaitForSingleObject(&Reg_Set_Event,Executive,KernelMode,FALSE,0);
	KeResetEvent(&Reg_Set_Event);                       

	buf = (CHAR*)pIrp->AssociatedIrp.SystemBuffer;
	RtlCopyMemory(buf,&Reg_Set_Msg,sizeof(MSG_STRUCT));
	pIrp->IoStatus.Status = STATUS_SUCCESS;   
	pIrp->IoStatus.Information = sizeof(MSG_STRUCT);
	IoCompleteRequest(pIrp, IO_NO_INCREMENT); 

	PsTerminateSystemThread(STATUS_SUCCESS);  
}
VOID Reg_Del_Thread_Func(PIRP  pIrp)
{
	CHAR * buf;
	KdPrint(("Reg Delete Thread"));
	KeWaitForSingleObject(&Reg_Del_Event,Executive,KernelMode,FALSE,0);
	KeResetEvent(&Reg_Del_Event);                       

	buf = (CHAR*)pIrp->AssociatedIrp.SystemBuffer;
	RtlCopyMemory(buf,&Reg_Del_Msg,sizeof(MSG_STRUCT));
	pIrp->IoStatus.Status = STATUS_SUCCESS;   
	pIrp->IoStatus.Information = sizeof(MSG_STRUCT);
	IoCompleteRequest(pIrp, IO_NO_INCREMENT); 

	PsTerminateSystemThread(STATUS_SUCCESS);  
}

