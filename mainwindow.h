#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "bluetoothmanager.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void Init();
    void BindBluetoothManager(BluetoothManager* BTManager);

    ~MainWindow();

private slots:
    void OnScanButtonClicked();
    void ShowBTDevice(QList<BTDeviceInfo>* BTDeviceList);
    void ShowDebug(QString Str);
private:
    QStandardItemModel* BTDeviceTable;
    BluetoothManager* BTManager;
    Ui::MainWindow *ui;
};



#endif // MAINWINDOW_H
