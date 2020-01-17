#ifndef NETWORKUTILISATIONEVENTSERIES_H
#define NETWORKUTILISATIONEVENTSERIES_H

#include <cfloat>
#include "eventseries.h"
#include "../Events/networkutilisationevent.h"

class NetworkUtilisationEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit NetworkUtilisationEventSeries(const QString& ID, QObject* parent = nullptr);

    void addEvent(MEDEA::Event* event);

    static QString getByteString(qint64 bytes);

private:
    static double trimDecimalPlaces(double number, int desired_decimal_places);
    static void rightPad(QString& str, int intended_length, char pad_char);

    qint64 max_ = 0;
    qint64 total_sent_ = 0;
    qint64 total_received_ = 0;
};

#endif // NETWORKUTILISATIONEVENTSERIES_H
