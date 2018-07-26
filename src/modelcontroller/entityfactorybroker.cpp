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

Key* EntityFactoryBroker::GetKey(const QString& key_name, QVariant::Type type){
    return factory_.GetKey(key_name, type);
}


void EntityFactoryBroker::RemoveData(Entity* entity, const QString& key_name){
    auto data = entity->getData(key_name);
    factory_.DestructEntity(data);
}

Data* EntityFactoryBroker::AttachData(Entity* entity, Key* key, ProtectedState protected_state, QVariant value){
    return factory_.AttachData(entity, key, protected_state, value);
}

Data* EntityFactoryBroker::AttachData(Entity* entity, const QString& key_name, QVariant::Type type, ProtectedState protected_state, QVariant value){
    return AttachData(entity, GetKey(key_name, type), protected_state, value);
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


void EntityFactoryBroker::SetAcceptsEdgeKind(Node* node, EDGE_KIND edge_kind, EDGE_DIRECTION direction, bool accept){
    if(node){
        node->setAcceptsEdgeKind(edge_kind, direction, accept);
    }
}