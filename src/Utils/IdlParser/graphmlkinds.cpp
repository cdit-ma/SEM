#include "graphmlkinds.h"
#include "graphmlexporter.h"
#include <iostream>

using namespace Graphml;

//ENTITY CLASSES
Entity::Entity(Entity* parent, std::string label, Kind kind){
    parent_ = parent;
    kind_ = kind;
    label_ = label;

    if(parent){
        index_ = parent->add_child(this);
    }
}

Entity::~Entity(){
    
}


int Entity::get_index_definition_id(int index) const{
    //std::cerr << this << " GETTING: " << index << std::endl;
    if(id_lookup_.count(index)){
        return id_lookup_.at(index);
    }
    return -1;
}

void Entity::add_index_definition_id(int index, int definition_id){
    if(!id_lookup_.count(index)){
        id_lookup_[index] = definition_id;
    }else{
        std::cerr << " Index ID already used: " << index << " " << definition_id << " BY: " << get_index_definition_id(index) << std::endl;
    }
}

void Entity::set_id(int id){
    id_ = id;
}
int Entity::get_index() const{
    return index_;
}

int Entity::add_child(Entity* child){
    if(child){
        auto index = children_.size();
        children_.push_back(child);
        return index;
    }
    return -1;
}

void Entity::remove_child(Entity* child){
    if(child){
        children_.erase(std::remove(children_.begin(), children_.end(), child), children_.end());
    }
}

Entity* Entity::get_child(std::string label) const{
    for(auto child : children_){
        if(child && child->get_label() == label){
            return child;
        }
    }
    return 0;
}

Entity* Entity::get_parent() const{
    return parent_;
}

void Entity::set_label(std::string label){
    label_ = label;
}

int Entity::get_id() const{
    return id_;
}
std::vector<Entity*> Entity::get_children() const{
    return children_;
}

std::string Entity::get_label() const{
    return label_;
}

Kind Entity::get_kind() const{
    return kind_;
}

std::string Entity::get_namespace() const{
    std::string ns;
    auto parent = this;
    while(parent){
        auto next_parent = parent->get_parent();
        //Ignore my root lad
        if(next_parent){
            ns = parent->get_label() + (ns.length() == 0 ? "" : "::") + ns;
        }
        parent = next_parent;
    }
    return ns;
}

std::string Aggregate::get_namespace() const{
    
    std::string ns;
    auto p = get_parent();
    if(p){
        ns = p->get_namespace();
    }
    return ns;
}

//NAMESPACE FUNCTIONS
Namespace::Namespace(Namespace* parent, std::string label): Entity(parent, label, Kind::NAMESPACE){
}

//ENUM FUNCTIONS
Enum::Enum(Namespace* parent, std::string label, std::vector<std::string> enum_values): Entity(parent, label, Kind::ENUM){
    enum_values_ = enum_values;
}

std::vector<std::string> Enum::get_enum_values() const{
    return enum_values_;
}

Aggregate::Aggregate(Namespace* parent, std::string label): Entity(parent, label, Kind::AGGREGATE){

}

Member::Member(Aggregate* parent, std::string label, std::string primitive_type, bool is_sequence): Entity(parent, label, Kind::MEMBER){
    is_sequence_ = is_sequence;
    primitive_type_ = primitive_type;
}

Member::Member(Aggregate* parent, std::string label, Entity* complex_type, bool is_sequence): Entity(parent, label, Kind::MEMBER){
    is_sequence_ = is_sequence;
    complex_type_ = complex_type;
}

bool Member::is_complex() const{
    return complex_type_ != 0;
}

bool Member::is_sequence() const{
    return is_sequence_;
}

void Member::set_is_key(bool is_key){
    is_key_ = is_key;
}

bool Member::is_key() const{
    return is_key_;
}

Entity* Member::get_complex_type() const{
    return complex_type_;
}
std::string Member::get_primitive_type() const{
    return primitive_type_;
}


Model::Model(){
    {
        auto entity = new Namespace(0, "ROOT");
        insert_into_map(entity);
        root_ = entity;
    }
}

Model::~Model(){

}
std::string Model::ToGraphml(){
    auto exporter = new Graphml::Exporter(true);
    
    //Setup the Model/InterfaceDefinitions
    exporter->export_node();
    exporter->export_data("kind", "Model");
    exporter->export_graph();
    exporter->export_node();
    exporter->export_data("kind", "InterfaceDefinitions");
    exporter->export_graph();
    
    export_entity(exporter, root_);

    //Close the exporter
    exporter->close();
    return exporter->ToGraphml();
}

int Model::export_entity(Exporter* exporter, Entity* entity, Member* member_instance, Aggregate* parent_aggregate, int current_index){
    auto namespace_var = AsNamespace(entity);
    auto aggregate_var = AsAggregate(entity);
    auto member_var = AsMember(entity);
    auto enum_var = AsEnum(entity);
    if(namespace_var){
        for(auto child : namespace_var->get_children()){
            export_entity(exporter, child, member_instance, parent_aggregate, current_index);
        }
    }else if(aggregate_var){
        current_index = export_aggregate(exporter, aggregate_var, member_instance, parent_aggregate, current_index);
    }else if(member_var){
        current_index = export_member(exporter, member_var, member_instance, parent_aggregate, current_index);
    }else if(enum_var){
        current_index = export_enum(exporter, enum_var, member_instance, parent_aggregate, current_index);
    }
    return current_index;
}

int Model::export_aggregate(Exporter* exporter, Aggregate* aggregate, Member* member_instance, Aggregate* parent_aggregate, int current_index){

    //If we haven't got a top_struct_entity, it means this is a top level struct.
    if(!parent_aggregate){
        parent_aggregate = aggregate;
    }

    bool is_top_aggregate = parent_aggregate == aggregate;
    bool is_instance = member_instance;

    //Get the Graphml_id of the exported <node> representation
    auto graphml_id = exporter->export_node();

    exporter->export_data("kind", is_instance ? "AggregateInstance" : "Aggregate");
    //Either use the member_instance label, or the structs label
    exporter->export_data("label", is_instance ? member_instance->get_label() : aggregate->get_label());
    
    exporter->export_data("type", aggregate->get_namespace() + "::" + aggregate->get_label());

    auto index = is_instance ? member_instance->get_index() : aggregate->get_index();
    exporter->export_data("index", std::to_string(index));
    
    if(is_top_aggregate){
        //Add the namespace for top level structs
        exporter->export_data("namespace", aggregate->get_namespace());
    }

    //Export a graph to contain children
    exporter->export_graph();

    //Insert the graphml_id into the id_lookup in the top_struct
    parent_aggregate->add_index_definition_id(current_index, graphml_id);

    //Go through all IDL_ELEMENTs (a struct can only have children members)
    for(auto child : aggregate->get_children()){
        auto member = AsMember(child);
        if(member){
            auto definition = member->get_complex_type();

            //Keep track of the old index
            auto start_index = ++current_index;
            
            //Export the member, and get the new ID
            current_index = export_member(exporter, member, member_instance, parent_aggregate, current_index);

            if(is_top_aggregate && definition){
                //Itterate through all elements we added
                for(int i = 0; i + start_index <= current_index; i++){
                    //Get the source/target of the edge
                    auto source_id = parent_aggregate->get_index_definition_id(start_index + i);
                    auto target_id = definition->get_index_definition_id(i);
                    if(target_id == -1){
                        //Try us
                        target_id = definition->get_index_definition_id(0);
                    }
                    //Construct edge
                    exporter->export_edge(source_id, target_id);
                    exporter->export_data("kind", "Edge_Definition");
                    exporter->close_element();
                }
            }
        }
    }
    

    //Close graph
    exporter->close_element();
    //Close node
    exporter->close_element();
    
    return current_index;
}

int Model::export_enum(Exporter* exporter, Enum* enum_val, Member* member_instance, Aggregate* parent_aggregate, int current_index){
    auto is_instance = member_instance != 0;
    auto graphml_id = exporter->export_node();

    exporter->export_data("kind", is_instance ? "EnumInstance" : "Enum");
    exporter->export_data("label", is_instance ? member_instance->get_label() : enum_val->get_label());
    auto index = is_instance ? member_instance->get_index() : enum_val->get_index();
    exporter->export_data("index", std::to_string(index));

    if(parent_aggregate){
        //Insert the graphml_id into the id_lookup in the top_struct
        parent_aggregate->add_index_definition_id(current_index, graphml_id);
    }else{
        enum_val->add_index_definition_id(0, graphml_id);
    }

    if(!is_instance){
        exporter->export_graph();
        auto enum_values = enum_val->get_enum_values();

        for(auto i = 0; i < enum_values.size(); i++){
            exporter->export_node();
            exporter->export_data("kind", "EnumMember");
            exporter->export_data("label", enum_values[i]); 
            exporter->export_data("index", std::to_string(i));
            exporter->close_element();
        }
        exporter->close_element();
    }else{
        exporter->export_data("type", enum_val->get_label());
    }
    exporter->close_element();
    return current_index;
}

int Model::export_member(Exporter* exporter, Member* member, Member* member_instance, Aggregate* parent_aggregate, int current_index){
    auto is_instance = member_instance != 0;
    if(member->is_sequence()){
        auto graphml_id = exporter->export_node();
        

        exporter->export_data("kind", is_instance ? "VectorInstance" : "Vector");
        exporter->export_data("label", member->get_label());
        exporter->export_data("index", std::to_string(member->get_index()));

        if(member->is_complex()){
            auto complex_type = member->get_complex_type();
            exporter->export_data("type", "Vector::" + complex_type->get_label());
            exporter->export_graph();
            
            current_index = export_entity(exporter, complex_type, member, parent_aggregate, current_index);
        }else{
            exporter->export_data("type", "Vector::" + member->get_primitive_type());
            exporter->export_graph();
            //Primitive type
            //current_index ++;
            auto member_graphml_id = exporter->export_node();
            //Insert the item into the lookup
            parent_aggregate->add_index_definition_id(current_index, member_graphml_id);
            exporter->export_data("kind", is_instance ? "MemberInstance" : "Member");
            exporter->export_data("label", "Member");
            exporter->export_data("type", member->get_primitive_type());
            exporter->export_data("index", std::to_string(0));
            exporter->close_element();
        }

        //Close the graph
        exporter->close_element();
        //Close the node
        exporter->close_element();
    }else{
        if(member->is_complex()){
            auto complex_type = member->get_complex_type();
            current_index = export_entity(exporter, complex_type, member, parent_aggregate, current_index);
        }else{
            auto graphml_id = exporter->export_node();
            parent_aggregate->add_index_definition_id(current_index, graphml_id);
            exporter->export_data("kind", is_instance ? "MemberInstance" : "Member");
            exporter->export_data("label", member->get_label()); 
            exporter->export_data("type", member->get_primitive_type());
            exporter->export_data("index", std::to_string(member->get_index()));

            if(!is_instance && member->is_key()){
                exporter->export_data("key", "true");
            }
            exporter->close_element();
        }
    }
    return current_index;
}

Entity* Model::get_entity(int id){
    if(entities_.count(id)){
        return entities_[id];
    }
    return 0;
}

Namespace* Model::get_root() const{
    return root_;
}

//Constructors
Namespace* Model::construct_namespace(Namespace* parent, std::string label){
    if(parent){
        auto entity = new Namespace(parent, label);
        insert_into_map(entity);
        return entity;
    }
    return 0;
}

Enum* Model::construct_enum(Namespace* parent, std::string label, std::vector<std::string> values){
    if(parent){
        auto entity = new Enum(parent, label, values);
        insert_into_map(entity);
        return entity;
    }
    return 0;
}

Aggregate* Model::construct_aggregate(Namespace* parent, std::string label){
    if(parent){
        auto entity = new Aggregate(parent, label);
        insert_into_map(entity);
        aggregate_ids_.push_back(entity->get_id());
        return entity;
    }
    return 0;
}

Member* Model::construct_complex_member(Aggregate* parent, std::string label, Entity* complex_type, bool is_sequence){
    if(parent){
        auto entity = new Member(parent, label, complex_type, is_sequence);
        insert_into_map(entity);
        return entity;
    }
    return 0;
}

Member* Model::construct_primitive_member(Aggregate* parent, std::string label, std::string primitive_type, bool is_sequence){
    if(parent){
        auto entity = new Member(parent, label, primitive_type, is_sequence);
        insert_into_map(entity);
        return entity;
    }
    return 0;
}

Graphml::Entity* Model::get_namespaced_entity(std::list<std::string> namespace_tokens){
    Graphml::Entity* current = 0;

    if(namespace_tokens.size()){
        current = get_root();
        while(current && namespace_tokens.size()){
            //Take the front of the token, this should be the lowest named element
            auto front_label = namespace_tokens.front();
            namespace_tokens.pop_front();

            //If we have a child with this label, continue down the child stack
            auto child = current->get_child(front_label);
            //If we get null, we will still break
            current = child;
        }
    }
    return current;
}

Graphml::Entity* Model::get_ancestor_entity(Graphml::Entity* entity, std::string label){
    while(entity){
        auto child = entity->get_child(label);
        if(child){
            return child;
        }else{
            entity = entity->get_parent();
        }
    }
    return 0;
}

void Model::insert_into_map(Entity* entity){
    if(entity){
        entity->set_id(entities_.size());
        entities_[entity->get_id()] = entity;
    }
}


Namespace* Graphml::AsNamespace(Entity* entity){
    if(entity && entity->get_kind() == Kind::NAMESPACE){
        return (Namespace*) entity;
    }
    return 0;
}

Aggregate* Graphml::AsAggregate(Entity* entity){
    if(entity && entity->get_kind() == Kind::AGGREGATE){
        return (Aggregate*) entity;
    }
    return 0;
}

Member* Graphml::AsMember(Entity* entity){
    if(entity && entity->get_kind() == Kind::MEMBER){
        return (Member*) entity;
    }
    return 0;
}

Enum* Graphml::AsEnum(Entity* entity){
    if(entity && entity->get_kind() == Kind::ENUM){
        return (Enum*) entity;
    }
    return 0;
}