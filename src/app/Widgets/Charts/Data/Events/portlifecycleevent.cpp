#include "portlifecycleevent.h"

/**
 * @brief PortLifecycleEvent::PortLifecycleEvent
 * @param port
 * @param type
 * @param time
 * @param parent
 */
PortLifecycleEvent::PortLifecycleEvent(const AggServerResponse::Port& port,
                                       AggServerResponse::LifecycleType type,
                                       qint64 time,
                                       QObject* parent)
	: MEDEA::Event(MEDEA::ChartDataKind::PORT_LIFECYCLE, time, parent),
	  type_(type),
	  id_(port.graphml_id + getTypeString(type) + QString::number(time)) {}

/**
 * @brief PortLifecycleEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString PortLifecycleEvent::toString(const QString& dateTimeFormat) const
{
    return getTypeString(type_) + " - " + getDateTimeString(dateTimeFormat) + "\n";
}

/**
 * @brief PortLifecycleEvent::getID
 * @return
 */
const QString& PortLifecycleEvent::getID() const
{
    return id_;
}

/**
 * @brief PortLifecycleEvent::getType
 * @return
 */
AggServerResponse::LifecycleType PortLifecycleEvent::getType() const
{
    return type_;
}

/**
 * @brief PortLifecycleEvent::getTypeString
 * @param type
 * @return
 */
const QString& PortLifecycleEvent::getTypeString(AggServerResponse::LifecycleType type)
{
	switch (type) {
		case AggServerResponse::LifecycleType::CONFIGURE: {
			static QString configureStr = "CONFIGURE";
			return configureStr;
		}
		case AggServerResponse::LifecycleType::ACTIVATE: {
			static QString activateStr = "ACTIVATE";
			return activateStr;
		}
		case AggServerResponse::LifecycleType::PASSIVATE: {
			static QString passivateStr = "PASSIVATE";
			return passivateStr;
		}
		case AggServerResponse::LifecycleType::TERMINATE: {
			static QString terminateStr = "TERMINATE";
			return terminateStr;
		}
		default: {
			static const QString defaultPortLifecycleTypeStr = "UNKNOWN";
			return defaultPortLifecycleTypeStr;
		}
	}
}