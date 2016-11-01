#include "notificationtoolbar.h"
#include "../../theme.h"

#include <QGraphicsDropShadowEffect>

/**
 * @brief NotificationToolbar::NotificationToolbar
 * @param vc
 * @param parent
 */
NotificationToolbar::NotificationToolbar(ViewController* vc, QWidget *parent) :
    QToolBar(parent)
{
    toggleNotificationsDialog = vc->getActionController()->window_showNotifications;
    loadingGifDisplayed = false;

    setupLayout();
    themeChanged();

    connect(toggleNotificationsDialog, &QAction::triggered, this, &NotificationToolbar::notificationsSeen);
    connect(toggleNotificationsDialog, &QAction::triggered, this, &NotificationToolbar::toggleDialog);

    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationToolbar::themeChanged);
    connect(NotificationManager::manager(), &NotificationManager::notificationAlert, this, &NotificationToolbar::notificationReceived);
    connect(NotificationManager::manager(), &NotificationManager::notificationSeen, this, &NotificationToolbar::notificationsSeen);
    connect(NotificationManager::manager(), &NotificationManager::lastNotificationDeleted, this, &NotificationToolbar::lastNotificationDeleted);
}


/**
 * @brief NotificationToolbar::themeChanged
 */
void NotificationToolbar::themeChanged()
{
    Theme* theme = Theme::theme();
    QString borderRadiusLeft = "border-top-right-radius: 0px; border-bottom-right-radius: 0px; ";
    QString borderRadiusRight = "border-top-left-radius: 0px; border-bottom-left-radius: 0px;";

    setStyleSheet("QToolBar{ spacing: 0px; padding: 0px; border-radius: 4px; background:" + theme->getAltBackgroundColorHex() + ";}"
                  "QToolBar::separator{ width: 3px; background:" + theme->getBackgroundColorHex() + ";}"
                  "QToolButton{ padding: 2px; }"
                  "QToolButton#LEFT_ACTION{" + borderRadiusLeft + "}"
                  "QToolButton#RIGHT_ACTION{" + borderRadiusRight + "}"
                  "QLabel{ background: rgba(0,0,0,0); }");

    //defaultIcon = theme->getIcon("Actions", "Exclamation");
    //notificationIcon = theme->getIcon(lastNotification.iconPath, lastNotification.iconName);

    defaultIcon = theme->getIcon("Actions", "Timer");
    notificationIcon = theme->getIcon("Actions", "Exclamation");
    updateButtonIcon();
}


/**
 * @brief NotificationToolbar::notificationReceived
 * This slot is called when a new notification is received.
 * It highlights the showMostRecentAction and updates its icon.
 */
void NotificationToolbar::notificationReceived()
{
    // re-enable notification button
    if (!showMostRecentAction->isEnabled()) {
        showMostRecentAction->setEnabled(true);
    }

    // highlight notification button
    if (!showMostRecentAction->isCheckable()) {
        showMostRecentAction->setCheckable(true);
    }
    showMostRecentAction->setChecked(true);

    // update notification button's icon
    //notificationIcon = Theme::theme()->getIcon(iconPath, iconName);
    updateButtonIcon();
}


/**
 * @brief NotificationToolbar::notificationsSeen
 * This slot is called when either the showMostRecentActionn is triggered or when the dialog is opened.
 * It removes the highlight from the showMostRecentAction.
 */
void NotificationToolbar::notificationsSeen()
{
    if (showMostRecentAction->isCheckable()) {
        showMostRecentAction->setCheckable(false);
        showMostRecentAction->setChecked(false);
        updateButtonIcon();
    }
}


/**
 * @brief NotificationToolbar::lastNotificationDeleted
 * This slot disables the showMostRecentAction when there are no notifications in the dialog.
 */
void NotificationToolbar::lastNotificationDeleted()
{
    showMostRecentAction->setIcon(defaultIcon);
    showMostRecentAction->setEnabled(false);
}


/**
 * @brief NotificationToolbar::displayLoadingGif
 * This slot turns the loading gif on and off.
 * @param show
 */
void NotificationToolbar::displayLoadingGif(bool show)
{
    if (loadingGifDisplayed != show) {
        if (show) {
            connect(loadingGif, SIGNAL(frameChanged(int)), this, SLOT(updateIconFrame(int)));
        } else {
            disconnect(loadingGif, SIGNAL(frameChanged(int)), this, SLOT(updateIconFrame(int)));
            showMostRecentAction->setIcon(notificationIcon);
        }
        loadingGifDisplayed = show;
    }
}


/**
 * @brief NotificationToolbar::updateIconFrame
 */
void NotificationToolbar::updateIconFrame(int)
{
    showMostRecentAction->setIcon(QIcon(loadingGif->currentPixmap()));
}


/**
 * @brief NotificationToolbar::updateSeverityCount
 * This slot updates the number displayed on the notification label for the specified severity.
 * @param severity
 * @param count
 */
void NotificationToolbar::updateSeverityCount(NOTIFICATION_SEVERITY severity, int count)
{
    QLabel* countLabel = severityCount.value(severity, 0);
    if (countLabel) {
        countLabel->setText(QString::number(count));
    }
}


/**
 * @brief NotificationToolbar::setupLayout
 */
void NotificationToolbar::setupLayout()
{
    // create a label for the following severities of notifications
    severityCount[NS_WARNING] = new QLabel("0", this);
    severityCount[NS_ERROR] = new QLabel("0", this);

    showMostRecentAction = addAction("");
    showMostRecentAction->setToolTip("Show Most Recent Notification");
    showMostRecentAction->setCheckable(true);
    showMostRecentAction->setChecked(false);
    showMostRecentAction->setEnabled(false);
    addSeparator();

    connect(showMostRecentAction, &QAction::triggered, this, &NotificationToolbar::notificationsSeen);
    connect(showMostRecentAction, &QAction::triggered, NotificationManager::manager(), &NotificationManager::showLastNotification);

    QFont labelFont(QFont(font().family(), 11, 1));
    int labelWidth = 30;

    foreach (NOTIFICATION_SEVERITY s, NotificationManager::getNotificationSeverities()) {
        QLabel* label = severityCount.value(s, 0);
        if (label) {
            label->setFont(labelFont);
            label->setMinimumWidth(labelWidth);
            label->setAlignment(Qt::AlignCenter);
            label->setToolTip(NotificationManager::getNotificationSeverityString(s) + " Count");
            label->setStyleSheet("QLabel{ padding: 0px 5px; color:" + NotificationManager::getNotificationSeverityColorStr(s) + ";}");
            addWidget(label);
            addSeparator();

            // add shadow to the label's text
            QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);
            effect->setBlurRadius(0);
            effect->setColor(Theme::theme()->black());
            effect->setOffset(1,1);
            label->setGraphicsEffect(effect);
        }
    }

    addAction(toggleNotificationsDialog);

    // set object names for the two actions - used in the stylesheet
    widgetForAction(showMostRecentAction)->setObjectName("LEFT_ACTION");
    widgetForAction(toggleNotificationsDialog)->setObjectName("RIGHT_ACTION");

    QAction* testAction = addAction("T");
    testAction->setCheckable(true);
    testAction->setVisible(false);
    connect(testAction, &QAction::triggered, this, &NotificationToolbar::displayLoadingGif);

    loadingGif = new QMovie(this);
    loadingGif->setFileName(":/Actions/Waiting");
    loadingGif->start();
}


/**
 * @brief NotificationToolbar::updateButtonIcon
 * This updates the showMostRecentAction's icon based on whether
 * it is checked or not or if the loading gif is currently displayed.
 */
void NotificationToolbar::updateButtonIcon()
{
    if (!loadingGifDisplayed) {
        if (showMostRecentAction->isChecked()) {
            showMostRecentAction->setIcon(notificationIcon);
        } else {
            showMostRecentAction->setIcon(defaultIcon);
        }
    }
}

