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
    connect(connectButton, SIGNAL(clicked()), view, SLOT(trigger_pressShift()));
    connect(deleteButton, SIGNAL(clicked()), view, SLOT(trigger_deletePressed()));
    connect(addChildButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(connectButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(hide()));
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
 * @brief ToolbarWidget::setupToolBar
 */
void ToolbarWidget::setupToolBar()
{
    QHBoxLayout* layout = new QHBoxLayout();
    QSize buttonSize = QSize(30,30);

    addChildButton = new QToolButton();
    deleteButton = new QToolButton();
    connectButton = new QToolButton();

    addChildButton->setIcon(QIcon(":/Resources/Icons/addChildNode.png"));
    connectButton->setIcon(QIcon(":/Resources/Icons/connectNode.png"));
    deleteButton->setIcon(QIcon(":/Resources/Icons/deleteNode.png"));

    addChildButton->setFixedSize(buttonSize);
    connectButton->setFixedSize(buttonSize);
    deleteButton->setFixedSize(buttonSize);

    addChildButton->setIconSize(buttonSize*0.75);
    connectButton->setIconSize(buttonSize*0.75);
    deleteButton->setIconSize(buttonSize*0.85);

    addChildButton->setToolTip("Add Child Node");
    connectButton->setToolTip("Connect Node");
    deleteButton->setToolTip("Delete Node");

    layout->addWidget(addChildButton);
    layout->addWidget(connectButton);
    layout->addWidget(deleteButton);

    setLayout(layout);
}
