#include "notificationmanager.h"
#include "../WindowManager/windowmanager.h"
#include "../../Widgets/Windows/mainwindow.h"
#include "theme.h"

/**
 * @brief NotificationManager::NotificationManager
 * @param vc
 * @param parent
 */
NotificationManager::NotificationManager(ViewController *vc, QWidget *parent) : QObject(parent)
{
    viewController = vc;
    toggleNotificationsDialog = vc->getActionController()->window_showNotifications;

    //notificationDialog = new NotificationDialog(WindowManager::manager()->getMainWindow());

    parentWidget = parent;
    notificationDialog = new NotificationDialog(parentWidget);

    loadingGifDisplayed = false;

    constructNotificationWidget();
    themeChanged();

    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationManager::themeChanged);
    connect(viewController, &ViewController::vc_showNotification, this, &NotificationManager::notificationReceived);
    connect(viewController, &ViewController::vc_setupModel, notificationDialog, &NotificationDialog::resetDialog);
    connect(toggleNotificationsDialog, &QAction::triggered, notificationDialog, &NotificationDialog::toggleVisibility);
    connect(toggleNotificationsDialog, &QAction::triggered, this, &NotificationManager::notificationsSeen);
    connect(notificationDialog, &NotificationDialog::updateTypeCount, this, &NotificationManager::updateTypeCount);
    connect(notificationDialog, &NotificationDialog::itemDeleted, this, &NotificationManager::deleteNotification);
    connect(notificationDialog, &NotificationDialog::centerOn, viewController, &ViewController::centerOnID);
}


/**
 * @brief NotificationManager::getNotificationWidget
 * @return
 */
QToolBar* NotificationManager::getNotificationWidget()
{
    return notificationWidget;
}


/**
 * @brief NotificationManager::themeChanged
 */
void NotificationManager::themeChanged()
{
    Theme* theme = Theme::theme();
    notificationWidget->setStyleSheet("QToolBar{ padding: 0px; }");
    notificationIcon = theme->getIcon("Actions", "Exclamation");
    if (!loadingGifDisplayed) {
        showLastNotificationButton->setIcon(notificationIcon);
    }
}


/**
 * @brief NotificationManager::notificationReceived
 * @param type
 * @param title
 * @param description
 * @param iconPath
 * @param iconName
 * @param ID
 */
void NotificationManager::notificationReceived(NOTIFICATION_TYPE type, QString title, QString description, QString iconPath, QString iconName, int ID)
{
    if (notifications.contains(ID)) {
        // TODO - At the moment, some (or all?) notifications with the same type share the same ID
        //return;
    }

    // store notification in hash
    Notification n;
    n.type = type;
    n.title = title;
    n.description = description;
    n.iconPath = iconPath;
    n.iconName = iconName;
    n.ID = ID;
    notifications[ID] = n;
    lastNotification = n;

    // add notification to dialog
    notificationDialog->addNotificationItem(type, title, description, QPair<QString, QString>(iconPath, iconName), ID);

    // highlight notification button
    if (!showLastNotificationButton->isCheckable()) {
        showLastNotificationButton->setCheckable(true);
        showLastNotificationButton->setChecked(true);
    }

    // send signal to main window to display notification toast
    emit notificationAdded(iconPath, iconName, description);
}


/**
 * @brief NotificationManager::notificationsSeen
 * Remove highlight from the notification button.
 */
void NotificationManager::notificationsSeen()
{
    if (showLastNotificationButton->isCheckable()) {
        showLastNotificationButton->setCheckable(false);
        showLastNotificationButton->setChecked(false);
    }
}


/**
 * @brief NotificationManager::showLastNotification
 * Send a signal to the main window to show the last notification (last item in the dialog's list).
 */
void NotificationManager::showLastNotification()
{
    if (!notifications.isEmpty()) {
        emit notificationAdded(lastNotification.iconPath, lastNotification.iconName, lastNotification.description);
    }
}


/**
 * @brief NotificationManager::deleteNotification
 * Remove notification with the provided ID from the hash and the notification dialog.
 * @param ID
 */
void NotificationManager::deleteNotification(int ID)
{
    if (notifications.contains(ID)) {        

        // if the sender object is the dialog, it means that the item has already been removed from it
        if (QObject::sender() != notificationDialog) {
            notificationDialog->removeNotificationItem(ID);
        }

        // remove from hash
        notifications.remove(ID);

        // update last notification
        if ((lastNotification.ID == ID) && !notifications.isEmpty()) {
            lastNotification = notifications.values().last();
        }
    }
}


/**
 * @brief NotificationManager::updateTypeCount
 * @param type
 * @param count
 */
void NotificationManager::updateTypeCount(NOTIFICATION_TYPE type, int count)
{
    QLabel* countLabel = typeCount.value(type, 0);
    if (countLabel) {
        countLabel->setText(QString::number(count));
    }
}


/**
 * @brief NotificationManager::displayLoadingGif
 * @param show
 */
void NotificationManager::displayLoadingGif(bool show)
{
    if (loadingGifDisplayed != show) {
        if (show) {
            connect(loadingGif, SIGNAL(frameChanged(int)), this, SLOT(updateIconFrame(int)));
        } else {
            disconnect(loadingGif, SIGNAL(frameChanged(int)), this, SLOT(updateIconFrame(int)));
            showLastNotificationButton->setIcon(notificationIcon);
        }
        loadingGifDisplayed = show;
    }
}


/**
 * @brief NotificationManager::updateIconFrame
 * @param frame
 */
void NotificationManager::updateIconFrame(int)
{
    showLastNotificationButton->setIcon(QIcon(loadingGif->currentPixmap()));
}


/**
 * @brief NotificationManager::constructNotificationWidget
 */
void NotificationManager::constructNotificationWidget()
{
    notificationWidget = new QToolBar(parentWidget);
    notificationWidget->setIconSize(QSize(20, 20));

    // create a label for the following types of notifications
    typeCount[NT_WARNING] = new QLabel("0", parentWidget);
    typeCount[NT_ERROR] = new QLabel("0", parentWidget);
    typeCount[NT_CRITICAL] = new QLabel("0", parentWidget);

    showLastNotificationButton = new QToolButton(parentWidget);
    showLastNotificationButton->setToolTip("Show Last Notification");
    showLastNotificationButton->setStyleSheet("border-radius: 0px; border-top-left-radius: 4px; border-bottom-left-radius: 4px;");

    showLastNotificationAction = notificationWidget->addWidget(showLastNotificationButton);

    connect(showLastNotificationButton, SIGNAL(clicked(bool)), showLastNotificationAction, SLOT(trigger()));
    connect(showLastNotificationAction, SIGNAL(triggered(bool)), this, SLOT(notificationsSeen()));
    connect(showLastNotificationAction, SIGNAL(triggered(bool)), this, SLOT(showLastNotification()));
    connect(showLastNotificationAction, SIGNAL(toggled(bool)), showLastNotificationButton, SLOT(setChecked(bool)));

    QFont labelFont(QFont(parentWidget->font().family(), 12));
    int labelWidth = 25;

    foreach (NOTIFICATION_TYPE t, GET_NOTIFICATION_TYPES()) {
        QLabel* label = typeCount.value(t, 0);
        if (label) {
            label->setFont(labelFont);
            label->setMinimumWidth(labelWidth);
            label->setAlignment(Qt::AlignCenter);
            label->setToolTip(GET_NOTIFICATION_TYPE_STRING(t) + " Count");
            label->setStyleSheet("QLabel{ color:" + GET_NOTIFICATION_TYPE_COLORSTR(t) + ";}");
            notificationWidget->addWidget(label);
        }
    }

    notificationWidget->addAction(toggleNotificationsDialog);

    QAction* testAction = notificationWidget->addAction("T");
    testAction->setCheckable(true);
    testAction->setVisible(false);
    connect(testAction, &QAction::triggered, this, &NotificationManager::displayLoadingGif);

    loadingGif = new QMovie(this);
    loadingGif->setFileName(":/Actions/Waiting");
    loadingGif->start();
}

