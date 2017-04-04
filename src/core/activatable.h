#ifndef ACTIVATABLE_H
#define ACTIVATABLE_H

#include <string>
#include <map>

class ModelLogger;

class Activatable{
    enum class STATE{
        PASSIVE = 0,
        ACTIVE = 1,
        DEAD = 2,
    };

    public: 
        const std::string get_name();
        const std::string get_id();
        const std::string get_type();
        void set_name(std::string name);
        void set_id(std::string id);
        void set_type(std::string type);
        const Activatable::STATE get_state();
        const bool is_active();
        ModelLogger* logger();

        virtual bool Activate();            //Start Component (Start Middleware etc)
        virtual bool Passivate();           //Stop Component 
        virtual bool Teardown();            //Teardown
   private:
        bool active_ = false;
        std::string name_;
        std::string type_;
        std::string id_;

        Activatable::STATE state_ = Activatable::STATE::PASSIVE;
};

#endif //ACTIVATABLE_H