#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QVBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QToolBar>
#include <QLabel>

#include "../../Widgets/optiongroupbox.h"
#include "../../Controllers/NotificationManager/notificationenumerations.h"

class ViewController;
class NotificationItem;
class NotificationObject;

class NotificationDialog : public QFrame
{
    Q_OBJECT

public:
    NotificationDialog(ViewController* viewController, QWidget *parent = 0);

signals:
    void mouseEntered();

public slots:
    void showSeveritySelection(Notification::Severity severity);
    void showSeverity(Notification::Severity severity);

private slots:
    void notificationAdded(QSharedPointer<NotificationObject> notification);
    void notificationDeleted(QSharedPointer<NotificationObject> notification);

    void themeChanged();
    void selectionChanged();
    void filtersChanged();
    void popupEntity();
    void centerEntity();
    void flashEntity(int ID);
    void selectAndCenterEntity(int ID);
    void clearFilteredNotifications();

private:
    NotificationItem* constructNotificationItem(QSharedPointer<NotificationObject> notification);
    NotificationItem* getNotificationItem(QSharedPointer<NotificationObject> notification);
    QSet<QSharedPointer<NotificationObject> > getFilteredNotifications();

    void notificationItemClicked(NotificationItem* item);

    void initialisePanel();
    void updateNotificationsVisibility();
    
    void setupLayout();
    void setupFilters();

    void clearSelection();

    void scrollBarValueChanged();
    void loadNextResults();
    void updateLabels();

    QLabel* info_label = 0;
    QLabel* status_label = 0;
    
    QWidget* filters_widget = 0;
    QWidget* notifications_widget = 0;
    QVBoxLayout* notifications_layout = 0;
    QVBoxLayout* filters_layout = 0;
    
    QWidget* notifications_status_widget = 0;

    QSet<QSharedPointer<NotificationObject> > filtered_notifications;

    QToolBar* top_toolbar = 0;
    QToolBar* bottom_toolbar = 0;
    QToolBar* left_toolbar = 0;

    QToolButton* load_more_button = 0;
    QAction* center_action = 0;
    QAction* popup_action = 0;
    QAction* clear_filtered_action = 0;
    QAction* sort_time_action = 0;
    QAction* clock_action = 0;
    QAction* reset_filters_action = 0;

    QSplitter* splitter = 0;

    QScrollArea* filters_scroll = 0;
    QScrollArea* notifications_scroll = 0;

    OptionGroupBox* context_filters = 0;
    OptionGroupBox* severity_filters = 0;
    OptionGroupBox* category_filters = 0;
    OptionGroupBox* source_filters = 0;

    int current_visible = 0;
    int filtered_match_count = 0;

    int max_visible = 0;

    int total_notifications = 0;
    int current_matched_notifications = 0;
    int current_visible_notifications = 0;

    int notification_count = 0;

    QHash<int, NotificationItem*> notification_items;
    NotificationItem* selected_notification = 0;
    ViewController* viewController = 0;

protected:
    void enterEvent(QEvent* event);

};

#endif // NOTIFICATIONDIALOG_H
