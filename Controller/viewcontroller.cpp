#include "viewcontroller.h"
#include "../View/nodeviewitem.h"
#include "../View/edgeviewitem.h"
#include "../View/Toolbar/toolbarwidgetnew.h"
#include "controller.h"
#include "filehandler.h"
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QApplication>
#include <QClipboard>
#include <QThreadPool>

#include "../Widgets/New/medeawindowmanager.h"
#define GRAPHML_FILE_EXT "GraphML Documents (*.graphml)"
#define GRAPHML_FILE_SUFFIX ".graphml"
#define GME_FILE_EXT "GME Documents (*.xme)"
#define GME_FILE_SUFFIX ".xme"

#define XMI_FILE_EXT "UML XMI Documents (*.xml)"
#define XMI_FILE_SUFFIX ".xml"

ViewController::ViewController(){
    modelItem = 0;
    controller = 0;
    _modelReady = false;
    _controllerReady = true;

    qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
    selectionController = new SelectionController(this);
    qint64 time1 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    actionController = new ActionController(this);
    qint64 time2 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    toolbarController = new ToolActionController(this);
    qint64 time3 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    toolbar = new ToolbarWidgetNew(this);
    qint64 time4 = QDateTime::currentDateTime().toMSecsSinceEpoch();





    connect(this, &ViewController::showToolbar, toolbar, &ToolbarWidgetNew::showToolbar);

    connect(this, SIGNAL(modelReady(bool)), actionController, SLOT(modelReady(bool)));
    emit modelReady(false);
    qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();
    qCritical() << "SelectionController in: " <<  time1 - timeStart << "MS";
    qCritical() << "ActionController in: " <<  time2 - time1 << "MS";
    qCritical() << "ToolActionController in: " <<  time3 - time2 << "MS";
    qCritical() << "ToolbarWidgetNew in: " <<  time4 - time3 << "MS";
    qCritical() << "ViewController in: " <<  timeFinish - timeStart << "MS";
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

QList<ViewItem*> ViewController::search(QString searchString)
{
    QList<ViewItem*> matchedItems = viewItems.values();

    if (showSearchSuggestions) {
        QStringList matchedDataValues;
        foreach (ViewItem* item, matchedItems) {
            matchedDataValues.append(item->getData("label").toString());
        }
        emit seachSuggestions(matchedDataValues);
    }

    return matchedItems;
}

void ViewController::searchSuggestionsRequested(QString searchString)
{
    showSearchSuggestions = true;
    search(searchString);
    showSearchSuggestions = false;
}

void ViewController::setController(NewController *c)
{
    controller = c;
}

void ViewController::table_dataChanged(int ID, QString key, QVariant data)
{
    emit triggerAction("Table Changed");
    emit setData(ID, key, data);
}

void ViewController::setModelReady(bool okay)
{
    if(okay != _modelReady){
        _modelReady = okay;
        emit modelReady(okay);
    }

}

void ViewController::setControllerReady(bool ready)
{
    if(ready != _controllerReady){
        _controllerReady = ready;
        emit controllerReady(ready);
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
        emit constructNode(ID, "DDS_QOSProfile");
    }
}

void ViewController::_teardownProject()
{
    if (controller) {
        setModelReady(false);
        setControllerReady(false);

        clearVisualItems();
        controller->disconnectViewController(this);
        controller = 0;

    }
}

bool ViewController::_newProject()
{

    if(!controller){
        initializeController();
        emit initializeModel();
        return true;
    }
    return false;
}

bool ViewController::_saveProject()
{
    if(controller){
        QString filePath = controller->getProjectFileName();

        if(filePath.isEmpty()){
            return _saveAsProject();
        }else{
            QString data = controller->getProjectAsGraphML();
            FileHandler::writeTextFile(filePath, data);
            return true;
        }
    }
    return false;
}

bool ViewController::_saveAsProject()
{
    if(controller){
        QString fileName = FileHandler::selectFile("Select a *.graphml file to save project as.", QFileDialog::AnyFile, true, GRAPHML_FILE_EXT, GRAPHML_FILE_SUFFIX);
        controller->setProjectFilePath(fileName);
        return _saveProject();
    }
    return false;
}

bool ViewController::_closeProject()
{
    if(controller){
        if(!controller->projectRequiresSaving()){
            //Ask User to confirm save?
            QMessageBox msgBox(QMessageBox::Question, "Save Changes",
                               "Do you want to save the changes made to '" /* + currentProjectFilePath +*/ "' ?",
                               QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);


            msgBox.setIconPixmap(Theme::theme()->getImage("Actions", "Save"));
            msgBox.setButtonText(QMessageBox::Yes, "Save");
            msgBox.setButtonText(QMessageBox::No, "Ignore Changes");

            int buttonPressed = msgBox.exec();

            if(buttonPressed == QMessageBox::Yes){
                bool saveSuccess = _saveProject();
                // if failed to save, do nothing
                if(!saveSuccess){
                    return false;
                }
            }else if(buttonPressed == QMessageBox::No){
                //Do Nothing
            }else{
                return false;
            }
        }
        _teardownProject();
    }
    return true;
}

bool ViewController::_openProject()
{
    QString filePath;
    if(controller){
        filePath = controller->getProjectFileName();
    }

    filePath = FileHandler::selectFile("Select Project to Open", QFileDialog::ExistingFile, false, GRAPHML_FILE_EXT, GRAPHML_FILE_SUFFIX, filePath);
    if(!filePath.isEmpty()){
        QString fileData = FileHandler::readTextFile(filePath);
        if(_closeProject() && _newProject()){
            emit vc_openProject(filePath, fileData);
        }else{
            return false;
        }
    }
    return true;
}

void ViewController::controller_entityConstructed(int ID, ENTITY_KIND eKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties)
{

    //qCritical() << "CREATED: " << ID;
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

void ViewController::setClipboardData(QString data)
{
    QApplication::clipboard()->setText(data);
}

void ViewController::newProject()
{
    if(_closeProject()){
        _newProject();

    }
}

void ViewController::openProject()
{
    _openProject();
}

void ViewController::closeProject()
{
    _closeProject();
    setControllerReady(true);
}

void ViewController::closeMEDEA()
{
    if(_closeProject()){
        //Destruct main window
        MedeaWindowManager::teardown();
    }
}

void ViewController::saveProject()
{
    _saveProject();
}

void ViewController::saveAsProject()
{
    _saveAsProject();
}

void ViewController::_importProjects()
{
    QStringList files = FileHandler::selectFiles("Select graphML File(s) to import.", QFileDialog::ExistingFiles, false, GRAPHML_FILE_EXT, GRAPHML_FILE_SUFFIX);

    QStringList fileData;
    foreach (QString file, files) {
        QString data = FileHandler::readTextFile(file);
        if(data != ""){
            fileData.append(data);
        }
    }
    emit importProjects(fileData);
}

void ViewController::threadDead()
{
    qCritical() << "GG";
    qCritical() << "ACTIVE THREADS: " << QThreadPool::globalInstance()->activeThreadCount();
}

void ViewController::cut()
{
    if(selectionController){
        emit triggerAction("Cutting Selection");
        emit cutEntities(selectionController->getSelectionIDs());
    }
}

void ViewController::copy()
{
    if(selectionController){
        emit copyEntities(selectionController->getSelectionIDs());
    }
}

void ViewController::paste()
{
    if(selectionController && selectionController->getSelectionCount() == 1){
        emit pasteIntoEntity(selectionController->getSelectionIDs()[0], QApplication::clipboard()->text());
    }
}

void ViewController::initializeController()
{
    if(!controller){
        setControllerReady(false);

        controller = new NewController();
        controller->connectViewController(this);

        //setControllerReady(true);
        _modelReady = false;
    }
}

QList<int> ViewController::getIDsOfKind(QString kind)
{
    return itemKindLists[kind];
}

bool ViewController::clearVisualItems()
{
    while(!viewItems.isEmpty()){
        int ID = viewItems.keys().first();
        controller_entityDestructed(ID, EK_NODE, "");
    }

    itemKindLists.clear();
    return true;
}

ViewItem *ViewController::getViewItem(int ID)
{
    if(viewItems.contains(ID)){
        return viewItems[ID];
    }
    return 0;
}

