#include "nodetableview.h"
#include "nodetable.h"
#include <QDebug>
NodeTableView::NodeTableView():QObject(), GraphMLView()
{
    qCritical() << "Constructed NodeTableViewManager";
    tableView = new NodeTable();

    qCritical() << "Built Table";
}

QAbstractTableModel *NodeTableView::getModel()
{

    return tableView;
}


void NodeTableView::connectController(NewController *controller)
{
    QObject::connect(controller, SIGNAL(view_ConstructGraphMLGUI(GraphML*)), this, SLOT(view_ConstructItem(GraphML*)));
    QObject::connect(controller, SIGNAL(view_DestructGraphMLGUIFromID(QString)), this, SLOT(view_DestructItem(QString)));
    QObject::connect(controller, SIGNAL(view_SetGraphMLSelected(GraphML*,bool)), this, SLOT(view_SelectItem(GraphML*, bool)));

    QObject::connect(this, SIGNAL(view_TriggerSelected(GraphML*,bool)), controller, SLOT(view_GraphMLSelected(GraphML*,bool)));

    QObject::connect(tableView, SIGNAL(triggerAction(QString)), controller, SLOT(view_TriggerAction(QString)));
    QObject::connect(tableView, SIGNAL(updateGraphMLData(GraphML*,QString,QString)), controller, SLOT(view_UpdateGraphMLData(GraphML*,QString,QString)));
}

void NodeTableView::disconnectController()
{

}

void NodeTableView::view_ConstructItem(GraphML *item)
{
    Node* node = dynamic_cast<Node*>(item);
    if(node){
        tableView->addItem(node);
    }
}

void NodeTableView::view_DestructItem(QString ID)
{
    tableView->removeItem(ID);

}

void NodeTableView::view_RefreshView()
{

}

void NodeTableView::view_SetViewDrawDepth(int depth)
{

}

void NodeTableView::view_PrintErrorText(GraphML *item, QString text)
{

}

void NodeTableView::view_FocusItem(GraphML *item)
{

}

void NodeTableView::view_SelectItem(GraphML *item, bool selected)
{



}

void NodeTableView::view_SetItemOpacity(GraphML *item, qreal opacity)
{

}

void NodeTableView::view_TreeViewItemSelected(QModelIndex index)
{
    Node* child = tableView->getNodeFromIndex(index);
    if(child){
        emit view_TriggerSelected(child, true);
    }
}

void NodeTableView::view_TreeViewItemCentered(QModelIndex index)
{
    Node* child = tableView->getNodeFromIndex(index);
    if(child){
        emit view_CenterItem(child);
    }

}

