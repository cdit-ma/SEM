#include "ineventportdelegate.h"
#include "../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::INEVENTPORT_DELEGATE;
const static QString kind_string = "InEventPortDelegate";

void InEventPortDelegate::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new InEventPortDelegate(factory, is_temp_node);
    });
}

InEventPortDelegate::InEventPortDelegate(EntityFactory& factory, bool is_temp) : EventPortAssembly(factory, node_kind, is_temp){

}