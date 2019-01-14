#ifndef MEDEAEVENT_H
#define MEDEAEVENT_H

#include <QObject>
#include "protomessagestructs.h"

enum class TIMELINE_DATA_KIND{DATA, STATE, NOTIFICATION, LINE, BAR, PORT_LIFECYCLE, WORKLOAD, CPU_UTILISATION, MEMORY_UTILISATION};

static QList<TIMELINE_DATA_KIND> GET_TIMELINE_DATA_KINDS()
{
    return {TIMELINE_DATA_KIND::DATA,
            TIMELINE_DATA_KIND::STATE,
            TIMELINE_DATA_KIND::NOTIFICATION,
            TIMELINE_DATA_KIND::LINE,
            TIMELINE_DATA_KIND::BAR,
            TIMELINE_DATA_KIND::PORT_LIFECYCLE,
            TIMELINE_DATA_KIND::WORKLOAD,
            TIMELINE_DATA_KIND::CPU_UTILISATION,
            TIMELINE_DATA_KIND::MEMORY_UTILISATION};
}

static QString GET_TIMELINE_DATA_KIND_STRING(TIMELINE_DATA_KIND kind)
{
    switch (kind) {
    case TIMELINE_DATA_KIND::STATE:
        return "State";
    case TIMELINE_DATA_KIND::NOTIFICATION:
        return "Notification";
    case TIMELINE_DATA_KIND::LINE:
        return "Line";
    case TIMELINE_DATA_KIND::BAR:
        return "Bar";
    case TIMELINE_DATA_KIND::PORT_LIFECYCLE:
        return "PortLifecycle";
    case TIMELINE_DATA_KIND::WORKLOAD:
        return "Workload";
    case TIMELINE_DATA_KIND::CPU_UTILISATION:
        return "CPUUtilisation";
    case TIMELINE_DATA_KIND::MEMORY_UTILISATION:
        return "MemoryUtilisation";
    default:
        return "Data";
    }
}

static QString GET_TIMELINE_DATA_KIND_STRING_SUFFIX(TIMELINE_DATA_KIND kind)
{
    switch (kind) {
    case TIMELINE_DATA_KIND::CPU_UTILISATION:
        return "_cpu";
    case TIMELINE_DATA_KIND::MEMORY_UTILISATION:
        return "_mem";
    default:
        return "";
    }
}

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
    virtual TIMELINE_DATA_KIND getKind() const = 0;

private:
    qint64 time_;
    QString name_;
    int eventID_;

    static int event_ID;
};

}

#endif // MEDEAEVENT_H
