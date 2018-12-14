#ifndef MEDEAEVENT_H
#define MEDEAEVENT_H

#include <QObject>
#include "protoMessageStructs.h"

enum class TIMELINE_EVENT_KIND{UNKNOWN, PORT_LIFECYCLE, CPU_UTILISATION, WORKLOAD};

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

    QString getID() const;

    virtual QString getEventID() const;
    virtual TIMELINE_EVENT_KIND getKind() const;

private:
    qint64 time_;
    QString name_;
    int eventID_;

    static int event_ID;
};

}

#endif // MEDEAEVENT_H
