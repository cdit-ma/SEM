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
NotificationItem::NotificationItem(QSharedPointer<NotificationObject> obj, QWidget *parent)
    : QFrame(parent)
{
    if (!obj) {
        qWarning("NotificationItem::NotificationItem - Notification object is null.");
        return;
    }
    
    notification = obj;

    setupLayout();

    connect(notification.data(), &NotificationObject::progressStateChanged, this, &NotificationItem::updateIcon);
    connect(notification.data(), &NotificationObject::notificationChanged, this, &NotificationItem::timeChanged);
    connect(notification.data(), &NotificationObject::descriptionChanged, this, &NotificationItem::descriptionChanged);
    connect(notification.data(), &NotificationObject::titleChanged, this, &NotificationItem::titleChanged);

    
    connect(notification.data(), &NotificationObject::severityChanged, this, &NotificationItem::updateIcon);
    connect(notification.data(), &NotificationObject::iconChanged, this, &NotificationItem::updateIcon);
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationItem::themeChanged);
    themeChanged();

    connect(action_delete, &QAction::triggered, [=](){
        NotificationManager::manager()->deleteNotification(getID());
    });
}

NotificationItem::~NotificationItem(){

}

void NotificationItem::setupDescriptionLayout(){
    if(!label_description){
        label_description = new QLabel(this);
        label_description->setObjectName("DESCRIPTION");
        label_description->setWordWrap(true);
        //Add to the main layout
        layout()->addWidget(label_description);
    }
}

void NotificationItem::setupLayout(){
    auto v_layout = new QVBoxLayout(this);
    v_layout->setMargin(2);
    v_layout->setSpacing(5);

    auto layout = new QHBoxLayout();
    v_layout->addLayout(layout);
    layout->setMargin(0);
    layout->setSpacing(5);

    label_icon = new QLabel(this);
    label_icon->setScaledContents(true);
    
    
    label_icon->setAlignment(Qt::AlignCenter);

    label_text = new QLabel(this);
    label_text->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    label_time = new QLabel(this);

    toolbar = new QToolBar(this);
    
    action_delete = toolbar->addAction("Delete Notification");
    
    /*
    button_expand = new QToolButton(this);
    button_expand->setCheckable(true);
    button_expand->setAutoRaise(false);
    button_expand->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    action_expand = toolbar->addWidget(button_expand);*/

    layout->addWidget(label_icon);
    layout->addWidget(label_text, 1);
    layout->addWidget(label_time);
    
    layout->addWidget(toolbar);
    layout->addStretch();

    descriptionChanged();
    titleChanged();
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
    if(!label_description){
        setupDescriptionLayout();
    }
    if(label_description){
        label_description->setText(description);
        label_description->setVisible(description.length());
    }
}


/**
 * @brief NotificationItem::descriptionChanged
 * @param description
 */
void NotificationItem::titleChanged()
{
    label_text->setText(notification->getTitle());
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
    auto icon_size = theme->getLargeIconSize();
    label_icon->setFixedSize(icon_size);
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

        auto pixmap = theme->getImage(icon.first, icon.second, icon_size, icon_color);
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
    auto icon_size = theme->getIconSize();
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
                  "QLabel#DESCRIPTION{padding-left: 5px; color: " + theme->getAltTextColorHex() + ";font-style: italic;}"
                  + theme->getToolBarStyleSheet()
                  + "QToolButton{ background: rgba(0,0,0,0); border: 0px; }"
    );

    
    
    if (action_delete) {
        action_delete->setIcon(theme->getIcon("Icons", "cross"));
    }
    if (toolbar){
        toolbar->setIconSize(icon_size);
    }
}
