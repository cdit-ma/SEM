#ifndef EVENTFILTER_H
#define EVENTFILTER_H

#include <QObject>
#include <QEvent>

class EventFilter : public QObject
{
    Q_OBJECT
public:
    explicit EventFilter(QObject *parent = 0);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // EVENTFILTER_H
