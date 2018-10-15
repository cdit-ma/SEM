#ifndef NOTIFICATIONSERIES_H
#define NOTIFICATIONSERIES_H

#include "dataseries.h"
#include <QPointF>
namespace MEDEA {

class NotificationSeries : public DataSeries
{
    Q_OBJECT

public:
    enum NOTIFICATION_TYPE{INFO, WARNING, ERROR, SUCCESS};

    explicit NotificationSeries(ViewItem* item = 0);
    ~NotificationSeries();

    void addTime(double time, NOTIFICATION_TYPE type);
    QList<QPair<double, NOTIFICATION_TYPE> > getTimes();

private:
    QList<QPair<double, NOTIFICATION_TYPE>> _times;
};

}

#endif // NOTIFICATIONSERIES_H
