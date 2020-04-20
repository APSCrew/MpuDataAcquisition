#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QDebug>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_connect_clicked();

    void on_pushButton_startAcquisition_clicked();

    void on_pushButton_refresh_clicked();

    void readData();

private:
    Ui::MainWindow *ui = nullptr;
    QSerialPort *serial = nullptr;
    bool isAcquisitionStarted;
    QLabel *serial_status_label = nullptr;
    QLabel *acquisition_status_label = nullptr;
    QCustomPlot *raw_data_plot = nullptr;
    QCustomPlot *euler_data_plot = nullptr;
    static constexpr uint8_t START_ACQUISITION = 0x01;
    static constexpr uint8_t STOP_ACQUISITION  = 0x00;

    void initializeGUI(void);
    void sendCommand(uint8_t c);
    void initializePlot(void);
    void checkSerialPort(void);
};
#endif // MAINWINDOW_H
