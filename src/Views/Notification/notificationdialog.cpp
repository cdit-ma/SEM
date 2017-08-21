#include "notificationdialog.h"
#include "notificationitem.h"

#define ENTITY_ID "entity_ID"
#define NO_SELECTION_ID -2

#define FILTER_KEY "filter_key"
#define FILTER_DEFAULT_WIDTH 150

#define ICON_SIZE 24


/**
 * @brief NotificationDialog::NotificationDialog
 * @param parent
 */
NotificationDialog::NotificationDialog(QWidget *parent)
    : QFrame(parent)
{
    selectedEntityID = -1;

    setupLayout();
    initialisePanel();
    updateSelectionBasedButtons();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}


/**
 * @brief NotificationDialog::filtersChanged
 */
void NotificationDialog::filtersChanged()
{
    OptionGroupBox* filterGroup = qobject_cast<OptionGroupBox*>(sender());
    if (filterGroup) {

        NOTIFICATION_FILTER filterKey = filterGroup->property(FILTER_KEY).value<NOTIFICATION_FILTER>();
        QList<QVariant> checkedFilters = filterGroup->getCheckedKeys();

        switch (filterKey) {
        case NOTIFICATION_FILTER::SEVERITY:
            foreach (NotificationItem* item, notificationItems) {
                int s = static_cast<int>(item->getSeverity());
                item->filtersChanged(filterKey, checkedFilters.contains(s));
            }
            break;
        case NOTIFICATION_FILTER::CATEGORY:
            foreach (NotificationItem* item, notificationItems) {
                int c = static_cast<int>(item->getCategory());
                item->filtersChanged(filterKey, checkedFilters.contains(c));
            }
            break;
        case NOTIFICATION_FILTER::TYPE:
            foreach (NotificationItem* item, notificationItems) {
                int t = static_cast<int>(item->getType());
                item->filtersChanged(filterKey, checkedFilters.contains(t));
            }
            break;
        default:
            break;
        }
    }
}


/**
 * @brief NotificationDialog::viewSelection
 * Center on or popout the corresponding entity for the selected notification item.
 */
void NotificationDialog::viewSelection()
{
    int numSelectedItems = selectedNotificationItems.count();
    if (numSelectedItems != 1) {
        return;
    }

    NotificationItem* selectedItem = selectedNotificationItems.at(0);
    int eID = selectedItem->getEntityID();
    if (sender() == centerOnAction) {
        emit centerOn(eID);
    } else if (sender() == popupAction) {
        emit popup(eID);
    }
}


/**
 * @brief NotificationDialog::themeChanged
 */
void NotificationDialog::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QScrollArea {"
                  "background:" + theme->getBackgroundColorHex() + ";"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "}"
                  "QLabel{ background: rgba(0,0,0,0); color:" + theme->getTextColorHex()+ ";}"
                  + theme->getAltAbstractItemViewStyleSheet()
                  + theme->getDialogStyleSheet());

    mainWidget->setStyleSheet("background:" + theme->getBackgroundColorHex() + ";");

    topToolbar->setStyleSheet(theme->getToolBarStyleSheet() + "QToolBar{ padding: 0px 4px; } QToolButton{ padding: 4px; }");
    bottomToolbar->setStyleSheet(theme->getToolBarStyleSheet() + "QToolBar{ padding: 0px 4px; } QToolButton{ padding: 4px; }");
    filtersToolbar->setStyleSheet(theme->getToolBarStyleSheet() +
                                  "QToolBar{ padding: 0px; spacing: 0px; }"
                                  "QToolButton{ border-radius:" + theme->getSharpCornerRadius() + ";}");

    sortTimeAction->setIcon(theme->getIcon("Icons", "clock"));
    sortSeverityAction->setIcon(theme->getIcon("Icons", "letterAZ"));
    centerOnAction->setIcon(theme->getIcon("Icons", "crosshair"));
    popupAction->setIcon(theme->getIcon("Icons", "popOut"));
    clearSelectedAction->setIcon(theme->getIcon("Icons", "bin"));
    clearVisibleAction->setIcon(theme->getIcon("Icons", "cross"));

    displayAllButton->setIcon(theme->getIcon("Icons", "list"));
    displayLinkedItemsButton->setIcon(theme->getIcon("Icons", "bug"));

    displaySplitter->setStyleSheet(theme->getSplitterStyleSheet());
}


/**
 * @brief NotificationDialog::selectionChanged
 * @param item - the notification item that was clicked
 * @param selected - the item's current selected state
 * @param controlDown - control key's down state
 */
void NotificationDialog::selectionChanged(NotificationItem* item, bool selected, bool controlDown)
{
    if (!item) {
        return;
    }

    bool selectItem = !selected;
    if (!controlDown) {
        if (selectedNotificationItems.count() > 1) {
            selectItem = true;
        }
        clearSelection();
    }

    if (selectItem) {
        selectedNotificationItems.append(item);
    } else {
        selectedNotificationItems.removeAll(item);
    }

    item->setSelected(selectItem);
    updateSelectionBasedButtons();
}


/**
 * @brief NotificationDialog::entitySelectionChanged
 * Update the selectedEntityID and the displayed notifications if necessary if this panel is visible.
 * @param ID
 */
void NotificationDialog::entitySelectionChanged(int ID)
{
    selectedEntityID = ID;
    if (this->isVisible() && displayLinkedItemsButton->isChecked()) {
        selectionFilterToggled(true);
    }
}


/**
 * @brief NotificationDialog::selectionFilterToggled
 * @param checked
 */
void NotificationDialog::selectionFilterToggled(bool checked)
{
    QToolButton* senderButton = qobject_cast<QToolButton*>(sender());
    QToolButton* otherButton = 0;

    if (senderButton == displayAllButton) {
        // don't allow the "All" button to be unchecked manually
        if (!checked) {
            displayAllButton->setChecked(true);
            return;
        }
        otherButton = displayLinkedItemsButton;
    } else {
        otherButton = displayAllButton;
    }
    otherButton->setChecked(!checked);

    bool displayAll = displayAllButton->isChecked();
    foreach (NotificationItem* item, notificationItems) {
        if (displayAll) {
            // if "All" display is checked, show all the items
            item->filtersChanged(ENTITY_ID, item->getEntityID());
        } else {
            // if there is nothing currently selected, hide the items
            if (selectedEntityID == -1) {
                item->filtersChanged(ENTITY_ID, NO_SELECTION_ID);
            } else {
                item->filtersChanged(ENTITY_ID, selectedEntityID);
            }
        }
    }
    QString newBoxTitle = "CONTEXT";
    if (!displayAll) {
        newBoxTitle += " (1)";
    }
    displayToggleBox->setTitle(newBoxTitle);
}


/**
 * @brief NotificationDialog::resetPanel
 */
void NotificationDialog::resetPanel()
{
    clearAll();
}


/**
 * @brief NotificationDialog::getLastNotificationID
 * Send a signal with the top most notification item's ID.
 */
void NotificationDialog::getLastNotificationID()
{
    if (!notificationItems.isEmpty()) {
        QLayoutItem* topItem = itemsLayout->itemAt(0);
        if (topItem) {
            NotificationItem* item = qobject_cast<NotificationItem*>(topItem->widget());
            if (item) {
                emit lastNotificationID(item->getID());
                return;
            }
        }
    }
    emit lastNotificationID(-1);
}


/**
 * @brief NotificationDialog::clearSelected
 */
void NotificationDialog::clearSelected()
{
    // delete selected items
    while (!selectedNotificationItems.isEmpty()) {
        removeItem(selectedNotificationItems.takeFirst());
    }
}


/**
 * @brief NotificationDialog::clearVisible
 */
void NotificationDialog::clearVisible()
{
    QList<NotificationItem*> visibleItems;
    foreach (NotificationItem* item, notificationItems) {
        if (item->isVisible()) {
            visibleItems.append(item);
        }
    }

    // delete visible items
    while (!visibleItems.isEmpty()) {
        removeItem(visibleItems.takeFirst());
    }
}


/**
 * @brief NotificationDialog::clearNotifications
 * @param filter
 * @param filterVal
 */
void NotificationDialog::clearNotifications(NOTIFICATION_FILTER filter, int filterVal)
{
    QList<NotificationItem*> itemsToDelete;
    foreach (NotificationItem* item, notificationItems.values()) {
        switch (filter) {
        case NOTIFICATION_FILTER::SEVERITY:
        {
            if (item->getSeverity() == (NOTIFICATION_SEVERITY)filterVal) {
                itemsToDelete.append(item);
            }
            break;
        }
        case NOTIFICATION_FILTER::TYPE:
        {
            if (item->getType() == (NOTIFICATION_TYPE)filterVal) {
                itemsToDelete.append(item);
            }
            break;
        }
        case NOTIFICATION_FILTER::CATEGORY:
        {
            if (item->getCategory() == (NOTIFICATION_CATEGORY)filterVal) {
                itemsToDelete.append(item);
            }
            break;
        }
        default:
            break;
        }
    }
    foreach (NotificationItem* item, itemsToDelete) {
        removeItem(item);
    }
}


/**
 * @brief NotificationDialog::notificationAdded
 * @param obj
 */
void NotificationDialog::notificationAdded(NotificationObject* obj)
{
    if (!obj) {
        return;
    }

    NotificationItem* item = new NotificationItem(obj, this);
    itemsLayout->insertWidget(0, item);
    notificationItems[obj->ID()] = item;

    // add filter for actively selected entity ID
    item->addFilter(ENTITY_ID, item->getEntityID());

    connect(item, SIGNAL(hoverEnter(int)), this, SIGNAL(itemHoverEnter(int)));
    connect(item, SIGNAL(hoverLeave(int)), this, SIGNAL(itemHoverLeave(int)));
    connect(item, &NotificationItem::itemClicked, this, &NotificationDialog::selectionChanged);

    // update the search item's visibility based on the currently checked filters
    // this also sets the correct visibility values for each of its filters
    foreach (NOTIFICATION_FILTER filter, getNotificationFilters()) {
        OptionGroupBox* group = filters.value(filter, 0);
        if (group) {
            int filterVal = item->getNotificationFilterValue(filter);
            bool showItem = group->getCheckedKeys().contains(filterVal);
            item->filtersChanged(filter, showItem);
        }
    }
    if (!displayAllButton->isChecked()) {
        item->filtersChanged(ENTITY_ID, selectedEntityID);
    }
}


/**
 * @brief NotificationDialog::notificationDeleted
 * Delete notification item with the provided ID from the hash and the items layout.
 * @param ID
 */
void NotificationDialog::notificationDeleted(int ID)
{
    if (notificationItems.contains(ID)) {
        NotificationItem* item = notificationItems.take(ID);
        selectedNotificationItems.removeAll(item);
        itemsLayout->removeWidget(item);
        updateSelectionBasedButtons();
        delete item;
    }
}


/**
 * @brief NotificationDialog::clearAll
 * NOTE: This function is the only one that doesn't use notificationDeleted to delete items.
 * It is only called when the notification panel is being reset.
 */
void NotificationDialog::clearAll()
{
    // remove widgets from the items layout
    QLayoutItem* child;
    while ((child = itemsLayout->takeAt(0)) != 0) {
        delete child;
    }

    notificationItems.clear();
    selectedNotificationItems.clear();

    // reset checked filter buttons and checked filter lists
    updateSelectionBasedButtons();
}


/**
 * @brief NotificationDialog::clearSelection
 */
void NotificationDialog::clearSelection()
{
    foreach (NotificationItem* item, selectedNotificationItems) {
        item->setSelected(false);
    }
    selectedNotificationItems.clear();
    updateSelectionBasedButtons();
}


/**
 * @brief NotificationDialog::initialisePanel
 * Construct items for notifications that were received before this dialog was constructed.
 * Show highlight alert and toast for the last notification item in the list.
 */
void NotificationDialog::initialisePanel()
{
    QList<NotificationObject*> notifications = NotificationManager::manager()->getNotificationItems();
    if (!notifications.isEmpty()) {
        foreach (NotificationObject* item, notifications) {
            notificationAdded(item);
        }
        //NotificationManager::manager()->showLastNotification();
    }
}


/**
 * @brief NotificationDialog::enterEvent
 * If this dialog is currently active, mark new notification as seen on mouse enter.
 * @param event
 */
void NotificationDialog::enterEvent(QEvent* event)
{
    QWidget::enterEvent(event);
    if (QApplication::activeWindow() == this) {
        emit mouseEntered();
    }
}


/**
 * @brief NotificationDialog::removeItem
 * This requests to delete the provided notification item.
 * Error items cannot be deleted using the notification dialog.
 * @param item
 */
void NotificationDialog::removeItem(NotificationItem* item)
{
    if (item) {
        NOTIFICATION_SEVERITY severity = item->getSeverity();
        if (severity != NOTIFICATION_SEVERITY::ERROR) {
            emit deleteNotification(item->getID());
        }
    }
}


/**
 * @brief NotificationDialog::setupLayout
 */
void NotificationDialog::setupLayout()
{
    /*
     * TOP TOOLBAR
     */
    topToolbar = new QToolBar(this);
    topToolbar->setIconSize(QSize(20,20));

    QWidget* stretchWidget = new QWidget(this);
    stretchWidget->setStyleSheet("background: rgba(0,0,0,0);");
    stretchWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topToolbar->addWidget(stretchWidget);

    centerOnAction = topToolbar->addAction("");
    centerOnAction->setToolTip("Center View Aspect On Selected Item");

    popupAction = topToolbar->addAction("");
    popupAction->setToolTip("View Selected Item In New Window");

    connect(centerOnAction, &QAction::triggered, this, &NotificationDialog::viewSelection);
    connect(popupAction, &QAction::triggered, this, &NotificationDialog::viewSelection);

    /*
     * BOTTOM TOOLBAR
     */
    bottomToolbar = new QToolBar(this);
    bottomToolbar->setIconSize(QSize(20,20));

    QWidget* stretchWidget2 = new QWidget(this);
    stretchWidget2->setStyleSheet("background: rgba(0,0,0,0);");
    stretchWidget2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bottomToolbar->addWidget(stretchWidget2);

    sortTimeAction = bottomToolbar->addAction("");
    sortTimeAction->setToolTip("Sort Notifications By Time");
    sortTimeAction->setCheckable(true);
    sortTimeAction->setChecked(true);
    sortTimeAction->setVisible(false);

    sortSeverityAction = bottomToolbar->addAction("");
    sortSeverityAction->setToolTip("Sort Notifications By Severity");
    sortSeverityAction->setCheckable(true);
    sortSeverityAction->setChecked(false);
    sortSeverityAction->setVisible(false);

    //bottomToolbar->addSeparator();

    clearSelectedAction = bottomToolbar->addAction("Clear Selected");
    clearSelectedAction->setToolTip("Clear Selected Items");

    clearVisibleAction = bottomToolbar->addAction("Clear Visible");
    clearVisibleAction->setToolTip("Clear Visible Items");

    connect(clearSelectedAction, &QAction::triggered, this, &NotificationDialog::clearSelected);
    connect(clearVisibleAction, &QAction::triggered, this, &NotificationDialog::clearVisible);

    QActionGroup* sortGroup = new QActionGroup(this);
    sortGroup->setExclusive(true);
    sortGroup->addAction(sortTimeAction);
    sortGroup->addAction(sortSeverityAction);

    /*
     * FILTERS (LEFT) TOOLBAR
     */
    filtersToolbar = new QToolBar(this);
    filtersToolbar->setOrientation(Qt::Vertical);
    filtersToolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    filtersToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QScrollArea* filtersArea = new QScrollArea(this);
    filtersArea->setWidget(filtersToolbar);
    filtersArea->setWidgetResizable(true);

    /*
     * DISPLAY SECTION
     */
    itemsLayout = new QVBoxLayout();
    itemsLayout->setMargin(0);
    itemsLayout->setSpacing(0);
    itemsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    itemsLayout->setSizeConstraint(QLayout::SetMinimumSize);

    QFrame* displayWidget = new QFrame(this);
    displayWidget->setStyleSheet("QFrame{ background: rgba(0,0,0,0); }");
    displayWidget->setLayout(itemsLayout);

    QScrollArea* displayArea = new QScrollArea(this);
    displayArea->setWidget(displayWidget);
    displayArea->setWidgetResizable(true);

    displaySplitter = new QSplitter(this);
    displaySplitter->addWidget(filtersArea);
    displaySplitter->addWidget(displayArea);
    displaySplitter->setStretchFactor(0, 0);
    displaySplitter->setStretchFactor(1, 1);
    displaySplitter->setSizes(QList<int>() << FILTER_DEFAULT_WIDTH << 300);

    mainWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(1, 1, 1, 1);
    mainLayout->addWidget(topToolbar);
    mainLayout->addWidget(displaySplitter, 1);
    mainLayout->addWidget(bottomToolbar);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(mainWidget);

    int minWidth = qMax(topToolbar->sizeHint().width(), bottomToolbar->sizeHint().width());
    int minHeight = topToolbar->sizeHint().height() + bottomToolbar->sizeHint().height() + 75;
    setMinimumSize(minWidth, minHeight);

    setupFilters();
}


/**
 * @brief NotificationDialog::setupFilters
 */
void NotificationDialog::setupFilters()
{
    displayAllButton = new QToolButton(this);
    displayAllButton->setText("All");
    displayAllButton->setCheckable(true);
    displayAllButton->setChecked(true);
    displayAllButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    displayAllButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    connect(displayAllButton, &QToolButton::clicked, this, &NotificationDialog::selectionFilterToggled);

    displayLinkedItemsButton = new QToolButton(this);
    displayLinkedItemsButton->setText("Selection");
    displayLinkedItemsButton->setCheckable(true);
    displayLinkedItemsButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    displayLinkedItemsButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    connect(displayLinkedItemsButton, &QToolButton::clicked, this, &NotificationDialog::selectionFilterToggled);

    displayToggleBox = new CustomGroupBox("CONTEXT", this);
    displayToggleBox->addWidget(displayAllButton);
    displayToggleBox->addWidget(displayLinkedItemsButton);
    filtersToolbar->addWidget(displayToggleBox);

    // setup SEVERITY group
    OptionGroupBox* severityGroup = new OptionGroupBox("SEVERITY", this);
    severityGroup->setProperty(FILTER_KEY, static_cast<int>(NOTIFICATION_FILTER::SEVERITY));
    foreach (NOTIFICATION_SEVERITY severity, getNotificationSeverities()) {
        QString iconName = getSeverityIcon(severity);
        severityGroup->addOption(static_cast<int>(severity), getSeverityString(severity), "Icons", iconName);
    }

    // setup CATEGORY group
    OptionGroupBox* categoryGroup = new OptionGroupBox("CATEGORY", this);
    categoryGroup->setProperty(FILTER_KEY, static_cast<int>(NOTIFICATION_FILTER::CATEGORY));
    foreach (NOTIFICATION_CATEGORY category, getNotificationCategories()){
        QString iconName = getCategoryIcon(category);
        categoryGroup->addOption(static_cast<int>(category), getCategoryString(category), "Icons", iconName);
    }

    // setup TYPE group
    OptionGroupBox* typeGroup = new OptionGroupBox("SOURCE", this);
    typeGroup->setProperty(FILTER_KEY, static_cast<int>(NOTIFICATION_FILTER::TYPE));
    foreach (NOTIFICATION_TYPE type, getNotificationTypes()) {
        QString iconName;
        if (type == NOTIFICATION_TYPE::MODEL) {
            iconName = "dotsInRectangle";
        } else if (type == NOTIFICATION_TYPE::APPLICATION) {
            iconName = "pencil";
        }
        typeGroup->addOption(static_cast<int>(type), getTypeString(type), "Icons", iconName);
    }

    // connect filter groups
    connect(severityGroup, &OptionGroupBox::checkedOptionsChanged, this, &NotificationDialog::filtersChanged);
    connect(categoryGroup, &OptionGroupBox::checkedOptionsChanged, this, &NotificationDialog::filtersChanged);
    connect(typeGroup, &OptionGroupBox::checkedOptionsChanged, this, &NotificationDialog::filtersChanged);

    // add filters to the toolbar
    filtersToolbar->addWidget(severityGroup);
    filtersToolbar->addWidget(categoryGroup);
    filtersToolbar->addWidget(typeGroup);

    filters.insert(NOTIFICATION_FILTER::SEVERITY, severityGroup);
    filters.insert(NOTIFICATION_FILTER::CATEGORY, categoryGroup);
    filters.insert(NOTIFICATION_FILTER::TYPE, typeGroup);
}


/**
 * @brief NotificationDialog::updateSelectionBasedButtons
 */
void NotificationDialog::updateSelectionBasedButtons()
{
    bool enable = !selectedNotificationItems.isEmpty();
    // TODO - Commented this out to match search centerOn and popup buttons
    //centerOnAction->setEnabled(enable);
    //popupAction->setEnabled(enable);
    clearSelectedAction->setEnabled(enable);
}
