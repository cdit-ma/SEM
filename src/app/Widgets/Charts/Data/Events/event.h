#ifndef MEDEAEVENT_H
#define MEDEAEVENT_H

#include <QObject>
#include "protomessagestructs.h"

enum class TIMELINE_EVENT_KIND{UNKNOWN, PORT_LIFECYCLE, WORKLOAD, CPU_UTILISATION, MEMORY_UTILISATION};

namespace MEDEA {

class Event : public QObject
{
    Q_OBJECT

public:
    // time should be in milliseconds since epoch
    explicit Event(qint64 time, QString name = "no_name", QObject* parent = 0);

    const qint64& getTimeMS() const;
    const QString& getName() const;

    QString getEventID() const;

    virtual QString getID() const = 0;
    virtual TIMELINE_EVENT_KIND getKind() const = 0;

private:
    qint64 time_;
    QString name_;
    int eventID_;

    static int event_ID;
};

}

#endif // MEDEAEVENT_H
