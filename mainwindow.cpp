#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "yielddata.h"
#include "crc.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mSerialPort = new QSerialPort;
    timer1 = new QTimer;
    timer1->setInterval(200);
    timer2 = new QTimer;
    timer2->setInterval(200);

    //PushButoon Slot
    QObject::connect(ui->pButton_Start, SIGNAL(clicked(bool)), this, SLOT(StartSlot()));
    QObject::connect(ui->pButton_Reset, SIGNAL(clicked(bool)), this, SLOT(ResetSlot()));
    QObject::connect(this->mSerialPort, SIGNAL(readyRead()), this, SLOT(HandlingReadDataSlot()));
    QObject::connect(ui->pButton_yield1, SIGNAL(clicked(bool)), ui->pButton_yield1, SLOT(setDisabled(bool)));
    QObject::connect(ui->pButton_yield2, SIGNAL(clicked(bool)), ui->pButton_yield2, SLOT(setDisabled(bool)));
    QObject::connect(ui->pButton_motion1, SIGNAL(clicked(bool)), ui->pButton_motion1, SLOT(setDisabled(bool)));
    QObject::connect(ui->pButton_motion2, SIGNAL(clicked(bool)), ui->pButton_motion2, SLOT(setDisabled(bool)));
    QObject::connect(this->timer1, SIGNAL(timeout()), this, SLOT(UploadDataSlot()));
    QObject::connect(this->timer2, SIGNAL(timeout()), this, SLOT(UploadMilkSlot()));

    qsrand(10);//产生随机数
    GenericPanel();//产生随机数

    //Init UI
    InitUI();
}

MainWindow::~MainWindow()
{
    delete mSerialPort;
    delete ui;
}

void MainWindow::StartSlot()
{
    QSerialPort::BaudRate mBaudRate;
    QSerialPort::Parity mParity;
    static bool portStatus = 0;

    if(!portStatus)
    {
        mSerialPort->setPortName(ui->comboBox_port->currentText());//port

        switch(ui->comboBox_baudrate->currentIndex())//baudRate
        {
            case 0: mBaudRate = QSerialPort::Baud115200; break;
            case 1: mBaudRate = QSerialPort::Baud57600; break;
            case 2: mBaudRate = QSerialPort::Baud38400; break;
            case 3: mBaudRate = QSerialPort::Baud19200; break;
            case 4: mBaudRate = QSerialPort::Baud9600; break;
            default: mBaudRate = QSerialPort::Baud57600; break;
        }
        mSerialPort->setBaudRate(mBaudRate);

        switch(ui->comboBox_Parity->currentIndex())//baudRate
        {
            case 0: mParity = QSerialPort::NoParity; break;
            case 1: mParity = QSerialPort::EvenParity; break;
            case 2: mParity = QSerialPort::OddParity; break;
            default: mParity = QSerialPort::NoParity; break;
        }
        mSerialPort->setParity(mParity);

        mSerialPort->setDataBits(QSerialPort::Data8); //other
        mSerialPort->setStopBits(QSerialPort::OneStop);
        mSerialPort->setFlowControl(QSerialPort::NoFlowControl);

        portStatus = mSerialPort->open(QIODevice::ReadWrite);
        if(portStatus){
            ui->label_PortStatus->setText("串口已开启");
            ui->pButton_Start->setText("close");
        }else{
            ui->label_PortStatus->setText("打开失败");
        }
    }else{
        mSerialPort->close();
        portStatus = 0;
        ui->label_PortStatus->setText("串口已关闭");
        ui->pButton_Start->setText("start");
    }
}

void MainWindow::ResetSlot()
{
    GenericPanel();
    FillTable(ui->panelTable);
    FillTable(ui->panelTable_2);
}

void MainWindow::InitUI()
{
    //Init combobox baudrate & parity
    ui->comboBox_baudrate->setCurrentText("57600");//57600
    ui->comboBox_Parity->setCurrentText("None");//

    //Init Serial Port
    QList<QSerialPortInfo> portList;
    QSerialPortInfo *mSerialPortInfo = new QSerialPortInfo;

    portList = mSerialPortInfo->availablePorts();
    for(int i=0; i < portList.count(); i++)
    {
        ui->comboBox_port->addItem(portList.at(i).portName());
    }
    delete mSerialPortInfo;
    FillTable(ui->panelTable);
    FillTable(ui->panelTable_2);
}

void MainWindow::UploadDataSlot()
{
    QByteArray buf;
    static uchar pos = 0;
    ushort cd,mv;
    ushort crcCheck;

    yd[pos].GetData(cd,mv);
    buf= QByteArray::fromHex("01F1BB010A1100008F0000000000000000000000000000000000000000000000000020000C000000042C518D");
    buf[3] = pass;
    buf[4] = pos++;
    buf[7] =(uchar)(cd >> 8);
    buf[8] =(uchar)cd;
    buf[39] =(uchar)(mv >> 8);
    buf[40] =(uchar)mv;
    crcCheck = generic_crc(42,(uchar*)buf.data());
    buf[42] = (uchar)(crcCheck >> 8);
    buf[43] = (uchar)crcCheck;

    mSerialPort->write(buf);
    if(pos >= 32){
        pos = 0;
        timer1->stop();
        if(pass == 1){
            ui->pButton_motion1->setEnabled(true);
        }else if(pass == 2){
            ui->pButton_motion2->setEnabled(true);
        }
    }
}

void MainWindow::UploadMilkSlot()
{
    QByteArray buf;
    static uchar pos = 0;
    ushort mv,cv;
    ushort crcCheck;
    bool bc;

    yd[pos].GetMilk(mv,cv,bc);
    buf= QByteArray::fromHex("01F2BB010A1100840B518D");
    buf[3] = pass;
    buf[4] = pos++;
    buf[5] =(uchar)(mv >> 8);
    buf[6] =(uchar)mv;
    buf[7] =(uchar)cv;
    crcCheck = generic_crc(42,(uchar*)buf.data());
    buf[9] = (uchar)(crcCheck >> 8);
    buf[10] = (uchar)crcCheck;

    mSerialPort->write(buf);
    if(pos >= 32){
        pos = 0;
        timer2->stop();
        if(pass == 1){
            ui->pButton_yield1->setEnabled(true);
        }else if(pass == 2){
            ui->pButton_yield2->setEnabled(true);
        }
    }
}

void MainWindow::HandlingReadDataSlot()
{
    QByteArray buffer;
    ushort crcCheck;

    if(mSerialPort->bytesAvailable() >= 7){
        crcCheck = generic_crc(5,(uchar*)buffer.data());
        buffer = mSerialPort->readAll();
        if((uchar)(crcCheck >> 8) == buffer[6] && (uchar)crcCheck == buffer[5]) //校验通过...
        {
            if((uchar)buffer[0] == 0xF1){
                if((uchar)buffer[3] == 0x01 && !ui->pButton_motion1->isEnabled()){
                    pass = buffer[3];
                    timer1->start();
                }else if((uchar)buffer[3] == 0x02 && !ui->pButton_motion2->isEnabled()){
                    pass = buffer[3];
                    timer1->start();
                }
            }else if((uchar)buffer[1] == 0xF2){
                if((uchar)buffer[3] == 0x01 && !ui->pButton_yield1->isEnabled()){
                    pass = buffer[3];
                    timer2->start();
                }else if((uchar)buffer[3] == 0x02 && !ui->pButton_yield2->isEnabled()){
                    pass = buffer[3];
                    timer2->start();
                }
            }
        }
    }

}

void MainWindow::GenericPanel()
{
    for(int i = 0; i < 32*2; i++)
    {
        yd[i].GenericData();
    }
}

void MainWindow::FillTable(QTableWidget *tb)
{
    QTableWidgetItem *item;
    QByteArray array[4];
    ushort mv, cv, cd, mo;
    QString str1,str2,str3,str4;
    bool bc;

    for(int i = 0; i < 32; i++) //通道一数据
    {
        yd[i].GetMilk(mv,cv,bc);
        yd[i].GetData(cd,mo);

        str1 = array[0].setNum(cd,16);
        str2 = array[1].setNum(mo,16);
        str3 = array[2].setNum(mv,16);
        str4 = array[3].setNum(cv,16);

        item = new QTableWidgetItem();
        int len = str1.length();
        for(int j = 0; j < 4-len; j++){
            str1.insert(0,"0");
        }
        str1.insert(0,"30301100");//计步器号
        item->setText(str1.toUpper());
        item->setTextAlignment(Qt::AlignCenter);
        tb->setItem(i,0,item);

        item = new QTableWidgetItem();
        len = str2.length();
        for(int j = 0; j < 4-len; j++){
            str2.insert(0,"0");
        }//活动量
        item->setText(str2.toUpper());
        item->setTextAlignment(Qt::AlignCenter);
        tb->setItem(i,1,item);

        item = new QTableWidgetItem();
        len = str3.length();
        for(int j = 0; j < 4-len; j++){
            str3.insert(0,"0");
        }
        item->setText(str3.toUpper());
        item->setTextAlignment(Qt::AlignCenter);
        tb->setItem(i,2,item);//奶量

        item = new QTableWidgetItem();
        len = str4.length();
        for(int j = 0; j < 4-len; j++){
            str4.insert(0,"0");
        }//电导率
        item->setText(str4.toUpper());
        item->setTextAlignment(Qt::AlignCenter);
        tb->setItem(i,3,item);

        item = new QTableWidgetItem();
        item->setCheckState(Qt::Unchecked);
        tb->setItem(i,4,item);
    }
}
