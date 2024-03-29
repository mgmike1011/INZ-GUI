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
    this->m_HomePosition_Needle_Injection = 100; //TODO check value
    this->m_HomePosition_Needle_Suction = 100; //TODO check value
    this->m_HomePosition_Syringe_Injection = 100; // TODO check value
    this->m_HomePosition_Syringe_Suction = 100; // TODO check value

    ui->SyringeFillProgressBar_Injection->setMinimum(0); // in ml
    ui->SyringeFillProgressBar_Injection->setMaximum(this->m_HomePosition_Syringe_Injection / MM_TO_ML); // Max ml in syringe
    ui->SyringeFillProgressBar_Suction->setMinimum(0);
    ui->SyringeFillProgressBar_Suction->setMaximum(this->m_HomePosition_Syringe_Suction / MM_TO_ML); // Max ml in syringe

    ui->mm_to_ml_spinBox->setValue(MM_TO_ML);
    ui->InjectionNeedleHomePositionspinBox->setValue(this->m_HomePosition_Needle_Injection);
    ui->InjectionSyringeHomePositionSpinBox->setValue( this->m_HomePosition_Syringe_Injection);
    ui->SuctionNeedleHomePositionspinBox->setValue(this->m_HomePosition_Needle_Suction);
    ui->SuctionSyringeHomePositionspinBox->setValue(this->m_HomePosition_Syringe_Suction);
    ui->NeedleSpeedSpinBox->setValue(100); // TODO check
    ui->SyringeSpeedSpinBox->setValue(100); // TODO check
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
        ui->SettingsActionPage->setEnabled(true);
    }else if(this->m_isPathChosen && !this->m_isConnected){
        ui->statusToDolabel->setText("Connect to the MCU.");
        ui->DataActionPage->setEnabled(false);
        ui->controlActionPage->setEnabled(false);
        ui->SettingsActionPage->setEnabled(false);
        ui->dataPushButton->setEnabled(false);
    }else if(! this->m_isPathChosen && this->m_isConnected){
        ui->statusToDolabel->setText("Select the path.");
        ui->DataActionPage->setEnabled(false);
        ui->controlActionPage->setEnabled(false);
        ui->dataPushButton->setEnabled(false);
        ui->SettingsActionPage->setEnabled(false);
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
                ui->SettingsActionPage->setEnabled(true);
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
            ui->SettingsActionPage->setEnabled(true);
        }else if(this->m_isPathChosen && !this->m_isConnected){
            ui->statusToDolabel->setText("Connect to the MCU.");
            ui->DataActionPage->setEnabled(false);
            ui->controlActionPage->setEnabled(false);
            ui->dataPushButton->setEnabled(false);
            ui->SettingsActionPage->setEnabled(false);
            this->m_LOGSystem->writeLOG("-","-","-","-","Disconnected from the MCU");
        }else if(! this->m_isPathChosen && this->m_isConnected){
            ui->statusToDolabel->setText("Select the path.");
            ui->DataActionPage->setEnabled(false);
            ui->controlActionPage->setEnabled(false);
            ui->dataPushButton->setEnabled(false);
            ui->SettingsActionPage->setEnabled(false);
        }else{
            ui->statusToDolabel->setText("Select the path and connect to the MCU.");
            ui->DataActionPage->setEnabled(false);
            ui->dataPushButton->setEnabled(false);
            ui->SettingsActionPage->setEnabled(false);
            ui->controlActionPage->setEnabled(false);
        }
    }
}
//
// Received message slot
//
void MainWindow::messageReceived_slot(const QString &message)
{
//    qInfo() << message;
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
    //
    // Message
    //
    switch (this->m_MCU_Data.status.toInt()) {
    case 0:
        this->m_MCU_Data.message = tr("Status: OK");
        break;
    case 1:
        this->m_MCU_Data.message = tr("Status: MOTOR STOP");
        break;
    default:
        this->m_MCU_Data.message = tr("Bad status");
        break;
    }
    //
    // Write LOG file
    //
    if (this->m_isPathChosen) {
        this->m_LOGSystem->writeLOG(this->m_MCU_Data.needle_set_position,this->m_MCU_Data.needle_position,this->m_MCU_Data.syringe_set_position,
                                    this->m_MCU_Data.syringe_position,this->m_MCU_Data.message);
    }

    // Update the page
    switch (ui->mainStackedWidget->currentIndex()) {
    case 0:
         // No data to show
        break;
    case 1:
         // No data to show
        break;
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
        if(!(ui->logMessageLineEdit->text() == this->m_MCU_Data.message)){
            ui->logMessageLineEdit->setText(this->m_MCU_Data.message);
        }
        break;
    case 3: // Choose surgery type
        break;
    case 4: // Injection
        if(!(ui->SyringeSetPositionLineEdit_Injection->text() == this->m_MCU_Data.syringe_set_position)){
            ui->SyringeSetPositionLineEdit_Injection->setText(this->m_MCU_Data.syringe_set_position);
        }
        if(!(ui->CurrentValueLineEdit_SyringeInjection->text() == this->m_MCU_Data.syringe_position)){
            ui->CurrentValueLineEdit_SyringeInjection->setText(this->m_MCU_Data.syringe_position);
            ui->SyringeFillProgressBar_Injection->setValue(this->m_MCU_Data.syringe_position.toInt() / MM_TO_ML); // Syringe fill progres bar
        }
        if(!(ui->NeedleSetPositionLineEdit_Injection->text() == this->m_MCU_Data.needle_set_position)){
            ui->NeedleSetPositionLineEdit_Injection->setText(this->m_MCU_Data.needle_set_position);
        }
        if(!(ui->CurrentValueLineEdit_Injection->text() == this->m_MCU_Data.needle_position)){
            ui->CurrentValueLineEdit_Injection->setText(this->m_MCU_Data.needle_position);
        }
        if(!(ui->TemperatureLineEdit_Injection->text() == this->m_MCU_Data.temperature)){
            ui->TemperatureLineEdit_Injection->setText(this->m_MCU_Data.temperature);
        }
        if(FN == 1){
            ui->FanInfoLabel_Injection->setText(tr("Fan: ON"));
        } else{
            ui->FanInfoLabel_Injection->setText(tr("Fan: OFF"));
        }
        if(!(ui->LogMessageLineEdit_Injection->text() == this->m_MCU_Data.message)){
            ui->LogMessageLineEdit_Injection->setText(this->m_MCU_Data.message);
        }
        break;
    case 5: // Suction
        if(!(ui->SyringeSetPositionLineEdit_Suction->text() == this->m_MCU_Data.syringe_set_position)){
            ui->SyringeSetPositionLineEdit_Suction->setText(this->m_MCU_Data.syringe_set_position);
        }
        if(!(ui->CurrentValueLineEdit_SyringeSuction->text() == this->m_MCU_Data.syringe_position)){
            ui->CurrentValueLineEdit_SyringeSuction->setText(this->m_MCU_Data.syringe_position);
            ui->SyringeFillProgressBar_Suction->setValue(this->m_MCU_Data.syringe_position.toInt() / MM_TO_ML); // Syringe fill progres bar
        }
        if(!(ui->NeedleSetPositionLineEdit_Suction->text() == this->m_MCU_Data.needle_set_position)){
            ui->NeedleSetPositionLineEdit_Suction->setText(this->m_MCU_Data.needle_set_position);
        }
        if(!(ui->CurrentValueLineEdit_Suction->text() == this->m_MCU_Data.needle_position)){
            ui->CurrentValueLineEdit_Suction->setText(this->m_MCU_Data.needle_position);
        }
        if(!(ui->TemperatureLineEdit_Suction->text() == this->m_MCU_Data.temperature)){
            ui->TemperatureLineEdit_Suction->setText(this->m_MCU_Data.temperature);
        }
        if(FN == 1){
            ui->FanInfoLabel_Suction->setText(tr("Fan: ON"));
        } else{
            ui->FanInfoLabel_Suction->setText(tr("Fan: OFF"));
        }
        if(!(ui->LogMessageLineEdit_Suction->text() == this->m_MCU_Data.message)){
            ui->LogMessageLineEdit_Suction->setText(this->m_MCU_Data.message);
        }
        break;
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
        ui->temperatureLineEdit->setText(this->m_MCU_Data.temperature);
        if(this->m_MCU_Data.fan == "1"){
            ui->temperatureFanLabel->setText(tr("Fan: ON"));
        } else{
            ui->temperatureFanLabel->setText(tr("Fan: OFF"));
        }
        ui->logMessageLineEdit->setText(this->m_MCU_Data.message);
        break;
    case 3: // Choose surgery type
        break;
    case 4: // Injection
        ui->SyringeSetPositionLineEdit_Injection->setText(this->m_MCU_Data.syringe_set_position);
        ui->CurrentValueLineEdit_SyringeInjection->setText(this->m_MCU_Data.syringe_position);
        ui->NeedleSetPositionLineEdit_Injection->setText(this->m_MCU_Data.needle_set_position);
        ui->CurrentValueLineEdit_Injection->setText(this->m_MCU_Data.needle_position);
        ui->TemperatureLineEdit_Injection->setText(this->m_MCU_Data.temperature);
        if(this->m_MCU_Data.fan == "1"){
            ui->FanInfoLabel_Injection->setText(tr("Fan: ON"));
        } else{
            ui->FanInfoLabel_Injection->setText(tr("Fan: OFF"));
        }
        ui->LogMessageLineEdit_Injection->setText(this->m_MCU_Data.message);
        ui->NewSetPositionSpinBox_NeedleInjection->setValue(this->m_MCU_Data.needle_set_position.toInt());
        ui->NewSetPositionSpinBox_SyringeInjection->setValue(this->m_MCU_Data.syringe_position.toInt());
        break;
    case 5: // Suction
        ui->SyringeSetPositionLineEdit_Suction->setText(this->m_MCU_Data.syringe_set_position);
        ui->CurrentValueLineEdit_SyringeSuction->setText(this->m_MCU_Data.syringe_position);
        ui->NeedleSetPositionLineEdit_Suction->setText(this->m_MCU_Data.needle_set_position);
        ui->CurrentValueLineEdit_Suction->setText(this->m_MCU_Data.needle_position);
        ui->TemperatureLineEdit_Suction->setText(this->m_MCU_Data.temperature);
        if(this->m_MCU_Data.fan == "1"){
            ui->FanInfoLabel_Suction->setText(tr("Fan: ON"));
        } else{
            ui->FanInfoLabel_Suction->setText(tr("Fan: OFF"));
        }
        ui->LogMessageLineEdit_Suction->setText(this->m_MCU_Data.message);
        ui->NewSetPositionSpinBox_Suction->setValue(this->m_MCU_Data.needle_set_position.toInt());
        ui->NewSetPositionSpinBox_SyringeSuction->setValue(this->m_MCU_Data.syringe_set_position.toInt());
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
        this->m_mcuCommunication->sendMessage("MMMM");
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
    ui->mainStackedWidget->setCurrentIndex(3);
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
    ui->NewSetPositionSpinBox_NeedleInjection->setValue(this->m_HomePosition_Needle_Injection);
    QByteArray _message;
    _message.append('N');
    QString _number = QString::number(this->m_HomePosition_Needle_Injection);
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
// Home position push button Syringe -> Control page - 4
//
void MainWindow::on_HomePositionPushButton_SyringeInjection_clicked()
{
    ui->NewSetPositionSpinBox_SyringeInjection->setValue(this->m_HomePosition_Syringe_Injection);
    QByteArray _message;
    _message.append('S');
    QString _number = QString::number(this->m_HomePosition_Syringe_Injection);
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
    number += MM_TO_ML;
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
    number -= MM_TO_ML;
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
        this->m_mcuCommunication->sendMessage("MMMM");
        this->m_isMCUWorking = false;
        ui->stopMotorsPushButton->setStyleSheet("QPushButton { background-color : rgb(60,179,113); }");
        ui->stopMotorsPushButton->setText("START");
        /////////////
        ui->stopMotorsPushButton_Injection->setStyleSheet("QPushButton { background-color : rgb(60,179,113); }");
        ui->stopMotorsPushButton_Injection->setText("START");
        /////////////
        ui->stopMotorsPushButton_Suction->setStyleSheet("QPushButton { background-color : rgb(60,179,113); }");
        ui->stopMotorsPushButton_Suction->setText("START");
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
        ui->stopMotorsPushButton_Suction->setStyleSheet("QPushButton { background-color : rgb(220,20,60); }");
        ui->stopMotorsPushButton_Suction->setText("STOP");
        /////////////
        ui->label_47->setText("STOP the motors:");
        ui->label_21->setText("STOP the motors:");
    }
}
//
// Injection button - Choose type page - 3
//
void MainWindow::on_InjectionButton_clicked()
{
    ui->mainStackedWidget->setCurrentIndex(4);
    statusBar()->showMessage("Injection.");
}
//
// Inject button - Injection page - 4
//
void MainWindow::on_InjectPushButton_Injection_clicked()
{
    int number_mm = ui->MililitersToInjectSpinBox_Injection->value() * MM_TO_ML;
    int number = this->m_MCU_Data.syringe_set_position.toInt() - number_mm;
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
    ui->NewSetPositionSpinBox_SyringeInjection->setValue(number);
    this->m_mcuCommunication->sendMessage(_message);
}
//
// Suction push button - Choose surgery type page - 3
//
void MainWindow::on_SuctionButton_clicked()
{
    ui->mainStackedWidget->setCurrentIndex(5);
    statusBar()->showMessage("Suction.");
}
//
// Stop push button - Suction page - 5
//
void MainWindow::on_stopMotorsPushButton_Suction_clicked()
{
    if( this->m_isMCUWorking){
        this->m_mcuCommunication->sendMessage("MMMM");
        this->m_isMCUWorking = false;
        ui->stopMotorsPushButton->setStyleSheet("QPushButton { background-color : rgb(60,179,113); }");
        ui->stopMotorsPushButton->setText("START");
        /////////////
        ui->stopMotorsPushButton_Injection->setStyleSheet("QPushButton { background-color : rgb(60,179,113); }");
        ui->stopMotorsPushButton_Injection->setText("START");
        /////////////
        ui->stopMotorsPushButton_Suction->setStyleSheet("QPushButton { background-color : rgb(60,179,113); }");
        ui->stopMotorsPushButton_Suction->setText("START");
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
        ui->stopMotorsPushButton_Suction->setStyleSheet("QPushButton { background-color : rgb(220,20,60); }");
        ui->stopMotorsPushButton_Suction->setText("STOP");
        /////////////
        ui->label_47->setText("STOP the motors:");
        ui->label_21->setText("STOP the motors:");
    }
}
//
// Data page push button - Suction page - 5
//
void MainWindow::on_DataPagePushButton_Suction_clicked()
{
    ui->mainStackedWidget->setCurrentIndex(2);
    statusBar()->showMessage("Data page.");
}
//
// Home position needle push button - Suction page - 5
//
void MainWindow::on_HomePositionPushButton_NeedleSuction_clicked()
{
    ui->NewSetPositionSpinBox_Suction->setValue(this->m_HomePosition_Needle_Suction);
    QByteArray _message;
    _message.append('N');
    QString _number = QString::number(this->m_HomePosition_Needle_Suction);
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
// Plus one mm needle - Suction page - 5
//
void MainWindow::on_PlusOnemmPushButton_NeedleSuction_clicked()
{
    int number = ui->NewSetPositionSpinBox_Suction->value();
    number += 1;
    ui->NewSetPositionSpinBox_Suction->setValue(number);
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
// Plus one mm needle - Suction page - 5
//
void MainWindow::on_MinusOnemmPushButton_NeedleSuction_clicked()
{
    int number = ui->NewSetPositionSpinBox_Suction->value();
    number -= 1;
    ui->NewSetPositionSpinBox_Suction->setValue(number);
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
// Send to MCU needle push button - Suction page - 5
//
void MainWindow::on_SendToMCUPushButton_NeedleSuction_clicked()
{
    QByteArray _message;
    _message.append('N');
    QString _number = ui->NewSetPositionSpinBox_Suction->text();
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
// Home position syringe push button - Suction page - 5
//
void MainWindow::on_HomePositionPushButton_SyringeSuction_clicked()
{
    ui->NewSetPositionSpinBox_SyringeSuction->setValue(this->m_HomePosition_Syringe_Suction);
    QByteArray _message;
    _message.append('S');
    QString _number = QString::number(this->m_HomePosition_Syringe_Suction);
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


void MainWindow::on_PlusOnemlPushButton_SyringeSuction_clicked()
{
    int number = ui->NewSetPositionSpinBox_SyringeSuction->value();
    number += MM_TO_ML;
    ui->NewSetPositionSpinBox_SyringeSuction->setValue(number);
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


void MainWindow::on_MinusOnemlPushButton_SyringeSuction_clicked()
{
    int number = ui->NewSetPositionSpinBox_SyringeSuction->value();
    number -= MM_TO_ML;
    ui->NewSetPositionSpinBox_SyringeSuction->setValue(number);
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


void MainWindow::on_SendToMCUPushButton_SyringeSuction_clicked()
{
    QByteArray _message;
    _message.append('S');
    QString _number = ui->NewSetPositionSpinBox_SyringeSuction->text();
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


void MainWindow::on_SuckPushButton_Suction_clicked()
{
    int number_mm = ui->MililitersToSuckSpinBox_Suction->value() * MM_TO_ML;
    int number = this->m_MCU_Data.syringe_set_position.toInt() + number_mm;
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
    ui->NewSetPositionSpinBox_SyringeSuction->setValue(number);
    this->m_mcuCommunication->sendMessage(_message);
}


void MainWindow::on_SettingsActionPage_triggered()
{
    ui->mainStackedWidget->setCurrentIndex(6);
    statusBar()->showMessage("Settings page");
}
//
// MM to ML ratio spin box - Settings - page 6
//
void MainWindow::on_mm_to_ml_spinBox_valueChanged(int arg1)
{
    MM_TO_ML = arg1;
}
//
// Injection Needle home position spin box - Settings - page 6
//
void MainWindow::on_InjectionNeedleHomePositionspinBox_valueChanged(int arg1)
{
    this->m_HomePosition_Needle_Injection = arg1;
}
//
// Injection Syringe home position spin box - Settings - page 6
//
void MainWindow::on_InjectionSyringeHomePositionSpinBox_valueChanged(int arg1)
{
    this->m_HomePosition_Syringe_Injection = arg1;
}
//
// Suction Needle home position spin box - Settings - page 6
//
void MainWindow::on_SuctionNeedleHomePositionspinBox_valueChanged(int arg1)
{
    this->m_HomePosition_Needle_Suction = arg1;
}
//
// Suction Syringe home position spin box - Settings - page 6
//
void MainWindow::on_SuctionSyringeHomePositionspinBox_valueChanged(int arg1)
{
    this->m_HomePosition_Syringe_Suction = arg1;
}
//
// Set Current home position injection needle push button - Settings - page 6
//
void MainWindow::on_SetCurrentInjectionNeedleHomePositionPushButton_clicked()
{
    this->m_HomePosition_Needle_Injection = this->m_MCU_Data.needle_position.toUInt();
    ui->InjectionNeedleHomePositionspinBox->setValue(this->m_HomePosition_Needle_Injection);
}
//
// Set Current home position injection syringe push button - Settings - page 6
//
void MainWindow::on_SetCurrentInjectionSyringeHomePositionPushButton_clicked()
{
    this->m_HomePosition_Syringe_Injection = this->m_MCU_Data.syringe_position.toUInt();
    ui->InjectionSyringeHomePositionSpinBox->setValue(this->m_HomePosition_Syringe_Injection);
}
//
// Set Current home position suction needle push button - Settings - page 6
//
void MainWindow::on_SetCurrentSuctionNeedleHomePositionPushButton_clicked()
{
    this->m_HomePosition_Needle_Suction = this->m_MCU_Data.needle_position.toUInt();
    ui->SuctionNeedleHomePositionspinBox->setValue(this->m_HomePosition_Needle_Suction);
}
//
// Set Current home position suction syringe push button - Settings - page 6
//
void MainWindow::on_SetCurrentSuctioSyringeHomePositionPushButton_clicked()
{
    this->m_HomePosition_Syringe_Suction = this->m_MCU_Data.syringe_position.toUInt();
    ui->SuctionSyringeHomePositionspinBox->setValue(this->m_HomePosition_Syringe_Suction);
}
//
// Send to MCU Needle speed push button - Settings - page 6
//
void MainWindow::on_SendToMCUNeedleSpeedPushButton_clicked()
{
    int number = ui->NeedleSpeedSpinBox->value();
    QByteArray _message;
    _message.append('Q');
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


void MainWindow::on_SendToMCUSyringeSpeedPushButton_clicked()
{
    int number = ui->SyringeSpeedSpinBox->value();
    QByteArray _message;
    _message.append('W');
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
// Info/about dialog
//
void MainWindow::on_actionAbout_triggered()
{
    this->m_infoDialog = new InfoDialog(this);
    this->m_infoDialog->show();
}

