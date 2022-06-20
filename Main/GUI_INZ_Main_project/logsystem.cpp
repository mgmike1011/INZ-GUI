#include "logsystem.h"

LOGSystem::LOGSystem(QObject *parent, const QString &filepath, const QString &name): QObject{parent}, m_file(filepath)
{
    if (m_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
        this->m_DateTime = QDateTime::currentDateTime();
        QTextStream stream(&m_file);
        this->m_TextMessage = "####################";
        stream << m_TextMessage << "\n";
        this->m_TextMessage = this->m_DateTime.toString(Qt::DateFormat::ISODate) + " " + name;
        stream << m_TextMessage << "\n";
        this->m_TextMessage = "LOG CSV(divider ';') format:";
        stream << m_TextMessage << "\n";
        this->m_TextMessage = "Date;Time;SetNeedlePosition;CurrentNeedlePosition;SetSygingePosition;CurrentSyringePosition;Messages";
        stream << m_TextMessage << "\n";
        this->m_TextMessage = "####################";
        stream << m_TextMessage << "\n";
        emit this->writeLOG_success();
    }else{
        emit this->writeLOG_error();
    }
}

LOGSystem::~LOGSystem()
{
    this->m_file.close();
}

void LOGSystem::writeLOG(const QString &SetNeedlePosition, const QString &CurrentNeedlePosition,
                         const QString &SetSyringePosition, const QString &CurrentSyringePosition,const QString &Messages)
{
    this->m_DateTime = QDateTime::currentDateTime();
    this->m_TextMessage = m_DateTime.date().toString(Qt::DateFormat::ISODate)+";"+m_DateTime.time().toString(Qt::DateFormat::ISODate)+";"
            +SetNeedlePosition+";"+CurrentNeedlePosition+";"+SetSyringePosition+";"+CurrentSyringePosition+";"+Messages;
    QTextStream stream(&m_file);
    stream << m_TextMessage << "\n";
    emit this->writeLOG_success();
}

void LOGSystem::writeLOG_slot(const QString &SetNeedlePosition, const QString &CurrentNeedlePosition, const QString &SetSyringePosition, const QString &CurrentSyringePosition, const QString &Messages)
{
    this->m_DateTime = QDateTime::currentDateTime();
    this->m_TextMessage = m_DateTime.date().toString(Qt::DateFormat::ISODate)+";"+m_DateTime.time().toString(Qt::DateFormat::ISODate)+";"
            +SetNeedlePosition+";"+CurrentNeedlePosition+";"+SetSyringePosition+";"+CurrentSyringePosition+";"+Messages;
    QTextStream stream(&m_file);
    stream << m_TextMessage << "\n";
    emit this->writeLOG_success();
}

