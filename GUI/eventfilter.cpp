#include "eventfilter.h"

EventFilter::EventFilter(QObject *parent) :
    QObject(parent) {}

bool EventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        //qDebug("Filtered Mouse Press");
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
}
