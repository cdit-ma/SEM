#include "viewcontroller.h"
#include "../View/nodeviewitem.h"
#include "../View/edgeviewitem.h"
#include "../View/Toolbar/toolbarwidgetnew.h"
#include "controller.h"
#include <QDebug>
ViewController::ViewController(){
    modelItem = 0;
    _modelReady = false;
    selectionController = new SelectionController(this);
    actionController = new ActionController(this);


    controller = 0;

    toolbarController = new ToolActionController(this);
    toolbar = new ToolbarWidgetNew(this);
    connect(this, &ViewController::showToolbar, toolbar, &ToolbarWidgetNew::showToolbar);

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

QList<int> ViewController::getValidEdges(Edge::EDGE_CLASS kind)
{
    if(selectionController && controller){
        int ID = selectionController->getFirstSelectedItem()->getID();
        return controller->getConnectableNodes(ID);
    }
    return QList<int>();
}

QStringList ViewController::getAdoptableNodeKinds()
{
    if(selectionController && controller && selectionController->getSelectionCount() == 1){
        int ID = selectionController->getFirstSelectedItem()->getID();
        return controller->getAdoptableNodeKinds(ID);
    }
    return QStringList();
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

void ViewController::setController(NewController *c)
{
    controller = c;
}

void ViewController::table_dataChanged(int ID, QString key, QVariant data)
{
    emit triggerAction("Table Changed");
    emit dataChanged(ID, key, data);
}

void ViewController::setModelReady(bool okay)
{
    if(okay != _modelReady){
        _modelReady = okay;
        emit modelReady(okay);
    }
}

void ViewController::deleteSelection()
{
    if(selectionController){
        emit triggerAction("Deleting Selection");
        emit deleteEntities(selectionController->getSelectionIDs());
    }
}

void ViewController::constructDDSQOSProfile()
{
    emit triggerAction("Constructing DDS QOS Profile");
    foreach(int ID, getIDsOfKind("AssemblyDefinitions")){
        emit constructChildNode(ID, "DDS_QOSProfile");
    }
}

void ViewController::controller_entityConstructed(int ID, ENTITY_KIND eKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties)
{

    ViewItem* viewItem = 0;

    if(eKind == EK_NODE){
        NodeViewItem* nodeItem = new NodeViewItem(ID, eKind, kind, data, properties);
        viewItem = nodeItem;
        int parentID = nodeItem->getParentID();

        ViewItem* parentItem = getViewItem(parentID);

        //Attach the node to it's parent
        if(parentItem){
            parentItem->addChild(nodeItem);
        }
    }else if(eKind == EK_EDGE){
        //DO LATER.
        EdgeViewItem* edgeItem = new EdgeViewItem(ID, eKind, kind, data, properties);
        viewItem = edgeItem;
    }

    if(viewItem){
        viewItems[ID] = viewItem;
        itemKindLists[kind].append(ID);
        setDefaultIcon(viewItem);

        if(kind == "Model"){
            modelItem = viewItem;
        }

        connect(viewItem->getTableModel(), SIGNAL(req_dataChanged(int, QString, QVariant)), this, SLOT(table_dataChanged(int, QString, QVariant)));

        //Tell Views
        emit viewItemConstructed(viewItem);
    }
}

void ViewController::controller_entityDestructed(int ID, ENTITY_KIND eKind, QString kind)
{
    ViewItem* viewItem = getViewItem(ID);

    if(viewItem){
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
        itemKindLists[kind].removeAll(ID);

        if(viewItem){
            emit viewItemDestructing(ID, viewItem);
            viewItem->destruct();
        }
    }
}

void ViewController::controller_dataChanged(int ID, QString key, QVariant data)
{
    ViewItem* viewItem = getViewItem(ID);

    if(viewItem){
        viewItem->changeData(key, data);
    }
}

void ViewController::controller_dataRemoved(int ID, QString key)
{
    ViewItem* viewItem = getViewItem(ID);

    if(viewItem){
        viewItem->removeData(key);
    }
}

void ViewController::controller_propertyChanged(int ID, QString property, QVariant data)
{
    ViewItem* viewItem = getViewItem(ID);

    if(viewItem){
        viewItem->changeProperty(property, data);
    }
}

void ViewController::controller_propertyRemoved(int ID, QString property)
{
    ViewItem* viewItem = getViewItem(ID);

    if(viewItem){
        viewItem->removeProperty(property);
    }
}

void ViewController::newProject()
{
    if(!controller){
        initializeController();
        emit initializeModel();
    }
}

void ViewController::initializeController()
{
    if(!controller){
        controller = new NewController();

        //Set External Worker Definitions Path.
        //controller->setExternalWorkerDefinitionPath(applicationDirectory + "/Resources/WorkerDefinitions/");
        controller->connectViewController(this);

        QThread* controllerThread = new QThread();
        controllerThread->start();
        controller->moveToThread(controllerThread);
        connect(controller, SIGNAL(destroyed(QObject*)), controllerThread, SIGNAL(finished()));
        _modelReady = false;
    }
}

QList<int> ViewController::getIDsOfKind(QString kind)
{
    return itemKindLists[kind];
}

ViewItem *ViewController::getViewItem(int ID)
{
    if(viewItems.contains(ID)){
        return viewItems[ID];
    }
    return 0;
}

