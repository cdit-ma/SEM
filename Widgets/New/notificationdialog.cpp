#include "notificationdialog.h"
#include "../../View/theme.h"

#include <QVBoxLayout>
#include <QToolBar>
#include <QLabel>

#define ICON_SIZE 24


/**
 * @brief NotificationDialog::NotificationDialog
 * @param parent
 */
NotificationDialog::NotificationDialog(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* topHLayout = new QHBoxLayout();
    QHBoxLayout* bottomHLayout = new QHBoxLayout();

    listWidget = new QListWidget(this);
    closeButtonListWidget = new QListWidget(this);

    listWidget->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    listWidget->setSortingEnabled(true);
    listWidget->setUniformItemSizes(true);

    closeButtonListWidget->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    closeButtonListWidget->setFixedWidth(ICON_SIZE + 10);

    clearButton = new QToolButton(this);
    clearButton->setText("Clear All");

    typeComboBox = new QComboBox(this);
    typeComboBox->setFixedHeight(clearButton->sizeHint().height());
    typeComboBox->addItem("Notification");
    typeComboBox->addItem("Warning");
    typeComboBox->addItem("Critical");
    typeComboBox->addItem("All");

    QWidget* stretcher = new QWidget(this);
    stretcher->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar->addWidget(typeComboBox);
    toolbar->addWidget(stretcher);
    toolbar->addWidget(clearButton);

    mainLayout->addLayout(topHLayout);
    mainLayout->addLayout(bottomHLayout);

    topHLayout->addWidget(toolbar, 1);

    bottomHLayout->setSpacing(0);
    bottomHLayout->addWidget(listWidget, 1);
    bottomHLayout->addWidget(closeButtonListWidget);

    addListItem("This is an info message");
    addListItem("This is a warning message", WARNING);
    addListItem("This is a critical message", CRITICAL);

    /*
    QLabel* label = new QLabel("This is a test item.", this);
    QToolButton* button = new QToolButton(this);
    button->setText("X");

    QListWidget* testWidget = new QListWidget(this);
    testWidget->setStyleSheet("background: rgba(0,0,0,0);");
    QHBoxLayout* h = new QHBoxLayout(testWidget);
    h->addWidget(label);
    h->addWidget(button);
    testWidget->setFixedHeight(ICON_SIZE);

    QListWidgetItem* item = new QListWidgetItem;
    listWidget->addItem(item);
    listWidget->setItemWidget(item, testWidget);
    */

    setWindowTitle("Notifications");
    themeChanged();

    connect(listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(listItemClicked(QListWidgetItem*)));
    connect(closeButtonListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(removeListItem(QListWidgetItem*)));
    connect(clearButton, SIGNAL(clicked(bool)), this, SLOT(clearAll()));
    connect(typeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(displayTypeChanged(int)));
}


/**
 * @brief NotificationDialog::addListItem
 * @param type
 * @param description
 */
void NotificationDialog::addListItem(QString description, NOTIFICATION_TYPE type)
{
    QListWidgetItem* listItem = new QListWidgetItem;
    listItem->setText(description);
    listItem->setData(Qt::UserRole, QVariant(type));
    listWidget->addItem(listItem);

    switch (type) {
    case WARNING:
        listItem->setIcon(Theme::theme()->getIcon("Actions", "Warning"));
        break;
    case CRITICAL:
        listItem->setIcon(Theme::theme()->getIcon("Actions", "Critical"));
        break;
    default:
        listItem->setIcon(Theme::theme()->getIcon("Actions", "Info"));
        break;
    }

    QListWidgetItem* closeButtonItem = new QListWidgetItem;
    closeButtonItem->setIcon(Theme::theme()->getIcon("Actions", "Close"));
    closeButtonListWidget->addItem(closeButtonItem);
}


/**
 * @brief NotificationDialog::addListAction
 * @param description
 * @param type
 */
void NotificationDialog::addListAction(QString description, NotificationDialog::NOTIFICATION_TYPE type)
{
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
}


/**
 * @brief NotificationDialog::themeChanged
 */
void NotificationDialog::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("background:" + theme->getBackgroundColorHex() + "; color:" + theme->getTextColorHex() + ";");
    clearButton->setStyleSheet("QToolButton{ border-color:" + theme->getAltBackgroundColorHex() + "; border-radius: 2px; }"
                               "QToolButton:hover{ background:" + theme->getHighlightColorHex() + ";"
                               "color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";"
                                                                                       "}");
}


/**
 * @brief NotificationDialog::displayTypeChanged
 * @param type
 */
void NotificationDialog::displayTypeChanged(int type)
{
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
}


/**
 * @brief NotificationDialog::clearAll
 */
void NotificationDialog::clearAll()
{
    listWidget->clear();
    closeButtonListWidget->clear();
}


/**
 * @brief NotificationDialog::removeListItem
 * @param item
 */
void NotificationDialog::removeListItem(QListWidgetItem* item)
{
    int row = closeButtonListWidget->row(item);
    delete listWidget->takeItem(row);
    delete closeButtonListWidget->takeItem(row);
}


/**
 * @brief NotificationDialog::listItemClicked
 * @param item
 */
void NotificationDialog::listItemClicked(QListWidgetItem* item)
{
    int row = listWidget->row(item);
    QListWidgetItem* closeItem = closeButtonListWidget->item(row);
    closeButtonListWidget->setItemSelected(closeItem, true);
}

