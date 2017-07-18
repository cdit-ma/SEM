#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSignalMapper>
#include <QToolBar>
#include <QToolButton>
#include <QSplitter>
#include <QPushButton>
#include <QMenu>
#include <QLabel>
#include <QTimer>

#include "../../theme.h"
#include "../../enumerations.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Controllers/NotificationManager/notificationEnumerations.h"
#include "../../Utils/actiongroup.h"
#include "../../Utils/filtergroup.h"

class NotificationItem;
class NotificationDialog : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationDialog(QWidget *parent = 0);

signals:
    void centerOn(int entityID);
    void popup(int entityID);

    void deleteNotification(int ID);
    void lastNotificationID(int ID);

    void filterCleared(NOTIFICATION_FILTER filter);
    void filtersChanged(NOTIFICATION_FILTER, QList<QVariant> checkedKeys);

    void mouseEntered();

public slots:
    void initialisePanel();
    void resetPanel();

    void notificationAdded(NotificationObject* obj);
    void notificationDeleted(int ID);

    void getLastNotificationID();

private slots:
    void on_themeChanged();
    void on_filtersChanged(QList<QVariant> checkedKeys);
    void on_selectionChanged(NotificationItem* item, bool selected, bool controlDown);

    void viewSelection();

    void clearSelected();
    void clearVisible();
    void clearNotifications(NOTIFICATION_FILTER filter, int filterVal);

    void intervalTimeout();

private:
    void setupLayout();
    void setupFilterGroups();

    void removeItem(NotificationItem* item);

    void clearAll();
    void clearSelection();

    void updateSelectionBasedButtons();
    void blinkInfoLabel(bool blink = true);
    
    QWidget* mainWidget;

    QToolBar* filtersToolbar;
    QToolBar* topToolbar;
    QToolBar* bottomToolbar;
    QSplitter* displaySplitter;

    QVBoxLayout* itemsLayout;

    QLabel* infoLabel;
    QAction* infoAction;
    QTimer* blinkTimer;
    int intervalTime;

    QAction* resetFiltersAction;
    QAction* sortTimeAction;
    QAction* sortSeverityAction;
    QAction* centerOnAction;
    QAction* popupAction;
    QAction* clearSelectedAction;
    QAction* clearVisibleAction;

    QHash<int, NotificationItem*> notificationItems;
    QList<NotificationItem*> selectedItems;

    QHash<NOTIFICATION_FILTER, QList<QVariant>> checkedFilterKeys;

protected:
    void enterEvent(QEvent* event);

};

#endif // NOTIFICATIONDIALOG_H
