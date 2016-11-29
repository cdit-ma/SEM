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

    int getID();
    int getEntityID();
    QString getIconPath();
    QString getIconName();
    NOTIFICATION_SEVERITY getSeverity();
    NOTIFICATION_TYPE2 getType();
    NOTIFICATION_CATEGORY getCategory();

    void setSelected(bool select);

signals:
    void itemClicked(NotificationItem* item, bool currentState, bool controlDown);

public slots:
    void themeChanged();

    void severityFilterToggled(QHash<NOTIFICATION_SEVERITY, bool> checkedStates);
    void typeFilterToggled(QHash<NOTIFICATION_TYPE2, bool> checkedStates);
    void categoryFilterToggled(QHash<NOTIFICATION_CATEGORY, bool> checkedStates);

protected:
    void mouseReleaseEvent(QMouseEvent* event);

private:
    void updateStyleSheet();

    QLabel* iconLabel;
    QLabel* descriptionLabel;

    NOTIFICATION_SEVERITY severity;
    NOTIFICATION_TYPE2 type;
    NOTIFICATION_CATEGORY category;

    QString backgroundColor;
    bool selected;
    //bool expanded;
};

#endif // NOTIFICATIONITEM_H
