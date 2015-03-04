#include "docknodeitem.h"
#include "dockscrollarea.h"

#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
/**
 * @brief DockNodeItem::DockNodeItem
 * @param parent
 */
DockNodeItem::DockNodeItem(NodeItem *node_item, QWidget* parent) :
    QPushButton(parent)
{
    nodeItem = node_item;
    selectedNode = 0;

    kind = nodeItem->getGraphML()->getDataValue("kind");
    label = nodeItem->getGraphML()->getDataValue("label");

    textLabel = new QLabel(label, this);
    imageLabel = new QLabel(this);

    setupLayout();
    makeConnections();
}


/**
 * @brief DockNodeItem::setupLayout
 */
void DockNodeItem::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout();

    setFlat(true);
    setFixedSize(100, 100);
    setStyleSheet("margin: 0px; padding: 0px;");

    // make the font size smaller to fit the whole text inside textLabel
    QFont font = textLabel->font();
    if (label.length() > 18) {
        QFontMetrics fm(font);
        font.setPointSizeF(fm.boundingRect(label).width()/label.size()*1.3);
    } else {
        font.setPointSizeF(7.5);
    }

    textLabel->setFont(font);
    textLabel->setFixedSize(width(), 21);
    textLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    QImage* image = new QImage(":/Resources/Icons/" + kind + ".png");
    QImage scaledImage = image->scaled(width(),
                                       height()-textLabel->height(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);

    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setPixmap(QPixmap::fromImage(scaledImage));

    layout->addWidget(imageLabel);
    layout->addWidget(textLabel);

    layout->setMargin(0);
    layout->setSpacing(2);
    layout->setAlignment(imageLabel, Qt::AlignHCenter | Qt::AlignBottom);
    layout->setAlignment(textLabel, Qt::AlignHCenter);

    setLayout(layout);
}


/**
 * @brief DockNodeItem::setContainer
 * @param container
 */
void DockNodeItem::setContainer(DockScrollArea *container)
{
    parentContainer = container;
}


/**
 * @brief DockNodeItem::getNodeItem
 * Returns the NodeItem this item is conneted to.
 * @return nodeItem
 */
NodeItem *DockNodeItem::getNodeItem()
{
    return nodeItem;
}


/**
 * @brief DockNodeItem::connectToNodeItem
 */
void DockNodeItem::makeConnections()
{
    connect(this, SIGNAL(clicked()), this , SLOT(buttonPressed()));

    connect(nodeItem, SIGNAL(updateDockNodeItem()), this, SLOT(updateData()));
    connect(nodeItem, SIGNAL(updateOpacity(qreal)), this, SLOT(setOpacity(qreal)));
    connect(nodeItem, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}


/**
 * @brief DockNodeItem::paintEvent
 * Change the node image (button icon) depending on the node kind.
 * NOTE: setIcon() calls this and the scroll area's paintEvent() infinitely.
 * @param e
 */
void DockNodeItem::paintEvent(QPaintEvent *e)
{
    setStyleSheet("QPushButton:hover{"
                  "background-color: rgba(0,0,0,0);"
                  "border: 1px solid black;"
                  "border-radius: 5px;"
                  "}");

    QPushButton::paintEvent(e);
}


/**
 * @brief DockNodeItem::buttonPressed
 * If the attached nodeItem is a Component, either try to add a ComponentInstance inside
 * the ComponentAssembly or connect the ComponentInstance to this dock item's Component.
 * If it's a HardwareNode, try to connect it to the currently selected node.
 */
void DockNodeItem::buttonPressed()
{
    emit getSelectedNode();

    if (selectedNode) {
        if (nodeItem->getNodeKind() == "Component") {
            QString nodeKind = selectedNode->getDataValue("kind");
            if (nodeKind == "ComponentAssembly") {
                emit dockNode_addComponentInstance(selectedNode, nodeItem->getNode());
            } else if (nodeKind == "ComponentInstance") {
                emit dockNode_connectComponentInstance(selectedNode, nodeItem->getNode());
            }
        } else if (nodeItem->getNodeKind() == "HardwareNode") {
            emit dockNode_connectHardwareNode(selectedNode, nodeItem->getNode());
        }
    }
}


/**
 * @brief DockNodeItem::updateData
 * This gets called when the dataTable value for the node item has been changed.
 */
void DockNodeItem::updateData()
{
    label = nodeItem->getGraphML()->getDataValue("label");
    textLabel->setText(label);
    repaint();
}


/**
 * @brief DockNodeItem::deleteLater
 */
void DockNodeItem::deleteLater()
{
    emit removeFromDockNodeList(this);
    /*

    if (parentContainer) {
        parentContainer->checkDockNodesList();
    }
    */

    QObject::deleteLater();
}


/**
 * @brief DockNodeItem::setOpacity
 * This disables this button when SHIFT is held down and the currently
 * selected node can't be connected to this button.
 * @param opacity
 */
void DockNodeItem::setOpacity(double opacity)
{
    if (opacity < 1) {
        setEnabled(false);
    } else {
        setEnabled(true);
    }
    repaint();
}


/**
 * @brief DockNodeItem::setSelectedNode
 * @param node
 */
void DockNodeItem::setSelectedNode(Node *node)
{
    selectedNode = node;
}
