#include "namespace.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"


const NODE_KIND node_kind = NODE_KIND::NAMESPACE;
const QString kind_string = "Namespace";

void Namespace::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new Namespace(broker, is_temp_node);
    });
}

Namespace::Namespace(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::ColumnCount, QVariant::Int, ProtectedState::UNPROTECTED, 5);
    broker.AttachData(this, KeyName::Index, QVariant::Int, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::Namespace, QVariant::String, ProtectedState::PROTECTED);
}

void Namespace::BindNamespace(Node* parent, Node* child, bool bind){
    auto src_namespace_data = parent->getData(KeyName::Namespace);
    auto dst_namespace_data = child->getData(KeyName::Namespace);
    if (src_namespace_data && dst_namespace_data) {
        src_namespace_data->linkData(dst_namespace_data, bind);
    }
}

void Namespace::DataAdded(Data* data)
{
    Node::DataAdded(data);

    auto key_name = data->getKeyName();
    if (key_name == KeyName::Label || key_name == KeyName::Namespace) {
        auto label_data = getData(KeyName::Label);
        auto namespace_data = getData(KeyName::Namespace);
        if (label_data && namespace_data) {
            label_data->linkData(namespace_data, true);
        }
    }
}

void Namespace::childAdded(Node* child){
    Node::childAdded(child);
    Namespace::BindNamespace(this, child, true);
}

void Namespace::childRemoved(Node* child){
    Node::childRemoved(child);
    Namespace::BindNamespace(this, child, false);
}

void Namespace::parentSet(Node* parent){
    //Allow my parents types
    for(auto node_kind : parent->getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
    
    setAcceptsNodeKind(NODE_KIND::SHARED_DATATYPES, false);
    setAcceptsNodeKind(NODE_KIND::COMPONENT_IMPL, false);
    Node::parentSet(this);
}
