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
#include "mcucommunication.h"
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

private:
    Ui::MainWindow *ui;
    QString m_PathLOGFile;
    LOGSystem *m_LOGSystem;
    bool m_isConnected, m_isPathChosen;
    QSerialPortInfo m_info;
};
#endif // MAINWINDOW_H
