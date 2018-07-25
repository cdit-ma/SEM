
#include "orbsvcs/CosNamingC.h"

void PrintContext(CosNaming::NamingContext_ptr naming_context, std::string parent_name){
    CosNaming::BindingList_var binding_list;
    CosNaming::BindingIterator_var binding_itterator;

    naming_context->list(1000, binding_list.out(), binding_itterator.out());

    for(int i = 0; i < binding_list->length(); i++){
        auto binding_type = binding_list[i].binding_type;
        auto binding_element = binding_list[i].binding_name;
        for(int j = 0; j < binding_element.length(); j++){
            std::cout << parent_name << "." << binding_element[j].id << std::endl;
        }
        if(binding_type == 1){
            auto name = std::string(binding_element[0].id);
            auto obj = naming_context->resolve(binding_element);
            auto new_context = CosNaming::NamingContext::_narrow(obj);
            PrintContext(new_context, parent_name + "." + name);
        }
    }
}

//tao_nslist -ORBInitRef NameService=corbaloc:iiop:>/NameService
int main(int){
    auto& helper;

    auto orb = helper.get_orb("iiop://localhost:5000", false);
    helper.register_initial_reference(orb, "NameService", "corbaloc:iiop:NAMESERVER:PORT/NameService");

    auto obj = helper.resolve_initial_references(orb, "NameService");
    auto naming_context = CosNaming::NamingContext::_narrow(obj);
    PrintContext(naming_context, "");

    CosNaming::Name name(1);
    name.length(1);
    name[0].id = "???";

    auto result = naming_context->resolve(name);
}