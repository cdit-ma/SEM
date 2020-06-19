#include "modelcontroller.h"

#include <iterator>
#include <algorithm>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QStringBuilder>
#include <QThread>
#include <QXmlStreamReader>
#include <QCoreApplication>

#include "version.h"

#include "entityfactory.h"
#include "entityfactorybroker.h"
#include "entityfactoryregistrybroker.h"
#include "tempentity.h"

#include "Entities/edge.h"
#include "Entities/data.h"
#include "Entities/node.h"
#include "Entities/key.h"
#include "Entities/Keys/exportidkey.h"

#include "Entities/InterfaceDefinitions/eventport.h"
#include "Entities/InterfaceDefinitions/aggregate.h"
#include "Entities/InterfaceDefinitions/datanode.h"
#include "Entities/Keys/versionkey.h"
#include "graphmlprinter.h"

inline QPair<bool, QString> readFile(const QString& filePath)
{
    QPair<bool, QString> result;

    QFile file(filePath);
    result.first = file.exists() && file.open(QFile::ReadOnly | QFile::Text);
    if (result.first) {
        QTextStream fileStream(&file);
        result.second = fileStream.readAll();
        file.close();
    }
    return result;
}

inline QString getXMLAttribute(const QXmlStreamReader &xml, const QString& attribute)
{
    //Get the Attributes of the current XML entity.
    const auto &attributes = xml.attributes();

    if(attributes.hasAttribute(attribute)){
        return attributes.value(attribute).toString();
    }else{
        qCritical() << "Cannot find Attribute Key: " << attribute;
        return "";
    }
}

ModelController::ModelController(const QString& application_dir):
QObject(0),
lock_(QReadWriteLock::Recursive)
{
    if(!VersionKey::IsVersionValid(Version::GetMedeaVersion())){
        throw std::runtime_error("MEDEA Version is invalid: '" + Version::GetMedeaVersion().toStdString() + "'");
    }
    this->application_dir = application_dir;
    controller_thread = new QThread();
    moveToThread(controller_thread);

    connect(this, &ModelController::InitiateTeardown, controller_thread, &QThread::quit);
    connect(controller_thread, &QThread::finished, this, &ModelController::deleteLater);
    connect(controller_thread, &QThread::finished, controller_thread, &QThread::deleteLater);
    controller_thread->start();

    entity_factory = EntityFactory::getNewFactory();

    //Register meta types
    qRegisterMetaType<GRAPHML_KIND>("GRAPHML_KIND");
    qRegisterMetaType<EDGE_KIND>("EDGE_KIND");
    qRegisterMetaType<NODE_KIND>("NODE_KIND");
    qRegisterMetaType<QList<int> >("QList<int>");
    qRegisterMetaType<MODEL_SEVERITY>("MODEL_SEVERITY");
    qRegisterMetaType<QSet<EDGE_DIRECTION> >("QSet<EDGE_DIRECTION>");
    qRegisterMetaType<DataUpdate>("DataUpdate");
}


ModelController::~ModelController()
{
    setModelAction(MODEL_ACTION::DESTRUCTING);
    destructEntities({workerDefinitions, model});
    delete entity_factory;
}

bool ListInList(const QList<NODE_KIND>& haystack, const QList<NODE_KIND>& needle){
    //Checks to see if a list(haystack) fully contains another list(needle)
    return std::find_end(haystack.constBegin(), haystack.constEnd(), needle.constBegin(), needle.constEnd()) != haystack.end();
}

bool ModelController::SuppressImportNodeError(const TempEntity& parent_entity, const TempEntity& child_entity, const QString& version){
    if(!VersionKey::IsVersionValid(version)){
        return false;
    }

    //Handling the breaking changes prior to v3.3.4
    //This check is to remove the errors about Function, CallbackFunction and ClassInstance children being no-longer valid children of ClassInstance's in the Assembly Aspect
    if(VersionKey::IsVersionOlder(version, "3.3.5")){
        const static QSet<NODE_KIND> now_invalid_child_kinds{NODE_KIND::FUNCTION, NODE_KIND::CALLBACK_FNC, NODE_KIND::CLASS_INST};
        
        auto child_kind = entity_factory->getNodeKind(child_entity.getKind());

        if(now_invalid_child_kinds.contains(child_kind)){
            const static QList<NODE_KIND> assembly_stack{NODE_KIND::MODEL, NODE_KIND::DEPLOYMENT_DEFINITIONS, NODE_KIND::ASSEMBLY_DEFINITIONS};
            const static QList<NODE_KIND> component_stack{NODE_KIND::COMPONENT_INST, NODE_KIND::CLASS_INST};
            
            const auto& stack = child_entity.getParentStack();
            const auto& in_assembly = ListInList(stack, assembly_stack);
            const auto& in_class_instance = ListInList(stack, component_stack);
            
            if(in_assembly && in_class_instance){
                return true;
            }
        }
    }

    return false;
}


bool ModelController::SuppressImportEdgeError(const TempEntity& edge_entity, const TempEntity* src_entity, const TempEntity* dst_entity, const QString& version){
    if(!VersionKey::IsVersionValid(version)){
        return false;
    }

    //Handling the breaking changes prior to v3.3.4
    //This check is to remove the errors about Function, CallbackFunction and ClassInstance children being no-longer valid children of ClassInstance's in the Assembly Aspect
    if(VersionKey::IsVersionOlder(version, "3.3.5")){
        const static QSet<NODE_KIND> now_invalid_child_kinds{NODE_KIND::FUNCTION, NODE_KIND::CALLBACK_FNC, NODE_KIND::CLASS_INST};
        auto child_kind = entity_factory->getNodeKind(src_entity->getKind());

        if(now_invalid_child_kinds.contains(child_kind)){
            //Check the Source stack for being in the right spot
            const static QList<NODE_KIND> assembly_stack{NODE_KIND::MODEL, NODE_KIND::DEPLOYMENT_DEFINITIONS, NODE_KIND::ASSEMBLY_DEFINITIONS};
            const static QList<NODE_KIND> component_stack{NODE_KIND::COMPONENT_INST, NODE_KIND::CLASS_INST};
            
            const auto& stack = src_entity->getParentStack();
            const auto& in_assembly = ListInList(stack, assembly_stack);
            const auto& in_class_instance = ListInList(stack, component_stack);
            if(in_assembly && in_class_instance){
                return true;
            }
        }
    }


    return false;
}

void ModelController::ConnectViewController(ViewControllerInterface* view_controller){
    if(view_controller){
        connect(view_controller, &ViewControllerInterface::SetupModelController, this, &ModelController::SetupController, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::ImportProjects, this, &ModelController::importProjects, Qt::QueuedConnection);

        connect(view_controller, &ViewControllerInterface::ReloadWorkerDefinitions, this, &ModelController::loadWorkerDefinitions, Qt::QueuedConnection);

        connect(view_controller, &ViewControllerInterface::TriggerAction, this, &ModelController::triggerAction, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::SetData, this, &ModelController::setData, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::RemoveData, this, &ModelController::removeData, Qt::QueuedConnection);

        connect(view_controller, &ViewControllerInterface::constructTriggerDefinition, this, &ModelController::constructTriggerDefinition, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::constructDDSQOSProfile, this, &ModelController::constructDDSQOSProfile, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::ConstructNodeAtIndex, this, &ModelController::constructNodeAtIndex, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::ConstructNodeAtPos, this, &ModelController::constructNodeAtPos, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::ConstructConnectedNodeAtIndex, this, &ModelController::constructConnectedNodeAtIndex, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::ConstructConnectedNodeAtPos, this, &ModelController::constructConnectedNodeAtPos, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::ConstructEdges, this, &ModelController::constructEdges, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::DestructEdges, this, &ModelController::destructEdges, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::DestructAllEdges, this, &ModelController::destructAllEdges, Qt::QueuedConnection);
        

        connect(view_controller, &ViewControllerInterface::Undo, this, &ModelController::undo, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::Redo, this, &ModelController::redo, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::Delete, this, &ModelController::remove, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::Paste, this, &ModelController::paste, Qt::QueuedConnection);
        connect(view_controller, &ViewControllerInterface::Replicate, this, &ModelController::replicate, Qt::QueuedConnection);
    }
}

bool ModelController::SetupController(const QString& file_path)
{
    setModelAction(MODEL_ACTION::SETUP);
    setupModel();
    loadWorkerDefinitions();
    clearHistory();
    unsetModelAction(MODEL_ACTION::SETUP);

    auto file = readFile(file_path);
    bool success = true;
    if(file.first){
        setModelAction(MODEL_ACTION::OPEN);
        success = importGraphML(file.second, model);
        unsetModelAction(MODEL_ACTION::OPEN);
        
        if(success){
            //Update the project filePath
            setProjectPath(file_path);
        }
    }else{
        emit ProjectFileChanged("Untitled Project");
    }

    //Clear the Undo/Redo History.
    clearHistory();
    setProjectModified(!success);
    emit ModelReady(true);
    return success;
}


/**
 * @brief NewController::loadWorkerDefinitions Loads in both the compiled in WorkerDefinitions and Externally defined worker defintions.
 */
void ModelController::loadWorkerDefinitions()
{
    if(workerDefinitions){
        QList<QDir> worker_directories{QDir(":/WorkerDefinitions"), QDir(application_dir + "/Resources/WorkerDefinitions/")};
        QStringList extensions{"*.worker.graphml"};
        
        setModelAction(MODEL_ACTION::IMPORT);
        for(auto dir : worker_directories){
            for(auto file : dir.entryList(extensions)){
                auto file_path = dir.absolutePath() + "/" + file;
                auto data = readFile(file_path);
                bool success = importGraphML(data.second, workerDefinitions);
                if(!success){
                    QString title = "Error Importing Worker Definition";
                    QString description = "loadWorkerDefinitions(): Importing '" % file_path % "' failed";
                    emit Notification(MODEL_SEVERITY::WARNING, title, description);
                }
            }
        }
        unsetModelAction(MODEL_ACTION::IMPORT);
    }
}


QList<EDGE_KIND> ModelController::GetEdgeOrderIndexes(){
    QList<EDGE_KIND> indices;

    indices << EDGE_KIND::DEFINITION;
    indices << EDGE_KIND::AGGREGATE;
    indices << EDGE_KIND::DATA;
    indices << EDGE_KIND::QOS;
    indices << EDGE_KIND::ASSEMBLY;
    indices << EDGE_KIND::DEPLOYMENT;
    indices << EDGE_KIND::TRIGGER;

    for(auto kind : entity_factory->getEdgeKinds()){
        if(!indices.contains(kind)){
            qCritical() << "Edge Kind: " << entity_factory->getEdgeKindString(kind) << "Not ordered";
            indices << kind;
        }
    }
    return indices;
}



bool ModelController::setData_(Entity *entity, const QString& key_name, QVariant value, bool add_action)
{
    bool data_changed = false;
    if(entity){
        //Get an undo action
        auto action = getNewAction(GRAPHML_KIND::DATA);
        action.entity_id = entity->getID();
        action.Data.key_name = key_name;

        if(entity->gotData(key_name)){
            //Set the type
            action.Action.type = ACTION_TYPE::MODIFIED;
            //Store the old value
            action.Data.old_value = entity->getDataValue(key_name);
            //Update the new value
            data_changed = entity->setDataValue(key_name, value);
        }else{
            //Set the type
            action.Action.type = ACTION_TYPE::CONSTRUCTED;
            auto key = entity_factory->GetKey(key_name, value.type());
            auto data = entity_factory->AttachData(entity, key, ProtectedState::IGNORED, value);
        }

        if(data_changed){
            action.Data.new_value = entity->getDataValue(key_name);
            addActionToStack(action, add_action);
        }
    }
    return data_changed;
}

/**
 * @brief NewController::destructData - Removes and destroys a Data attached to an Entity
 * @param parent - The Entity the Data is attached to.
 * @param keyName - The Name of the Key of the Data
 * @param addAction - Add an undo state
 */
bool ModelController::destructData_(Entity* entity, const QString& key_name)
{  
    if(entity){
        //Get the data
        auto data = entity->getData(key_name);
        if(data){
            //Get an undo action
            auto action = getNewAction(GRAPHML_KIND::DATA);
            action.entity_id = entity->getID();
            action.Action.kind = GRAPHML_KIND::DATA;
            action.Action.type = ACTION_TYPE::DESTRUCTED;
            action.Data.key_name = key_name;
            action.Data.old_value = data->getValue();

            //Destroy the data
            entity_factory->DestructEntity(data);
            addActionToStack(action);
            return true;
        }
    }
    return false;
}




Node* ModelController::get_persistent_node(NODE_KIND node_kind){
    switch(node_kind){
        case NODE_KIND::MODEL:{
            return model;
        }
        case NODE_KIND::INTERFACE_DEFINITIONS:{
            return interfaceDefinitions;
        }
        case NODE_KIND::BEHAVIOUR_DEFINITIONS:{
            return behaviourDefinitions;
        }
        case NODE_KIND::WORKER_DEFINITIONS:{
            return workerDefinitions;
        }
        case NODE_KIND::ASSEMBLY_DEFINITIONS:{
            return assemblyDefinitions;
        }
        case NODE_KIND::DEPLOYMENT_DEFINITIONS:{
            return deploymentDefinitions;
        }
        case NODE_KIND::HARDWARE_DEFINITIONS:{
            return hardwareDefinitions;
        }
        case NODE_KIND::TRIGGER_DEFINITIONS:{
            return triggerDefinitions;
        }
        default:
            return 0;
    }
}

void ModelController::set_persistent_node(Node* node){
    if(node){
        auto node_kind = node->getNodeKind();
        protected_nodes << node;
        switch(node_kind){
        case NODE_KIND::MODEL:{
            model = node;
            break;
        }
        case NODE_KIND::INTERFACE_DEFINITIONS:{
            interfaceDefinitions = node;
            break;
        }
        case NODE_KIND::BEHAVIOUR_DEFINITIONS:{
            behaviourDefinitions = node;
            break;
        }
        case NODE_KIND::WORKER_DEFINITIONS:{
            workerDefinitions = node;
            break;
        }
        case NODE_KIND::ASSEMBLY_DEFINITIONS:{
            assemblyDefinitions = node;
            break;
        }
        case NODE_KIND::DEPLOYMENT_DEFINITIONS:{
            deploymentDefinitions = node;
            break;
        }
        case NODE_KIND::HARDWARE_DEFINITIONS:{
            hardwareDefinitions = node;
            break;
        }
        case NODE_KIND::TRIGGER_DEFINITIONS:{
            triggerDefinitions = node;
            break;
        }
        default:
            break;
        }
    }
}

Node* ModelController::construct_node(Node* parent_node, NODE_KIND node_kind, int index){
    auto persistent_node = get_persistent_node(node_kind);
    if(!persistent_node){
        //Construct node with default data
        auto node = entity_factory->CreateNode(node_kind, true); 
        if(index != -1){
            node->setDataValue(KeyName::Index, index);
        }
        return node;
    }
    return persistent_node;
}

Node* ModelController::construct_child_node(Node* parent_node, NODE_KIND node_kind, int index, bool notify_view){
    Node* node = 0;
    if(parent_node){
        //Don't construct nodes:
        node = construct_node(parent_node, node_kind, index);
        if(node){
            auto current_parent = node->getParentNode();

            //Check the current parent
            if(!current_parent){
                //If we don't have one set, try set the parent_node
                attachChildNode(parent_node, node, notify_view);
                current_parent = node->getParentNode();
            }

            //If we couldn't attach to the parent_node, we should remove 
            if(current_parent != parent_node){
                entity_factory->DestructEntity(node);
                //Free Memory
                node = 0;
            }
        }
    }
    return node;
}

void ModelController::addDependantsToDependants(Node* parent_node, Node* source){
    if(parent_node && source){
        if(isUserAction() && source->isDefinition()){
            for(auto dependant : parent_node->getDependants()){
                QList<QSet<NODE_KIND> > dependant_kind_list;
                
                if(dependant->isInstance()){
                    dependant_kind_list.append(source->getInstanceKinds());
                }else{
                    dependant_kind_list.append(source->getImplKinds());
                    dependant_kind_list.append(source->getInstanceKinds());
                }

                for(auto &dependant_kinds : dependant_kind_list){
                    bool constructed_dependant = false;
                    //Try Either
                    for(auto kind : dependant_kinds){
                        auto dependant_child = construct_connected_node(dependant, kind, source, EDGE_KIND::DEFINITION);
                        if(dependant_child){
                            constructed_dependant = true;
                            break;
                        }
                    }
                    if(constructed_dependant){
                        break;
                    }
                }
            }
        }
    }
}

Node* ModelController::construct_connected_node(Node* parent_node, NODE_KIND node_kind, Node* destination, EDGE_KIND edge_kind, int index){
    auto source = construct_node(parent_node, node_kind, index);
    bool success = false;
    if(source){
        //Try attach
        if(!source->getParentNode()){
            attachChildNode(parent_node, source, false);
        }

        auto source_parent_node = source->getParentNode();

        
        if(source_parent_node == parent_node && source_parent_node){
            auto edge = construct_edge(edge_kind, source, destination, false);
            if(edge){
                storeNode(source);
                storeEdge(edge);
                success = true;
            }else{
                qCritical() << "COULDN'T MAKE EDGE BETWEEN: " << source->toString() << " IN: " << parent_node->toString() << " < - > " << destination->toString() << " IN: " << (destination->getParentNode() ? destination->getParentNode()->toString() : " NO PARENT? ");
            }
        }
    }

    if(success){
        //Make sure we add Dependants
        addDependantsToDependants(parent_node, source);
    }else{
        if(source){
            //If we haven't construct an edge, we should delete the source element we created.
            entity_factory->DestructEntity(source);
            source = 0;
        }
    }
    return source;
}

Node* ModelController::constructNode(Node* parent_node, NODE_KIND kind, int index){
    //Add undo step
    triggerAction("Constructing Child Node");
    
    Node* node = 0;
    switch(kind){
        case NODE_KIND::COMPONENT:{
            node = construct_component_node(parent_node, index);
            break;
        }
        default:{
            node = construct_child_node(parent_node, kind, index);
            break;
        }
    }
    
    if(node){
        addDependantsToDependants(parent_node, node);
    }
    return node;
}

//SIGNAL
void ModelController::constructNodeAtPos(int parent_id, NODE_KIND kind, QPointF pos)
{
    QWriteLocker lock(&lock_);
    auto parent_node = entity_factory->GetNode(parent_id);
    auto node = constructNode(parent_node, kind);

    if (node && !pos.isNull()) {
        setData_(node, KeyName::X, pos.x());
        setData_(node, KeyName::Y, pos.y());
    }
    emit ActionFinished();
}

void ModelController::constructDDSQOSProfile(){
    if(assemblyDefinitions){
        constructNodeAtIndex(assemblyDefinitions->getID(), NODE_KIND::QOS_DDS_PROFILE, -1);
    }
}

void ModelController::constructTriggerDefinition()
{
    if (triggerDefinitions) {
        constructNodeAtIndex(triggerDefinitions->getID(), NODE_KIND::TRIGGER_DEFN, -1);
    }
}

void ModelController::constructNodeAtIndex(int parent_id, NODE_KIND kind, int index)
{
    QWriteLocker lock(&lock_);
    auto parent_node = entity_factory->GetNode(parent_id);
    auto node = constructNode(parent_node, kind, index);

    if(node && index >= 0){
        setData_(node, KeyName::Index, index);
    }
    emit ActionFinished();
}




void ModelController::constructEdges(QList<int> src_ids, QList<int> dst_ids, EDGE_KIND edge_kind){
    QWriteLocker lock(&lock_);
    
    auto src_nodes = getNodes(src_ids);
    auto dst_nodes = getNodes(dst_ids);
    bool success = true;
    //Have to go all to one
    if(src_nodes.count() == 1 || dst_nodes.count() == 1){
        for(auto src : src_nodes){
            for(auto dst : dst_nodes){

                auto edge = construct_edge(edge_kind, src, dst);
                if(!edge){
                    success = false;
                    break;
                }
            }
        }
    }
    emit ActionFinished();
}


void ModelController::destructEdges(QList<int> src_ids, QList<int> dst_ids, EDGE_KIND edge_kind)
{
    QWriteLocker lock(&lock_);

    triggerAction("Destructing edges");

    QList<Entity*> edges;
    
    for(auto src_id : src_ids){
        auto src = entity_factory->GetNode(src_id);

        for(auto dst_id : dst_ids){
            auto dst = entity_factory->GetNode(dst_id);
            if(src && dst){
                auto edge = src->getEdgeTo(dst, edge_kind);
                if(edge){
                    edges << edge;
                }
            }
        }
    }
    destructEntities(edges);
    emit ActionFinished();
}

void ModelController::destructAllEdges(QList<int> src_ids, EDGE_KIND edge_kind, QSet<EDGE_DIRECTION> edge_directions)
{
    QWriteLocker lock(&lock_);

    triggerAction("Destructing All edges");

    QList<Entity*> edges;

    for(auto src_id : src_ids){
        auto src = entity_factory->GetNode(src_id);
        for(auto edge : src->getEdgesOfKind(edge_kind)){
            if(edge){
                auto is_source = edge->getSource() == src;
                
                auto direction_to_delete = is_source ? EDGE_DIRECTION::TARGET : EDGE_DIRECTION::SOURCE;

                if(edge_directions.contains(direction_to_delete)){
                    edges << edge;
                }
            }
        }
    }
    destructEntities(edges);
    emit ActionFinished();
}

Node* ModelController::construct_component_node(Node* parent, int index){
    if(parent){
        triggerAction("Constructing Component");

        auto node = construct_child_node(parent, NODE_KIND::COMPONENT, index);
        
        if(node){
            auto impl = construct_connected_node(behaviourDefinitions, NODE_KIND::COMPONENT_IMPL, node, EDGE_KIND::DEFINITION);
            return node;
        }
    }
    return 0;
}

void ModelController::constructConnectedNodeAtPos(int parent_id, NODE_KIND node_kind, int dst_id, EDGE_KIND edge_kind, QPointF pos)
{
    QWriteLocker lock(&lock_);
    auto parent_node = entity_factory->GetNode(parent_id);
    auto dst_node = entity_factory->GetNode(dst_id);
    auto node = constructConnectedNode(parent_node, node_kind, dst_node, edge_kind);

    if (node && !pos.isNull()) {
        setData_(node, KeyName::X, pos.x());
        setData_(node, KeyName::Y, pos.y());
    }
    emit ActionFinished();
}

void ModelController::constructConnectedNodeAtIndex(int parent_id, NODE_KIND node_kind, int dst_id, EDGE_KIND edge_kind, int index){
    QWriteLocker lock(&lock_);
    auto parent_node = entity_factory->GetNode(parent_id);
    auto dst_node = entity_factory->GetNode(dst_id);
    auto node = constructConnectedNode(parent_node, node_kind, dst_node, edge_kind);

    if(node && index >= 0){
        setData_(node, KeyName::Index, index);
    }
    emit ActionFinished();
}

Node* ModelController::constructConnectedNode(Node* parent_node, NODE_KIND node_kind, Node* dst_node, EDGE_KIND edge_kind, int index){
    Node* node = 0;
    if(parent_node && dst_node){
        triggerAction("Constructed Connected Node");
        node = construct_connected_node(parent_node, node_kind, dst_node, edge_kind, index);
    }
    return node;
}


Edge* ModelController::construct_edge(EDGE_KIND edge_kind, Node *src, Node *dst, bool notify_view)
{
    Edge* edge = entity_factory->CreateEdge(src, dst, edge_kind);

    if(edge && notify_view){
        storeEdge(edge);
    }
    return edge;
}


QList<Node *> ModelController::getNodes(QList<int> ids)
{
    QList<Node* > nodes;
    for(auto id : ids){
        auto node = entity_factory->GetNode(id);
        if(node){
            nodes << node;
        }
    }
    return nodes;
}


void ModelController::triggerAction(const QString& actionName)
{
    actionCount++;
    currentAction = actionName;
    currentActionID = actionCount;
    emit UndoRedoUpdated();
}

void ModelController::undo()
{   
    if(canUndo()){
        ProgressUpdated_("Undoing");
        setModelAction(MODEL_ACTION::UNDO);
        undoRedo();
        unsetModelAction(MODEL_ACTION::UNDO);
        emit ShowProgress(false);
    }
    
    emit ActionFinished();
}

void ModelController::redo()
{
    if(canRedo()){
        ProgressUpdated_("Redoing");
        setModelAction(MODEL_ACTION::REDO);
        undoRedo();
        unsetModelAction(MODEL_ACTION::REDO);
        emit ShowProgress(false);
    }
    emit ActionFinished();
}

/**
 * @brief NewController::copy - Attempts to copy a list of entities defined by their IDs
 * @param IDs - The list of entity IDs
 */
QString ModelController::copy(QList<int> ids)
{
    QReadLocker lock(&lock_);
    auto selection = getOrderedEntities(ids);

    if(canCopy(selection)){
        auto value = _copy(selection);

        return value;
    }
    return "";
}

void ModelController::remove(QList<int> ids)
{
    QWriteLocker lock(&lock_);
    auto selection = getOrderedEntities(ids);
    if(canRemove(selection)){
        triggerAction("Removing Selection");
        destructEntities(selection);
    }
    emit ActionFinished();
}

void ModelController::replicate(QList<int> IDs)
{
    QWriteLocker lock(&lock_);

    auto selection = getOrderedEntities(IDs);
    bool success = false;

    if(canReplicate(selection)){
        success = _replicate(selection);
    }

    emit ActionFinished();
}

QString ModelController::cut(QList<int> ids)
{
    QWriteLocker lock(&lock_);

    auto selection = getOrderedEntities(ids);
    QString data;
    if(canCut(selection)){
        data = GraphmlPrinter({GraphmlPrinter::ExportFlags::EXPORT_ALL_EDGES}).ToGraphml(selection);
        emit triggerAction("Cutting Selection");
        destructEntities(selection);
    }
    emit ActionFinished();
    return data;
}

void ModelController::paste(QList<int> ids, const QString& xml)
{
    QWriteLocker lock(&lock_);

    auto selection = getOrderedEntities(ids);
    bool success = false;

    if(canPaste(selection)){
        auto first = selection.first();
        if(first->isNode()){
            success = _paste((Node*)first, xml);
        }
    }

    emit ActionFinished();
}

bool ModelController::_paste(Node* node, const QString& xml)
{
    if(node){
        triggerAction("Pasting Selection.");
        //Paste it into the current Selected Node,
        setModelAction(MODEL_ACTION::PASTE);
        bool success = importGraphML(xml, node);
        unsetModelAction(MODEL_ACTION::PASTE);
        return success;
    }
    return false;
}

bool ModelController::_replicate(QList<Entity *> items)
{
    auto xml = GraphmlPrinter({GraphmlPrinter::ExportFlags::EXPORT_ALL_EDGES}).ToGraphml(items);
    //Paste into the first parent node
    for(auto item : items){
        if(item->isNode()){
            auto parent_node = ((Node*)item)->getParentNode();
            if(parent_node){
                return _paste(parent_node, xml);
            }
        }
    }
    return false;
}



QHash<EDGE_DIRECTION, int> ModelController::getConnectableNodes(QList<int> src_ids, EDGE_KIND edge_kind){
    QHash<EDGE_DIRECTION, int> id_map;

    QReadLocker lock(&lock_);

    auto node_map = _getConnectableNodes(getNodes(src_ids), edge_kind);
    for(const auto& key : node_map.uniqueKeys()){
        for(const auto& value : node_map.values(key)){
            id_map.insertMulti(key, value->getID());
        }
    }
    return id_map;
}

QHash<EDGE_DIRECTION, Node*> ModelController::_getConnectableNodes(QList<Node*> src_nodes, EDGE_KIND edge_kind){
    QHash<EDGE_DIRECTION, Node*> node_map;
    
    bool srcs_accept_source_edge = true;
    bool srcs_accept_target_edge = true;
    
    //Check if they can all accept edges of the kind we care about.
    for(auto src : src_nodes){
        if(!src->canCurrentlyAcceptEdgeKind(edge_kind, EDGE_DIRECTION::SOURCE)){
            srcs_accept_source_edge = false;
            break;
        }
        if(!src->canCurrentlyAcceptEdgeKind(edge_kind, EDGE_DIRECTION::TARGET)){
            srcs_accept_target_edge = false;
        }
    }

    //Only itterate if we have nodes
    if((srcs_accept_source_edge || srcs_accept_target_edge) && src_nodes.size()){
        if(srcs_accept_source_edge){
            
            for(auto dst : entity_factory->GetNodesWhichAcceptEdgeKinds(edge_kind, EDGE_DIRECTION::TARGET)){
                bool src2dst_valid = true;

                for(auto src : src_nodes){
                    //Only check if for edge adoption if true, otherwise something can't adopt and thus no need to check
                    src2dst_valid = src2dst_valid ? src->canAcceptEdge(edge_kind, dst) : false;
                }
                
                if(src2dst_valid){
                    node_map.insertMulti(EDGE_DIRECTION::TARGET, dst);
                }
            }
        }
        if(srcs_accept_target_edge){
            for(auto dst : entity_factory->GetNodesWhichAcceptEdgeKinds(edge_kind, EDGE_DIRECTION::SOURCE)){
                bool dst2src_valid = true;

                for(auto src : src_nodes){
                    dst2src_valid = dst2src_valid ? dst->canAcceptEdge(edge_kind, src) : false;
                }
                
                if(dst2src_valid){
                    node_map.insertMulti(EDGE_DIRECTION::SOURCE, dst);
                }
            }
        }
    }
    return node_map;
}

QList<Entity*> ModelController::getOrderedEntities(const QList<int>& ids){
    return getOrderedEntities(getUnorderedEntities(ids));
}

QList<Entity*> ModelController::getOrderedEntities(QList<Entity*> entities){
    //Sort the node (Deepest Nodes later, edges last)
    std::sort(entities.begin(), entities.end(), [](const Entity* a, const Entity* b){
        auto is_a_node = a->isNode();
        auto is_b_node = b->isNode();

        if(is_a_node && is_b_node){
            auto a_node = (const Node*)a;
            auto b_node = (const Node*)b;
            //Proper depth
            auto a_depth = a_node->getDepth();
            auto b_depth = b_node->getDepth();
            return a_depth < b_depth;
        }else{
            return is_a_node;
        }
        });

    //return entities;
    QList<Entity*> top_entities;
    
    //Foreach node
    for(auto node : entities){
        bool got_parent = false;
        //Check the node if we have a parent for it.
        for(auto top : top_entities){
            if(top->isNode()){
                auto parent_node = (Node*) top;
                if(parent_node->isAncestorOf(node)){
                    got_parent = true;
                    break;
                }
            }
        }
        if(!got_parent){
            top_entities += node;
        }
    }
    return top_entities;
}

QList<QVariant> ModelController::getValidKeyValues(int id, const QString& key_name)
{
    QReadLocker lock(&lock_);
    QList<QVariant> valid_values;
    
    auto entity = entity_factory->GetEntity(id);
    if(entity){
        auto data = entity->getData(key_name);
        if(data){
            valid_values = data->getValidValues();
        }
    }
    return valid_values;
}

int ModelController::getSharedParent(int id, int id2){
    QReadLocker lock(&lock_);

    auto node_1 = entity_factory->GetNode(id);
    auto node_2 = entity_factory->GetNode(id2);
    if(node_1 && node_2){
        auto parent = node_1->getCommonAncestor(node_2);
        if(parent){
            return parent->getID();
        }
    }
    return -1;
}

bool ModelController::isNodeAncestor(int ID, int ID2){
    QReadLocker lock(&lock_);
    bool is_ancestor = false;
    auto node_1 = entity_factory->GetNode(ID);
    auto node_2 = entity_factory->GetNode(ID2);
    if(node_1 && node_2){
        is_ancestor = node_1->isAncestorOf(node_2);
    }
    return is_ancestor;
}

VIEW_ASPECT ModelController::getNodeViewAspect(int ID){
    QReadLocker lock(&lock_);
    VIEW_ASPECT aspect = VIEW_ASPECT::NONE;
    auto node = entity_factory->GetNode(ID);
    
    if(node){
        aspect = node->getViewAspect();
    }
    return aspect;
}


QList<DataUpdate> ModelController::getEntityDataList(int ID){
    QList<DataUpdate> data_list;
    QReadLocker lock(&lock_);

    auto entity = entity_factory->GetEntity(ID);
    if(entity){
        for(auto data : entity->getData()){

            data_list += DataUpdate({data->getKeyName(), data->getValue(), data->isProtected()});
        }
    }
    return data_list;
}


QVariant ModelController::getEntityDataValue(int ID, const QString& key_name){
    QReadLocker lock(&lock_);
    QVariant value;

    auto entity = entity_factory->GetEntity(ID);
    if(entity){
        value = entity->getDataValue(key_name);
    }
    return value;
}

QSet<NODE_TYPE> ModelController::getNodesTypes(int ID){
    QReadLocker lock(&lock_);
    auto node = entity_factory->GetNode(ID);
    if(node){
        return node->getTypes();
    }
    return {};
}


void ModelController::setProjectSaved(const QString& path)
{
    setProjectModified(false);
    if(path != ""){
        //Update the file save path.
        setProjectPath(path);
    }
}



bool ModelController::storeEntity(Entity* item, int desired_id)
{
    bool success = false;
    if(item){
        try{
            entity_factory->RegisterEntity(item, desired_id);
        }catch(const std::exception & ex){
            qCritical() << ex.what();
            return false;
        }
        auto id = item->getID();

        bool inserted = false;
        if(item->isNode()){
            if(!node_ids_.contains(id)){
                node_ids_.insert(id);
                auto node = (Node*) item;
                connect(node, &Node::acceptedEdgeKindsChanged, [=](){emit NodeEdgeKindsChanged(id);});
                connect(node, &Node::typesChanged, [=](){emit NodeTypesChanged(id);});
                
                emit NodeConstructed(node->getParentNodeID(), id, node->getNodeKind());
                inserted = true;
            }
        }else if(item->isEdge()){
            if(!edge_ids_.contains(id)){
                edge_ids_.insert(id);
                auto edge = (Edge*) item;
                emit EdgeConstructed(id, edge->getEdgeKind(), edge->getSourceID(), edge->getDestinationID());
                inserted = true;
            }
        }

        if(inserted){
            
            connect(item, &Entity::dataChanged, [=](int ID, QString key_name, QVariant value, bool is_protected){
                DataUpdate data;
                data.key_name = key_name;
                data.value = value;
                data.is_protected = is_protected;

                emit DataChanged(ID, data);
            });
            
            connect(item, &Entity::dataRemoved, this, &ModelController::DataRemoved, Qt::UniqueConnection);
        }
        success = inserted;
    }
    return success;
}


void ModelController::removeEntity(Entity* entity)
{
    if(entity){
        auto id = entity->getID();
        auto kind = entity->getGraphMLKind();
        emit EntityDestructed(id, kind);
        node_ids_.remove(id);
        edge_ids_.remove(id);
        entity_factory->DestructEntity(entity);
    }
}


bool ModelController::attachChildNode(Node *parent_node, Node *node, bool notify_view)
{
    if(parent_node->addChild(node)){
        if(notify_view){
            storeNode(node);
        }   
        return true;
    }
    return false;
}

QList<int> ModelController::getIDs(QList<Entity *> items)
{
    QList<int> ids;
    for(auto item : items){
        ids << item->getID();
    }
    return ids;
}

QList<int> ModelController::getIDs(QList<Node *> nodes)
{
    QList<int> ids;
    for(auto item : nodes){
        ids << item->getID();
    }
    return ids;
}


QList<Entity *> ModelController::getUnorderedEntities(const QList<int>& ids){
    QList<Entity*> entities;
    for(const auto& id : ids){
        auto entity = entity_factory->GetEntity(id);
        if(entity){
            entities << entity;
        }
    }
    return entities;
}


//Look inside target_node for things which could become a dependant of the the definition
QList<Node*> ModelController::get_matching_dependant_of_definition(Node* target_node, Node* definition){
    QList<QSet<NODE_KIND> > dependant_kind_list;
    
    if(target_node->isInstance()){
        dependant_kind_list.append(definition->getInstanceKinds());
    }else{
        dependant_kind_list.append(definition->getImplKinds());
        dependant_kind_list.append(definition->getInstanceKinds());
    }
    
    QList<Node*> matching_nodes;
    
    for(auto &dependant_kinds : dependant_kind_list){
        //For each child in parent, check to see if any Nodes match Label/Type
        for(auto target_child : target_node->getChildrenOfKinds(dependant_kinds)){
            if(target_child->getDefinition() == definition){
                matching_nodes << target_child;
            }else if(target_child->getDefinition()){
                
            }else{
                auto target_type_data = target_child->getData(KeyName::Type);
                auto definition_type_data = definition->getData(KeyName::Type);
                if (Data::CompareData(target_type_data, definition_type_data)) {
                    matching_nodes << target_child;
                }
            }
        }
    }

    return matching_nodes;
}

void ModelController::destructEdge_(Edge *edge){
    if(edge){
        auto id = edge->getID();
        //Teardown specific edge classes.
        auto edge_kind = edge->getEdgeKind();
        auto src = edge->getSource();
        auto dst = edge->getDestination();
        
        if(src && dst){
            switch(edge_kind){
            case EDGE_KIND::DEFINITION:{
                setupDefinitionRelationship(src, dst, false);
                break;
            }
            case EDGE_KIND::AGGREGATE:{
                setupAggregateRelationship(src, dst, false);
                break;
            }
            case EDGE_KIND::DATA:{
                DataNode::BindDataRelationship(src, dst, false);
                break;
            }
            default:
                break;
            }

            //Remove it from the hash of GraphML
            removeEntity(edge);
        }
    }
}

void ModelController::destructEntity(int ID){
    destructEntity(entity_factory->GetEntity(ID));
}

void ModelController::destructEntity(Entity* item){
    destructEntities({item});
}

void ModelController::destructEntities(QList<Entity*> entities)
{
    QSet<Entity*> nodes;
    QSet<Entity*> edges;

    for(auto entity : entities){
        if(entity){
            if(entity->isNode()){
                auto node = (Node*) entity;
                nodes += node;
                for(auto child : node->getNestedDependants()){
                    nodes += child;
                }
            }else if(entity->isEdge()){
                auto edge = (Edge*) entity;
                edges += edge;
            }
        }
    }
    auto sorted_nodes = getOrderedEntities(nodes.toList());
    //Get all the edges
    for(auto n : sorted_nodes){
        for(auto edge : ((Node*)n)->getAllEdges()){
            edges += edge;
        }
    }
    if(edges.size()){
        //Create an undo state which groups all edges together
        auto action = getNewAction(GRAPHML_KIND::EDGE);
        action.Action.type = ACTION_TYPE::DESTRUCTED;

        action.xml = GraphmlPrinter({GraphmlPrinter::ExportFlags::EXPORT_ALL_EDGES}).ToGraphml(edges.toList());
        addActionToStack(action);
        

        std::for_each(edges.begin(), edges.end(), [this](Entity* edge){
            destructEdge_((Edge*)edge);
            }
            );
    }
    
    for(auto entity : sorted_nodes){
        auto node = (Node*) entity;
        
        //Don't do this
        if(!isModelAction(MODEL_ACTION::DESTRUCTING)){
            auto action = getNewAction(GRAPHML_KIND::NODE);
            action.entity_id = node->getID();
            action.parent_id = node->getParentNodeID();
            action.Action.type = ACTION_TYPE::DESTRUCTED;
            
            action.xml = GraphmlPrinter().ToGraphml({node});
            addActionToStack(action);
        }

        destructNode_(node);
    }
}


void ModelController::destructNode_(Node* node){
    if(node){
        //Remove children
        auto children = node->getChildren();
        std::for_each(children.rbegin(), children.rend(), std::bind(&ModelController::removeEntity, this, std::placeholders::_1));
        removeEntity(node);
    }
}


bool ModelController::reverseAction(HistoryAction action)
{   
    switch(action.Action.type){
    case ACTION_TYPE::CONSTRUCTED:{
        auto entity = entity_factory->GetEntity(action.entity_id);

        switch(action.Action.kind){
        case GRAPHML_KIND::NODE:
        case GRAPHML_KIND::EDGE:{
            if(!entity){
                return false;
            }
            destructEntities({entity});
            return true;
        }
        case GRAPHML_KIND::DATA:{
            return destructData_(entity, action.Data.key_name);
        }
        default:
            break;
        }
        break;
    }
    case ACTION_TYPE::DESTRUCTED:{
        switch(action.Action.kind){
        case GRAPHML_KIND::EDGE:
        case GRAPHML_KIND::NODE:{
            auto parent_node = entity_factory->GetNode(action.parent_id);
            //Don't pass a MODEL_ACTION
            return importGraphML(action.xml, parent_node);
        }
        case GRAPHML_KIND::DATA:{
            auto entity = entity_factory->GetEntity(action.entity_id);
            return setData_(entity, action.Data.key_name, action.Data.old_value);
        }
        default:
            break;
        }
        break;
    }
    case ACTION_TYPE::MODIFIED:{
        switch(action.Action.kind){
        case GRAPHML_KIND::DATA:{
            auto entity = entity_factory->GetEntity(action.entity_id);
            return setData_(entity, action.Data.key_name, action.Data.old_value);
        }
        default:
            break;
        }
        break;
    }
    }
    return false;
}


void ModelController::addActionToStack(HistoryAction action, bool useAction)
{
    //Change made, make dirty the project
    setProjectModified(true);

    if(useAction){
        if(isModelAction(MODEL_ACTION::UNDO)){
            redo_stack.push(action);
        }else{
            undo_stack.push(action);
        }
        emit UndoRedoUpdated();
    }
}

bool ModelController::undoRedo()
{
    QStack<HistoryAction> action_stack;

    //replace the stacks
    if(isModelAction(MODEL_ACTION::UNDO)){
        undo_stack.swap(action_stack);
    }else if(isModelAction(MODEL_ACTION::REDO)){
        redo_stack.swap(action_stack);
    }

    auto action_id = action_stack.top().Action.id;
    auto action_name = action_stack.top().Action.name;

    triggerAction(action_name);

    QList<HistoryAction> reverse_actions;
    
    while(!action_stack.empty()){
        auto action = action_stack.top();
        if(action.Action.id == action_id){
            action_stack.pop();
            //If the action has the same id as the top, it's part of the same state.
            reverse_actions.push_back(action);
        }else{
            break;
        }
    }
    
    double action_count = reverse_actions.size() / 100.0;
    int actions_reversed = 0;
    
    bool success = true;
    for(auto action : reverse_actions){
        if(reverseAction(action)){
            actions_reversed ++;
            ProgressChanged_(++actions_reversed / action_count);
        }else{
            success = false;
        }
    }

    //replace the stacks
    if(isModelAction(MODEL_ACTION::UNDO)){
        undo_stack.swap(action_stack);
    }else if(isModelAction(MODEL_ACTION::REDO)){
        redo_stack.swap(action_stack);
    }

    emit UndoRedoUpdated();
    return success;
}

bool ModelController::canDeleteNode(Node *node)
{
    if(node){
        if(protected_nodes.contains(node)){
            return false;
        }

        if(node->isImplicitlyConstructed()){
            return false;
        }

        auto node_kind = node->getNodeKind();
        auto parent_node = node->getParentNode();
        auto parent_node_kind = parent_node ? parent_node->getNodeKind() : NODE_KIND::NONE;

        switch(node_kind){
            case NODE_KIND::ATTRIBUTE_INST:
                if(parent_node_kind == NODE_KIND::PORT_PERIODIC){
                    return false;
                }
                break;
            default:
                break;
        }

        if(node->getDefinition()){
            switch(node_kind){
            case NODE_KIND::CLASS_INST:{
                auto parent_node_kind = parent_node ? parent_node->getNodeKind() : NODE_KIND::NONE;
                 switch(parent_node_kind){
                    case NODE_KIND::COMPONENT_INST:
                        return false;
                    default:
                        break;
                }
                break;
            }
            case NODE_KIND::EXTERNAL_TYPE:
            case NODE_KIND::FUNCTION_CALL:
            case NODE_KIND::CALLBACK_FNC_INST:
            case NODE_KIND::PORT_REQUESTER_IMPL:
            case NODE_KIND::PORT_PUBLISHER_IMPL:
            case NODE_KIND::PORT_REPLIER:
            case NODE_KIND::PORT_REQUESTER:
            case NODE_KIND::COMPONENT_INST:
                // These node kinds can be destroyed at any time
                break;
            case NODE_KIND::ENUM_INST:
            case NODE_KIND::AGGREGATE_INST:{
                auto parent_node_kind = parent_node ? parent_node->getNodeKind() : NODE_KIND::NONE;

                switch(parent_node_kind){
                    case NODE_KIND::AGGREGATE:
                    case NODE_KIND::VECTOR:
                    case NODE_KIND::VARIABLE:
                    case NODE_KIND::INPUT_PARAMETER_GROUP:
                    case NODE_KIND::INPUT_PARAMETER_GROUP_INST:
                    case NODE_KIND::RETURN_PARAMETER_GROUP:
                    case NODE_KIND::RETURN_PARAMETER_GROUP_INST:
                        break;
                    default:
                        return false;
                }
                break;
            }
            case NODE_KIND::TRIGGER_INST:
                break;
            default:
                //Can't remove instances
                return false;
            }
        }
    }

    return true;
}





void ModelController::clearHistory()
{
    currentActionID = 0;
    actionCount = 0;
    currentAction = "";
    undo_stack.clear();
    redo_stack.clear();
    emit UndoRedoUpdated();
}


QList<EDGE_KIND> ModelController::getPotentialEdgeClasses(Node *src, Node *dst)
{
    QList<EDGE_KIND> edge_kinds;

    for(auto edge_kind : GetEdgeOrderIndexes()){
        if(src->canCurrentlyAcceptEdgeKind(edge_kind, EDGE_DIRECTION::SOURCE) && dst->canCurrentlyAcceptEdgeKind(edge_kind, EDGE_DIRECTION::TARGET)){
            edge_kinds << edge_kind;
        }
    }
    return edge_kinds;
}

QString ModelController::_copy(QList<Entity *> selection)
{
    return GraphmlPrinter().ToGraphml(selection);
}


bool ModelController::storeNode(Node* node, int desired_id, bool store_children, bool add_action)
{
    bool success = false;
    if(node){
        success = storeEntity(node, desired_id);

        if(success){
            if(store_children){
                //Get all our children
                for(auto child : node->getChildren()){
                    if(child->isImplicitlyConstructed()){
                        storeEntity(child, -1);
                    }
                }
                
                for(auto edge : node->getAllEdges()){
                    if(edge->isImplicitlyConstructed()){
                        storeEntity(edge, -1);
                    }
                }
            }

            if(add_action){
                //Construct an ActionItem to reverse Node Construction.
                auto action = getNewAction(GRAPHML_KIND::NODE);
                action.Action.type = ACTION_TYPE::CONSTRUCTED;
                action.entity_id = node->getID();
                action.parent_id = node->getParentNodeID();
                action.xml = GraphmlPrinter().ToGraphml(*node);
                addActionToStack(action);
            }
        }else{
            //qCritical() << "Trying to store: " << node->toString() << " @ ID: " << desired_id;
        }
    }
    return success;
}

void ModelController::setupModel()
{
    set_persistent_node(construct_node(0, NODE_KIND::MODEL));
    if(model){
        for(auto child : model->getChildren()){
            set_persistent_node(child);
        }
        
        connect(model, &Entity::dataChanged, [=](int ID, QString key_name, QVariant value, bool is_protected){
            if (key_name == KeyName::Label) {
                emit ProjectNameChanged(value.toString());
            }
        });

        storeNode(model, -1, true, false);
    }
}


bool ModelController::setupDefinitionRelationship(Node* instance, Node* definition, bool setup){
    if(!(instance && definition)){
        return false;
    }

    auto construct_instance = instance->isInstance();
    auto construct_implementation = instance->isImpl();


    //Construct the Relationship at the Node Level
    if(instance->gotEdgeTo(definition, EDGE_KIND::DEFINITION)){
        if(construct_instance){
            if(setup){
                definition->addInstance(instance);
            }else{
                definition->removeInstance(instance);
            }
        }else if(construct_implementation){
            if(setup){
                definition->addImplementation(instance);
            }else{
                definition->removeImplementation(instance);
            }
        }
    }

    if(setup && isUserAction()){
        //Get our direct children of definitions
        for(auto def_child : instance->getRequiredInstanceDefinitions()){
            bool construct_dependant = true;


            for(auto matching_dependant : get_matching_dependant_of_definition(instance, def_child)){
                construct_edge(EDGE_KIND::DEFINITION, matching_dependant, def_child);

                if(matching_dependant->getDefinition() == def_child){
                    construct_dependant = false;
                    break;
                }
            }

            if(construct_dependant){
                QList<QSet<NODE_KIND> > dependant_kind_list;

                if(construct_instance){
                    dependant_kind_list.append(def_child->getInstanceKinds());
                }else{
                    dependant_kind_list.append(def_child->getImplKinds());
                    dependant_kind_list.append(def_child->getInstanceKinds());
                }

                
                for(auto &dependant_kinds : dependant_kind_list){
                    bool got_node = false;    
                    //Try Either
                    for(auto kind : dependant_kinds){
                        auto dependant_child = construct_connected_node(instance, kind, def_child, EDGE_KIND::DEFINITION);
                        if(dependant_child){
                            got_node = true;
                            break;
                        }
                    }
                    if(got_node){
                        break;
                    }
                }
            }
        }
    }
    
    return true;
}


/**
 * @brief NewController::setupAggregateRelationship
 * Attempts to find/construct an AggregateInstance of the provided Aggregate inside the EventPort.
 * @param eventPort - The EventPort which the Aggregate is getting set to.
 * @param aggregate - the Aggregate to set.
 * @return true if Aggregate Relation was setup correctly.
 */
bool ModelController::setupAggregateRelationship(Node *src, Node *dst, bool setup)
{
    if(src && dst && src->isNodeOfType(NODE_TYPE::EVENTPORT)){
        auto eventport = (EventPort*) src;
        auto dst_nodekind = dst->getNodeKind();


        bool construct_instance = setup && isUserAction() && !eventport->IsEdgeRuleActive(Node::EdgeRule::IGNORE_REQUIRED_INSTANCE_DEFINITIONS);
        //Only auto construct if we are processing a user action.
        if(construct_instance){
            
            QList<Node*> instances_to_make;
            if(dst_nodekind == NODE_KIND::AGGREGATE){
                instances_to_make += dst;
            }else{
                instances_to_make += dst->getChildren(0);
            }


            //Make Instances of all things
            for(auto definition : instances_to_make){
                auto instance_kind = *(definition->getInstanceKinds().begin());

                Node* instance_node = 0;
                for(auto child : eventport->getChildrenOfKind(instance_kind)){
                    instance_node = child;
                    break;
                }

                if(!instance_node){
                    //Construct a connected AggregateInstance inside the eventport, connected to the aggregate (The instance section will)
                    instance_node = construct_connected_node(eventport, instance_kind, definition, EDGE_KIND::DEFINITION);
                }

                //If we have an aggregate instance
                if(definition){
                    if(!instance_node->gotEdgeTo(definition, EDGE_KIND::DEFINITION)){
                        //Try and construct an edge between the AggregateInstance and it's Instance
                        construct_edge(EDGE_KIND::DEFINITION, instance_node, definition);
                    }
                }
            }
        }

        if(eventport->getNodeKind() == NODE_KIND::EXTERNAL_PUBSUB_DELEGATE || eventport->getNodeKind() == NODE_KIND::EXTERNAL_SERVER_DELEGATE){
            //Connect our children
            for(auto child : eventport->getChildren()){
                construct_edge(EDGE_KIND::AGGREGATE, child, dst);
            }
        }

        //Check for an edge between the EventPort and the Aggregate
        if(eventport->gotEdgeTo(dst, EDGE_KIND::AGGREGATE)){
            if(setup){
                //Edge Created Set Aggregate relation.
                eventport->setPortType(dst);
            }else{
                //Unset the Aggregate
                eventport->unsetPortType();
            }
            return true;
        }
    }
    return false;
}


bool ModelController::isGraphMLValid(const QString& inputGraphML)
{
    //Construct a Stream Reader for the XML graph
    QXmlStreamReader xmlErrorChecking(inputGraphML);

    //Check for Errors
    while(!xmlErrorChecking.atEnd()){
        xmlErrorChecking.readNext();
        if(xmlErrorChecking.hasError()){
            qCritical() << "isGraphMLValid(): Parsing Error! Line #" << xmlErrorChecking.lineNumber();
            qCritical() << "\t" << xmlErrorChecking.errorString();
            return false;
        }
    }
    return true;
}

bool ModelController::storeEdge(Edge *edge, int desired_id)
{
    bool success = false;
    if(edge){
        //Used to be Before
        success = storeEntity(edge, desired_id);

        //Construct an ActionItem to reverse an Edge Construction.
        auto action = getNewAction(GRAPHML_KIND::EDGE);
        action.Action.type = ACTION_TYPE::CONSTRUCTED;
        action.Action.kind = edge->getGraphMLKind();
        action.entity_id = edge->getID();
        //Add Action to the Undo/Redo Stack
        addActionToStack(action);


        auto src = edge->getSource();
        auto dst = edge->getDestination();

        //Do Special GUI related things
        switch(edge->getEdgeKind()){
        case EDGE_KIND::DEFINITION:{
            setupDefinitionRelationship(src, dst, true);
            break;
        }
        case EDGE_KIND::AGGREGATE:{
            setupAggregateRelationship(src, dst, true);
            break;
        }
        case EDGE_KIND::DATA:{
            DataNode::BindDataRelationship(src, dst, true);
            break;
        }
        default:
            break;
        }
        
    }
    return success;
}

QString ModelController::getProjectAsGraphML(bool human_readable, bool functional_export)
{
    QReadLocker locker(&lock_);
    
    QSet<GraphmlPrinter::ExportFlags> flags;
    if(human_readable){
        flags += GraphmlPrinter::ExportFlags::HUMAN_READABLE_DATA;
    }
    if(functional_export){
        flags += GraphmlPrinter::ExportFlags::STRIP_VISUAL_DATA;
    }

    return GraphmlPrinter(flags).ToGraphml({model});
}



QSet<EDGE_KIND> ModelController::getCurrentEdgeKinds(QList<int> ids)
{
    QReadLocker lock(&lock_);
    QSet<EDGE_KIND> edge_kinds;

    for(auto entity : getUnorderedEntities(ids)){
        if(entity->isNode()){
            auto node = (Node*) entity;
            for(auto edge : node->getEdges()){
                edge_kinds.insert(edge->getEdgeKind());
            }
        }
    }
    return edge_kinds;
}


QSet<NODE_KIND> ModelController::getValidNodeKinds(int ID)
{
    QReadLocker lock(&lock_);
    QSet<NODE_KIND> node_kinds;

    auto parent_node = entity_factory->GetNode(ID);

    if(parent_node){
        for(auto node_kind : parent_node->getUserConstructableNodeKinds()){
            auto temp_node = entity_factory->CreateTempNode(node_kind);
            if(temp_node){
                if(parent_node->canAdoptChild(temp_node)){
                    node_kinds << node_kind;
                }
                entity_factory->DestructEntity(temp_node);
            }
        }
    }
    return node_kinds;
}

QPair<QSet<EDGE_KIND>, QSet<EDGE_KIND> > ModelController::getValidEdgeKinds(QList<int> ids){
    QReadLocker lock(&lock_);
    QPair<QSet<EDGE_KIND>, QSet<EDGE_KIND> > valid_pairs;

    auto entities = getUnorderedEntities(ids);

    if(!entities.isEmpty()){
        valid_pairs.first = entity_factory->getEdgeKinds().toSet();
        valid_pairs.second = entity_factory->getEdgeKinds().toSet();
    }

    for(auto entity : entities){
        if(entity->isNode()){
            auto node = (Node*) entity;
            valid_pairs.first &= node->getAcceptedEdgeKind(EDGE_DIRECTION::SOURCE);
            valid_pairs.second &= node->getAcceptedEdgeKind(EDGE_DIRECTION::TARGET);
        }else{
            valid_pairs.first.clear();
            valid_pairs.second.clear();
        }
        if(valid_pairs.first.isEmpty() && valid_pairs.second.isEmpty()){
            break;
        }
    }
    return valid_pairs;
}


QList<int> ModelController::getConstructablesConnectableNodes(int constructable_parent_id, NODE_KIND constructable_node_kind, EDGE_KIND connection_edge_kind){
    QWriteLocker lock(&lock_);
    
    QList<Node*> nodes;

    auto parent_node = entity_factory->GetNode(constructable_parent_id);
    if(parent_node){
        auto child_node = entity_factory->CreateTempNode(constructable_node_kind);
        if(child_node){
            if(parent_node->addChild(child_node)){
                nodes = _getConnectableNodes({child_node}, connection_edge_kind).values(EDGE_DIRECTION::TARGET);
            }
            entity_factory->DestructEntity(child_node);
        }
    }
    
    return getIDs(nodes);
}


/**
 * @brief NewController::setData Sets the Value of the Data of an Entity.
 * @param parentID - The ID of the Entity
 * @param keyName - The name of the Key
 * @param dataValue - The new value of the Data.
 */
void ModelController::setData(int parentID, const QString& keyName, QVariant dataValue)
{
    Entity* graphML = entity_factory->GetEntity(parentID);
    if(graphML){
        //qCritical() << "== REQUEST: " << parentID << " KEY: " << keyName << " = " << dataValue;
        setData_(graphML, keyName, dataValue, true);
    }
}

void ModelController::removeData(int id, const QString& key_name)
{
    destructData_(entity_factory->GetEntity(id), key_name);
}



/**
 * @brief NewController::importProjects
 * @param xmlDataList
 */
bool ModelController::importProjects(QStringList xml_list)
{
    bool success = true;
    //ProgressUpdated_("Importing Projects");
    if(xml_list.length() > 0){
        triggerAction("Importing GraphML Projects.");
        setModelAction(MODEL_ACTION::IMPORT);
        for(auto xml : xml_list){
            auto result = importGraphML(xml, model);
            if(!result){
                QString title = "Cannot import project";
                emit Notification(MODEL_SEVERITY::ERROR, title);
                success = false;
            }
        }
        unsetModelAction(MODEL_ACTION::IMPORT);
    }
    emit ShowProgress(false);
    emit ActionFinished();
    return success;
}


void ModelController::ProgressChanged_(int progress){
    if(progress != last_progress){
        last_progress = progress;
        emit ProgressChanged(progress);
    }
}

void ModelController::ProgressUpdated_(const QString& description){
    if(description != last_description){
        last_description = description;
        emit ShowProgress(true, description);
        last_progress = -2;
    }
}

bool ModelController::isKeyNameVisual(const QString& key_name){
    return GraphmlPrinter::IsKeyVisual(key_name);
}

bool ModelController::importGraphML(const QString& document, Node *parent)
{
    //Lookup for key's ID to Key* object
    QHash <QString, Key*> key_hash;

    QStack<QString> node_ids;
    QStack<QString> edge_ids;
    QStack<QString> unique_entity_ids;

    QHash<QString, TempEntity*> entity_hash;

    if(!parent){
        //Set parent as Model item.
        parent = model;
    }

    if(!isGraphMLValid(document)){
        QString title = "Cannot import graphml";
        QString description = "importGraphML(): Document is not valid XML";
        emit Notification(MODEL_SEVERITY::ERROR, title, description);
        return false;
    }

    int error_count = 0;

    //Now we know we have no errors, so read Stream again.
    QXmlStreamReader xml(document);

    auto current_entity = new TempEntity(GRAPHML_KIND::NODE);
    current_entity->setID(parent->getID());

    //State flags
    const bool IS_OPEN = isModelAction(MODEL_ACTION::OPEN);
    const bool IS_IMPORT = isModelAction(MODEL_ACTION::IMPORT);
    const bool IS_UNDO_REDO = isModelAction(MODEL_ACTION::UNDO) || isModelAction(MODEL_ACTION::REDO);
    const bool IS_PASTE = isModelAction(MODEL_ACTION::PASTE);
    const bool IS_PARTIAL_IMPORT = IS_UNDO_REDO || IS_PASTE;
    const bool IS_SETUP = isModelAction(MODEL_ACTION::SETUP);
    const bool UPDATE_PROGRESS = !isModelAction(MODEL_ACTION::NONE) && !IS_SETUP;
    const bool GENERATE_UUID_FROM_ID = IS_IMPORT && (parent == workerDefinitions);

    if(UPDATE_PROGRESS){
        ProgressUpdated_("Parsing Project");
        ProgressChanged_(-1);
    }
    
    auto start = QDateTime::currentDateTime().toMSecsSinceEpoch();

    QString model_import_version = Version::GetMedeaVersion();

    while(xml.readNext() != QXmlStreamReader::EndDocument){
        auto kind = getGraphMLKindFromString(xml.name());

        //Handle the elements
        if(xml.isStartElement()){
            switch(kind){
                case GRAPHML_KIND::KEY:{
                    auto key_id = getXMLAttribute(xml, "id");
                    auto key_name = getXMLAttribute(xml, "attr.name");
                    auto key_type = Key::getTypeFromGraphML(getXMLAttribute(xml, "attr.type"));
                    auto key = entity_factory->GetKey(key_name, key_type);
                    if(key){
                        if(!key_hash.contains(key_id)){
                            //First time we've seen it
                            key_hash.insert(key_id, key);
                        }else{
                            qCritical() << "ImportGraphML: Duplicate key id '" << key_id << "'";
                            error_count ++;
                            continue;
                        }
                    }else{
                        qCritical() << "ImportGraphML: Couldn't construct key id '" << key_id << "'";
                        error_count ++;
                        continue;
                    }
                    break;
                }
                case GRAPHML_KIND::DATA:{
                    auto key_id = getXMLAttribute(xml, "key");
                    auto key = key_hash.value(key_id, 0);

                    if(current_entity && key){
                        auto key_name = key->getName();
                        auto value = xml.readElementText();
                        
                        if(key_name == KeyName::UUID){
                            //Run the UUID through
                            value = ExportIDKey::GetUUIDOfValue(value);
                            unique_entity_ids.push_back(current_entity->getIDStr());
                        }else if(key_name == KeyName::MedeaVersion){
                            model_import_version = value;
                            int model_version = 0;
                            try {
                                model_version = VersionKey::CompareVersion(value, Version::GetMedeaVersion());
                            } catch(const std::invalid_argument& ex){
                                qCritical() << "ImportGraphML: Invalid version number format id in medea_version: " << ex.what();
                                error_count ++;
                                continue;
                            }

                            if(model_version > 0){
                                QString title = "Loading model from future MEDEA";
                                QString description = "Model was created in a new version of MEDEA (v" % value % "). Some functionality may not be supported.";
                                emit Notification(MODEL_SEVERITY::WARNING, title, description);
                            }else if(model_version < 0){
                                QString title = "Loading model from legacy MEDEA";
                                QString description = "Model was created in MEDEA (v" % value % "). Some functionality may have changed. Model version updated to: " % Version::GetMedeaVersion();
                                emit Notification(MODEL_SEVERITY::INFO, title, description);
                                //Update
                                value = Version::GetMedeaVersion();
                            }else{
                                value = Version::GetMedeaVersion();
                            }
                        }
                        //Add the data to the entity
                        current_entity->addData(key_name, value);
                    }else{
                        qCritical() << "ImportGraphML: Couldn't attach Data with key id: '" << key_id << "'";
                        error_count ++;
                        continue;
                    }
                    break;
                }
                case GRAPHML_KIND::EDGE:
                case GRAPHML_KIND::NODE:{
                    auto id = getXMLAttribute(xml, "id");
                    auto entity = new TempEntity(kind, current_entity);
                    entity->setIDStr(id);
                    entity->setLineNumber(xml.lineNumber());

                    if(GENERATE_UUID_FROM_ID){
                        //If we are meant to generate UUIDS
                        //Use the String'd ID as the seed for the UUID
                        auto uuid_id = ExportIDKey::GetUUIDOfValue(id);
                        entity->addData(KeyName::UUID, uuid_id);
                        unique_entity_ids.push_back(id);
                    }

                    if(!entity_hash.contains(id)){
                        //Insert into the hash.
                        entity_hash.insert(id, entity);
                    }else{
                        qCritical() << "ImportGraphML: Duplicate Entity Ids found '" << id << "'";
                        error_count ++;
                        continue;
                    }

                    if(kind == GRAPHML_KIND::NODE){
                        node_ids.push(id);
                    }else if(kind == GRAPHML_KIND::EDGE){
                        //Handle Source/Target for edges.
                        entity->setSourceIDStr(getXMLAttribute(xml, "source"));
                        entity->setTargetIDStr(getXMLAttribute(xml, "target"));

                        edge_ids.push(id);
                    }
                    //Set the Item as the current Entity.
                    current_entity = entity;
                }
                default:
                    break;
            }
        }

        if(xml.isEndElement()){
            //For Nodes/Edges, step up a parent.
            if(kind == GRAPHML_KIND::NODE || kind == GRAPHML_KIND::EDGE){
                current_entity = current_entity->getParent();
            }
        }
    }

    
    auto parsing_finished = QDateTime::currentDateTime().toMSecsSinceEpoch();
    //qCritical() << "Parsing took: " <<  parsing_finished - start << "MS";

    //Gain the write lock
    QWriteLocker lock(&lock_);

    QList<Entity*> to_remove;

    QSet<Node*> uuid_changed_entities;
    QSet<QString> uuids_changed;
    
    //Handle unique ids
    for(auto id : unique_entity_ids){
        auto entity = entity_hash.value(id, 0);
        if(!entity){
            qCritical() << "ImportGraphML: Cannot process UUID'd entity with id: '" << id << "'";
            continue;
        }

        bool handle_uuid = IS_IMPORT;
        auto kind = entity->getDataValue(KeyName::Kind).toString();

        if(!handle_uuid && entity->isNode()){
            auto parent_node_kinds_set = entity->getParentStack().toSet();
            const QSet<NODE_KIND> always_handle_kinds = {NODE_KIND::HARDWARE_DEFINITIONS, NODE_KIND::WORKER_DEFINITIONS};
            handle_uuid = parent_node_kinds_set.intersects(always_handle_kinds);
        }


        //Handle UUIDS
        if(handle_uuid){
            const auto& uuid = entity->getDataValue(KeyName::UUID).toString();
            //If we have a uuid, we should set the entity as read-only
            entity->addData(KeyName::ReadOnly, true);
            entity->setUUIDMatched(true);
            
            //Lookup the entity in the 
            auto matched_entity = entity_factory->GetEntityByUUID(uuid);
            if(matched_entity && matched_entity->isNode()){
                auto matched_node = (Node*) matched_entity;
                //Produce a notification for updating shared_datatypes
                
                if(matched_node->getNodeKind() == NODE_KIND::SHARED_DATATYPES || matched_node->getNodeKind() == NODE_KIND::CLASS){
                    auto version = entity->getDataValue(KeyName::Version).toString();
                    auto old_version = matched_entity->getDataValue(KeyName::Version).toString();
                    auto old_label = matched_entity->getDataValue(KeyName::Label).toString();
                    auto node_kind = matched_node->getDataValue(KeyName::Kind).toString();

                    if(!version.isEmpty() && !old_version.isEmpty()){
                        int version_compare = 0;
                        try {
                            const static QSet<NODE_KIND> valid_kinds{NODE_KIND::CLASS, NODE_KIND::SHARED_DATATYPES};
                            if(valid_kinds.contains(matched_node->getNodeKind())){
                                version_compare = VersionKey::CompareVersion(version, old_version);
                            }
                        } catch (const std::invalid_argument& ex){
                            qCritical() << "ImportGraphML: Invalid version number format found in node labelled: '" << old_label << "'";
                            error_count ++;
                            continue;
                        }

                        if(version_compare > 0){
                            QString title = "Loaded Model contains a newer " + node_kind + " named '" + old_label + "'";
                            QString description = "Updated from '" % old_version % "' to '" % version % "'. Please check usage.";
                            emit Notification(MODEL_SEVERITY::WARNING, title, description);
                        }else if(version_compare < 0){
                            MODEL_SEVERITY severity = MODEL_SEVERITY::WARNING;
                            QString title = "Loaded Model contains an older " + node_kind + " named '" + old_label + "'";
                            QString description = "Reverting current version '" % old_version % "' to '" % "'. Please check usage.";

                            if(matched_node->getNodeKind() == NODE_KIND::CLASS && matched_node->getViewAspect() == VIEW_ASPECT::WORKERS){
                                description += "Please reimport Worker Definitions to ensure runtime compatibility.";
                                severity = MODEL_SEVERITY::ERROR;
                            }
                            emit Notification(severity, title, description);
                        }
                    }
                }
                //Set the entity to use this.
                entity->setID(matched_entity->getID());

                if(matched_entity->isNode()){
                    auto matched_node = (Node*) matched_entity;

                    uuid_changed_entities.insert(matched_node);
                    uuids_changed.insert(uuid);

                    //Create a list of all required uuids this entity we are loading requires
                    QStringList required_uuids;
                    for(auto child : entity->getChildren()){
                        required_uuids << child->getDataValue(KeyName::UUID).toString();
                    }
                    
                    //Remove all visual data.
                    for(const auto& key_name : entity->getKeys()){
                        if(isKeyNameVisual(key_name)){
                            entity->removeData(key_name);
                        }
                    }

                    //Compare the children we already have in the Model to the children we need to import. Remove any which aren't needed
                    for(auto child : matched_node->getChildren(0)){
                        if(child->gotData(KeyName::UUID)){
                            auto child_uuid = child->getDataValue(KeyName::UUID).toString();
                            if(!required_uuids.contains(child_uuid)){
                                to_remove.push_back(child);
                            }
                        }
                    }
                }
            }
        }
    }

    //Get the ordered list of entities to remove
    to_remove = getOrderedEntities(to_remove);
    
    //unset any entities which 
    destructEntities(to_remove);



    lock.unlock();

    auto removing_finished = QDateTime::currentDateTime().toMSecsSinceEpoch();
    //qCritical() << "Removing entities took: " <<  removing_finished - parsing_finished << "MS";
    
    //This is will update as a percentage
    double entities_total_perc = entity_hash.size() / 100.0;
    double entities_made = 0;

    if(UPDATE_PROGRESS){
        ProgressUpdated_("Constructing Nodes");
    }
    QList<Node*> implicitly_constructed_nodes;

    //Reset any delete nodes
    for(auto id : node_ids){
        auto entity = entity_hash.value(id, 0);
        //Check for removed nodes.
        if(entity->isUUIDMatched() && entity->gotID()){
            //Get the already existant node.
            if(!entity_factory->GetNode(entity->getID())){
                entity->setID(-1);
                entity->setPreviousID(-1);
            }
        }
    }

    //Construct all nodes
    for(auto id : node_ids){
        auto entity = entity_hash.value(id, 0);

        if(!entity || !entity->isNode()){
            qCritical() << "ImportGraphML: Couldn't find Node with ID:" << id;
            error_count ++;
            continue;
        }

        //Get the parent
        auto parent_entity = entity->getParent();

        //Check if our parent_entity exists
        if(!parent_entity || !parent_entity->gotID()){
            qCritical() << "ImportGraphML: Node with ID:" << id << "has no Parent Entity";
            error_count ++;
            continue;
        }

        //This is the top most element
        if(IS_PASTE && parent_entity == current_entity){
            for(auto key_name : entity->getKeys()){
                if(isKeyNameVisual(key_name)){
                    entity->removeData(key_name);
                }
            }
        }
        auto parent_node = entity_factory->GetNode(parent_entity->getID());
        
        if(!parent_node){
            qCritical() << "ImportGraphML: Node with ID:" << id << " Has a parent entity: " << parent_entity->getID() << " which doesn't exist in the Entity Factory";
            error_count ++;
            continue;
        }
        
        auto parent_kind = parent_node->getNodeKind();
        auto kind = entity_factory->getNodeKind(entity->getKind());

        Node* node = 0;

        if(entity->gotID()){
            //Get the already existant node.
            node = entity_factory->GetNode(entity->getID());
        }
        
        if(!node){
            if(parent_entity->GotImplicitlyConstructedNodeID(kind)){
                //Get the ID of the next auto constructed ID
                auto id = parent_entity->TakeNextImplicitlyConstructedNodeID(kind);
                node = entity_factory->GetNode(id);
            }else{
                node = construct_node(parent_node, kind);
            }
        }

        if(node){
            //Check to see if the node constructed some other stuff, It shouldn 't have any children.
            for(auto child : node->getChildren()){
                if(child->isImplicitlyConstructed()){
                    entity->AddImplicitlyConstructedNodeID(child->getNodeKind(), child->getID());
                    if(!implicitly_constructed_nodes.contains(child)){
                        implicitly_constructed_nodes << child;
                    }
                }
            }

            for(auto edge : node->getAllEdges()){
                if(edge->isImplicitlyConstructed()){
                    entity->AddImplicitlyConstructedEdgeID(edge->getEdgeKind(), edge->getID());
                }
            }

            //If we have matched something which is already got a parent node we shouldn't overwrite its data
            //This will stop any data stored in the aspects getting overwritten
            if(node->getID() > 0 && !IS_OPEN){
                if(!entity->isUUIDMatched()){
                    entity->clearData();
                }
            }

            bool is_model = kind == NODE_KIND::MODEL;

            bool got_parent = node->getParentNode();
            bool requires_parenting = !got_parent && !is_model;
            bool implicitly_created = node->isImplicitlyConstructed();
            bool need_to_store = false;
            
            if(requires_parenting || implicitly_created){
                //If the node is not attached to a parent, or it was implicilty constructed, set the data ahead of time
                for(const auto& key_name : entity->getKeys()){
                    const auto& value = entity->getDataValue(key_name);
                    
                    //Don't overwrite implicitly set data with a blank value
                    if(implicitly_created && node->gotData(key_name) && value == ""){
                        continue;
                    }
                    //TODO: WORK OUT IF WE SHOULD ALLOW DATA WE DON'T IMPICILTLY CREATE
                    setData_(node, key_name, value, false);
                }
                //Remove the data as we have already attached it
                entity->clearData();
            }

            if(requires_parenting){
                got_parent = attachChildNode(parent_node, node, false);

                if(got_parent){
                    //Only progress when parent setting actually worked.
                    need_to_store = true;
                    addDependantsToDependants(parent_node, node);
                }
            }

            if((need_to_store || implicitly_created) && (got_parent || is_model)){
                auto add_action_state = !implicitly_created;
                storeNode(node, entity->getPreviousID(), false, add_action_state);
            }

            if(got_parent || is_model){
                for(auto key_name : entity->getKeys()){
                    auto value = entity->getDataValue(key_name);
                    setData_(node, key_name, value, true);
                }
                entity->setID(node->getID());
            }else{
                for(auto child : node->getChildren()){
                    if(child->isImplicitlyConstructed()){
                        implicitly_constructed_nodes.removeAll(child);
                    }
                }
                entity_factory->DestructEntity(node);
                node = 0;
            }
        }

        if(!node){
            QString title = "Cannot create Node '" + entity->getKind() + "'";
            if(!SuppressImportNodeError(*parent_entity, *entity, model_import_version)){
                QString description = "importGraphML(): Document line #" % QString::number(entity->getLineNumber());
                error_count ++;
                emit Notification(MODEL_SEVERITY::ERROR, title, description);
            }
        }

        if(UPDATE_PROGRESS){
            ProgressChanged_(++entities_made / entities_total_perc);
        }
    }

    while(implicitly_constructed_nodes.size()){
        auto node = implicitly_constructed_nodes.takeFirst();
        storeNode(node, -1, false, false);
    }

    auto constructing_node_finished = QDateTime::currentDateTime().toMSecsSinceEpoch();

    QMultiMap<EDGE_KIND, TempEntity*> edge_map;

    QHash<EDGE_KIND, QQueue<TempEntity*> > edge_map_2;

    QList<TempEntity*> edges_list;

    for(auto id : edge_ids){
        auto entity = entity_hash.value(id, 0);

        if(!entity || !entity->isEdge()){
            qCritical() << "ImportGraphML: Couldn't find Edge with ID:" << id;
            error_count ++;
            continue;
        }

        //Try find the Entity we should be creating.
        auto src_entity = entity_hash.value(entity->getSourceIDStr(), 0);
        auto dst_entity = entity_hash.value(entity->getTargetIDStr(), 0);

        Node* src_node = 0;
        Node* dst_node = 0;

        if(src_entity && src_entity->gotID()){
            src_node = entity_factory->GetNode(src_entity->getID());
        }else if(IS_PARTIAL_IMPORT){
            //Try and see if the integer version of the ID stored in the GraphML Still exists in the model.
            src_node = entity_factory->GetNode(entity->getSourceIDInt());
        }

        if(dst_entity && dst_entity->gotID()){
            dst_node = entity_factory->GetNode(dst_entity->getID());
        }else if(IS_PARTIAL_IMPORT){
            //Try and see if the integer version of the ID stored in the GraphML Still exists in the model.
            dst_node = entity_factory->GetNode(entity->getTargetIDInt());
        }

        if(src_node && dst_node){
            //Set destination.
            entity->setSourceID(src_node->getID());
            entity->setTargetID(dst_node->getID());

            EDGE_KIND edge_kind = entity_factory->getEdgeKind(entity->getKind());

            if(edge_kind != EDGE_KIND::NONE){
                entity->appendEdgeKind(edge_kind);
            }else{
                //If the edge class stored in the model is invalid we should try all of the edge classes these items can take, in order.
                entity->appendEdgeKinds(getPotentialEdgeClasses(src_node, dst_node));
            }

            if(entity->gotEdgeKind()){
                edges_list.append(entity);
            }else{
                if(!SuppressImportEdgeError(*entity, src_entity, dst_entity, model_import_version)){
                    QString title = "Cannot create edge";
                    QString description = "importGraphML(): Document line # " % QString::number(entity->getLineNumber()) % "\nNo valid edge kinds";
                    error_count ++;
                    emit Notification(MODEL_SEVERITY::ERROR, title, description);
                }
            }

        }else{
            if(!SuppressImportEdgeError(*entity, src_entity, dst_entity, model_import_version)){
                QString title = "Cannot create edge";
                QString description = "importGraphML(): Document line # " % QString::number(entity->getLineNumber()) % "\nMissing Source or destination";
                error_count ++;
                emit Notification(MODEL_SEVERITY::ERROR, title, description);
            }
        }
    }

    

    auto entity_fac = entity_factory;

    //Have to sort edges so the higher a Edge's Target is, the sooner that edge gets processed
    std::sort(edges_list.begin(), edges_list.end(), [entity_fac](const TempEntity* a, const TempEntity* b){
        auto is_a_edge = a->isEdge();
        auto is_b_edge = b->isEdge();

        if(is_a_edge && is_b_edge){
            auto a_node = entity_fac->GetNode(a->getTargetID());
            auto b_node = entity_fac->GetNode(b->getTargetID());
            
            //Proper depth
            auto a_depth = a_node->getDepth();
            auto b_depth = b_node->getDepth();
            return a_depth > b_depth;
        }else{
            throw std::runtime_error("Got a Node in an Edge List");
        }
    });

    for(auto edge : edges_list){
        if(edge->gotEdgeKind()){
            auto edge_kind = edge->getEdgeKind();
            //Insert the item in the lookup
            edge_map_2[edge_kind].enqueue(edge);
        }
    }

    auto constructing_edge_type_finished = QDateTime::currentDateTime().toMSecsSinceEpoch();


    if(UPDATE_PROGRESS){
        ProgressUpdated_("Constructing Edges");
    }

    int edge_iterations = 0;
    auto edge_kind_keys = GetEdgeOrderIndexes();

    while(true){
        int constructed_edges = 0;
        QQueue<TempEntity*> unconstructed_edges;
        
        auto current_edge_kind = EDGE_KIND::NONE;
        
        //Find the first index which still has edges left
        for(auto edge_kind : edge_kind_keys){
            if(edge_map_2[edge_kind].size()){
                current_edge_kind = edge_kind;
                break;
            }
        }
        
        if(current_edge_kind == EDGE_KIND::NONE){
            break;
        }

        auto& edges = edge_map_2[current_edge_kind];
        while(edges.size()){
            auto entity = edges.dequeue();

            //Get the src/dst node from the edge
            auto src_node = entity_factory->GetNode(entity->getSourceID());
            auto dst_node = entity_factory->GetNode(entity->getTargetID());

            Edge* edge = 0;
            if(src_node && dst_node){
                bool need_to_store = false;
                
                //Check if we have an edge first
                edge = src_node->getEdgeTo(dst_node, current_edge_kind);

                if(!edge){
                    //Try and construct Edge
                    edge = construct_edge(current_edge_kind, src_node, dst_node, false);
                    need_to_store = true;
                }

                if(edge){
                    if(need_to_store){
                        storeEdge(edge, entity->getPreviousID());
                    }

                    for(auto key_name : entity->getKeys()){
                        auto value = entity->getDataValue(key_name);
                        setData_(edge, key_name, value, true);
                    }

                    //Point the tempEntity to the newly constructed entity
                    entity->setID(edge->getID());
                }
            }else{
                QString title = "Cannot create edge";
                QString description = "importGraphML(): Document line # " % QString::number(entity->getLineNumber()) % "\nInternal Error";
                error_count ++;
                emit Notification(MODEL_SEVERITY::ERROR, title, description);
            }

            if(edge){
                if(UPDATE_PROGRESS){
                    ProgressChanged_(++entities_made / entities_total_perc);
                }
                constructed_edges ++;
            }else{
                //If we couldn't construct an edge, push it onto the list of edges we couldn't construct
                unconstructed_edges.enqueue(entity);
            }
        }

        //Go through the list of unconstructed edges.
        for(auto entity : unconstructed_edges){
            //If no edges were constructed this pass, it means no edges can be made of this EDGE_KIND
            if(constructed_edges == 0){
                //Remove the current edge kind, So try the next valid edge kind (If it has one)
                entity->removeEdgeKind(current_edge_kind);
            }

            if(entity->gotEdgeKind()){
                auto edge_kind = entity->getEdgeKind();
                //Enqueueing from a QStack should mean we flip the queue
                edge_map_2[edge_kind].enqueue(entity);
            }else{
                //Can't find source/destination
                QString message = "Cannot create edge from document at line #" + QString::number(entity->getLineNumber()) + ". No valid edge_kinds.";
                emit Notification(MODEL_SEVERITY::ERROR, message);
                error_count ++;
            }
        }
        edge_iterations ++;
    }

    auto constructing_edges_finished = QDateTime::currentDateTime().toMSecsSinceEpoch();
    //qCritical() << "Constructing edges took: " <<  constructing_edges_finished - constructing_edge_type_finished << "MS : ITTERATIONS: " << edge_iterations << " COUNT: " << edges_list.size();

    if(UPDATE_PROGRESS){
        ProgressChanged_(100);
    }
    
    QSet<Node*> nodes_to_remove;
    for(const auto& uuid : uuids_changed){
        auto matched_entity = entity_factory->GetEntityByUUID(uuid);

        if(matched_entity && matched_entity->isNode()){
            auto node = (Node*) matched_entity;
            for(auto instance : node->getDependants()){
                nodes_to_remove += UpdateDefinitions(node, instance);
            }
        }
    }

    QList<Entity*> nodes_to_remove_list;
    std::for_each(nodes_to_remove.begin(), nodes_to_remove.end(), [&nodes_to_remove_list](Node* node){
        nodes_to_remove_list.push_back(node);
    });
    destructEntities(nodes_to_remove_list);


    for(auto entity : entity_hash){
        delete entity;
    }
    delete current_entity;

    return error_count == 0;
}

const QSet<NODE_KIND>& GetValidInstanceKinds(const NODE_KIND& instance_kind){
    switch(instance_kind){
        case NODE_KIND::VECTOR_INST:{
            static const QSet<NODE_KIND> instance_kinds{NODE_KIND::ENUM_INST, NODE_KIND::AGGREGATE_INST, NODE_KIND::MEMBER_INST};
            return instance_kinds;
        }
        case NODE_KIND::AGGREGATE_INST:
        case NODE_KIND::INPUT_PARAMETER_GROUP_INST:
        case NODE_KIND::RETURN_PARAMETER_GROUP_INST:{
            static const QSet<NODE_KIND> instance_kinds{NODE_KIND::ENUM_INST, NODE_KIND::AGGREGATE_INST, NODE_KIND::MEMBER_INST, NODE_KIND::VECTOR_INST};
            return instance_kinds;
        }
        case NODE_KIND::CLASS_INST:{
            static const QSet<NODE_KIND> instance_kinds{NODE_KIND::FUNCTION, NODE_KIND::CALLBACK_FNC, NODE_KIND::ATTRIBUTE_INST};
            return instance_kinds;
        }
        case NODE_KIND::FUNCTION_CALL:
        case NODE_KIND::CALLBACK_FNC_INST:{
            static const QSet<NODE_KIND> instance_kinds{NODE_KIND::INPUT_PARAMETER_GROUP_INST, NODE_KIND::RETURN_PARAMETER_GROUP_INST};
            return instance_kinds;
        }
        case NODE_KIND::CALLBACK_FNC:
        case NODE_KIND::ATTRIBUTE_INST:
        case NODE_KIND::MEMBER_INST:
        case NODE_KIND::FUNCTION:{
            //None of these types can have children
            break;
        }
        default:{
            qCritical() << "Got Unexcepted Instance Kind: " << EntityFactory::getNodeKindString(instance_kind);
            break;
        }
    }
    static const QSet<NODE_KIND> empty;
    return empty;
}



QSet<Node*> ModelController::UpdateDefinitions(Node* definition, Node* instance){
    QSet<Node*> nodes_to_remove;
    QList<Node*> definition_nodes;

    //Check if the definition has a definition
    auto def_def = definition->getDefinition(true);
    auto def = def_def ? def_def : definition;

    
    //Get the children of the definition
    for(auto child : def->getChildren(0)){
        if(child->isDefinition()){
            //Check if the child itself has a definition
            auto c_def = child->getDefinition(true);
            auto def = c_def ? c_def : child;
            //Add this to the list of nodes 
            definition_nodes += def;
        }
    }

    const auto& instance_kind = instance->getNodeKind();
    const auto& instance_kinds = GetValidInstanceKinds(instance_kind);

    for(auto child : instance->getChildren(0)){
        auto node_kind = child->getNodeKind();
        if(child->isInstanceImpl() && instance_kinds.contains(instance_kind)){
            auto c_def = child->getDefinition(true);

            if(c_def){
                if(definition_nodes.contains(c_def)){
                    definition_nodes.removeOne(c_def);
                    continue;
                }else{
                    nodes_to_remove += child;
                }
            }
        }
    }

    for(auto dep : instance->getDependants()){
        nodes_to_remove += UpdateDefinitions(definition, dep);
    }

    return nodes_to_remove;
}


HistoryAction ModelController::getNewAction(GRAPHML_KIND kind)
{
    HistoryAction action;
    action.Action.id = currentActionID;
    action.Action.name = currentAction;
    action.Action.action_id = currentActionItemID++;
    action.Action.kind = kind;
    return action;
}

void ModelController::setProjectModified(bool dirty)
{
    if(project_modified != dirty){
        project_modified = dirty;
        emit ProjectModified(dirty);
    }
}

void ModelController::setProjectPath(const QString& path)
{
    if(project_path != path){
        project_path = path;
        emit ProjectFileChanged(project_path);
    }
}


QSet<SELECTION_PROPERTIES> ModelController::getSelectionProperties(int active_id, QList<int> ids){
    QReadLocker lock(&lock_);
    QSet<SELECTION_PROPERTIES> properties;

    auto item = entity_factory->GetEntity(active_id);
    
    auto unordered_items = getUnorderedEntities(ids);
    //Got a list of entites
    auto ordered_items = getOrderedEntities(unordered_items);

    if(canCut(ordered_items)){
        properties.insert(SELECTION_PROPERTIES::CAN_CUT);
    }

    if(canCopy(ordered_items)){
        properties.insert(SELECTION_PROPERTIES::CAN_COPY);
    }

    if(canRemove(ordered_items)){
        properties.insert(SELECTION_PROPERTIES::CAN_REMOVE);
    }

    if(canPaste(ordered_items)){
        properties.insert(SELECTION_PROPERTIES::CAN_PASTE);
    }

    if(canReplicate(ordered_items)){
        properties.insert(SELECTION_PROPERTIES::CAN_REPLICATE);
    }

    if(canChangeIndex(unordered_items)){
        properties.insert(SELECTION_PROPERTIES::CAN_CHANGE_INDEX);
    }

    if(canChangeRow(unordered_items)){
        properties.insert(SELECTION_PROPERTIES::CAN_CHANGE_ROW);
    }

    if(unordered_items.count() == 1){
        bool all_okay = true;
        QSet<NODE_TYPE> code_type = {NODE_TYPE::BEHAVIOUR_CONTAINER, NODE_TYPE::BEHAVIOUR_ELEMENT};
        QSet<NODE_KIND> code_kinds = {NODE_KIND::COMPONENT, NODE_KIND::COMPONENT_INST, NODE_KIND::COMPONENT_IMPL};
        for(auto item : unordered_items){
            if(item->isNode()){
                auto node = (Node*) item;
                auto node_kind = node->getNodeKind();
                auto node_types = node->getTypes();
                if(!(node_types.intersects(code_type) || code_kinds.contains(node_kind))){
                    all_okay = false;
                    break;
                }
            }
        }
        if(all_okay){
            properties.insert(SELECTION_PROPERTIES::CAN_GENERATE_CODE);
        }
    }

    

    //Check Active Selection
    if(item){
        auto label = item->getData(KeyName::Label);
        if(label && !item->isReadOnly() && !label->isProtected()){
            properties.insert(SELECTION_PROPERTIES::CAN_RENAME);
        }

        if(item->isNode()){
            auto node = (Node*) item;

            if(getDefinition(node)){
                properties.insert(SELECTION_PROPERTIES::GOT_DEFINITION);
            }

            if(gotImplementation(node)){
                properties.insert(SELECTION_PROPERTIES::GOT_IMPLEMENTATION);
            }

            if(gotInstances(node)){
                properties.insert(SELECTION_PROPERTIES::GOT_INSTANCES);
            }

            if(node->getEdgeCount() > 0){
                properties.insert(SELECTION_PROPERTIES::GOT_EDGES);
            }
        }
    }

    return properties;
}

bool ModelController::canCut(const QList<Entity *>& ordered_selection)
{
    if(canRemove(ordered_selection)){
        for(const auto& item : ordered_selection){
            if(item->isNode()){
                Node* node = (Node*) item;
               
                //Dont allow copy from any definitions, except 
                if(node->isDefinition()){
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}



bool ModelController::canCopy(const QList<Entity *>& ordered_selection)
{
    bool valid = !ordered_selection.isEmpty();
    Node* parent = 0;

    for(const auto& item : ordered_selection){
        if(item->isNode()){
            Node* node = (Node*) item;
            if(!canDeleteNode(node)){
                return false;
            }

            auto current_parent = node->getParentNode();

            //Dont allow copy from any definitions, except 
            if(node->isDefinition()){
                switch(node->getNodeKind()){
                    case NODE_KIND::ENUM:
                    case NODE_KIND::AGGREGATE:
                    case NODE_KIND::COMPONENT:
                        break;
                    default:
                        return false;
                }
            }


            //Don't allow selection from different parents to be copyable
            if(!parent){
                parent = current_parent;
            }else if(parent != current_parent){
                return false;
            }
        }
    }

    return valid;
}

bool ModelController::canPaste(const QList<Entity *>& ordered_selection)
{
    if(ordered_selection.size() == 1){
        Entity* item = ordered_selection.first();

        if(item && item->isNode()){
            auto node = (Node*) item;

            if(!node->isReadOnly() && !node->isNodeOfType(NODE_TYPE::BEHAVIOUR_CONTAINER)){
                if(node->isInstanceImpl() ){
                    return false;
                }

                if(node->isDefinition()){
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

bool ModelController::canChangeIndex(const QList<Entity *>& unordered_selection)
{
    bool valid = unordered_selection.count() == 1;

    if(valid){
        for(const auto& entity : unordered_selection){
            auto data = entity->getData(KeyName::Index);
            if(!data || data->isProtected()){
                valid = false;
                break;
            }
        }
    }
    return valid;
}

bool ModelController::canChangeRow(const QList<Entity *>& unordered_selection)
{
    bool valid = unordered_selection.count() == 1;

    if(valid){
        for(const auto& entity : unordered_selection){
            auto data = entity->getData(KeyName::Row);
            if(!data || data->isProtected()){
                valid = false;
                break;
            }
        }
    }
    return valid;
}

bool ModelController::canReplicate(const QList<Entity *>& ordered_selection)
{
    if(canCopy(ordered_selection)){
        //Get the first item, check if we can paste into it
        return canPaste({ordered_selection.first()});
    }
    return false;
}

bool ModelController::canRemove(const QList<Entity *>& ordered_selection)
{
    if(ordered_selection.isEmpty()){
        return false;
    }

    for(const auto& entity : ordered_selection){
        if(entity->isNode()){
            auto node = (Node*) entity;
            auto parent_node = node->getParentNode();

            if(!canDeleteNode(node)){
                return false;
            }
        }else if(entity->isEdge()){
            Edge* edge = (Edge*) entity;

            if(edge->isReadOnly()){
                return false;
            }
        }
    }
    return true;
}

bool ModelController::canUndo()
{
    QReadLocker lock(&lock_);
    return !undo_stack.isEmpty();
}

bool ModelController::canRedo()
{
    QReadLocker lock(&lock_);
    return !redo_stack.isEmpty();
}

QString ModelController::getProjectPath()
{
    QReadLocker lock(&lock_);
    return project_path;
}

bool ModelController::isProjectSaved()
{
    QReadLocker lock(&lock_);
    return project_modified == false;
}

int ModelController::getProjectActionCount()
{
    QReadLocker lock(&lock_);
    return currentActionItemID;
}

bool ModelController::gotImplementation(Node* node){
    auto definition = node;
    if(definition && definition->getDefinition()){
        definition = definition->getDefinition(true);
    }
    return definition ? definition->getImplementations().count(): false;
}

Node* ModelController::getDefinition(Node* node){
    if(node){
        return node->getDefinition(true);
    }
    return 0;
}

bool ModelController::gotInstances(Node* node){
    auto definition = node ? node->getDefinition(true) : 0;
    auto def = definition ? definition : node;
    return def ? def->getInstances().count() > 0 : false;
}


int ModelController::getDefinition(int ID)
{
    QReadLocker lock(&lock_);
    auto def = getDefinition(entity_factory->GetNode(ID));
    return def ? def->getID() : -1;
}

QList<int> ModelController::getInstances(int ID)
{
    QList<int> instanceIDs;
    auto node = entity_factory->GetNode(ID);
    auto definition = node->isDefinition() ? node : getDefinition(node);
    if (definition) {
        for (auto inst : definition->getInstances()) {
            instanceIDs.append(inst->getID());
        }
    }
    return instanceIDs;
}

int ModelController::getImplementation(int id)
{
    QReadLocker lock(&lock_);

    auto definition = entity_factory->GetNode(id);
    if(definition->getDefinition()){
        definition = definition->getDefinition(true);
    }
    if(definition){
        auto impls = definition->getImplementations();
        if(impls.size()){
            return (*impls.begin())->getID();
        }
    }
    return -1;
}

bool ModelController::isUserAction()
{
    if(isModelAction(MODEL_ACTION::UNDO) || isModelAction(MODEL_ACTION::REDO) || isModelAction(MODEL_ACTION::OPEN)){
        return false;
    }else{
        return true;
    }
}

void ModelController::setModelAction(MODEL_ACTION action){
    this->model_actions.insert(action);
}

void ModelController::unsetModelAction(MODEL_ACTION action){
    this->model_actions.remove(action);
}

bool ModelController::isModelAction(MODEL_ACTION action){
    return model_actions.contains(action);
}
