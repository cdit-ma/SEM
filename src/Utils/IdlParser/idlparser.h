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

    enum class ELEMENT{
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
    std::string toString(ELEMENT);

    
    class IdlParser{
    private:
        class Entity;
        class Member;
    
    public:
        static std::string ParseIdl(std::string idl_path, bool pretty);
    private:
        IdlParser(std::string idl_path, bool pretty);
        ~IdlParser();
        void parse_file(std::string idl_path);
        std::string ToGraphml();

        bool resolve_member_label(Member* member, std::string label);
        bool resolve_member_type(Member* member, std::string type);

        std::string get_namespace(Entity* entity);


        Entity* get_parent(Entity* entity);

        void attach_entity(Entity* parent, Entity* child);
        void remove_entity(Entity* parent, Entity* child);


        Member* construct_member(Entity* parent, RegexMatch* match);
        Member* construct_typedef(Entity* parent, RegexMatch* match);
        Entity* construct_entity(Entity* parent, RegexMatch* match);

        int process_member(Member* member, Member* member_inst = 0, Entity* top_struct_entity = 0, int current_index = 0);
        int process_struct(Entity* struct_entity, Member* member_inst = 0, Entity* top_struct_entity = 0, int current_index = 0);
        
        
        Entity* get_ancestor_entity_by_label(Entity* child, std::string label);
        
        Entity* get_child_entity(Entity* parent, std::string label);
        
        Entity* get_namespaced_entity(std::string type);
        Entity* get_entity(int id);
    private:
        std::set<std::string> parsed_files_;
        std::set<int> struct_ids_;
        std::map<int, Entity*> entities_;
        
        
        GraphmlExporter* exporter_ = 0;


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
                std::string type;
                std::set<int> children_ids;
                //Hold a map for id > graphml id lookup of children
                std::map<int, int> id_lookup;
            protected:
                ELEMENT kind;
        };

        struct Member : public Entity{
            Member() : Entity(ELEMENT::MEMBER){};
            Member(ELEMENT element) : Entity(element){};

            bool is_key = false;

            bool is_sequence = false;
            bool is_complex = false;
            
            int complex_type_id = -1;
            std::string primitive_type;

            std::string to_string(){
                std::string str;
                str += toString(getKind());
                str += " Label: " + label + " ";
                str += is_sequence ? "Sequence Type: " : "Type: ";
                str += is_complex ? std::to_string(complex_type_id) : primitive_type;
                return str;
            }
        };
    };


#endif //IDL_PARSER_H
