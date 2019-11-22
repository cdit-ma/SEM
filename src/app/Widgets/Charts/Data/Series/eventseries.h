#ifndef BASESERIES_H
#define BASESERIES_H

#include "../Events/event.h"

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

    virtual void addEvent(Event* event) = 0;
    const QList<Event*>& getEvents() const;

    void setLabel(const QString& label);
    const QString& getLabel() const;

    qint64 getMinTimeMS() const;
    qint64 getMaxTimeMS() const;
    QPair<qint64, qint64> getTimeRangeMS() const;

    MEDEA::ChartDataKind getKind() const;

    const QString& getID() const;
    const QString& getEventSeriesID() const;

    qint64 getPreviousTime(qint64 time) const;
    qint64 getNextTime(qint64 time) const;

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
    void eventAdded(Event* event);
    void minTimeChanged(qint64 min);
    void maxTimeChanged(qint64 max);
    void minYValueChanged(double minY);
    void maxYValueChanged(double maxY);

protected:
    QString getDataString (
            const QList<Event*>::const_iterator& from,
            const QList<Event*>::const_iterator& to,
            int numberOfItems,
            const QString& dateTimeFormat) const;

    void addEventToList(Event& event);
    void updateTimeRange(qint64 new_timestamp);

    QList<Event*> events_;

private:
    QString ID_;
    QString label_;
    MEDEA::ChartDataKind kind_;

    qint64 minTime_;
    qint64 maxTime_;

    int eventSeriesID_;
    QString eventSeriesIDStr_;

    static std::atomic<int> eventSeries_ID;
};

}

#endif // BASESERIES_H
