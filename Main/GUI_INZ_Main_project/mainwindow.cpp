#include "mainwindow.h"
#include "./ui_mainwindow.h"
//
// Konstruktor
//
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusBar()->showMessage("Welcome - Start Page"); // Informacja na pasku dolnym
    ui->mainStackedWidget->setCurrentIndex(0); // Ustawienie początkowego ekranu
    this->m_isPathChosen = false; // Czy wybrany jest zapis do pliku Log
    this->m_isConnected = false;  // Czy jest połączony
    QSerialPortInfo info; // Dostępne porty COM
    this->m_info = info; // Dostępne porty COM
    auto portList = info.availablePorts();
    ui->serialPortComboBox->clear();
    for(auto &th:portList){ // Zapisanie dostępnych portów COM do Combo Box ekran 1
        ui->serialPortComboBox->addItem(th.portName());
        ui->portNameLineEdit->setText(th.serialNumber());
    }

}
//
// Destruktor
//
MainWindow::~MainWindow()
{
    delete ui;
    this->m_mcuCommunication->stopWork();
}
//
// Wywołanie strony początkowej nr 0
//
void MainWindow::on_StartActionPage_triggered()
{
    ui->mainStackedWidget->setCurrentIndex(0);
    statusBar()->showMessage("Start Page");
}
//
// Zamknięcie aplikacji przyciskiem Quit
//
void MainWindow::on_actionQuit_triggered()
{
    // Zamknięcie aplikacji
    int ret = QMessageBox::question(this, tr("Quit the application"),
                                    tr("Do you want to quit the application?"),
                                    QMessageBox::Ok | QMessageBox::Cancel);
    if(ret == QMessageBox::Ok){
        QApplication::quit();
    }
}
//
// Wywołanie ekranu Connection nr 1
//
void MainWindow::on_connectionActionPage_triggered()
{
    ui->mainStackedWidget->setCurrentIndex(1);
    statusBar()->showMessage("Connection Page");
}
//
// Wywołanie ekranu Connection nr 1 przyciskiem start
//
void MainWindow::on_startPushButton_page_1_Start_clicked()
{
    ui->mainStackedWidget->setCurrentIndex(1);
    statusBar()->showMessage("Connection Page");
}
//
// Wywołanie okna About Qt
//
void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}
//
// Obsługa przycisku Save ekran Connection nr 1
//
void MainWindow::on_savePathLogPushButton_clicked()
{
    if(ui->nameMCULineEdit->text().isEmpty()){
        QMessageBox::information(this,tr("Specify the name"),tr("To add the path you need to specify the MCU name first."));
    }else{
        this->m_PathLOGFile = QFileDialog::getSaveFileName(this,"Save file","/home/Output.txt","Text files (*.txt)");
        if(!m_PathLOGFile.isEmpty()){
            ui->saveFileLogLineEdit->setText(m_PathLOGFile);
            this->m_LOGSystem = new LOGSystem(this,m_PathLOGFile,ui->nameMCULineEdit->text());
            this->m_isPathChosen = true;
        }
    }
    if(this->m_isPathChosen && this->m_isConnected){
        ui->statusToDolabel->setText("");
        ui->DataActionPage->setEnabled(true);
        ui->dataPushButton->setEnabled(true);
    }else if(this->m_isPathChosen && !this->m_isConnected){
        ui->statusToDolabel->setText("Connect to the MCU.");
    }else if(! this->m_isPathChosen && this->m_isConnected){
        ui->statusToDolabel->setText("Select the path.");
    }

}

//
// Obsługa Refresh ekran Connection nr 1
//
void MainWindow::on_pushButton_2_clicked()
{
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
// Aktualizacja nazwy portu przy zmianie wyboru COM
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

        port->flush();
        port->write("T");
        // Musi przesłać i odebrać T
        port->flush();
        QByteArray buf;
        if(port->waitForReadyRead(10)){
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
            this->m_mcuCommunication->start();
            // Przeprowadzenie testu łączności
            this->m_mcuCommunication->sendMessage("T");
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
            this->m_LOGSystem->writeLOG("-","-","-","-","Disconnected to the MCU");
        }else if(! this->m_isPathChosen && this->m_isConnected){
            ui->statusToDolabel->setText("Select the path.");
        }else{
            ui->statusToDolabel->setText("Select the path and connect to the MCU.");
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
// Załączenie strony Connection
//
void MainWindow::on_connectionPushButton_clicked()
{
    ui->mainStackedWidget->setCurrentIndex(1);
}

