#include "notificationdialog.h"
#include "../../theme.h"

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
        updateTypeAction(type);
    }

    setupLayout();
    setWindowTitle("Notifications");

    connect(listWidget, &QListWidget::itemSelectionChanged, this, &NotificationDialog::listSelectionChanged);
    connect(listWidget, &QListWidget::itemClicked, this, &NotificationDialog::notificationItemClicked);
    connect(clearSelectedAction, &QAction::triggered, this, &NotificationDialog::clearSelected);
    connect(clearVisibleAction, &QAction::triggered, this, &NotificationDialog::clearVisible);
    connect(typeActionMapper, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped),this, &NotificationDialog::typeActionToggled);
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));

    themeChanged();
    updateVisibilityCount(0, true);
}


/**
 * @brief NotificationDialog::typeActionToggled
 * @param t
 */
void NotificationDialog::typeActionToggled(int t)
{
    NOTIFICATION_TYPE type = (NOTIFICATION_TYPE) t;
    QAction* action = typeActionHash.value(type, 0);
    bool clearSelection = false;
    if (action) {
        bool visible = action->isChecked();
        foreach (QListWidgetItem* item, notificationHash.values(type)) {
            int row = listWidget->row(item);
            listWidget->setRowHidden(row, !visible);
            if (visible) {
                updateVisibilityCount(1);
            } else {
                updateVisibilityCount(-1);
            }
            clearSelection = clearSelection || item->isSelected();
        }
    }
    if (clearSelection) {
        listWidget->clearSelection();
    }
}


/**
 * @brief NotificationDialog::themeChanged
 */
void NotificationDialog::themeChanged()
{
    Theme* theme = Theme::theme();

    setStyleSheet("QListWidget{ border: 1px solid " + theme->getDisabledBackgroundColorHex() + "; }"
                  + theme->getAltAbstractItemViewStyleSheet()
                  + theme->getDialogStyleSheet());

    topToolbar->setStyleSheet(theme->getToolBarStyleSheet() + "QToolButton{ padding: 2px; border-radius:" + theme->getSharpCornerRadius() + ";}");
    bottomToolbar->setStyleSheet(theme->getToolBarStyleSheet());

    for (int i = 0; i < listWidget->count(); i++) {
        QListWidgetItem* item = listWidget->item(i);
        if (item) {
            QString iconPath = item->data(IR_ICONPATH).toString();
            QString iconName = item->data(IR_ICONNAME).toString();
            item->setIcon(theme->getIcon(iconPath, iconName));
        }
    }

    foreach (NOTIFICATION_TYPE type, getNotificationTypes()) {
        QAction* action = typeActionHash.value(type, 0);
        if (action) {
             QPair<QString, QString> iconPath = getActionIcon(type);
             action->setIcon(theme->getIcon(iconPath));
        }
    }
}


/**
 * @brief NotificationDialog::listSelectionChanged
 */
void NotificationDialog::listSelectionChanged()
{
    bool csEnabled = clearSelectedAction->isEnabled();
    if (csEnabled != (!listWidget->selectedItems().isEmpty())) {
        clearSelectedAction->setEnabled(!csEnabled);
    }
}


/**
 * @brief NotificationDialog::toggleVisibility
 */
void NotificationDialog::toggleVisibility()
{
    setVisible(!isVisible());
}


/**
 * @brief NotificationDialog::resetDialog
 */
void NotificationDialog::resetDialog()
{
    hide();
    clearAll();
}


/**
 * @brief NotificationDialog::clearSelected
 */
void NotificationDialog::clearSelected()
{
    QList<NOTIFICATION_TYPE> removedTypes = notificationHash.uniqueKeys();
    foreach (QListWidgetItem* item, listWidget->selectedItems()) {
        NOTIFICATION_TYPE type = (NOTIFICATION_TYPE) item->data(IR_TYPE).toInt();
        removeItem(item);
        if (!removedTypes.contains(type)) {
            removedTypes.append(type);
        }
    }
    updateTypeActions(removedTypes);
}


/**
 * @brief NotificationDialog::clearVisible
 */
void NotificationDialog::clearVisible()
{
    QList<NOTIFICATION_TYPE> removedTypes;
    for (int i = 0; i < listWidget->count();) {
        QListWidgetItem* item = listWidget->item(i);
        if (item && !listWidget->isRowHidden(i)) {
            NOTIFICATION_TYPE type = (NOTIFICATION_TYPE) item->data(IR_TYPE).toInt();
            removeItem(item);
            if (!removedTypes.contains(type)) {
                removedTypes.append(type);
            }
        } else {
            i++;
        }
    }
    updateVisibilityCount(0, true);
    updateTypeActions(removedTypes);
}


/**
 * @brief NotificationDialog::clearAll
 */
void NotificationDialog::clearAll()
{
    /*
    QList<QListWidgetItem*> items = notificationHash.values();
    foreach (QListWidgetItem* item, items) {
        removeItem(item);
    }
    */
    notificationHash.clear();
    listWidget->clear();
    updateTypeActions(getNotificationTypes());
}


/**
 * @brief NotificationDialog::addNotificationItem
 * @param type
 * @param title
 * @param description
 * @param iconPath
 * @param ID
 */
void NotificationDialog::addNotificationItem(NOTIFICATION_TYPE type, QString title, QString description, QPair<QString, QString> iconPath, int ID)
{
    if (iconPath.first.isEmpty() || iconPath.second.isEmpty()) {
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
    if (typeAction) {
        bool actionVisible = typeAction->isChecked();
        listWidget->setRowHidden(0, !actionVisible);
        updateTypeAction(type);
        if (actionVisible) {
            updateVisibilityCount(1);
        }
    }

    emit notificationAdded();
}


/**
 * @brief NotificationDialog::removeItem
 * @param item
 */
void NotificationDialog::removeItem(QListWidgetItem* item)
{
    if (listWidget && item) {
        int row = listWidget->row(item);
        NOTIFICATION_TYPE type = (NOTIFICATION_TYPE) item->data(IR_TYPE).toInt();

        //Remove from the hash
        notificationHash.remove(type, item);
        delete listWidget->takeItem(row);
    }
}


/**
 * @brief NotificationDialog::getTypeAction
 * @param type
 * @return
 */
QAction* NotificationDialog::getTypeAction(NOTIFICATION_TYPE type) const
{
    return typeActionHash.value(type, 0);
}


/**
 * @brief NotificationDialog::getActionIcon
 * @param type
 * @return
 */
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
    if (item) {
        int ID = item->data(IR_ID).toInt();
        if (ID > 0) {
            emit centerOn(ID);
        }
    }
}


/**
 * @brief NotificationDialog::updateTypeActions
 * @param types
 */
void NotificationDialog::updateTypeActions(QList<NOTIFICATION_TYPE> types)
{
    foreach (NOTIFICATION_TYPE type, types) {
        updateTypeAction(type);
    }
}


/**
 * @brief NotificationDialog::updateTypeAction
 * @param type
 */
void NotificationDialog::updateTypeAction(NOTIFICATION_TYPE type)
{
    QAction* typeAction = getTypeAction(type);
    if (typeAction) {
        int count = notificationHash.values(type).length();
        QString text = "(" % QString::number(count) % ")";
        typeAction->setText(text);
    }
}


/**
 * @brief NotificationDialog::setupLayout
 */
void NotificationDialog::setupLayout()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(DIALOG_SPACING);

    listWidget = new QListWidget(this);
    listWidget->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    listWidget->setUniformItemSizes(true);
    listWidget->setFocusPolicy(Qt::NoFocus);

    topToolbar = new QToolBar(this);
    topToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    //topToolbar->setOrientation(Qt::Vertical);

    foreach(NOTIFICATION_TYPE type, getNotificationTypes()){
        QAction* action = typeActionHash.value(type, 0);
        if (action) {
            topToolbar->addAction(action);
            action->setToolTip("Show/Hide " + GET_NOTIFICATION_TYPE_STRING(type) + " Notifications");
        }
    }

    /*
    QWidget* stretchWidget = new QWidget(this);
    stretchWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topToolbar->addWidget(stretchWidget);
    */

    bottomToolbar = new QToolBar(this);
    bottomToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    clearSelectedAction = bottomToolbar->addAction("Clear Selected");
    clearVisibleAction = bottomToolbar->addAction("Clear Visible");
    clearSelectedAction->setToolTip("Clear Selected Items");
    clearVisibleAction->setToolTip("Clear Visible Items");
    clearSelectedAction->setEnabled(false);
    clearVisibleAction->setEnabled(false);

    mainLayout->addWidget(topToolbar, 0, Qt::AlignHCenter);
    mainLayout->addWidget(listWidget, 1);
    mainLayout->addWidget(bottomToolbar, 0, Qt::AlignRight);

    /*
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(topToolbar);
    hLayout->addWidget(listWidget, 1);
    mainLayout->addLayout(hLayout, 1);
    mainLayout->addWidget(bottomToolbar, 0, Qt::AlignRight);
    */

    setMinimumSize(DIALOG_MIN_WIDTH, DIALOG_MIN_HEIGHT);
    //setMinimumSize(mainLayout->sizeHint().width() + 100, 250);
}


/**
 * @brief NotificationDialog::updateVisibilityCount
 * @param val
 * @param set
 */
void NotificationDialog::updateVisibilityCount(int val, bool set)
{
    if (set) {
        visibleCount = val;
    } else {
        visibleCount += val;
    }
    bool enableAction = visibleCount > 0;
    if (enableAction != clearVisibleAction->isEnabled()) {
        clearVisibleAction->setEnabled(enableAction);
    }
}

