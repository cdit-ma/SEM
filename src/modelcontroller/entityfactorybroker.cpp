#include "entityfactorybroker.h"
#include "entityfactory.h"
#include "entityfactoryregistrybroker.h"
#include "Entities/node.h"

EntityFactoryBroker::EntityFactoryBroker(EntityFactory& factory) : factory_(factory){

}

EntityFactory& EntityFactoryBroker::GetEntityFactory(){
    return factory_;
}

void EntityFactoryBroker::DeregisterGraphML(GraphML* graphml){
    factory_.DeregisterGraphML(graphml);
}

void EntityFactoryBroker::DeregisterNode(Node* node){
    factory_.DeregisterNode(node);
}

void EntityFactoryBroker::DeregisterEdge(Edge* edge){
    factory_.DeregisterEdge(edge);
}

Node* EntityFactoryBroker::ConstructChildNode(Node& parent, NODE_KIND node_kind){
    return factory_.ConstructChildNode(parent, node_kind);
}

Key* EntityFactoryBroker::GetKey(QString key_name, QVariant::Type type){
    return factory_.GetKey(key_name, type);
}

Data* EntityFactoryBroker::AttachData(Entity* entity, Key* key, QVariant value, bool is_protected){
    return factory_.AttachData(entity, key, value, is_protected);
}

void EntityFactoryBroker::RemoveData(Entity* entity, QString key_name){
    auto data = entity->getData(key_name);
    factory_.DestructEntity(data);
}

Data* EntityFactoryBroker::AttachData(Entity* entity, QString key_name, QVariant::Type type, QVariant value, bool is_protected){
    return AttachData(entity, GetKey(key_name, type), value, is_protected);
}

QString EntityFactoryBroker::GetEdgeKindString(EDGE_KIND edge_kind){
    return factory_.getEdgeKindString(edge_kind);
}

QString EntityFactoryBroker::GetNodeKindString(NODE_KIND node_kind){
    return factory_.getNodeKindString(node_kind);
}

void EntityFactoryBroker::AcceptedEdgeKindsChanged(Node* node){
    factory_.AcceptedEdgeKindsChanged(node);
}

void EntityFactoryBroker::ProtectData(Entity* entity, QString key_name, bool is_protected){
    auto data = entity->getData(key_name);
    if(data){
        data->setProtected(is_protected);
    }
}

void EntityFactoryBroker::SetAcceptsEdgeKind(Node* node, EDGE_KIND edge_kind, EDGE_DIRECTION direction, bool accept){
    if(node){
        node->setAcceptsEdgeKind(edge_kind, direction, accept);
    }
}