#include "notificationtoolbar.h"
#include "../../Controllers/NotificationManager/notificationobject.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"

#include <QToolButton>
#include <QGraphicsDropShadowEffect>

/**
 * @brief NotificationToolbar::NotificationToolbar
 * @param parent
 */
NotificationToolbar::NotificationToolbar(QWidget *parent)
	: QToolBar(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setupLayout();
    updateCount();
    
    auto manager = NotificationManager::manager();
    connect(this, &NotificationToolbar::showPanel, manager, &NotificationManager::showNotificationPanel);
    
    //Enforce the button to always return to an unchecked state
    connect(show_most_recent_action, &QAction::triggered, [=](){show_most_recent_action->setChecked(!show_most_recent_action->isChecked());});
    connect(show_most_recent_action, &QAction::triggered, manager, &NotificationManager::toastLatestNotification);
    
    connect(manager, &NotificationManager::notificationUpdated, this, &NotificationToolbar::notificationAdded);
    connect(manager, &NotificationManager::notificationUpdated, this, &NotificationToolbar::updateCount);
    connect(manager, &NotificationManager::notificationDeleted, this, &NotificationToolbar::updateCount);
    connect(manager, &NotificationManager::notificationsSeen, this, &NotificationToolbar::notificationsSeen);
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationToolbar::themeChanged);
    themeChanged();
}

/**
 * @brief NotificationToolbar::themeChanged
 */
void NotificationToolbar::themeChanged()
{
    Theme* theme = Theme::theme();
    setIconSize(theme->getIconSize());

    auto border_radius = "border-radius: " + theme->getCornerRadius() + "; ";
    QString borderRadiusLeft = border_radius + "border-top-right-radius: 0px; border-bottom-right-radius: 0px; ";
    QString borderRadiusRight = border_radius + "border-top-left-radius: 0px; border-bottom-left-radius: 0px;";

    auto font = theme->getFont();
    auto width = theme->getIconSize().width() + 6 + QWidget::fontMetrics().horizontalAdvance(QString::number(max_count) + "+");
	
	setStyleSheet("QToolBar{spacing: 2px; background: none;padding:0px;}"
					"QToolButton{border-radius:none;padding:2px;}"
					"QToolButton#MID_ACTION{width:" + QString::number(width) + "px;}"
					"QToolButton#LEFT_ACTION{" + borderRadiusLeft + "}"
					"QToolButton#RIGHT_ACTION{" + borderRadiusRight + "}"
	);

    for (auto severity : Notification::getSeverities()) {
        auto severity_str = Notification::getSeverityString(severity);
        auto action = severity_actions.value(severity, nullptr);
        if (action) {
            QIcon icon;
            icon.addPixmap(theme->getImage("Notification", severity_str, QSize(), theme->getSeverityColor(severity)));
            icon.addPixmap(theme->getImage("Notification", severity_str, QSize(), theme->getMenuIconColor(ColorRole::SELECTED)), QIcon::Active);
            action->setIcon(icon);
        }
    }
    
    default_icon = theme->getIcon("ToggleIcons", "newNotification");
    updateButtonIcon();
}

/**
 * @brief NotificationToolbar::notificationReceived
 * This slot is called when a new notification is received.
 * It highlights the showMostRecentAction and updates its icon.
 */
void NotificationToolbar::notificationAdded()
{
    show_most_recent_action->setChecked(true);
    updateButtonIcon();
}

/**
 * @brief NotificationToolbar::notificationsSeen
 * This slot is called when either the showMostRecentAction is triggered, the dialog is opened or the most recent notification is deleted.
 * It removes the highlight from the showMostRecentAction.
 */
void NotificationToolbar::notificationsSeen()
{
    show_most_recent_action->setChecked(false);
    updateButtonIcon();
}

/**
 * @brief NotificationToolbar::setupLayout
 */
void NotificationToolbar::setupLayout()
{
    show_most_recent_action = addAction("Show most recent notification");
    show_most_recent_action->setCheckable(true);
    show_most_recent_action->setChecked(false);
    show_most_recent_action->setEnabled(false);
    
    auto severities = Notification::getSeverities().toList();
    std::sort(severities.begin(), severities.end());
    severities.removeAll(Notification::Severity::NONE);

    for (auto severity : severities) {
        auto action = addAction("");
        severity_actions[severity] = action;
        action->setToolTip("Number of " + Notification::getSeverityString(severity) + " notifications");
        connect(action, &QAction::triggered, [=](){emit showSeverity(severity);emit showPanel();});
        widgetForAction(action)->setObjectName("MID_ACTION");
    }
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    
    auto left_widget = (QToolButton*)widgetForAction(show_most_recent_action);
    left_widget->setToolButtonStyle(Qt::ToolButtonIconOnly);
    left_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    left_widget->setObjectName("LEFT_ACTION");
}

void NotificationToolbar::loadingGifTicked()
{
    if (loading_gif) {
        show_most_recent_action->setIcon(loading_gif->currentPixmap());
    }
}

void NotificationToolbar::setLoadingMode(bool loading)
{
    if (loading && !loading_gif) {
        loading_gif = Theme::theme()->getGif("Icons", "loading");
        connect(loading_gif, &QMovie::frameChanged, this, &NotificationToolbar::loadingGifTicked);
    } else if (!loading && loading_gif) {
        //Disconnect all signals from loading_gif to this
        disconnect(loading_gif, nullptr, this, nullptr);
        loading_gif = nullptr;
    }
}

/**
 * @brief NotificationToolbar::updateButtonIcon
 * This updates the showMostRecentAction's icon based on whether
 * it is checked or not or if the loading gif is currently displayed.
 */
void NotificationToolbar::updateButtonIcon()
{
    auto last_notification = NotificationManager::manager()->getLatestNotification();
    bool valid = !last_notification.isNull();
    if(valid && last_notification->getSeverity() == Notification::Severity::RUNNING){
        setLoadingMode(true);
    }else{
        setLoadingMode(false);
        show_most_recent_action->setIcon(default_icon);
    }
    show_most_recent_action->setEnabled(valid);
}

void NotificationToolbar::updateCount()
{
    QHash<Notification::Severity, int> severity_counts;
    for (const auto& notification : NotificationManager::manager()->getNotifications()) {
        auto severity = notification->getSeverity();
        severity_counts[severity] ++;
    }
    for (auto severity : Notification::getSeverities()) {
        auto action = severity_actions.value(severity, nullptr);
        auto severity_count = severity_counts.value(severity, 0);
        auto count = qMin(severity_count, max_count);
        if (action) {
            auto str = QString::number(count) + (count == max_count ? "+" : "");
            action->setText(str);
        }
    }
    updateButtonIcon();
}