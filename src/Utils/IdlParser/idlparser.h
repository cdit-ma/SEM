#ifndef IDL_PARSER_H
#define IDL_PARSER_H

#include <string>
#include <regex>
#include <map>
#include <set>
#include <list>

    //Forward Declare
    enum class ELEMENT;
    class GraphmlExporter;
   

    struct RegexMatch{
        ELEMENT kind;
        std::smatch match;
        bool got_match = false;
    };

    enum class ELEMENT{MODULE, STRUCT, MEMBER, END_BRACKET, IS_KEY};

    
    class IdlParser{
    private:
        class Entity;
        class Member;
    
    public:
        static std::string ParseIdl(std::string idl_path, bool pretty);
        //static std::string ParseIdl(std::string idl_path, bool pretty);
    private:
        IdlParser(std::string idl_path, bool pretty);
        ~IdlParser();

        std::string ToGraphml();

        void parse_file(std::string idl_path);


        std::set<std::string> parsed_files_;
        std::set<int> struct_ids_;
        std::map<int, IdlParser::Entity*> entities_;
        GraphmlExporter* exporter_;

        std::string get_namespace(Entity* entity);

        Entity* get_parent(Entity* entity);
        Entity* get_entity(Entity* parent, std::string child_label);
        std::set<IdlParser::Entity*> get_entities(ELEMENT kind, std::string label);
        Entity* get_struct(std::string struct_label, std::string module_label = "");

        Member* construct_member(RegexMatch* match);
        Entity* construct_entity(RegexMatch* match);

        int process_member(Member* member, Member* member_inst = 0, Entity* top_struct_entity = 0, int current_index = 0);
        int process_struct(Entity* struct_entity, Member* member_inst = 0, Entity* top_struct_entity = 0, int current_index = 0);
        std::string path;
    private:
        struct Entity{
                Entity(ELEMENT kind){
                    this->kind = kind;
                };
                ELEMENT getKind(){
                    return this->kind;
                }
                int id = -1;
                int parent_id = -1;
                int index = -1;
                std::string label;
                std::list<int> children_ids;
                //Hold a map for id > graphml id lookup of children
                std::map<int, int> id_lookup;
            protected:
                ELEMENT kind;
        };

        struct Member : public Entity{
            Member() : Entity(ELEMENT::MEMBER){};

            std::string type;
            bool is_key = false;
            bool is_sequence = false;
            std::string sequence_type;
            int sequence_type_id = -1;
            int definition_id = -1;
        };
    };


#endif //IDL_PARSER_H
