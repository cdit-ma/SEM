#include "dockwidget.h"
#include "../theme.h"

#include <QDebug>

/**
 * @brief DockWidget::DockWidget
 * @param parent
 */
DockWidget::DockWidget(ToolActionController* tc, ToolActionController::DOCK_TYPE type, QWidget *parent) : QScrollArea(parent)
{
    toolActionController = tc;
    dockType = type;

    switch (dockType) {
    case ToolActionController::DEFINITIONS:
    case ToolActionController::FUNCTIONS:
        containsHeader = true;
        break;
    default:
        containsHeader = false;
        break;
    }

    infoLabel = new QLabel(this);
    infoLabel->setWordWrap(true);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    infoLabel->setStyleSheet("margin: 10px 0px 0px 0px; padding: 0px;");
    infoLabel->setFont(QFont(font().family(), 8));

    mainLayout = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
    mainLayout->setSpacing(5);
    mainLayout->setMargin(0);

    alignLayout = new QVBoxLayout();
    alignLayout->setMargin(0);
    alignLayout->addWidget(infoLabel);
    alignLayout->addLayout(mainLayout);
    alignLayout->addStretch();

    mainWidget = new QWidget(this);
    mainWidget->setObjectName("DOCKWIDGET_MAIN");
    mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainWidget->setLayout(alignLayout);

    setWidget(mainWidget);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setupHeaderLayout();
    displayInfoLabel(false);

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}


/**
 * @brief DockWidget::addItem
 * @param item
 * @return
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
        mainLayout->addWidget(actionItem);        
    } else if (isParentActionItem) {
        DockWidgetParentActionItem* parentItem = (DockWidgetParentActionItem*)item;
        QToolBar* toolbar = new QToolBar(this);
        toolbar->addWidget(item);
        parentActionItemIDHash[parentItem->getProperty("ID").toInt()] = parentItem;
        itemToolbarHash[item] = toolbar;
        mainLayout->addWidget(toolbar);
    } else {
        mainLayout->addWidget(item);
    }

    if (itemKind != DockWidgetItem::DEFAULT_ITEM) {
        connect(item, &DockWidgetItem::hoverEnter, toolActionController, &ToolActionController::actionHoverEnter);
        connect(item, &DockWidgetItem::hoverLeave, toolActionController, &ToolActionController::actionHoverLeave);
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
                mainLayout->removeWidget(tb);
                delete tb;
            } else {
                mainLayout->removeWidget(item);
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
ToolActionController::DOCK_TYPE DockWidget::getDockType()
{
    return dockType;
}


/**
 * @brief DockWidget::themeChanged
 */
void DockWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QWidget#DOCKWIDGET_MAIN{ background: rgba(0,0,0,0); }"
                  "QScrollArea {"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "background:" + theme->getBackgroundColorHex() + ";"
                  "}");

    if (backButton) {
        backButton->setIcon(theme->getIcon("Actions", "Arrow_Back"));
        backButton->setStyleSheet(theme->getToolBarStyleSheet() +
                                  "QToolButton {"
                                  "background:" + theme->getAltBackgroundColorHex() + ";"
                                  "border-radius: 2px;"
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
    } else {
        if (actionItemIDHash.contains(ID)) {
            // highlight item
            DockWidgetActionItem* item = actionItemIDHash.value(ID);
            item->highlightItem(true);
            prevHighlightedItem = item;
        }
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
        QAction* itemAction = toolActionController->getNodeAction(ID)->constructSubAction(false);
        DockWidgetActionItem* dockItem = new DockWidgetActionItem(itemAction, this);
        dockItem->setProperty("ID", ID);
        addItem(dockItem);
    }
}


/**
 * @brief ToolbarWidgetNew::viewItemDestructed
 * @param ID
 */
void DockWidget::viewItemDestructed(int ID)
{
    if (actionItemIDHash.contains(ID)) {
        DockWidgetItem* item = actionItemIDHash.take(ID);
        delete item;
    }
    if (parentActionItemIDHash.contains(ID)) {
        DockWidgetItem* item = parentActionItemIDHash.take(ID);
        delete item;
    }
}


/**
 * @brief DockWidget::setupHeaderLayout
 */
void DockWidget::setupHeaderLayout()
{
    if (!containsHeader) {
        headerLayout = 0;
        kindLabel = 0;
        backButton = 0;
        return;
    }

    backButton = new QToolButton(this);
    backButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    backButton->setToolTip("Go back to the parts list");
    backButton->setFixedHeight(23);
    connect(backButton, SIGNAL(clicked(bool)), this, SIGNAL(backButtonClicked()));

    QToolBar* toolbar = new QToolBar(this);
    toolbar->addWidget(backButton);

    QLabel* descriptionLabel = new QLabel("Select to construct", this);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setAlignment(Qt::AlignCenter);
    descriptionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    descriptionLabel->setStyleSheet("margin: 10px 0px 0px 0px; padding: 0px;");
    descriptionLabel->setFont(QFont(font().family(), 8));

    kindLabel = new DockWidgetItem("This is a description for kind", this);
    kindLabel->setFont(QFont(font().family(), 8));

    headerLayout = new QVBoxLayout();
    headerLayout->setMargin(0);
    headerLayout->setSpacing(0);
    headerLayout->addWidget(descriptionLabel);
    headerLayout->addWidget(kindLabel);
    headerLayout->addSpacerItem(new QSpacerItem(0,5));
    headerLayout->addWidget(toolbar);
    alignLayout->insertLayout(0, headerLayout);
}

