; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CToolDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "FireWall.h"

ClassCount=10
Class1=CFireWallApp
Class2=CFireWallDlg

ResourceCount=10
Resource2=IDD_REG_DIALOG
Resource1=IDR_MAINFRAME
Class3=CProcessDlg
Resource3=IDD_PROCESS_DIALOG
Class4=CRegDlg
Resource4=IDD_FILE_DIALOG
Class5=CLogListDlg
Resource5=IDD_TOOL_DIALOG
Class6=CFileDlg
Resource6=IDD_FIREWALL_DIALOG
Class7=CIndexDlg
Resource7=IDD_NET_DIALOG
Resource8=IDD_LOG_LIST
Class8=CNetDlg
Class9=CToolDlg
Resource9=IDD_INDEX_DIALOG
Class10=CBoxDlg
Resource10=IDD_BOX_DIALOG

[CLS:CFireWallApp]
Type=0
HeaderFile=FireWall.h
ImplementationFile=FireWall.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=CFireWallApp

[CLS:CFireWallDlg]
Type=0
HeaderFile=FireWallDlg.h
ImplementationFile=FireWallDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CFireWallDlg



[DLG:IDD_FIREWALL_DIALOG]
Type=1
Class=CFireWallDlg
ControlCount=1
Control1=IDC_TAB,SysTabControl32,1342179584

[DLG:IDD_PROCESS_DIALOG]
Type=1
Class=CProcessDlg
ControlCount=8
Control1=IDC_LIST_PROCESS,SysListView32,1350631425
Control2=IDC_BTN_ADD,button,1342242816
Control3=IDC_BTN_UPDATE,button,1342242816
Control4=IDC_BTN_DEL,button,1342242816
Control5=IDC_BTN_SAVE,button,1342242816
Control6=IDC_BTN_LOG,button,1342242816
Control7=IDC_EDIT_LIST,edit,1350631552
Control8=IDC_BTN_READ,button,1342242816

[CLS:CProcessDlg]
Type=0
HeaderFile=ProcessDlg.h
ImplementationFile=ProcessDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CProcessDlg
VirtualFilter=dWC

[DLG:IDD_REG_DIALOG]
Type=1
Class=CRegDlg
ControlCount=8
Control1=IDC_LIST_REG,SysListView32,1350631425
Control2=IDC_BTN_ADD,button,1342242816
Control3=IDC_BTN_UPDATE,button,1342242816
Control4=IDC_BTN_DEL,button,1342242816
Control5=IDC_BTN_SAVE,button,1342242816
Control6=IDC_BTN_LOG,button,1342242816
Control7=IDC_EDIT_LIST,edit,1350631552
Control8=IDC_BTN_READ,button,1342242816

[CLS:CRegDlg]
Type=0
HeaderFile=RegDlg.h
ImplementationFile=RegDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_EDIT_LIST
VirtualFilter=dWC

[DLG:IDD_LOG_LIST]
Type=1
Class=CLogListDlg
ControlCount=3
Control1=IDC_LIST,SysListView32,1350631427
Control2=IDC_DEL_LOG,button,1342242816
Control3=IDC_RE_READ,button,1342242816

[CLS:CLogListDlg]
Type=0
HeaderFile=LogListDlg.h
ImplementationFile=LogListDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CLogListDlg
VirtualFilter=dWC

[DLG:IDD_FILE_DIALOG]
Type=1
Class=CFileDlg
ControlCount=8
Control1=IDC_LIST_FILE,SysListView32,1350631425
Control2=IDC_BTN_ADD,button,1342242816
Control3=IDC_BTN_UPDATE,button,1342242816
Control4=IDC_BTN_DEL,button,1342242816
Control5=IDC_BTN_SAVE,button,1342242816
Control6=IDC_BTN_LOG,button,1342242816
Control7=IDC_EDIT_LIST,edit,1350631552
Control8=IDC_BTN_READ,button,1342242816

[CLS:CFileDlg]
Type=0
HeaderFile=FileDlg.h
ImplementationFile=FileDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CFileDlg
VirtualFilter=dWC

[DLG:IDD_INDEX_DIALOG]
Type=1
Class=CIndexDlg
ControlCount=1
Control1=IDC_STATIC,static,1342177294

[CLS:CIndexDlg]
Type=0
HeaderFile=IndexDlg.h
ImplementationFile=IndexDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CIndexDlg

[DLG:IDD_NET_DIALOG]
Type=1
Class=CNetDlg
ControlCount=10
Control1=IDC_LIST_FILE,SysListView32,1350631425
Control2=IDC_BTN_ADD,button,1342242816
Control3=IDC_BTN_UPDATE,button,1342242816
Control4=IDC_BTN_DEL,button,1342242816
Control5=IDC_BTN_SAVE,button,1342242816
Control6=IDC_BTN_LOG,button,1342242816
Control7=IDC_EDIT_LIST,edit,1350631552
Control8=IDC_BTN_READ,button,1342242816
Control9=IDC_BTN_DOWN,button,1342242816
Control10=IDC_BTN_UP,button,1342242816

[DLG:IDD_TOOL_DIALOG]
Type=1
Class=CToolDlg
ControlCount=17
Control1=IDC_STATIC,button,1342177287
Control2=IDC_NO_CMD,button,1342242819
Control3=IDC_NO_PING,button,1342242819
Control4=IDC_NO_CHANGE_TIME,button,1342242819
Control5=IDC_NO_ADD_USER,button,1342242819
Control6=IDC_NO_DEL_USER,button,1342242819
Control7=IDC_NO_REG,button,1342242819
Control8=IDC_SAVE,button,1342242816
Control9=IDC_RESET,button,1342242816
Control10=IDC_NO_NET,button,1342242819
Control11=IDC_NO_HIDE_THREAD,button,1342242819
Control12=IDC_NO_LOAD_DRVIER,button,1342242819
Control13=IDC_STATIC,button,1342177287
Control14=IDC_RUN_START,button,1342242819
Control15=IDC_EXIT_TO_DISH,button,1342242819
Control16=IDC_SAVE_SYSTEM,button,1342242816
Control17=IDC_RESET_SYSTEM,button,1342242816

[CLS:CNetDlg]
Type=0
HeaderFile=NetDlg.h
ImplementationFile=NetDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CNetDlg
VirtualFilter=dWC

[CLS:CToolDlg]
Type=0
HeaderFile=ToolDlg.h
ImplementationFile=ToolDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_RUN_START
VirtualFilter=dWC

[DLG:IDD_BOX_DIALOG]
Type=1
Class=CBoxDlg
ControlCount=3
Control1=IDC_TEXT,static,1342308352
Control2=IDC_MY_CLOSE,button,1342242816
Control3=IDC_SEE,button,1342242816

[CLS:CBoxDlg]
Type=0
HeaderFile=BoxDlg.h
ImplementationFile=BoxDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CBoxDlg
VirtualFilter=dWC

