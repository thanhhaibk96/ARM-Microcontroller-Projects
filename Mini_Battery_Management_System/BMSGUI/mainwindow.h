#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
  QTimer *timer;

private slots:
    void on_button_Connect_clicked();
    void on_button_Disconnect_clicked();
    void on_receive_respone();
    void UCOM_Convert_Float_to_Bytes(float _data_in, char _data_out[4]);
    float UCOM_Convert_Bytes_to_Float(char _data_in[4]);
    char UCOM_Checksum(QByteArray _data);
    void on_button_init_clicked();
    void on_button_pass_clicked();
private:
    Ui::MainWindow *ui;
    QByteArray Tx_buff;
    QByteArray Rx_buff;
    int hour,min,sec;
    float total,capacity,used,percent,current,voltage;
    char btotal[4];
    char bcapacity[4];
    char bused[4];
    char bpercent[4];
    char bcurrent[4];
    char bvoltage[4];

};

#endif // MAINWINDOW_H
