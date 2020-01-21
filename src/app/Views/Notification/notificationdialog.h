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
    NotificationDialog(ViewController* viewController, QWidget *parent = nullptr);

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

    QLabel* info_label = nullptr;
    QLabel* status_label = nullptr;
    
    QWidget* filters_widget = nullptr;
    QWidget* notifications_widget = nullptr;
    QVBoxLayout* notifications_layout = nullptr;
    QVBoxLayout* filters_layout = nullptr;
    
    QWidget* notifications_status_widget = nullptr;

    QSet<QSharedPointer<NotificationObject> > filtered_notifications;

    QToolBar* top_toolbar = nullptr;
    QToolBar* bottom_toolbar = nullptr;
    QToolBar* left_toolbar = nullptr;

    QToolButton* load_more_button = nullptr;
    QAction* center_action = nullptr;
    QAction* popup_action = nullptr;
    QAction* clear_filtered_action = nullptr;
    QAction* sort_time_action = nullptr;
    QAction* clock_action = nullptr;
    QAction* reset_filters_action = nullptr;

    QSplitter* splitter = nullptr;

    QScrollArea* filters_scroll = nullptr;
    QScrollArea* notifications_scroll = nullptr;

    OptionGroupBox* context_filters = nullptr;
    OptionGroupBox* severity_filters = nullptr;
    OptionGroupBox* category_filters = nullptr;
    OptionGroupBox* source_filters = nullptr;

    int max_visible = 0;

    int total_notifications = 0;
    int current_matched_notifications = 0;
    int current_visible_notifications = 0;

    QHash<int, NotificationItem*> notification_items;
    NotificationItem* selected_notification = nullptr;
    ViewController* viewController = nullptr;

protected:
    void enterEvent(QEvent* event);

};

#endif // NOTIFICATIONDIALOG_H
