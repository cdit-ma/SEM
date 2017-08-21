#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QVBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QToolBar>
#include <QToolButton>
#include <QLabel>

#include "../../theme.h"
#include "../../enumerations.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Controllers/NotificationManager/notificationEnumerations.h"
#include "../../Utils/actiongroup.h"
#include "../../Utils/filtergroup.h"
#include "../../Widgets/optiongroupbox.h"

class ViewController;
class NotificationItem;
class NotificationDialog : public QFrame
{
    Q_OBJECT

public:
    explicit NotificationDialog(ViewController* controller, QWidget *parent = 0);

signals:
    void centerOn(int entityID);
    void popup(int entityID);

    void deleteNotification(int ID);
    void lastNotificationID(int ID);

    void mouseEntered();

    void itemHoverEnter(int ID);
    void itemHoverLeave(int ID);

public slots:
    void initialisePanel();
    void resetPanel();

    void notificationAdded(NotificationObject* obj);
    void notificationDeleted(int ID);
    void selectionChanged();

private slots:
    void themeChanged();
    void filtersChanged();

    void viewSelection();

    void clearSelected();
    void clearVisible();
    void clearNotifications(NOTIFICATION_FILTER filter, int filterVal);

private:
    void updateNotificationVisibility(QList<NotificationItem*> items);
    void setupLayout();
    void setupFilters();

    void removeItem(NotificationItem* item);

    void clearAll();
    void clearSelection();

    void updateSelectionBasedButtons();

private:
    QLabel* info_label = 0;
    
    QWidget* filters_widget = 0;
    QWidget* notifications_widget = 0;
    QVBoxLayout* notifications_layout = 0;
    QVBoxLayout* filters_layout = 0;

    QToolBar* top_toolbar = 0;

    QAction* center_action = 0;
    QAction* popup_action = 0;
    
    QSplitter* splitter = 0;

    QScrollArea* filters_scroll = 0;
    QScrollArea* notifications_scroll = 0;

    OptionGroupBox* context_filters = 0;
    OptionGroupBox* severity_filters = 0;
    OptionGroupBox* category_filters = 0;
    OptionGroupBox* source_filters = 0;

    QHash<int, NotificationItem*> notification_items;
private:
    void ToggleSelection(NotificationItem* item);

    QWidget* mainWidget;

    QSplitter* displaySplitter;
    QToolBar* filtersToolbar;
    QToolBar* topToolbar;
    QToolBar* bottomToolbar;

    QVBoxLayout* itemsLayout;

    CustomGroupBox* displayToggleBox;
    QToolButton* displayAllButton;
    QToolButton* displayLinkedItemsButton;

    QAction* sortTimeAction;
    QAction* sortSeverityAction;
    QAction* centerOnAction;
    QAction* popupAction;
    QAction* clearSelectedAction;
    QAction* clearVisibleAction;

    QHash<int, NotificationItem*> notificationItems;
    QHash<NOTIFICATION_FILTER, OptionGroupBox*> filters;

    
    NotificationItem* selected_notification = 0;
    //QSet<NotificationItem*> selected_notifications;

    QList<NotificationItem*> selectedNotificationItems;
    int selectedEntityID;

    ViewController* viewController = 0;

protected:
    void enterEvent(QEvent* event);

};

#endif // NOTIFICATIONDIALOG_H
