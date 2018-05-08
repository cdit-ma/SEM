#include "shareddatatypes.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::SHARED_DATATYPES;
const QString kind_string = "SharedDatatypes";

void SharedDatatypes::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new SharedDatatypes(factory, is_temp_node);
    });
}

SharedDatatypes::SharedDatatypes(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setAcceptsNodeKind(NODE_KIND::AGGREGATE);
    setAcceptsNodeKind(NODE_KIND::ENUM);
    setAcceptsNodeKind(NODE_KIND::NAMESPACE);

    //Setup Data
    factory.AttachData(this, "uuid", QVariant::String, "", true);
    factory.AttachData(this, "version", QVariant::String, "v1.0", false);
}