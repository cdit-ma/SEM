#ifndef GRAPHML_KINDS_H
#define GRAPHML_KINDS_H

#include <string>
#include <map>
#include <set>
#include <vector>

namespace Graphml{
    enum class Kind{
        NONE,
        NAMESPACE,
        ENUM,
        AGGREGATE,
        MEMBER
    };
    class Model;

    class Entity{
        friend class Model;
        protected:
            Entity(Entity* parent, std::string label, Kind kind);
            ~Entity();
            void set_id(int id);
            void add_child(Entity* child);
            void remove_child(Entity* child);
        public:
            void set_label(std::string label);
            int get_id() const;
            std::set<Entity*> get_children() const;
            std::string get_label() const;
            Kind get_kind() const;
        private:
            Kind kind_;
            int id_ = -1;
            std::string label_;
            Entity* parent_ = 0;
            std::set<Entity*> children_;
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
        private:
            Entity* complex_type_ = 0;
            std::string primitive_type_;
            bool is_sequence_ = false;
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
        private:
            void insert_into_map(Entity* entity);
            std::map<int, Entity*> entities_;
            Namespace* root_ = 0;
    };

    Namespace* AsNamespace(Entity* entity);
    Aggregate* AsAggregate(Entity* entity);
};

#endif //GRAPHML_KINDS_H
