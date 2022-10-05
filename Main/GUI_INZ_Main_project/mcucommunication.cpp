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
    this->m_SerialPort->flush();
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
        this->m_buffor.clear();
        if(this->m_SerialPort->waitForReadyRead(100)){
            this->m_SerialPort->flush();
            this->m_buffor = this->m_SerialPort->readAll();
            // Wysłanie odberanej wiadomości do klasy głównej
            try {
                // JSON READ
                this->m_JSON_Document= QJsonDocument::fromJson(m_buffor);
                this->m_JSON_Object = this->m_JSON_Document.object();
                if (this->m_JSON_Object.isEmpty()) {
                    throw "Bad JSON";
                } else {
                    this->m_JSON_NP = this->m_JSON_Object.value(QString("NP"));
                    this->m_JSON_SP = this->m_JSON_Object.value(QString("SP"));
                    this->m_JSON_NS = this->m_JSON_Object.value(QString("NS"));
                    this->m_JSON_SS = this->m_JSON_Object.value(QString("SS"));
                    this->m_JSON_TM = this->m_JSON_Object.value(QString("TM"));
                    this->m_JSON_FN = this->m_JSON_Object.value(QString("FN"));
                    this->m_JSON_ST = this->m_JSON_Object.value(QString("ST"));
                    emit messageReceived(this->m_buffor);
                }
            } catch (...) {
                qWarning() << "Bad JSON!";
                this->m_buffor.clear();
                this->m_SerialPort->flush();
                this->m_SerialPort->clear();
            }

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
