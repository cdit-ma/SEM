#include "notificationtoolbar.h"
#include "../../theme.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"

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
    connect(NotificationManager::manager(), &NotificationManager::notificationAdded, this, &NotificationToolbar::notificationReceived);
    connect(NotificationManager::manager(), &NotificationManager::notificationSeen, this, &NotificationToolbar::notificationsSeen);
    connect(NotificationManager::manager(), &NotificationManager::lastNotificationDeleted, this, &NotificationToolbar::lastNotificationDeleted);
}


/**
 * @brief NotificationToolbar::themeChanged
 */
void NotificationToolbar::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QToolBar{ spacing: 0px; padding: 0px; border-radius: 4px; background:" + theme->getAltBackgroundColorHex() + ";}"
                  "QToolBar::separator{ width: 3px; background:" + theme->getBackgroundColorHex() + ";}"
                  "QToolButton{ border-top-left-radius: 0px; border-bottom-left-radius: 0px; padding: 2px; }"
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
 * It highlights the showMostRecentButton and updates its icon.
 */
void NotificationToolbar::notificationReceived()
{
    // re-enable notification button
    if (!showMostRecentButton->isEnabled()) {
        showMostRecentButton->setEnabled(true);
    }

    // highlight notification button
    if (!showMostRecentButton->isCheckable()) {
        showMostRecentButton->setCheckable(true);
        showMostRecentButton->setChecked(true);
    }

    // update notification button's icon
    //notificationIcon = Theme::theme()->getIcon(iconPath, iconName);
    updateButtonIcon();
}


/**
 * @brief NotificationToolbar::notificationsSeen
 * This slot is called when either the showMostRecentButton is triggered or when the dialog is opened.
 * It removes the highlight from the showMostRecentButton.
 */
void NotificationToolbar::notificationsSeen()
{
    if (showMostRecentButton->isCheckable()) {
        showMostRecentButton->setCheckable(false);
        showMostRecentButton->setChecked(false);
        updateButtonIcon();
    }
}


/**
 * @brief NotificationToolbar::lastNotificationDeleted
 * This slot disables the showMostRecentButton when there are no notifications in the dialog.
 */
void NotificationToolbar::lastNotificationDeleted()
{
    showMostRecentButton->setEnabled(false);
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
            showMostRecentButton->setIcon(notificationIcon);
        }
        loadingGifDisplayed = show;
    }
}


/**
 * @brief NotificationToolbar::updateIconFrame
 */
void NotificationToolbar::updateIconFrame(int)
{
    showMostRecentButton->setIcon(QIcon(loadingGif->currentPixmap()));
}


/**
 * @brief NotificationToolbar::updateTypeCount
 * This slot updates the number displayed on the notification label for the specified type.
 * @param type
 * @param count
 */
void NotificationToolbar::updateTypeCount(NOTIFICATION_TYPE type, int count)
{
    QLabel* countLabel = typeCount.value(type, 0);
    if (countLabel) {
        countLabel->setText(QString::number(count));
    }
}


/**
 * @brief NotificationToolbar::setupLayout
 */
void NotificationToolbar::setupLayout()
{
    // create a label for the following types of notifications
    typeCount[NT_WARNING] = new QLabel("0", this);
    typeCount[NT_ERROR] = new QLabel("0", this);

    showMostRecentButton = new QToolButton(this);
    showMostRecentButton->setToolTip("Show Most Recent Notification");
    showMostRecentButton->setStyleSheet("border-radius: 0px; border-top-left-radius: 4px; border-bottom-left-radius: 4px;");
    showMostRecentButton->setEnabled(false);

    showMostRecentAction = addWidget(showMostRecentButton);
    addSeparator();

    connect(showMostRecentButton, SIGNAL(clicked(bool)), showMostRecentAction, SLOT(trigger()));
    connect(showMostRecentAction, SIGNAL(triggered(bool)), this, SLOT(notificationsSeen()));
    connect(showMostRecentAction, SIGNAL(triggered(bool)), this, SIGNAL(showMostRecentNotification()));
    connect(showMostRecentAction, SIGNAL(toggled(bool)), showMostRecentButton, SLOT(setChecked(bool)));

    QFont labelFont(QFont(font().family(), 11, 1));
    int labelWidth = 30;

    foreach (NOTIFICATION_TYPE t, GET_NOTIFICATION_TYPES()) {
        QLabel* label = typeCount.value(t, 0);
        if (label) {
            label->setFont(labelFont);
            label->setMinimumWidth(labelWidth);
            label->setAlignment(Qt::AlignCenter);
            label->setToolTip(GET_NOTIFICATION_TYPE_STRING(t) + " Count");
            label->setStyleSheet("QLabel{ padding: 0px 5px; color:" + GET_NOTIFICATION_TYPE_COLORSTR(t) + ";}");
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
 * This updates the showMostRecentButton's icon based on whether
 * it is checked or not or if the loading gif is currently displayed.
 */
void NotificationToolbar::updateButtonIcon()
{
    if (!loadingGifDisplayed) {
        if (showMostRecentButton->isChecked()) {
            showMostRecentButton->setIcon(notificationIcon);
        } else {
            showMostRecentButton->setIcon(defaultIcon);
        }
    }
}

