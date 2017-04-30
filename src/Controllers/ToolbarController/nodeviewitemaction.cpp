#include "nodeviewitemaction.h"
#include "../../theme.h"
#include "../../Model/nodekinds.h"
NodeViewItemAction::NodeViewItemAction(NodeViewItem *item):RootAction("NodeItem", "")
{
    nodeViewItem = item;
    //Register.
    nodeViewItem->registerObject(this);

    if(nodeViewItem->hasData("description")){
        this->setToolTip(nodeViewItem->getData("description").toString());
    }

    connect(nodeViewItem, SIGNAL(iconChanged()), this, SLOT(_iconChanged()));
    connect(nodeViewItem, SIGNAL(labelChanged(QString)), this, SLOT(_labelChanged(QString)));

    _iconChanged();
    _labelChanged(nodeViewItem->getData("label").toString());

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

NODE_KIND NodeViewItemAction::getNodeKind()
{
    if(nodeViewItem){
        return nodeViewItem->getNodeKind();
    }
    return NODE_KIND::NONE;
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

void NodeViewItemAction::_iconChanged()
{
    auto pair = nodeViewItem->getIcon();
    setIconPath(pair.first, pair.second);
}

void NodeViewItemAction::_labelChanged(QString label)
{
    setText(label);
}
