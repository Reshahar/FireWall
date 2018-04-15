#include "precomp.h"
#include "ntstrsafe.h"
#include "Global.h"

extern BOOLEAN		IsStart;
extern BOOLEAN		MyDefault;
extern BOOLEAN		MyPING;
extern KEVENT		Net_PASS_Event;
extern KEVENT		Net_DENY_Event;
extern LIST_ENTRY	NetNameList;
extern MSG_STRUCT	Net_Pass_Msg;
extern MSG_STRUCT	Net_Deny_Msg;
int arp=1,icmp=0;

typedef struct in_addr {
	union {
		struct { UCHAR s_b1,s_b2,s_b3,s_b4; } S_un_b;
		struct { USHORT s_w1,s_w2; } S_un_w;
		ULONG S_addr;
	} S_un;
} IN_ADDR, *PIN_ADDR, FAR *LPIN_ADDR;

typedef struct IP_HEADER
{
	unsigned char  VIHL;          // Version and IHL
	unsigned char  TOS;           // Type Of Service
	short          TotLen;        // Total Length
	short          ID;            // Identification
	short          FlagOff;       // Flags and Fragment Offset
	unsigned char  TTL;           // Time To Live
	unsigned char  Protocol;      // Protocol
	unsigned short Checksum;      // Checksum
	struct in_addr        iaSrc;  // Internet Address - Source
	struct in_addr        iaDst;  // Internet Address - Destination
}IP_HEADER, *PIP_HEADER;

#define IP_OFFSET                               0x0E

//IP Protocol Types
#define PROT_ICMP                               0x01 
#define PROT_TCP                                0x06 
#define PROT_UDP                                0x11 


// ���������
//	Packet�� ��������NDIS��������
//	bRecOrSend: ����ǽ��հ���ΪTRUE;���Ϊ���Ͱ���ΪFALSE��
// ����ֵ��
//	���������£�������ͨ������ֵ�Ծ�����δ���NDIS����������ʧ�ܡ�ת��
FILTER_STATUS AnalysisPacket(PNDIS_PACKET Packet,  BOOLEAN bRecOrSend)
{
	FILTER_STATUS status = STATUS_PASS; // Ĭ��ȫ��ͨ��
	PNDIS_BUFFER NdisBuffer ;
	UINT TotalPacketLength = 0;
	UINT copysize = 0;
	UINT DataOffset = 0 ;
	UINT PhysicalBufferCount;
	UINT BufferCount   ;
	PUCHAR pPacketContent = NULL;
	char* tcsPacketBuf = NULL;
	PUCHAR tembuffer = NULL ; 
	char * sourceip=NULL;
	char * destip = NULL;
	UINT j;
	ANSI_STRING			father_name;
	PLIST_ENTRY			plist;
	PMY_NET_DATA		pData;
	ULONG				pid;
	
	__try{

		status = NdisAllocateMemoryWithTag( &pPacketContent, 2048, TAG); 
		if( status != NDIS_STATUS_SUCCESS ){
			status = NDIS_STATUS_FAILURE ;
			__leave;
		}

		NdisZeroMemory( pPacketContent, 2048 ) ;

		// �ҵ���һ��Ndis_Buffer��Ȼ��ͨ��ͨ��NdisGetNextBuffer����ú�����NDIS_BUFFER��
		// ���ֻ���ҵ�һ���ڵ㣬�����ҷ���ķ����ǵ���NdisGetFirstBufferFromPacket��
		NdisQueryPacket(Packet,  // NDIS_PACKET        
			&PhysicalBufferCount,// �ڴ��е��������
			&BufferCount,		 // ���ٸ�NDIS_BUFFER��
			&NdisBuffer,         // �����ص�һ����
			&TotalPacketLength	 // �ܹ��İ����ݳ���
			);

		while(TRUE){

			// ȡ��Ndis_Buffer�д洢�������������ַ��
			// �����������һ���汾��NdisQueryBuffer��
			// ������ϵͳ��Դ�ͻ��������ľ���ʱ�򣬻����Bug Check������������
			NdisQueryBufferSafe(NdisBuffer,
				&tembuffer,// ��������ַ
				&copysize, // ��������С
				NormalPagePriority
				);

			// ���tembufferΪNULL��˵����ǰϵͳ��Դ�ѷ���
			if(tembuffer != NULL){
				NdisMoveMemory( pPacketContent + DataOffset , tembuffer, copysize) ;			
				DataOffset += copysize;
			}

			// �����һ��NDIS_BUFFER��
			// ����õ�����һ��NULLָ�룬˵���Ѿ�������ʽ��������ĩβ�����ǵ�ѭ��Ӧ�ý����ˡ�
			NdisGetNextBuffer(NdisBuffer , &NdisBuffer ) ;

			if( NdisBuffer == NULL )
				break ;
		}

		// ȡ�����ݰ����ݺ����潫�������ݽ��й��ˡ�
		// ��������������е�ʵ�֣������򵥵ش�ӡһЩ�ɶ���Log��Ϣ��
		if(pPacketContent[12] == 8 &&  pPacketContent[13] == 0 )  //is ip packet
		{	
			PIP_HEADER pIPHeader = (PIP_HEADER)(pPacketContent + IP_OFFSET);
			switch(pIPHeader->Protocol)
			{
			case PROT_ICMP:
				icmp =1;
				if(bRecOrSend)
					DbgPrint("Receive ICMP packet");
				else
					DbgPrint("Send ICMP packet");

				//
				// ȡ��ICMPͷ��������Ĺ����жϡ�
				// 

				break;
			case PROT_UDP:
				if(bRecOrSend)
					DbgPrint("Receive UDP packet");
				else
					DbgPrint("Send UDP packet");

				//
				// ȡ��UDPͷ��������Ĺ����жϡ�
				//

				break;
			case PROT_TCP:
				if(bRecOrSend)
					DbgPrint("Receive TCP packet");
				else
					DbgPrint("Send TCP packet");

				//
				// ȡ��TCPͷ��������Ĺ����жϡ�
				//

				break;
			}
		}else if(pPacketContent[12] == 8 &&  pPacketContent[13] == 6 ){
			arp=0;
			if(bRecOrSend)
				DbgPrint("Receive ARP packet");
			else
				DbgPrint("Send ARP packet");
		}else{
			arp=0;
			if(bRecOrSend)
				DbgPrint("Receive unknown packet");
			else
				DbgPrint("Send unknown packet");
		}

		// �򵥴�ӡ������������
		status = NdisAllocateMemoryWithTag( &tcsPacketBuf, 2048*3, TAG);  //�����ڴ��
		if( status != NDIS_STATUS_SUCCESS ){
			status = NDIS_STATUS_FAILURE ;
			__leave;
		}
		for(j=0;j<=DataOffset;j++)
			RtlStringCbPrintfA(tcsPacketBuf+j*2, 2048*3-j*2, "%02x",pPacketContent[j]);
			
		
		if(IsStart)
		{
			if(!MyDefault)
				return STATUS_DROP;

			if(icmp)
			{
				icmp =0;
				if(!MyPING)
				{
					return STATUS_DROP;
				}
			}
			if(arp&&bRecOrSend)
			{
				plist = NetNameList.Flink;
				while (plist!=&NetNameList)
				{
					pData = CONTAINING_RECORD(plist,MY_NET_DATA,list);
					if(CheckIPAndSub(pData->ip.Buffer,pData->ip.Length,pData->sub,tcsPacketBuf+26*2)==1)
					{	
						KdPrint(("CHECK"));
						if(pData->pass==1)
						{
							RtlZeroMemory(&Net_Pass_Msg,sizeof(MSG_STRUCT));
							Net_Pass_Msg.flag = NET_PASS;

							sourceip = ExAllocatePool(NonPagedPool,30);
							RtlZeroMemory(sourceip,30);
							StrToIP(sourceip,tcsPacketBuf+26*2);
							RtlCopyMemory(Net_Pass_Msg.OtherMsg,sourceip,strlen(sourceip));
							Net_Pass_Msg.OtherMsglen = strlen(sourceip);

							ExFreePool(sourceip);
							sourceip =NULL;
							
							KdPrint(("PASS"));
							destip = ExAllocatePool(NonPagedPool,30);
							RtlZeroMemory(destip,30);
							StrToIP(destip,tcsPacketBuf+30*2);
							RtlCopyMemory(Net_Pass_Msg.ProcessName,destip,strlen(destip));
							Net_Pass_Msg.ProcessNameLen = strlen(destip);
							ExFreePool(destip);
							destip = NULL;
							
							Net_Pass_Msg.Pid = (ULONG)PsGetCurrentProcessId();

							KeSetEvent(&Net_PASS_Event,IO_NO_INCREMENT,FALSE);

							return STATUS_PASS;
						}else
						{
							KdPrint(("DENY"));
							Net_Deny_Msg.Pid = (ULONG)PsGetCurrentProcessId();
							
							RtlZeroMemory(&Net_Deny_Msg,sizeof(MSG_STRUCT));
							Net_Deny_Msg.flag = NET_DENY;
							
							sourceip = ExAllocatePool(NonPagedPool,30);
							RtlZeroMemory(sourceip,30);
							StrToIP(sourceip,tcsPacketBuf+26*2);
							RtlCopyMemory(Net_Deny_Msg.OtherMsg,sourceip,strlen(sourceip));
							Net_Deny_Msg.OtherMsglen = strlen(sourceip);
							
							ExFreePool(sourceip);
							sourceip =NULL;

							Net_Deny_Msg.Pid = (ULONG)PsGetCurrentProcessId();

							destip = ExAllocatePool(NonPagedPool,30);
							RtlZeroMemory(destip,30);
							StrToIP(destip,tcsPacketBuf+30*2);
							KdPrint(("deny IP:%s",destip));
							RtlCopyMemory(Net_Deny_Msg.ProcessName,destip,strlen(destip));
							Net_Deny_Msg.ProcessNameLen = strlen(destip);
							ExFreePool(destip);
							destip = NULL;
							
							KeSetEvent(&Net_DENY_Event,IO_NO_INCREMENT,FALSE);

							
							return STATUS_DROP;
						}
						
					}	
					plist = plist->Flink;
				}	
				
			}else
			{
				KdPrint(("Arp %d\n",PsGetCurrentProcessId()));
				arp =1;
			}
		}else
		{
			KdPrint(("No Start"));
		}
		
	}__finally{
		if(pPacketContent)NdisFreeMemory(pPacketContent, 0, 0);
		if(tcsPacketBuf)NdisFreeMemory(tcsPacketBuf, 0, 0);
	}
}

