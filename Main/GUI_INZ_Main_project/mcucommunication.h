#ifndef MCUCOMMUNICATION_H
#define MCUCOMMUNICATION_H

#include <QThread>
#include <QSerialPort>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
class MCUCommunication : public QThread
{
    Q_OBJECT
public:
    MCUCommunication(const QString &port_name);
    ~MCUCommunication();
signals:
    void messageReceived(const QString &message);
private:
// Members:
    QSerialPort *m_SerialPort;
    bool m_loop_condidtion;
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
    void run();
public:
    void sendMessage(const QByteArray &message);
    void stopWork();
};

#endif // MCUCOMMUNICATION_H
