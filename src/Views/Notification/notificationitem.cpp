#include "notificationitem.h"

#include <QMouseEvent>


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

    /*
    creationDateTime = obj->creationDateTime();
    timestampLabel = new QLabel("0s ago", this);

    QTimer* ellapsedTimer = new QTimer(this);
    ellapsedTimer->setTimerType(Qt::VeryCoarseTimer);
    connect(ellapsedTimer, &QTimer::timeout, this, &NotificationItem::updateEllapsedTime);
    ellapsedTimer->start(10000);
    */

    notificationObject = obj;
    selected = true;
    setSelected(false);

    _iconPath = obj->iconPath();
    _iconName = obj->iconName();

    descriptionLabel = new QLabel(obj->description(), this);
    iconLabel = new QLabel(this);
    timestampLabel = new QLabel(obj->creationDateTime().time().toString("H:mm a"), this);

    //descriptionLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    //descriptionLabel->setMinimumWidth(10);

    loadingGif = 0;
    loadingOn = false;

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(iconLabel);
    layout->addWidget(descriptionLabel);
    layout->addWidget(timestampLabel, 1, Qt::AlignRight);

    /*
    // this item is visible by default - initialise all filter visibility to true
    foreach (NOTIFICATION_FILTER filter, getNotificationFilters()) {
        filterVisibility[filter] = true;
    }*/

    connect(obj, &NotificationObject::loading, this, &NotificationItem::loading);
    connect(obj, &NotificationObject::timestampChanged, this, &NotificationItem::timestampChanged);
    connect(obj, &NotificationObject::descriptionChanged, this, &NotificationItem::descriptionChanged);
    connect(obj, &NotificationObject::iconChanged, this, &NotificationItem::iconChanged);
    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationItem::themeChanged);
    themeChanged();
}


/**
 * @brief NotificationItem::getID
 * @return
 */
int NotificationItem::getID()
{
    if (notificationObject) {
        return notificationObject->ID();
    }
    return -1;
}


/**
 * @brief NotificationItem::getEntityID
 * @return
 */
int NotificationItem::getEntityID()
{
    if (notificationObject) {
        return notificationObject->entityID();
    }
    return -1;
}


/**
 * @brief NotificationItem::getIconPath
 * @return
 */
QString NotificationItem::getIconPath()
{
    return _iconPath;
}


/**
 * @brief NotificationItem::getIconName
 * @return
 */
QString NotificationItem::getIconName()
{
    return _iconName;
}


/**
 * @brief NotificationItem::getSeverity
 * @return
 */
NOTIFICATION_SEVERITY NotificationItem::getSeverity()
{
    if (notificationObject) {
        return notificationObject->severity();
    }
    return NOTIFICATION_SEVERITY::INFO;
}


/**
 * @brief NotificationItem::getType
 * @return
 */
NOTIFICATION_TYPE NotificationItem::getType()
{
    if (notificationObject) {
        return notificationObject->type();
    }
    return NOTIFICATION_TYPE::MODEL;
}


/**
 * @brief NotificationItem::getCategory
 * @return
 */
NOTIFICATION_CATEGORY NotificationItem::getCategory()
{
    if (notificationObject) {
        return notificationObject->category();
    }
    return NOTIFICATION_CATEGORY::NONE;
}


/**
 * @brief NotificationItem::getNotificationFilterValue
 * @param filter
 * @return
 */
int NotificationItem::getNotificationFilterValue(NOTIFICATION_FILTER filter)
{
    int value = -1;
    switch (filter) {
    case NOTIFICATION_FILTER::SEVERITY:
        value = static_cast<int>(getSeverity());
        break;
    case NOTIFICATION_FILTER::CATEGORY:
        value = static_cast<int>(getCategory());
        break;
    case NOTIFICATION_FILTER::TYPE:
        value = static_cast<int>(getType());
        break;
    default:
        break;
    }
    return value;
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
 * @brief NotificationItem::addFilter
 * @param filter
 * @param filterVal
 */
void NotificationItem::addFilter(QVariant filter, QVariant filterVal)
{
    if (!customFilters.contains(filter)) {
        customFilters[filter] = filterVal;
        customFilterVisibility[filter] = true;
    }
}


/**
 * @brief NotificationItem::filtersChanged
 * @param filter
 * @param filterVal
 */
void NotificationItem::filtersChanged(QVariant filter, QVariant filterVal)
{
    if (customFilters.contains(filter)) {
        bool filterMatched = customFilters[filter] == filterVal;
        customFilterVisibility[filter] = filterMatched;
        updateVisibility(filterMatched);
    } else {
        updateVisibility(false);
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
    iconChanged(getIconPath(), getIconName());
}


/**
 * @brief NotificationItem::descriptionChanged
 * @param description
 */
void NotificationItem::descriptionChanged(QString description)
{
    if (description.isEmpty()) {
        description = "...";
    }
    descriptionLabel->setText(description);
}


/**
 * @brief NotificationItem::iconChanged
 * @param iconPath
 * @param iconName
 */
void NotificationItem::iconChanged(QString iconPath, QString iconName)
{
    _iconPath = iconPath;
    _iconName = iconName;

    if (_iconPath.isEmpty() || _iconName.isEmpty()) {
        _iconPath = "Icons";
        _iconName = getSeverityIcon(getSeverity());
    }

    if (!loadingOn) {
        iconLabel->setPixmap(Theme::theme()->getImage(_iconPath, _iconName, QSize(28,28), getSeverityColor(getSeverity())));
    }
}


/**
 * @brief NotificationItem::timestampChanged
 * @param time
 */
void NotificationItem::timestampChanged(QTime time)
{
    timestampLabel->setText(time.toString("H:mm a"));
}


/**
 * @brief NotificationItem::loading
 * @param on
 */
void NotificationItem::loading(bool on)
{
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
    }
}


/**
 * @brief NotificationItem::updateEllapsedTime
 */
/*
void NotificationItem::updateEllapsedTime()
{
    int timeElapsed = creationDateTime.time().elapsed();
    timestampLabel->setText(QString::number(timeElapsed/1000) + "s ago");
}
*/


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
                  + theme->getToolBarStyleSheet());
}


/**
 * @brief NotificationItem::updateVisibility
 * @param filterMatched
 */
void NotificationItem::updateVisibility(bool filterMatched)
{
    if (isVisible() != filterMatched) {

        bool showItem = filterMatched;
        if (filterMatched) {
            foreach (bool visible, filterVisibility.values()) {
                if (!visible) {
                    showItem = false;
                    break;
                }
            }
            if (showItem) {
                foreach (bool visible, customFilterVisibility.values()) {
                    if (!visible) {
                        showItem = false;
                        break;
                    }
                }
            }
        }

        // de-select this item if it is hidden
        if (!showItem && selected) {
            emit itemClicked(this, selected, true);
        }

        setVisible(showItem);
    }
}


/**
 * @brief NotificationItem::updateVisibility
 * This is called when a filter has been triggered.
 * It updates the visibily of this item based on whether all of its filters match the currently checked filters.
 * @param filter
 * @param filterMatched
 */
void NotificationItem::filtersChanged(NOTIFICATION_FILTER filter, bool filterMatched)
{
    if (filterVisibility.contains(filter)) {
        filterVisibility[filter] = filterMatched;
        updateVisibility(filterMatched);
    }
}


inline uint qHash(QVariant key, uint seed)
{
    return ::qHash(key.toUInt(), seed);
}
