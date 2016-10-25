#ifndef NOTIFICATIONITEM_H
#define NOTIFICATIONITEM_H

#include <QObject>

class NotificationItem : public QObject
{
    Q_OBJECT
public:
    explicit NotificationItem(QObject *parent = 0);

signals:

public slots:
};

#endif // NOTIFICATIONITEM_H
