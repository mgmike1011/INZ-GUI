#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusBar()->showMessage("Welcome - Start Page");
    ui->mainStackedWidget->setCurrentIndex(0);
    this->m_isPathChosen = false;
    this->m_isConnected = false;
    QSerialPortInfo info;
    this->m_info = info;
    auto portList = info.availablePorts();
    ui->serialPortComboBox->clear();
    for(auto &th:portList){
        ui->serialPortComboBox->addItem(th.portName());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_StartActionPage_triggered()
{
    ui->mainStackedWidget->setCurrentIndex(0);
    statusBar()->showMessage("Start Page");
}


void MainWindow::on_actionQuit_triggered()
{
    // Zamknięcie aplikacji
    int ret = QMessageBox::question(this, "Quit the application",
                                    "Do you want to quit the application?",
                                    QMessageBox::Ok | QMessageBox::Cancel);
    if(ret == QMessageBox::Ok){
        QApplication::quit();
    }
}


void MainWindow::on_connectionActionPage_triggered()
{
    ui->mainStackedWidget->setCurrentIndex(1);
    statusBar()->showMessage("Connection Page");
}


void MainWindow::on_startPushButton_page_1_Start_clicked()
{
    ui->mainStackedWidget->setCurrentIndex(1);
    statusBar()->showMessage("Connection Page");
}


void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}


void MainWindow::on_savePathLogPushButton_clicked()
{
    if(ui->nameMCULineEdit->text().isEmpty()){
        QMessageBox::information(this,"Specify the name","To add the path you need to specify the MCU name first.");
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


void MainWindow::on_pushButton_2_clicked()
{
    QSerialPortInfo info;
    auto portList = info.availablePorts();
    this->m_info = info;
    ui->serialPortComboBox->clear();
    for(auto &th:portList){
        ui->serialPortComboBox->addItem(th.serialNumber());
    }
}


void MainWindow::on_serialPortComboBox_currentTextChanged(const QString &arg1)
{
    auto portList = m_info.availablePorts();
    for(auto &th:portList){
        if(th.serialNumber() == arg1){
            ui->portNameLineEdit->setText(th.portName());
        }
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    if(! ui->portNameLineEdit->text().isEmpty()){
        statusBar()->showMessage("Testing");
        QSerialPort port;
        port.setPortName(ui->portNameLineEdit->text());
        port.setBaudRate(QSerialPort::Baud115200);
        port.setDataBits(QSerialPort::Data8);
        port.setParity(QSerialPort::NoParity);
        port.setStopBits(QSerialPort::OneStop);
        port.setFlowControl(QSerialPort::NoFlowControl);
        port.open(QIODevice::ReadWrite);
        //    port.flush();
        port.write("T");
//        Musi przesłać i odebrać T
        //    port.flush();
        QByteArray buf;
        buf = port.readAll();
        //    port.flush();
        port.close();
        if(buf.toStdString() == "T"){
            ui->testLabel->setText("Test: Success");
        }else{
            ui->testLabel->setText("Test: Error");
        }
        statusBar()->showMessage("Connection Page");
    }else{
        QMessageBox::critical(this,"No port selected","To perform a test select a port");
    }
}

