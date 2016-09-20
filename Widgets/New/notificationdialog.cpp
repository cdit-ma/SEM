#include "notificationdialog.h"
#include "../../View/theme.h"

#include <QVBoxLayout>
#include <QToolBar>
#include <QScrollArea>

#define ICON_SIZE 24


/**
 * @brief NotificationDialog::NotificationDialog
 * @param parent
 */
NotificationDialog::NotificationDialog(QWidget *parent) : QDialog(parent)
{
    /*
    QWidget* scrollableWidget = new QWidget(this);
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(scrollableWidget);
    scrollArea->setWidgetResizable(true);
    */

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* topHLayout = new QHBoxLayout();
    QHBoxLayout* bottomHLayout = new QHBoxLayout();
    //QHBoxLayout* bottomHLayout = new QHBoxLayout(scrollableWidget);

    listWidget = new QListWidget(this);
    typeIconListWidget = new QListWidget(this);

    listWidget->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    listWidget->setSortingEnabled(true);
    listWidget->setUniformItemSizes(true);

    typeIconListWidget->setSelectionMode(QListWidget::NoSelection);
    typeIconListWidget->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    typeIconListWidget->setFixedWidth(ICON_SIZE + 10);
    typeIconListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    typeIconListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    typeIconListWidget->hide();

    clearAllButton = new QToolButton(this);
    clearAllButton->setText("Clear All");

    clearSelectedButton = new QToolButton(this);
    clearSelectedButton->setText("Clear Selected");

    /*
    typeComboBox = new QComboBox(this);
    typeComboBox->setFixedHeight(clearAllButton->sizeHint().height());
    typeComboBox->addItem("Notification");
    typeComboBox->addItem("Warning");
    typeComboBox->addItem("Critical");
    typeComboBox->addItem("All");
    */

    //QWidget* stretcher = new QWidget(this);
    //stretcher->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //toolbar->addWidget(typeComboBox);
    //toolbar->addWidget(stretcher);
    toolbar->addWidget(clearSelectedButton);
    toolbar->addWidget(clearAllButton);

    mainLayout->addLayout(topHLayout);
    mainLayout->addLayout(bottomHLayout, 1);
    //mainLayout->addWidget(scrollArea, 1);

    topHLayout->addWidget(toolbar, 1, Qt::AlignRight);

    bottomHLayout->setSpacing(0);
    bottomHLayout->addWidget(typeIconListWidget);
    bottomHLayout->addWidget(listWidget, 1);

    /*
    addNotificationItem(NT_INFO, "", "This is an info message", QPair<QString, QString>());
    addNotificationItem(NT_WARNING, "", "This is a warning message", QPair<QString, QString>());
    addNotificationItem(NT_ERROR, "", "This is a critical message", QPair<QString, QString>());
    addNotificationItem(NT_INFO, "", "This is an info message", QPair<QString, QString>());
    addNotificationItem(NT_WARNING, "", "This is a warning message", QPair<QString, QString>());
    addNotificationItem(NT_ERROR, "", "This is a critical message", QPair<QString, QString>());
    addNotificationItem(NT_INFO, "", "This is an info message", QPair<QString, QString>());
    addNotificationItem(NT_WARNING, "", "This is a warning message", QPair<QString, QString>());
    addNotificationItem(NT_ERROR, "", "This is a critical message", QPair<QString, QString>());
    */

    setWindowTitle("Notifications");
    setMinimumSize(toolbar->sizeHint().width()*2.5, 300);

    connect(listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(listItemClicked(QListWidgetItem*)));
    connect(clearAllButton, SIGNAL(clicked(bool)), this, SLOT(clearAll()));
    connect(clearSelectedButton, SIGNAL(clicked(bool)), this, SLOT(clearSelected()));
    //connect(typeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(displayTypeChanged(int)));

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}


/**
 * @brief NotificationDialog::addListAction
 * @param description
 * @param type
 */
void NotificationDialog::addListAction(QString description, NOTIFICATION_TYPE type)
{
    /*
    QAction* listAction = new QAction(description, this);
    listWidget->addAction(listAction);

    QActionGroup* group = 0;

    switch (type) {
    case WARNING:
        group = warningActionGroup;
        listAction->setIcon(Theme::theme()->getIcon("Actions", "Warning"));
        break;
    case CRITICAL:
        group = criticalActionGroup;
        listAction->setIcon(Theme::theme()->getIcon("Actions", "Critical"));
        break;
    default:
        group = infoActionGroup;
        listAction->setIcon(Theme::theme()->getIcon("Actions", "Info"));
        break;
    }

    QListWidgetItem* closeButtonItem = new QListWidgetItem;
    closeButtonItem->setIcon(Theme::theme()->getIcon("Actions", "Close"));
    closeButtonListWidget->addItem(closeButtonItem);

    group->addAction(listAction);
    closeButtons.insertMulti(group, closeButtonItem);
    */
}


/**
 * @brief NotificationDialog::themeChanged
 */
void NotificationDialog::themeChanged()
{
    Theme* theme = Theme::theme();
    QString toolButtonStyle = "QToolButton{ border-color:" + theme->getAltBackgroundColorHex() + "; border-radius: 2px; }"
                              "QToolButton:hover{ background:" + theme->getHighlightColorHex() + ";"
                              "color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";}";

    clearAllButton->setStyleSheet(toolButtonStyle);
    clearSelectedButton->setStyleSheet(toolButtonStyle);

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

    //listWidget->setStyleSheet("border: 10px solid red;");

    foreach (QListWidgetItem* item, icons.keys()) {
        item->setIcon(theme->getIcon(icons.value(item)));
    }
}


/**
 * @brief NotificationDialog::displayTypeChanged
 * @param type
 */
void NotificationDialog::displayTypeChanged(int type)
{
    /*
    NOTIFICATION_TYPE nType = (NOTIFICATION_TYPE) type;
    switch (nType) {
    case INFO:

        break;
    case WARNING:

        break;
    case CRITICAL:

        break;
    default:

        break;
    }
    */
}


/**
 * @brief NotificationDialog::clearAll
 */
void NotificationDialog::clearAll()
{
    listWidget->clear();
    typeIconListWidget->clear();
    icons.clear();
}


/**
 * @brief NotificationDialog::clearSelected
 */
void NotificationDialog::clearSelected()
{
    QList<QListWidgetItem*> selectedItems = listWidget->selectedItems();
    while (!selectedItems.isEmpty()) {
       QListWidgetItem* item = selectedItems.takeFirst();
       removeListItem(item);
    }
}


/**
 * @brief NotificationDialog::addNotificationItem
 * @param type
 * @param title
 * @param description
 * @param iconPath
 */
void NotificationDialog::addNotificationItem(NOTIFICATION_TYPE type, QString title, QString description, QPair<QString, QString> iconPath)
{
    QIcon icon = Theme::theme()->getIcon(iconPath);
    if (icon.isNull()) {
        iconPath.first = "Actions";
        iconPath.second = "Help";
        icon = Theme::theme()->getIcon(iconPath);
    }
    addListItem(type, icon, title, description);
}


/**
 * @brief NotificationDialog::addListItem
 * @param type
 * @param icon
 * @param title
 * @param description
 */
void NotificationDialog::addListItem(NOTIFICATION_TYPE type, QIcon icon, QString title, QString description)
{
    QListWidgetItem* listItem = new QListWidgetItem;
    listItem->setText(description);
    //listItem->setIcon(icon);
    listItem->setData(Qt::UserRole, QVariant(type));
    listWidget->addItem(listItem);

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

    listItem->setIcon(Theme::theme()->getIcon(iconPath));
    icons[listItem] = iconPath;

    QListWidgetItem* iconItem = new QListWidgetItem;
    //iconItem->setIcon(typeIcon);
    typeIconListWidget->addItem(iconItem);
}


/**
 * @brief NotificationDialog::removeListItem
 * @param item
 */
void NotificationDialog::removeListItem(QListWidgetItem* item)
{
    if (icons.contains(item)) {
        icons.remove(item);
    }
    int row = listWidget->row(item);
    delete listWidget->takeItem(row);
    delete typeIconListWidget->takeItem(row);
}


/**
 * @brief NotificationDialog::listItemClicked
 * @param item
 */
void NotificationDialog::listItemClicked(QListWidgetItem* item)
{
    /*
    int row = listWidget->row(item);
    QListWidgetItem* closeItem = typeIconListWidget->item(row);
    typeIconListWidget->setItemSelected(closeItem, true);
    */
}

