#include "event.h"


int MEDEA::Event::event_ID = 0;

/**
 * @brief MEDEA::Event::Event
 * @param time
 * @param name
 * @param parent
 */
MEDEA::Event::Event(qint64 time, QString name, QObject *parent)
    : QObject(parent)
{
    time_ = time;
    name_ = name;
    eventID_ = event_ID++;
}


/**
 * @brief MEDEA::Event::getTimeMS
 * @return
 */
const qint64& MEDEA::Event::getTimeMS() const
{
    return time_;
}


/**
 * @brief MEDEA::Event::getName
 * @return
 */
const QString& MEDEA::Event::getName() const
{
    return name_;
}


/**
 * @brief MEDEA::Event::getID
 * @return
 */
const QString& MEDEA::Event::getID()
{
    return QString::number(eventID_);
}
