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

//Some constants to clean up the regex
#define WS "\\s+"
#define OPTIONAL_WS "\\s*"
#define NOT_WS "\\S+"

//IDL REGEXES
//typedef NAME TYPE;
const std::regex re_type_def("typedef" WS "(.+?)" OPTIONAL_WS ";");
//#include "INCLUDE_H"
//#include <INCLUDE_H>
const std::regex re_include("#include" WS "([<" NOT_WS ">|\"" NOT_WS "\"])");
//module MODULE_NAME {
const std::regex re_module("module" WS "(" NOT_WS ")" OPTIONAL_WS "\\{");
//struct STRUCT_NAME {
const std::regex re_struct("struct" WS "(" NOT_WS ")" OPTIONAL_WS "\\{");
//enum ENUM_LABEL {ENUM_VALUES};
const std::regex re_enum("enum" WS "(" NOT_WS ")" OPTIONAL_WS "\\{(.*?)\\};");
//type label; (with )
const std::regex re_member("(" NOT_WS ".+?)" OPTIONAL_WS ";" OPTIONAL_WS "(#@key)?");
//RTI's setting of key | //@key
const std::regex re_is_key("//" OPTIONAL_WS "@key(.*)");
//OpenSplice setting of key | #pragma keylist STRUCT_LABEL MEMBER_LABEL
const std::regex re_is_pragma_key("#pragma" WS "keylist" WS "(" NOT_WS ")" WS "(" NOT_WS ")");
//An array in IDL is listed always after the label | LABEL[SIZE]
const std::regex re_array("(.*?)" OPTIONAL_WS "\\[(.*)\\]");
//sequence <SEQUENCE_TYPE>
const std::regex re_sequence("sequence" OPTIONAL_WS "<" OPTIONAL_WS "(.*)" OPTIONAL_WS ">");

//Comment structures
const std::regex re_comment_block("/\\*" "([" WS "|" NOT_WS "]*)" "\\*/");
const std::regex re_comment("//(.*)");

//Helper Regexes
const std::regex re_split_path("(.*[/\\\\])*(.*)");

//END bracket
const std::regex re_end_bracket("\\}");

//Trim Helpers
const std::regex re_trim_start("^" WS);
const std::regex re_trim_end(WS "$");

//Splits a string into a list, split by the regex token
std::list<std::string> split(const std::string& input, const std::string& regex) {
    // passing -1 as the submatch index parameter performs splitting
    std::regex re(regex);
    std::sregex_token_iterator first{input.begin(), input.end(), re, -1};
    std::sregex_token_iterator last;
    return {first, last};
};

std::string toString(ELEMENT ele){
    switch(ele){
        case ELEMENT::MODULE:
            return "MODULE";
        case ELEMENT::STRUCT:
            return "STRUCT";
        case ELEMENT::MEMBER:
            return "MEMBER";
        case ELEMENT::END_BRACKET:
            return "END_BRACKET";
        case ELEMENT::IS_KEY:
            return "IS_KEY";
        case ELEMENT::PRAGMA_KEY:
            return "PRAGMA_KEY";
        case ELEMENT::COMMENT:
            return "COMMENT";
        case ELEMENT::BLOCK_COMMENT:
            return "BLOCK_COMMENT";
        case ELEMENT::TYPEDEF:
            return "TYPEDEF";
        case ELEMENT::ENUM:
            return "ENUM";
        case ELEMENT::NONE:
            return "NONE";
    }
    return "";
}

std::pair<std::string, std::string> split_label_and_type(std::string def_string){
    std::pair<std::string, std::string> results;

    //Tokenize on chunks of whitespace
    auto tokens = split(def_string, WS);
    if(tokens.size() > 1){
        //The label will always be the last element
        results.first = tokens.back();
        //Remove the label from the tokens list
        tokens.pop_back();
        
        //Construct a new single space seperated type
        for(auto s : tokens){
            if(s != ""){
                results.second += s + " ";
            }
        }
        //Remove the last space lel
        results.second.pop_back();
    }
    return results;
}

//Given a file path, get the relative directory
std::string get_file_path(const std::string file_path){
    std::smatch match;
    if(std::regex_search(file_path, match, re_split_path)){
        return match[1];
    }
    return "";
}

//Removes any white space at the start and end of a string
std::string trim(std::string str){
    str = std::regex_replace(str, re_trim_start, "");
    str = std::regex_replace(str, re_trim_end, "");
    return str;
}


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

    if(return_type != "" && !caseless_compare(type, return_type)){
        std::cerr << "IDL Parser: Type '" << type << "' converted to '" << return_type << "'" << std::endl;
    }
    return return_type;
};



IdlParser::Entity* IdlParser::get_parent(Entity* entity){
    if(entity && entities_.count(entity->parent_id)){
        return entities_[entity->parent_id];
    }
    return 0;
};

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



std::string IdlParser::ParseIdl(std::string idl_path, bool pretty){
    auto parser = IdlParser(idl_path, pretty);
    return parser.ToGraphml();
};


//Searches up the Entity tree, checking all siblings for a matching label
IdlParser::Entity* IdlParser::get_ancestor_entity_by_label(IdlParser::Entity* current, std::string label){
    while(current){
        auto child = get_child_entity(current, label);
        if(child){
            return child;
        }
        current = get_parent(current);
    }
    return 0;
}

//Gets a direct child of parent, with label
IdlParser::Entity* IdlParser::get_child_entity(IdlParser::Entity* parent, std::string label){
    if(parent){
        for(auto id : parent->children_ids){
            auto child = get_entity(id);
            if(child && child->label == label){
                return child;
            }
        }
    }
    return 0;
}

//Gets an entity which matches the namespace provided
IdlParser::Entity* IdlParser::get_namespaced_entity(std::string type){
    auto current = get_entity(0);

    auto tokens = split(type, "::");
    
    while(current && tokens.size()){
        //Take the front of the token, this should be the lowest named element
        auto front_label = trim(tokens.front());
        tokens.pop_front();

        //If we have a child with this label, continue down the child stack
        auto child = get_child_entity(current, front_label);
        if(child){
            current = child;
        }else{
            return 0;
        }
    }
    return current;
}

//Get an entity by its id
IdlParser::Entity* IdlParser::get_entity(int id){
    if(entities_.count(id)){
        return entities_[id];
    }
    return 0;
}

bool IdlParser::resolve_member_type(IdlParser::Member* member, std::string type){
    auto unresolved_type = trim(type);
    auto primitive_type = get_primitive_type(unresolved_type);

    //need to further resolve
    if(primitive_type == ""){
        auto sequence_match = re_search(unresolved_type, ELEMENT::NONE, re_sequence);
        if(sequence_match){
            if(member->is_sequence){
                std::cerr << "IDL Parser: Cannot have a sequence of sequences." << std::endl;
                return false;
            }else{
                member->is_sequence = true;
                //Resolve the rest of the sequence type
                return resolve_member_type(member, sequence_match->match[1]);
            }
        }else{
            //Check for complex types and type defs
            //Split on namespaces
            auto parent = get_parent(member);
            //std::cerr << "\t\t\t Need to resolve Resolve_type: $" << unresolved_type << "$" << std::endl;
            
            auto tokens = split(unresolved_type, "::");

            Entity* resolved_entity = 0;
            //Look only in parent chain
            if(tokens.size() == 1){
                resolved_entity = get_ancestor_entity_by_label(parent, tokens.back());
            }else{
                resolved_entity = get_namespaced_entity(unresolved_type);
            }

            if(resolved_entity){
                if(resolved_entity->getKind() == ELEMENT::TYPEDEF){
                    auto type_def = (Member*) resolved_entity;
                    if(type_def->is_complex){
                        if(!member->is_complex){
                            member->is_complex = true;
                            member->complex_type_id = type_def->complex_type_id;
                        }
                    }else{
                        member->primitive_type = type_def->primitive_type;
                    }

                    if(type_def->is_sequence){
                        if(member->is_sequence){
                            std::cerr << "IDL Parser: Cannot have a sequence of sequences." << std::endl;
                            return false;
                        }else{
                            member->is_sequence = true;
                        }
                    }
                }else if(resolved_entity->getKind() == ELEMENT::STRUCT){
                    member->is_complex = true;
                    member->complex_type_id = resolved_entity->id;
                }
                return true;
            }else{
                return false;
            }
        }
    }else{
        member->primitive_type = primitive_type;
        return true;
    }
    return false;
}
void IdlParser::attach_entity(IdlParser::Entity* parent, IdlParser::Entity* child){
    auto id = entities_.size();
    child->id = id;
    //Insert into the map
    entities_[id] = child;

    if(parent){
        child->parent_id = parent->id;
        //Set the index of the entity
        child->index = parent->children_ids.size();
        //Add as a child
        parent->children_ids.insert(id);
    }
}

void IdlParser::remove_entity(IdlParser::Entity* parent, IdlParser::Entity* child){
    if(child){
        auto id = child->id;
        entities_.erase(id);
        if(parent){
            parent->children_ids.erase(id);
        }
    }
}

bool IdlParser::resolve_member_label(IdlParser::Member* member, std::string label){
    std::smatch match;
    if(member){
        if(std::regex_search(label, match, re_array)){
            //Is an array
            member->label = match[1];
            member->is_sequence = true;
            std::cerr << "IDL Parser: Resolved " << label << " as a sequence." << std::endl;
        }else{
            member->label = label;
        }
        return !member->label.empty();
    }
    return false;
}

//Make GraphML Structures
IdlParser::Member* IdlParser::construct_member(IdlParser::Entity* parent, RegexMatch* m){
    auto entity = new Member();
    attach_entity(parent, entity);
    
    
    //Gets the entire type and label so we can resolve it.
    auto label_type_pair = split_label_and_type(m->match[1]);
    
    auto resolved_label = resolve_member_label(entity, label_type_pair.first);
    auto resolved_type = resolve_member_type(entity, label_type_pair.second);

    if(m->match[2].length()){
        entity->is_key = true;
    }

    if(resolved_label && resolved_type){
        return entity;
    }else{
        remove_entity(parent, entity);
        delete entity;
        std::cerr << "IDL Parser: Cannot Resolve type: " << label_type_pair.second << std::endl;
        return 0;
    }
};

//Make GraphML Structures
IdlParser::Member* IdlParser::construct_typedef(IdlParser::Entity* parent, RegexMatch* m){
    auto entity = new Member(ELEMENT::TYPEDEF);
    attach_entity(parent, entity);
    
    //Gets the entire type and label so we can resolve it.
    auto label_type_pair = split_label_and_type(m->match[1]);
    
    auto resolved_label = resolve_member_label(entity, label_type_pair.first);
    auto resolved_type = resolve_member_type(entity, label_type_pair.second);

    if(resolved_label && resolved_type){
        return entity;
    }else{
        remove_entity(parent, entity);
        delete entity;
        std::cerr << "IDL Parser: Cannot Resolve TypeDef type: " << label_type_pair.second << std::endl;
        return 0;
    }
};

IdlParser::Entity* IdlParser::construct_entity(IdlParser::Entity* parent, RegexMatch* match){
    auto entity = new Entity(match->kind);
    attach_entity(parent, entity);
    entity->label = match->match[1];
    return entity;
};

int IdlParser::process_member(Member* member, Member* member_inst, Entity* top_struct_entity, int current_index){
    bool is_instance = member_inst != 0;

    if(member->is_sequence){
        auto graphml_id = exporter_->export_node();
        top_struct_entity->id_lookup[current_index] = graphml_id;

        //If we are sequence
        exporter_->export_data("kind", is_instance ? "VectorInstance" : "Vector");
        exporter_->export_data("label", member->label);      
        exporter_->export_data("index", std::to_string(member->index));
        
        if(member->is_complex){
            auto complex_type = entities_[member->complex_type_id];
            exporter_->export_data("type", "Vector::" + complex_type->label);
            exporter_->export_graph();
            current_index = process_struct(complex_type, member, top_struct_entity, current_index);
        }else{
            exporter_->export_data("type", "Vector::" + member->primitive_type);
            exporter_->export_graph();
            //Primitive type
            current_index ++;
            auto member_graphml_id = exporter_->export_node();
            //Insert the item into the lookup
            top_struct_entity->id_lookup[current_index] = member_graphml_id;
            exporter_->export_data("kind", is_instance ? "MemberInstance" : "Member");
            exporter_->export_data("label", member->label); 
            exporter_->export_data("type", member->primitive_type);
            exporter_->export_data("index", std::to_string(member->index));
            exporter_->close_element();
        }
        //Close the graph
        exporter_->close_element();
        //Close the node
        exporter_->close_element();
    }else{
        if(member->is_complex){
            auto complex_type = entities_[member->complex_type_id];
            current_index = process_struct(complex_type, member, top_struct_entity, current_index);
        }else{
            auto graphml_id = exporter_->export_node();
            top_struct_entity->id_lookup[current_index] = graphml_id;

            exporter_->export_data("kind", is_instance ? "MemberInstance" : "Member");
            exporter_->export_data("label", member->label); 
            exporter_->export_data("type", member->primitive_type);
            exporter_->export_data("index", std::to_string(member->index));
            if(!is_instance && member->is_key){
                exporter_->export_data("key", "true");
            }
            exporter_->close_element();
        }
    }
    return current_index;
}

std::string IdlParser::get_namespace(Entity* entity){
    auto parent = get_parent(entity);
    std::string namespace_str;
    while(parent){
        auto p_label = parent->label;
        parent = get_parent(parent);
        if(parent){
            namespace_str = "::" + p_label + namespace_str;
        }
    }
    return namespace_str;
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
            if(member->is_complex){
                definition = entities_[member->complex_type_id];
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

    auto file_path = get_file_path(idl_path);
    
    std::cerr << "Parsing: " << idl_path << std::endl;
    parsed_files_.insert(idl_path);

    //Read the file
    std::string file;
    {
        std::ifstream input(idl_path);
        std::stringstream buffer;
        buffer << input.rdbuf();
        input.close();
        file = buffer.str();
    }

    //Preprocess parse the file to find the other files to import
    {
        auto search_str = file;
        std::smatch import_match;
        //Keep parsing through the file
        while(std::regex_search(search_str, import_match, re_include)){
            //Get the matching group, and parse that file first
            auto include_idl_path = file_path + std::string(import_match[1]);

            //Append on the relative path to files
            parse_file(include_idl_path);
            search_str = import_match.suffix();
        }
    }

    std::stack<Entity*> parent_entity;
    if(!entities_.size()){
        auto entity = new Entity(ELEMENT::MODULE);
        entity->label = "ROOT";
        attach_entity(0, entity);
    }

    auto top_parent = get_entity(0);
    if(top_parent){
        parent_entity.push(get_entity(0));
    }


    auto search_str = file;
    //LOSE ALL COMMENTS
    //Replace the Syntactical //@key with #@key//
    search_str = std::regex_replace(search_str, re_is_key, "#@key//");
    search_str = std::regex_replace(search_str, re_comment, "");
    search_str = std::regex_replace(search_str, re_comment_block, "");
    
    while(true){
        //Ordered map, to work out what is first element to parse
        std::map<int, RegexMatch*> ordered_matches;

        std::list<RegexMatch*> matches;

        ELEMENT parent_kind = ELEMENT::NONE;
        //Get the parent entity
        if(!parent_entity.empty()){
            auto parent = parent_entity.top();
            parent_kind = parent->getKind();
        }

        std::set<ELEMENT> valid_syntax;
        switch(parent_kind){
            case ELEMENT::MODULE:
                //Module can have everything NONE can have, and a closing brace
                valid_syntax.insert(ELEMENT::END_BRACKET);
            case ELEMENT::NONE: 
                valid_syntax.insert(ELEMENT::TYPEDEF);
                valid_syntax.insert(ELEMENT::MODULE);
                valid_syntax.insert(ELEMENT::STRUCT);
                valid_syntax.insert(ELEMENT::ENUM);
                valid_syntax.insert(ELEMENT::PRAGMA_KEY);
                break;
            case ELEMENT::STRUCT:
                //Members and closing braces are the only thing which can be defined inside a Struct    
                valid_syntax.insert(ELEMENT::MEMBER);
                valid_syntax.insert(ELEMENT::END_BRACKET);
                break;
            default:
                break;
        }

        //Try and match the regex elements with the search_str (Returns 0 on no match)
        //Order these in the order which they need to be dealt with
        if(valid_syntax.count(ELEMENT::TYPEDEF)){
            matches.push_back(re_search(search_str, ELEMENT::TYPEDEF, re_type_def));
        }
        if(valid_syntax.count(ELEMENT::MODULE)){
            matches.push_back(re_search(search_str, ELEMENT::MODULE, re_module));
        }
        if(valid_syntax.count(ELEMENT::STRUCT)){
            matches.push_back(re_search(search_str, ELEMENT::STRUCT, re_struct));
        }
        if(valid_syntax.count(ELEMENT::END_BRACKET)){
            matches.push_back(re_search(search_str, ELEMENT::END_BRACKET, re_end_bracket));
        }
        if(valid_syntax.count(ELEMENT::PRAGMA_KEY)){
            matches.push_back(re_search(search_str, ELEMENT::PRAGMA_KEY, re_is_pragma_key));
        }
        if(valid_syntax.count(ELEMENT::MEMBER)){
            matches.push_back(re_search(search_str, ELEMENT::MEMBER, re_member));
        }

        if(valid_syntax.count(ELEMENT::ENUM)){
            matches.push_back(re_search(search_str, ELEMENT::ENUM, re_enum));
        }

        
        
        for(auto match : matches){
            if(match){
                auto pos = match->match.position();
                if(!ordered_matches.count(pos)){
                    //std::cerr << "Matched: " << toString(match->kind) << " @ " << pos << std::endl;
                    //Place all successful matches into map (Ordered by their position in the string)
                    ordered_matches[match->match.position()] = match;
                }
            }
        }

        std::string next_search_str;

        for(auto m : ordered_matches){
            auto match = m.second;

            Entity* entity = 0;
            Entity* parent = parent_entity.empty() ? 0 : parent_entity.top();
            
            switch(match->kind){
                case ELEMENT::MODULE:{
                    entity = construct_entity(parent, match);
                    parent_entity.push(entity);
                    break;
                }
                case ELEMENT::STRUCT:{
                   entity = construct_entity(parent, match);
                   struct_ids_.insert(entity->id);
                   parent_entity.push(entity);
                   break;
                }
                case ELEMENT::TYPEDEF:{
                    entity = construct_typedef(parent, match);
                    break;
                }
                case ELEMENT::MEMBER:{
                    entity = construct_member(parent, match);
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
                    auto struct_entity = get_child_entity(parent, struct_label);
                    auto member_entity = get_child_entity(struct_entity, member_label);
                    if(member_entity && member_entity->getKind() == ELEMENT::MEMBER){
                        auto member = (Member*) member_entity;
                        member->is_key = true;
                    }else{
                        std::cerr << "IdlParser: Can't find Member: '" << member_label << "' inside Struct: '" << struct_label << "' inside Module: '" << parent->label << "'" << std::endl;
                    }
                    break;
                }
                default:
                    break;
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
