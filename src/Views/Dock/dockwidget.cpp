#include "dockwidget.h"
#include "../../theme.h"

#include <QDebug>

//TODO: REWRITE DOCKWIDGETS to handle ID's and removal of entities.
//Should allow easier creation of groups. Potentially have a SET_KIND as GROUP kind of functionality which allows auto construction of the write type of dock widgets

/**
 * @brief DockWidget::DockWidget
 * @param tc
 * @param type
 * @param parent
 */
DockWidget::DockWidget(ToolbarController* tc, ToolbarController::DOCK_TYPE type, QWidget *parent) : QWidget(parent)
{
    toolActionController = tc;
    dockType = type;

    prevHighlightedItem = 0;
    prevHighlightedItemID = -1;

    setupLayout();
    displayInfoLabel(false);

    switch (dockType) {
    case ToolbarController::DEFINITIONS:
    case ToolbarController::FUNCTIONS:
        setupHeaderLayout();
        break;
    default:
        kindLabel = 0;
        backButton = 0;
        break;
    }

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}


/**
 * @brief DockWidget::addItem
 * @param item
 */
void DockWidget::addItem(DockWidgetItem* item)
{
    if (childrenItems.contains(item)) {
        return;
    }

    DockWidgetItem::DOCKITEM_KIND itemKind = item->getItemKind();
    bool isActionItem = itemKind == DockWidgetItem::ACTION_ITEM;
    bool isParentActionItem = itemKind == DockWidgetItem::GROUP_ACTION_ITEM;
    if (isActionItem || isParentActionItem) {
        childrenItems.append(item);
    }


    if (isActionItem) {
        DockWidgetActionItem* actionItem = (DockWidgetActionItem*)item;
        actionItem->setProperty("kind", actionItem->getAction()->text());
        connect(actionItem, SIGNAL(clicked(bool)), this, SLOT(dockActionClicked()));
        actionItemIDHash[actionItem->getProperty("ID").toInt()] = actionItem;
        itemsLayout->addWidget(actionItem);
    } else if (isParentActionItem) {
        DockWidgetParentActionItem* parentItem = (DockWidgetParentActionItem*)item;
        QToolBar* toolbar = new QToolBar(this);
        toolbar->addWidget(item);
        toolbar->setIconSize(QSize(16,16));
        parentActionItemIDHash[parentItem->getProperty("ID").toInt()] = parentItem;
        itemToolbarHash[item] = toolbar;
        itemsLayout->addWidget(toolbar);
    } else {
        itemsLayout->addWidget(item);
    }

    if (itemKind != DockWidgetItem::DEFAULT_ITEM) {
        connect(item, &DockWidgetItem::hoverEnter, toolActionController, &ToolbarController::actionHoverEnter);
        connect(item, &DockWidgetItem::hoverLeave, toolActionController, &ToolbarController::actionHoverLeave);
    }
}


/**
 * @brief DockWidget::addItems
 * @param items
 */
void DockWidget::addItems(QList<DockWidgetItem*> items)
{
    foreach (DockWidgetItem* item, items) {
        addItem(item);
    }
}


/**
 * @brief DockWidget::addItems
 * @param actions
 */
void DockWidget::addItems(QList<QAction*> actions)
{
    foreach (QAction* action, actions) {
        DockWidgetActionItem* dockAction = new DockWidgetActionItem(action, this);
        dockAction->setProperty("kind", action->text());
        addItem(dockAction);
    }
}


/**
 * @brief DockWidget::clearDock
 */
void DockWidget::clearDock()
{
    while (!childrenItems.isEmpty()) {
        DockWidgetItem* item = childrenItems.takeFirst();
        if (item) {
            if (itemToolbarHash.contains(item)) {
                // if there is a toolbar container for the dock item, delete it
                QToolBar* tb = itemToolbarHash.take(item);
                itemsLayout->removeWidget(tb);
                delete tb;
            } else {
                itemsLayout->removeWidget(item);
                delete item;
            }
        }
    }

    /*
    // for some reason, this doesn't delete all the widgets inside of mainLayout
    QList<QWidget*> widgets = mainLayout->findChildren<QWidget*>();
    foreach (QWidget* w, widgets) {
        mainLayout->removeWidget(w);
        delete w;
    }
    */
}


/**
 * @brief DockWidget::isEmpty
 * @return 
 */
bool DockWidget::isEmpty()
{
    return childrenItems.isEmpty();
}


/**
 * @brief DockWidget::updateHeaderText
 * @param text
 */
void DockWidget::updateHeaderText(QString text)
{
    if (kindLabel) {
        kindLabel->setText(text);
    }
}


/**
 * @brief DockWidget::updateInfoLabel
 * @param text
 */
void DockWidget::updateInfoLabel(QString text)
{
    infoLabel->setText(text);
}


/**
 * @brief DockWidget::displayInfoLabel
 * @param display
 */
void DockWidget::displayInfoLabel(bool display)
{
    infoLabel->setVisible(display);
}


/**
 * @brief DockWidget::getDockType
 * @return
 */
ToolbarController::DOCK_TYPE DockWidget::getDockType()
{
    return dockType;
}


/**
 * @brief DockWidget::themeChanged
 */
void DockWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("#DOCKWIDGET_MAINWIDGET {"
                  "background:" + theme->getBackgroundColorHex() + ";"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "}"
                  "#DOCKWIDGET_SCROLLWIDGET {"
                  "background: rgba(0,0,0,0);"
                  "border: 0px;"
                  "}"
                  "QScrollArea {"
                  "background: rgba(0,0,0,0);"
                  "border: 0px;"
                  "}");
/*
    mainWidget->setStyleSheet("#DOCKWIDGET_SCROLLWIDGET {"
                              "background: rgba(0,0,0,0);"
                              "border: 0px;"
                              "}");

   scrollArea->setStyleSheet("QScrollArea {"
                             "background: rgba(0,0,0,0);"
                             "border: 0px;"
                             "}");
*/
    if (backButton) {
        backButton->setIcon(theme->getIcon("Icons", "arrowLeft"));
        backButton->setStyleSheet(theme->getToolBarStyleSheet() +
                                  "QToolButton {"
                                  "background:" + theme->getAltBackgroundColorHex() + ";"
                                  "border-radius: " + theme->getSharpCornerRadius() + ";"
                                  "}");
    }
}


/**
 * @brief DockWidget::dockActionClicked
 */
void DockWidget::dockActionClicked()
{
    DockWidgetActionItem* senderAction = qobject_cast<DockWidgetActionItem*>(sender());
    emit actionClicked(senderAction);
}


/**
 * @brief DockWidget::highlightItem
 * @param ID
 */
void DockWidget::highlightItem(int ID)
{
    if (ID == -1 && prevHighlightedItem) {
        // remove highlight
        prevHighlightedItem->highlightItem(false);
        prevHighlightedItem = 0;
        prevHighlightedItemID = -1;
    } else {
        if (actionItemIDHash.contains(ID)) {
            // highlight item
            DockWidgetActionItem* item = actionItemIDHash.value(ID);
            item->highlightItem(true);
            scrollArea->ensureWidgetVisible(item);
            prevHighlightedItem = item;
            prevHighlightedItemID = ID;
        }
    }
}


void DockWidget::viewItemGroupConstructed(int ID){
    if (!actionItemIDHash.contains(ID)) {
        auto node_action = toolActionController->getNodeAction(ID)->constructSubAction(false);
        auto dockItem = new DockWidgetParentActionItem(node_action, this);
        dockItem->setProperty("ID", ID);
        addItem(dockItem);
    }
}
/**
 * @brief DockWidget::viewItemConstructed
 * @param ID
 */
void DockWidget::viewItemConstructed(int ID)
{
    // can have an action item and a parent action item with the same ID
    if (!actionItemIDHash.contains(ID)) {
        auto node_action = toolActionController->getNodeAction(ID)->constructSubAction(false);
        auto dockItem = new DockWidgetActionItem(node_action, this);
        dockItem->setProperty("ID", ID);
        addItem(dockItem);
    }

    /*
    // TODO - Do the grouping by parent view item actions here
    switch (dockType) {
    case ToolbarController::DEFINITIONS:
    case ToolbarController::FUNCTIONS:

        QAction

        DockWidgetParentActionItem* parentItem = parentActionItemIDHash.value(ID, 0);
        if (!parentItem) {
            parentItem =
        }

        break;
    default:
        break;
    }
    */
}


/**
 * @brief ToolbarWidgetNew::viewItemDestructed
 * @param ID
 */
void DockWidget::viewItemDestructed(int ID)
{
    if (actionItemIDHash.contains(ID)) {
        DockWidgetItem* item = actionItemIDHash.take(ID);
        item->deleteLater();
    }
    if (parentActionItemIDHash.contains(ID)) {
        DockWidgetItem* item = parentActionItemIDHash.take(ID);
        item->deleteLater();
    }
    if (prevHighlightedItem && prevHighlightedItemID == ID) {
        prevHighlightedItem->deleteLater();
        prevHighlightedItem = 0;
        prevHighlightedItemID = -1;
    }
}


/**
 * @brief DockWidget::setupLayout
 */
void DockWidget::setupLayout()
{
    setObjectName("DOCKWIDGET_MAINWIDGET");

    infoLabel = new QLabel(this);
    infoLabel->setWordWrap(true);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    infoLabel->setStyleSheet("margin: 10px 2px 0px 2px; padding: 0px;");
    infoLabel->setFont(QFont(font().family(), 8));

    itemsLayout = new QVBoxLayout();
    itemsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    itemsLayout->setSizeConstraint(QLayout::SetMinimumSize);
    itemsLayout->setSpacing(5);
    itemsLayout->setMargin(0);

    mainWidget = new QWidget(this);
    mainWidget->setObjectName("DOCKWIDGET_SCROLLWIDGET");
    mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout* scrollLayout = new QVBoxLayout(mainWidget);
    scrollLayout->setMargin(0);
    scrollLayout->addWidget(infoLabel);
    scrollLayout->addLayout(itemsLayout);
    scrollLayout->addStretch();

    scrollArea = new QScrollArea(this);
    scrollArea->setWidget(mainWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->addWidget(scrollArea);
}


/**
 * @brief DockWidget::setupHeaderLayout
 */
void DockWidget::setupHeaderLayout()
{
    backButton = new QToolButton(this);
    backButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    backButton->setToolTip("Go back to the parts list");
    connect(backButton, SIGNAL(clicked(bool)), this, SIGNAL(backButtonClicked()));

    QToolBar* toolbar = new QToolBar(this);
    toolbar->addWidget(backButton);
    toolbar->setIconSize(QSize(16,16));

    QLabel* descriptionLabel = new QLabel("Select to construct", this);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setAlignment(Qt::AlignCenter);
    descriptionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    descriptionLabel->setStyleSheet("margin: 10px 0px 0px 0px; padding: 0px;");
    descriptionLabel->setFont(QFont(font().family(), 8));

    kindLabel = new DockWidgetItem("This is a description for kind", this);
    kindLabel->setFont(QFont(font().family(), 8));

    QVBoxLayout* headerLayout = new QVBoxLayout();
    headerLayout->setMargin(0);
    headerLayout->setSpacing(0);
    headerLayout->addWidget(descriptionLabel);
    headerLayout->addWidget(kindLabel);
    headerLayout->addSpacerItem(new QSpacerItem(0,5));
    headerLayout->addWidget(toolbar);

    mainLayout->insertLayout(0, headerLayout);
}

