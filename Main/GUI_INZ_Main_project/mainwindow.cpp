#include "mainwindow.h"
#include "./ui_mainwindow.h"
//
// Main constructor of program
//
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusBar()->showMessage("Welcome - Start Page"); // Information at the bottom
    ui->mainStackedWidget->setCurrentIndex(0); // Setting the first page
    this->m_isPathChosen = false; // Is the path chosen for saving the Log file
    this->m_isConnected = false;  // IS the MCU connected
    QSerialPortInfo info; // Available COM ports
    this->m_info = info; // Available COM ports
    this->m_isMCUWorking = false;
    auto portList = info.availablePorts();
    ui->serialPortComboBox->clear();
    for(auto &th:portList){ // Saving the available COM ports list to ComboBox screen 1
        ui->serialPortComboBox->addItem(th.portName());
        ui->portNameLineEdit->setText(th.description());
    }
}
//
// Main destructor
//
MainWindow::~MainWindow()
{
    delete ui;
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
        ui->controlActionPage->setEnabled(true);
        ui->dataPushButton->setEnabled(true);
    }else if(this->m_isPathChosen && !this->m_isConnected){
        ui->statusToDolabel->setText("Connect to the MCU.");
        ui->DataActionPage->setEnabled(false);
        ui->controlActionPage->setEnabled(false);
        ui->dataPushButton->setEnabled(false);
    }else if(! this->m_isPathChosen && this->m_isConnected){
        ui->statusToDolabel->setText("Select the path.");
        ui->DataActionPage->setEnabled(false);
        ui->controlActionPage->setEnabled(false);
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
        ui->portNameLineEdit->setText(th.description());
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
// Test button Connection page - 1
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
        port->clear();
        port->flush();
        port->write("TXXX");
        port->flush();
        QByteArray buf;
        if(port->waitForReadyRead(10)){
            port ->flush();
            buf = port->readAll();
        }
        port->flush();
        port->close();
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
// Copy MCU name button
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
// Connect button on Connecction page - 1
//
void MainWindow::on_pushButton_4_clicked()
{
    if(!this->m_isConnected){ // if is not connected
        if(!ui->serialPortComboBox->currentText().isEmpty()){
            this->m_mcuCommunication = new MCUCommunication(ui->serialPortComboBox->currentText());
            connect(m_mcuCommunication,&MCUCommunication::messageReceived,this,&MainWindow::messageReceived_slot);
            connect(m_mcuCommunication,&MCUCommunication::messageReceivedJSONData,this,&MainWindow::messageReceivedJSONData_slot);
            // Start the MCU
            this->m_mcuCommunication->sendMessage("OXXX");
            this->m_isConnected = true;
            this->m_isMCUWorking = true;
            ui->pushButton_3->setEnabled(false);
            ui->serialPortComboBox->setEnabled(false);
            ui->pushButton_2->setEnabled(false);
            ui->Conncectionstatuslabel->setText("Status: Connected");
            ui->pushButton_4->setText("Disconnect");
            if(this->m_isPathChosen && this->m_isConnected){
                ui->statusToDolabel->setText("");
                ui->DataActionPage->setEnabled(true);
                ui->controlActionPage->setEnabled(true);
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
        this->m_mcuCommunication->sendMessage("EEEE"); // STOP work
        delete this->m_mcuCommunication;
        ui->pushButton_3->setEnabled(true);
        this->m_isConnected = false;
        this->m_isMCUWorking = false;
        ui->Conncectionstatuslabel->setText("Status: Disconnected");
        ui->pushButton_4->setText("Connect");
        ui->serialPortComboBox->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        // Message bar
        if(this->m_isPathChosen && this->m_isConnected){
            ui->statusToDolabel->setText("");
            ui->DataActionPage->setEnabled(true);
            ui->controlActionPage->setEnabled(true);
            ui->dataPushButton->setEnabled(true);
        }else if(this->m_isPathChosen && !this->m_isConnected){
            ui->statusToDolabel->setText("Connect to the MCU.");
            ui->DataActionPage->setEnabled(false);
            ui->controlActionPage->setEnabled(false);
            ui->dataPushButton->setEnabled(false);
            this->m_LOGSystem->writeLOG("-","-","-","-","Disconnected from the MCU");
        }else if(! this->m_isPathChosen && this->m_isConnected){
            ui->statusToDolabel->setText("Select the path.");
            ui->DataActionPage->setEnabled(false);
            ui->controlActionPage->setEnabled(false);
            ui->dataPushButton->setEnabled(false);
        }else{
            ui->statusToDolabel->setText("Select the path and connect to the MCU.");
            ui->DataActionPage->setEnabled(false);
            ui->dataPushButton->setEnabled(false);
        }
    }
}
//
// Received message slot
//
void MainWindow::messageReceived_slot(const QString &message)
{
    qInfo() << message;
}
//
// Copy path to Log file button
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
// Data page button
//
void MainWindow::on_dataPushButton_clicked()
{
    ui->mainStackedWidget->setCurrentIndex(2);
    statusBar()->showMessage("Live data page");
}
//
// Connection page (1) button on Data page - 2
//
void MainWindow::on_connectionPushButton_clicked()
{
    ui->mainStackedWidget->setCurrentIndex(1);
    statusBar()->showMessage("Connection Page");
}
//
// Data page - 2
//
void MainWindow::on_DataActionPage_triggered()
{
    ui->mainStackedWidget->setCurrentIndex(2);
    statusBar()->showMessage("Live data page");
}
//
// message Received in JSON Data slot
//
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
                                    this->m_MCU_Data.syringe_position,"-");
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
        if(FN == 1){ //TODO change
            ui->temperatureFanLabel->setText(tr("Fan: ON"));
        } else{
            ui->temperatureFanLabel->setText(tr("Fan: OFF"));
        }
        ui->logMessageLineEdit->setText("LOG"); // TODO
        break;
    case 3:
        break;
    case 4:
        ui->SyringeSetPositionLineEdit_Injection->setText(this->m_MCU_Data.syringe_set_position);
        ui->CurrentValueLineEdit_SyringeInjection->setText(this->m_MCU_Data.syringe_position);
        ui->NeedleSetPositionLineEdit_Injection->setText(this->m_MCU_Data.needle_set_position);
        ui->CurrentValueLineEdit_Injection->setText(this->m_MCU_Data.needle_position);
        ui->TemperatureLineEdit_Injection->setText(this->m_MCU_Data.temperature);
        if(FN == 1){ //TODO change
            ui->FanInfoLabel_Injection->setText(tr("Fan: ON"));
        } else{
            ui->FanInfoLabel_Injection->setText(tr("Fan: OFF"));
        }
        ui->LogMessageLineEdit_Injection->setText("LOG"); // TODO
        break;
        //TODO next page
    default:
        break;
    }
}
//
// Refresh the data when page changed
//
void MainWindow::on_mainStackedWidget_currentChanged(int arg1)
{
    switch (arg1) {
    case 0:
        // No data to show
        break;
    case 1:
        // No data to show
        break;
    case 2: // Data page
        ui->syringeSetPointLineEdit->setText(this->m_MCU_Data.syringe_set_position);
        ui->syringeCurrentpositionLineEdit->setText(this->m_MCU_Data.syringe_position);
        ui->needleSetPositionLineEdit->setText(this->m_MCU_Data.needle_set_position);
        ui->needleCurrentPositiomLineEdit->setText(this->m_MCU_Data.needle_position);
        ui->temperatureLineEdit->setText(this->m_MCU_Data.temperature); //TODO change fan
        ui->logMessageLineEdit->setText("LOG"); // TODO
        break;
    case 3:
        break;
    case 4:
        ui->SyringeSetPositionLineEdit_Injection->setText(this->m_MCU_Data.syringe_set_position);
        ui->CurrentValueLineEdit_SyringeInjection->setText(this->m_MCU_Data.syringe_position);
        ui->NeedleSetPositionLineEdit_Injection->setText(this->m_MCU_Data.needle_set_position);
        ui->CurrentValueLineEdit_Injection->setText(this->m_MCU_Data.needle_position);
        ui->TemperatureLineEdit_Injection->setText(this->m_MCU_Data.temperature);
        ui->LogMessageLineEdit_Injection->setText("LOG"); // TODO
        break;
    default:
        break;
    }
}
//
// Stop motors button - Data page - 2
//
void MainWindow::on_stopMotorsPushButton_clicked()
{
    if( this->m_isMCUWorking){
        this->m_mcuCommunication->sendMessage("EEEE");
        this->m_isMCUWorking = false;
        ui->stopMotorsPushButton->setStyleSheet("QPushButton { background-color : rgb(60,179,113); }");
        ui->stopMotorsPushButton->setText("START");
        /////////////
        ui->stopMotorsPushButton_Injection->setStyleSheet("QPushButton { background-color : rgb(60,179,113); }");
        ui->stopMotorsPushButton_Injection->setText("START");
        /////////////
        ui->label_21->setText("START the motors:");
        ui->label_47->setText("START the motors:");
    }else{
        this->m_mcuCommunication->sendMessage("OOOO");
        this->m_isMCUWorking = true;
        ui->stopMotorsPushButton->setStyleSheet("QPushButton { background-color : rgb(220,20,60); }");
        ui->stopMotorsPushButton->setText("STOP");
        /////////////
        ui->stopMotorsPushButton_Injection->setStyleSheet("QPushButton { background-color : rgb(220,20,60); }");
        ui->stopMotorsPushButton_Injection->setText("STOP");
        /////////////
        ui->label_47->setText("STOP the motors:");
        ui->label_21->setText("STOP the motors:");
    }
}
//
// Disconnect button - Data page - 2
//
void MainWindow::on_disconnectPushButton_clicked()
{
    this->m_mcuCommunication->sendMessage("EEEE"); // STOP work
    ui->pushButton_3->setEnabled(true);
    this->m_isConnected = false;
    this->m_isMCUWorking = false;
    ui->Conncectionstatuslabel->setText("Status: Disconnected");
    ui->pushButton_4->setText("Connect");
    ui->serialPortComboBox->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    // Message bar
    if(this->m_isPathChosen && this->m_isConnected){
        ui->statusToDolabel->setText("");
        ui->DataActionPage->setEnabled(true);
        ui->controlActionPage->setEnabled(true);
        ui->dataPushButton->setEnabled(true);
    }else if(this->m_isPathChosen && !this->m_isConnected){
        ui->statusToDolabel->setText("Connect to the MCU.");
        ui->DataActionPage->setEnabled(false);
        ui->controlActionPage->setEnabled(false);
        ui->dataPushButton->setEnabled(false);
        this->m_LOGSystem->writeLOG("-","-","-","-","Disconnected from the MCU");
    }else if(! this->m_isPathChosen && this->m_isConnected){
        ui->statusToDolabel->setText("Select the path.");
        ui->DataActionPage->setEnabled(false);
        ui->controlActionPage->setEnabled(false);
        ui->dataPushButton->setEnabled(false);
    }else{
        ui->statusToDolabel->setText("Select the path and connect to the MCU.");
        ui->DataActionPage->setEnabled(false);
        ui->controlActionPage->setEnabled(false);
        ui->dataPushButton->setEnabled(false);
    }
    ui->mainStackedWidget->setCurrentIndex(1);
    delete this->m_mcuCommunication;
}
//
// Control button - Data page - 2
//
void MainWindow::on_controlPushButton_clicked()
{
    ui->mainStackedWidget->setCurrentIndex(4);
    statusBar()->showMessage("Select the type.");
    ui->controlActionPage->setEnabled(true);
}
//
// Control action page button
//
void MainWindow::on_controlActionPage_triggered()
{
    ui->mainStackedWidget->setCurrentIndex(3);
    statusBar()->showMessage("Select the type.");
}
//
// Send to MCU button Needle -> Control page - 4
//
void MainWindow::on_SendToMCUPushButton_NeedleInjection_clicked()
{
    QByteArray _message;
    _message.append('N');
    QString _number = ui->NewSetPositionSpinBox_NeedleInjection->text();
    switch (_number.size()) {
    case 1:
        _message.append('0');
        _message.append('0');
        _message.append(_number.toStdString());
        break;
    case 2:
        _message.append('0');
        _message.append(_number.toStdString());
        break;
    default:
        _message.append(_number.toStdString());
        break;
    }
    this->m_mcuCommunication->sendMessage(_message);
}
//
// Send to MCU button Syringe -> Control page - 4
//
void MainWindow::on_SendToMCUPushButton_SyringeInjection_clicked()
{
    QByteArray _message;
    _message.append('S');
    QString _number = ui->NewSetPositionSpinBox_SyringeInjection->text();
    switch (_number.size()) {
    case 1:
        _message.append('0');
        _message.append('0');
        _message.append(_number.toStdString());
        break;
    case 2:
        _message.append('0');
        _message.append(_number.toStdString());
        break;
    default:
        _message.append(_number.toStdString());
        break;
    }
    this->m_mcuCommunication->sendMessage(_message);
}
//
// Home position push button Needle -> Control page - 4
//
void MainWindow::on_HomePositionPushButton_NeedleInjection_clicked()
{
    ui->NewSetPositionSpinBox_NeedleInjection->setValue(100); // TODO check home position
    this->m_mcuCommunication->sendMessage("N100"); // TODO check home position
}
//
// Home position push button Syringe -> Control page - 4
//
void MainWindow::on_HomePositionPushButton_SyringeInjection_clicked()
{
    ui->NewSetPositionSpinBox_SyringeInjection->setValue(100); // TODO check home position
    this->m_mcuCommunication->sendMessage("S100"); // TODO check home position
}
//
// Plus one mm push button Needle -> Control page - 4
//
void MainWindow::on_PlusOnemmPushButton_NeedleInjection_clicked()
{
    int number = ui->NewSetPositionSpinBox_NeedleInjection->value();
    number += 1;
    ui->NewSetPositionSpinBox_NeedleInjection->setValue(number);
    QByteArray _message;
    _message.append('N');
    QString _number = QString::number(number);
    switch (_number.size()) {
    case 1:
        _message.append('0');
        _message.append('0');
        _message.append(_number.toStdString());
        break;
    case 2:
        _message.append('0');
        _message.append(_number.toStdString());
        break;
    default:
        _message.append(_number.toStdString());
        break;
    }
    this->m_mcuCommunication->sendMessage(_message);
}
//
// Minus one mm push button Needle -> Control page - 4
//
void MainWindow::on_MinusOnemmPushButton_NeedleInjection_clicked()
{
    int number = ui->NewSetPositionSpinBox_NeedleInjection->value();
    number -= 1;
    ui->NewSetPositionSpinBox_NeedleInjection->setValue(number);
    QByteArray _message;
    _message.append('N');
    QString _number = QString::number(number);
    switch (_number.size()) {
    case 1:
        _message.append('0');
        _message.append('0');
        _message.append(_number.toStdString());
        break;
    case 2:
        _message.append('0');
        _message.append(_number.toStdString());
        break;
    default:
        _message.append(_number.toStdString());
        break;
    }
    this->m_mcuCommunication->sendMessage(_message);
}
//
// Plus one ml push button Syringe -> Control page - 4
//
void MainWindow::on_PlusOnemlPushButton_SyringeInjection_clicked()
{
    int number = ui->NewSetPositionSpinBox_SyringeInjection->value();
    number += 1; // TODO chceck how much is 1 ml
    ui->NewSetPositionSpinBox_SyringeInjection->setValue(number);
    QByteArray _message;
    _message.append('S');
    QString _number = QString::number(number);
    switch (_number.size()) {
    case 1:
        _message.append('0');
        _message.append('0');
        _message.append(_number.toStdString());
        break;
    case 2:
        _message.append('0');
        _message.append(_number.toStdString());
        break;
    default:
        _message.append(_number.toStdString());
        break;
    }
    this->m_mcuCommunication->sendMessage(_message);
}
//
// Minus one ml push button Syringe -> Control page - 4
//
void MainWindow::on_MinusOnemlPushButton_SyringeInjection_clicked()
{
    int number = ui->NewSetPositionSpinBox_SyringeInjection->value();
    number -= 1; // TODO chceck how much is 1 ml
    ui->NewSetPositionSpinBox_SyringeInjection->setValue(number);
    QByteArray _message;
    _message.append('S');
    QString _number = QString::number(number);
    switch (_number.size()) {
    case 1:
        _message.append('0');
        _message.append('0');
        _message.append(_number.toStdString());
        break;
    case 2:
        _message.append('0');
        _message.append(_number.toStdString());
        break;
    default:
        _message.append(_number.toStdString());
        break;
    }
    this->m_mcuCommunication->sendMessage(_message);
}
//
// Data page push button - Control page - 4
//
void MainWindow::on_DataPagePushButton_Injection_clicked()
{
    ui->mainStackedWidget->setCurrentIndex(2);
    statusBar()->showMessage("Data page.");
}


void MainWindow::on_stopMotorsPushButton_Injection_clicked()
{
    if( this->m_isMCUWorking){
        this->m_mcuCommunication->sendMessage("EEEE");
        this->m_isMCUWorking = false;
        ui->stopMotorsPushButton->setStyleSheet("QPushButton { background-color : rgb(60,179,113); }");
        ui->stopMotorsPushButton->setText("START");
        /////////////
        ui->stopMotorsPushButton_Injection->setStyleSheet("QPushButton { background-color : rgb(60,179,113); }");
        ui->stopMotorsPushButton_Injection->setText("START");
        /////////////
        ui->label_21->setText("START the motors:");
        ui->label_47->setText("START the motors:");
    }else{
        this->m_mcuCommunication->sendMessage("OOOO");
        this->m_isMCUWorking = true;
        ui->stopMotorsPushButton->setStyleSheet("QPushButton { background-color : rgb(220,20,60); }");
        ui->stopMotorsPushButton->setText("STOP");
        /////////////
        ui->stopMotorsPushButton_Injection->setStyleSheet("QPushButton { background-color : rgb(220,20,60); }");
        ui->stopMotorsPushButton_Injection->setText("STOP");
        /////////////
        ui->label_47->setText("STOP the motors:");
        ui->label_21->setText("STOP the motors:");
    }
}

