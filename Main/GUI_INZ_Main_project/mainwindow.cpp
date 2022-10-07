#include "mainwindow.h"
#include "./ui_mainwindow.h"
//
// Main constructor for whole program
//
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusBar()->showMessage("Welcome - Start Page"); // Informacja na pasku dolnym
    ui->mainStackedWidget->setCurrentIndex(0); // Setting the first page
    this->m_isPathChosen = false; // Is the path chosen for saving the Log file
    this->m_isConnected = false;  // IS the MCU connected
    QSerialPortInfo info; // Available COM ports
    this->m_info = info; // Available COM ports
    auto portList = info.availablePorts();
    ui->serialPortComboBox->clear();
    for(auto &th:portList){ // Saving the available COM ports list to ComboBox screen 1
        ui->serialPortComboBox->addItem(th.portName());
        ui->portNameLineEdit->setText(th.serialNumber());
    }
}
//
// Main destructor
//
MainWindow::~MainWindow()
{
    delete ui;
    this->m_mcuCommunication->stopWork();
}
//
// Executing the start page - 0
//
void MainWindow::on_StartActionPage_triggered()
{
    ui->mainStackedWidget->setCurrentIndex(0); // Show start page - 0
    statusBar()->showMessage("Start Page"); // Status bar message
}
//
// Closing the application with Quit button
//
void MainWindow::on_actionQuit_triggered()
{
    // Close the application
    int ret = QMessageBox::question(this, tr("Quit the application"),
                                    tr("Do you want to quit the application?"),
                                    QMessageBox::Ok | QMessageBox::Cancel);
    // If confirmed then exit
    if(ret == QMessageBox::Ok){
        QApplication::quit();
    }
}
//
// Executing the Connection page - 1
//
void MainWindow::on_connectionActionPage_triggered()
{
    ui->mainStackedWidget->setCurrentIndex(1); // Show connection page - 1
    statusBar()->showMessage("Connection Page"); // Status bar message
}
//
// Executing the Connection page - 1 with Start button
//
void MainWindow::on_startPushButton_page_1_Start_clicked()
{
    ui->mainStackedWidget->setCurrentIndex(1); // Show connection page - 1
    statusBar()->showMessage("Connection Page"); // Status bar message
}
//
// Executing AboutQT dialog
//
void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt(); // Show dialog
}
//
// Save button on Connection page - 1
//
void MainWindow::on_savePathLogPushButton_clicked()
{
    if(ui->nameMCULineEdit->text().isEmpty()){ // Chceck if Name of the MCU in Save section is empty
        QMessageBox::information(this,tr("Specify the name"),tr("To add the path you need to specify the MCU name first."));
    }else{
        this->m_PathLOGFile = QFileDialog::getSaveFileName(this,"Save file","/home/Output.txt","Text files (*.txt)"); // Choosing the place to sotre the Log file
        if(!m_PathLOGFile.isEmpty()){ // Chceck if path is empty
            ui->saveFileLogLineEdit->setText(m_PathLOGFile);
            this->m_LOGSystem = new LOGSystem(this,m_PathLOGFile,ui->nameMCULineEdit->text());
            this->m_isPathChosen = true;
        }
    }
    // Update info about what to do
    if(this->m_isPathChosen && this->m_isConnected){
        ui->statusToDolabel->setText("");
        ui->DataActionPage->setEnabled(true);
        ui->dataPushButton->setEnabled(true);
    }else if(this->m_isPathChosen && !this->m_isConnected){
        ui->statusToDolabel->setText("Connect to the MCU.");
        ui->DataActionPage->setEnabled(false);
        ui->dataPushButton->setEnabled(false);
    }else if(! this->m_isPathChosen && this->m_isConnected){
        ui->statusToDolabel->setText("Select the path.");
        ui->DataActionPage->setEnabled(false);
        ui->dataPushButton->setEnabled(false);
    }

}

//
// Refresh button Connection page - 1
//
void MainWindow::on_pushButton_2_clicked()
{
    ui->portNameLineEdit->clear();
    QSerialPortInfo info;
    auto portList = info.availablePorts();
    this->m_info = info;
    ui->serialPortComboBox->clear();
    for(auto &th:portList){
        ui->serialPortComboBox->addItem(th.portName());
        ui->portNameLineEdit->setText(th.serialNumber());
    }
}

//
// Update COM port name when choice changed on Connection page - 1
//
void MainWindow::on_serialPortComboBox_currentTextChanged(const QString &arg1)
{
    auto portList = m_info.availablePorts();
    for(auto &th:portList){
        if(th.serialNumber() == arg1){
            ui->portNameLineEdit->setText(th.serialNumber());
        }
    }
}

//
// Obsługa przycisku Test strona Connection nr 1
//
void MainWindow::on_pushButton_3_clicked()
{
    if(! ui->portNameLineEdit->text().isEmpty()){
        statusBar()->showMessage("Testing");
        QSerialPort *port = new QSerialPort;
        port->setPortName(ui->serialPortComboBox->currentText());
        port->setBaudRate(QSerialPort::Baud115200);
        port->setDataBits(QSerialPort::Data8);
        port->setParity(QSerialPort::NoParity);
        port->setStopBits(QSerialPort::OneStop);
        port->setFlowControl(QSerialPort::NoFlowControl);
        port->open(QIODevice::ReadWrite);
        port->clear(); // TODO check if is working!!!
        port->flush();
        port->write("TXXX");
        // Musi przesłać i odebrać T
        port->flush();
        QByteArray buf;
        if(port->waitForReadyRead(10)){
            port ->flush();
            buf = port->readAll();
        }
        port->flush();
        port->close();
        //////// TODO
        qInfo() << buf;
        /////////////
        if(buf.toStdString() == "T"){
            ui->testLabel->setText("Test: Success");
            ui->testLabel->setStyleSheet("QLabel { color : green; }");
        }else{
            ui->testLabel->setText("Test: Error");
            ui->testLabel->setStyleSheet("QLabel { color : red; }");
        }
        statusBar()->showMessage("Connection Page");
        delete port;
    }else{
        QMessageBox::critical(this,"No port selected","To perform a test select a port.");
    }
}
//
// Obsługa kopiowania nazwy mcu
//
void MainWindow::on_actionCopy_serial_port_name_triggered()
{
    if(ui->portNameLineEdit->text().isEmpty()){
        QMessageBox::critical(this,"No port selected","To copy port name select a port.");
    }else{
        ui->portNameLineEdit->selectAll();
        ui->portNameLineEdit->copy();
        ui->portNameLineEdit->deselect();
    }
}
//
// Obsługa przycisku connect
//
void MainWindow::on_pushButton_4_clicked()
{
    if(!this->m_isConnected){
        if(!ui->serialPortComboBox->currentText().isEmpty()){
            this->m_mcuCommunication = new MCUCommunication(ui->serialPortComboBox->currentText());
            connect(m_mcuCommunication, SIGNAL(finished()),m_mcuCommunication, SLOT(deleteLater()));
            connect(m_mcuCommunication,&MCUCommunication::messageReceived,this,&MainWindow::messageReceived_slot);
            connect(m_mcuCommunication,&MCUCommunication::messageReceivedJSONData,this,&MainWindow::messageReceivedJSONData_slot);
            this->m_mcuCommunication->start();
            // Przeprowadzenie testu łączności
            this->m_mcuCommunication->sendMessage("OXXX");
            // TODO implement if connection failed
            this->m_isConnected = true;
            ui->pushButton_3->setEnabled(false);
            ui->serialPortComboBox->setEnabled(false);
            ui->pushButton_2->setEnabled(false);
            ui->Conncectionstatuslabel->setText("Status: Connected");
            ui->pushButton_4->setText("Disconnect");
            // Napis na dole ekarnu
            if(this->m_isPathChosen && this->m_isConnected){
                ui->statusToDolabel->setText("");
                ui->DataActionPage->setEnabled(true);
                ui->dataPushButton->setEnabled(true);
                this->m_LOGSystem->writeLOG("-","-","-","-","Connected to the MCU");
            }else if(this->m_isPathChosen && !this->m_isConnected){
                ui->statusToDolabel->setText("Connect to the MCU.");
            }else if(! this->m_isPathChosen && this->m_isConnected){
                ui->statusToDolabel->setText("Select the path.");
            }else{
                ui->statusToDolabel->setText("Select the path and connect to the MCU.");
            }
        }else{
            QMessageBox::critical(this,"No port selected","To perform a connection select a port.");
        }
    }else{
        this->m_mcuCommunication->stopWork();
        ui->pushButton_3->setEnabled(true);
        this->m_isConnected = false;
        ui->Conncectionstatuslabel->setText("Status: Disconnected");
        ui->pushButton_4->setText("Connect");
        ui->serialPortComboBox->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        // Napis na dole ekarnu
        if(this->m_isPathChosen && this->m_isConnected){
            ui->statusToDolabel->setText("");
            ui->DataActionPage->setEnabled(true);
            ui->dataPushButton->setEnabled(true);
        }else if(this->m_isPathChosen && !this->m_isConnected){
            ui->statusToDolabel->setText("Connect to the MCU.");
            ui->DataActionPage->setEnabled(false);
            ui->dataPushButton->setEnabled(false);
            this->m_LOGSystem->writeLOG("-","-","-","-","Disconnected from the MCU");
        }else if(! this->m_isPathChosen && this->m_isConnected){
            ui->statusToDolabel->setText("Select the path.");
            ui->DataActionPage->setEnabled(false);
            ui->dataPushButton->setEnabled(false);
        }else{
            ui->statusToDolabel->setText("Select the path and connect to the MCU.");
            ui->DataActionPage->setEnabled(false);
            ui->dataPushButton->setEnabled(false);
        }
    }
}
//
// Slot otrzymanej wiadomości
//
void MainWindow::messageReceived_slot(const QString &message)
{
    // TODO: implement !!!
    qInfo() << message;
    //std::cout << message.toStdString();
    //qInfo() << (message.toStdString().c_str());
    // Aktualizacja danych dla ekranu Data -> index 2
}
//
// Obsługa kopiowania sciezki log
//
void MainWindow::on_actionCopy_path_to_Log_file_triggered()
{
    if(ui->saveFileLogLineEdit->text().isEmpty() || ui->saveFileLogLineEdit->text() == "Path"){
        QMessageBox::critical(this,tr("No path selected"),tr("To copy path select a save path."));
    }else{
        ui->saveFileLogLineEdit->selectAll();
        ui->saveFileLogLineEdit->copy();
        ui->saveFileLogLineEdit->deselect();
    }
}
//
// Licenses dialog
//
void MainWindow::on_actionLicenses_triggered()
{
    this->m_licensesDialog = new licensesDialog(this);
    this->m_licensesDialog->show();
}
//
// Załączenie strony Data
//
void MainWindow::on_dataPushButton_clicked()
{
    ui->mainStackedWidget->setCurrentIndex(2);
    statusBar()->showMessage("Live data page");
}
//
// Załączenie strony Connection z przycisku na stronie Data
//
void MainWindow::on_connectionPushButton_clicked()
{
    ui->mainStackedWidget->setCurrentIndex(1);
    statusBar()->showMessage("Connection Page");
}
//
// Załączenie strony Data
//
void MainWindow::on_DataActionPage_triggered()
{
    ui->mainStackedWidget->setCurrentIndex(2);
    statusBar()->showMessage("Live data page");
}

void MainWindow::messageReceivedJSONData_slot(const double &NP, const double &SP, const double &NS, const double &SS, const double &TM, const double &FN, const double &ST)
{
    this->m_MCU_Data.needle_position = QString::number(NP);
    this->m_MCU_Data.syringe_position = QString::number(SP);
    this->m_MCU_Data.needle_set_position = QString::number(NS);
    this->m_MCU_Data.syringe_set_position = QString::number(SS);
    this->m_MCU_Data.temperature = QString::number(TM);
    this->m_MCU_Data.fan = QString::number(FN);
    this->m_MCU_Data.status = QString::number(ST);
    // Write LOG file
    if (this->m_isPathChosen) {
        this->m_LOGSystem->writeLOG(this->m_MCU_Data.needle_set_position,this->m_MCU_Data.needle_position,this->m_MCU_Data.syringe_set_position,
                                    this->m_MCU_Data.syringe_position,"");
    }

    // Update the page
    switch (ui->mainStackedWidget->currentIndex()) {
    case 2: //Data page
        if(! (ui->syringeSetPointLineEdit->text() == this->m_MCU_Data.syringe_set_position)){
            ui->syringeSetPointLineEdit->setText(this->m_MCU_Data.syringe_set_position);
        }
        if(!(ui->syringeCurrentpositionLineEdit->text() == this->m_MCU_Data.syringe_position)){
            ui->syringeCurrentpositionLineEdit->setText(this->m_MCU_Data.syringe_position);
        }
        if(!(ui->needleSetPositionLineEdit->text() == this->m_MCU_Data.needle_set_position)){
            ui->needleSetPositionLineEdit->setText(this->m_MCU_Data.needle_set_position);
        }
        if(!(ui->needleCurrentPositiomLineEdit->text() == this->m_MCU_Data.needle_position)){
            ui->needleCurrentPositiomLineEdit->setText(this->m_MCU_Data.needle_position);
        }
        if(!(ui->temperatureLineEdit->text() == this->m_MCU_Data.temperature)){
            ui->temperatureLineEdit->setText(this->m_MCU_Data.temperature);
        }
        if(FN == 1){
            ui->temperatureFanLabel->setText(tr("Fan: ON"));
        } else{
            ui->temperatureFanLabel->setText(tr("Fan: OFF"));
        }
        ui->logMessageLineEdit->setText("LOG");
        break;
    default:
        break;
    }
}


void MainWindow::on_mainStackedWidget_currentChanged(int arg1)
{
    switch (arg1) {
    case 0:

        break;
    case 1:

        break;
    case 2:
        ui->syringeSetPointLineEdit->setText(this->m_MCU_Data.syringe_set_position);
        ui->syringeCurrentpositionLineEdit->setText(this->m_MCU_Data.syringe_position);
        ui->needleSetPositionLineEdit->setText(this->m_MCU_Data.needle_set_position);
        ui->needleCurrentPositiomLineEdit->setText(this->m_MCU_Data.needle_position);
        ui->temperatureLineEdit->setText(this->m_MCU_Data.temperature);
        ui->logMessageLineEdit->setText("LOG");
        break;
    default:
        break;
    }
}

