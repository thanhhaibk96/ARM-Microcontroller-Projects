#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort>
#include "string.h"
#include <stdlib.h>
#include <stdio.h>



#include <Qdebug>

QSerialPort * serialPort;
QTimer *comtimer;


void MainWindow::UCOM_Convert_Float_to_Bytes(float _data_in, char _data_out[4])
{
    union
    {
        float _value;
        char _byte[4];
    }_part;

    _part._value = _data_in;

    _data_out[0] = _part._byte[0];
    _data_out[1] = _part._byte[1];
    _data_out[2] = _part._byte[2];
    _data_out[3] = _part._byte[3];
}

float MainWindow::UCOM_Convert_Bytes_to_Float(char _data_in[4])
{
    union
    {
        float _value;
        char _byte[4];
    }_part;

   _part._byte[0] =  _data_in[0];
   _part._byte[1] =  _data_in[1];
   _part._byte[2] =  _data_in[2];
   _part._byte[3] =  _data_in[3];

    return _part._value;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("BMS");
    comtimer = new QTimer(this);
    connect(comtimer,SIGNAL(timeout()),this,SLOT(on_receive_respone()));

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_Connect_clicked()
{
    serialPort= new QSerialPort(this);
    serialPort->setPortName(ui->comboBoxName->currentText());
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->open(QIODevice::ReadWrite);
    qDebug()<<serialPort->open(QIODevice::ReadWrite)<<endl;
    ui->label_Status->setText("CONNECTED");
    ui->label_Status->setStyleSheet("QLabel{font-size: 14pt;font-weight:600; font-style:italic;color: rgb(255, 255, 255);color: #00FF00;}");
    ui->label_Status->setAlignment(Qt::AlignCenter);
    comtimer->start(100);
}

void MainWindow::on_button_Disconnect_clicked()
{
    serialPort->close();
    ui->label_Status->setText("DISCONNECTED");
    ui->label_Status->setStyleSheet("QLabel{font-size: 14pt;font-weight:600; font-style:italic;color: rgb(255, 255, 255);color: #000000;}");
    ui->label_Status->setAlignment(Qt::AlignCenter);
}

void MainWindow::on_receive_respone()
{


while(serialPort->bytesAvailable() >= 38)
{
    Rx_buff = serialPort->readAll();
    qDebug()<<Rx_buff<<endl;
    if(!Rx_buff.isEmpty())
    {
        if((Rx_buff[0] == '$') && (Rx_buff[1] == 'A') && (Rx_buff[2] == 'U') && (Rx_buff[3] == 'V')
                && (Rx_buff[4] == '2') && (Rx_buff[5] == '0') && (Rx_buff[6] == '0') && (Rx_buff[7] == '0')&& (Rx_buff[36] == '\r')&& (Rx_buff[37] == '\n'))
            {
            char cs=  UCOM_Checksum(Rx_buff);
             //qDebug()<<"Byte check sum:"<< cs << Rx_buff[35]<<endl;
            if( cs== Rx_buff[35] )
            {
                for(int u= 0 ;u < 4; u++)
                {
                    btotal[u]=Rx_buff[u+11];
                }
                for(int u= 0 ;u < 4;u++)
                {
                     bcapacity[u]=Rx_buff[u+15];
                }

                for(int u= 0 ;u<4;u++)
                {
                    bused[u]=Rx_buff[u+19];
                }

                for(int u= 0 ;u<4;u++)
                {
                     bpercent[u]=Rx_buff[u+23];
                }

                for(int u= 0 ;u<4;u++)
                {
                     bcurrent[u]=Rx_buff[u+27];
                }
                for(int u= 0 ;u<4;u++)
                {
                    bvoltage[u]=Rx_buff[u+31];
                }

                hour = Rx_buff[8];
                min = Rx_buff[9];
                sec = Rx_buff[10];
                total = UCOM_Convert_Bytes_to_Float(btotal);
                capacity = UCOM_Convert_Bytes_to_Float(bcapacity);
                used = UCOM_Convert_Bytes_to_Float(bused);
                percent = UCOM_Convert_Bytes_to_Float(bpercent);
                current = UCOM_Convert_Bytes_to_Float(bcurrent);
                voltage = UCOM_Convert_Bytes_to_Float(bvoltage);

                QString time;
                time.append(QString::number(round(hour)));
                time.append("h");
                time.append(QString::number(round(min)));
                time.append("m");
                time.append(QString::number(round(sec)));
                time.append("s");

                ui->lineEdit_time->setText(time);
                ui->lineEdit_total->setText(QString::number(round(total)));
                ui->lineEdit_capaticy->setText(QString::number(round(capacity)));
                ui->lineEdit_used->setText(QString::number(round(used)));
                ui->lineEdit_percent->setText(QString::number(round(percent)));
                ui->lineEdit_current->setText(QString::number(round(current)));
                ui->lineEdit_voltage->setText(QString::number(round(voltage)));

            }
            }
        Rx_buff.clear();
    }
}

}

char MainWindow::UCOM_Checksum(QByteArray _data)
{
    char value = 0;
    //Calculate CheckSum (Byte)
    for (int i = 0; i < 38-3; i++)
    {
        value += _data[i];
    }
    value = ~value;
    value++;
    return value;
}

void MainWindow::on_button_init_clicked()
{
    Tx_buff="$AUV2000";//0 1 2 3 4 5 6 7
    Tx_buff.append("INIT");// 8 9 10 11
    char BatteryTotalInit[4];
    UCOM_Convert_Float_to_Bytes(ui->lineEdit_batterytotalinit->text().toFloat(),BatteryTotalInit);
    Tx_buff.append(BatteryTotalInit[0]);//12
    Tx_buff.append(BatteryTotalInit[1]);//13
    Tx_buff.append(BatteryTotalInit[2]);//14
    Tx_buff.append(BatteryTotalInit[3]);//15
    Tx_buff.append("0000000000000000000");//16-34
    Tx_buff.append(UCOM_Checksum(Tx_buff));//35
    Tx_buff.append("\r");//36
    Tx_buff.append("\n");//37
    qDebug()<<Tx_buff<<endl;
    serialPort->write(Tx_buff);
    Tx_buff.clear();
}

void MainWindow::on_button_pass_clicked()
{
    Tx_buff="$AUV2000";// 0 1 2 3 4 5 6 7
    Tx_buff.append("PASS");// 8 9 10 11
    Tx_buff.append("00000000000000000000000");//12-34
    Tx_buff.append(UCOM_Checksum(Tx_buff));//35
    Tx_buff.append("\r");//36
    Tx_buff.append("\n");//37
     qDebug()<<Tx_buff<<endl;
    serialPort->write(Tx_buff);
    Tx_buff.clear();
}
