#include "graphmlkinds.h"

using namespace Graphml;

//ENTITY CLASSES
Entity::Entity(Entity* parent, std::string label, Kind kind){
    parent_ = parent;
    kind_ = kind;
    label_ = label;
}

Entity::~Entity(){
    //TODO
}

void Entity::set_id(int id){
    id_ = id;
}

void Entity::add_child(Entity* child){
    children_.insert(child);
}

void Entity::remove_child(Entity* child){
    children_.erase(child);
}
void Entity::set_label(std::string label){
    label_ = label;
}

int Entity::get_id() const{
    return id_;
}
std::set<Entity*> Entity::get_children() const{
    return children_;
}

std::string Entity::get_label() const{
    return label_;
}

Kind Entity::get_kind() const{
    return kind_;
}

//NAMESPACE FUNCTIONS
Namespace::Namespace(Namespace* parent, std::string label): Entity(parent, label, Kind::NAMESPACE){
}

//ENUM FUNCTIONS
Enum::Enum(Namespace* parent, std::string label, std::vector<std::string> enum_values): Entity(parent, label, Kind::NAMESPACE){
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
    return "";
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
        return entity;
    }
    return 0;
}

Member* Model::construct_complex_member(Aggregate* parent, std::string label, Entity* complex_type, bool is_sequence){\
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