#include "toolbarwidget.h"

#include <QHBoxLayout>
#include <QToolButton>

#include <QDebug>

ToolbarWidget::ToolbarWidget(NodeItem* item, QWidget *parent) :
    QWidget(parent)
{
    if (item) {
        nodeItem = item;
    } else {
        nodeItem = 0;
    }

    setMinimumSize(300,150);
    //move(item->pos().x() - width(), item->pos().y() - height());
    setWindowFlags(windowFlags() | Qt::Popup);
    setupToolBar();

    qDebug() << "ToolbarWidget";
}


/**
 * @brief ToolbarWidget::setupToolBar
 */
void ToolbarWidget::setupToolBar()
{
    QHBoxLayout* layout = new QHBoxLayout();
    QToolButton* addChildButton = new QToolButton();
    QToolButton* deleteButton = new QToolButton();
    QToolButton* connectButton = new QToolButton();
    QSize buttonSize = QSize(60,60);

    //addChildButton->setIcon(QIcon(":/Resources/Icons/addChildNode2.png"));
    //deleteButton->setIcon(QIcon(":/Resources/Icons/deleteNode.png"));
    //connectButton->setIcon(QIcon(":/Resources/Icons/connectNode.png"));

    qDebug() << "addChildButton icon size = " << addChildButton->size();
    qDebug() << "deleteButton icon size = " << deleteButton->size();
    qDebug() << "connectButton icon size = " << connectButton->size();

    addChildButton->setFixedSize(buttonSize);
    deleteButton->setFixedSize(buttonSize);
    connectButton->setFixedSize(buttonSize);

    //addChildButton->setIconSize(buttonSize*0.8);
    //deleteButton->setIconSize(buttonSize*0.8);
    //connectButton->setIconSize(buttonSize*0.8);

    addChildButton->setToolTip("Add Child Node");
    deleteButton->setToolTip("Delete Node");
    connectButton->setToolTip("Connect Node");

    layout->addWidget(addChildButton);
    layout->addWidget(deleteButton);
    layout->addWidget(connectButton);

    setLayout(layout);
}
