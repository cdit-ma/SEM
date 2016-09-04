#include "viewcontroller.h"
#include "../View/nodeviewitem.h"
#include "../View/edgeviewitem.h"
#include "../View/Toolbar/toolbarwidgetnew.h"
#include "../View/nodeviewnew.h"
#include "../Widgets/New/medeawindownew.h"
#include "../GUI/codebrowser.h"
#include "controller.h"
#include "filehandler.h"
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QApplication>
#include <QClipboard>
#include <QThreadPool>
#include <QListIterator>

#include "../Widgets/New/medeawindowmanager.h"
#include "../Widgets/New/medeanodeviewdockwidget.h"
#define GRAPHML_FILE_EXT "GraphML Documents (*.graphml)"
#define GRAPHML_FILE_SUFFIX ".graphml"
#define GME_FILE_EXT "GME Documents (*.xme)"
#define GME_FILE_SUFFIX ".xme"

#define XMI_FILE_EXT "UML XMI Documents (*.xml)"
#define XMI_FILE_SUFFIX ".xml"

ViewController::ViewController(){
    modelItem = 0;
    controller = 0;

    codeViewer = 0;
    validationDialog = 0;

    _modelReady = false;
    _controllerReady = true;

    rootItem = new ViewItem(this);

    qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
    selectionController = new SelectionController(this);
    qint64 time1 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    actionController = new ActionController(this);
    qint64 time2 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    toolbarController = new ToolActionController(this);
    qint64 time3 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    toolbar = new ToolbarWidgetNew(this);
    qint64 time4 = QDateTime::currentDateTime().toMSecsSinceEpoch();





    connect(this, &ViewController::vc_showToolbar, toolbar, &ToolbarWidgetNew::showToolbar);

    qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();
    qCritical() << "SelectionController in: " <<  time1 - timeStart << "MS";
    qCritical() << "ActionController in: " <<  time2 - time1 << "MS";
    qCritical() << "ToolActionController in: " <<  time3 - time2 << "MS";
    qCritical() << "ToolbarWidgetNew in: " <<  time4 - time3 << "MS";
    qCritical() << "ViewController in: " <<  timeFinish - timeStart << "MS";


}

ViewController::~ViewController()
{
    if(validationDialog){
        validationDialog->deleteLater();
    }

    delete rootItem;
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

QList<ViewItem *> ViewController::getConstructableNodeDefinitions(QString kind)
{
    Edge::EDGE_CLASS ec = Edge::EC_DEFINITION;

    if(kind.endsWith("Delegate") || kind.endsWith("EventPort")){
        ec = Edge::EC_AGGREGATE;
    }

    QList<ViewItem*> items;
    if(controller  && selectionController && selectionController->getSelectionCount() == 1){
        int parentID = selectionController->getFirstSelectedItem()->getID();
        QList<int> IDs = controller->getConstructableConnectableNodes(parentID, kind, ec);
        items = getViewItems(IDs);
    }
    return items;
}

QList<ViewItem*> ViewController::getValidEdges(Edge::EDGE_CLASS kind)
{
    qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
    QList<ViewItem*> items;
    int selection = 0;
    if(selectionController && controller){
        int i=0;
        QList<int> selectedIDs = selectionController->getSelectionIDs();
        QList<int> IDs = controller->getConnectableNodeIDs(selectedIDs, kind);
        items = getViewItems(IDs);
        selection = selectedIDs.count();
        i++;
    }

    qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();
    //qCritical() << "ViewController::getValidEdges(" << kind << ", " << selection << ") = " << items.count() <<"  In : "<<  (timeFinish - timeStart) / 100 << "MS";
    return items;
}

QStringList ViewController::_getSearchSuggestions()
{
    QStringList suggestions;

    foreach(ViewItem* item, viewItems.values()){
        //ID's
        suggestions.append(QString::number(item->getID()));
        //Data
        foreach(QString key, item->getKeys()){
            QString data = item->getData(key).toString();
            if(!suggestions.contains(data)){
                suggestions.append(data);
            }
        }
    }
    return suggestions;
}

QStringList ViewController::getAdoptableNodeKinds()
{
    if(selectionController && controller && selectionController->getSelectionCount() == 1){
        int ID = selectionController->getFirstSelectedItem()->getID();
        return controller->getAdoptableNodeKinds(ID);
    }
    return QStringList();
}

QList<Edge::EDGE_CLASS> ViewController::getValidEdgeKindsForSelection()
{
    QList<Edge::EDGE_CLASS> edgeKinds;
    if(selectionController && controller){
        edgeKinds = controller->getValidEdgeKindsForSelection(selectionController->getSelectionIDs());
    }
    return edgeKinds;
}

QStringList ViewController::getValidValuesForKey(int ID, QString keyName)
{
    QStringList values;
    if(controller){
        values = controller->getValidKeyValues(ID, keyName);
    }
    return values;
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
        emit vc_gotSearchSuggestions(matchedDataValues);
    }

    return matchedItems;
}

void ViewController::requestSearchSuggestions()
{
    emit vc_gotSearchSuggestions(_getSearchSuggestions());
}

void ViewController::setController(NewController *c)
{
    controller = c;
}

void ViewController::gotExportedSnippet(QString snippetData)
{
    if(selectionController){
        ViewItem* item = selectionController->getActiveSelectedItem();

        if(item && item->getParentItem()){
            QString itemKind = item->getParentItem()->getData("kind").toString();

            QStringList files = FileHandler::selectFiles("Export " + itemKind + ".snippet", QFileDialog::AnyFile,true,"GraphML " + itemKind + " Snippet (*." + itemKind+ ".snippet)", "." + itemKind + ".snippet");
            if(files.size() == 1){
                QString snippetFilePath = files.first();
                FileHandler::writeTextFile(snippetFilePath, snippetData);
            }
        }
    }
}

void ViewController::askQuestion(QString title, QString message, int ID)
{
    if(ID != -1){
        emit centerOnID(ID);
    }

    QMessageBox msgBox(QMessageBox::Question, title, message, QMessageBox::Yes | QMessageBox::No);

    msgBox.setIconPixmap(Theme::theme()->getImage("Actions", "Help").scaled(50,50));
    int reply = msgBox.exec();
    emit vc_answerQuestion(reply == QMessageBox::Yes);
}

void ViewController::modelValidated(QString reportPath)
{
    if(!validationDialog){
        MedeaWindowNew* window = MedeaWindowManager::manager()->getMainWindow();
        if(window){
            validationDialog = new ValidateDialog();
            connect(validationDialog, &ValidateDialog::revalidate_Model, this, &ViewController::validateModel);
            connect(validationDialog, &ValidateDialog::searchItem_centerOnItem, this, &ViewController::centerOnID);
        }
    }
    if(validationDialog){
        validationDialog->gotResults(reportPath);
        validationDialog->show();
    }
}

void ViewController::importGraphMLFile(QString graphmlPath)
{
    _importProjectFiles(QStringList(graphmlPath));
}

void ViewController::importGraphMLExtract(QString data)
{
    if(!data.isEmpty()){
        // fit the contents in all the view aspects after import when no model has been imported yet?
        emit vc_importProjects(QStringList(data));
    }
}

void ViewController::showCodeViewer(QString tabName, QString content)
{
    if(!codeViewer){
        codeViewer = MedeaWindowManager::constructViewDockWidget("Code Browser");
        codeViewer->setCloseVisible(false);
        CodeBrowser* codeBrowser = new CodeBrowser(codeViewer);
        codeViewer->setWidget(codeBrowser);
        codeViewer->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
        MedeaWindowNew* window = MedeaWindowManager::manager()->getActiveWindow();
        if(window){
            window->addDockWidget(codeViewer);
        }
    }
    if(codeViewer){
        CodeBrowser* codeBrowser = qobject_cast<CodeBrowser*>(codeViewer->widget());
        if(codeBrowser){
            codeBrowser->showCode(tabName, content, false);
        }
        codeViewer->show();
    }
}

void ViewController::jenkinsManager_IsBusy(bool busy)
{
    emit vc_JenkinsReady(!busy);
}

void ViewController::jenkinsManager_SettingsValidated(bool success, QString errorString)
{
    emit vc_JenkinsReady(success);
    qCritical() << success << errorString;
}

void ViewController::jenkinsManager_GotJenkinsNodesList(QString graphmlData)
{
   if(!graphmlData.isEmpty()){
        emit vc_triggerAction("Loading Jenkins Nodes");
        QStringList fileData;
        fileData << graphmlData;
        emit vc_importProjects(fileData);
   }else{
       //Hanlde error!
   }
}

void ViewController::getCodeForComponent()
{
    if(selectionController && controller){
        ViewItem* item = selectionController->getActiveSelectedItem();
        if(item && item->isNode()){
            NodeViewItem* node = (NodeViewItem*) item;

            if(node->getNodeKind() == Node::NK_COMPONENT_IMPL || node->getNodeKind() == Node::NK_COMPONENT || node->getNodeKind() == Node::NK_COMPONENT_INSTANCE){
                QString componentName = node->getData("label").toString();
                qCritical() << componentName;
                QString filePath = getTempFileForModel();
                if(!componentName.isEmpty() && !filePath.isEmpty()){
                    emit vc_getCodeForComponent(filePath, componentName);
                }
            }
        }
    }
}

void ViewController::validateModel()
{
    if(controller){
        QString filePath = getTempFileForModel();
        QString reportPath = FileHandler::getTempFileName("-ValidateReport.xml");

        if(!filePath.isEmpty()){
            emit vc_validateModel(filePath, reportPath);
        }
    }
}

void ViewController::launchLocalDeployment()
{
    if(controller){
        QString filePath = getTempFileForModel();
        if(!filePath.isEmpty()){
            emit vc_launchLocalDeployment(filePath);
        }
    }
}

void ViewController::actionFinished(bool success, QString gg)
{
    setControllerReady(true);
    emit vc_actionFinished();
}

void ViewController::table_dataChanged(int ID, QString key, QVariant data)
{
    emit vc_triggerAction("Table Changed");
    emit vc_setData(ID, key, data);
}

QString ViewController::getTempFileForModel()
{
    QString filePath;
    if(controller){
        QString data = controller->getProjectAsGraphML();
        if(!data.isEmpty()){
            filePath = FileHandler::writeTempTextFile(data, ".graphml");
        }
    }
    return filePath;
}

void ViewController::spawnSubView(ViewItem * item)
{
    MedeaWindowNew* window = MedeaWindowManager::manager()->getActiveWindow();

    if(window && item && item->isNode()){
        MedeaDockWidget *dockWidget = MedeaWindowManager::constructNodeViewDockWidget("SubView", Qt::TopDockWidgetArea);
        dockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
        dockWidget->setParent(window);
        dockWidget->setIcon(item->getIcon());
        dockWidget->setTitle(item->getData("label").toString());
        window->addDockWidget(Qt::TopDockWidgetArea, dockWidget);

        NodeViewNew* nodeView = new NodeViewNew(dockWidget);
        nodeView->setContainedNodeViewItem((NodeViewItem*)item);
        nodeView->setViewController(this);
        dockWidget->setWidget(nodeView);
        dockWidget->show();
        nodeView->show();
        nodeView->fitToScreen();
    }
}

bool ViewController::isControllerReady()
{
    return _controllerReady;
}

bool ViewController::canUndo()
{
    if(controller){
        return controller->canUndo();
    }
    return false;
}

bool ViewController::canRedo()
{
    if(controller){
        return controller->canRedo();
    }
    return false;
}

bool ViewController::canExportSnippet()
{
    if(controller){
        return controller->canExportSnippet(selectionController->getSelectionIDs());
    }
    return false;
}

bool ViewController::canImportSnippet()
{
    if(controller){
        return controller->canImportSnippet(selectionController->getSelectionIDs());
    }
    return false;
}

QVector<ViewItem *> ViewController::getOrderedSelection(QList<int> selection)
{
    QVector<ViewItem *> items;
    if(controller){
        foreach(int ID, controller->getOrderedSelectionIDs(selection)){
            items.append(getViewItem(ID));
        }
    }
    return items;
}

bool ViewController::destructViewItem(ViewItem *viewItem)
{
    if(viewItem){
        //Delete children first!
        destructChildItems(viewItem);

        QString treeKey;

        if(viewItem->isNode()){
            treeKey = ((NodeViewItem*)viewItem)->getTreeIndex();
        }

        if(!treeKey.isEmpty() && treeLookup.contains(treeKey)){
            treeLookup.remove(treeKey);
        }

        int ID = viewItem->getID();
        QString kind = viewItem->getData("kind").toString();
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
        topLevelItems.removeAll(ID);

        emit vc_viewItemDestructing(ID, viewItem);
        viewItem->destruct();
    }
    return false;
}

QList<ViewItem *> ViewController::getViewItems(QList<int> IDs)
{
    QList<ViewItem*> items;

    foreach(int ID, IDs){
        ViewItem* item = getViewItem(ID);
        if(item){
            items.append(item);
        }
    }
    return items;
}

QList<NodeViewNew *> ViewController::getNodeViewsContainingID(int ID)
{
    QList<NodeViewNew *> nodeViews;

    foreach(MedeaNodeViewDockWidget* dock, MedeaWindowManager::manager()->getNodeViewDockWidgets()){
        if(dock){
            NodeViewNew* view = dock->getNodeView();
            if(view && view->getIDsInView().contains(ID)){
                nodeViews.append(view);
            }
        }
    }
    return nodeViews;
}


NodeViewItem *ViewController::getNodeViewItem(int ID)
{
    ViewItem* item = getViewItem(ID);
    if(item && item->isNode()){
        return (NodeViewItem*) item;
    }
    return 0;
}

NodeViewItem *ViewController::getNodesImpl(int ID)
{
    if(controller){
        int implID = controller->getImplementation(ID);
        return getNodeViewItem(implID);
    }
    return 0;
}

NodeViewItem *ViewController::getNodesDefinition(int ID)
{
    if(controller){
        int defID = controller->getDefinition(ID);
        return getNodeViewItem(defID);
    }
    return 0;
}

NodeViewItem *ViewController::getSharedParent(NodeViewItem *node1, NodeViewItem *node2)
{
    QString tree1;
    QString tree2;

    if(node1){
        tree1 = node1->getTreeIndex();
    }
    if(node2){
        tree2 = node2->getTreeIndex();
    }

    int i = 0;

    while(true){
        if(tree1.length() < i || tree2.length() < i){
            //End of a string
            break;
        }
        if(tree1.at(i) != tree2.at(i)){
            //Different chars
            break;
        }
        i++;
    }

    QString parentTree = tree1.left(i);

    if(treeLookup.contains(parentTree)){
        int ID = treeLookup[parentTree];
        return getNodeViewItem(ID);
    }
    return 0;
}

NodeViewNew *ViewController::getActiveNodeView()
{
    MedeaViewDockWidget* dock = MedeaWindowManager::manager()->getActiveViewDockWidget();
    if(dock && dock->isNodeViewDock()){
        MedeaNodeViewDockWidget* viewDock = (MedeaNodeViewDockWidget*) dock;
        NodeViewNew* nodeView = viewDock->getNodeView();
        return nodeView;
    }
    return 0;
}

void ViewController::setModelReady(bool okay)
{
    if(okay != _modelReady){
        _modelReady = okay;
        emit mc_modelReady(okay);
    }
}

void ViewController::setControllerReady(bool ready)
{
    _controllerReady = ready;
    emit vc_controllerReady(ready);
}

void ViewController::deleteSelection()
{
    if(selectionController){
        emit vc_deleteEntities(selectionController->getSelectionIDs());
    }
}

void ViewController::renameActiveSelection()
{
    if(selectionController){
        ViewItem* item = selectionController->getActiveSelectedItem();
        if(item){
            emit vc_editTableCell(item->getID(), "label");
        }
    }
}

void ViewController::constructDDSQOSProfile()
{
    foreach(int ID, getIDsOfKind("AssemblyDefinitions")){
        emit vc_constructNode(ID, "DDS_QOSProfile");
    }
}

void ViewController::_teardownProject()
{
    if(isControllerReady()){
        if (controller) {
            setModelReady(false);
            setControllerReady(false);
            emit vc_projectPathChanged("");
            emit mc_projectModified(false);
            destructChildItems(rootItem);
            itemKindLists.clear();

            controller->disconnectViewController(this);
            controller = 0;
            setControllerReady(true);
        }
    }
}

bool ViewController::_newProject()
{
    if(_closeProject()){
        if(!controller){
            initializeController();
            emit vc_setupModel();
            return true;
        }
    }
    return false;
}

bool ViewController::_saveProject()
{
    if(controller){
        QString filePath = controller->getProjectPath();

        if(filePath.isEmpty()){
            return _saveAsProject();
        }else{
            QString data = controller->getProjectAsGraphML();
            if(FileHandler::writeTextFile(filePath, data)){
                emit vc_projectSaved(filePath);
                emit vc_addProjectToRecentProjects(filePath);
            }
            return true;
        }
    }
    return false;
}

bool ViewController::_saveAsProject()
{
    if(controller){
        QString fileName = FileHandler::selectFile("Select a *.graphml file to save project as.", QFileDialog::AnyFile, true, GRAPHML_FILE_EXT, GRAPHML_FILE_SUFFIX);
        if(!fileName.isEmpty()){
            controller->setProjectPath(fileName);
            return _saveProject();
        }
    }
    return false;
}

bool ViewController::_closeProject()
{
    if(isControllerReady()){
        if(controller){
            if(controller->isProjectSaved()){
                QString filePath = controller->getProjectPath();

                if(filePath == ""){
                    filePath = "Untitled Project";
                }

                //Ask User to confirm save?
                QMessageBox msgBox(QMessageBox::Question, "Save Changes",
                                   "Do you want to save the changes made to '" + filePath + "' ?",
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
        }
        _teardownProject();
        return true;
    }else{
        return false;
    }
}

bool ViewController::_openProject(QString filePath)
{
    if(_newProject()){
        if(filePath.isEmpty()){
            filePath = FileHandler::selectFile("Select Project to Open", QFileDialog::ExistingFile, false, GRAPHML_FILE_EXT, GRAPHML_FILE_SUFFIX);
        }
        if(!filePath.isEmpty()){
            QString fileData = FileHandler::readTextFile(filePath);
            emit vc_openProject(filePath, fileData);
            emit vc_addProjectToRecentProjects(filePath);
            return true;
        }
    }
    return false;
}

void ViewController::controller_entityConstructed(int ID, ENTITY_KIND eKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties)
{
    ViewItem* viewItem = 0;

    if(eKind == EK_NODE){
        NodeViewItem* nodeItem = new NodeViewItem(this, ID, eKind, kind, data, properties);
        viewItem = nodeItem;
        int parentID = nodeItem->getParentID();

        ViewItem* parentItem = getViewItem(parentID);

        QString treeKey = nodeItem->getTreeIndex();



        if(!treeLookup.contains(treeKey)){
            treeLookup[treeKey] = ID;
        }

        //Attach the node to it's parent
        if(parentItem){
            parentItem->addChild(nodeItem);
        }else{
            rootItem->addChild(nodeItem);
            topLevelItems.append(ID);
        }
    }else if(eKind == EK_EDGE){
        Edge::EDGE_CLASS edgeKind = Edge::EC_NONE;

        if(properties.contains("kind")){
            edgeKind = (Edge::EDGE_CLASS)properties["kind"].toInt();
        }

        if(!(edgeKind == Edge::EC_ASSEMBLY || edgeKind == Edge::EC_DATA || edgeKind == Edge::EC_WORKFLOW)){
            return;
        }

        int srcID = properties["srcID"].toInt();
        int dstID = properties["dstID"].toInt();
        NodeViewItem* source = getNodeViewItem(srcID);
        NodeViewItem* destination = getNodeViewItem(dstID);

        NodeViewItem* parent = getSharedParent(source, destination);

        EdgeViewItem* edgeItem = new EdgeViewItem(this, ID, source, destination, kind, data, properties);

        if(parent){
            parent->addChild(edgeItem);
        }else{
            qCritical() << "NO PARENT EDGE" << edgeItem;
            rootItem->addChild(edgeItem);
            topLevelItems.append(ID);
        }
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
        emit vc_viewItemConstructed(viewItem);
    }
}

void ViewController::controller_entityDestructed(int ID, ENTITY_KIND eKind, QString kind)
{
    ViewItem* viewItem = getViewItem(ID);
    destructViewItem(viewItem);

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
    _newProject();
}

void ViewController::openProject()
{
    _openProject();
}

void ViewController::openExistingProject(QString file)
{
    //Check for file first.
    if(FileHandler::isFileReadable(file)){
        _openProject(file);
    }else{
        qCritical() << file << "Not openable";
    }
}

void ViewController::importProjects()
{
    _importProjects();
}

void ViewController::importXMEProject()
{
    QStringList files = FileHandler::selectFiles("Select an XME File to import.", QFileDialog::ExistingFile, false, GME_FILE_EXT, GME_FILE_SUFFIX);
    if(files.length() == 1){
        QString xmePath = files.first();
        QFile file(xmePath);
        QFileInfo fileInfo = QFileInfo(file);
        QString tempFile = FileHandler::getTempFileName(fileInfo.baseName() + "_FromXME.graphml");
        emit mc_showProgress(true, "Transforming XME Project");
        emit mc_progressChanged(-1);
        emit vc_importXMEProject(xmePath, tempFile);
    }
}

void ViewController::importXMIProject()
{
    QStringList files = FileHandler::selectFiles("Select an XMI File to import.", QFileDialog::ExistingFile, false, XMI_FILE_EXT, XMI_FILE_SUFFIX);
    if(files.length() == 1){
        QString xmiPath = files.first();
        emit vc_importXMIProject(xmiPath);
    }
}

void ViewController::importSnippet()
{
    if(getSelectionController() && canImportSnippet()){
        ViewItem* item = selectionController->getActiveSelectedItem();
        if(item){
            QString itemKind = item->getData("kind").toString();

            QStringList files = FileHandler::selectFiles("Import " + itemKind + ".snippet", QFileDialog::ExistingFile, false,"GraphML " + itemKind + " Snippet (*." + itemKind+ ".snippet)", "." + itemKind + ".snippet");
            if(files.size() == 1){
                QString filePath = files.at(0);
                QString fileData = FileHandler::readTextFile(filePath);
                emit vc_importSnippet(selectionController->getSelectionIDs(), filePath, fileData);
            }
        }
    }
}

void ViewController::exportSnippet()
{
    if(canExportSnippet() && selectionController){
        emit vc_exportSnippet(selectionController->getSelectionIDs());
    }

}

void ViewController::closeProject()
{
    _closeProject();
}

void ViewController::closeMEDEA()
{
    if(_closeProject()){
        //Destruct main window
        MedeaWindowManager::teardown();
    }
}

void ViewController::executeJenkinsJob()
{
    if(controller){
        QString data = controller->getProjectAsGraphML();
        if(!data.isEmpty()){
            QString filePath = FileHandler::writeTempTextFile(data, ".graphml");
            emit vc_executeJenkinsJob(filePath);
        }
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
    _importProjectFiles(files);
}

void ViewController::_importProjectFiles(QStringList files)
{

    QStringList fileData;
    foreach (QString file, files) {
        QString data = FileHandler::readTextFile(file);
        if(data != ""){
            fileData.append(data);
        }
    }
    if(!fileData.isEmpty()){
        // fit the contents in all the view aspects after import when no model has been imported yet?
        emit vc_importProjects(fileData);
    }
}

void ViewController::fitView()
{
    NodeViewNew* view = getActiveNodeView();
    if(view){
        view->fitToScreen();
    }
}

void ViewController::fitAllViews()
{
    emit vc_fitToScreen();
}

void ViewController::centerSelection()
{
    NodeViewNew* view = getActiveNodeView();
    if(view){
        view->centerSelection();
    }
}

void ViewController::centerOnID(int ID)
{
    emit vc_centerItem(ID);
}

void ViewController::centerImpl()
{
    if(selectionController){
        ViewItem* item = selectionController->getActiveSelectedItem();
        if(item){
            NodeViewItem* impl = getNodesImpl(item->getID());
            if(impl){
                emit vc_centerItem(impl->getID());
            }
        }
    }
}

void ViewController::centerDefinition()
{
    if(selectionController){
        ViewItem* item = selectionController->getActiveSelectedItem();
        if(item){
            NodeViewItem* def = getNodesDefinition(item->getID());
            if(def){
                emit vc_centerItem(def->getID());
            }
        }
    }
}

void ViewController::popupImpl()
{
    if(selectionController){
        ViewItem* item = selectionController->getActiveSelectedItem();
        if(item){
            NodeViewItem* impl = getNodesImpl(item->getID());
            if(impl){
                spawnSubView(impl);
            }
        }
    }
}

void ViewController::popupDefinition()
{
    if(selectionController){
        ViewItem* item = selectionController->getActiveSelectedItem();
        if(item){
            NodeViewItem* def = getNodesDefinition(item->getID());
            if(def){
                spawnSubView(def);
            }
        }
    }
}

void ViewController::popupSelection()
{
    if(selectionController){
        ViewItem* item = selectionController->getActiveSelectedItem();
        spawnSubView(item);
    }
}


void ViewController::cut()
{
    if(selectionController){
        emit vc_cutEntities(selectionController->getSelectionIDs());
    }
}

void ViewController::copy()
{
    if(selectionController){
        emit vc_copyEntities(selectionController->getSelectionIDs());
    }
}

void ViewController::paste()
{
    if(selectionController && selectionController->getSelectionCount() == 1){
        emit vc_paste(selectionController->getSelectionIDs(), QApplication::clipboard()->text());
    }
}

void ViewController::replicate()
{
    if(selectionController && selectionController->getSelectionCount() > 0){
        emit vc_replicateEntities(selectionController->getSelectionIDs());
    }
}

void ViewController::initializeController()
{
    if(!controller){
        setControllerReady(false);
        setModelReady(false);
        controller = new NewController();
        controller->connectViewController(this);
    }
}

QList<int> ViewController::getIDsOfKind(QString kind)
{
    return itemKindLists[kind];
}

bool ViewController::destructChildItems(ViewItem *parent)
{
    QListIterator<ViewItem*> it(parent->getChildren());

    it.toBack();
    while(it.hasPrevious()){
        destructViewItem(it.previous());
    }
    return true;
}

bool ViewController::clearVisualItems()
{
    QListIterator<ViewItem*> it(rootItem->getChildren());

    it.toBack();
    while(it.hasPrevious()){
        destructViewItem(it.previous());
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

