#ifndef MEDEAEVENT_H
#define MEDEAEVENT_H

#include <QObject>
#include "../protomessagestructs.h"

namespace MEDEA {

enum class ChartDataKind {
	DATA,
	PORT_LIFECYCLE,
	WORKLOAD,
	CPU_UTILISATION,
	MEMORY_UTILISATION,
	MARKER,
	PORT_EVENT
};

class Event : public QObject
{
    Q_OBJECT

public:
    // time should be in milliseconds since epoch
    explicit Event(ChartDataKind kind,
                   qint64 time,
                   QString name = "no_name",
                   QObject* parent = nullptr);

    ChartDataKind getKind() const;

    qint64 getTimeMS() const;
    QString getDateTimeString(const QString& format) const;

    const QString& getSeriesName() const;

    // The getSeriesID is used to group the events together
    // Combined with the ChartDataKind and experiment_run_id, it identifies the series that the event belongs to
    virtual const QString& getSeriesID() const = 0;
    virtual const QString& getID() const = 0;
    virtual QString toString(const QString& dateTimeFormat) const = 0;

    static const QList<ChartDataKind>& GetChartDataKinds();
    static const QString& GetChartDataKindString(ChartDataKind kind);
    static const QString& GetChartDataKindStringSuffix(ChartDataKind kind);

private:
    ChartDataKind kind_;
    qint64 time_;
    QString series_name_;
};

}

#endif // MEDEAEVENT_H