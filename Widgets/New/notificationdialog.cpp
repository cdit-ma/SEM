#include "notificationdialog.h"
#include "../../View/theme.h"

#include <QVBoxLayout>
#include <QToolBar>
#include <QScrollArea>
#include <QStringBuilder>

#define ICON_SIZE 24


/**
 * @brief NotificationDialog::NotificationDialog
 * @param parent
 */
NotificationDialog::NotificationDialog(QWidget *parent) : QDialog(parent)
{
    typeActionMapper = new QSignalMapper(this);

    foreach(NOTIFICATION_TYPE type, getNotificationTypes()){
        QAction* action = new QAction(this);
        action->setCheckable(true);
        action->setChecked(true);

        connect(action, &QAction::toggled, typeActionMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));

        typeActionMapper->setMapping(action, type);
        typeActionHash.insert(type, action);
    }

    setupLayout();

    setWindowTitle("Notifications");
    //setMinimumSize(toolbar->sizeHint().width()*2.5, 300);

    connect(listWidget, &QListWidget::itemClicked, this, &NotificationDialog::notificationItemClicked);

    connect(clearAllAction, &QAction::triggered, this, &NotificationDialog::clearAll);
    connect(clearVisibleAction, &QAction::triggered, this, &NotificationDialog::clearVisible);

    connect(typeActionMapper, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped),this, &NotificationDialog::typeActionToggled);

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}

void NotificationDialog::typeActionToggled(int t)
{
    NOTIFICATION_TYPE type = (NOTIFICATION_TYPE) t;
    QAction* action = typeActionHash.value(type, 0);
    if(action){
        bool visible = action->isChecked();
        foreach(QListWidgetItem* item, notificationHash.values(type)){
            int row = listWidget->row(item);
            listWidget->setRowHidden(row, !visible);
        }
    }
}

/**
 * @brief NotificationDialog::themeChanged
 */
void NotificationDialog::themeChanged()
{
    Theme* theme = Theme::theme();
    /*
    QString toolButtonStyle = "QToolButton{ border-color:" + theme->getAltBackgroundColorHex() + "; border-radius: 2px; }"
                              "QToolButton:hover{ background:" + theme->getHighlightColorHex() + ";"
                              "color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";}";

    clearAllButton->setStyleSheet(toolButtonStyle);
    clearSelectedButton->setStyleSheet(toolButtonStyle);
*/
    setStyleSheet("QDialog{background:" + theme->getBackgroundColorHex() + ";}"
                  "QListWidget{ border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";}"
                  //"QListWidget::focus{ border: 0px; }"
                  "QAbstractItemView {"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "background:" + theme->getBackgroundColorHex() + ";"
                  "}"
                  "QAbstractItemView::item {"
                  "outline: none;"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "background:" + theme->getBackgroundColorHex() + ";"
                  "color:" + theme->getTextColorHex() + ";"
                  "}"
                  "QAbstractItemView::item:selected {"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "background:" + theme->getDisabledBackgroundColorHex() + ";"
                  "}"
                  "QAbstractItemView::item:hover {"
                  "background:" + theme->getDisabledBackgroundColorHex() + ";"
                  "}");


    for(int i = 0; i < listWidget->count(); i++){
        QListWidgetItem* item = listWidget->item(i);
        if(item){
            QString iconPath = item->data(IR_ICONPATH).toString();
            QString iconName = item->data(IR_ICONNAME).toString();
            item->setIcon(theme->getIcon(iconPath, iconName));
        }
    }


    foreach(NOTIFICATION_TYPE type, getNotificationTypes()){
        QAction* action = typeActionHash.value(type, 0);
        if(action){
             QPair<QString, QString> iconPath = getActionIcon(type);
             action->setIcon(theme->getIcon(iconPath));
        }
    }

}

void NotificationDialog::toggleVisibility()
{
    setVisible(!isVisible());
}

/**
 * @brief NotificationDialog::clearAll
 */
void NotificationDialog::clearAll()
{
    QList<NOTIFICATION_TYPE> removedTypes = notificationHash.uniqueKeys();

    while(listWidget->count() != 0){
        QListWidgetItem* item = listWidget->item(0);
        if(item){
            removeItem(item);
        }
    }

    foreach(NOTIFICATION_TYPE type, removedTypes){
        updateTypeAction(type);
    }
}

void NotificationDialog::clearVisible()
{
    QList<NOTIFICATION_TYPE> removedTypes;
    for(int i = 0; i < listWidget->count();){
        QListWidgetItem* item = listWidget->item(i);
        if(item && !listWidget->isRowHidden(i)){
            NOTIFICATION_TYPE type = (NOTIFICATION_TYPE) item->data(IR_TYPE).toInt();
            removeItem(item);
            if(!removedTypes.contains(type)){
                removedTypes.append(type);
            }
        }else{
            i++;
        }
    }
    foreach(NOTIFICATION_TYPE type, removedTypes){
        updateTypeAction(type);
    }
}

/**
 * @brief NotificationDialog::addNotificationItem
 * @param type
 * @param title
 * @param description
 * @param iconPath
 */
void NotificationDialog::addNotificationItem(NOTIFICATION_TYPE type, QString title, QString description, QPair<QString, QString> iconPath, int ID)
{
    if(iconPath.first.isNull() || iconPath.second.isNull()){
        iconPath = getActionIcon(type);
    }

    QListWidgetItem* listItem = new QListWidgetItem();

    //Set Data to item.
    listItem->setData(IR_TYPE, type);
    listItem->setData(IR_ICONPATH, iconPath.first);
    listItem->setData(IR_ICONNAME, iconPath.second);
    listItem->setData(IR_ID, ID);

    //Set the Icon and Text
    listItem->setText("[" % title % "] " % description);
    listItem->setIcon(Theme::theme()->getIcon(iconPath));

    //Insert in the listWidget at 0
    listWidget->insertItem(0, listItem);
    //Put in the multimap
    notificationHash.insertMulti(type, listItem);

    QAction* typeAction = getTypeAction(type);
    if(typeAction){
        bool actionVisible = typeAction->isChecked();
        listWidget->setRowHidden(0, !actionVisible);
        updateTypeAction(type);
    }

    emit notificationAdded();
}

void NotificationDialog::removeItem(QListWidgetItem* item)
{
    if(listWidget && item){
        int row = listWidget->row(item);
        NOTIFICATION_TYPE type = (NOTIFICATION_TYPE) item->data(IR_TYPE).toInt();

        //Remove from the hash
        notificationHash.remove(type, item);
        delete listWidget->takeItem(row);
    }
}

QAction *NotificationDialog::getTypeAction(NOTIFICATION_TYPE type) const
{
    return typeActionHash.value(type, 0);
}

QPair<QString, QString> NotificationDialog::getActionIcon(NOTIFICATION_TYPE type) const
{
    QPair<QString, QString> iconPath;
    iconPath.first = "Actions";

    switch (type) {
    case NT_INFO:
        iconPath.second = "Info";
        break;
    case NT_WARNING:
        iconPath.second = "Warning";
        break;
    case NT_CRITICAL:
        iconPath.second = "Critical";
        break;
    case NT_ERROR:
        iconPath.second = "Failure";
        break;
    default:
        iconPath.second = "Help";
        break;
    }
    return iconPath;
}


/**
 * @brief NotificationDialog::listItemClicked
 * @param item
 */
void NotificationDialog::notificationItemClicked(QListWidgetItem* item)
{
    if(item){
        int ID = item->data(IR_ID).toInt();
        if(ID > 0){
            emit centerOn(ID);
        }
    }
}

void NotificationDialog::updateTypeAction(NOTIFICATION_TYPE type)
{
    QAction* typeAction = getTypeAction(type);
    if(typeAction){
        int count = notificationHash.values(type).length();
        QString text;
        if(count > 0){
            text = QString::number(count);
        }
        typeAction->setText(text);
    }
}

void NotificationDialog::setupLayout()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    listWidget = new QListWidget(this);
    listWidget->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    listWidget->setUniformItemSizes(true);

    toolbar = new QToolBar(this);
    toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    foreach(NOTIFICATION_TYPE type, getNotificationTypes()){
        toolbar->addAction(typeActionHash.value(type, 0));
    }
    QWidget* stretchWidget = new QWidget(this);
    stretchWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar->addWidget(stretchWidget);

    clearVisibleAction = toolbar->addAction("Clear Visible");
    clearAllAction = toolbar->addAction("Clear All");

    mainLayout->addWidget(toolbar);
    mainLayout->addWidget(listWidget, 1);
}

