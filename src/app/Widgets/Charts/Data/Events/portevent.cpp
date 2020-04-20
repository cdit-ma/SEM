#include "portevent.h"
#include <QDebug>
/**
 * @brief PortEvent::PortEvent
 * @param port
 * @param sequence_num
 * @param type
 * @param message
 * @param time
 * @param parent
 */
PortEvent::PortEvent(const AggServerResponse::Port& port,
                     quint64 sequence_num,
                     PortEventType type,
                     const QString& message,
                     qint64 time,
                     QObject* parent)
    : MEDEA::Event(MEDEA::ChartDataKind::PORT_EVENT, time, port.name, parent),
      type_(type),
      sequence_num_(sequence_num),
      series_id_(port.graphml_id),
      id_(port.graphml_id + getTypeString(type) + QString::number(time)),
      message_(message) {}


/**
 * @brief PortEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString PortEvent::toString(const QString& dateTimeFormat) const
{
    return getTypeString(type_) + " - " + getDateTimeString(dateTimeFormat) + "\n";
            //+ "Message: " + message_ + "\n";
}


/**
 * @brief PortEvent::getSeriesID
 * @return
 */
const QString& PortEvent::getSeriesID() const
{
    return series_id_;
}


/**
 * @brief PortEvent::getID
 * @return
 */
const QString& PortEvent::getID() const
{
    return id_;
}


/**
 * @brief PortEvent::getType
 * @return
 */
PortEvent::PortEventType PortEvent::getType() const
{
    return type_;
}


/**
 * @brief PortEvent::getTypeString
 * @param type
 * @return
 */
const QString& PortEvent::getTypeString(PortEvent::PortEventType type)
{
    switch (type) {
    case PortEvent::PortEventType::SENT: {
        static const QString sentStr = "SENT";
        return sentStr;
    }
    case PortEvent::PortEventType::IGNORED: {
        static const QString ignoredStr = "IGNORED";
        return ignoredStr;
    }
    case PortEvent::PortEventType::MESSAGE: {
        static const QString messageStr = "MESSAGE";
        return messageStr;
    }
    case PortEvent::PortEventType::RECEIVED: {
        static const QString receivedStr = "RECEIVED";
        return receivedStr;
    }
    case PortEvent::PortEventType::EXCEPTION: {
        static const QString exceptionStr = "EXCEPTION";
        return exceptionStr;
    }
    case PortEvent::PortEventType::STARTED_FUNC: {
        static const QString startedFuncStr = "STARTED_FUNC";
        return startedFuncStr;
    }
    case PortEvent::PortEventType::FINISHED_FUNC: {
        static const QString finishedFuncStr = "FINISHED_FUNC";
        return finishedFuncStr;
    }
    default: {
        static const QString defaultPortEventTypeStr = "UNKNOWN";
        return defaultPortEventTypeStr;
    }
    }
}
