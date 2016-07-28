#include "nodeviewitemaction.h"
#include "../View/theme.h"

NodeViewItemAction::NodeViewItemAction(NodeViewItem *item):QAction(0)
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
}

NodeViewItemAction::~NodeViewItemAction()
{
    //Unregister
    disconnect(nodeViewItem, SIGNAL(iconChanged()), this, SLOT(iconChanged()));
    disconnect(nodeViewItem, SIGNAL(labelChanged(QString)), this, SLOT(labelChanged(QString)));
    nodeViewItem->unregisterObject(this);
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
