#include "outeventportdelegate.h"
#include "../../entityfactory.h"

auto node_kind = NODE_KIND::OUTEVENTPORT_DELEGATE;
QString kind_string = "OutEventPortDelegate";

void OutEventPortDelegate::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new OutEventPortDelegate(factory, is_temp_node);
    });
}

OutEventPortDelegate::OutEventPortDelegate(EntityFactory& factory, bool is_temp) : EventPortAssembly(factory, node_kind, is_temp){

}