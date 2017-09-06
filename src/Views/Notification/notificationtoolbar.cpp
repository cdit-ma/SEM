#include "notificationtoolbar.h"
#include "../../Controllers/NotificationManager/notificationobject.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
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
    setupLayout();
    updateCount();
    
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationToolbar::themeChanged);
    
    auto manager = NotificationManager::manager();
    connect(show_notification_dialog_action, &QAction::triggered, manager, &NotificationManager::showNotificationPanel);
    connect(show_most_recent_action, &QAction::triggered, manager, &NotificationManager::toastLatestNotification);
    //Enforce the button to always return to an unchecked state
    connect(show_most_recent_action, &QAction::triggered, [=](){show_most_recent_action->setChecked(!show_most_recent_action->isChecked());});


    connect(manager, &NotificationManager::notificationUpdated, this, &NotificationToolbar::notificationAdded);
    connect(manager, &NotificationManager::notificationUpdated, this, &NotificationToolbar::updateCount);
    connect(manager, &NotificationManager::notificationDeleted, this, &NotificationToolbar::updateCount);
    
    connect(manager, &NotificationManager::notificationsSeen, this, &NotificationToolbar::notificationsSeen);

    
    themeChanged();
}


/**
 * @brief NotificationToolbar::themeChanged
 */
void NotificationToolbar::themeChanged()
{
    Theme* theme = Theme::theme();
    QString borderRadiusLeft = "border-top-right-radius: 0px; border-bottom-right-radius: 0px; ";
    QString borderRadiusRight = "border-top-left-radius: 0px; border-bottom-left-radius: 0px;";

    setStyleSheet("QToolBar{ spacing: 0px; padding: 0px; border-radius: 5px; background:" + theme->getAltBackgroundColorHex() + ";}"
                  "QToolBar::separator{ width: 2px; background:" + theme->getBackgroundColorHex() + ";}"
                  "QToolButton{ padding: 2px; }"
                  "QToolButton#LEFT_ACTION{" + borderRadiusLeft + "}"
                  "QToolButton#RIGHT_ACTION{" + borderRadiusRight + "}"
                  "QLabel{background:transparent;font-weight:bold;}"
                );
            

    for(auto severity : Notification::getSeverities()){
        auto severity_label = severity_labels.value(severity, 0);
        if(severity_label){
            auto color = Theme::QColorToHex(Theme::theme()->getSeverityColor(severity));
            severity_label->setStyleSheet("QLabel{color: " + color + ";}");
        }
    }
    default_icon = theme->getIcon("ToggleIcons", "newNotification");
    show_notification_dialog_action->setIcon(theme->getIcon("Icons", "popOut"));

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
    
    addSeparator();
   

    QFont labelFont(QFont(font().family(), 11, 1));
    int labelWidth = 30;

    auto severities = Notification::getSeverities().toList();
    qSort(severities.begin(), severities.end());
    //Remove Info?
    severities.removeAll(Notification::Severity::SUCCESS);
    
    //Get them in order
    for(auto severity : severities){
        auto severity_label = new QLabel(this);
        severity_labels[severity] = severity_label;
        
        severity_label->setFont(labelFont);
        severity_label->setMinimumWidth(labelWidth);
        severity_label->setAlignment(Qt::AlignCenter);
        severity_label->setToolTip("Number of " + Notification::getSeverityString(severity) + " Notifications");
        
        addWidget(severity_label);
        addSeparator();

        // add shadow to the label's text
        auto effect = new QGraphicsDropShadowEffect(this);
        //effect->setBlurRadius(2);
        effect->setColor(Theme::theme()->black());
        effect->setOffset(1,1);
        severity_label->setGraphicsEffect(effect);
    }

    show_notification_dialog_action = addAction("Show notifications panel");

    // set object names for the two actions - used in the stylesheet
    widgetForAction(show_most_recent_action)->setObjectName("LEFT_ACTION");
    widgetForAction(show_notification_dialog_action)->setObjectName("RIGHT_ACTION");
}

void NotificationToolbar::loadingGifTicked(){
    if(loading_gif){
        auto pixmap = loading_gif->currentPixmap();
        show_most_recent_action->setIcon(pixmap);
    }
}
void NotificationToolbar::setLoadingMode(bool loading){
    if(loading && !loading_gif){
        loading_gif = Theme::theme()->getGif("Icons", "loading");
        connect(loading_gif, &QMovie::frameChanged, this, &NotificationToolbar::loadingGifTicked);
    }else if(!loading && loading_gif){
        //Disconnect all signals from loading_gif to this
        disconnect(loading_gif, 0, this, 0);
        loading_gif = 0;
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

    if(last_notification && last_notification->getInProgressState()){
        setLoadingMode(true);
    }else{
        setLoadingMode(false);
        show_most_recent_action->setIcon(default_icon);
    }
    show_most_recent_action->setEnabled(last_notification);
}

void NotificationToolbar::updateCount(){
    QHash<Notification::Severity, int> severity_counts;

    for(auto notification : NotificationManager::manager()->getNotifications()){
        auto severity = notification->getSeverity();
        severity_counts[severity] ++;
    }

    for(auto severity : Notification::getSeverities()){
        auto label = severity_labels.value(severity, 0);
        auto count = severity_counts.value(severity, 0);
        if(label){
            label->setText(QString::number(count));
        }
    }
    updateButtonIcon();
}
