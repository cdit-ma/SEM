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

    notificationObject = obj;
    selected = true;
    setSelected(false);

    _iconPath = obj->iconPath();
    _iconName = obj->iconName();

    descriptionLabel = new QLabel(obj->description(), this);
    iconLabel = new QLabel(this);

    loadingGif = 0;
    loadingOn = false;

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(iconLabel);
    layout->addWidget(descriptionLabel, 1);

    // this item is visible by default - initialise all filter visibility to true
    foreach (NOTIFICATION_FILTER filter, getNotificationFilters()) {
        filterVisibility[filter] = true;
    }

    connect(obj, &NotificationObject::loading, this, &NotificationItem::loading);
    connect(obj, &NotificationObject::descriptionChanged, this, &NotificationItem::on_descriptionChanged);
    connect(obj, &NotificationObject::iconChanged, this, &NotificationItem::on_iconChanged);
    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationItem::on_themeChanged);
    on_themeChanged();
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
    return NOTIFICATION_CATEGORY::NOCATEGORY;
}


/**
 * @brief NotificationItem::on_themeChanged
 */
void NotificationItem::on_themeChanged()
{
    Theme* theme = Theme::theme();
    if (selected) {
        backgroundColor =  theme->getAltBackgroundColorHex();
    } else {
        backgroundColor = theme->getBackgroundColorHex();
    }
    updateStyleSheet();
    on_iconChanged(getIconPath(), getIconName());
}


/**
 * @brief NotificationItem::on_descriptionChanged
 * @param description
 */
void NotificationItem::on_descriptionChanged(QString description)
{
    if (description.isEmpty()) {
        description = "...";
    }
    descriptionLabel->setText(description);
}


/**
 * @brief NotificationItem::on_iconChanged
 * @param iconPath
 * @param iconName
 */
void NotificationItem::on_iconChanged(QString iconPath, QString iconName)
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
 * @brief NotificationItem::showItem
 * This forces this item to be visible.
 */
void NotificationItem::showItem()
{
    foreach (NOTIFICATION_FILTER filter, filterVisibility.keys()) {
        filterVisibility[filter] = true;
    }
    show();
}


/**
 * @brief NotificationItem::on_filterCleared
 * This is called when a filter group has been cleared.
 * This means that none of the filters in that group is checked; hence, set that filter to visible.
 * @param filter
 */
void NotificationItem::on_filterCleared(NOTIFICATION_FILTER filter)
{
    updateVisibility(filter, true);
}


/**
 * @brief NotificationItem::on_filtersChanged
 * This is called when the checked filters have changed.
 * It updates this item's visibility based on that change.
 * @param filter
 * @param checkedKeys
 */
void NotificationItem::on_filtersChanged(NOTIFICATION_FILTER filter, QList<QVariant> checkedKeys)
{
    bool visible = true;

    switch (filter) {
    case NOTIFICATION_FILTER::SEVERITY:
    {
        int sInt = static_cast<int>(getSeverity());
        visible = checkedKeys.contains(sInt);
        break;
    }
    case NOTIFICATION_FILTER::CATEGORY:
    {
        int cInt = static_cast<int>(getCategory());
        visible = checkedKeys.contains(cInt);
        break;
    }
    case NOTIFICATION_FILTER::TYPE:
    {
        int tInt = static_cast<int>(getType());
        visible = checkedKeys.contains(tInt);
        break;
    }
    default:
        break;
    }

    updateVisibility(filter, visible);
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
 * This is called when a filter has been triggered.
 * It updates the visibily of this item based on whether all of its filters match the currently checked filters.
 * @param filter
 * @param visible
 */
void NotificationItem::updateVisibility(NOTIFICATION_FILTER filter, bool visible)
{
    if (!filterVisibility.contains(filter)) {
        return;
    }

    filterVisibility[filter] = visible;

    if (isVisible() != visible) {
        bool allVisible = true;
        foreach (bool visible, filterVisibility.values()) {
            if (!visible) {
                allVisible = false;
                break;
            }
        }
        if (isVisible() != allVisible) {
            setVisible(allVisible);
            // de-select this item if it is hidden
            if (!allVisible && selected) {
                emit itemClicked(this, selected, true);
            }
        }
    }
}
