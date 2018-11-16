#ifndef MEDEAEVENT_H
#define MEDEAEVENT_H

#include <QObject>

namespace MEDEA {

class Event : public QObject
{
    Q_OBJECT

public:
    explicit Event(qint64 time, QString name = "", QObject* parent = 0);

    const qint64 getTime();
    const QString getName();

private:
    qint64 time_;
    QString name_;
};

}

#endif // MEDEAEVENT_H
