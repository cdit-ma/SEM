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
    setProperty("ID", ID);
    setProperty("description", description);
    setProperty("iconPath", iconPath);
    setProperty("iconName", iconName);
    setProperty("entityID", eID);

    severity = s;
    type = t;
    category = c;

    expanded = false;
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

    QString path = property("iconPath").toString();
    QString name = property("iconName").toString();
    ///*
    QColor tintColor = NotificationManager::getSeverityColor(severity);
    iconLabel->setPixmap(theme->getImage(path, name, QSize(28,28), tintColor));
    //*/
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
void NotificationItem::mouseReleaseEvent(QMouseEvent *event)
{
    bool append = false;
    if (event->modifiers().testFlag(Qt::ControlModifier)) {
        append = true;
    }
    setSelected(true, append);
}


/**
 * @brief NotificationItem::setSelected
 */
void NotificationItem::setSelected(bool select, bool append)
{
    if (selected != select) {
        selected = select;
        if (selected) {
            emit itemSelected(property("ID").toInt(), append);
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
