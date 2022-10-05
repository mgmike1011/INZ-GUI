#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include "logsystem.h"
#include <QSerialPortInfo>
#include <QSerialPort>
#include <algorithm>
#include <QThread>
#include <mcucommunication.h>
#include "licensesdialog.h"
#include <iostream>

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
    void on_StartActionPage_triggered();
    void on_actionQuit_triggered();
    void on_connectionActionPage_triggered();
    void on_startPushButton_page_1_Start_clicked();
    void on_actionAbout_Qt_triggered();
    void on_savePathLogPushButton_clicked();
    void on_pushButton_2_clicked();
    void on_serialPortComboBox_currentTextChanged(const QString &arg1);
    void on_pushButton_3_clicked();
    void on_actionCopy_serial_port_name_triggered();
    void on_pushButton_4_clicked();
    // Communication with MCU
    void messageReceived_slot(const QString &message);
    void on_actionCopy_path_to_Log_file_triggered();
    void on_actionLicenses_triggered();
    void on_dataPushButton_clicked();
    void on_connectionPushButton_clicked();
    void on_DataActionPage_triggered();

private:
    // Main window UI form
    Ui::MainWindow *ui;
    // Path for saving the log file
    QString m_PathLOGFile;
    // Log system
    LOGSystem *m_LOGSystem;
    // Chcecking if the path is chosen and the MCU is connected
    bool m_isConnected, m_isPathChosen;
    // Serial port communication - port selection
    QSerialPortInfo m_info;
    // MCU communication system
    MCUCommunication *m_mcuCommunication;
    // Licenses dialog system
    licensesDialog *m_licensesDialog;
};
#endif // MAINWINDOW_H
