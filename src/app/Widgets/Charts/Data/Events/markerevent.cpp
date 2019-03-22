#include "markerevent.h"


/**
 * @brief MarkerEvent::MarkerEvent
 * @param markerName
 * @param markerID
 * @param inst
 * @param time
 * @param parent
 */
MarkerEvent::MarkerEvent(QString markerName, qint64 markerID, AggServerResponse::ComponentInstance inst, qint64 time, QObject *parent)
    : MEDEA::Event(time, markerName, parent)
{
    componentInstance_ = inst;
    markerID_ = markerID;
}


/**
 * @brief MarkerEvent::getMarkerID
 * @return
 */
qint64 MarkerEvent::getMarkerID() const
{
    return markerID_;
}


/**
 * @brief MarkerEvent::getID
 * @return
 */
QString MarkerEvent::getID() const
{
    return getName();
    //return componentInstance_.graphml_id + getName() + markerID_;
}


/**
 * @brief MarkerEvent::getKind
 * @return
 */
TIMELINE_DATA_KIND MarkerEvent::getKind() const
{
    return TIMELINE_DATA_KIND::MARKER;
}

