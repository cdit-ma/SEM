#include "idlparser.h"
#include "graphmlexporter.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <stack>
#include <map>
#include <set>
#include <list>
#include <ctype.h>
//Define the regex functions

#define NO_SYNTAX_GROUP "[^\\{\\}\\;]"
const std::regex re_include("#include\\s+[<|\"](\\S+)[>|\"]");
const std::regex re_module("module\\s+(\\S+)\\s*\\{");
const std::regex re_struct("struct\\s+(\\S+)\\s*\\{");
const std::regex re_block_comment("/\\*((.|\n)*?)\\*/");
const std::regex re_comment("//(.*)");
 
const std::regex re_split_path("(.*[/\\\\])*(.*)");
//const std::regex re_member("\\s*(" NO_SYNTAX_GROUP "+)\\s*;(\\s*//@key)?");
const std::regex re_member("(.*?)\\s*;(\\s*//@key)?");

const std::regex re_is_pragma_key("#pragma\\s+keylist\\s+(\\S+)\\s+(\\S+)");
const std::regex re_end_bracket("\\}\\s*;*");
const std::regex re_complex_member("(\\S*::)?(\\S+)");

const std::regex re_sequence_member("sequence\\s*\\<(.*)>");
//const std::regex re_sequence_member("sequence\\s*\\<\\s*([^\\s\\,]+)\\s*(?:\\,\\s*(\\S+))?\\s*\\>");


bool compare_char(char a, char b){
    return std::tolower(a, std::locale()) == std::tolower(b, std::locale());
};

bool caseless_compare(std::string const& a, std::string const& b)
{
    if(a.length() == b.length()){
        return std::equal(b.begin(), b.end(), a.begin(), compare_char);
    }
    return false;
};

//Gets the primitive RE type from the IDL type
std::string get_primitive_type(const std::string type){
    std::string return_type;
    if(type == "short"){
        return_type = "Integer";
    }else if(type == "long"){
        return_type = "Integer";
    }else if(type == "unsigned short"){
        return_type = "Integer";
    }else if(type == "unsigned long"){
        return_type = "Integer";
    }else if(type == "float"){
        return_type = "Float";
    }else if(type == "double"){
        return_type = "Double";
    }else if(type == "char"){
        return_type = "Character";
    }else if(type == "boolean"){
        return_type = "Boolean";
    }else if(type == "octet"){
        return_type = "Character";
    }else if(type == "wchar"){
        return_type = "Character";
    }else if(type == "string"){
        return_type = "String";
    }else if(type == "wstring"){
        return_type = "String";
    }

    if(!caseless_compare(type, return_type)){
        //std::cerr << "Warning: Type '" << type << "' converted to '" << return_type << "'" << std::endl;
    }
    return return_type;
};

std::vector<std::string> split(const std::string& input, const std::string& regex) {
    // passing -1 as the submatch index parameter performs splitting
    std::regex re(regex);
    std::sregex_token_iterator first{input.begin(), input.end(), re, -1};
    std::sregex_token_iterator last;
    return {first, last};
};

IdlParser::Entity* IdlParser::get_parent(Entity* entity){
    if(entities_.count(entity->parent_id)){
        return entities_[entity->parent_id];
    }
    return 0;
};

std::set<IdlParser::Entity*> IdlParser::get_entities(ELEMENT kind, std::string entity_name){
    std::set<Entity*> entities;

    for(auto e : entities_){
        //Look for members
        auto entity = e.second;

        if(entity->label == entity_name && entity->getKind() == kind){
            entities.insert(entity);
        }
    }
    return entities;
};

IdlParser::Entity* IdlParser::get_struct(std::string struct_label, std::string module_label){
    //Split the module label into a list of strings, so we can get the parent
    auto labels = split(module_label, "::");
    if(module_label.empty()){
        //If it's empty, clear the list, otherwise it's length 1
        labels.clear();
    }

    //Get all the structs labelled struct_label
    for(auto entity : get_entities(ELEMENT::STRUCT, struct_label)){
        //Get the parent
        auto parent = get_parent(entity);
        //If we don't have any modules prefixing, give back the first struct.
        bool got_match = true;
        
        //Itterate backwards through the labels
        for(int i = labels.size() - 1; i >= 0; i--){
            //If we have a parent and our labels match, continue
            if(parent && parent->label == labels[i]){
                parent = get_parent(parent);
            }else{
                got_match = false;
                break;
            }
        }

        if(got_match){
            return entity;
        }
    }

    return 0;
}

inline RegexMatch* re_search(std::string &str, ELEMENT kind, const std::regex re){
    auto me = new RegexMatch();
    me->got_match = std::regex_search(str, me->match, re);
    me->kind = kind;
    if(!me->got_match){
        delete me;
        me = 0;
    }
    return me;
};

std::string get_file_path(const std::string file_path){
    std::smatch match;
    if(std::regex_search(file_path, match, re_split_path)){
        return match[1];
    }
    return "";
}

std::string IdlParser::ParseIdl(std::string idl_path, bool pretty){
    auto parser = IdlParser(idl_path, pretty);
    return parser.ToGraphml();
};

IdlParser::Entity* IdlParser::get_entity(Entity* parent, std::string child_label){
    if(parent){
        for(auto c_id : parent->children_ids){
            auto entity = entities_[c_id];
            if(entity->label == child_label){
                return entity;
            }
        }
    }else{
        //Maybe a top level entity
        for(auto c_id : struct_ids_){
            auto entity = entities_[c_id];
            if(entity->label == child_label){
                return entity;
            }
        }
    }
    return 0;
};


//Make GraphML Structures
IdlParser::Member* IdlParser::construct_member(RegexMatch* m){
    auto entity = new Member();
    //Fill in values
    entity->id = entities_.size();
    entity->is_key = m->match[2].length();

    std::string member_data = m->match[1];


    /*
    std::cerr << "0|" << m->match[0] << "|0" << std::endl;
    std::cerr << "1|" <<  m->match[1] << "|1" << std::endl;
    std::cerr << "2|" <<  m->match[2] << "|2" << std::endl<< std::endl;
*/
    std::cerr << "MEMBER DATA: [" << member_data << "]" << std::endl;
    //Tokenize on chunks of whitespace
    auto tokens = split(member_data, "\\s+");
    if(tokens.size() > 1){
        //The label will always be the last element
        entity->label = tokens.back();
        //Remove the label from the tokens list
        tokens.pop_back();

        std::string type;
        //Construct a new single space seperated type
        for(auto s : tokens){
            if(s != ""){
                type += s + " ";
            }
        }
        //Remove the last space lel
        type.pop_back();
        entity->type = type;
    }

    std::cerr << "Member: " << entity->label << " [" << entity->type << "]" << std::endl; 

    std::smatch match;
    //Search for sequence bruush
    if(std::regex_search(entity->type, match, re_sequence_member)){
        entity->is_sequence = true; 

        std::string sequence_type = match[1];

        //Get rid of all space
        sequence_type.erase(std::remove(sequence_type.begin(), sequence_type.end(), ' '), sequence_type.end());

        //Tokenize by comma
        auto tokens = split(sequence_type, "\\,");
        if(tokens.size() > 0){
            entity->sequence_type = tokens[0];
            if(tokens.size() > 1){
                std::cerr << "Warning: Sequence Size '" << tokens[1] << "' ignored!" << std::endl;
            }
        }

        auto primitive_type = get_primitive_type(entity->sequence_type);
        if(primitive_type == ""){
            std::string modules_label = "";
            std::string struct_label = "";
            //Check if it's a complex sequence
            if(std::regex_search(entity->sequence_type, match, re_complex_member)){
                modules_label = match[1];
                struct_label = match[2];
            }

            if(modules_label == "::"){
                //Look in global scope
                modules_label = "";
            }

            auto struct_entity = get_struct(struct_label, modules_label);
            if(struct_entity){
                entity->sequence_type_id = struct_entity->id;
                entity->sequence_type = struct_label;
            }else{
                std::cerr << "IdlParser: Can't find Struct (For Sequence): '" << struct_label << "' in Module: '" << modules_label << "'" << std::endl;
            }
        }else{
            entity->sequence_type = primitive_type;
        }
    }else{
        //Check for primitive types, otherwise probably a complex type
        auto primitive_type = get_primitive_type(entity->type);
        if(primitive_type == ""){
            std::string modules_label = "";
            std::string struct_label = "";
            
            std::cerr << "ENTTITY TYPE: "<< entity->type << std::endl;
            //Try get module
            if(std::regex_search(entity->type, match, re_complex_member)){
                modules_label = match[1];
                struct_label = match[2];
            }

            if(modules_label == "::"){
                //Look in global scope
                modules_label = "";
            }

            auto struct_entity = get_struct(struct_label, modules_label);
            if(struct_entity){
                entity->definition_id = struct_entity->id;
                std::cerr << "DEFINITION ID: " << struct_entity->id << std::endl;
            }else{
                std::cerr << "IdlParser: Can't find Struct: '" << struct_label << "' in Module: '" << modules_label << "'" << std::endl;
            }
        }else{
            entity->type = primitive_type;
        }
    }    
    
    return entity;
};

IdlParser::Entity* IdlParser::construct_entity(RegexMatch* match){
    auto entity = new Entity(match->kind);
    entity->id = entities_.size();
    entity->label = match->match[1];

    if(entity->getKind() == ELEMENT::STRUCT){
        std::cerr << "Struct: " << entity->label << std::endl;
        struct_ids_.insert(entity->id);
    }else{
        std::cerr << "Module: " << entity->label << std::endl;
    }
    return entity;
};



int IdlParser::process_member(Member* member, Member* member_inst, Entity* top_struct_entity, int current_index){
    if(member->definition_id >= 0){
        //If we have a member, which has a definition, we should call into the process_struct function, with member as the instance
        auto definition = entities_[member->definition_id];
        current_index = process_struct(definition, member, top_struct_entity, current_index);
    }else{
        auto graphml_id = exporter_->export_node();
        //Insert the item into the lookup
        top_struct_entity->id_lookup[current_index] = graphml_id;

        bool is_instance = member_inst;

        if(member->is_sequence){
            exporter_->export_data("kind", is_instance ? "VectorInstance" : "Vector");
            exporter_->export_data("label", member->label);      
            exporter_->export_data("type", "Vector::" + member->sequence_type);
            exporter_->export_data("index", std::to_string(member->index));
            exporter_->export_graph();

            if(member->sequence_type_id == -1){
                //Increment the index
                current_index ++;
                auto member_graphml_id = exporter_->export_node();
                //Insert the item into the lookup
                top_struct_entity->id_lookup[current_index] = member_graphml_id;
                exporter_->export_data("kind", is_instance ? "MemberInstance" : "Member");
                exporter_->export_data("label", member->label); 
                exporter_->export_data("type", member->sequence_type);
                exporter_->export_data("index", std::to_string(member->index));
                exporter_->close_element();
            }else{
                auto definition = entities_[member->sequence_type_id];
                current_index = process_struct(definition, member, top_struct_entity, current_index);
            }
            //Close the graph
            exporter_->close_element();
        }else{
            exporter_->export_data("kind", is_instance ? "MemberInstance" : "Member");
            exporter_->export_data("label", member->label); 
            exporter_->export_data("type", member->type);
            exporter_->export_data("index", std::to_string(member->index));
            if(!is_instance && member->is_key){
                exporter_->export_data("key", "true");
            }
        }
        //Close the node
        exporter_->close_element();
    }
    return current_index;
}
std::string IdlParser::get_namespace(Entity* entity){

    auto parent = get_parent(entity);
    std::string name_str;
    while(parent){
        name_str = parent->label + name_str;
        parent = get_parent(parent);
        if(parent){
            name_str = "::" + name_str;
        }
    }
    return name_str;
};

int IdlParser::process_struct(Entity* struct_entity, Member* member_inst, Entity* top_struct_entity, int current_index){
    //If we haven't got a top_struct_entity, it means this is a top level struct.
    if(!top_struct_entity){
        //Set our top_struct_entity
        top_struct_entity = struct_entity;
    }

    bool is_top_level = top_struct_entity == struct_entity;
    bool is_instance = member_inst != 0;

    //Get the Graphml_id of the exported <node> representation
    auto graphml_id = exporter_->export_node();
    exporter_->export_data("kind", is_instance ? "AggregateInstance" : "Aggregate");
    //Either use the member_instance label, or the structs label
    exporter_->export_data("label", is_instance ? member_inst->label : struct_entity->label);
    if(is_top_level){
        //Add the namespace for top level structs
        exporter_->export_data("namespace", get_namespace(struct_entity));
    }
    if(is_instance){
        //Attach the index, if we are an index
        exporter_->export_data("index", std::to_string(member_inst->index));
    }

    //Export a graph to contain children
    exporter_->export_graph();

    //Insert the graphml_id into the id_lookup in the top_struct
    top_struct_entity->id_lookup[current_index] = graphml_id;


    //Go through all elements (a struct can only have children members)
    for(auto c_id : struct_entity->children_ids){
        auto member = (Member*) entities_[c_id];
        Entity* definition = 0;

        //Only do edge construction if we are parsing the top most element
        if(is_top_level){
            if(member->definition_id > 0){
                definition = entities_[member->definition_id];
            }else if(member->sequence_type_id > 0){
                definition = entities_[member->sequence_type_id];
            }
        }

        //Keep track of the old index
        auto start_index = ++current_index;
        //Export the member, and get the new ID
        current_index = process_member(member, member_inst, top_struct_entity, current_index);

        //If we have a definition to link against, we should construct edges
        if(definition){
            //Itterate through all elements we added
            for(int i = 0; i + start_index <= current_index; i++){
                //Get the source/target of the edge
                auto source_id = top_struct_entity->id_lookup[start_index + i];
                auto target_id = definition->id_lookup[i];
                //Construct edge
                exporter_->export_edge(source_id, target_id);
                exporter_->export_data("kind", "Edge_Definition");
                exporter_->close_element();
            }
        }
    }
    //Close graph
    exporter_->close_element();
    //Close node
    exporter_->close_element();
    return current_index;
}


IdlParser::IdlParser(std::string idl_path, bool pretty){
    path = get_file_path(idl_path);
    exporter_ = new GraphmlExporter(pretty);
    //Setup the Model/InterfaceDefinitions
    exporter_->export_node();
    exporter_->export_data("kind", "Model");
    exporter_->export_graph();
    exporter_->export_node();
    exporter_->export_data("kind", "InterfaceDefinitions");
    exporter_->export_graph();

    //Parse the files 
    parse_file(idl_path);
    
    //Export each struct
    for(auto s_id : struct_ids_){
        auto entity = entities_[s_id];
        process_struct(entity);
    }

    //Close the exporter
    exporter_->close();
};

void IdlParser::parse_file(std::string idl_path){
    if(parsed_files_.count(idl_path)){
        return;
    }
    
    std::cerr << "Parsing: " << idl_path << std::endl;
    parsed_files_.insert(idl_path);

    //Read the file
    std::ifstream input(idl_path);
    std::stringstream buffer;
    buffer << input.rdbuf();
    input.close();
    auto file = buffer.str();

    //Preprocess parse the file to find the other files to import
    {
        auto search_str = file;
        std::smatch import_match;
        //Keep parsing through the file
        while(std::regex_search(search_str, import_match, re_include)){
            //Get the matching group, and parse that file first
            auto include_idl_path = path + std::string(import_match[1]);

            //Append on the relative path to files
            parse_file(include_idl_path);
            //Get the tail of the document
            search_str = import_match.suffix();
        }
    }

    std::stack<Entity*> parent_entity;

    auto search_str = file;
    while(true){
        //Ordered map, to work out what is first element to parse
        std::map<int, RegexMatch*> ordered_matches;

        //Try and match the regex elements with the search_str (Returns 0 on no match)
        auto module_match = re_search(search_str, ELEMENT::MODULE, re_module);
        auto struct_match = re_search(search_str, ELEMENT::STRUCT, re_struct);
        auto member_match = re_search(search_str, ELEMENT::MEMBER, re_member);
        auto end_match = re_search(search_str, ELEMENT::END_BRACKET, re_end_bracket);
        auto key_match = re_search(search_str, ELEMENT::IS_KEY, re_is_pragma_key);
        auto comment_match = re_search(search_str, ELEMENT::COMMENT, re_comment);
        auto block_comment_match = re_search(search_str, ELEMENT::BLOCK_COMMENT, re_block_comment);
        
        //Place all successful matches into map (Ordered by their position in the string)
        if(module_match){
            ordered_matches[module_match->match.position()] = module_match;
        }

        if(struct_match){
            ordered_matches[struct_match->match.position()] = struct_match;
        }

        if(member_match){
            ordered_matches[member_match->match.position()] = member_match;
        }

        if(end_match){
            ordered_matches[end_match->match.position()] = end_match;
        }

        if(key_match){
            ordered_matches[key_match->match.position()] = key_match;
        }

        if(comment_match){
            ordered_matches[comment_match->match.position()] = comment_match;
        }

        if(block_comment_match){
            ordered_matches[block_comment_match->match.position()] = block_comment_match;
        }

        std::string next_search_str;



        for(auto m : ordered_matches){
            auto match = m.second;
            
            Entity* entity = 0;
            Entity* parent = 0;

            //Get the parent entity
            if(!parent_entity.empty()){
                parent = parent_entity.top();
            }

            switch(match->kind){
                case ELEMENT::MODULE:
                case ELEMENT::STRUCT:{
                    entity = construct_entity(match);
                    parent_entity.push(entity);
                    break;
                }
                case ELEMENT::MEMBER:{
                    entity = construct_member(match);
                    break;
                }
                case ELEMENT::END_BRACKET:{
                    if(!parent_entity.empty()){
                        parent_entity.pop();
                    }else{
                        std::cerr << "IdlParser: Got uneven brackets in IDL" << std::endl;
                    }
                    break;
                }
                case ELEMENT::IS_KEY:{
                    //#pragma keylist must be contained in a module
                    //This should mean that our current parent should contain a struct, and a member
                    auto struct_label = match->match[1];
                    auto member_label = match->match[2];
                    auto struct_entity = get_entity(parent, struct_label);
                    auto member_entity = get_entity(struct_entity, member_label);
                    if(member_entity && member_entity->getKind() == ELEMENT::MEMBER){
                        auto member = (Member*) member_entity;
                        member->is_key = true;
                    }else{
                        std::cerr << "IdlParser: Can't find Member: '" << member_label << "' inside Struct: '" << struct_label << "' inside Module: '" << parent->label << "'" << std::endl;
                    }
                    break;
                }
                case ELEMENT::COMMENT:
                case ELEMENT::BLOCK_COMMENT:
                    break;
                
            }

            if(entity){
                //Put into the map of entities
                entities_[entity->id] = entity;
                if(parent){
                    entity->parent_id = parent->id;
                    //Set the index of the entity
                    entity->index = parent->children_ids.size();
                    //Add as a child
                    parent->children_ids.push_back(entity->id);
                }
            }

            //Get the suffix of the string of the first element we matched
            next_search_str = match->match.suffix();
            break;
        }

        //Free memory from the regex_match objects
        for(auto m : ordered_matches){
            delete m.second;
        }

        //If we have less string to match, we should keep parsing.
        if(next_search_str.size() < search_str.size()){
            search_str = next_search_str;
        }else{
            //When we stop parsing anymore string we should break
            break;
        }
    }
}

IdlParser::~IdlParser(){
    delete exporter_;
};

std::string IdlParser::ToGraphml(){
    return exporter_->ToGraphml();
};
