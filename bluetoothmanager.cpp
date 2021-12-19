#include "bluetoothmanager.h"

union {
    CHAR buf[ 5000 ];
    SOCKADDR_BTH _Unused_;  // ensure proper alignment
} butuh;

BluetoothManager::BluetoothManager(){
    this->BTDeviceList = new QList<QBluetoothHostInfo>;
}

int BluetoothManager::ListBTService(BT_ADDR BluetoothAddress){
    int iResult = 0, iRet;
    HRESULT CoInitRet;
    BLOB blob;
    SOCKADDR_BTH sa;
    CSADDR_INFO csai;
    WSAQUERYSET wsaq1;
    HANDLE hLookup1;
    CHAR buf1[ 5000 ];
    DWORD dwSize;
    LPWSAQUERYSET pwsaResults1;
    BTHNS_RESTRICTIONBLOB RBlob;
    // Initializes the COM library for use by the calling thread
    CoInitRet = CoInitializeEx( 0, COINIT_MULTITHREADED );
    if ( CoInitRet != S_OK )
        printf( "Something wrong with CoInitializeEx()!\n" );
    // http://msdn.microsoft.com/en-us/library/ms881237.aspx
    // This structure contains details about a query restriction
    memset( &RBlob, 0, sizeof( RBlob ) );
    RBlob.type = SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST;
    RBlob.numRange = 1;
    RBlob.pRange[ 0 ].minAttribute = SDP_ATTRIB_PROTOCOL_DESCRIPTOR_LIST;
    RBlob.pRange[ 0 ].maxAttribute = SDP_ATTRIB_PROTOCOL_DESCRIPTOR_LIST;
    RBlob.uuids[ 0 ].uuidType = SDP_ST_UUID16;
    RBlob.uuids[ 0 ].u.uuid16 = SerialPortServiceClassID_UUID16;
    // BrowseGroupDescriptorServiceClassID_UUID;
    // This structure is used for an arbitrary array of bytes
    blob.cbSize = sizeof( RBlob );
    blob.pBlobData = ( BYTE* )&RBlob;
    // This structure defines the Bluetooth socket address
    memset( &sa, 0, sizeof( sa ) );
    sa.btAddr = BluetoothAddress;
    sa.addressFamily = AF_BTH;
    // Do some verification
    printf( "\n  sa.btAddr: %012X\n", sa.btAddr );
    printf( "  GET_NAP(sa.btAddr): %04X\n", GET_NAP( sa.btAddr ) );
    printf( "  GET_SAP(sa.btAddr): %08X\n", GET_SAP( sa.btAddr ) );
    printf( "  sa.addressFamily: %d\n", sa.addressFamily );
    printf( "  sa.port: %u\n", sa.port );
    memset( &csai, 0, sizeof( csai ) );
    csai.RemoteAddr.lpSockaddr = ( SOCKADDR* )&sa;
    csai.RemoteAddr.iSockaddrLength = sizeof( sa );
    // Do some verification
    printf( "\n  csai.RemoteAddr: %012X\n", csai.RemoteAddr.lpSockaddr );
    printf( "  csai.LocalAddr: %X\n", csai.LocalAddr.lpSockaddr );
    printf( "  csai.iSocketType: %d\n", csai.iSocketType );
    printf( "  csai.iProtocol: %d\n", csai.iProtocol );
    memset( &wsaq1, 0, sizeof( wsaq1 ) );
    wsaq1.dwSize = sizeof( wsaq1 );
    // NS_BTH - The Bluetooth namespace. However this 'bastard' namespace identifier
    // is supported on Windows Vista and later. Can also try NS_NLA
    // Using NS_BTH failed with 10022 though with #define _WIN32_WINNT 0x0600
    wsaq1.dwNameSpace = NS_ALL;
    wsaq1.lpBlob = &blob;
    wsaq1.lpcsaBuffer = &csai;
    // Do some verification
    printf( "\n  wsaq1.lpcsaBuffer->RemoteAddr: %X\n\n", wsaq1.lpcsaBuffer->RemoteAddr );
    iRet = WSALookupServiceBegin( &wsaq1, 0, &hLookup1 );
    if ( iRet == 0 )  // ERROR_SUCCESS
    {
        //printf( "BtServiceSearch(): WSALookupServiceBegin() should be OK!\n" );
        pwsaResults1 = ( LPWSAQUERYSET )buf1;
        dwSize = sizeof( buf1 );
        memset( pwsaResults1, 0, sizeof( WSAQUERYSET ) );
        pwsaResults1->dwSize = sizeof( WSAQUERYSET );
        // But this part using NS_BTH is OK! Retard lor! %@%#&^%(^@^@^$($&$%
        pwsaResults1->dwNameSpace = NS_BTH;
        pwsaResults1->lpBlob = NULL;
        iRet = WSALookupServiceNext( hLookup1, 0, &dwSize, pwsaResults1 );
        if ( iRet == 0 )  // ERROR_SUCCESS - got the stream
        {
            //printf( "BtServiceSearch(): WSALookupServiceNext() is OK!\n" );
            printf( "\n(The SDP result processing routine should be called...)\n\n" );
            // ...
            // Source codes for this routine are for Windows CE!
            // ...
        }
        else
            printf( "BtServiceSearch(): WSALookupServiceNext() failed with error code %ld\n", WSAGetLastError() );
        if ( WSALookupServiceEnd( hLookup1 ) == 0 )
            printf( "BtServiceSearch(): WSALookupServiceEnd(hLookup1) failed with error code %ld\n", WSAGetLastError() );
    }
    else
        printf( "BtServiceSearch(): WSALookupServiceBegin() failed with error code %ld\n", WSAGetLastError() );
    // Closes the COM library on the current thread, unloads all DLLs loaded by the thread,
    // frees any other resources that the thread maintains, and forces all RPC connections
    // on the thread to close.
    CoUninitialize();
    return iResult;
}
//------------------------------------------------------------------------
// Function: FindingBtDevices
// Purpose: Performs a device inquiry displays the device name in a console
//------------------------------------------------------------------------
int BluetoothManager::ScanBTDevice()
{
    WSADATA wsd;
    BOOL retVal;
    if ( WSAStartup( MAKEWORD( 2, 2 ), &wsd ) != 0 )
        printf( "WSAStartup() failed with error code %ld\n", WSAGetLastError() );

    WSAQUERYSET WSAQuerySet;
    HANDLE hLookup;
    LPWSAQUERYSET pwsaResults;
    DWORD dwSize;
    BOOL bHaveName;
    BT_ADDR btAddr;
    DWORD dwNameSpace;
    int nPssRet;
    pwsaResults = ( LPWSAQUERYSET )butuh.buf;
    dwSize = sizeof( butuh.buf );
    ZeroMemory( &WSAQuerySet, sizeof( WSAQuerySet ) );
    WSAQuerySet.dwSize = sizeof( WSAQuerySet );
    WSAQuerySet.dwNameSpace = NS_BTH;
    WSAQuerySet.lpcsaBuffer = NULL;
    printf( "\n" );
    if ( WSALookupServiceBegin( &WSAQuerySet, LUP_CONTAINERS, &hLookup ) == SOCKET_ERROR ){
        printf( "ScanBTDevice: WSALookupServiceBegin() failed %d\r\n", WSAGetLastError() );
        return false;
    }
    else
        printf( "ScanBTDevice: WSALookupServiceBegin() pretty damn OK!\n" );
    ZeroMemory( pwsaResults, sizeof( WSAQUERYSET ) );
    pwsaResults->dwSize = sizeof( WSAQUERYSET );
    pwsaResults->dwNameSpace = NS_BTH;
    pwsaResults->lpBlob = NULL;
    while ( WSALookupServiceNext( hLookup, LUP_RETURN_NAME | LUP_RETURN_ADDR, &dwSize, pwsaResults ) == 0 ){
        // to get the remote address, the btAddr field should be properly cast
        btAddr = ( ( SOCKADDR_BTH* )pwsaResults->lpcsaBuffer->RemoteAddr.lpSockaddr )->btAddr;
        // Verify
        wprintf( L"Device Address is 0X%012X\n", btAddr );
        // Verify the returned name
        bHaveName = ( pwsaResults->lpszServiceInstanceName ) && *( pwsaResults->lpszServiceInstanceName );
        // Some more info
        dwNameSpace = pwsaResults->dwNameSpace;
        // http://msdn.microsoft.com/en-us/library/cc510863.aspx
        // http://msdn.microsoft.com/en-us/library/ms892245.aspx
        // SAP = 32 bits (8-hex-string)+ NAP = 16 bits 94-hex-string)= 48 bits = bluetooth address
        wprintf( L"Name\tNAP Address\tSAP Address\tName Space\n" );
        wprintf( L"%s\t0X%04X\t\t0X%08X\t0X%0X\n", pwsaResults->lpszServiceInstanceName, GET_NAP( btAddr ), GET_SAP( btAddr ),
                 dwNameSpace );
        //nPssRet = ListBTService( btAddr );
        printf( "Service search return value: %d\n", nPssRet );
    }
    if ( WSALookupServiceEnd( hLookup ) != 0 )
        printf( "FindingBtDevices(): WSALookupServiceEnd(hLookup) failed with error code %ld\n", WSAGetLastError() );

    if ( ScanBTDevice() == FALSE )
        printf( "\nNo bulutooth device in range found!\n" );
    else
        printf( "\nFound some bluetooth devices around!\n" );
    if ( WSACleanup() != 0 )
        printf( "WSACleanup() failed with error code %ld\n", WSAGetLastError() );

    return 0;
}




