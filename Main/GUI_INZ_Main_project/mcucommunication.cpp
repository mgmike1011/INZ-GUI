#include "mcucommunication.h"
#include <QDebug>
//
// Construcotr
//
MCUCommunication::MCUCommunication(const QString &port_name)
{
    this->m_SerialPort = new QSerialPort(this);
    this->m_SerialPort->setPortName(port_name);
    this->m_SerialPort->setBaudRate(QSerialPort::Baud115200);
    this->m_SerialPort->setDataBits(QSerialPort::Data8);
    this->m_SerialPort->setParity(QSerialPort::NoParity);
    this->m_SerialPort->setStopBits(QSerialPort::OneStop);
    this->m_SerialPort->setFlowControl(QSerialPort::NoFlowControl);
    this->m_SerialPort->open(QIODevice::ReadWrite);
    connect(this->m_SerialPort,&QSerialPort::readyRead, this ,&MCUCommunication::MessageReceived_slot);
    this->m_SerialPort->flush();
}
//
// Destructor
//
MCUCommunication::~MCUCommunication()
{
    this->sendMessage("EXXX");
    this->m_SerialPort->close();
    delete this->m_SerialPort;
}

//
// Main function - received message
//
void MCUCommunication::MessageReceived_slot()
{
        while (this->m_SerialPort->canReadLine()) {
            this->m_buffor = this->m_SerialPort->readLine();
            // Read JSON data from message
            try {
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
                    emit messageReceivedJSONData(this->m_JSON_NP.toDouble(),this->m_JSON_SP.toDouble(),this->m_JSON_NS.toDouble(),this->m_JSON_SS.toDouble(),
                                                 this->m_JSON_TM.toDouble(),this->m_JSON_FN.toDouble(),this->m_JSON_ST.toDouble());
                }
            } catch (...) {
                qWarning() << "Bad JSON!";
                qInfo() << this->m_buffor;
                this->m_buffor.clear();
                this->m_SerialPort->flush();
                this->m_SerialPort->clear();
                this->sendMessage("EEEE");
                this->sendMessage("OEEE");
            }

        }

}

//
// Write message slot
//
void MCUCommunication::sendMessage(const QByteArray &message)
{
    this->m_SerialPort->write(message);
}
