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
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");

    QList<QCustomPlot *> allPlot = ui->tabWidget->findChildren<QCustomPlot *>();
    raw_data_plot = allPlot.at(1);
    // create graph
    raw_data_plot->addGraph();
    raw_data_plot->graph(0)->setPen(QPen(Qt::blue));       // blue line       > ax
    raw_data_plot->addGraph();
    raw_data_plot->graph(1)->setPen(QPen(Qt::red));        // red line        > ay
    raw_data_plot->addGraph();
    raw_data_plot->graph(2)->setPen(QPen(Qt::green));      // green line      > az
    raw_data_plot->addGraph();
    raw_data_plot->graph(3)->setPen(QPen(Qt::black));      // black line      > gx
    raw_data_plot->addGraph();
    raw_data_plot->graph(4)->setPen(QPen(Qt::cyan));       // cyan line       > gy
    raw_data_plot->addGraph();
    raw_data_plot->graph(5)->setPen(QPen(Qt::magenta));    // magenta line    > gz
    raw_data_plot->addGraph();
    raw_data_plot->graph(6)->setPen(QPen(Qt::darkCyan));   // darkCyan line   > mx
    raw_data_plot->addGraph();
    raw_data_plot->graph(7)->setPen(QPen(Qt::darkRed));    // darkRed line    > my
    raw_data_plot->addGraph();
    raw_data_plot->graph(8)->setPen(QPen(Qt::darkYellow)); // darkYellow line > mz


    raw_data_plot->xAxis->setTicker(timeTicker);
    raw_data_plot->axisRect()->setupFullAxesBox();

    // give the axes some labels:
    raw_data_plot->xAxis->setLabel("time [s]");
    raw_data_plot->yAxis->setLabel("data");
    // make left and bottom axes transfer their ranges to right and top axes:
    connect(raw_data_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), raw_data_plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(raw_data_plot->yAxis, SIGNAL(rangeChanged(QCPRange)), raw_data_plot->yAxis2, SLOT(setRange(QCPRange)));

    euler_data_plot = allPlot.at(0);
    // create graph
    euler_data_plot->addGraph();
    euler_data_plot->graph(0)->setPen(QPen(Qt::blue));       // blue line  > roll
    euler_data_plot->addGraph();
    euler_data_plot->graph(1)->setPen(QPen(Qt::red));        // red line   > pitch
    euler_data_plot->addGraph();
    euler_data_plot->graph(2)->setPen(QPen(Qt::green));      // green line > yaw

    euler_data_plot->xAxis->setTicker(timeTicker);
    euler_data_plot->axisRect()->setupFullAxesBox();
    // give the axes some labels:
    euler_data_plot->xAxis->setLabel("time [s]");
    euler_data_plot->yAxis->setLabel("data");
    // make left and bottom axes transfer their ranges to right and top axes:
    connect(euler_data_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), euler_data_plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(euler_data_plot->yAxis, SIGNAL(rangeChanged(QCPRange)), euler_data_plot->yAxis2, SLOT(setRange(QCPRange)));


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
            QMessageBox::critical(this, tr("Error"), "Nessun dispositivo collegato!\n"
                                                     "Collegare un dispositivo prima di avviare la connessione seriale.");
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
    // devo ricevere un array di 13 elementi (double) -> 52 char
    // 3 dell'accelerometro (ax,ay,az)
    // 3 del giroscopio (wx,wy,wz)
    // 3 del magnetometro (mx,my,mz)
    // 3 per gli angoli di eulero (roll, pitch, yaw)
    // 1 per il tempo
    const QByteArray data = serial->readAll();

    // trasformo tutti i bytes ricevuti in double
    const auto doubleSize = 4;
    QVector<double> receivedData;
    for(int i=0; i<data.size(); i+=doubleSize){
        receivedData.append( data.mid(i, doubleSize).toDouble() );
    }

    double t = data.at(time);
    QList<QCheckBox *> allCheckBoxRawPlot = ui->tab_rawData->findChildren<QCheckBox *>();
    QCheckBox *checkBox;
    auto i = 0;
    foreach(checkBox, allCheckBoxRawPlot){//per ogni elemento della lista, aggiungi i dati ricevuti
        if( (*checkBox).isChecked() ){ // a seconda di che misura voglio visualizzare
            raw_data_plot->graph(i)->addData(t, receivedData.at(i)); // prendo i dati
            // setto la legenda
            i++;
        }
    }
    raw_data_plot->xAxis->setRange(t, 8, Qt::AlignRight);
    raw_data_plot->replot(); // li plotto


    QList<QCheckBox *> allCheckBoxEulerPlot = ui->tab_eulerAngles->findChildren<QCheckBox *>();
    i = 0;
    foreach(checkBox, allCheckBoxEulerPlot){//per ogni elemento della lista, aggiungi i dati ricevuti
        if( (*checkBox).isChecked() ){ // a seconda di che misura voglio visualizzare
            euler_data_plot->graph(i)->addData(t, receivedData.at(i)); // prendo i dati
            // setto la legenda
            i++;
        }
    }
    euler_data_plot->xAxis->setRange(t, 8, Qt::AlignRight);
    euler_data_plot->replot(); // li plotto
}
