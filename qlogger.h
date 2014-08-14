#ifndef QLOGGER_H
#define QLOGGER_H
#include <QObject>



class QLogger:public QObject
{
    Q_OBJECT
public:
    QLogger();
signals:
    void debugPrint(QString output);
};


#endif // QLOGGER_H
