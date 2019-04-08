#ifndef MEDEAEVENT_H
#define MEDEAEVENT_H

#include <QObject>
#include "protomessagestructs.h"

namespace MEDEA {

enum class ChartDataKind{DATA, PORT_LIFECYCLE, WORKLOAD, CPU_UTILISATION, MEMORY_UTILISATION, MARKER};

class Event : public QObject
{
    Q_OBJECT

public:
    // time should be in milliseconds since epoch
    explicit Event(ChartDataKind kind,
                   qint64 time,
                   const QString& name = "no_name",
                   QObject* parent = 0);

    ChartDataKind getKind() const;

    qint64 getTimeMS() const;
    QString getDateTimeString(const QString& format) const;

    const QString& getName() const;

    virtual const QString& getID() const = 0;
    virtual QString toString(const QString& dateTimeFormat) const = 0;

    static const QList<ChartDataKind>& GetChartDataKinds();
    static const QString& toString(ChartDataKind kind);
    static const QString& GetChartDataKindString(ChartDataKind kind);
    static const QString& GetChartDataKindStringSuffix(ChartDataKind kind);

private:
    const ChartDataKind kind_;
    const qint64 time_;
    const QString name_;

    const int eventID_;
    static int event_ID;
};

}

#endif // MEDEAEVENT_H
