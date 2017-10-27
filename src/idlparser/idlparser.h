#ifndef IDL_PARSER_H
#define IDL_PARSER_H

#include <string>
#include <regex>
#include <map>
#include <set>

enum class IDL_ELEMENT{
    NONE,
    MODULE,
    STRUCT,
    MEMBER,
    END_BRACKET,
    IS_KEY,
    PRAGMA_KEY,
    COMMENT,
    BLOCK_COMMENT,
    TYPEDEF,
    ENUM
};

namespace Graphml{
    class Model;
    class Entity;
}

struct RegexMatch{
    IDL_ELEMENT kind;
    std::smatch match;
    bool got_match = false;
};


std::string toString(IDL_ELEMENT);


class IdlParser{
    public:
        static std::pair<bool, std::string> ParseIdls(std::vector<std::string> idl_file_paths, bool pretty);
    private:
        struct MemberType{
            std::string label;
            bool is_sequence = false;
            bool is_complex = false;
            std::string primitive_type;
    
            Graphml::Entity* parent = 0;
            Graphml::Entity* complex_type = 0;
        };
        IdlParser(std::vector<std::string> idl_file_paths, bool pretty);
        ~IdlParser();
        int parse_file(std::string idl_path);
        std::string ToGraphml();

        MemberType* find_typedef(Graphml::Entity* current, std::string label);
    private:

    

        bool resolve_member_label(MemberType* member, std::string label);
        bool resolve_member_type(MemberType* member, std::string type);
    private:
        std::set<std::string> parsed_files_;
        std::map<std::string, MemberType*> type_defs;
        bool successful_parse = false;
        Graphml::Model* model_ = 0;
    };


#endif //IDL_PARSER_H
