#ifndef MCUCOMMUNICATION_H
#define MCUCOMMUNICATION_H

#include <QThread>
#include <QSerialPort>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
class MCUCommunication : public QObject
{
    Q_OBJECT
public:
    MCUCommunication(const QString &port_name);
    ~MCUCommunication();
signals:
    void messageReceived(const QString &message);
    void messageReceivedJSONData(const double &NP, const double &SP, const double &NS, const double &SS, const double &TM, const double &FN, const double &ST);
private:
// Members:
    QSerialPort *m_SerialPort;
    QByteArray m_buffor;
    QJsonDocument m_JSON_Document;
    QJsonObject m_JSON_Object;
    // JSON message members
    QJsonValue m_JSON_NP;
    QJsonValue m_JSON_SP;
    QJsonValue m_JSON_NS;
    QJsonValue m_JSON_SS;
    QJsonValue m_JSON_TM;
    QJsonValue m_JSON_FN;
    QJsonValue m_JSON_ST;
// Methods:
private slots:
    void MessageReceived_slot();
public:
    void sendMessage(const QByteArray &message);
};

#endif // MCUCOMMUNICATION_H
