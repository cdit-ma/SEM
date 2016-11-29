#include "notificationitem.h"

#include <QMouseEvent>

/**
 * @brief NotificationItem::NotificationItem
 * @param ID
 * @param description
 * @param iconPath
 * @param iconName
 * @param eID
 * @param s
 * @param t
 * @param c
 * @param parent
 */
NotificationItem::NotificationItem(int ID, QString description, QString iconPath, QString iconName, int eID, NOTIFICATION_SEVERITY s, NOTIFICATION_TYPE2 t, NOTIFICATION_CATEGORY c, QWidget *parent)
    : QFrame(parent)
{
    if (iconPath.isEmpty() || iconName.isEmpty()) {
        iconPath = "Actions";
        iconName = "Help";
    }

    setProperty("ID", ID);
    setProperty("description", description);
    setProperty("iconPath", iconPath);
    setProperty("iconName", iconName);
    setProperty("entityID", eID);

    severity = s;
    type = t;
    category = c;

    //expanded = false;
    selected = true;

    setSelected(false);

    descriptionLabel = new QLabel(description, this);
    iconLabel = new QLabel(this);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(iconLabel);
    layout->addWidget(descriptionLabel, 1);

    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationItem::themeChanged);
    themeChanged();
}


/**
 * @brief NotificationItem::getID
 * @return
 */
int NotificationItem::getID()
{
    return property("ID").toInt();
}


/**
 * @brief NotificationItem::getEntityID
 * @return
 */
int NotificationItem::getEntityID()
{
    return property("entityID").toInt();
}


/**
 * @brief NotificationItem::getIconPath
 * @return
 */
QString NotificationItem::getIconPath()
{
    return property("iconPath").toString();
}


/**
 * @brief NotificationItem::getIconName
 * @return
 */
QString NotificationItem::getIconName()
{
    return property("iconName").toString();
}


/**
 * @brief NotificationItem::getSeverity
 * @return
 */
NOTIFICATION_SEVERITY NotificationItem::getSeverity()
{
    return severity;
}


/**
 * @brief NotificationItem::getType
 * @return
 */
NOTIFICATION_TYPE2 NotificationItem::getType()
{
    return type;
}


/**
 * @brief NotificationItem::getCategory
 * @return
 */
NOTIFICATION_CATEGORY NotificationItem::getCategory()
{
    return category;
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

    QString path = getIconPath();
    QString name = getIconName();
    QColor tintColor = NotificationManager::getSeverityColor(severity);
    iconLabel->setPixmap(theme->getImage(path, name, QSize(28,28), tintColor));
    //iconLabel->setPixmap(theme->getImage(path, name, QSize(28,28), theme->getMenuIconColor()));
}


/**
 * @brief NotificationItem::severityFilterToggled
 * @param checkedStates
 */
void NotificationItem::severityFilterToggled(QHash<NOTIFICATION_SEVERITY, bool> checkedStates)
{
    bool visible = checkedStates.value(severity, false);
    if (isVisible() != visible) {
        setVisible(visible);
    }
}


/**
 * @brief NotificationItem::typeFilterToggled
 * @param checkedStates
 */
void NotificationItem::typeFilterToggled(QHash<NOTIFICATION_TYPE2, bool> checkedStates)
{
    bool visible = checkedStates.value(type, false);
    if (isVisible() != visible) {
        setVisible(visible);
    }
}


/**
 * @brief NotificationItem::categoryFilterToggled
 * @param checkedStates
 */
void NotificationItem::categoryFilterToggled(QHash<NOTIFICATION_CATEGORY, bool> checkedStates)
{
    bool visible = checkedStates.value(category, false);
    if (isVisible() != visible) {
        setVisible(visible);
    }
}


/**
 * @brief NotificationItem::mouseReleaseEvent
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
