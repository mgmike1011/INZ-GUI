#include "mcucommunication.h"
#include <QDebug>
//
// Konstruktor
//
MCUCommunication::MCUCommunication(const QString &port_name)
{
    this->m_loop_condidtion = true;
    this->m_SerialPort = new QSerialPort(this);
    this->m_SerialPort->setPortName(port_name);
    this->m_SerialPort->setBaudRate(QSerialPort::Baud115200);
    this->m_SerialPort->setDataBits(QSerialPort::Data8);
    this->m_SerialPort->setParity(QSerialPort::NoParity);
    this->m_SerialPort->setStopBits(QSerialPort::OneStop);
    this->m_SerialPort->setFlowControl(QSerialPort::NoFlowControl);
    this->m_SerialPort->open(QIODevice::ReadWrite);
}
//
// Destruktor
//
MCUCommunication::~MCUCommunication()
{
    this->m_SerialPort->close();
    delete this->m_SerialPort;
}
//
// Główna funckja i pętla
//
void MCUCommunication::run()
{
    while (this->m_loop_condidtion) {
        this->m_SerialPort->flush();
        if(this->m_SerialPort->waitForReadyRead(100)){
            this->m_buffor = this->m_SerialPort->readAll();
            // Wysłanie odberanej wiadomości do klasy głównej
            emit messageReceived(this->m_buffor);
        }
        this->m_SerialPort->flush();
    }
    this->m_SerialPort->close();
}
//
// Slot do wysłania wiadomości
//
void MCUCommunication::sendMessage(const QByteArray &message)
{
    this->m_SerialPort->write(message);
}

void MCUCommunication::stopWork()
{
    this->m_loop_condidtion = false;
}
