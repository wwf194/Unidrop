#ifndef BluetoothManager_H
#define BluetoothManager_H

#include <QObject>
#include <QBluetoothHostInfo>
#include <QList>
#include <QUuid>

// Link to ws2_32.lib
#include <winsock2.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServiceInfo>
#include <QBluetoothDeviceInfo>
#include <QBluetoothServer>
#include <stdio.h>
// Minimum Vista
// #define _WIN32_WINNT 0x0600
#define MAX_NAME 248
#define MYBUFFSIZE 16384

typedef ULONGLONG bt_addr, *pbt_addr, BT_ADDR, *PBT_ADDR;

struct BTServiceInfo{
    QString Name;
    QString Description;
    QUuid ID; //ServiceUUID
    QUuid ClassID; //ServiceClassID
};

struct BTDeviceInfo{
    QBluetoothAddress Address;
    QString Name;
    QList<BTServiceInfo> ServiceList;
};


class BluetoothManager: public QObject{
    Q_OBJECT
public:
    //void BluetoothManager(); //构造函数不能具有返回值
    BluetoothManager();
    //void ~BluetoothManager(); //析构函数不能具有返回值
    ~BluetoothManager();
    void StartService();
    int ScanBTDevice();
    int _ScanBTDevice();
    int ListBTService(BT_ADDR BluetoothAddress, BTDeviceInfo& btDeviceInfo);
    //int ListBTService2(BT_ADDR BluetoothAddress, BTDeviceInfo& btDeviceInfo);
signals:
    void ScanFinished(QList<BTDeviceInfo>* btDeviceList);
    void ShowDebug(QString Str);
private slots:
    void OnDiscoveredBTService();
    void OnDiscoveredBTDevice();

private:
    QList<BTDeviceInfo>* btDeviceList;
    QList<QBluetoothServiceInfo>* btServiceList;
    QBluetoothServiceDiscoveryAgent * btServiceAgent;
    QBluetoothDeviceDiscoveryAgent  * btDeviceAgent;
    QBluetoothServer * btServer;
};

#endif // BluetoothManager_H
