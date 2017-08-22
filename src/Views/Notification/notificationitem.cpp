#include "notificationitem.h"

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


    connect(notification, &NotificationObject::loading, this, &NotificationItem::updateIcon);
    connect(notification, &NotificationObject::timestampChanged, this, &NotificationItem::timestampChanged);
    connect(notification, &NotificationObject::descriptionChanged, this, &NotificationItem::descriptionChanged);
    connect(notification, &NotificationObject::iconChanged, this, &NotificationItem::updateIcon);
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationItem::themeChanged);
    themeChanged();

    connect(action_delete, &QAction::triggered, [=](){
        NotificationManager::manager()->deleteNotification(notification->ID());
    });
}


void NotificationItem::setupLayout(){

    auto layout = new QHBoxLayout(this);
    layout->setMargin(5);
    layout->setSpacing(5);

    label_icon = new QLabel(this);
    label_icon->setFixedSize(icon_size);
    label_icon->setAlignment(Qt::AlignCenter);

    label_text = new QLabel(this);

    label_time = new QLabel(this);

    auto toolbar = new QToolBar(this);
    toolbar->setIconSize(icon_size/2);
    action_delete = toolbar->addAction("Delete Notification");

    layout->addWidget(label_icon);
    layout->addWidget(label_text, 1);
    layout->addWidget(label_time);
    layout->addWidget(toolbar);

    descriptionChanged();
    timestampChanged();
}


/**
 * @brief NotificationItem::getID
 * @return
 */
int NotificationItem::getID()
{
    if (notification) {
        return notification->ID();
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
        return notification->entityID();
    }
    return -1;
}


/**
 * @brief NotificationItem::getIconPath
 * @return
 */
QString NotificationItem::getIconPath()
{
    return "";
}


/**
 * @brief NotificationItem::getIconName
 * @return
 */
QString NotificationItem::getIconName()
{
    return "";
}


/**
 * @brief NotificationItem::getSeverity
 * @return
 */
NOTIFICATION_SEVERITY NotificationItem::getSeverity()
{
    if (notification) {
        return notification->severity();
    }
    return NOTIFICATION_SEVERITY::INFO;
}


/**
 * @brief NotificationItem::getType
 * @return
 */
NOTIFICATION_TYPE NotificationItem::getType()
{
    if (notification) {
        return notification->type();
    }
    return NOTIFICATION_TYPE::MODEL;
}


/**
 * @brief NotificationItem::getCategory
 * @return
 */
NOTIFICATION_CATEGORY NotificationItem::getCategory()
{
    if (notification) {
        return notification->category();
    }
    return NOTIFICATION_CATEGORY::NONE;
}


/**
 * @brief NotificationItem::setSelected
 * @param select
 */
void NotificationItem::setSelected(bool select)
{
    if (selected != select) {
        selected = select;
        if (selected) {
            backgroundColor =  Theme::theme()->getAltBackgroundColorHex();
        } else {
            backgroundColor =  Theme::theme()->getBackgroundColorHex();
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
    updateStyleSheet();

    updateIcon();
}


/**
 * @brief NotificationItem::descriptionChanged
 * @param description
 */
void NotificationItem::descriptionChanged()
{
    auto description = notification->description();
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
    
    if(notification->isLoading()){
        qCritical() << notification << " LOADING";
        auto movie = Theme::theme()->getGif("Icons", "loading");
        label_icon->setMovie(movie);
    }else{
        qCritical() << notification << " NOT LOADING";
        auto icon_path = notification->iconPath();
        auto icon_name = notification->iconName();
        if (icon_path.isEmpty() || icon_name.isEmpty()) {
            icon_path = "Icons";
            icon_name = getSeverityIcon(notification->severity());
        }
        auto pixmap = Theme::theme()->getImage(icon_path, icon_name, icon_size, getSeverityColor(getSeverity()));
        label_icon->setMovie(0);
        label_icon->setPixmap(pixmap);
    }

}


/**
 * @brief NotificationItem::timestampChanged
 * @param time
 */
void NotificationItem::timestampChanged()
{
    label_time->setText(notification->time().toString("H:mm:ss"));
}


/**
 * @brief NotificationItem::loading
 * @param on
 */
void NotificationItem::loading(bool on)
{
    return;
    /*
    if (loadingOn == on) {
        return;
    }

    if (!loadingGif) {
        loadingGif = new QMovie(this);
        loadingGif->setFileName(":/Images/Icons/loading");
        loadingGif->setScaledSize(QSize(16,16));
    }

    if (on) {
        loadingGif->start();
        iconLabel->setMovie(loadingGif);
    } else {
        loadingGif->stop();
        iconLabel->setPixmap(Theme::theme()->getImage(_iconPath, _iconName, QSize(28,28), getSeverityColor(getSeverity())));
    }*/
}

/**
 * @brief NotificationItem::mouseReleaseEvent
 * This sends a signal to the notification dialog notifying it of this item's current selected state and whether the CONTROL key is down.
 * @param event
 */
void NotificationItem::mouseReleaseEvent(QMouseEvent* event)
{
    emit itemClicked(this, selected, event->modifiers().testFlag(Qt::ControlModifier));
}


/**
 * @brief NotificationItem::enterEvent
 */
void NotificationItem::enterEvent(QEvent *)
{
    emit hoverEnter(getEntityID());
}


/**
 * @brief NotificationItem::leaveEvent
 */
void NotificationItem::leaveEvent(QEvent *)
{
    emit hoverLeave(getEntityID());
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