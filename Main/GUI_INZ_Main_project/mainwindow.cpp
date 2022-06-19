#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusBar()->showMessage("Welcome - Start Page");
    ui->mainStackedWidget->setCurrentIndex(0);

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

