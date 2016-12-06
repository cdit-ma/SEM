#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
//#include <QActionGroup>
#include <QSignalMapper>
#include <QToolBar>
#include <QToolButton>
#include <QSplitter>
#include <QPushButton>
#include <QMenu>

//#include "enumerations.h"
#include "../../enumerations.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Utils/actiongroup.h"
#include "notificationobject.h"

class NotificationItem;
class NotificationDialog : public QDialog
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

    void itemDeleted(int ID);
    void lastNotificationID(int ID);

    void updateSeverityCount(NOTIFICATION_SEVERITY severity, int count);

    void mouseEntered();

    void filtersCleared();
    void filterCleared(NOTIFICATION_FILTER filter);
    void severityFiltersChanged(QHash<NOTIFICATION_SEVERITY, bool> states);
    void typeFiltersChanged(QHash<NOTIFICATION_TYPE2, bool> states);
    void categoryFiltersChanged(QHash<NOTIFICATION_CATEGORY, bool> states);

public slots:
    void initialiseDialog();
    void resetDialog();
    void showDialog();

    void toggleVisibility();
    void getLastNotificationID();

private slots:
    void themeChanged();

    void filterMenuTriggered(QAction* action);
    void filterToggled(bool checked);

    void updateSelection(NotificationItem* item, bool selected, bool controlDown);
    void viewSelection();

    void clearSelected();
    void clearVisible();
    void clearNotifications(NOTIFICATION_FILTER filter, int filterVal);

    void notificationItemAdded(NotificationObject* obj);
    void notificationItemDeleted(int ID, NOTIFICATION_SEVERITY severity);

    void backgroundProcess(bool inProgress, BACKGROUND_PROCESS process);

private:
    void setupLayout();
    void setupBackgroundProcessItems();

    QAction* constructFilterButtonAction(ITEM_ROLES role, int roleVal, QString label = "", QString iconPath = "", QString iconName = "", bool addToGroup = true);
    void setActionButtonChecked(QAction *action, bool checked);

    void removeItem(int ID);

    void clearAll();
    void clearSelection();

    void clearFilter(NOTIFICATION_FILTER filter);
    void clearFilters();

    void updateVisibilityCount(int val, bool set = false);

    void updateSeverityActions(QList<NOTIFICATION_SEVERITY> severities);
    void updateSeverityAction(NOTIFICATION_SEVERITY severity);

    NOTIFICATION_FILTER getNotificationFilter(ITEM_ROLES role);

    QToolButton* filtersButton;
    QMenu* filtersMenu;
    QToolBar* filtersToolbar;
    QToolBar* topToolbar;
    QToolBar* iconOnlyToolbar;
    QSplitter* displaySplitter;

    QVBoxLayout* processLayout;
    QVBoxLayout* itemsLayout;

    QAction* allAction;
    QAction* sortTimeAction;
    QAction* sortSeverityAction;
    QAction* centerOnAction;
    QAction* popupAction;
    QAction* clearSelectedAction;
    QAction* clearVisibleAction;

    QFrame* displayedSeparatorFrame;
    QFrame* displaySeparator;

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
    int visibleCount;
    int visibleProcessCount;

    QHash<int, NotificationItem*> notificationItems;
    QHash<BACKGROUND_PROCESS, QFrame*> backgroundProcesses;

protected:
    void enterEvent(QEvent* event);

};

#endif // NOTIFICATIONDIALOG_H
