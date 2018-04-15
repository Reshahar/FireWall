#include <ntddk.h>
#include "CTL_CODE.h"
#include "Global.h"
#include "ProcessHook.h"
#include "RegHook.h"
#include "FileHook.h"
#include "OtherHook.h"

//全局变量
KSPIN_LOCK	Spin_lock;
UNICODE_STRING us_Current_Path;

extern LIST_ENTRY	ProcessNameList,RegNameList,FileNameList;
extern LIST_ENTRY   NameList[3];
extern MSG_STRUCT	Process_Create_Msg;
extern MSG_STRUCT	Process_Exit_Msg;
extern MSG_STRUCT	Reg_Set_Msg;
extern MSG_STRUCT	Reg_Del_Msg;
extern MSG_STRUCT	File_Create_Msg;
extern MSG_STRUCT	File_Delete_Msg;
extern KEVENT		Process_Create_Event;
extern KEVENT		Reg_Set_Event;
extern KEVENT       File_Create_Event;
extern KEVENT		Process_Exit_Event;
extern KEVENT		Reg_Del_Event;
extern KEVENT       File_Delete_Event;
extern BOOLEAN		IsStart;
extern BOOLEAN     IsSetTime ;
extern BOOLEAN		IsLoadDriver;
extern ULONG		g_ntquerysysteminfo;

extern KEVENT Reg_Set_Event;
extern MSG_STRUCT Reg_Set_Msg;

//函数声明
NTSTATUS MyCreateDevice(PDRIVER_OBJECT pDriver_Object);
VOID MyDelDevice(PDRIVER_OBJECT pDriver_Obj);
NTSTATUS MyIRPFunc(PDEVICE_OBJECT pDriver_Object,PIRP pIrp);
NTSTATUS MyDeviceControlFunc(PDEVICE_OBJECT pDevice_Object,PIRP pIrp);			//异步IO
VOID MyFreeList(int List_Chose);
VOID MyGetCmpName(int File_Chose);
NTSTATUS MyCreateList(PCHAR CmpNamebuf,ULONG len,int List_Chose);
VOID StartListen();			//初始化
VOID EndListen();


VOID DriverUnload(PDRIVER_OBJECT driver)
{
	EndListen();
	MyDelDevice(driver);
}
NTSTATUS DriverEntry(PDRIVER_OBJECT driver,PUNICODE_STRING reg_path)  
{
	
	NTSTATUS Status;

	NameList[0] = ProcessNameList;
	NameList[1] = RegNameList;
	NameList[2] = FileNameList;
	Status = MyCreateDevice(driver);
	if(NT_SUCCESS(Status))
	{
		KdPrint(("OK"));
	}else
	{
		KdPrint(("NO"));
	}
	driver->MajorFunction[IRP_MJ_CREATE] = MyIRPFunc;
	driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyDeviceControlFunc;
	StartListen();
	driver->DriverUnload=DriverUnload;    //调用卸载函数
	return STATUS_SUCCESS;
}
VOID EndListen()
{
	UnSetOtherSSDTHook();
	//MyFreeList(PROCESS_CHOSE);
	UnSetProcessSSDTHook();
	//MyFreeList(REG_CHOSE);
	UnSetRegSSDTHook();
	//MyFreeList(FILE_CHOSE);
	UnSetFileSSDTHook();
}
VOID ListenInit()
{
	KeInitializeEvent(&Process_Create_Event,NotificationEvent,FALSE);
	KeInitializeEvent(&Process_Exit_Event,NotificationEvent,FALSE);
	KeInitializeEvent(&Reg_Set_Event,NotificationEvent,FALSE);
	KeInitializeEvent(&Reg_Del_Event,NotificationEvent,FALSE);
	KeInitializeEvent(&File_Create_Event,NotificationEvent,FALSE);
	KeInitializeEvent(&File_Delete_Event,NotificationEvent,FALSE);
	MyGetCmpName(PROCESS_CHOSE);
	MyGetCmpName(REG_CHOSE);
	MyGetCmpName(FILE_CHOSE);
}
VOID StartListen()
{
	
	GetQuertSystemAddress();
	//MyGetCmpName(PROCESS_CHOSE);
	SetProcessSSDTHook();
	//MyGetCmpName(REG_CHOSE);
	SetRegSSDTHook();
	//MyGetCmpName(FILE_CHOSE);
	SetFileSSDTHook();
	SetOtherSSDTHook();
}

NTSTATUS MyCreateDevice(PDRIVER_OBJECT pDriver_Object)
{
	NTSTATUS		Status;
	UNICODE_STRING	usDevName;
	UNICODE_STRING  usSymName;
	PDEVICE_OBJECT  pDevObj;

	RtlInitUnicodeString(&usDevName,L"\\Device\\FIREWALLSSDT");

	Status = IoCreateDevice(
		pDriver_Object,
		0,
		&usDevName,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&pDevObj
		);
	if(!NT_SUCCESS(Status))
	{
		return Status;
	}
	pDevObj->Flags|=DO_BUFFERED_IO;
	RtlInitUnicodeString(&usSymName,L"\\??\\FIREWALLSSDT");

	Status = IoCreateSymbolicLink(&usSymName,&usDevName);
	
	if(!NT_SUCCESS(Status))
	{
		IoDeleteDevice(pDevObj);
		return Status;
	}
	return STATUS_SUCCESS;
}
VOID MyDelDevice(PDRIVER_OBJECT pDriver_Obj)
{
	UNICODE_STRING usSymName;
	RtlInitUnicodeString(&usSymName,L"\\??\\FIREWALLSSDT");

	if(pDriver_Obj->DeviceObject)
	{
		IoDeleteSymbolicLink(&usSymName);
		IoDeleteDevice(pDriver_Obj->DeviceObject);
	}
}

NTSTATUS MyIRPFunc(PDEVICE_OBJECT pDevice_Object,PIRP pIrp)
{
	NTSTATUS Status;
	Status = STATUS_SUCCESS;
	
	KdPrint(("IRP Create"));
	pIrp->IoStatus.Status = Status;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp,IO_NO_INCREMENT);
	return Status;
}

NTSTATUS MyDeviceControlFunc(PDEVICE_OBJECT pDevice_Object,PIRP pIrp)
{
	HANDLE hEvent;
	HANDLE hProcessCreateThread,hProcessExitThread;
	HANDLE hRegSetThread,hRegDelThread;
	HANDLE hFileCreateThread,hFileDeleteThread;
	NTSTATUS Status;
	ULONG code; 
	PIO_STACK_LOCATION pStack;
	ANSI_STRING current_path;
	CHAR * buf;
	int nohide,chsettime,chsetload;
	int process_thread_chose,reg_thread_chose,file_thread_chose;

	pStack = IoGetCurrentIrpStackLocation(pIrp);	

	code = pStack->Parameters.DeviceIoControl.IoControlCode;
	
	switch(code)
	{
	case MY_PROCESS_CTL_CODE:
		{
			hProcessCreateThread=NULL;
			hProcessExitThread =NULL;
			process_thread_chose = *(int*)pIrp->AssociatedIrp.SystemBuffer;
			switch(process_thread_chose)
			{
			case MY_PROCESS_CREATE_THREAD:
				Status = PsCreateSystemThread(&hProcessCreateThread,THREAD_ALL_ACCESS , NULL,NULL,NULL,(PKSTART_ROUTINE)Process_Create_Thread_Func, pIrp);
				break;
			case MY_PROCESS_EXIT_THREAD:
				Status = PsCreateSystemThread(&hProcessExitThread,THREAD_ALL_ACCESS , NULL,NULL,NULL,(PKSTART_ROUTINE)Process_Exit_Thread_Func, pIrp);
				break;
			}
			if (!NT_SUCCESS(Status))
			{
				KdPrint(("Process Thread Failed!"));
				return Status;
			}
			Status = STATUS_PENDING;	
			IoMarkIrpPending(pIrp);
			pIrp->IoStatus.Status = Status;
			pIrp->IoStatus.Information = 0;
			return Status;
		}
	case MY_PROCESS_EXIT_CTL_CODE:
		{
			IsStart = FALSE;
			KeSetEvent(&Process_Create_Event,IO_NO_INCREMENT,FALSE);
			KeSetEvent(&Process_Exit_Event,IO_NO_INCREMENT,FALSE);	
			MyFreeList(PROCESS_CHOSE);
			break;
		}
	case MY_PROCESS_RESTART_CTL_CODE:
		{
			//KeSetEvent(&Process_Create_Event,IO_NO_INCREMENT,FALSE);
			//KeSetEvent(&Process_Exit_Event,IO_NO_INCREMENT,FALSE);
			MyFreeList(PROCESS_CHOSE);
			MyGetCmpName(PROCESS_CHOSE);
			//KeResetEvent(&Process_Create_Event);
			//KeResetEvent(&Process_Exit_Event);
			break;
		}
	case MY_REG_CTL_CODE:
		{
			hRegSetThread = NULL;
			hRegDelThread = NULL;
			reg_thread_chose = *(int*)pIrp->AssociatedIrp.SystemBuffer;
			switch(reg_thread_chose)
			{
			case MY_REG_SET_THREAD:
				Status = PsCreateSystemThread(&hRegSetThread,THREAD_ALL_ACCESS , NULL,NULL,NULL,(PKSTART_ROUTINE)Reg_Set_Thread_Func, pIrp);
				break;
			case MY_REG_DEL_THREAD:
				Status = PsCreateSystemThread(&hRegDelThread,THREAD_ALL_ACCESS , NULL,NULL,NULL,(PKSTART_ROUTINE)Reg_Del_Thread_Func, pIrp);
				break;
			}
			if (!NT_SUCCESS(Status))
			{
				KdPrint(("Reg Thread Failed!"));
				return Status;
			}
			Status = STATUS_PENDING;	
			IoMarkIrpPending(pIrp);
			pIrp->IoStatus.Status = Status;
			pIrp->IoStatus.Information = 0;
			return Status;
			break;
		}
	case MY_REG_RESTART_CTL_CODE:
		{
			//KeSetEvent(&Reg_Set_Event,IO_NO_INCREMENT,FALSE);
			//KeSetEvent(&Reg_Del_Event,IO_NO_INCREMENT,FALSE);
			MyFreeList(REG_CHOSE);
			MyGetCmpName(REG_CHOSE);
			//KeResetEvent(&Reg_Set_Event);
			//KeResetEvent(&Reg_Del_Event);
			break;
		}
	case MY_REG_EXIT_CTL_CODE:
		{
			IsStart = FALSE;
			KeSetEvent(&Reg_Set_Event,IO_NO_INCREMENT,FALSE);
			KeSetEvent(&Reg_Del_Event,IO_NO_INCREMENT,FALSE);
			MyFreeList(REG_CHOSE);
			break;
		}
	case MY_FILE_CTL_CODE:
		{
			file_thread_chose = *(int*)pIrp->AssociatedIrp.SystemBuffer;
			switch(file_thread_chose)
			{
			case MY_FILE_CREATE_THREAD:
				hFileCreateThread = NULL;
				Status = PsCreateSystemThread(&hFileCreateThread,THREAD_ALL_ACCESS , NULL,NULL,NULL,(PKSTART_ROUTINE)File_Create_Thread_Func, pIrp);
				break;
				hFileDeleteThread = NULL;
			case MY_FILE_DELETE_THREAD:
				Status = PsCreateSystemThread(&hFileDeleteThread,THREAD_ALL_ACCESS , NULL,NULL,NULL,(PKSTART_ROUTINE)File_Delete_Thread_Func, pIrp);
				break;
			}
			if (!NT_SUCCESS(Status))
			{
				KdPrint(("File Thread Failed!"));
				return Status;
			}
			Status = STATUS_PENDING;	
			IoMarkIrpPending(pIrp);
			pIrp->IoStatus.Status = Status;
			pIrp->IoStatus.Information = 0;
			return Status;
			break;
		}
	case MY_FILE_RESTART_CTL_CODE:
		{
			//KeSetEvent(&File_Create_Event,IO_NO_INCREMENT,FALSE);
			//KeSetEvent(&File_Delete_Event,IO_NO_INCREMENT,FALSE);
			MyFreeList(FILE_CHOSE);
			MyGetCmpName(FILE_CHOSE);
			//KeResetEvent(&File_Create_Event);
			//KeResetEvent(&File_Delete_Event);
			break;
		}
	case MY_FILE_EXIT_CTL_CODE:
		{
			IsStart = FALSE;
			KeSetEvent(&File_Create_Event,IO_NO_INCREMENT,FALSE);
			KeSetEvent(&File_Delete_Event,IO_NO_INCREMENT,FALSE);
			MyFreeList(FILE_CHOSE);
			RtlFreeUnicodeString(&us_Current_Path);
			break;
		}
	case MY_CURRENT_PATH_CTL_CODE:
		{
			buf = (CHAR *)pIrp->AssociatedIrp.SystemBuffer;
			us_Current_Path.Buffer = ExAllocatePool(NonPagedPool,MAXPATH*4);
			us_Current_Path.MaximumLength=MAXPATH*4;
			RtlInitAnsiString(&current_path,buf);
			RtlAnsiStringToUnicodeString(&us_Current_Path,&current_path,FALSE);
			KdPrint(("%Z",&current_path));
			ListenInit();
			break;
		}
	case MY_SSDT_IS_START_CTL_CODE:
		IsStart = TRUE;
		break;
	case MY_SSDT_IS_LOAD_DRIVER_CTL_CODE:
		chsetload = *(int *)pIrp->AssociatedIrp.SystemBuffer;
		if(chsetload)
			IsLoadDriver = TRUE;
		else
			IsLoadDriver = FALSE;
		break;
	case MY_SSDT_IS_SET_TIME_CTL_CODE:
		chsettime = *(int *)pIrp->AssociatedIrp.SystemBuffer;
		if(chsettime)
			IsSetTime = TRUE;
		else
			IsSetTime = FALSE;
		break;
	case MY_SSDT_IS_HIDE_CTL_CODE:
		nohide = *(int *)pIrp->AssociatedIrp.SystemBuffer;
		if(nohide)
		{
			MyStartTimer();
		}else
		{
			MyEndTimer();
		}
		break;
	default:
		KdPrint(("Other"));
	}
		
	Status = STATUS_SUCCESS;
	pIrp->IoStatus.Status = Status;
	pIrp->IoStatus.Information = 0;
	
	IoCompleteRequest(pIrp,IO_NO_INCREMENT);
	return Status;
}

VOID MyGetCmpName(int File_Chose)
{
	HANDLE				hFile;
	IO_STATUS_BLOCK		isb;
	OBJECT_ATTRIBUTES	ObjAttr;
	NTSTATUS			Status;
	UNICODE_STRING		usFileName,usFileBase;
	LARGE_INTEGER		li;
	ULONG				FileLen;
	FILE_STANDARD_INFORMATION fsi;
	PCHAR				CmpNamebuf;
	LARGE_INTEGER		liOffSet;
	CHAR				buf[1024];
	int					i,f;
	KIRQL				Irql;
	

	RtlInitUnicodeString(&usFileBase,L"\\??\\");

	usFileName.Buffer = ExAllocatePool(PagedPool,MAXPATH);
	usFileName.MaximumLength = MAXPATH;
	RtlCopyUnicodeString(&usFileName,&usFileBase);
	RtlAppendUnicodeStringToString(&usFileName,&us_Current_Path);
	switch (File_Chose)
	{
	case PROCESS_CHOSE:
		RtlAppendUnicodeToString(&usFileName,PROCESS_LIST_PATH);
		break;
	case REG_CHOSE:
		RtlAppendUnicodeToString(&usFileName,REG_LIST_PATH);
		break;
	case FILE_CHOSE:
		RtlAppendUnicodeToString(&usFileName,FILE_LIST_PATH);
	}
	KdPrint(("%wZ",&usFileName));
	InitializeListHead(&NameList[File_Chose]);//初始化链表

	InitializeObjectAttributes(&ObjAttr,&usFileName,OBJ_CASE_INSENSITIVE,NULL,NULL);
	//打开文件
	Status = ZwCreateFile(&hFile,
		FILE_ALL_ACCESS ,
		&ObjAttr,
		&isb,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ,
		FILE_OPEN_IF,
		FILE_NON_DIRECTORY_FILE,
		NULL,
		0);

	if(!NT_SUCCESS(Status))
	{
		KdPrint(("Faild   %X",Status));
		RtlFreeUnicodeString(&usFileName);
		return ;
	}
	//文件字节数
	Status = ZwQueryInformationFile(hFile,
						&isb,
						&fsi,
						sizeof(FILE_STANDARD_INFORMATION),
						FileStandardInformation);
	if(!NT_SUCCESS(Status))
	{
		KdPrint(("查询失败!"));
		RtlFreeUnicodeString(&usFileName);
		ZwClose(hFile);
		return ;
	}
	KdPrint(("%d",fsi.EndOfFile));
	FileLen = fsi.EndOfFile.QuadPart;
	
	CmpNamebuf  = (PCHAR)ExAllocatePool(NonPagedPool,FileLen+10);
	RtlZeroMemory(CmpNamebuf,FileLen+10);
	liOffSet.QuadPart = 0;
	Status = ZwReadFile(hFile,
		NULL,
		NULL,
		NULL,
		&isb,
		CmpNamebuf,
		FileLen,
		&liOffSet,
		NULL);

	if(!NT_SUCCESS(Status))
	{
		KdPrint(("%X",Status));
		KdPrint(("读文件失败!"));
		ExFreePool(CmpNamebuf);
		RtlFreeUnicodeString(&usFileName);
		ZwClose(hFile);
		return ;
	}
	ZwClose(hFile);

	KeInitializeSpinLock(&Spin_lock);//初始化自旋锁

	KeAcquireSpinLock(&Spin_lock,&Irql);//激活自旋锁
	f=0;
	for (i=0;i<FileLen;i++)
	{
		if(CmpNamebuf[i]=='|')
		{	
			RtlZeroMemory(buf,MAXPATH);
			RtlCopyMemory(buf,CmpNamebuf+f,i-f);
			MyCreateList(buf,i-f,File_Chose);
			f = i+1;
		}
	}	
	KeReleaseSpinLock(&Spin_lock,Irql);//关闭自旋锁
	ExFreePool(CmpNamebuf);
	RtlFreeUnicodeString(&usFileName);

}
VOID MyFreeList(int List_Chose)
{
	PMY_DATA	pData;
	while(!IsListEmpty(&NameList[List_Chose]))
	{		
		PLIST_ENTRY plist = RemoveTailList(&NameList[List_Chose]);
		pData = CONTAINING_RECORD(plist,MY_DATA,list);
		ExFreePool(pData->CmpName.Buffer);
		ExFreePool(pData);
	}

}
NTSTATUS MyCreateList(PCHAR CmpNamebuf,ULONG len,int List_Chose)
{
	NTSTATUS		Status;
	PMY_DATA		pData;

	pData = (PMY_DATA)ExAllocatePool(NonPagedPool,sizeof(MY_DATA));
	pData->CmpName.Buffer = ExAllocatePool(NonPagedPool,MAXPATH);
	pData->CmpName.MaximumLength = MAXPATH;
	pData->CmpName.Length = len;
	RtlCopyMemory(pData->CmpName.Buffer,CmpNamebuf,len);
	InsertTailList(&NameList[List_Chose],&pData->list);	
	
	Status = STATUS_SUCCESS;
	return Status;
}