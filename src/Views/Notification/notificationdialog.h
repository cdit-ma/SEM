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

#include "../../enumerations.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Utils/actiongroup.h"
#include "notificationobject.h"

class NotificationItem;
class NotificationDialog : public QWidget
{
    enum ITEM_ROLES{
        IR_ID = Qt::UserRole + 1,
        IR_ICONPATH = Qt::UserRole + 2,
        IR_ICONNAME = Qt::UserRole + 3,
        IR_ENTITYID = Qt::UserRole + 4,
        IR_TYPE = Qt::UserRole + 5,
        IR_CATEGORY = Qt::UserRole + 6,
        IR_SEVERITY = Qt::UserRole + 7,
        IR_TIMESTAMP = Qt::UserRole + 8
    };

    Q_OBJECT
public:
    explicit NotificationDialog(QWidget *parent = 0);

signals:
    void centerOn(int entityID);
    void popup(int entityID);

    void deleteNotification(int ID);
    void lastNotificationID(int ID);

    void updateSeverityCount(NOTIFICATION_SEVERITY severity, int count);

    void mouseEntered();

    void filtersCleared();
    void filterCleared(NOTIFICATION_FILTER filter);
    void severityFiltersChanged(QHash<NOTIFICATION_SEVERITY, bool> states);
    void typeFiltersChanged(QHash<NOTIFICATION_TYPE2, bool> states);
    void categoryFiltersChanged(QHash<NOTIFICATION_CATEGORY, bool> states);

public slots:
    void initialisePanel();
    void resetPanel();

    void getLastNotificationID();

    void testSlot(QStringList checkedList);

private slots:
    void themeChanged();

    void filterMenuTriggered(QAction* action);
    void filterToggled(bool checked);

    void updateSelection(NotificationItem* item, bool selected, bool controlDown);
    void viewSelection();

    void clearSelected();
    void clearVisible();
    void clearNotifications(NOTIFICATION_FILTER filter, int filterVal);

    void notificationAdded(NotificationObject* obj);
    void notificationDeleted(int ID);

    void backgroundProcess(bool inProgress, BACKGROUND_PROCESS process);

    void intervalTimeout();

private:
    void test();
    void setupLayout();
    void setupBackgroundProcessItems();

    QAction* constructFilterButtonAction(ITEM_ROLES role, int roleVal, QString label = "", QString iconPath = "", QString iconName = "", bool addToGroup = true);
    void setActionButtonChecked(QAction *action, bool checked);

    void updateSelectionBasedButtons();
    void blinkInfoLabel(bool blink = true);

    void removeItem(NotificationItem* item);

    void clearAll();
    void clearSelection();

    void clearFilter(NOTIFICATION_FILTER filter);
    void clearFilters();

    void updateSeverityActions(QList<NOTIFICATION_SEVERITY> severities);
    void updateSeverityAction(NOTIFICATION_SEVERITY severity);

    NOTIFICATION_FILTER getNotificationFilter(ITEM_ROLES role);

    QWidget* mainWidget;

    QToolButton* filtersButton;
    QMenu* filtersMenu;
    QToolBar* filtersToolbar;
    QToolBar* topToolbar;
    QToolBar* bottomToolbar;
    QSplitter* displaySplitter;
    QFrame* displayedSeparatorFrame;
    QFrame* displaySeparator;

    QLabel* infoLabel;
    QAction* infoAction;
    QTimer* blinkTimer;
    int intervalTime;

    QVBoxLayout* processLayout;
    QVBoxLayout* itemsLayout;

    QAction* allAction;
    QAction* sortTimeAction;
    QAction* sortSeverityAction;
    QAction* centerOnAction;
    QAction* popupAction;
    QAction* clearSelectedAction;
    QAction* clearVisibleAction;

    QList<ActionGroup*> actionGroups;
    QList<QAction*> groupSeparators;
    QHash<ITEM_ROLES, int> indexMap;

    QHash<ITEM_ROLES, ActionGroup*> filterGroups;
    QHash<QAction*, QToolButton*> filterButtonHash;

    QList<QAction*> checkedFilterActions;
    QList<NotificationItem*> selectedItems;

    QHash<NOTIFICATION_TYPE2, bool> typeCheckedStates;
    QHash<NOTIFICATION_CATEGORY, bool> categoryCheckedStates;
    QHash<NOTIFICATION_SEVERITY, bool> severityCheckedStates;

    QHash<NOTIFICATION_SEVERITY, int> severityItemsCount;
    int visibleProcessCount;

    QHash<int, NotificationItem*> notificationItems;
    QHash<BACKGROUND_PROCESS, QFrame*> backgroundProcesses;

protected:
    void enterEvent(QEvent* event);

};

#endif // NOTIFICATIONDIALOG_H
