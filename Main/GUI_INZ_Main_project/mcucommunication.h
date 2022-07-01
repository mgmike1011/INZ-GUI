#ifndef MCUCOMMUNICATION_H
#define MCUCOMMUNICATION_H

#include <QThread>
#include <QSerialPort>
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
// Methods:
    void run();
public:
    void sendMessage(const QByteArray &message);
    void stopWork();
};

#endif // MCUCOMMUNICATION_H
