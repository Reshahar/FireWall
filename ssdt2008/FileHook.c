#include "FileHook.h"
#include "Global.h"


extern KEVENT	 File_Create_Event;
extern KEVENT    File_Delete_Event;
extern MSG_STRUCT File_Create_Msg;
extern MSG_STRUCT File_Delete_Msg;
extern LIST_ENTRY NameList[3];
extern ULONG	g_ntcreatefile;
extern ULONG	g_ntdeletefile;
extern ULONG	g_ntsetfile;
extern BOOLEAN		IsStart;

/*******************定义函数指针*******************/
//NtCreateFile
typedef NTSTATUS (*NTCREATEFILE) (
								  PHANDLE FileHandle,
								  ACCESS_MASK DesiredAccess,
								  POBJECT_ATTRIBUTES ObjectAttributes,
								  PIO_STATUS_BLOCK IoStatusBlock,
								  PLARGE_INTEGER AllocationSize OPTIONAL,
								  ULONG FileAttributes,
								  ULONG ShareAccess,
								  ULONG CreateDisposition,
								  ULONG CreateOptions,
								  PVOID EaBuffer OPTIONAL,
								  ULONG EaLength
								  );

//NtDeleteFile
typedef NTSTATUS (*NTDELETEFILE)(
								 POBJECT_ATTRIBUTES ObjectAttributes
								 );

typedef NTSTATUS (*NTSETFILE)(
								HANDLE FileHandle,
								PIO_STATUS_BLOCK IoStatusBlock,
								PVOID FileInformation, 
								ULONG Length, 
								FILE_INFORMATION_CLASS FileInformationClass);



NTSTATUS MyNTSetInformationFile(
								HANDLE FileHandle,
								PIO_STATUS_BLOCK IoStatusBlock,
								PVOID FileInformation, 
								ULONG Length, 
								FILE_INFORMATION_CLASS FileInformationClass);
//新NtCreateFile函数
NTSTATUS NewNtCreateFile(
						 PHANDLE FileHandle,
						 ACCESS_MASK DesiredAccess,
						 POBJECT_ATTRIBUTES ObjectAttributes,
						 PIO_STATUS_BLOCK IoStatusBlock,
						 PLARGE_INTEGER AllocationSize OPTIONAL,
						 ULONG FileAttributes,
						 ULONG ShareAccess,
						 ULONG CreateDisposition,
						 ULONG CreateOptions,
						 PVOID EaBuffer OPTIONAL,
						 ULONG EaLength
						 )
{

	ANSI_STRING			str;
	ANSI_STRING			ansi_FilePath;
	ANSI_STRING			father_name;
	PLIST_ENTRY			plist;
	PMY_DATA			pData;
	if(IsStart)
	{
		if(ObjectAttributes->RootDirectory<0)
			KdPrint(("Create Root:%d\r\n",ObjectAttributes->RootDirectory));
		if(ObjectAttributes->ObjectName->Buffer!=NULL)
		{
			RtlUnicodeStringToAnsiString(&str, ObjectAttributes->ObjectName, TRUE);
			if (str.Buffer != NULL)
			{
				if (wcsstr(ObjectAttributes->ObjectName->Buffer, L"\\??\\")&&str.Buffer[5]==':')
				{
					RtlInitAnsiString(&ansi_FilePath,str.Buffer+4);
					plist = NameList[FILE_CHOSE].Flink;
					while (plist!=&NameList[FILE_CHOSE])
					{
						pData = CONTAINING_RECORD(plist,MY_DATA,list);
						if(RtlCompareString(&pData->CmpName,&ansi_FilePath,FALSE)==0)
						{
							KdPrint(("%Z",&pData->CmpName));
							//消息结构体的填充
							RtlZeroMemory(&File_Create_Msg,sizeof(MSG_STRUCT));
							RtlCopyMemory(File_Create_Msg.OtherMsg,ansi_FilePath.Buffer,ansi_FilePath.Length);
							File_Create_Msg.OtherMsglen = ansi_FilePath.Length;

							father_name.Buffer=ExAllocatePool(NonPagedPool,MAXPATH*4);
							father_name.MaximumLength=MAXPATH*4;
							GetProcessPathBySectionObject((ULONG)PsGetCurrentProcessId(),&father_name);
							RtlCopyMemory(File_Create_Msg.ProcessName,father_name.Buffer,father_name.Length);
							File_Create_Msg.ProcessNameLen = father_name.Length;
							RtlFreeAnsiString(&father_name);

							File_Create_Msg.Pid = (ULONG)PsGetCurrentProcessId();
							File_Create_Msg.flag = MYFILE_CREATE;

							KdPrint(("%d",CreateDisposition));
							if(CreateDisposition==FILE_OVERWRITE_IF)  
							{
								KeSetEvent(&File_Create_Event,IO_NO_INCREMENT,FALSE);
								RtlFreeAnsiString(&str);
								return STATUS_ACCESS_DENIED;
							}
							if(CreateDisposition==FILE_CREATE|CreateDisposition==FILE_OPEN_IF|CreateDisposition==FILE_OVERWRITE)  
							{  
								KeSetEvent(&File_Create_Event,IO_NO_INCREMENT,FALSE);
								RtlFreeAnsiString(&str);
								return STATUS_UNSUCCESSFUL;
							}	
						}
						plist = plist->Flink;
					}
				}
			}

			RtlFreeAnsiString(&str);
		}
	}
	//返回原函数
	return ((NTCREATEFILE)g_ntcreatefile)(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		AllocationSize OPTIONAL,
		FileAttributes,
		ShareAccess,
		CreateDisposition,
		CreateOptions,
		EaBuffer OPTIONAL,
		EaLength);
}

NTSTATUS NewNtDeleteFile(
						 POBJECT_ATTRIBUTES ObjectAttributes
						 )
{
	ANSI_STRING			str;
	ANSI_STRING			ansi_FilePath;
	ANSI_STRING			father_name;
	PLIST_ENTRY			plist;
	PMY_DATA			pData;
	if(IsStart)
	{
		if(ObjectAttributes->RootDirectory<0)
			KdPrint(("Root:%d\r\n",ObjectAttributes->RootDirectory));
		if(ObjectAttributes->ObjectName->Buffer!=NULL)
		{
			RtlUnicodeStringToAnsiString(&str, ObjectAttributes->ObjectName, TRUE);
			if (str.Buffer != NULL)
			{
				if (wcsstr(ObjectAttributes->ObjectName->Buffer, L"\\??\\")&&str.Buffer[5]==':')
				{
					RtlInitAnsiString(&ansi_FilePath,str.Buffer+4);
					plist = NameList[FILE_CHOSE].Flink;
					while (plist!=&NameList[FILE_CHOSE])
					{
						pData = CONTAINING_RECORD(plist,MY_DATA,list);
						if(RtlCompareString(&pData->CmpName,&ansi_FilePath,FALSE)==0)
						{
							//消息结构体的填充
							RtlZeroMemory(&File_Delete_Msg,sizeof(MSG_STRUCT));
							RtlCopyMemory(File_Delete_Msg.OtherMsg,ansi_FilePath.Buffer,ansi_FilePath.Length);
							File_Delete_Msg.OtherMsglen = ansi_FilePath.Length;

							father_name.Buffer=ExAllocatePool(NonPagedPool,MAXPATH*4);
							father_name.MaximumLength=MAXPATH*4;
							GetProcessPathBySectionObject((ULONG)PsGetCurrentProcessId(),&father_name);
							RtlCopyMemory(File_Delete_Msg.ProcessName,father_name.Buffer,father_name.Length);
							File_Delete_Msg.ProcessNameLen = father_name.Length;
							RtlFreeAnsiString(&father_name);

							File_Delete_Msg.Pid = (ULONG)PsGetCurrentProcessId();
							File_Delete_Msg.flag = MYFILE_DELETE;

							//KeSetEvent(&File_Delete_Event,IO_NO_INCREMENT,FALSE);
							
							RtlFreeAnsiString(&str);
							return STATUS_UNSUCCESSFUL;
								
						}
						plist = plist->Flink;
					}
				}
			}
			
			RtlFreeAnsiString(&str);
		}
	}
	//返回原函数
	return ((NTDELETEFILE)g_ntdeletefile)(
		ObjectAttributes
		);
}
NTSTATUS SetFileSSDTHook()
{
	PageOff();
	g_ntdeletefile = KeServiceDescriptorTable.ServiceTableBase[62];
	KeServiceDescriptorTable.ServiceTableBase[62] = (unsigned int)NewNtDeleteFile;
	g_ntcreatefile = KeServiceDescriptorTable.ServiceTableBase[37];
	KeServiceDescriptorTable.ServiceTableBase[37] = (unsigned int)NewNtCreateFile;
	g_ntsetfile = KeServiceDescriptorTable.ServiceTableBase[224];
	KeServiceDescriptorTable.ServiceTableBase[224] = (unsigned int)MyNTSetInformationFile;
	PageOn();
	return STATUS_SUCCESS;
}
NTSTATUS UnSetFileSSDTHook()
{
	PageOff();
	KeServiceDescriptorTable.ServiceTableBase[37] = (unsigned int)g_ntcreatefile;
	KeServiceDescriptorTable.ServiceTableBase[62] = (unsigned int)g_ntdeletefile;
	KeServiceDescriptorTable.ServiceTableBase[224] = (unsigned int)g_ntsetfile;
	PageOn();
	return STATUS_SUCCESS;
}

VOID File_Create_Thread_Func(PIRP  pIrp)
{
	CHAR * buf;
	KdPrint(("File Create Thread"));
	KeWaitForSingleObject(&File_Create_Event,Executive,KernelMode,FALSE,0);
	KeResetEvent(&File_Create_Event);                                                                                                                                          
	buf = (CHAR*)pIrp->AssociatedIrp.SystemBuffer;
	RtlCopyMemory(buf,&File_Create_Msg,sizeof(MSG_STRUCT));
	pIrp->IoStatus.Status = STATUS_SUCCESS;   
	pIrp->IoStatus.Information = sizeof(MSG_STRUCT);

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);  
	PsTerminateSystemThread(STATUS_SUCCESS);  
}
VOID File_Delete_Thread_Func(PIRP  pIrp)
{
	CHAR * buf;
	KdPrint(("File Delete Thread"));
	KeWaitForSingleObject(&File_Delete_Event,Executive,KernelMode,FALSE,0);
	KeResetEvent(&File_Delete_Event);                                                                                                                                          
	buf = (CHAR*)pIrp->AssociatedIrp.SystemBuffer;
	RtlCopyMemory(buf,&File_Delete_Msg,sizeof(MSG_STRUCT));
	pIrp->IoStatus.Status = STATUS_SUCCESS;   
	pIrp->IoStatus.Information = sizeof(MSG_STRUCT);

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);  
	PsTerminateSystemThread(STATUS_SUCCESS);  
}

BOOLEAN GetPathByHandle(HANDLE FileHandle,PANSI_STRING pansi_fullname)
{
	PFILE_OBJECT pFile=NULL;
	UNICODE_STRING   dosName,fullname;
	if(NT_SUCCESS(ObReferenceObjectByHandle(FileHandle,0,*IoFileObjectType,KernelMode,(PVOID)&pFile,0)))
	{
		fullname.Buffer = (PWCH)ExAllocatePool(PagedPool,MAXPATH);
		fullname.MaximumLength = MAXPATH;
		__try
		{

			ObReferenceObjectByPointer(pFile,0,NULL,KernelMode);//异常

			IoVolumeDeviceToDosName(((PFILE_OBJECT)pFile)->DeviceObject,&dosName);

			RtlCopyUnicodeString(&fullname,&dosName);


			RtlAppendUnicodeStringToString(&fullname,&((PFILE_OBJECT)pFile)->FileName);

			ObDereferenceObject(pFile);
			
			RtlUnicodeStringToAnsiString(pansi_fullname,&fullname,FALSE);

			ExFreePool(dosName.Buffer);
			ExFreePool(fullname.Buffer);
			return TRUE;
		}
		__except(1)
		{
			if(fullname.Buffer) ExFreePool(fullname.Buffer);
			return   FALSE;
		}
	}else
	{
		return FALSE;
	}

}


NTSTATUS MyNTSetInformationFile(
				HANDLE FileHandle,
				PIO_STATUS_BLOCK IoStatusBlock,
				PVOID FileInformation, 
				ULONG Length, 
				FILE_INFORMATION_CLASS FileInformationClass)
{

	ANSI_STRING			path_name;
	ANSI_STRING			father_name;
	PLIST_ENTRY			plist;
	PMY_DATA			pData;
	if(IsStart)
	{
		if(FileInformationClass==FileRenameInformation||FileInformationClass==FileDispositionInformation)
		{ 
			path_name.Buffer = ExAllocatePool(NonPagedPool,MAXPATH);
			path_name.MaximumLength = MAXPATH;
			GetPathByHandle(FileHandle,&path_name);
			KdPrint(("%Z",&path_name));
			plist = NameList[FILE_CHOSE].Flink;
			while (plist!=&NameList[FILE_CHOSE])
			{
				pData = CONTAINING_RECORD(plist,MY_DATA,list);
				if(RtlCompareString(&pData->CmpName,&path_name,FALSE)==0)
				{
					//消息结构体的填充
					RtlZeroMemory(&File_Delete_Msg,sizeof(MSG_STRUCT));
					RtlCopyMemory(File_Delete_Msg.OtherMsg,path_name.Buffer,path_name.Length);
					File_Delete_Msg.OtherMsglen = path_name.Length;

					father_name.Buffer=ExAllocatePool(NonPagedPool,MAXPATH*4);
					father_name.MaximumLength=MAXPATH*4;
					GetProcessPathBySectionObject((ULONG)PsGetCurrentProcessId(),&father_name);
					RtlCopyMemory(File_Delete_Msg.ProcessName,father_name.Buffer,father_name.Length);
					File_Delete_Msg.ProcessNameLen = father_name.Length;
					RtlFreeAnsiString(&father_name);

					File_Delete_Msg.Pid = (ULONG)PsGetCurrentProcessId();
					File_Delete_Msg.flag = MYFILE_DELETE;

					KeSetEvent(&File_Delete_Event,IO_NO_INCREMENT,FALSE);

					return STATUS_UNSUCCESSFUL;

				}
				plist = plist->Flink;
			}
			RtlFreeAnsiString(&path_name);
		}
		
	}
	return ((NTSETFILE)g_ntsetfile)
		(
		FileHandle,
		IoStatusBlock,
		FileInformation,
		Length,
		FileInformationClass
		);
}