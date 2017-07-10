#ifndef NOTIFICATIONITEM_H
#define NOTIFICATIONITEM_H

#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "notificationobject.h"

#include <QFrame>
#include <QLabel>
#include <QMovie>

class NotificationItem : public QFrame
{
    Q_OBJECT
public:
    explicit NotificationItem(NotificationObject* obj, QWidget* parent = 0);

    int getID();
    int getEntityID();
    QString getIconPath();
    QString getIconName();
    NOTIFICATION_SEVERITY getSeverity();
    NOTIFICATION_TYPE getType();
    NOTIFICATION_CATEGORY getCategory();

    void setSelected(bool select);

signals:
    void itemClicked(NotificationItem* item, bool currentState, bool controlDown);

public slots:
    void themeChanged();

    void showItem();
    void filterCleared(NOTIFICATION_FILTER filter);
    void severityFilterToggled(QHash<NOTIFICATION_SEVERITY, bool> checkedStates);
    void typeFilterToggled(QHash<NOTIFICATION_TYPE, bool> checkedStates);
    void categoryFilterToggled(QHash<NOTIFICATION_CATEGORY, bool> checkedStates);

private slots:
    void descriptionChanged(QString description);
    void iconChanged(QString iconPath, QString iconName);
    void loading(bool on);

protected:
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent();

private:
    void updateIcon();
    void updateStyleSheet();
    void updateVisibility(NOTIFICATION_FILTER filter = NOTIFICATION_FILTER::NOFILTER, bool visible = false);

    NotificationObject* notificationObject;

    QString _iconPath;
    QString _iconName;

    QLabel* iconLabel;
    QLabel* descriptionLabel;

    QMovie* loadingGif;
    bool loadingOn;

    QString backgroundColor;
    bool selected;

    QHash<NOTIFICATION_FILTER, bool> filterVisibility;

};

#endif // NOTIFICATIONITEM_H
