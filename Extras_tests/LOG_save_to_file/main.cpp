#include <QCoreApplication>
#include <logsystem.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    LOGSystem *log = new LOGSystem(&a,"C:/Users/mgmil/Desktop/Output.txt");

    delete log;
    return a.exec();
}
