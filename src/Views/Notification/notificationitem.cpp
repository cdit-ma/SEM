#include "notificationitem.h"

#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Controllers/NotificationManager/notificationobject.h"

#include <QMouseEvent>
#include <QToolBar>


/**
 * @brief NotificationItem::NotificationItem
 * @param obj
 * @param parent
 */
NotificationItem::NotificationItem(NotificationObject* obj, QWidget *parent)
    : QFrame(parent)
{
    if (!obj) {
        qWarning() << "NotificationItem::NotificationItem - Notification object is null.";
        return;
    }
    
    notification = obj;

    setupLayout();

    connect(notification, &NotificationObject::progressStateChanged, this, &NotificationItem::updateIcon);
    connect(notification, &NotificationObject::notificationChanged, this, &NotificationItem::timeChanged);
    connect(notification, &NotificationObject::descriptionChanged, this, &NotificationItem::descriptionChanged);

    
    connect(notification, &NotificationObject::severityChanged, this, &NotificationItem::updateIcon);
    connect(notification, &NotificationObject::iconChanged, this, &NotificationItem::updateIcon);
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationItem::themeChanged);
    themeChanged();

    connect(action_delete, &QAction::triggered, [=](){
        NotificationManager::manager()->deleteNotification(getID());
    });
}

NotificationItem::~NotificationItem(){

}


void NotificationItem::setupLayout(){

    auto layout = new QHBoxLayout(this);
    layout->setMargin(2);
    layout->setSpacing(5);

    label_icon = new QLabel(this);
    label_icon->setScaledContents(true);
    
    label_icon->setAlignment(Qt::AlignCenter);

    label_text = new QLabel(this);
    label_time = new QLabel(this);

    toolbar = new QToolBar(this);
    
    action_delete = toolbar->addAction("Delete Notification");

    layout->addWidget(label_icon);
    layout->addWidget(label_text, 1);
    layout->addWidget(label_time);
    layout->addWidget(toolbar);
    layout->addStretch();

    descriptionChanged();
    timeChanged();
}


/**
 * @brief NotificationItem::getID
 * @return
 */
int NotificationItem::getID()
{
    if (notification) {
        return notification->getID();
    }
    return -1;
}

NotificationObject* NotificationItem::getNotification() const{
    return notification;
}


/**
 * @brief NotificationItem::getEntityID
 * @return
 */
int NotificationItem::getEntityID()
{
    if (notification) {
        return notification->getEntityID();
    }
    return -1;
}

/**
 * @brief NotificationItem::setSelected
 * @param select
 */
void NotificationItem::setSelected(bool select)
{
    if (selected != select) {
        selected = select;
        auto entity_id = getEntityID();
        if (selected) {
            backgroundColor =  Theme::theme()->getAltBackgroundColorHex();
        } else {
            backgroundColor =  Theme::theme()->getBackgroundColorHex();
        }
        if(entity_id != -1){
            emit highlightEntity(entity_id, selected);
        }
        updateStyleSheet();
    }
}



/**
 * @brief NotificationItem::themeChanged
 */
void NotificationItem::themeChanged()
{
    Theme* theme = Theme::theme();
    if (selected) {
        backgroundColor =  theme->getAltBackgroundColorHex();
    } else {
        backgroundColor = theme->getBackgroundColorHex();
    }
    auto icon_size = theme->getIconSize();
    label_icon->setFixedSize(icon_size);
    toolbar->setIconSize(icon_size);

    updateStyleSheet();

    updateIcon();
}


/**
 * @brief NotificationItem::descriptionChanged
 * @param description
 */
void NotificationItem::descriptionChanged()
{
    auto description = notification->getDescription();
    if (description.isEmpty()) {
        description = "...";
    }
    label_text->setText(description);
}


/**
 * @brief NotificationItem::iconChanged
 * @param iconPath
 * @param iconName
 */
void NotificationItem::updateIcon()
{
    auto theme = Theme::theme();
    auto severity = notification->getSeverity();
    auto is_running = severity == Notification::Severity::RUNNING;
    if(is_running){
        //Use a GIF if we are loading
        auto movie = theme->getGif("Icons", "loading");
        label_icon->setMovie(movie);
    }else{
        //Use an icon otherwise
        auto icon = notification->getIcon();
        if (icon.first.isEmpty() || icon.second.isEmpty()) {
            icon.first = "Notification";
            icon.second = Notification::getSeverityString(severity);
        }
        auto icon_color = theme->getSeverityColor(severity);

        auto pixmap = theme->getImage(icon.first, icon.second, theme->getIconSize(), icon_color);
        label_icon->setPixmap(pixmap);
    }

    //Can only delete finished notifications
    action_delete->setEnabled(!is_running);
}


/**
 * @brief NotificationItem::timestampChanged
 * @param time
 */
void NotificationItem::timeChanged()
{
    label_time->setText(notification->getModifiedTime().toString("H:mm:ss"));
}


/**
 * @brief NotificationItem::mouseReleaseEvent
 * This sends a signal to the notification dialog notifying it of this item's current selected state and whether the CONTROL key is down.
 * @param event
 */
void NotificationItem::mouseReleaseEvent(QMouseEvent* event)
{
    emit itemClicked(this);
}

/**
 * @brief NotificationItem::updateStyleSheet
 */
void NotificationItem::updateStyleSheet()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QFrame {"
                  "border-style: solid;"
                  "border-width: 0px 0px 1px 0px;"
                  "border-color:" + theme->getDisabledBackgroundColorHex() + ";"
                  "background:" + backgroundColor + ";"
                  "color:" + theme->getTextColorHex() + ";"
                  "padding:2px;"
                  "}"
                  "QFrame:hover { background:" + theme->getDisabledBackgroundColorHex() + ";}"
                  "QLabel{ background: rgba(0,0,0,0); border: 0px; }"
                  + theme->getToolBarStyleSheet()
                  + "QToolButton{ background: rgba(0,0,0,0); border: 0px; }"
    );
    
    if (action_delete) {
        action_delete->setIcon(theme->getIcon("Icons", "cross"));
    }
}