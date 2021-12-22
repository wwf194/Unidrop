#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QUtils.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this); //必须在其他ui界面设置之前执行

    //QUtils::SetBackgroundColor(this, Qt::GlobalColor::white);

    QStringList BTDeviceTableHeaders;

    BTDeviceTableHeaders.append("DeviceName");
    BTDeviceTableHeaders.append("DeviceAddress");
    BTDeviceTableHeaders.append("ServiceNum");
    BTDeviceTableHeaders.append("ServiceName");

    BTDeviceTable = new QStandardItemModel();
    BTDeviceTable->setHorizontalHeaderLabels(BTDeviceTableHeaders);
    BTDeviceTable->setColumnCount(BTDeviceTableHeaders.count());
    ui->DeviceList->setModel(this->BTDeviceTable);
    connect(ui->ScanBTDevice, SIGNAL(clicked()), this, SLOT(OnScanButtonClicked()));
    ui->DeviceList->setEditTriggers(QAbstractItemView::NoEditTriggers); //Make uneditable
    ui->DeviceList->setColumnWidth(1, 200);

}

void MainWindow::ShowDebug(QString Str){
    qDebug() << "Showing debug" << Str;
    ui->DebugInfo->setText(Str);
    ui->DebugInfo->repaint();
}


void MainWindow::Init(){
    connect(BTManager, SIGNAL(ScanFinished(QList<BTDeviceInfo>*)), this, SLOT(ShowBTDevice(QList<BTDeviceInfo>*)));
    connect(BTManager, SIGNAL(ShowDebug(QString)), this, SLOT(ShowDebug(QString)));
    BTManager->ScanBTDevice();
}

void MainWindow::BindBluetoothManager(BluetoothManager *BTManager){
    this->BTManager = BTManager;
}

void MainWindow::ShowBTDevice(QList<BTDeviceInfo>* BTDeviceList){
    while(ui->DeviceList->model()->rowCount() > 0){
        ui->DeviceList->model()->removeRow(0);
    }
    //ui->DeviceList->model()->removeRows(0, ui->DeviceList->model()->rowCount());
    qDebug() << "Showing BT Device";
    QStringList BTDeviceStrList;
    int BTDeviceNum = BTDeviceList->count();

    BTDeviceTable->setRowCount(BTDeviceList->count());
    //BTDeviceTable->setColumnCount(3);

    int rowIndex = 0;
    for(int i = 0; i < BTDeviceNum; i++)
    {
        int ServiceNum = BTDeviceList->at(i).ServiceList.count();
        qDebug() << "ServiceNum:" << ServiceNum;

        BTDeviceTable->setItem(rowIndex, 0, new QStandardItem(BTDeviceList->at(i).Name));
        BTDeviceTable->setItem(rowIndex, 1, new QStandardItem(BTDeviceList->at(i).Address.toString()));
        BTDeviceTable->setItem(rowIndex, 2, new QStandardItem(QString::number(ServiceNum)));

        for(int j=0; j<ServiceNum; j++){
            BTDeviceTable->setItem(rowIndex, 3, new QStandardItem(BTDeviceList->at(i).ServiceList.at(j).Name));
            rowIndex += 1;
        }

        if(ServiceNum==0){
            rowIndex += 1;
        }
        //QString string = static_cast<QString>(BTDeviceStrList.at(i));
        //QStandardItem *item = new QStandardItem(BTDeviceStrList.at(i));
        //BTDeviceTable->appendRow(item);
    }
    //ui->DeviceList->setModel(BTDeviceTable);
    ui->DeviceList->repaint();
}

MainWindow::~MainWindow()
{
    delete this->BTDeviceTable;
    delete ui;
}

void MainWindow::OnScanButtonClicked()
{
    ShowDebug("Clicked ScanBTDevice Button.");
    this->BTManager->ScanBTDevice();
}
