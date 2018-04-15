#ifndef OTHERHOOK_H
#define OTHERHOOK_H

#include <ntddk.h>

NTSTATUS UnSetOtherSSDTHook();
NTSTATUS SetOtherSSDTHook();
NTSTATUS SetQuertSystemAddress();
NTSTATUS GetQuertSystemAddress();
VOID MyStartTimer();
VOID MyEndTimer();
#endif