#ifndef FILEHOOK_H
#define FILEHOOK_H

#include <ntddk.h>



VOID File_Delete_Thread_Func(PIRP  pIrp);
VOID File_Create_Thread_Func(PIRP  pIrp);
NTSTATUS SetFileSSDTHook();
NTSTATUS UnSetFileSSDTHook();

#endif