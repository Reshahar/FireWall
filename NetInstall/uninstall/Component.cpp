//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 2001.
//
//  File:       C O M P O N E N T . C P P
//
//  Contents:   Functions to illustrate
//              o How to enumerate network components.
//              o How to install protocols, clients and services.
//              o How to uninstall protocols, clients and services.
//              o How to bind/unbind network components.
//
//  Notes:      
//
//  Author:     Alok Sinha    15-May-01
//
//----------------------------------------------------------------------------

#include "bindview.h"

//
// Function:  InstallSpecifiedComponent
//
// Purpose:   Install a network component from an INF file.
//
// Arguments:
//    lpszInfFile [in]  INF file.
//    lpszPnpID   [in]  PnpID of the network component to install.
//    pguidClass  [in]  Class GUID of the network component.
//
// Returns:   None.
//
// Notes:
//

HRESULT InstallSpecifiedComponent (LPWSTR lpszInfFile,
                                   LPWSTR lpszPnpID,
                                   const GUID *pguidClass)
{
    INetCfg    *pnc;
    LPWSTR     lpszApp;
    HRESULT    hr;

    hr = HrGetINetCfg( TRUE,
                       APP_NAME,
                       &pnc,
                       &lpszApp );

    if ( hr == S_OK ) {

        //
        // Install the network component.
        //

        hr = HrInstallNetComponent( pnc,
                                    lpszPnpID,
                                    pguidClass,
                                    lpszInfFile );
        if ( (hr == S_OK) || (hr == NETCFG_S_REBOOT) ) {

            hr = pnc->Apply();
        }
        else {
            if ( hr != HRESULT_FROM_WIN32(ERROR_CANCELLED) ) {
            }
        }

        HrReleaseINetCfg( pnc,
                          TRUE );
    }
    else {
        if ( (hr == NETCFG_E_NO_WRITE_LOCK) && lpszApp ) {

            CoTaskMemFree( lpszApp );
        }
        else {
        }
    }

    return hr;
}