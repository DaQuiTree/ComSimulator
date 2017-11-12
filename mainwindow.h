#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QList>
#include <QIODevice>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QTimer>
#include <QtGlobal>
#include <QTableWidget>
#include <QByteArray>
#include <QSignalMapper>

#include "yielddata.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void StartSlot();
    void ResetSlot();
    void HandlingReadDataSlot();
    void UploadDataSlot();
    void UploadMilkSlot();
    void SetDisableSlot(int a);

private:
    Ui::MainWindow *ui;
    QSerialPort *mSerialPort;
    YieldData yd[32*2];
    QTimer *timer1, *timer2;
    uchar pass;

    void InitUI();
    void GenericPanel();
    void FillTable(int tbID);
    void ChangeClearSta(int pos);

};

#endif // MAINWINDOW_H
