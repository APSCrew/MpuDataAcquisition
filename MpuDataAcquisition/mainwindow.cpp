#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serial(new QSerialPort(this))
    , serialInfo(new QSerialPortInfo())
    , isAcquisitionStarted(false)
{
    ui->setupUi(this);

    // inizializza l'interfaccia con i dati della porta seriale
//    foreach(QSerialPort::)
//    ui->comboBox_serialPortBaudRate->setItemData(0,)
    // scrivi messaggio "non connesso" sulla toolbar

}

MainWindow::~MainWindow()
{
    delete ui;
    delete serial;
    delete serialInfo;
}


void MainWindow::on_pushButton_connect_clicked()
{
    if (serial->isOpen()){
        // inizializza la porta seriale
        serial->setPortName(ui->comboBox_serialPortName->currentData().toString());
        serial->setBaudRate(ui->comboBox_serialPortBaudRate->currentData().toInt());
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        // apri la connessione seriale
        if (serial->open(QIODevice::ReadWrite)) {
            // scrivi messaggio "connesso" sulla toolbar
            // cambia stato al pulsante di connessione

        }
        else{
            QMessageBox::critical(this, tr("Error"), serial->errorString());

//            showStatusMessage(tr("Open error"));
        }
    }
    else{
        isAcquisitionStarted = false;
        // ferma l'acquisizione
        // cancella messaggio "in acquisizione" sulla toolbar
        // cambia stato al pulsante di start

        // chiudi la connessione seriale
         serial->close();

        // scrivi messaggio "non connesso" sulla toolbar
        // cambia stato al pulsante di connessione
    }
}

void MainWindow::on_pushButton_startAcquisition_clicked()
{

    if(!serial->isOpen()){
        QMessageBox::critical(this, tr("Error"), "Avviare la connessione seriale prima");
    }
    else{
        if(!isAcquisitionStarted){
            isAcquisitionStarted = true;
            // avvia l'aquisizione
            // scrivi messaggio "in acquisizione" sulla toolbar
            // cambia stato al pulsante di start
        }
        else{
            isAcquisitionStarted = false;
            // ferma l'aquisizione
            // cancella messaggio "in acquisizione" sulla toolbar
            // cambia stato al pulsante di start
        }
    }
}
