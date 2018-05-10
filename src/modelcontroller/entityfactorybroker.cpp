#include "entityfactorybroker.h"
#include "entityfactory.h"
#include "entityfactoryregistrybroker.h"

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