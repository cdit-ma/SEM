#ifndef MARKEREVENTSERIES_H
#define MARKEREVENTSERIES_H

#include "eventseries.h"

class MarkerEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit MarkerEventSeries(QString ID, QObject* parent = 0);

    QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay, QString displayFormat);

};

#endif // MARKEREVENTSERIES_H
