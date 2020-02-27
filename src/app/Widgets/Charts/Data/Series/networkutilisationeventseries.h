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
    static double trimDecimalPlaces(double number, int desired_decimal_places);
    static void rightPad(QString& str, int intended_length, char pad_char);
};

#endif // NETWORKUTILISATIONEVENTSERIES_H
