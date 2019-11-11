#ifndef BASESERIES_H
#define BASESERIES_H

#include "../Events/event.h"

#include <QList>
#include <QDebug>

namespace MEDEA {

class EventSeries : public QObject
{
    Q_OBJECT

public:
    explicit EventSeries(const QString& ID, MEDEA::ChartDataKind kind = MEDEA::ChartDataKind::DATA, QObject* parent = nullptr);
    ~EventSeries();

    void clear();
    bool isEmpty() const;

    template<class T>
    void addEvents(const QVector<T>& events)
    {
        for (const auto& event : events) {
            addEvent(event);
        }
    }

    virtual void addEvent(Event* event);
    const QList<Event*>& getEvents() const;

    qint64 getMinTimeMS() const;
    qint64 getMaxTimeMS() const;
    QPair<qint64, qint64> getTimeRangeMS() const;

    MEDEA::ChartDataKind getKind() const;

    const QString& getID() const;
    const QString& getEventSeriesID() const;

    QList<Event*>::const_iterator getFirstAfterTime(const qint64 timeMS) const;
    QList<Event*> getEventsBetween(qint64 fromTimeMS = -1, qint64 toTimeMS = -1) const;

    virtual QString getHoveredDataString (
            qint64 fromTimeMS,
            qint64 toTimeMS,
            int numberOfItemsToDisplay = getDefaultNumberOfItemsToDisplay(),
            const QString& displayFormat = getDefaultDisplayFormat()) const;

    static const QString& getDefaultDisplayFormat();
    static int getDefaultNumberOfItemsToDisplay();

signals:
    void minChanged(qint64 min);
    void maxChanged(qint64 max);
    void minYValueChanged(double minY);
    void maxYValueChanged(double maxY);

protected:
    QString getDataString (
            const QList<Event*>::const_iterator& from,
            const QList<Event*>::const_iterator& to,
            int numberOfItems,
            const QString& dateTimeFormat) const;

private:
    QString ID_;
    MEDEA::ChartDataKind kind_;

    qint64 minTime_;
    qint64 maxTime_;

    QList<Event*> events_;

    int eventSeriesID_;
    QString eventSeriesIDStr_;

    static std::atomic<int> eventSeries_ID;
};

}

#endif // BASESERIES_H
