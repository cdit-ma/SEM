#include "notificationtoolbar.h"
#include "notificationobject.h"
#include "../../theme.h"

#include <QGraphicsDropShadowEffect>

/**
 * @brief NotificationToolbar::NotificationToolbar
 * @param vc
 * @param parent
 */
NotificationToolbar::NotificationToolbar(QWidget *parent) :
    QToolBar(parent)
{
    //toggleNotificationsDialog = vc->getActionController()->window_showNotifications;
    loadingGifDisplayed = false;

    setupLayout();
    themeChanged();
    initialiseToolbar();

    //connect(toggleNotificationsDialog, &QAction::triggered, this, &NotificationToolbar::notificationsSeen);
    //connect(toggleNotificationsDialog, &QAction::triggered, this, &NotificationToolbar::toggleDialog);
    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationToolbar::themeChanged);
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
                  "QToolButton#RIGHT_ACTION{" + borderRadiusRight + "}");

    //defaultIcon = theme->getIcon("Actions", "Exclamation");
    //notificationIcon = theme->getIcon(lastNotification.iconPath, lastNotification.iconName);

    defaultIcon = theme->getIcon("Icons", "clock");
    notificationIcon = theme->getIcon("Icons", "exclamation");
    toggleNotificationsDialog->setIcon(theme->getIcon("Icons", "popOut"));
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
 * This slot is called when either the showMostRecentAction is triggered, the dialog is opened or the most recent notification is deleted.
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
void NotificationToolbar::updateSeverityCount(Notification::Severity severity, int count)
{
    QLabel* countLabel = severityCount.value(severity, 0);
    if (countLabel) {
        countLabel->setText(QString::number(count));
        //setMinimumWidth(sizeHint().width());
    }
}


/**
 * @brief NotificationToolbar::setupLayout
 */
void NotificationToolbar::setupLayout()
{
    // create a label for the following severities of notifications
    severityCount[Notification::Severity::WARNING] = new QLabel("0", this);
    severityCount[Notification::Severity::ERROR] = new QLabel("0", this);

    showMostRecentAction = addAction("");
    showMostRecentAction->setToolTip("Show Most Recent Notification");
    showMostRecentAction->setCheckable(true);
    showMostRecentAction->setChecked(false);
    showMostRecentAction->setEnabled(false);
    addSeparator();

    connect(showMostRecentAction, &QAction::triggered, this, &NotificationToolbar::notificationsSeen);
    connect(showMostRecentAction, &QAction::triggered, this, &NotificationToolbar::showLastNotification);

    QFont labelFont(QFont(font().family(), 11, 1));
    int labelWidth = 30;

    for(auto s : Notification::getSeverities()) {
        QLabel* label = severityCount.value(s, 0);
        if (label) {
            label->setFont(labelFont);
            label->setMinimumWidth(labelWidth);
            label->setAlignment(Qt::AlignCenter);
            label->setToolTip(Notification::getSeverityString(s) + " Count");
            auto color = Theme::QColorToHex(Notification::getSeverityColor(s));
            label->setStyleSheet("QLabel{ background: rgba(0,0,0,0); padding: 0px 5px; color:" + color + ";}");
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

    toggleNotificationsDialog = new QAction(this);
    toggleNotificationsDialog->setToolTip("Show Notifications Panel");
    connect(toggleNotificationsDialog, &QAction::triggered, this, &NotificationToolbar::notificationsSeen);
    connect(toggleNotificationsDialog, &QAction::triggered, this, &NotificationToolbar::toggleDialog);
    addAction(toggleNotificationsDialog);

    // set object names for the two actions - used in the stylesheet
    widgetForAction(showMostRecentAction)->setObjectName("LEFT_ACTION");
    widgetForAction(toggleNotificationsDialog)->setObjectName("RIGHT_ACTION");

    loadingGif = new QMovie(this);
    loadingGif->setFileName(":/Actions/Waiting");
    loadingGif->start();
}


/**
 * @brief NotificationToolbar::initialiseToolbar
 * Check if any notifications were received before this toolbar was constructed.
 * Update the displayed warning/error count and highlight showMostRecent button if necessary.
 */
void NotificationToolbar::initialiseToolbar()
{
    QList<NotificationObject*> notifications = NotificationManager::manager()->getNotificationItems();
    if (!notifications.isEmpty()) {
        QHash<Notification::Severity, int> severityCount;
        foreach (NotificationObject* obj, notifications) {
            Notification::Severity s = obj->severity();
            int count = severityCount.value(s,0);
            severityCount[s] = count + 1;
        }
        foreach (Notification::Severity s, severityCount.keys()) {
            updateSeverityCount(s, severityCount.value(s));
        }
        notificationReceived();
    } else {
        notificationsSeen();
    }
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
