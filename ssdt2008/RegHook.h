#ifndef REGHOOK_H
#define REGHOOK_H

#include <ntddk.h>

void GetRegistryPath(HANDLE pKeyHandle, PUNICODE_STRING pRegPath);

VOID Reg_Del_Thread_Func(PIRP  pIrp);
VOID Reg_Set_Thread_Func(PIRP  pIrp);
NTSTATUS SetRegSSDTHook();
NTSTATUS UnSetRegSSDTHook();

#endif