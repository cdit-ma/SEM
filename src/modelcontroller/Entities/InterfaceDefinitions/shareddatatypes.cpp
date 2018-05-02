#include "shareddatatypes.h"


SharedDatatypes::SharedDatatypes(EntityFactory* factory) : Node(factory, NODE_KIND::SHARED_DATATYPES, "SharedDatatypes"){
	auto node_kind = NODE_KIND::SHARED_DATATYPES;
	QString kind_string = "SharedDatatypes";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new SharedDatatypes();});

    RegisterDefaultData(factory, node_kind, "uuid", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "version", QVariant::String, false, "v1.0");
};

SharedDatatypes::SharedDatatypes(): Node(NODE_KIND::SHARED_DATATYPES)
{
    setAcceptsNodeKind(NODE_KIND::AGGREGATE);
    setAcceptsNodeKind(NODE_KIND::ENUM);
    setAcceptsNodeKind(NODE_KIND::NAMESPACE);
}