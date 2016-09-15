#include "nodeviewitemaction.h"
#include "../View/theme.h"

NodeViewItemAction::NodeViewItemAction(NodeViewItem *item):RootAction("TEST")
{
    nodeViewItem = item;
    //Register.
    nodeViewItem->registerObject(this);

    if(nodeViewItem->hasData("description")){
        this->setToolTip(nodeViewItem->getData("description").toString());
    }

    connect(nodeViewItem, SIGNAL(iconChanged()), this, SLOT(iconChanged()));
    connect(nodeViewItem, SIGNAL(labelChanged(QString)), this, SLOT(labelChanged(QString)));

    iconChanged();
    labelChanged(nodeViewItem->getData("label").toString());

    parentViewItemAction = 0;
}

void NodeViewItemAction::setParentNodeViewItemAction(NodeViewItemAction *item)
{
    parentViewItemAction = item;
}

NodeViewItemAction::~NodeViewItemAction()
{
    //Unregister
    nodeViewItem->unregisterObject(this);
}

NodeViewItemAction *NodeViewItemAction::getParentViewItemAction()
{
    return parentViewItemAction;
}

int NodeViewItemAction::getID()
{
    if(nodeViewItem){
        return nodeViewItem->getID();
    }
    return -1;
}

QString NodeViewItemAction::getKind()
{
    if(nodeViewItem){
        return nodeViewItem->getData("kind").toString();
    }
    return "";
}

QString NodeViewItemAction::getLabel()
{
    if (nodeViewItem) {
        return nodeViewItem->getData("label").toString();
    }
    return "";
}

NodeViewItem *NodeViewItemAction::getNodeViewItem()
{
    return nodeViewItem;
}

void NodeViewItemAction::iconChanged()
{
    //Update the icon.
    setIcon(Theme::theme()->getIcon(nodeViewItem->getIcon()));
}

void NodeViewItemAction::labelChanged(QString label)
{
    setText(label);
}
