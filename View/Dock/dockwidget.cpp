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

    mainLayout = new QVBoxLayout();
       mainLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
       mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
       mainLayout->setSpacing(5);

       alignLayout = new QVBoxLayout();
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

       connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
       themeChanged();
}


/**
 * @brief DockWidget::addItem
 * @param text
 * @return
 */
DockWidgetItem* DockWidget::addItem(QString text)
{
    DockWidgetItem* dockItem = new DockWidgetItem(text, this);
    mainLayout->addWidget(dockItem);
    return dockItem;
}


/**
 * @brief DockWidget::addActionItem
 * @param action
 * @return
 */
DockWidgetActionItem* DockWidget::addActionItem(QAction* action)
{
    DockWidgetActionItem* dockAction = 0;
    if (action && !childrenActions.contains(action)) {
        dockAction = new DockWidgetActionItem(action, this);
        dockAction->setToolButtonStyle(Qt::ToolButtonIconOnly);

        dockAction->setProperty("kind", action->text());
        if (toolActionController->kindsWithSubActions.contains(action->text())) {
            dockAction->setSubActionRequired(true);
        }
        childrenActions[action] = dockAction;
        mainLayout->addWidget(dockAction, 0, Qt::AlignTop);
        connect(dockAction, SIGNAL(clicked(bool)), this, SLOT(dockActionClicked()));
    }
    return dockAction;
}


/**
 * @brief DockWidget::addActionItems
 * @param actions
 */
void DockWidget::addActionItems(QList<QAction*> actions)
{
    foreach (QAction* action, actions) {
        addActionItem(action);
    }
}


/**
 * @brief DockWidget::clearDock
 */
void DockWidget::clearDock()
{
    foreach (DockWidgetActionItem* actionWidget, childrenActions.values()) {
        mainLayout->removeWidget(actionWidget);
        delete actionWidget;
    }
    childrenActions.clear();
}


/**
 * @brief DockWidget::updateHeaderText
 * @param text
 */
void DockWidget::updateHeaderText(QString text)
{
    if (descriptionLabel && !text.isEmpty()) {
        descriptionLabel->setText("Select to construct a <br/>" + text);
    }
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
                  "border: 0px;"
                  "background:" + theme->getAltBackgroundColorHex() + ";"
                  "}");

    if (backButton) {
        backButton->setIcon(theme->getIcon("Actions", "Arrow_Back"));
        backButton->setStyleSheet(theme->getToolBarStyleSheet() +
                                  "QToolButton {"
                                  "background:" + theme->getBackgroundColorHex() + ";"
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
 * @brief DockWidget::setupHeaderLayout
 */
void DockWidget::setupHeaderLayout()
{
    if (!containsHeader) {
        headerLayout = 0;
        descriptionLabel = 0;
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

    descriptionLabel = new QLabel("This is a description for dock type " + QString::number(dockType), this);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setAlignment(Qt::AlignCenter);
    descriptionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    descriptionLabel->setStyleSheet("margin: 5px 0px; padding: 0px;");
    descriptionLabel->setFont(QFont(font().family(), 8));

    headerLayout = new QVBoxLayout();
    headerLayout->setMargin(0);
    headerLayout->setSpacing(5);
    headerLayout->addWidget(descriptionLabel);
    headerLayout->addWidget(toolbar);
    alignLayout->insertLayout(0, headerLayout);
}

