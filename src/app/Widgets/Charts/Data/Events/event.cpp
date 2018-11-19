#include "event.h"


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
}


/**
 * @brief MEDEA::Event::getTimeMS
 * @return
 */
const qint64 MEDEA::Event::getTimeMS()
{
    return time_;
}


/**
 * @brief MEDEA::Event::getName
 * @return
 */
const QString MEDEA::Event::getName()
{
    return name_;
}
