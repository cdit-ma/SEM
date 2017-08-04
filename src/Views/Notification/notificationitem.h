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

    void addFilter(QVariant filter, QVariant filterVal);
    void filtersChanged(QVariant filter, QVariant filterVal);

    int getNotificationFilterValue(NOTIFICATION_FILTER filter);
    void filtersChanged(NOTIFICATION_FILTER filter, bool filterMatched);

signals:
    void hoverEnter(int ID);
    void hoverLeave(int ID);
    void itemClicked(NotificationItem* item, bool currentState, bool controlDown);

public slots:
    void themeChanged();

private slots:
    void descriptionChanged(QString description);
    void iconChanged(QString iconPath, QString iconName);
    void loading(bool on);

protected:
    void mouseReleaseEvent(QMouseEvent* event);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    void updateIcon();
    void updateStyleSheet();
    void updateVisibility(bool filterMatched);

    NotificationObject* notificationObject;

    QString _iconPath;
    QString _iconName;

    QLabel* iconLabel;
    QLabel* descriptionLabel;

    QMovie* loadingGif;
    bool loadingOn;

    QString backgroundColor;
    bool selected;

    QHash<QVariant, QVariant> customFilters;
    QHash<QVariant, bool> customFilterVisibility;
    QHash<NOTIFICATION_FILTER, bool> filterVisibility;
};

#endif // NOTIFICATIONITEM_H
