#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QActionGroup>
#include <QSignalMapper>
#include <QToolBar>

#include "enumerations.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Views/Notification/notificationobject.h"

class NotificationDialog : public QDialog
{
    enum ITEM_ROLES{
        IR_ID = Qt::UserRole + 1,
        IR_SEVERITY = Qt::UserRole + 2,
        IR_ICONPATH = Qt::UserRole + 3,
        IR_ICONNAME = Qt::UserRole + 4,
        IR_ENTITYID = Qt::UserRole + 5,
        IR_TIMESTAMP = Qt::UserRole + 6
    };

    Q_OBJECT
public:
    explicit NotificationDialog(QWidget *parent = 0);

    void removeNotificationItem(int ID);

signals:
    void centerOn(int entityID);
    void popup(int entityID);

    void itemDeleted(int ID);

    void lastNotificationID(int ID);

    void updateSeverityCount(NOTIFICATION_SEVERITY severity, int count);

    void mouseEntered();

public slots:
    void toggleVisibility();
    void resetDialog();
    void getLastNotificationID();

private slots:
    void themeChanged();
    void listSelectionChanged();

    void severityActionToggled(int actionSeverity);

    void displaySelection();

    void clearSelected();
    void clearVisible();

    void notificationItemAdded(NotificationObject* item);
    void notificationItemClicked(QListWidgetItem* item);

private:
    void setupLayout();

    void updateVisibilityCount(int val, bool set = false);

    void constructNotificationItem(int ID, NOTIFICATION_SEVERITY severity, QString title, QString description, QString iconPath, QString iconName, int entityID);
    void removeItem(QListWidgetItem* item);

    void clearNotificationsOfSeverity(NOTIFICATION_SEVERITY severity);
    void clearAll();

    void updateSeverityActions(QList<NOTIFICATION_SEVERITY> severities);
    void updateSeverityAction(NOTIFICATION_SEVERITY severity);

    QAction* getSeverityAction(NOTIFICATION_SEVERITY severity) const;
    QPair<QString, QString> getActionIcon(NOTIFICATION_SEVERITY severity) const;

    QSignalMapper* severityActionMapper;

    QListWidget* listWidget;
    QToolBar* topToolbar;
    QToolBar* bottomToolbar;
    QToolBar* iconOnlyToolbar;

    QAction* sortTimeAction;
    QAction* sortSeverityAction;
    QAction* centerOnAction;
    QAction* popupAction;
    QAction* clearSelectedAction;
    QAction* clearVisibleAction;

    QHash<NOTIFICATION_SEVERITY, QAction*> severityActionHash;
    QMultiMap<NOTIFICATION_SEVERITY, QListWidgetItem*> notificationHash;
    QHash<int, QListWidgetItem*> notificationIDHash;

    int visibleCount;

protected:
    void enterEvent(QEvent* event);

};

#endif // NOTIFICATIONDIALOG_H
