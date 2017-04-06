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
    notificationObject = obj;
    selected = true;
    setSelected(false);

    if (!obj) {
        qWarning() << "NotificationItem::NotificationItem - Notification object is null.";
        return;
    }

    iconPath = obj->iconPath();
    iconName = obj->iconName();
    if (iconPath.isEmpty() || iconName.isEmpty()) {
        QPair<QString, QString> severityIcon = NotificationManager::getSeverityIcon(getSeverity());
        iconPath = severityIcon.first;
        iconName = severityIcon.second;
    }

    descriptionLabel = new QLabel(obj->description(), this);
    iconLabel = new QLabel(this);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(iconLabel);
    layout->addWidget(descriptionLabel, 1);

    // this item is visible by default - initialise all filter visibility to true
    foreach (NOTIFICATION_FILTER filter, NotificationManager::getNotificationFilters()) {
        filterVisibility[filter] = true;
    }

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
    return iconPath;
}


/**
 * @brief NotificationItem::getIconName
 * @return
 */
QString NotificationItem::getIconName()
{
    return iconName;
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
    return NS_INFO;
}


/**
 * @brief NotificationItem::getType
 * @return
 */
NOTIFICATION_TYPE2 NotificationItem::getType()
{
    if (notificationObject) {
        return notificationObject->type();
    }
    return NT_MODEL;
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
    return NC_NOCATEGORY;
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

    QColor tintColor = NotificationManager::getSeverityColor(getSeverity());
    iconLabel->setPixmap(theme->getImage(getIconPath(), getIconName(), QSize(28,28), tintColor));
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
 * @brief NotificationItem::filterCleared
 * This is called when a filter group has been cleared.
 * This means that none of the filters in that group is checked; hence, set that filter to visible.
 * @param filter
 */
void NotificationItem::filterCleared(NOTIFICATION_FILTER filter)
{
    updateVisibility(filter, true);
}


/**
 * @brief NotificationItem::severityFilterToggled
 * This is called when the checked filter(s) from the SEVERITY filter group has changed.
 * It updates this item's visibility based on that change.
 * @param checkedStates
 */
void NotificationItem::severityFilterToggled(QHash<NOTIFICATION_SEVERITY, bool> checkedStates)
{
    bool visible = checkedStates.value(getSeverity(), false);
    updateVisibility(NF_SEVERITY, visible);
}


/**
 * @brief NotificationItem::typeFilterToggled
 * This is called when the checked filter(s) from the TYPE filter group has changed.
 * It updates this item's visibility based on that change.
 * @param checkedStates
 */
void NotificationItem::typeFilterToggled(QHash<NOTIFICATION_TYPE2, bool> checkedStates)
{
    bool visible = checkedStates.value(getType(), false);
    updateVisibility(NF_TYPE, visible);
}


/**
 * @brief NotificationItem::categoryFilterToggled
 * This is called when the checked filter(s) from the CATEGORY filter group has changed.
 * It updates this item's visibility based on that change.
 * @param checkedStates
 */
void NotificationItem::categoryFilterToggled(QHash<NOTIFICATION_CATEGORY, bool> checkedStates)
{
    bool visible = checkedStates.value(getCategory(), false);
    updateVisibility(NF_CATEGORY, visible);
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
