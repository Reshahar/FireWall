//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 2001.
//
//  File:       B I N D V I E W . H
//
//  Contents:   Function Prototypes
//
//  Notes:      
//
//  Author:     Alok Sinha    15-May-01
//
//----------------------------------------------------------------------------

#ifndef _BINDVIEW_H_INCLUDED

#define _BINDVIEW_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>
#include <wchar.h>
#include <commctrl.h>        // For common controls, e.g. Tree
#include <commdlg.h>
#include <setupapi.h>
#include <devguid.h>

#include "NetCfgAPI.h"
#include "resource.h"

#define ID_STATUS           100
#define APP_NAME            L"BindView"

#define CLIENTS_SELECTED    0
#define SERVICES_SELECTED   1
#define PROTOCOLS_SELECTED  2
#define ADAPTERS_SELECTED   3

#define ITEM_NET_COMPONENTS 1
#define ITEM_NET_BINDINGS   2
#define ITEM_NET_ADAPTERS   4

#define DEFAULT_COMPONENT_SELECTED  CLIENTS_SELECTED

#define WM_NO_COMPONENTS    WM_USER+1

#define MENUITEM_ENABLE     L"Enable"
#define MENUITEM_DISABLE    L"Disable"

extern HINSTANCE    hInstance;
extern const GUID   *pguidNetClass [];
extern LPWSTR       lpszNetClass [];

typedef struct _BIND_UNBIND_INFO {
  LPWSTR lpszInfId;
  BOOL   fBindTo;
} BIND_UNBIND_INFO, *LPBIND_UNBIND_INFO;

HRESULT InstallSpecifiedComponent (LPWSTR lpszInfFile,
                                   LPWSTR lpszPnpID,
                                   const GUID *pguidClass);

#endif
