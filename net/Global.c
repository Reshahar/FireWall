
#include "precomp.h"
#include "Global.h"

LIST_ENTRY NetNameList;
KSPIN_LOCK	Spin_lock;
KEVENT		Net_PASS_Event;
KEVENT		Net_DENY_Event;
BOOLEAN		IsStart=FALSE;
MSG_STRUCT	Net_Pass_Msg;
MSG_STRUCT	Net_Deny_Msg;
BOOLEAN		MyDefault=TRUE;
BOOLEAN		MyPING = TRUE;
UNICODE_STRING	us_Current_Path;



ULONG StringToInt(char * hex,int l,int base)
{
	ULONG ret = 0,tmp;
	ULONG count = 1;
	int i;
	for(i=l-1;i>=0;i--)   
	{   
		if ((hex[i]>='0') && (hex[i]<='9'))   
			tmp=hex[i]-48;
		else if ((hex[i]>='a') && (hex[i]<='f'))   
			tmp=hex[i]-'a'+10;   
		else if ((hex[i]>='A') && (hex[i]<='F'))   
			tmp=hex[i]-'A'+10;   
		else   
			tmp=0; 
		ret=ret+tmp*count;   
		count=count*base;  
	} 
	return ret;
}

void StrToIP(char * ipstr,char *hexstr)
{
	size_t num;
	int a;
	int i;
	char *buf=NULL;
	RtlStringCbLengthA(hexstr,NTSTRSAFE_MAX_CCH * sizeof(char),&num);
	if (num<8)
		return ;
	if(ipstr==NULL)
		return ;
	buf = ExAllocatePool(NonPagedPool,30);
	RtlStringCbPrintfA(buf,30,"%d.%d.%d.%d",StringToInt(hexstr,2,16),StringToInt(hexstr+2,2,16),StringToInt(hexstr+4,2,16),StringToInt(hexstr+6,2,16));	
	RtlCopyMemory(ipstr,buf,strlen(buf));
	ExFreePool(buf);

}
void IPToStr(char * ip,int len,char *out)
{
	char buf[4];
	int i,f,c;
	f =0;
	c =0;
	for(i=0;i<len;i++)
	{
		if((ip[i]=='.')||(i==len-1))
		{
			if(i==len-1)
				i++;
			RtlZeroMemory(buf,4);
			RtlCopyMemory(buf,ip+f,i-f);
			RtlStringCbPrintfA(out+2*c,10-2*c,"%02x",StringToInt(buf,i-f,10));
			c++;
			f = i+1;
		}
		
	}
}
NTSTATUS MyCreateList(PCHAR CmpNamebuf,ULONG len,ULONG sub,ULONG pass)
{
	NTSTATUS			Status;
	PMY_NET_DATA		pData;

	pData = (PMY_NET_DATA)ExAllocatePool(NonPagedPool,sizeof(MY_NET_DATA));
	pData->ip.Buffer = ExAllocatePool(NonPagedPool,MAXPATH);
	pData->ip.MaximumLength = MAXPATH;
	pData->ip.Length = len;
	RtlCopyMemory(pData->ip.Buffer,CmpNamebuf,len);
	pData->pass = pass;
	pData->sub = sub;
	InsertTailList(&NetNameList,&pData->list);	

	Status = STATUS_SUCCESS;
	return Status;
}


VOID MyInitNameList()
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
	CHAR				ip[1024],tmp[20];
	int					i,f,c,l;
	KIRQL				Irql;
	ULONG				sub,pass;

	RtlInitUnicodeString(&usFileBase,L"\\??\\");

	usFileName.Buffer = ExAllocatePool(NonPagedPool,MAXPATH);
	usFileName.MaximumLength = MAXPATH;
	RtlCopyUnicodeString(&usFileName,&usFileBase);
	RtlAppendUnicodeStringToString(&usFileName,&us_Current_Path);
	
	RtlAppendUnicodeToString(&usFileName,NET_LIST_PATH);

	KdPrint(("%wZ",&usFileName));
	InitializeListHead(&NetNameList);//初始化链表

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
	c = 0;
	for (i=0;i<FileLen;i++)
	{	
		if(CmpNamebuf[i]=='|')
		{	
			switch(c)
			{
			case 0:
				RtlZeroMemory(ip,MAXPATH);
				RtlCopyMemory(ip,CmpNamebuf+f,i-f);
				l = i-f;
				break;
			case 1:
				RtlZeroMemory(tmp,20);
				RtlCopyMemory(tmp,CmpNamebuf+f,i-f);
				sub = StringToInt(tmp,i-f,10);
				break;
			case 2:
				RtlZeroMemory(tmp,20);
				RtlCopyMemory(tmp,CmpNamebuf+f,i-f);
				pass = StringToInt(tmp,i-f,10);
				KdPrint(("%s,%d,%d,%d",ip,l,sub,pass));
				MyCreateList(ip,l,sub,pass);
				c = -1;
				i+=2;
				break;
			}	
			f = i+1;
			c++;
		}
	}	
	KeReleaseSpinLock(&Spin_lock,Irql);//关闭自旋锁
	//KdPrint(("%s",CmpNamebuf));
	ExFreePool(CmpNamebuf);
	RtlFreeUnicodeString(&usFileName);

}
VOID MyFreeList()
{
	PMY_NET_DATA	pData;
	while(!IsListEmpty(&NetNameList))
	{		
		PLIST_ENTRY plist = RemoveTailList(&NetNameList);
		pData = CONTAINING_RECORD(plist,MY_NET_DATA,list);
		ExFreePool(pData->ip.Buffer);
		ExFreePool(pData);
	}

}
VOID Net_Deny_Thread_Func(PIRP  pIrp)
{
	CHAR * buf;
	if(Net_Deny_Msg.ProcessNameLen==0)
		KeResetEvent(&Net_DENY_Event);	
	KdPrint(("Deny Thread\n"));	
	KeWaitForSingleObject(&Net_DENY_Event,Executive,KernelMode,FALSE,0);
	KeResetEvent(&Net_DENY_Event);                                                                                                                                          
	buf = (CHAR*)pIrp->AssociatedIrp.SystemBuffer;
	RtlCopyMemory(buf,&Net_Deny_Msg,sizeof(MSG_STRUCT));
	pIrp->IoStatus.Status = STATUS_SUCCESS;   
	pIrp->IoStatus.Information = sizeof(MSG_STRUCT);
	
	IoCompleteRequest(pIrp, IO_NO_INCREMENT); 
	RtlZeroMemory(&Net_Deny_Msg,sizeof(MSG_STRUCT));
	PsTerminateSystemThread(STATUS_SUCCESS); 
}
VOID Net_Pass_Thread_Func(PIRP  pIrp)
{
	CHAR * buf;
	if(Net_Pass_Msg.ProcessNameLen==0)
		KeResetEvent(&Net_PASS_Event);
	KdPrint(("Pass Thread\n"));
	KeWaitForSingleObject(&Net_PASS_Event,Executive,KernelMode,FALSE,0);
	KeResetEvent(&Net_PASS_Event);                                                                                                                                          
	buf = (CHAR*)pIrp->AssociatedIrp.SystemBuffer;
	RtlCopyMemory(buf,&Net_Pass_Msg,sizeof(MSG_STRUCT));
	pIrp->IoStatus.Status = STATUS_SUCCESS;   
	pIrp->IoStatus.Information = sizeof(MSG_STRUCT);
	
	IoCompleteRequest(pIrp, IO_NO_INCREMENT); 

	RtlZeroMemory(&Net_Pass_Msg,sizeof(MSG_STRUCT));
	PsTerminateSystemThread(STATUS_SUCCESS);  

}


VOID MyStart()
{
	KeInitializeEvent(&Net_PASS_Event,NotificationEvent,FALSE);
	KeInitializeEvent(&Net_DENY_Event,NotificationEvent,FALSE);
	MyInitNameList();

}
VOID MyEnd()
{
	MyFreeList();
}
int CheckIPAndSub(char * ip,int len,ULONG sub,char * packetip)
{
	int ret = 0,i;
	char strsubnum[32];
	char strip[10];
	ULONG uip,upacketip,usubnum;
	RtlZeroMemory(strsubnum,32);
	RtlZeroMemory(strip,10);
	for(i=0;i<32;i++)
	{
		if(i<sub)
			strsubnum[i]='1';
		else
			strsubnum[i]='0';
	}
	usubnum = StringToInt(strsubnum,32,2);
	upacketip = StringToInt(packetip,8,16);
	IPToStr(ip,len,strip);
	//KdPrint(("%s",strip));
	uip = StringToInt(strip,8,16);
	
	if((usubnum&upacketip) == (usubnum &uip))
	{
		return 1;
	}else
	{
		return 0;
	}
	
}
NTSTATUS PsLookupProcessByProcessId(
									HANDLE    ProcessId,
									PEPROCESS *Process
									);

NTSTATUS IoQueryFileDosDeviceName(
								  PFILE_OBJECT             FileObject,
								  POBJECT_NAME_INFORMATION *ObjectNameInformation
								  );


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
	UNICODE_STRING u_strPath,dosName;
	NTSTATUS status;
	POBJECT_NAME_INFORMATION ObjectNameInformation = NULL;

	__try
	{
		if (FileObject && MmIsAddressValid(FileObject) && pAnsi_strPath)
		{
			if(NT_SUCCESS(IoVolumeDeviceToDosName(FileObject->DeviceObject,&dosName)))
			{
				u_strPath.Buffer = ExAllocatePool(NonPagedPool, MAXPATH*4);
				u_strPath.MaximumLength = MAXPATH*4;
				RtlCopyUnicodeString(&u_strPath,&dosName);
				RtlAppendUnicodeStringToString(&u_strPath,&(FileObject->FileName));
				RtlUnicodeStringToAnsiString(pAnsi_strPath, &u_strPath, FALSE);
				
				ExFreePool(dosName.Buffer);
				ExFreePool(u_strPath.Buffer);
				bGetPath = TRUE;
			}
			//if (NT_SUCCESS(IoQueryFileDosDeviceName(FileObject, &ObjectNameInformation))) 
			//{
			//	//status = IoQueryFileDosDeviceName(FileObject, &ObjectNameInformation);
			//	u_strPath.Buffer = ExAllocatePool(NonPagedPool, 2048);
			//	u_strPath.MaximumLength = 2048;
			//	RtlCopyUnicodeString(&u_strPath, &(ObjectNameInformation->Name));
			//	RtlUnicodeStringToAnsiString(pAnsi_strPath, &u_strPath, FALSE);

			//	bGetPath = TRUE;
			//	ExFreePool(u_strPath.Buffer);
			//	ExFreePool(ObjectNameInformation);
			//}
		}
	}
	__except(1)
	{
		KdPrint(("[GetPathByFileObject]NULL"));
		bGetPath = FALSE;
	}

	return bGetPath;
}
