#ifndef GLOBALINTERFACES_H
#define GLOBALINTERFACES_H

#include <deque>

#define EXPORT_FUNC __attribute__((visibility("default")))

//Interface for a standard In Event Port
template <class T> class InEventPort{
    public:
        virtual void rx_(T*) = 0;
};

//Interface for a standard Out Event Port
template <class T> class OutEventPort{
    public:
        virtual void tx_(T*) = 0;
};

class Component{
    public:
        Component(std::string inst_name);
        const std::string get_name();
        virtual void activate() = 0;
        virtual void passivate() = 0;
    private:
        std::string inst_name_;
};

class NodeContainer{
    public:
        void activate();
        void passivate();
        virtual void startup() = 0;
        void teardown();

        void add_component(Component* component);

    private:
        std::deque<Component*> components_;
};


inline Component::Component(std::string inst_name){
    inst_name_ = inst_name;
};

inline const std::string Component::get_name(){
    return inst_name_;
};

inline void NodeContainer::activate(){
    for(auto c : components_){
        c->activate();
    }
};

inline void NodeContainer::passivate(){
    for(auto c : components_){
        c->passivate();
    }
};

inline void NodeContainer::teardown(){
    while(!components_.empty()){
        delete components_.back();
        components_.pop_back();
    }
};

inline void NodeContainer::add_component(Component* component){
    components_.push_front(component);
};


#endif //GLOBALINTERFACES_H