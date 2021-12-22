#include "bluetoothmanager.h"
#include "QUtils.h"
union {
    CHAR buf[ 5000 ];
    SOCKADDR_BTH _Unused_;  // ensure proper alignment
} butuh;

BluetoothManager::BluetoothManager(){
    this->btDeviceList = new QList<BTDeviceInfo>;

    this->btDeviceAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(btDeviceAgent, SIGNAL(finished()), this, SLOT(OnDiscoveredBTDevice()));

    this->btServiceAgent = new QBluetoothServiceDiscoveryAgent(this);
    connect(btServiceAgent, SIGNAL(finished()), this, SLOT(OnDiscoveredBTService()));

    this->btServer = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol);

}

BluetoothManager::~BluetoothManager(){
    this->btDeviceList->clear();
    delete this->btDeviceList;
}

//int BluetoothManager::ListBTService2(BT_ADDR BluetoothAddress, BTDeviceInfo& btDeviceInfo){
//    WSADATA          m_data;
//    SOCKET           Socket;
//    WSAPROTOCOL_INFO protocolInfo;
//    int              protocolInfoSize;
//    WSAQUERYSET      querySet, *pResults, QueryParamDevice;
//    HANDLE           hLookup1, hLookup2;
//    int              result;
//    static int       i;
//    BYTE             buffer1[ 1000 ];
//    BYTE             buffer2[ 2000 ];
//    DWORD            flags, AddressSize;
//    CSADDR_INFO*     pCSAddr;
//    BTH_DEVICE_INFO* pDeviceInfo;
//    // wchar_t          AddressStr[ 2000 ];
//    char  AddressStr[ 2000 ];
//    BLOB* pBlob;
//    GUID  protocol;
//    pCSAddr     = ( CSADDR_INFO* )pResults->lpcsaBuffer;
//    pDeviceInfo = ( BTH_DEVICE_INFO* )pResults->lpBlob;
//    memset( &QueryParamDevice, 0, sizeof( QueryParamDevice ) );
//    QueryParamDevice.dwSize           = sizeof( QueryParamDevice );
//    protocol                          = L2CAP_PROTOCOL_UUID;
//    QueryParamDevice.lpServiceClassId = &protocol;
//    QueryParamDevice.dwNameSpace      = NS_BTH;
//    AddressSize                = sizeof( AddressStr );
//    // Print the local bluetooth device address...
//    if ( WSAAddressToString( pCSAddr->LocalAddr.lpSockaddr, pCSAddr->LocalAddr.iSockaddrLength, &protocolInfo, (LPSTR)AddressStr, &AddressSize ) == 0 ) {
//        //printf( "WSAAddressToString() for local address is fine!\n" );
//        printf("LocalAddress: %s\n", AddressStr ); //hS for LPSTR
//    }
//    else
//        printf( "LocalAddress: ERRORCODE%ld\n", WSAGetLastError() );
//    AddressSize = sizeof( AddressStr );
//    // Prepare for service query set
//    // QueryParamDevice.lpszContext = ( LPWSTR )AddressStr;
//    // QueryParamDevice.lpszContext = char2wchar( AddressStr );

//    flags                 = LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_TYPE | LUP_RETURN_ADDR | LUP_RETURN_BLOB | LUP_RETURN_COMMENT;
//    // Start service query

//    QueryParamDevice.lpszContext = AddressStr;  // Query supported services of device with given address AddressStr
//    result = WSALookupServiceBegin( &QueryParamDevice, flags, &hLookup2 );
//    if ( result != 0 ){
//        printf( "WSALookupServiceBegin() failed with error code %ld\n", WSAGetLastError() );
//        break;
//    }
//    //else printf( "WSALookupServiceBegin() is OK!\n" );
//    while (true) {
//        DWORD bufferLength2 = sizeof( buffer2 );
//        pResults      = ( WSAQUERYSET* )&buffer2;
//        // Next service query
//        result = WSALookupServiceNext( hLookup2, flags, &bufferLength2, pResults );
//        if ( result != 0 ) {
//            // printf( "WSALookupServiceNext() failed with error code %ld\n", WSAGetLastError() );
//            // printf( "Error code = 11011 ~ WSA_E_NO_MORE ~ No more device!\n" );
//            break;
//        }
//        // Populate the service info
//        //printf( "WSALookupServiceNext() is OK!\n" );
//        printf( "    ServiceInstance2: %s\n", pResults->lpszServiceInstanceName );
//        printf( "    Comment2: %s\n", pResults->lpszComment );
//        pCSAddr = ( CSADDR_INFO* )pResults->lpcsaBuffer;
//        // Extract the sdp info
//        if ( pResults->lpBlob ) {
//            pBlob = ( BLOB* )pResults->lpBlob;
//            if ( !BluetoothSdpEnumAttributes( pBlob->pBlobData, pBlob->cbSize, callback, 0 ) ) {
//                printf( "    BluetoothSdpEnumAttributes() failed with error code %ld\n", WSAGetLastError() );
//            }
//        }
//    }
//    // Close the handle to service query
//    if ( WSALookupServiceEnd( hLookup2 ) != 0 )
//        printf( "WSALookupServiceEnd(hLookup2) failed with error code %ld\n", WSAGetLastError() );
//}

int BluetoothManager::ListBTService(BT_ADDR BluetoothAddress, BTDeviceInfo& btDeviceInfo){
    int iResult = 0, iRet;
    HRESULT CoInitRet;
    BLOB blob;
    SOCKADDR_BTH SocketAddress;
    CSADDR_INFO SocketAddressInfo;
    WSAQUERYSET WSAQueryParam;
    BTHNS_RESTRICTIONBLOB RBlob;
    // Initializes the COM library for use by the calling thread
    //CoInitRet = CoInitializeEx( 0, COINIT_MULTITHREADED );
    if ( CoInitRet != S_OK )
        printf( "Something wrong with CoInitializeEx()!\n" );
    // http://msdn.microsoft.com/en-us/library/ms881237.aspx
    // This structure contains details about a query restriction
    memset( &RBlob, 0, sizeof( RBlob ) );

//    RBlob.type = SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST;
//    RBlob.numRange = 1;
//    RBlob.pRange[ 0 ].minAttribute = SDP_ATTRIB_PROTOCOL_DESCRIPTOR_LIST;
//    RBlob.pRange[ 0 ].maxAttribute = SDP_ATTRIB_PROTOCOL_DESCRIPTOR_LIST;
//    RBlob.uuids[ 0 ].uuidType = SDP_ST_UUID16;
//    RBlob.uuids[ 0 ].u.uuid16 = SerialPortServiceClassID_UUID16;

    // BrowseGroupDescriptorServiceClassID_UUID;
    // This structure is used for an arbitrary array of bytes
    blob.cbSize = sizeof( RBlob );
    blob.pBlobData = ( BYTE* )&RBlob;
    // This structure defines the Bluetooth socket address
    memset( &SocketAddress, 0, sizeof( SocketAddress ) );
    SocketAddress.btAddr = BluetoothAddress;
    SocketAddress.addressFamily = AF_BTH;
    // Do some verification
//    printf( "\n  SocketAddress.btAddr: %012X\n", SocketAddress.btAddr );
//    printf( "  GET_NAP(SocketAddress.btAddr): %04X\n", GET_NAP( SocketAddress.btAddr ) );
//    printf( "  GET_SAP(SocketAddress.btAddr): %08X\n", GET_SAP( SocketAddress.btAddr ) );
//    printf( "  SocketAddress.addressFamily: %d\n", SocketAddress.addressFamily );
//    printf( "  SocketAddress.port: %u\n", SocketAddress.port );
    memset( &SocketAddressInfo, 0, sizeof( SocketAddressInfo ) );
    SocketAddressInfo.RemoteAddr.lpSockaddr = ( SOCKADDR* )&SocketAddress;
    SocketAddressInfo.RemoteAddr.iSockaddrLength = sizeof( SocketAddress );
    // Do some verification
//    printf( "\n  SocketAddressInfo.RemoteAddr: %012X\n", SocketAddressInfo.RemoteAddr.lpSockaddr );
//    printf( "  SocketAddressInfo.LocalAddr: %X\n", SocketAddressInfo.LocalAddr.lpSockaddr );
//    printf( "  SocketAddressInfo.iSocketType: %d\n", SocketAddressInfo.iSocketType );
//    printf( "  SocketAddressInfo.iProtocol: %d\n", SocketAddressInfo.iProtocol );
    memset( &WSAQueryParam, 0, sizeof( WSAQueryParam ) );
    WSAQueryParam.dwSize = sizeof( WSAQueryParam );
    // NS_BTH - The Bluetooth namespace. However this 'bastard' namespace identifier
    // is supported on Windows Vista and later. Can also try NS_NLA
    // Using NS_BTH failed with 10022 though with #define _WIN32_WINNT 0x0600
    //wsaq1.dwNameSpace = NS_BTH;
    WSAQueryParam.dwNameSpace = NS_ALL;
    WSAQueryParam.lpBlob = &blob;
    WSAQueryParam.lpcsaBuffer = &SocketAddressInfo;
    // Do some verification
    //printf( "\n  wsaq1.lpcsaBuffer->RemoteAddr: %X\n\n", wsaq1.lpcsaBuffer->RemoteAddr );
    HANDLE HandleForLookupService;
    //DWORD flags = LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_TYPE | LUP_RETURN_ADDR | LUP_RETURN_BLOB | LUP_RETURN_COMMENT;
    //DWORD flags = LUP_RETURN_NAME | LUP_RETURN_TYPE | LUP_RETURN_COMMENT;
    DWORD flags = LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_COMMENT;

    int result = WSALookupServiceBegin( &WSAQueryParam, flags, &HandleForLookupService );
    if(result != 0){
        qDebug() << "BtServiceSearch(): WSALookupServiceBegin() failed with error code" << WSAGetLastError();
    }

    LPWSAQUERYSET WSABTServiceInfo;
    CHAR BufferForBTServiceInfo[ 5000 ];
    WSABTServiceInfo = ( LPWSAQUERYSET )BufferForBTServiceInfo;
    DWORD BufferForBTServiceInfoSize = sizeof( BufferForBTServiceInfo );
    memset(WSABTServiceInfo, 0, sizeof( WSAQUERYSET ) );
    WSABTServiceInfo->dwSize = sizeof( WSAQUERYSET );
    // But this part using NS_BTH is OK! Retard lor! %@%#&^%(^@^@^$($&$%
    WSABTServiceInfo->dwNameSpace = NS_BTH;
    WSABTServiceInfo->lpBlob = NULL;

    while(true){
        result = WSALookupServiceNext(HandleForLookupService, flags, &BufferForBTServiceInfoSize, WSABTServiceInfo);
        if(result != 0){
            qDebug() << "WSALookupServiceNext() failed with error code :" << WSAGetLastError();
            break;
        }
        //printf( "BtServiceSearch(): WSALookupServiceNext() is OK!\n" );
        //printf( "\n(The SDP result processing routine should be called...)\n\n" );

        // Populate the service info
        //printf( "WSALookupServiceNext() is OK!\n" );
//        printf( "    ServiceInstance2: %s\n", BTServiceInfo->lpszServiceInstanceName );
//        printf( "    Comment2: %s\n", BTServiceInfo->lpszComment );
//        CSADDR_INFO* pCSAddr = ( CSADDR_INFO* )BTServiceInfo->lpcsaBuffer;
//        // Extract the sdp info
//        if ( BTServiceInfo->lpBlob ) {
//            pBlob = ( BLOB* )pResults->lpBlob;
//            if ( !BluetoothSdpEnumAttributes( pBlob->pBlobData, pBlob->cbSize, callback, 0 ) ) {
//                printf( "    BluetoothSdpEnumAttributes() failed with error code %ld\n", WSAGetLastError() );
//            }
//        }
        struct BTServiceInfo btServiceInfo;
        btServiceInfo.Name = QString::fromStdWString(WSABTServiceInfo->lpszServiceInstanceName);
        if(WSABTServiceInfo->lpszComment!=NULL){
            btServiceInfo.Description = QString::fromStdWString(WSABTServiceInfo->lpszComment);
        }

        btDeviceInfo.ServiceList.append(btServiceInfo);
    }

    if ( WSALookupServiceEnd( HandleForLookupService )!= 0){
        qDebug() << "BtServiceSearch(): WSALookupServiceEnd(hLookup1) failed with error code " << WSAGetLastError();
    }
    // Closes the COM library on the current thread, unloads all DLLs loaded by the thread,
    // frees any other resources that the thread maintains, and forces all RPC connections
    // on the thread to close.
    //CoUninitialize();
    return btDeviceInfo.ServiceList.count();
}

int ScanBTDeviceAndService();
int BluetoothManager::ScanBTDevice(){
    ScanBTDeviceAndService();
    //this->btDeviceAgent->start(QBluetoothDeviceDiscoveryAgent::ClassicMethod);
    this->btDeviceAgent->start();
    this->btServiceAgent->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);
    return 0;
}

int BluetoothManager::_ScanBTDevice(){
    WSADATA wsd;
    if ( WSAStartup( MAKEWORD( 2, 2 ), &wsd ) != 0 )
        printf( "WSAStartup() failed with error code %d\n", WSAGetLastError() );

    WSAQUERYSET WSAQuerySet;
    HANDLE hLookup;
    LPWSAQUERYSET pwsaResults;
    DWORD dwSize;
    BOOL HasName;
    BT_ADDR BTAddress;
    DWORD dwNameSpace;
    int nPssRet;
    pwsaResults = ( LPWSAQUERYSET )butuh.buf;
    dwSize = sizeof( butuh.buf );
    ZeroMemory( &WSAQuerySet, sizeof( WSAQuerySet ) );
    WSAQuerySet.dwSize = sizeof( WSAQuerySet );
    WSAQuerySet.dwNameSpace = NS_BTH;
    WSAQuerySet.lpcsaBuffer = NULL;

    //LUP_CONTAINERS: Scan BT Device, otherwise scan BT Service.
    DWORD flags = LUP_RETURN_ADDR | LUP_RETURN_NAME | LUP_RES_SERVICE | LUP_CONTAINERS | LUP_RETURN_BLOB | LUP_RETURN_TYPE | LUP_RETURN_COMMENT;
    //DWORD flags = LUP_RETURN_NAME | LUP_CONTAINERS | LUP_RETURN_ADDR | LUP_FLUSHCACHE | LUP_RETURN_TYPE | LUP_RETURN_BLOB | LUP_RES_SERVICE;

    if ( WSALookupServiceBegin( &WSAQuerySet, flags, &hLookup ) == SOCKET_ERROR ){
        qDebug() << "ScanBTDevice: WSALookupServiceBegin() failed %d\r\n" << WSAGetLastError();
        return false;
    }

    ZeroMemory( pwsaResults, sizeof( WSAQUERYSET ) );
    pwsaResults->dwSize = sizeof( WSAQUERYSET );
    pwsaResults->dwNameSpace = NS_BTH;
    pwsaResults->lpBlob = NULL;
    btDeviceList->clear();

    while ( WSALookupServiceNext(hLookup, flags, &dwSize, pwsaResults ) == 0 ){
        //LUP_FLUSHCACHE avoid simply returning previously cached results.
        // to get the remote address, the btAddr field should be properly cast
        BTAddress = ( ( SOCKADDR_BTH* )pwsaResults->lpcsaBuffer->RemoteAddr.lpSockaddr )->btAddr;
        // Verify
        //wprintf( L"Device Address is 0X%012X\n", BTAddress );
        // Verify the returned name
        HasName = ( pwsaResults->lpszServiceInstanceName ) && *( pwsaResults->lpszServiceInstanceName );
        // Some more info
        dwNameSpace = pwsaResults->dwNameSpace;
        // http://msdn.microsoft.com/en-us/library/cc510863.aspx
        // http://msdn.microsoft.com/en-us/library/ms892245.aspx
        // SAP = 32 bits (8-hex-string)+ NAP = 16 bits 94-hex-string)= 48 bits = bluetooth address
//        wprintf( L"Name\tNAP Address\tSAP Address\tName Space\n" );
//        wprintf( L"%s\t0X%04X\t\t0X%08X\t0X%0X\n", pwsaResults->lpszServiceInstanceName, GET_NAP( BTAddress ), GET_SAP( BTAddress ),
//                 dwNameSpace );

        QString BTName = QString::fromWCharArray(pwsaResults->lpszServiceInstanceName);
        BTDeviceInfo btDeviceInfo;

        btDeviceInfo.Address = QBluetoothAddress(BTAddress);

        if(HasName){
            btDeviceInfo.Name = BTName;
        }else{
            btDeviceInfo.Name = "Anonymous Device";
        }
        int ServiceNum = ListBTService(BTAddress, btDeviceInfo);
        btDeviceList->append(btDeviceInfo);

        //printf( "Service search return value: %d\n", nPssRet );
    }
    if ( WSALookupServiceEnd( hLookup ) != 0 )
        printf( "FindingBtDevices(): WSALookupServiceEnd(hLookup) failed with error code %d\n", WSAGetLastError() );

    qDebug() << QString("Found %1 BTDevice.\n").arg(btDeviceList->count());
    if ( WSACleanup() != 0 )
        printf( "WSACleanup() failed with error code %d\n", WSAGetLastError() );

    emit ScanFinished(this->btDeviceList);
    return 0;
}

typedef ULONGLONG BT_ADDR;

static BOOL PerformInquiry();
// int main1() {
//     // WSAQUERYSET wsaq;
//     // ZeroMemory(&wsaq, sizeof(wsaq));
//     // wsaq.dwSize = sizeof(wsaq);
//     // wsaq.dwNameSpace = NS_BTH;
//     // wsaq.lpcsaBuffer = NULL;

//     // HANDLE hLookup;
//     // int iRet = WSALookupServiceBegin(&wsaq, LUP_CONTAINERS, &hLookup);
//     PerformInquiry();
//     return 0;
// }

bool CheckIsValidBluetoothService(int Socket) {
    if ( Socket == INVALID_SOCKET ) {
        printf( "Socket creation failed. Error code: %d\n", WSAGetLastError() );
        return false;
    }
    else {
        printf( "Socket:%d successfully created!\n", Socket );
        return true;
    }
}

int CreateBlueToothSocket() {
    SOCKET BluetoothSocket;
    BluetoothSocket = socket( AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM );
    // af: bluetooth connection, AF_BTH
    // type: for bluetooth, connection type should always be SOCK_STREAM
    // protocol: for bluetooth, BTHPROTO_RFCOMM
    if ( !CheckIsValidBluetoothService( BluetoothSocket ) ) {
        return -1;
    }
    return BluetoothSocket;
}

//------------------------------------------------------------------------
// Function: PerformInquiry
// Purpose: Performs a device inquiry displays the device name in a user
// interface.
//------------------------------------------------------------------------
#define MAX_NAME 248
#define DEFAULT_BUFLEN 512
static BOOL PerformInquiry() {
    WSADATA wsd;
    WSAStartup( MAKEWORD( 1, 0 ), &wsd );

    WSAQUERYSET QueryParam;
    ZeroMemory( &QueryParam, sizeof( QueryParam ) );
    QueryParam.dwSize      = sizeof( QueryParam );
    QueryParam.dwNameSpace = NS_BTH;  // Restrict to bluetooth devices
    QueryParam.lpcsaBuffer = NULL;

    HANDLE hLookup;
    // dwFlats: LUP_CONTAINERS for device search based on SDP(Service Discovery
    // Protocol); 0 for service search.
    if ( ERROR_SUCCESS != WSALookupServiceBegin( &QueryParam, LUP_CONTAINERS, &hLookup ) ) {
        printf( "WSALookupServiceBegin failed %d\r\n", GetLastError() );
        return FALSE;
    }

    // union {
    //     CHAR buf[5000];
    //     double __unused; // ensure proper alignment
    // };
    CHAR          buf[ 4096 ];
    LPWSAQUERYSET QueryResults = ( LPWSAQUERYSET )buf;
    DWORD         BufSize      = sizeof( buf );
    ZeroMemory( QueryResults, sizeof( WSAQUERYSET ) );
    QueryResults->dwSize      = sizeof( WSAQUERYSET );
    QueryResults->dwNameSpace = NS_BTH;
    QueryResults->lpBlob      = NULL;

    BOOL HaveName;
    while ( ERROR_SUCCESS == WSALookupServiceNext( hLookup, LUP_RETURN_NAME | LUP_RETURN_ADDR, &BufSize, QueryResults ) ) {
        //assert( QueryResults->dwNumberOfCsAddrs == 1 );
        BT_ADDR BluetoothID = ( ( SOCKADDR_BTH* )QueryResults->lpcsaBuffer->RemoteAddr.lpSockaddr )->btAddr;
        HaveName            = QueryResults->lpszServiceInstanceName && *( QueryResults->lpszServiceInstanceName );
        if ( HaveName ) {
            printf( "%s(%04x%08x)\n", QueryResults->lpszServiceInstanceName, GET_NAP( BluetoothID ), GET_SAP( BluetoothID ) );
        }
        else {
            printf( "AnonymousBluetoothService(%04x%08x)\n", GET_NAP( BluetoothID ), GET_SAP( BluetoothID ) );
        }
    }
    WSALookupServiceEnd( hLookup );

    SOCKET       BluetoothSocket = CreateBlueToothSocket();
    SOCKADDR_BTH BluetoothSocketInfo;
    memset( &BluetoothSocketInfo, 0, sizeof( BluetoothSocketInfo ) );
    BluetoothSocketInfo.addressFamily  = AF_BTH;
    BluetoothSocketInfo.btAddr         = 0;
    GUID GUID_NULL                     = { 00000000 - 0000 - 0000 - 0000 - 000000000000 };
    BluetoothSocketInfo.serviceClassId = GUID_NULL;
    BluetoothSocketInfo.port           = 0;  // number of service channel, 0 or BT_PORT_ANY;
    // 0 if it is a client, >=1 if it is a server.

    if ( 0 != bind( BluetoothSocket, ( SOCKADDR* )&BluetoothSocketInfo, sizeof( BluetoothSocketInfo ) ) ) {
        printf( "bind() failed with error code %d\n", WSAGetLastError() );
        closesocket( BluetoothSocket );
        return false;
    }

    if ( listen( BluetoothSocket, 5 ) == 0 )
        printf( "listen() is OK! Listening for connection...\n" );
    else
        printf( "listen() failed with error code %d\n", WSAGetLastError() );

    SOCKADDR_BTH BluetoothSocketInfo2;
    SOCKET       BluetoothSocket2;
    for ( ;; ) {
        // Get information on the port assigned
        int BluetoothSocketInfo2Size = sizeof( BluetoothSocketInfo2 );
        BluetoothSocket2             = accept( BluetoothSocket, ( SOCKADDR* )&BluetoothSocketInfo2, &BluetoothSocketInfo2Size );
        if ( BluetoothSocket2 == INVALID_SOCKET ) {
            printf( "accept() failed with error code %d\n", WSAGetLastError() );
            break;
        }
        else {
            printf( "accept(), is OK buddy!\n" );
        }
        // Print the info
        printf( "Connection came from %04x%08x to channel %d\n", GET_NAP( BluetoothSocketInfo2.btAddr ), GET_SAP( BluetoothSocketInfo2.btAddr ), BluetoothSocketInfo2.port );
    }

    // Receive until the peer closes the connection
    // This should be const void * type for non-char data
    //char* SendBuf        = "Test data from receiver...";
    int   ReceiveBufSize = DEFAULT_BUFLEN;
    // Change the type accordingly for non-char data
    char ReceiveBuf[ DEFAULT_BUFLEN ] = "";

    int Result;
    do {
        Result = recv( BluetoothSocket2, ReceiveBuf, ReceiveBufSize, 0 );
        if ( Result > 0 )
            printf( " %d Bytes received from sender\n", Result );
        else if ( Result == 0 )
            printf( "Connection closed by peer!\n" );
        else
            printf( "recv() failed with error code %d\n", WSAGetLastError() );
    } while ( Result > 0 );

    // Echo back the data
    Result = send( BluetoothSocket2, ReceiveBuf, ReceiveBufSize, 0 );
    if ( Result == SOCKET_ERROR ) {
        printf( "send() failed with error code %d\n", WSAGetLastError() );
        closesocket( BluetoothSocket2 );
        WSACleanup();
        return 1;
    }
    else {
        printf( "send() is OK!\n" );
        printf( "Bytes Sent: %d\n", Result );
    }
    if ( closesocket( BluetoothSocket ) == 0 )
        printf( "closesocket() pretty fine!\n" );
    if ( WSACleanup() == 0 )
        printf( "WSACleanup() is OK!\n" );
    return TRUE;
}

// int Connect(SOCKET m_client_socket){
//     int nNo = 0;
//     //先关闭连接
//     closesocket(m_client_socket);
//     WSACleanup();

//     WSADATA wsd;
//     int err = WSAStartup(MAKEWORD(2, 2), &wsd);
//     if (err != 0)
//     {
//         printf("WinSock初始化失败");
//         return FALSE;
//     }
//     m_client_socket = socket(AF_BTH, SOCK_STREAM , BTHPROTO_RFCOMM);
//     if (m_client_socket == INVALID_SOCKET)
//     {
//         printf("创建Socket失败");
//         return FALSE;
//     }

//     SOCKADDR_BTH sa;
//     memset(&sa, 0, sizeof(sa));
//     sa.addressFamily = AF_BTH;
//     sa.btAddr = m_vBluetoothInfo[nNo].btAddr;
//     sa.port = 0;
//     sa.serviceClassId = RFCOMM_PROTOCOL_UUID;

//     //调用connect函数来连接到蓝牙套接字。
//     err = connect(m_client_socket, (SOCKADDR *)&sa, sizeof(sa));
//     if (err == INVALID_SOCKET)
//     {
//         printf("连接蓝牙套接字失败。");
//         //关闭Socket
//         closesocket(m_client_socket);
//         WSACleanup();
//         return FALSE;
//     }
//     return TRUE;
// }
// Link to ws2_32.lib

// callback for BluetoothSdpEnumAttributes()
BOOL __stdcall callback( ULONG uAttribId, LPBYTE pValueStream, ULONG cbStreamSize, LPVOID pvParam ) {
    SDP_ELEMENT_DATA element;
    // Just a verification, uncomment to see the output!!!
    // printf("Callback() uAttribId: %ul\n", uAttribId);
    // printf("Callback() pValueStream: %d\n ", pValueStream);
    // printf("Callback() cbStreamSize: %ul\n ", cbStreamSize);
    if ( BluetoothSdpGetElementData( pValueStream, cbStreamSize, &element ) != ERROR_SUCCESS ) {
        // Just a verification
        // printf("BluetoothSdpGetElementData() failed with error code %ld\n",
        // WSAGetLastError());
        return FALSE;
    }
    else {
        // Just a verification
        // printf("BluetoothSdpGetElementData() is OK!\n");
        return TRUE;
    }
}

// // int main( int argc, char** argv ) {
int ScanBTDeviceAndService() {
    WSADATA          m_data;
    SOCKET           Socket;
    WSAPROTOCOL_INFO protocolInfo;
    int              protocolInfoSize;
    WSAQUERYSET      querySet, *pResults, QueryParamDevice;
    HANDLE           hLookup1, hLookup2;
    int              result;
    static int       i;
    BYTE             buffer1[ 1000 ];
    BYTE             buffer2[ 2000 ];
    DWORD            flags, AddressSize;
    CSADDR_INFO*     pCSAddr;
    BTH_DEVICE_INFO* pDeviceInfo;
    // wchar_t          AddressStr[ 2000 ];
    char  AddressStr[ 2000 ];
    BLOB* pBlob;
    GUID  protocol;
    //printf("aaa");
    //Load the winsock2 library
    if ( WSAStartup( MAKEWORD( 2, 2 ), &m_data ) != 0 ){
        printf( "WSAStartup() failed with error code %d\n", WSAGetLastError() );
        return false;
    }
    // else
    //     printf( "WSAStartup() should be fine!\n" );

    // Create a blutooth socket
    Socket = socket( AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM );
    if ( Socket == INVALID_SOCKET ) {
        printf( "Failed to get bluetooth socket with error code %ld\n", WSAGetLastError() );
        return false;
    }
    // else
    //     printf( "socket() is OK!\n" );

    protocolInfoSize = sizeof( protocolInfo );
    // Get the bluetooth device info using getsockopt()
    if ( getsockopt( Socket, SOL_SOCKET, SO_PROTOCOL_INFO, ( char* )&protocolInfo, &protocolInfoSize ) != 0 ) {
        printf( "getsockopt(SO_PROTOCOL_INFO) failed with error code %ld\n", WSAGetLastError() );
        return false;
    }
    // else
    //     printf( "getsockopt(SO_PROTOCOL_INFO) is OK!\n" );

    // Query set criteria
    memset( &querySet, 0, sizeof( querySet ) );
    querySet.dwSize      = sizeof( querySet );
    querySet.dwNameSpace = NS_BTH;
    flags                = LUP_RETURN_NAME | LUP_CONTAINERS | LUP_RETURN_ADDR | LUP_FLUSHCACHE | LUP_RETURN_TYPE | LUP_RETURN_BLOB | LUP_RES_SERVICE;
    // Start a device in range query...
    result = WSALookupServiceBegin( &querySet, flags, &hLookup1 );
    if(result != 0){
        printf( "WSALookupServiceBegin() failed with error code %d\n", WSAGetLastError() );
        return false;
    }
    // Set the flags for query

    i = 0;
    while (true) {
        DWORD bufferLength1 = sizeof( buffer1 );
        pResults     = ( WSAQUERYSET* )&buffer1;
        // Next query...
        result = WSALookupServiceNext( hLookup1, flags, &bufferLength1, pResults );
        if (result != 0) {
            printf( "WSALookupServiceNext() failed with error code %d\n", WSAGetLastError() );
            break;
        }
        qDebug() << "ServiceInstance: " << pResults->lpszServiceInstanceName;

        pCSAddr     = ( CSADDR_INFO* )pResults->lpcsaBuffer;
        pDeviceInfo = ( BTH_DEVICE_INFO* )pResults->lpBlob;
        memset( &QueryParamDevice, 0, sizeof( QueryParamDevice ) );
        QueryParamDevice.dwSize           = sizeof( QueryParamDevice );
        protocol                          = L2CAP_PROTOCOL_UUID;
        QueryParamDevice.lpServiceClassId = &protocol;
        QueryParamDevice.dwNameSpace      = NS_BTH;
        AddressSize                = sizeof( AddressStr );
        // Print the local bluetooth device address...
        if ( WSAAddressToString( pCSAddr->LocalAddr.lpSockaddr, pCSAddr->LocalAddr.iSockaddrLength, &protocolInfo, (LPWSTR)AddressStr, &AddressSize ) == 0 ) {
            //printf( "WSAAddressToString() for local address is fine!\n" );
            printf("LocalAddress: %s\n", AddressStr ); //hS for LPSTR
        }
        else
            printf( "LocalAddress: ERRORCODE%d\n", WSAGetLastError() );
        AddressSize = sizeof( AddressStr );
        // Prepare for service query set
        // QueryParamDevice.lpszContext = ( LPWSTR )AddressStr;
        // QueryParamDevice.lpszContext = char2wchar( AddressStr );

        flags                 = LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_TYPE | LUP_RETURN_ADDR | LUP_RETURN_BLOB | LUP_RETURN_COMMENT;
        // Start service query

        QueryParamDevice.lpszContext = QUtils::char2wchar(AddressStr);  // Query supported services of device with given address AddressStr
        result = WSALookupServiceBegin( &QueryParamDevice, flags, &hLookup2 );
        if ( result != 0 ){
            printf( "WSALookupServiceBegin() failed with error code %ld\n", WSAGetLastError() );
            break;
        }
        //else printf( "WSALookupServiceBegin() is OK!\n" );
        while (true) {
            DWORD bufferLength2 = sizeof( buffer2 );
            pResults      = ( WSAQUERYSET* )&buffer2;
            // Next service query
            result = WSALookupServiceNext( hLookup2, flags, &bufferLength2, pResults );
            if ( result != 0 ) {
                printf( "WSALookupServiceNext() failed with error code %d\n", WSAGetLastError() );
                printf( "Error code = 11011 ~ WSA_E_NO_MORE ~ No more device!\n" );
                break;
            }
            // Populate the service info
            //printf( "WSALookupServiceNext() is OK!\n" );
            qDebug() << "    ServiceInstance2: %s\n" << pResults->lpszServiceInstanceName;
            qDebug() << "    Comment2: %s\n" << pResults->lpszComment;
            pCSAddr = ( CSADDR_INFO* )pResults->lpcsaBuffer;
            // Extract the sdp info
            if ( pResults->lpBlob ) {
                pBlob = ( BLOB* )pResults->lpBlob;
                if ( !BluetoothSdpEnumAttributes( pBlob->pBlobData, pBlob->cbSize, callback, 0 ) ) {
                    qDebug() << "BluetoothSdpEnumAttributes() failed with error code" << WSAGetLastError();
                }
            }
        }
        // Close the handle to service query
        if ( WSALookupServiceEnd( hLookup2 ) != 0 )
            qDebug() << "WSALookupServiceEnd(hLookup2) failed with error code %d\n", WSAGetLastError();
    }
    // Close handle to the device query
    if ( WSALookupServiceEnd( hLookup1 ) != 0 )
        printf( "WSALookupServiceEnd(hLookup) failed with error code %d\n", WSAGetLastError() );

    // Cleanup the winsock library startup
    if ( WSACleanup() != 0)
        printf( "WSACleanup() failed with error code %d\n", WSAGetLastError() );
    return 0;
}

void BluetoothManager::OnDiscoveredBTService(){
    const QList<QBluetoothServiceInfo>& btServiceListQt = btServiceAgent->discoveredServices();
    int btServiceNum = btServiceListQt.count();
    qDebug() << QString("QBluetoothServiceDiscoveryAgent found %1 services.").arg(btServiceNum);

    QString Str;
    for(int i=0;i<btServiceNum;i++){
        QBluetoothServiceInfo btServiceQt = btServiceListQt.at(i);
        Str.append("Found service:").append(btServiceQt.serviceName()).append('-').append(btServiceQt.device().address().toString());
        Str.append("\n");
    }
    this->ShowDebug(Str);
    qDebug() << Str;
}

void BluetoothManager::OnDiscoveredBTDevice(){
    const QList<QBluetoothDeviceInfo>& btDeviceListQt = this->btDeviceAgent->discoveredDevices();
    int btDeviceNum = btDeviceListQt.count();
    qDebug() << QString("QBluetoothDeviceDiscoveryAgent found %1 devices.").arg(btDeviceNum);
    this->btDeviceList->clear();
    for(int i=0; i<btDeviceNum; i++){
        BTDeviceInfo btDeviceInfo;
        QBluetoothDeviceInfo btDeviceInfoQt = btDeviceListQt.at(i);
        btDeviceInfo.Address = btDeviceInfoQt.address();
        btDeviceInfo.Name = btDeviceInfoQt.name();
        this->btDeviceList->append(btDeviceInfo);
        if(btDeviceInfoQt.coreConfigurations()==QBluetoothDeviceInfo::BaseRateCoreConfiguration){ //Avoid BLE devices.

        }
    }
    emit ScanFinished(this->btDeviceList);
}

void BluetoothManager::StartService(){

}
