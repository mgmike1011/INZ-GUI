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
    void messageReceivedJSONData_slot(const double &NP, const double &SP, const double &NS, const double &SS, const double &TM, const double &FN, const double &ST);
    void on_mainStackedWidget_currentChanged(int arg1);

    void on_stopMotorsPushButton_clicked();

    void on_disconnectPushButton_clicked();

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
    // Is working and sending data
    bool m_isMCUWorking;
    // MCU communication system
    MCUCommunication *m_mcuCommunication;
    // Licenses dialog system
    licensesDialog *m_licensesDialog;
    // Info from MCU
    struct
    {
        QString needle_position = 0;
        QString syringe_position = 0;
        QString needle_set_position = 0;
        QString syringe_set_position = 0;
        QString temperature = 0;
        QString fan = 0;
        QString status = 0;
    } m_MCU_Data;
};
#endif // MAINWINDOW_H
