#include "notificationitem.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"

#include <QMouseEvent>

/**
 * @brief NotificationItem::NotificationItem
 * @param obj
 * @param parent
 * @throws std::invalid_argument
 */
NotificationItem::NotificationItem(QSharedPointer<NotificationObject> obj, QWidget *parent)
	: QFrame(parent)
{
    if (obj.isNull()) {
        throw std::invalid_argument("NotificationItem::NotificationItem - Notification object cannot be null.");
    }
    
    notification = obj;

    setupLayout();

    connect(notification.data(), &NotificationObject::notificationChanged, this, &NotificationItem::timeChanged);
    connect(notification.data(), &NotificationObject::descriptionChanged, this, &NotificationItem::descriptionChanged);
    connect(notification.data(), &NotificationObject::titleChanged, this, &NotificationItem::titleChanged);

    connect(notification.data(), &NotificationObject::progressStateChanged, this, &NotificationItem::updateIcon);
    connect(notification.data(), &NotificationObject::severityChanged, this, &NotificationItem::updateIcon);
    connect(notification.data(), &NotificationObject::iconChanged, this, &NotificationItem::updateIcon);

    connect(notification.data(), &NotificationObject::progressStateChanged, this, &NotificationItem::updateActionDeleteEnabled);
    connect(notification.data(), &NotificationObject::severityChanged, this, &NotificationItem::updateActionDeleteEnabled);

    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationItem::themeChanged);
    themeChanged();

    connect(action_delete, &QAction::triggered, [=]() {
        NotificationManager::manager()->deleteNotification(getID());
    });
}

/**
 * @brief NotificationItem::setupDescriptionLayout
 */
void NotificationItem::setupDescriptionLayout()
{
    label_description = new QLabel(this);
    label_description->setObjectName("DESCRIPTION");
    label_description->setWordWrap(true);

    //Add to the main layout
    layout()->addWidget(label_description);
}

/**
 * @brief NotificationItem::setupLayout
 */
void NotificationItem::setupLayout()
{
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
    updateActionDeleteEnabled();

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
    return notification->getID();
}

/**
 * @brief NotificationItem::getEntityID
 * @return
 */
int NotificationItem::getEntityID()
{
    return notification->getEntityID();
}

/**
 * @brief NotificationItem::isSelected
 * @return
 */
bool NotificationItem::isSelected()
{
    return selected_;
}

/**
 * @brief NotificationItem::setSelected
 * @param selected
 */
void NotificationItem::setSelected(bool selected)
{
    if (selected_ != selected) {
        selected_ = selected;
        updateStyleSheet();
    }
}

/**
 * @brief NotificationItem::themeChanged
 */
void NotificationItem::themeChanged()
{
    Theme* theme = Theme::theme();

    toolbar->setIconSize(theme->getIconSize());
    label_icon->setFixedSize(theme->getLargeIconSize());
    action_delete->setIcon(theme->getIcon("Icons", "cross"));

    auto delete_button = toolbar->widgetForAction(action_delete);
    delete_button->setStyleSheet(theme->getToolBarStyleSheet() + "QToolButton{ background: rgba(0,0,0,0); border: 0px; }");

    updateStyleSheet();
    updateIcon();
}

/**
 * @brief NotificationItem::descriptionChanged
 */
void NotificationItem::descriptionChanged()
{
    auto description = notification->getDescription();
    if (!label_description) {
        setupDescriptionLayout();
    }
    label_description->setText(description);
    label_description->setVisible(description.length());
}

/**
 * @brief NotificationItem::titleChanged
 * @param description
 */
void NotificationItem::titleChanged()
{
    label_text->setText(notification->getTitle());
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

    if (is_running) {
        //Use a GIF if we are loading
        auto movie = theme->getGif("Icons", "loading");
        label_icon->setMovie(movie);
    } else {
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
}


/**
 * @brief NotificationItem::updateActionDeleteEnabled
 */
void NotificationItem::updateActionDeleteEnabled()
{
    // Can only delete finished notifications
    action_delete->setEnabled(notification->getSeverity() != Notification::Severity::RUNNING);
}

/**
 * @brief NotificationItem::mouseReleaseEvent
 * This sends a signal to the notification dialog notifying it of this item's current selected state and whether the CONTROL key is down.
 * @param event
 */
void NotificationItem::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (!doubleClicked_) {
            // toggle the item's selected state
            toggleSelected();
        } else {
            doubleClicked_ = false;
        }
    } else if (event->button() == Qt::MiddleButton) {
        // center on the linked entity item
        emit centerEntityItem(getEntityID());
    }
}


/**
 * @brief NotificationItem::mouseDoubleClickEvent
 * @param event
 */
void NotificationItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // always select the item on double-click
        doubleClicked_ = true;
        selected_ = false;
        toggleSelected();
        emit selectAndCenterEntityItem(getEntityID());
    }
}

/**
 * @brief NotificationItem::toggleSelected
 * This should only be called from within the mouse events, when this item is clicked/double-clicked.
 */
void NotificationItem::toggleSelected()
{
    setSelected(!selected_);
    emit notificationItemClicked(this);

    // when selected, flash the linked entity item if there is one
    // unfortunately, this is also triggered when the item is expanded/contracted
    if (selected_) {
        auto entityID = getEntityID();
        if (entityID != -1) {
            emit flashEntityItem(entityID);
        }
    }
}

/**
 * @brief NotificationItem::updateStyleSheet
 */
void NotificationItem::updateStyleSheet()
{
    Theme* theme = Theme::theme();
    if (selected_) {
        backgroundColor_ = theme->getAltBackgroundColorHex();
    } else {
        backgroundColor_ = theme->getBackgroundColorHex();
    }

    setStyleSheet("QFrame {"
                  "border-style: solid;"
                  "border-width: 0px 0px 1px 0px;"
                  "border-color:" + theme->getDisabledBackgroundColorHex() + ";"
                  "background:" + backgroundColor_ + ";"
                  "color:" + theme->getTextColorHex() + ";"
                  "padding:2px;"
                  "}"
                  "QFrame:hover { background:" + theme->getDisabledBackgroundColorHex() + ";}"
                  "QLabel{ background: rgba(0,0,0,0); border: 0px; }"
                  "QLabel#DESCRIPTION{ padding-left: 5px; color: " + theme->getAltTextColorHex() + ";font-style: italic; }"
    );
}