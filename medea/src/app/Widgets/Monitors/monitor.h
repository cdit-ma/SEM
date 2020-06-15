#ifndef MONITOR_H
#define MONITOR_H

#include <QFrame>

#include "../../Controllers/NotificationManager/notificationenumerations.h"

class Monitor: public QFrame
{
    Q_OBJECT

public:
    Monitor(QWidget* widget) : QFrame(widget) {
        connect(this, &Monitor::StateChanged, [=](Notification::Severity state) {
            this->state = state;
        });
    };

    Notification::Severity getState() const {
        return state;
    }

signals:
    void Clear();
    void Abort();
    void Close();
    void AppendLine(QString text);
    void StateChanged(Notification::Severity state);

private:
    Notification::Severity state = Notification::Severity::RUNNING;
};

#endif // MONITOR_H