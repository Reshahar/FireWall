//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 2001.
//
//  File:       B I N D V I E W . C P P
//
//  Contents:  
//
//  Notes:      
//
//  Author:     Alok Sinha    15-Amy-01
//
//----------------------------------------------------------------------------


#include "BindView.h"

//----------------------------------------------------------------------------
// Globals
//

//
// Image list for devices of various setup class.
//

SP_CLASSIMAGELIST_DATA ClassImageListData;

HINSTANCE              hInstance;
HMENU                  hMainMenu;
HMENU                  hComponentSubMenu;
HMENU                  hBindingPathSubMenu;

//
// Network components whose bindings are enumerated.
//

LPWSTR   lpszNetClass[] = {
                    L"All Clients",
                    L"All Services",
                    L"All Protocols"
         };

//
// GUIDs of network components.
//

const GUID     *pguidNetClass [] = {
                     &GUID_DEVCLASS_NETCLIENT,
                     &GUID_DEVCLASS_NETSERVICE,
                     &GUID_DEVCLASS_NETTRANS,
                     &GUID_DEVCLASS_NET
         };



DWORD GetServiceInfFilePath_Passthru( 
									 LPTSTR lpFilename,
									 DWORD nSize
									 )
{
	// Get Path to This Module
	DWORD    nResult;
	TCHAR    szDrive[ _MAX_DRIVE ];
	TCHAR    szDir[ _MAX_DIR ];
	
	nResult = GetModuleFileName( NULL, lpFilename, nSize );
	
	if( nResult == 0 )
	{
		return 0;
	}
	
	_wsplitpath( lpFilename, szDrive, szDir, NULL, NULL );
	
	_wmakepath( lpFilename, szDrive, szDir, L"netsf", L".inf");
	
	return (DWORD )wcslen( lpFilename );
}

DWORD GetServiceInfFilePath_PassthruMP( 
									   LPTSTR lpFilename,
									   DWORD nSize
									   )
{
	// Get Path to This Module
	DWORD    nResult;
	TCHAR    szDrive[ _MAX_DRIVE ];
	TCHAR    szDir[ _MAX_DIR ];
	
	nResult = GetModuleFileName( NULL, lpFilename, nSize );	
	if( nResult == 0 )
	{
		return 0;
	}
	
    _wsplitpath( lpFilename, szDrive, szDir, NULL, NULL );
	
    _wmakepath( lpFilename, szDrive, szDir, L"netsf_m", L".inf");
	
	return (DWORD )wcslen( lpFilename );
}

DWORD InstallDriver_Passthru()
{
	DWORD    nResult;
		
	// Get Path to Service INF File
	// ----------------------------
	// The INF file is assumed to be in the same folder as this application...
	TCHAR    szFileFullPath[ _MAX_PATH ];
    
	
	//-----------------------------------------------------------------------
	//第一次在一个系统上使用这个程序安装Passthru时，会出现安装失败的情况，或者
	//安装成功，但passthru miniport的部分没有安装上去,在windows 目录下 的setupapi.log文件中可以看到
	//安装失败记录，错误是找不到文件。在“设备管理器”选择显示隐藏设备后
	//也不会在 网络适配器 下面看到 passthru miniport项。但手动安装在'本地网络属性"->"安装"->”服务“选择
	//硬盘上netsf.inf进行安装成功候，再用程序安装就可以了。
	//在网络上查了一下，这个问题应该是因为 Passthru这个驱动需要两个inf文件，而netsf_m.inf并不没有被复制到
	//系统的inf 目录（c:\windows\inf）去。虽然 netsf.inf 里面有[CopyInf   = netsf_m.inf]项要求复制netsf_m.inf
	//但这个不能 正常工作（The \"CopyINF\" directive, by design, is only observed if the original INF is 
	//not yet in the INF directory. So to work around the problem, you have to 
	//update your installation app (snetcfg) to also copy the Net class (miniport) 
	//inf using SetupCopyOEMInf when it comes to installing IM drivers. Make sure 
	//you specify a fully qualified path of the INF in the SetupCopyOEMInf 
	//arguments. 
	//）
	//
	//所以这个问题的解决就是自己把 netsf_m.inf这个文件放到c:\windows\inf目录去。这可以通过在 netsf.inf里面添加
	//copyfile项，像copy Passthru.sys一样添加一项copy   netsf_m.inf的项。另一种方法就是像下面这样添加调用
	//SetupCopyOEMInfW来复制netsf_m.inf的代码
	TCHAR      szDrive[ _MAX_DRIVE ];
	TCHAR      szDir[ _MAX_DIR ];
	TCHAR      szDirWithDrive[_MAX_DRIVE+_MAX_DIR];
	nResult =   GetServiceInfFilePath_PassthruMP( szFileFullPath, MAX_PATH );
	if( nResult == 0 )
	{
		//_tprintf( "Unable to get INF file path.\n") );
		//MessageBoxW(NULL, L"获取INF文件路径失败！", L"Passthru安装程序错误提示",MB_OK);
		return -1;
	}
	//
	// Get the path where the INF file is.
	//
	_wsplitpath( szFileFullPath, szDrive, szDir, NULL, NULL );
	
	wcscpy( szDirWithDrive, szDrive );
    wcscat( szDirWithDrive, szDir );
	if ( !SetupCopyOEMInfW(
		(PWSTR)szFileFullPath,
		(PWSTR)szDirWithDrive, // Other files are in the
		// same dir. as primary INF
		SPOST_PATH,     // First param is path to INF
		0,              // Default copy style
		NULL,           // Name of the INF after
		// it's copied to %windir%\inf
		0,              // Max buf. size for the above
		NULL,           // Required size if non-null
		NULL)           // Optionally get the filename
		// part of Inf name after it is copied.
		)
	{
		//MessageBoxW(NULL, L"复制 PasstruMP 的inf安装文件到系统目录失败！", L"Passthru安装程序错误提示",MB_OK);
		return -2;
	}
	//------------------------------------------------------------------------
	
	
	nResult = GetServiceInfFilePath_Passthru( szFileFullPath, MAX_PATH );
	
	if( nResult == 0 )
    {
		//     _tprintf( "Unable to get INF file path.\n" );
		//MessageBoxW(NULL, L"获取INF文件路径失败！", L"Passthru安装程序错误提示",MB_OK);
		return -3;
	}
	
	//_tprintf( "INF Path: %s\n", szFileFullPath );
	
	HRESULT hr=S_OK;
	
	//_tprintf( "PnpID: %s\n"),    "ms_passthru");
	
	WCHAR wszPassthru[] = L"ms_passthru" ;
	hr = InstallSpecifiedComponent(
		(LPWSTR)szFileFullPath,                    //驱动安装的inf文件路径 , 适当修改吧
		(LPWSTR)wszPassthru,               // NDISPROT_SERVICE_PNP_DEVICE_ID,    //这个也要适当修改的
		pguidNetClass[1]            //NDIS Protocal 类型
		);
	
	if( hr != S_OK )
	{
		/*ErrMsg( hr, L"InstallSpecifiedComponent\n" );*/
		//MessageBoxW(NULL, L"安装驱动失败！", L"Passthru安装程序错误提示",MB_OK);
		return -4;
	}else
	{
		//MessageBoxW(NULL, L"成功安装驱动！", L"Passthru安装程序提示",MB_OK);
		return 1;
	}
	
	return 0;
}




//
// Program entry point.
//

int __cdecl main()
{
    return InstallDriver_Passthru() ;
}
