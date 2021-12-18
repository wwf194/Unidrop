#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils_qt.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this); //必须在其他ui界面设置之前执行

    QUtils::SetBackgroundColor(this, Qt::GlobalColor::white);

    ItemModel = new QStandardItemModel(this);

    QStringList strList;
    strList.append("A");
    strList.append("B");
    strList.append("C");
    strList.append("D");
    strList.append("E");
    strList.append("F");
    strList.append("G");

    int nCount = strList.size();
    for(int i = 0; i < nCount; i++)
    {
        QString string = static_cast<QString>(strList.at(i));
        QStandardItem *item = new QStandardItem(string);
        ItemModel->appendRow(item);
    }
    ui->DeviceList->setModel(ItemModel);
    //ui->listView->setFixedSize(200,300);

    connect(ui->ScanBTDevice, SIGNAL(clicked()), this, SLOT(OnScanButtonClicked()));

}

void MainWindow::ShowDebug(QString Str){
    qDebug() << "Showing debug" << Str;
    ui->DebugInfo->setText(Str);
    ui->DebugInfo->repaint();
}


void MainWindow::Init(){
    connect(BTManager, SIGNAL(ScanFinished(QList<QBluetoothHostInfo>*)), this, SLOT(ShowBTDevice(QList<QBluetoothHostInfo>*)));
    connect(BTManager, SIGNAL(ShowDebug(QString)), this, SLOT(ShowDebug(QString)));
    BTManager->ScanBTDevice();
}

void MainWindow::BindBluetoothManager(BluetoothManager *BTManager){
    this->BTManager = BTManager;
}

void MainWindow::ShowBTDevice(QList<QBluetoothHostInfo>* BTDeviceList){
    ui->DeviceList->model()->removeRows(0, ui->DeviceList->model()->rowCount());
    qDebug() << "Showing BT Device";
    QStringList BTDeviceStrList;
    int BTDeviceNum = BTDeviceList->count();
    for(int i = 0; i < BTDeviceNum; i++){
        const QBluetoothHostInfo& BTDeviceInfo = BTDeviceList->at(i);
        QString BTDeviceStr = BTDeviceInfo.name() + ":" + BTDeviceInfo.address().toString();
        qDebug() << "Showing Device" << BTDeviceStr;
        BTDeviceStrList.append(BTDeviceStr);
    }

    BTDeviceStrList.append("AAA");

    for(int i = 0; i < BTDeviceStrList.size(); i++)
    {
        //QString string = static_cast<QString>(BTDeviceStrList.at(i));
        QStandardItem *item = new QStandardItem(BTDeviceStrList.at(i));
        ItemModel->appendRow(item);
    }

    ui->DeviceList->setModel(ItemModel);
    ui->DeviceList->repaint();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnScanButtonClicked()
{
    ShowDebug("Clicked ScanBTDevice Button.");
    this->BTManager->ScanBTDevice();
}
