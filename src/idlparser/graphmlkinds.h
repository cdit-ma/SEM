#ifndef GRAPHML_KINDS_H
#define GRAPHML_KINDS_H

#include <string>
#include <map>
#include <set>
#include <vector>
#include <list>

namespace Graphml{
    enum class Kind{
        NONE,
        NAMESPACE,
        ENUM,
        AGGREGATE,
        MEMBER
    };
    class Model;
    class Exporter;

    class Entity{
        friend class Model;
        protected:
            Entity(Entity* parent, std::string label, Kind kind);
            ~Entity();
            void set_id(int id);
            int add_child(Entity* child);
            void remove_child(Entity* child);
            void add_index_definition_id(int index, int definition_id);
            int get_index_definition_id(int index) const;
        public:
            void set_label(std::string label);
            int get_id() const;
            int get_index() const;
            std::vector<Entity*> get_children() const;
            std::string get_label() const;
            Kind get_kind() const;
            Entity* get_parent() const;
            virtual std::string get_namespace() const;

            Entity* get_child(std::string label) const;
        private:
            Kind kind_;
            int id_ = -1;
            int index_ = 0;
            std::string label_;
            Entity* parent_ = 0;
            std::vector<Entity*> children_;

            //Hold a map for id > graphml id lookup of children
            std::map<int, int> id_lookup_;
    };

    class Namespace : public Entity{
        friend class Model;
        protected:
            Namespace(Namespace* parent, std::string label);
            ~Namespace();
    };

    class Enum : public Entity{
        friend class Model;
        protected:
            Enum(Namespace* parent, std::string label, std::vector<std::string> enum_values);
            ~Enum();
        public:
            std::vector<std::string> get_enum_values() const;
        private:
            std::vector<std::string> enum_values_;
    };

    class Aggregate : public Entity{
        friend class Model;
        protected:
            Aggregate(Namespace* parent, std::string label);
            ~Aggregate();
        public:
            std::string get_namespace() const;
    };

    class Member : public Entity{
        friend class Model;
        protected:
            Member(Aggregate* parent, std::string label, std::string primitive_type, bool is_sequence);
            Member(Aggregate* parent, std::string label, Entity* complex_type, bool is_sequence);
            ~Member();
        public:
            bool is_complex() const;
            bool is_sequence() const;
            Entity* get_complex_type() const;
            std::string get_primitive_type() const;
            void set_is_key(bool is_key);
            bool is_key() const;
        private:
            Entity* complex_type_ = 0;
            std::string primitive_type_;
            bool is_sequence_ = false;
            bool is_key_ = false;
    };

    class Model{
        public:
            Model();
            ~Model();
            std::string ToGraphml();
        
            Namespace* get_root() const;
            //Constructors
            Namespace* construct_namespace(Namespace* parent, std::string label);
            Enum* construct_enum(Namespace* parent, std::string label, std::vector<std::string> values);
            Aggregate* construct_aggregate(Namespace* parent, std::string label);
            Member* construct_complex_member(Aggregate* parent, std::string label, Entity* complex_type, bool is_sequence = false);
            Member* construct_primitive_member(Aggregate* parent, std::string label, std::string primitive_type, bool is_sequence = false);

            Entity* get_ancestor_entity(Entity* entity, std::string label);
            Entity* get_namespaced_entity(std::list<std::string> namespace_tokens);
        private:
            int export_entity(Exporter* exporter, Entity* entity, Member* member_instance = 0, Aggregate* top_aggregate = 0, int current_index = 0);
            int export_complex_type(Exporter* exporter, Member* entity, Member* member_instance, Aggregate* top_aggregate, int current_index);
            int export_aggregate(Exporter* exporter, Aggregate* aggregate, Member* member_instance = 0, Aggregate* top_aggregate = 0, int current_index = 0);
            int export_member(Exporter* exporter, Member* aggregate, Member* member_instance = 0, Aggregate* top_aggregate = 0, int current_index = 0);
            int export_enum(Exporter* exporter, Enum* enum_val, Member* member_instance = 0, Aggregate* top_aggregate = 0, int current_index = 0);
            Entity* get_entity(int id);

            void insert_into_map(Entity* entity);
            std::map<int, Entity*> entities_;
            std::list<int> aggregate_ids_;
            Namespace* root_ = 0;
    };

    Namespace* AsNamespace(Entity* entity);
    Aggregate* AsAggregate(Entity* entity);
    Member* AsMember(Entity* entity);
    Enum* AsEnum(Entity* entity);
};

#endif //GRAPHML_KINDS_H
