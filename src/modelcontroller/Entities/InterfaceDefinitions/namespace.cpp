#include "namespace.h"
#include "../../entityfactory.h"


const NODE_KIND node_kind = NODE_KIND::NAMESPACE;
const QString kind_string = "Namespace";

void Namespace::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new Namespace(factory, is_temp_node);
    });
}

Namespace::Namespace(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    addInstancesDefinitionKind(NODE_KIND::MEMBER);
    setChainableDefinition();

    //Setup Data
    factory.AttachData(this, "index", QVariant::Int, -1, false);
    factory.AttachData(this, "namespace", QVariant::String, "", true);
}

void Namespace::BindNamespace(Node* parent, Node* child, bool bind){
    auto src_namespace_data = parent->getData("namespace");
    auto dst_namespace_data = child->getData("namespace");
    if(src_namespace_data && dst_namespace_data){
        src_namespace_data->linkData(dst_namespace_data, bind);
    }
}

void Namespace::DataAdded(Data* data){
    Node::DataAdded(data);

    auto key_name = data->getKeyName();

    if(key_name == "label" || key_name == "namespace"){
        auto label_data = getData("label");
        auto namespace_data = getData("namespace");

        if(label_data && namespace_data){
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
    Node::parentSet(this);
}
