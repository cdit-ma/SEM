#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QToolButton>
#include <QComboBox>
#include <QActionGroup>
#include <QSignalMapper>
#include <QToolBar>

#include "enumerations.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Views/Notification/notificationitem.h"

class NotificationDialog : public QDialog
{
    enum ITEM_ROLES{
        IR_ID = Qt::UserRole + 1,
        IR_SEVERITY = Qt::UserRole + 2,
        IR_ICONPATH = Qt::UserRole + 3,
        IR_ICONNAME = Qt::UserRole + 4,
        IR_ENTITYID = Qt::UserRole + 5,
    };

    Q_OBJECT
public:
    explicit NotificationDialog(QWidget *parent = 0);

    void addNotificationItem(int ID, NotificationManager::NOTIFICATION_SEVERITY severity, QString title, QString description, QPair<QString, QString> iconPath, int entityID);
    void removeNotificationItem(int ID);

signals:
    void centerOn(int entityID);
    void itemDeleted(int ID);

    void lastNotificationID(int ID);

    void updateSeverityCount(NotificationManager::NOTIFICATION_SEVERITY severity, int count);

    void mouseEntered();

public slots:
    void toggleVisibility();
    void resetDialog();
    void getLastNotificationID();

private slots:
    void themeChanged();
    void listSelectionChanged();

    void severityActionToggled(int actionSeverity);

    void clearSelected();
    void clearVisible();

    void notificationItemAdded(NotificationItem* item);
    void notificationItemClicked(QListWidgetItem* item);

private:
    void setupLayout();

    void updateVisibilityCount(int val, bool set = false);

    void constructNotificationItem(int ID, NotificationManager::NOTIFICATION_SEVERITY severity, QString title, QString description, QString iconPath, QString iconName, int entityID);
    void removeItem(QListWidgetItem* item);

    void clearNotificationsOfSeverity(NotificationManager::NOTIFICATION_SEVERITY severity);
    void clearAll();

    void updateSeverityActions(QList<NotificationManager::NOTIFICATION_SEVERITY> severities);
    void updateSeverityAction(NotificationManager::NOTIFICATION_SEVERITY severity);

    QAction* getSeverityAction(NotificationManager::NOTIFICATION_SEVERITY severity) const;
    QPair<QString, QString> getActionIcon(NotificationManager::NOTIFICATION_SEVERITY severity) const;

    QSignalMapper* severityActionMapper;
    QListWidget* listWidget;
    QToolBar* topToolbar;
    QToolBar* bottomToolbar;
    QToolBar* iconOnlyToolbar;

    QAction* clearSelectedAction;
    QAction* clearVisibleAction;

    QHash<NotificationManager::NOTIFICATION_SEVERITY, QAction*> severityActionHash;
    QMultiMap<NotificationManager::NOTIFICATION_SEVERITY, QListWidgetItem*> notificationHash;
    QHash<int, QListWidgetItem*> notificationIDHash;

    int visibleCount;

protected:
    void enterEvent(QEvent* event);

};

#endif // NOTIFICATIONDIALOG_H
