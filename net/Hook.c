#include "precomp.h"
#include "ntstrsafe.h"
#include "Global.h"

PDEVICE_OBJECT gDeviceObject = NULL;

extern LIST_ENTRY NetNameList;
extern KEVENT Net_PASS_Event;
extern KEVENT Net_DENY_Event;
extern BOOLEAN		IsStart;
extern BOOLEAN		MyDefault;
extern BOOLEAN		MyPING;
extern UNICODE_STRING	us_Current_Path;
extern MSG_STRUCT	Net_Pass_Msg;
extern MSG_STRUCT	Net_Deny_Msg;
// ���������Լ�����IRP_MJ_CREATE�ķ���
NTSTATUS 
myCreate(
		 IN PDEVICE_OBJECT DeviceObject, 
		 IN PIRP Irp
		 )
{
	KdPrint(("Create\n"));
	
	// ����������ǵ��豸��ֱ�ӷ��سɹ�
	if(DeviceObject == gDeviceObject)
	{
		KdPrint(("6666"));
		return STATUS_SUCCESS;
	}
	// ��Ҫ����ϵͳ����
	return systemCreate(DeviceObject, Irp);
}


// ���������Լ�����IRP_MJ_WRITE�ķ���
NTSTATUS 
myWrite(
		IN PDEVICE_OBJECT DeviceObject, 
		IN PIRP Irp
		)
{
	KdPrint(("Write\n"));
	// �ж��ǲ������ǵ��豸
	if(DeviceObject == gDeviceObject){
		// 
		NTSTATUS status = STATUS_SUCCESS;

		//
		// ����������Ҫ��IRP�е����ݽ�����Ӧ�ı��桢����
		//

		// ֱ�ӷ���
		return status;
	}

	// ��Ҫ����ϵͳ����
	return systemWrite(DeviceObject, Irp);
}


// ���������Լ�����IRP_MJ_READ�ķ���
NTSTATUS 
myRead(
	   IN PDEVICE_OBJECT DeviceObject, 
	   IN PIRP Irp
	   )
{
	// �ж��ǲ������ǵ��豸
	if(DeviceObject == gDeviceObject){
		// 
		NTSTATUS status = STATUS_SUCCESS;

		//
		// ����������Ҫ��һЩ������䵽IRP�е����ݻ�������
		//

		// ֱ�ӷ���
		return status;
	}

	// ��Ҫ����ϵͳ����
	return systemRead(DeviceObject, Irp);
}


// ���������Լ�����IRP_MJ_DEVICE_CONTROL�ķ���
NTSTATUS 
myDeviceControl(
				IN PDEVICE_OBJECT DeviceObject, 
				IN PIRP Irp
				)
{
	// �ж��ǲ������ǵ��豸
	if(DeviceObject == gDeviceObject)
	{
		//
		NTSTATUS status = STATUS_SUCCESS;
		PIO_STACK_LOCATION pStack;
		ULONG code; 
		HANDLE hNetPassThread;
		HANDLE hNetDelayThread;
		int net_thread_chose;
		ANSI_STRING current_path;
		CHAR *buf=NULL;
		int chping,chnet;
		pStack = IoGetCurrentIrpStackLocation(Irp);	

		code = pStack->Parameters.DeviceIoControl.IoControlCode;

		switch(code)
		{
		case MY_NET_CTL_CODE:
			hNetPassThread=NULL;
			hNetDelayThread =NULL;
			net_thread_chose = *(int*)Irp->AssociatedIrp.SystemBuffer;
			switch(net_thread_chose)
			{
			case MY_NET_PASS_THREAD:
				status = PsCreateSystemThread(&hNetPassThread,THREAD_ALL_ACCESS , NULL,NULL,NULL,(PKSTART_ROUTINE)Net_Pass_Thread_Func, Irp);
				break;
			case MY_NET_DENY_THREAD:
				status = PsCreateSystemThread(&hNetDelayThread,THREAD_ALL_ACCESS , NULL,NULL,NULL,(PKSTART_ROUTINE)Net_Deny_Thread_Func, Irp);
				break;
			}
			if (!NT_SUCCESS(status))
			{
				KdPrint(("Net Thread Failed!"));
				return status;
			}
			status = STATUS_PENDING;	
			IoMarkIrpPending(Irp);
			Irp->IoStatus.Status = status;
			Irp->IoStatus.Information = 0;
			return status;
		case MY_NET_EXIT_CTL_CODE:
			IsStart =FALSE;
			KeSetEvent(&Net_PASS_Event,IO_NO_INCREMENT,FALSE);
			KeSetEvent(&Net_DENY_Event,IO_NO_INCREMENT,FALSE);
			MyEnd();
			RtlFreeUnicodeString(&us_Current_Path);
			break;
		case MY_NET_RESTART_CTL_CODE:
			RtlZeroMemory(&Net_Pass_Msg,sizeof(MSG_STRUCT));
			RtlZeroMemory(&Net_Deny_Msg,sizeof(MSG_STRUCT));
			//KeSetEvent(&Net_PASS_Event,IO_NO_INCREMENT,FALSE);
			//KeSetEvent(&Net_DENY_Event,IO_NO_INCREMENT,FALSE);
			MyFreeList();
			MyInitNameList();
			//
			//KeResetEvent(&Net_PASS_Event);
			//KeResetEvent(&Net_DENY_Event);
			break;
		case MY_NET_ISSTART_CTL_CODE:
			MyStart();
			IsStart = TRUE;
			break;
		case MY_NET_DEF_PASS_CTL_CODE://def true pass
			chnet = *(int*)Irp->AssociatedIrp.SystemBuffer;
			if(chnet)
			{
				MyDefault = TRUE;
			}else
			{
				MyDefault = FALSE;
			}
			break;
		case MY_NET_NO_PING_CTL_CODE://def  ping
			chping = *(int*)Irp->AssociatedIrp.SystemBuffer;
			if(chping)
			{
				MyPING = TRUE;
			}else
			{
				MyPING = FALSE;
			}
			break;
		case MY_CURRENT_PATH_CTL_CODE:
			buf = (CHAR *)Irp->AssociatedIrp.SystemBuffer;
			us_Current_Path.Buffer = ExAllocatePool(NonPagedPool,MAXPATH*4);
			us_Current_Path.MaximumLength=MAXPATH*4;
			RtlInitAnsiString(&current_path,buf);
			RtlAnsiStringToUnicodeString(&us_Current_Path,&current_path,FALSE);
			break;
		}
		Irp->IoStatus.Status = status;
		Irp->IoStatus.Information = 0;

		IoCompleteRequest(Irp,IO_NO_INCREMENT);
		return status;
	}

	// ��Ҫ����ϵͳ����
	return systemDeviceControl(DeviceObject, Irp);
}

// 
// ʵ�������Լ���AddDevice����
//
NTSTATUS myAddDevice(
					 IN PDRIVER_OBJECT  DriverObject,
					 IN PDEVICE_OBJECT  PhysicalDeviceObject 
					 )
{
	PLIST_ENTRY			plist;
	PMY_NET_DATA		pData;


	if(gDeviceObject == NULL)
	{
		NTSTATUS	status;
		UNICODE_STRING	usDevName;
		UNICODE_STRING  usSymName;
		PDEVICE_OBJECT  pDevObj;

		RtlInitUnicodeString(&usDevName,L"\\Device\\FIREWALLNDIS");
		
		status = IoCreateDevice(
			DriverObject,
			0,
			&usDevName,
			FILE_DEVICE_UNKNOWN,
			FILE_DEVICE_SECURE_OPEN,
			FALSE,
			&gDeviceObject
			);

		if(status != STATUS_SUCCESS){
			KdPrint(("Failed to create device %ws\n", usDevName));
			return status;
		}
		gDeviceObject->Flags|=DO_BUFFERED_IO;
		gDeviceObject->Flags&=~DO_DEVICE_INITIALIZING;
		//sym
		RtlInitUnicodeString(&usSymName,L"\\??\\FIREWALLNDIS");

		status = IoCreateSymbolicLink(&usSymName,&usDevName);

		if(!NT_SUCCESS(status))
		{
			IoDeleteDevice(gDeviceObject);
			gDeviceObject =NULL;
			KdPrint(("Failed to create sym"));
			return status;
		}
		
	}
	
	return systemAddDevice(DriverObject, PhysicalDeviceObject);
}
NTSTATUS myClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	// �ж��ǲ������ǵ��豸
	if(DeviceObject == gDeviceObject){
		// 
		NTSTATUS status = STATUS_SUCCESS;


		// ֱ�ӷ���
		return status;
	}

	// ��Ҫ����ϵͳ����
	return systemClose(DeviceObject, Irp);
}
NTSTATUS myCleanUp(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	// �ж��ǲ������ǵ��豸
	if(DeviceObject == gDeviceObject){
		// 
		NTSTATUS status = STATUS_SUCCESS;

		// ֱ�ӷ���
		return status;
	}

	// ��Ҫ����ϵͳ����
	return systemCleanUp(DeviceObject, Irp);
}