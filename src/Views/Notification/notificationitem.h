#ifndef NOTIFICATIONITEM_H
#define NOTIFICATIONITEM_H

#include "../../Controllers/NotificationManager/notificationmanager.h"

#include <QFrame>
#include <QLabel>

class NotificationItem : public QFrame
{
    Q_OBJECT
public:
    explicit NotificationItem(int ID,
                              QString description,
                              QString iconPath,
                              QString iconName,
                              int eID,
                              NOTIFICATION_SEVERITY s,
                              NOTIFICATION_TYPE2 t,
                              NOTIFICATION_CATEGORY c,
                              QWidget *parent = 0);

signals:
    void itemSelected(int ID, bool append = false);

public slots:
    void themeChanged();
    void filterButtonToggled(NOTIFICATION_FILTER filter, int filterVal, bool checked);

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    //void enterEvent(QEvent *event);
    //void leaveEvent(QEvent *event);

private:
    void setSelected(bool select, bool append = false);
    void updateStyleSheet();

    QLabel* iconLabel;
    QLabel* descriptionLabel;

    NOTIFICATION_SEVERITY severity;
    NOTIFICATION_TYPE2 type;
    NOTIFICATION_CATEGORY category;

    QString backgroundColor;
    bool expanded;
    bool selected;
};

#endif // NOTIFICATIONITEM_H
