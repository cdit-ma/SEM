#ifndef BASESERIES_H
#define BASESERIES_H

#include "../Events/event.h"

#include <QDateTime>
#include <QTextStream>

namespace MEDEA {

class EventSeries : public QObject
{
    Q_OBJECT

public:
    explicit EventSeries(QString ID, TIMELINE_DATA_KIND kind = TIMELINE_DATA_KIND::DATA, QObject* parent = 0);

    void clear();

    void addEvent(Event* event);
    void addEvents(QList<Event*>& events);
    const QList<Event*>& getEvents();

    const qint64& getMinTimeMS() const;
    const qint64& getMaxTimeMS() const;
    QPair<qint64, qint64> getTimeRangeMS() const;

    const TIMELINE_DATA_KIND& getKind() const;

    QString getEventSeriesID() const;
    const QString& getID() const;

    QString getHoveredDataString(QPair<qint64, qint64> timeRangeMS, int numberOfItemsToDisplay = getDefaultNumberOfItemsToDisplay(), QString displayFormat = getDefaultDisplayFormat());
    virtual QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay = getDefaultNumberOfItemsToDisplay(), QString displayFormat = getDefaultDisplayFormat());

    static QString getDefaultDisplayFormat();
    static int getDefaultNumberOfItemsToDisplay();

private:
    QString ID_;
    TIMELINE_DATA_KIND kind_;

    qint64 minTime_;
    qint64 maxTime_;

    QList<Event*> events_;

    int eventSeriesID_;
    static int eventSeries_ID;
};

}



#endif // BASESERIES_H
