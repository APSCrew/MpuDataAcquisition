#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serial(new QSerialPort(this))
    , isAcquisitionStarted(false)
    , serial_status_label(new QLabel)
    , acquisition_status_label(new QLabel)
{
    ui->setupUi(this);

    // inizializza l'interfaccia con i dati della porta seriale
    ui->comboBox_serialPortName->setDuplicatesEnabled(false);

    ui->comboBox_serialPortBaudRate->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->comboBox_serialPortBaudRate->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->comboBox_serialPortBaudRate->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->comboBox_serialPortBaudRate->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);

    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    if(serialPortInfos.count() == 0){
        ui->comboBox_serialPortName->addItem("no serial port detected");
    }
    else{
        for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
            ui->comboBox_serialPortName->addItem(serialPortInfo.portName());
        }
    }

    // inizializza qcustomplot
//    ((QCustomPlot *)ui->tabWidget->currentWidget())

    // scrivi messaggio "non connesso" sulla toolbar
    ui->statusbar->addWidget(serial_status_label);
    ui->statusbar->addWidget(acquisition_status_label);
    serial_status_label->setText("Seriale non Connessa");

}

MainWindow::~MainWindow()
{
    delete ui;
    delete serial;
    delete serial_status_label;
    delete acquisition_status_label;
}


void MainWindow::on_pushButton_connect_clicked()
{
    if (!serial->isOpen()){
        // inizializza la porta seriale
//        qDebug() << ui->comboBox_serialPortName->currentText();
//        qDebug() << ui->comboBox_serialPortBaudRate->currentData().toInt();


        serial->setPortName(ui->comboBox_serialPortName->currentText());
        serial->setBaudRate(ui->comboBox_serialPortBaudRate->currentData().toInt());
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneAndHalfStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        // apri la connessione seriale
        if (serial->open(QIODevice::ReadWrite)) {
            // scrivi messaggio "connesso" sulla toolbar
            serial_status_label->setText("Seriale Connessa");
            // cambia stato al pulsante di connessione
            ui->pushButton_connect->setText("Disconnect");

        }
        else{
            QMessageBox::critical(this, tr("Error"), serial->errorString());
        }
    }
    else{
        if(isAcquisitionStarted){
            isAcquisitionStarted = false;
            // ferma l'acquisizione   --> invia STOP_ACQUISITION
            // cancella messaggio "in acquisizione" sulla toolbar
            acquisition_status_label->setText("");
            // cambia stato al pulsante di start
            ui->pushButton_startAcquisition->setText("Start");
        }

        // chiudi la connessione seriale
         serial->close();

        // scrivi messaggio "non connesso" sulla toolbar
        serial_status_label->setText("Seriale non Connessa");
        // cambia stato al pulsante di connessione
        ui->pushButton_connect->setText("Connect");
    }
}

void MainWindow::on_pushButton_startAcquisition_clicked()
{

    if(!serial->isOpen()){
        QMessageBox::critical(this, tr("Error"), "Avviare la connessione seriale prima di iniziare l'acquisizione");
    }
    else{
        if(!isAcquisitionStarted){
            isAcquisitionStarted = true;
            // avvia l'aquisizione --> invia START_ACQUISITION
            //serial->write(....);
            // scrivi messaggio "in acquisizione" sulla toolbar
            acquisition_status_label->setText("In Acquisizione");
            // cambia stato al pulsante di start
            ui->pushButton_startAcquisition->setText("Stop");
        }
        else{
            isAcquisitionStarted = false;
            // ferma l'aquisizione  --> invia STOP_ACQUISITION
            //serial->write(....);
            // cancella messaggio "in acquisizione" sulla toolbar
            acquisition_status_label->setText("");
            // cambia stato al pulsante di start
            ui->pushButton_startAcquisition->setText("Start");

        }
    }
}

void MainWindow::on_pushButton_refresh_clicked()
{
    // da rivedere, può essere fatto meglio....
    ui->comboBox_serialPortName->clear();

    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    if(serialPortInfos.count() == 0){
        ui->comboBox_serialPortName->addItem("no serial port detected");
    }
    else{
        for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
            ui->comboBox_serialPortName->addItem(serialPortInfo.portName());
        }
    }
}

void MainWindow::readData()
{
    const QByteArray data = serial->readAll();
    //m_console->putData(data);

}
