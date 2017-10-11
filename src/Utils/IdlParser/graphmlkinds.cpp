#include "graphmlkinds.h"
#include "graphmlexporter.h"

using namespace Graphml;

//ENTITY CLASSES
Entity::Entity(Entity* parent, std::string label, Kind kind){
    parent_ = parent;
    kind_ = kind;
    label_ = label;

    if(parent){
        parent->add_child(this);
    }
}

Entity::~Entity(){
    
}

void Entity::set_id(int id){
    id_ = id;
}

void Entity::add_child(Entity* child){
    if(child){
        children_.insert(child);
    }
}

void Entity::remove_child(Entity* child){
    if(child){
        children_.erase(child);
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
std::set<Entity*> Entity::get_children() const{
    return children_;
}

std::string Entity::get_label() const{
    return label_;
}

Kind Entity::get_kind() const{
    return kind_;
}

std::string Entity::get_namespace() const{
    if(!parent_){
        return "";
    }else{
        auto parent_namespace = parent_->get_namespace();
        auto ns = parent_namespace;
        ns += parent_namespace == "" ? "" : "::";
        ns += get_label();
        return ns;
    }
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
    auto exporter = new Graphml::Exporter(true);
    
    //Setup the Model/InterfaceDefinitions
    exporter->export_node();
    exporter->export_data("kind", "Model");
    exporter->export_graph();
    exporter->export_node();
    exporter->export_data("kind", "InterfaceDefinitions");
    exporter->export_graph();

    for(auto aggregate_id : aggregate_ids_){
        auto aggregate = AsAggregate(get_entity(aggregate_id));
        if(aggregate){
            export_aggregate(exporter, aggregate);
        }
    }

    //Close the exporter
    exporter->close();


    return exporter->ToGraphml();
}

int Model::export_aggregate(Exporter* exporter, Aggregate* aggregate, Member* member_instance, Aggregate* top_aggregate, int current_index){

    //If we haven't got a top_struct_entity, it means this is a top level struct.
    if(!top_aggregate){
        top_aggregate = aggregate;
    }

    bool is_top_aggregate = top_aggregate == aggregate;
    bool is_instance = member_instance;

    //Get the Graphml_id of the exported <node> representation
    auto graphml_id = exporter->export_node();

    exporter->export_data("kind", is_instance ? "AggregateInstance" : "Aggregate");
    //Either use the member_instance label, or the structs label
    exporter->export_data("label", is_instance ? member_instance->get_label() : aggregate->get_label());
    
    if(is_top_aggregate){
        //Add the namespace for top level structs
        exporter->export_data("namespace", aggregate->get_namespace());
    }

    if(is_instance){
        //Attach the index, if we are an index
        //exporter->export_data("index", std::to_string(member_inst->index));
    }

    //Export a graph to contain children
    exporter->export_graph();

    //Insert the graphml_id into the id_lookup in the top_struct
    //top_struct_entity->id_lookup[current_index] = graphml_id;


    //Go through all IDL_ELEMENTs (a struct can only have children members)
    for(auto child : aggregate->get_children()){
        auto member = AsMember(child);
        if(member){
            auto definition = member->get_complex_type();

            //Keep track of the old index
            auto start_index = ++current_index;
            
            //Export the member, and get the new ID
            current_index = export_member(exporter, member, member_instance, top_aggregate, current_index);
        }
    }
            /*
            //If we have a definition to link against, we should construct edges
            if(definition){
                //Itterate through all IDL_ELEMENTs we added
                for(int i = 0; i + start_index <= current_index; i++){
                    //Get the source/target of the edge
                    auto source_id = top_struct_entity->id_lookup[start_index + i];
                    auto target_id = definition->id_lookup[i];
                    //Construct edge
                    exporter_->export_edge(source_id, target_id);
                    exporter_->export_data("kind", "Edge_Definition");
                    exporter_->close_element();
                }
            }*/
        
    

    //Close graph
    exporter->close_element();
    //Close node
    exporter->close_element();
    
    return current_index;
}

int Model::export_member(Exporter* exporter,Member* aggregate, Member* member_instance, Aggregate* parent_aggregate, int current_index){

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