#include "toolbarwidget.h"
#include "nodeview.h"

#include <QDebug>
#include <QHBoxLayout>


ToolbarWidget::ToolbarWidget(QWidget *parent) :
    QWidget(parent)
{
    nodeItem = 0;

    setMinimumSize(100,40);
    setBackgroundRole(QPalette::Dark);
    setWindowFlags(windowFlags() | Qt::Popup);

    setupToolBar();
}


/**
 * @brief ToolbarWidget::connectNodeItem
 * @param item
 */
void ToolbarWidget::connectToView()
{
    NodeView* view = dynamic_cast<NodeView*>(parentWidget());

    connect(addChildButton, SIGNAL(clicked()), view, SLOT(view_ConstructNodeAction()));
    connect(connectButton, SIGNAL(clicked()), view, SLOT(trigger_shiftPressed()));
    connect(deleteButton, SIGNAL(clicked()), view, SLOT(trigger_deletePressed()));

    connect(definitionButton, SIGNAL(clicked()), this, SLOT(goToDefinition()));
    connect(this, SIGNAL(goToDefinition(Node*)), view, SLOT(goToDefinition(Node*)));
    connect(implementationButton, SIGNAL(clicked()), this, SLOT(goToImplementation()));
    connect(this, SIGNAL(goToImplementation(Node*)), view, SLOT(goToImplementation(Node*)));

    connect(addChildButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(connectButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(definitionButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(implementationButton, SIGNAL(clicked()), this, SLOT(hide()));
}


/**
 * @brief ToolbarWidget::setNodeItem
 * @param item
 */
void ToolbarWidget::setNodeItem(NodeItem *item)
{
   if (item) {
       nodeItem = item;
   }
}


/**
 * @brief ToolbarWidget::goToDefinition
 */
void ToolbarWidget::goToDefinition()
{
    emit goToDefinition(nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::goToImplementation
 */
void ToolbarWidget::goToImplementation()
{
    emit goToImplementation(nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::setupToolBar
 */
void ToolbarWidget::setupToolBar()
{
    QHBoxLayout* layout = new QHBoxLayout();
    QSize buttonSize = QSize(30,30);

    addChildButton = new QToolButton();
    deleteButton = new QToolButton();
    connectButton = new QToolButton();
    definitionButton = new QToolButton();
    implementationButton = new QToolButton();

    addChildButton->setIcon(QIcon(":/Resources/Icons/addChildNode.png"));
    connectButton->setIcon(QIcon(":/Resources/Icons/connectNode.png"));
    deleteButton->setIcon(QIcon(":/Resources/Icons/deleteNode.png"));
    definitionButton->setIcon(QIcon(":/Resources/Icons/definition.png"));
    implementationButton->setIcon(QIcon(":/Resources/Icons/implementation.png"));

    addChildButton->setFixedSize(buttonSize);
    connectButton->setFixedSize(buttonSize);
    deleteButton->setFixedSize(buttonSize);
    definitionButton->setFixedSize(buttonSize);
    implementationButton->setFixedSize(buttonSize);

    addChildButton->setIconSize(buttonSize*0.75);
    connectButton->setIconSize(buttonSize*0.75);
    deleteButton->setIconSize(buttonSize*0.85);
    definitionButton->setIconSize(buttonSize*0.85);
    implementationButton->setIconSize(buttonSize*0.85);

    addChildButton->setToolTip("Add Child Node");
    connectButton->setToolTip("Connect Node");
    deleteButton->setToolTip("Delete Node");
    definitionButton->setToolTip("Go to Definition");
    implementationButton->setToolTip("Go to Implementation");

    layout->addWidget(addChildButton);
    layout->addWidget(connectButton);
    layout->addWidget(deleteButton);
    layout->addSpacerItem(new QSpacerItem(10,0));
    layout->addWidget(definitionButton);
    layout->addWidget(implementationButton);

    layout->setMargin(5);
    setLayout(layout);
}
