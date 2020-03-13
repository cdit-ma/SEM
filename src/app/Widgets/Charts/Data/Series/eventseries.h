#ifndef BASEEVENTSERIES_H
#define BASEEVENTSERIES_H

#include "../Events/event.h"

namespace MEDEA {

class EventSeries : public QObject
{
    Q_OBJECT

public:
    explicit EventSeries(const QString& ID, MEDEA::ChartDataKind kind = MEDEA::ChartDataKind::DATA, QObject* parent = nullptr);
    ~EventSeries() override;

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

    QList<Event*>::const_iterator getFirstAfterTime(qint64 timeMS) const;
    QList<Event*> getEventsBetween(qint64 fromTimeMS = -1, qint64 toTimeMS = -1) const;
	
	// TODO: Ask Mitch/Jackson why default parameters are prohibited on virtual or override methods
    virtual QString getHoveredDataString (
            qint64 fromTimeMS,
            qint64 toTimeMS,
            int numberOfItemsToDisplay,
            const QString& displayFormat) const;

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

    bool contains(Event* event) const;

    void addEventToList(Event& event);
    void updateTimeRange(qint64 new_timestamp);

private:
    QList<Event*> events_;
    QStringList event_ids_;

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

#endif // BASEEVENTSERIES_H