#ifndef MEDEAEVENT_H
#define MEDEAEVENT_H

#include <QObject>
#include "protoMessageStructs.h"

namespace MEDEA {

class Event : public QObject
{
    friend class EventSeries;
    Q_OBJECT

protected:
    // time should be in milliseconds since epoch
    explicit Event(qint64 time, QString name = "no_name", QObject* parent = 0);

public:
    const qint64& getTimeMS() const;
    const QString& getName() const;

    virtual QString getID() const;

private:
    qint64 time_;
    QString name_;
    int eventID_;

    static int event_ID;
};

}

#endif // MEDEAEVENT_H
