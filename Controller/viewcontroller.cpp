#include "viewcontroller.h"
#include "../View/nodeviewitem.h"
#include "../View/edgeviewitem.h"
#include "../View/Toolbar/toolbarwidgetnew.h"
#include <QDebug>
ViewController::ViewController(){
    modelItem = 0;
    _modelReady = false;
    selectionController = new SelectionController(this);
    actionController = new ActionController(this);
    actionController->connectViewController(this);



    toolbarController = new ToolActionController(this);
    toolbar = new ToolbarWidgetNew(this);
    connect(this, SIGNAL(modelReady(bool)), actionController, SLOT(modelReady(bool)));
    emit modelReady(false);

}

QStringList ViewController::getNodeKinds()
{
    QStringList nodeKinds;
    nodeKinds << "IDL" << "Component" << "Attribute" << "ComponentAssembly" << "ComponentInstance" << "BlackBox" << "BlackBoxInstance";
    nodeKinds << "Member" << "Aggregate";
    nodeKinds << "InEventPort"  << "OutEventPort";
    nodeKinds << "InEventPortDelegate"  << "OutEventPortDelegate";
    nodeKinds << "AggregateInstance";
    nodeKinds << "ComponentImpl";
    nodeKinds << "Vector" << "VectorInstance";
    nodeKinds << "HardwareCluster";
    nodeKinds << "WorkerDefinitions";

    nodeKinds << "IDL" << "Component" << "Attribute" << "ComponentAssembly" << "ComponentInstance" << "BlackBox" << "BlackBoxInstance";
    nodeKinds << "Member" << "Aggregate";
    nodeKinds << "InEventPort"  << "OutEventPort";
    nodeKinds << "InEventPortDelegate"  << "OutEventPortDelegate";
    nodeKinds << "AggregateInstance";
    nodeKinds << "ComponentImpl";

    nodeKinds << "Vector" << "VectorInstance";








    nodeKinds << "BranchState" << "Condition" << "PeriodicEvent" << "Process" << "Termination" << "Variable" << "Workload" << "OutEventPortImpl";
    nodeKinds << "WhileLoop" << "InputParameter" << "ReturnParameter" << "AggregateInstance" << "VectorInstance" << "WorkerProcess";


    //Append Kinds which can't be constructed by the GUI.
    nodeKinds << "MemberInstance" << "AttributeImpl";
    nodeKinds << "OutEventPortInstance" << "MemberInstance" << "AggregateInstance";
    nodeKinds << "AttributeInstance" << "AttributeImpl";
    nodeKinds << "InEventPortInstance" << "InEventPortImpl";
    nodeKinds << "OutEventPortInstance" << "OutEventPortImpl" << "HardwareNode";

    nodeKinds << "ComponentImpl" << "InterfaceDefinitions";
    nodeKinds << "OutEventPortImpl" << "InEventPortImpl";
    nodeKinds.removeDuplicates();

    return nodeKinds;
}

SelectionController *ViewController::getSelectionController()
{
    return selectionController;
}

ActionController *ViewController::getActionController()
{
    return actionController;
}

ToolActionController *ViewController::getToolbarController()
{
    return toolbarController;
}


void ViewController::setDefaultIcon(ViewItem *viewItem)
{
    if(viewItem->isNode()){

        QString nodeKind = viewItem->getData("kind").toString();
        QString imageName = nodeKind;
        if(nodeKind == "HardwareNode"){
            bool localhost = viewItem->hasData("localhost") && viewItem->getData("localhost").toBool();

            if(localhost){
                imageName = "Localhost";
            }else{
                QString os = viewItem->getData("os").toString();
                QString arch = viewItem->getData("architecture").toString();
                imageName = os + "_" + arch;
                imageName = imageName.remove(" ");
            }
        }else if(nodeKind == "Process"){


        }
        viewItem->setDefaultIcon("Items", imageName);
    }
}

ViewItem *ViewController::getModel()
{
    return modelItem;
}

bool ViewController::isModelReady()
{
    return _modelReady;
}

void ViewController::table_dataChanged(int ID, QString key, QVariant data)
{
    emit triggerAction("Table Changed");
    emit dataChanged(ID, key, data);
}

void ViewController::showToolbar(QPointF pos)
{
    toolbar->move(pos.toPoint());
    toolbar->show();
}

void ViewController::setModelReady(bool okay)
{
    if(okay != _modelReady){
        _modelReady = okay;
        emit modelReady(okay);
    }
}

void ViewController::entityConstructed(EntityAdapter *entity)
{
    ViewItem* viewItem = 0;
    bool isModel = false;
    if(entity->isNodeAdapter()){
        NodeAdapter* nodeAdapter = (NodeAdapter*)entity;
        int parentID = nodeAdapter->getParentNodeID();

        viewItem = new NodeViewItem(nodeAdapter);

        //Attach the node to it's parent
        if(viewItems.contains(parentID)){
            ViewItem* parent = viewItems[parentID];
            parent->addChild(viewItem);
        }

        if(entity->getDataValue("kind") == "Model"){
            isModel = true;
        }
    }else if(entity->isEdgeAdapter()){
        viewItem = new EdgeViewItem((EdgeAdapter*)entity);
    }

    if(viewItem){
        int ID = viewItem->getID();
        viewItems[ID] = viewItem;
        setDefaultIcon(viewItem);

        if(isModel){
            modelItem = viewItem;
        }

        connect(viewItem->getTableModel(), SIGNAL(req_dataChanged(int, QString, QVariant)), this, SLOT(table_dataChanged(int, QString, QVariant)));

        //Tell Views
        emit viewItemConstructed(viewItem);
    }

}

void ViewController::entityDestructed(EntityAdapter *entity)
{
    if(entity){
        int ID = entity->getID();
        if(viewItems.contains(ID)){
            ViewItem* viewItem = viewItems[ID];


            //Unset modelItem
            if(viewItem == modelItem){
                modelItem = 0;
            }

            ViewItem* parentItem = viewItem->getParentItem();
            if(parentItem){
                parentItem->removeChild(viewItem);
            }

            //Remove the item from the Hash
            viewItems.remove(ID);

            if(viewItem){
                emit viewItemDestructing(ID, viewItem);
                viewItem->destruct();
            }
        }
    }
}

