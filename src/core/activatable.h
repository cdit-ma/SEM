#ifndef ACTIVATABLE_H
#define ACTIVATABLE_H

#include <string>
#include <map>

class ModelLogger;

class Activatable{  
    public:
        const std::string get_name();
        void set_name(std::string name);
        virtual bool Activate();            //Start Component (Start Middleware etc)
        virtual bool Passivate();           //Stop Component 
        //virtual bool Startup(std::map<std::string, Attribute> attributes); //Configure Component
        //virtual bool Teardown();

        ModelLogger* logger();
        const bool is_active();
        //const bool is_started();
   private:
        bool active_ = false;
        std::string name_;
        //bool started_ = false;
};

#endif //ACTIVATABLE_H