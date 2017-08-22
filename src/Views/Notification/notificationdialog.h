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

    void notificationAdded(NotificationObject* obj);
    void notificationDeleted(int ID);
    void selectionChanged();

private slots:
    void themeChanged();
    void filtersChanged();

private:
    void toggleSort();
    void updateNotificationVisibility(QList<NotificationItem*> items);
    void setupLayout();
    void setupFilters();

    void removeItem(NotificationItem* item);

    void clearAll();
    void clearSelection();
    void updateVisibleCount();
private:
    QLabel* info_label = 0;
    QLabel* status_label = 0;
    QLabel* clock_label = 0;
    
    QWidget* filters_widget = 0;
    QWidget* notifications_widget = 0;
    QVBoxLayout* notifications_layout = 0;
    QVBoxLayout* filters_layout = 0;
    
    QWidget* notifications_status_widget = 0;

    QToolBar* top_toolbar = 0;
    QToolBar* bottom_toolbar = 0;

    QAction* center_action = 0;
    QAction* popup_action = 0;
    QAction* sort_time_action = 0;
    QAction* reset_filters_action = 0;

    
    QSplitter* splitter = 0;

    QScrollArea* filters_scroll = 0;
    QScrollArea* notifications_scroll = 0;

    OptionGroupBox* context_filters = 0;
    OptionGroupBox* severity_filters = 0;
    OptionGroupBox* category_filters = 0;
    OptionGroupBox* source_filters = 0;

    QHash<int, NotificationItem*> notification_items;
    NotificationItem* selected_notification = 0;
    ViewController* viewController = 0;
private:
    void ToggleSelection(NotificationItem* item);

protected:
    void enterEvent(QEvent* event);

};

#endif // NOTIFICATIONDIALOG_H
