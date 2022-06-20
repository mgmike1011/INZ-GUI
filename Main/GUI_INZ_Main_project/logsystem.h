#ifndef LOGSYSTEM_H
#define LOGSYSTEM_H

#include <QObject>
#include <QDateTime>
#include <QFile>

class LOGSystem : public QObject
{
private:
    Q_OBJECT
    QDateTime m_DateTime; // Current date and time
    QString m_TextMessage; // Text message that will be added to .txt file
    QFile m_file; // Output file
public:
    explicit LOGSystem(QObject *parent = nullptr, const QString &filepath="Outputfile.txt", const QString &name="");
    ~LOGSystem();
    void writeLOG(const QString &SetNeedlePosition, const QString &CurrentNeedlePosition,
                  const QString &SetSyringePosition, const QString &CurrentSyringePosition,const QString &Messages="None");
public slots:
    void writeLOG_slot(const QString &SetNeedlePosition, const QString &CurrentNeedlePosition,
                  const QString &SetSyringePosition, const QString &CurrentSyringePosition,const QString &Messages="None");
signals:
    void  writeLOG_success();
    void  writeLOG_error();
};

#endif // LOGSYSTEM_H
