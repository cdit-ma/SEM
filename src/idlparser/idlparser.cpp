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

#include "graphmlkinds.h"

//Some constants to clean up the regex
#define WS "\\s+"
#define OPTIONAL_WS "\\s*"
#define NOT_WS "\\S+"

#define ANY_CHAR "[\\S\\s]"

//IDL REGEXES
//typedef NAME TYPE;
const std::regex re_type_def("typedef" WS "(" ANY_CHAR "+?)" OPTIONAL_WS ";");
//#include "INCLUDE_H"
//#include <INCLUDE_H>
const std::regex re_local_include("#include" WS "\"(" ANY_CHAR "+?)\"");
const std::regex re_lib_include("#include" WS "<(" ANY_CHAR "+?)>");
//module MODULE_NAME {
const std::regex re_module("module" WS "(" NOT_WS ")" OPTIONAL_WS "\\{");
//struct STRUCT_NAME {
const std::regex re_struct("struct" WS "(" NOT_WS ")" OPTIONAL_WS "\\{");
//enum ENUM_LABEL {ENUM_VALUES};
const std::regex re_enum("enum" WS "(" NOT_WS ")" OPTIONAL_WS "\\{(" ANY_CHAR "*?)\\}");
//type label; (with )
const std::regex re_member("(" ANY_CHAR "+?)" OPTIONAL_WS ";" OPTIONAL_WS "(#@key)?");
//RTI's setting of key | //@key
const std::regex re_is_key("//" OPTIONAL_WS "@key");
//RTI's setting of key | //@key
//OpenSplice setting of key | #pragma keylist STRUCT_LABEL MEMBER_LABEL
const std::regex re_is_pragma_key("#pragma" WS "keylist" WS "(" NOT_WS ")" WS "(" NOT_WS ")");
//An array in IDL is listed always after the label | LABEL[SIZE]
const std::regex re_array("(.*?)" OPTIONAL_WS "[\\[<](.*)[\\]>]");
//sequence <SEQUENCE_TYPE>
const std::regex re_sequence("sequence" OPTIONAL_WS "<" OPTIONAL_WS "(.*)" OPTIONAL_WS ">");

//Comment structures (Don't be greedy!)
const std::regex re_comment_block("/\\*" "([" WS "|" NOT_WS "]*?)" "\\*/");
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

std::string toString(IDL_ELEMENT ele){
    switch(ele){
        case IDL_ELEMENT::MODULE:
            return "MODULE";
        case IDL_ELEMENT::STRUCT:
            return "STRUCT";
        case IDL_ELEMENT::MEMBER:
            return "MEMBER";
        case IDL_ELEMENT::END_BRACKET:
            return "END_BRACKET";
        case IDL_ELEMENT::IS_KEY:
            return "IS_KEY";
        case IDL_ELEMENT::PRAGMA_KEY:
            return "PRAGMA_KEY";
        case IDL_ELEMENT::COMMENT:
            return "COMMENT";
        case IDL_ELEMENT::BLOCK_COMMENT:
            return "BLOCK_COMMENT";
        case IDL_ELEMENT::TYPEDEF:
            return "TYPEDEF";
        case IDL_ELEMENT::ENUM:
            return "ENUM";
        case IDL_ELEMENT::NONE:
            return "NONE";
    }
    return "";
}

std::pair<std::string, std::string> split_label_and_type(std::string def_string){
    std::pair<std::string, std::string> results;

    //Tokenize on chunks of whitespace
    auto tokens = split(def_string, WS);
    if(tokens.size() > 1){
        //The label will always be the last IDL_ELEMENT
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
    }else if(type == "long double"){
        return_type = "Double";
    }else if(type == "unsigned long long"){
        return_type = "Integer";
    }else if(type == "long long"){
        return_type = "Integer";
    }

    if(return_type != "" && !caseless_compare(type, return_type)){
        std::cerr << "IDL Parser: Type '" << type << "' converted to '" << return_type << "'" << std::endl;
    }
    return return_type;
};

inline RegexMatch* re_search(std::string &str, IDL_ELEMENT kind, const std::regex re){
    auto me = new RegexMatch();
    me->got_match = std::regex_search(str, me->match, re);
    me->kind = kind;
    if(!me->got_match){
        delete me;
        me = 0;
    }
    return me;
};

std::pair<bool, std::string> IdlParser::ParseIdl(std::string idl_path, bool pretty){
    auto parser = IdlParser(idl_path, pretty);

    std::string result;

    if(parser.successful_parse){
        result = parser.ToGraphml();
    }
    return {parser.successful_parse, result};
};

bool IdlParser::resolve_member_label(MemberType* member, std::string label){
    auto unresolved_label = trim(label);
    
    auto sequence_match = re_search(unresolved_label, IDL_ELEMENT::NONE, re_array);
    
    if(sequence_match){
        std::cerr << "IDL Parser: '" << unresolved_label << "' converted to Sequence." << std::endl;
        member->label = sequence_match->match[1];
        member->is_sequence = true;
    }else{
        member->label = unresolved_label;
    }
    return member->label != "";
}

bool IdlParser::resolve_member_type(MemberType* member, std::string type){
    auto unresolved_type = trim(type);
    auto primitive_type = get_primitive_type(unresolved_type);



    //need to further resolve
    if(primitive_type == ""){
        //Get rid of spaces
        primitive_type = std::regex_replace(primitive_type, std::regex(WS), "");
 

        auto sequence_match = re_search(unresolved_type, IDL_ELEMENT::NONE, re_sequence);
        if(sequence_match){
            if(member->is_sequence){
                std::cerr << "IDL Parser: Cannot have a sequence of sequences." << std::endl;
                return false;
            }else{
                member->is_sequence = true;

                //Remove the size of a sequence
                auto remainder = sequence_match->match[1];
                std::string unresolved_type;
                for(auto token : split(remainder, ",")){
                    unresolved_type = token;
                    break;
                }

                //Resolve the rest of the sequence type (By removing the sequence part)
                return resolve_member_type(member, unresolved_type);
            }
        }else{
            //Split the type into a list of trimmed tokens (Split on ::)
            std::list<std::string> tokens;
            for(auto token : split(unresolved_type, "::")){
                tokens.push_back(trim(token));
            }

            Graphml::Entity* resolved_entity = 0;

            if(tokens.size() == 1){
                //If we only have one token, we should look in the current namespace stack
                resolved_entity = model_->get_ancestor_entity(member->parent, tokens.back());
            }else{
                //Try get an entire element 
                resolved_entity = model_->get_namespaced_entity(tokens);
            }

            auto resolved_kind = resolved_entity ? resolved_entity->get_kind() : Graphml::Kind::NONE;
            //Check if the resolved_entity is an Aggregate
            if(resolved_kind == Graphml::Kind::AGGREGATE || resolved_kind == Graphml::Kind::ENUM){
                member->complex_type = resolved_entity;
                return true;
            }else{
                //Maybe it's a typedef, check for the entire unresolved_type
                auto type_def = find_typedef(member->parent, unresolved_type);
                if(type_def){
                    //Set the state on the member
                    if(type_def->complex_type){
                        member->complex_type = type_def->complex_type;
                    }else{
                        member->primitive_type = type_def->primitive_type;
                    }
                    
                    if(type_def->is_sequence){
                        if(!member->is_sequence){
                            member->is_sequence = true;
                        }else{
                            std::cerr << "IDL Parser: Cannot have a sequence of sequences." << std::endl;
                            return false;
                        }
                    }

                    return true;
                }
            }

            std::cerr << "IDL Parser: Cannot find an entity named '" << unresolved_type << "'" << std::endl;
            return false;
        }
    }else{
        member->primitive_type = primitive_type;
        return true;
    }
}

IdlParser::IdlParser(std::string idl_path, bool pretty){
    
    model_ = new Graphml::Model();
    auto count = parse_file(idl_path);
    successful_parse = count == 0;
};

std::string combine_namespace(std::string parent_namespace, std::string label){
    return (parent_namespace == "" ? "" : parent_namespace + "::") + label;
}

IdlParser::MemberType* IdlParser::find_typedef(Graphml::Entity* current, std::string label){
    while(current){
        auto namespace_str = combine_namespace(current->get_namespace(), label);

        //Check if the current parent contains a namespace typedef
        if(type_defs.count(namespace_str)){
            return type_defs[namespace_str];
        }
        //Go up a parent
        current = current->get_parent();
    }
    return 0;
}

int IdlParser::parse_file(std::string idl_path){
    int error_count = 0;
    if(parsed_files_.count(idl_path)){
        return error_count;
    }

    auto file_path = get_file_path(idl_path);
    
    

    //Read the file
    std::string file;
    {
        std::ifstream input(idl_path);
        if(input.good()){
            std::stringstream buffer;
            buffer << input.rdbuf();
            input.close();
            file = buffer.str();
        }else{
            std::cerr << "IDL Parser: Can't read file '" << idl_path  << "'" << std::endl;
            error_count++;
            return error_count;
        }
    }

    std::cerr << "IDL Parser: Parsing: '" << idl_path << "'" << std::endl;
    parsed_files_.insert(idl_path);

    auto search_str = file;
    //LOSE ALL COMMENTS
    //Replace the Syntactical //@key with #@key//
    search_str = std::regex_replace(search_str, re_is_key, "#@key");
    search_str = std::regex_replace(search_str, re_comment, "");
    search_str = std::regex_replace(search_str, re_comment_block, "");

    //Preprocess parse the file to find the other files to import
    {
        auto include_str = search_str;
        //Keep parsing through the file
        while(true){
            auto local_include = re_search(include_str, IDL_ELEMENT::NONE, re_local_include);
            auto library_include = re_search(include_str, IDL_ELEMENT::NONE, re_lib_include);

            RegexMatch* include = 0;
            if(local_include && library_include){
                include = local_include->match.position() < library_include->match.position() ? local_include : library_include;
            }else if(local_include){
                include = local_include;
            }else if(library_include){
                include = library_include;
            }
            
            if(include){
                //Get the matching group, and parse that file first
                auto include_idl_path = file_path + std::string(include->match[1]);
                //Append on the relative path to files
                error_count += parse_file(include_idl_path);
                include_str = include->match.suffix();
            }else{
                break;
            }
        }
    }

    std::stack<Graphml::Entity*> parent_entities;
    parent_entities.push(model_->get_root());


   

    while(true){
        //Ordered map, to work out what is first IDL_ELEMENT to parse
        std::map<int, RegexMatch*> ordered_matches;

        std::list<RegexMatch*> matches;

        Graphml::Kind parent_kind = Graphml::Kind::NONE;

        //Get the parent entity
        if(!parent_entities.empty()){
            auto parent = parent_entities.top();
            parent_kind = parent->get_kind();
        }

        std::set<IDL_ELEMENT> valid_syntax;
        switch(parent_kind){
            case Graphml::Kind::NAMESPACE:
                valid_syntax.insert(IDL_ELEMENT::TYPEDEF);
                valid_syntax.insert(IDL_ELEMENT::MODULE);
                valid_syntax.insert(IDL_ELEMENT::STRUCT);
                valid_syntax.insert(IDL_ELEMENT::ENUM);
                valid_syntax.insert(IDL_ELEMENT::PRAGMA_KEY);
                valid_syntax.insert(IDL_ELEMENT::END_BRACKET);
                break;
            case Graphml::Kind::AGGREGATE:
                //Members and closing braces are the only thing which can be defined inside a Struct    
                valid_syntax.insert(IDL_ELEMENT::MEMBER);
                valid_syntax.insert(IDL_ELEMENT::END_BRACKET);
                break;
            default:
                break;
        }

        //Try and match the regex IDL_ELEMENTs with the search_str (Returns 0 on no match)
        //Order these in the order which they need to be dealt with
        if(valid_syntax.count(IDL_ELEMENT::TYPEDEF)){
            matches.push_back(re_search(search_str, IDL_ELEMENT::TYPEDEF, re_type_def));
        }
        if(valid_syntax.count(IDL_ELEMENT::MODULE)){
            matches.push_back(re_search(search_str, IDL_ELEMENT::MODULE, re_module));
        }
        if(valid_syntax.count(IDL_ELEMENT::STRUCT)){
            matches.push_back(re_search(search_str, IDL_ELEMENT::STRUCT, re_struct));
        }
        if(valid_syntax.count(IDL_ELEMENT::END_BRACKET)){
            matches.push_back(re_search(search_str, IDL_ELEMENT::END_BRACKET, re_end_bracket));
        }
        if(valid_syntax.count(IDL_ELEMENT::PRAGMA_KEY)){
            matches.push_back(re_search(search_str, IDL_ELEMENT::PRAGMA_KEY, re_is_pragma_key));
        }
        if(valid_syntax.count(IDL_ELEMENT::MEMBER)){
            matches.push_back(re_search(search_str, IDL_ELEMENT::MEMBER, re_member));
        }

        if(valid_syntax.count(IDL_ELEMENT::ENUM)){
            matches.push_back(re_search(search_str, IDL_ELEMENT::ENUM, re_enum));
        }

        for(auto match : matches){
            if(match){
                auto pos = match->match.position();
                if(!ordered_matches.count(pos)){
                    //Place all successful matches into map (Ordered by their position in the string)
                    ordered_matches[match->match.position()] = match;
                }
            }
        }

        std::string next_search_str;



        for(auto m : ordered_matches){
            auto match = m.second;

            auto new_parent = parent_entities.top();
            switch(match->kind){
                case IDL_ELEMENT::MODULE:{
                    auto label = match->match[1];
                    auto namespace_parent = Graphml::AsNamespace(new_parent);
                    //Construct a namespace
                    auto new_entity = model_->construct_namespace(namespace_parent, label);
                    parent_entities.push(new_entity);
                    break;
                }
                case IDL_ELEMENT::STRUCT:{
                    auto label = match->match[1];
                    auto namespace_parent = Graphml::AsNamespace(new_parent);
                    auto new_entity = model_->construct_aggregate(namespace_parent, label);
                    parent_entities.push(new_entity);
                    break;
                }
                case IDL_ELEMENT::ENUM:{
                    auto label = match->match[1];
                    std::string type = match->match[2];
                    auto enum_value_str = std::regex_replace(type, std::regex(WS), "");
                    auto enum_tokens_list = split(enum_value_str, ",");
                    std::vector<std::string> enum_token_v{std::begin(enum_tokens_list), std::end(enum_tokens_list)};
                    auto namespace_parent = Graphml::AsNamespace(new_parent);
                    auto new_entity = model_->construct_enum(namespace_parent, label, enum_token_v);
                    break;
                }
                case IDL_ELEMENT::TYPEDEF:
                case IDL_ELEMENT::MEMBER:{

                    //Gets the entire type and label so we can resolve it.
                    auto label_type_pair = split_label_and_type(match->match[1]);
                    auto unresolved_label = label_type_pair.first;
                    bool is_key = false;
                    if(match->kind == IDL_ELEMENT::MEMBER){
                        is_key = match->match[2].length();
                    }
                    
                    //Remove all spaces in the unresolved_type
                    auto unresolved_type = label_type_pair.second;
                    
                    
                    auto member = new MemberType();
                    member->parent = new_parent;
                    
                    auto resolved_label = resolve_member_label(member, unresolved_label);
                    auto resolved_type = resolve_member_type(member, unresolved_type);
                    

                    
                    if(resolved_label && resolved_type){
                        if(match->kind == IDL_ELEMENT::TYPEDEF){
                            auto typedef_ns = combine_namespace(member->parent->get_namespace(), member->label);

                            //add it to the typedef map
                            if(!type_defs.count(typedef_ns)){
                                type_defs[typedef_ns] = member;
                            }else{
                                std::cerr << "IDL Parser: Got duplicate typedef with resolved namespace '" << typedef_ns << "'" << std::endl;
                                error_count ++;
                            }
                        
                        }else{
                            //A Member
                            auto namespace_parent = Graphml::AsAggregate(member->parent);
                            if(namespace_parent){
                                Graphml::Member *g_member = 0;
                                if(member->complex_type){
                                    g_member = model_->construct_complex_member(namespace_parent, member->label, member->complex_type, member->is_sequence);
                                }else{
                                    g_member = model_->construct_primitive_member(namespace_parent, member->label, member->primitive_type, member->is_sequence);
                                }
                                if(g_member && is_key){
                                    g_member->set_is_key(is_key);
                                }
                            }
                        }
                    }else{
                        //Free up memory
                        delete member;
                    }
                    break;
                }
                case IDL_ELEMENT::END_BRACKET:{
                    if(!parent_entities.empty()){
                        parent_entities.pop();
                    }else{
                        std::cerr << "IdlParser: Got uneven brackets in IDL" << std::endl;
                        error_count ++;
                    }
                    break;
                }
                case IDL_ELEMENT::PRAGMA_KEY:{
                    std::string aggregate_name = match->match[1];
                    std::string member_name = match->match[2];
                    member_name = std::regex_replace(member_name, std::regex(";"), "");

                    auto aggregate = new_parent->get_child(aggregate_name);
                    if(aggregate){
                        auto child = aggregate->get_child(member_name);
                        auto member = Graphml::AsMember(child);
                        if(member){
                            member->set_is_key(true);
                        }
                    }

                    break;
                }
                default:
                    break;
            }

            //Get the suffix of the string of the first IDL_ELEMENT we matched
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
    return error_count;
}

IdlParser::~IdlParser(){
    delete model_;
};

std::string IdlParser::ToGraphml(){
    return model_->ToGraphml();
};
