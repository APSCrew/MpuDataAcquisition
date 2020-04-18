#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>

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

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QSerialPortInfo *serialInfo;
    bool isAcquisitionStarted;
};
#endif // MAINWINDOW_H
