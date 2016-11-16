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

    QColor tintColor = NotificationManager::getSeverityColor(severity);
    QString path = property("iconPath").toString();
    QString name = property("iconName").toString();
    iconLabel->setPixmap(theme->getImage(path, name, QSize(28,28), tintColor));
}


/**
 * @brief NotificationItem::filterButtonToggled
 * @param filter
 * @param filterVal
 * @param checked
 */
void NotificationItem::filterButtonToggled(NOTIFICATION_FILTER filter, int filterVal, bool checked)
{
    if (checked && isVisible()) {
        return;
    }

    int itemVal = -1;
    switch (filter) {
    case NF_SEVERITY:
        itemVal = severity;
        break;
    case NF_TYPE:
        itemVal = type;
        break;
    case NF_CATEGORY:
        itemVal = category;
        break;
    default:
        return;
    }

    bool visible = checked && (itemVal == filterVal);
    if (visible != isVisible()) {
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
