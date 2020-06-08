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
	PORT_EVENT,
	NETWORK_UTILISATION
};

class Event : public QObject
{
    Q_OBJECT

public:
    // time should be in milliseconds since epoch
    explicit Event(ChartDataKind kind,
                   qint64 time,
                   QObject* parent = nullptr);

    ChartDataKind getKind() const;

    qint64 getTimeMS() const;
    QString getDateTimeString(const QString& format) const;

    virtual const QString& getID() const = 0;
    virtual QString toString(const QString& dateTimeFormat) const = 0;

    static const QList<ChartDataKind>& GetChartDataKinds();
    static const QString& GetChartDataKindString(ChartDataKind kind);
    static const QString& GetChartDataKindStringSuffix(ChartDataKind kind);

private:
	ChartDataKind kind_;
	qint64 time_;
};

inline uint qHash(ChartDataKind key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
}

Q_DECLARE_METATYPE(MEDEA::ChartDataKind)

#endif // MEDEAEVENT_H