#ifndef ACTIVATABLE_H
#define ACTIVATABLE_H

#include <string>
#include <map>
/*
struct Attribute{
    enum ATTRIBUTE_TYPE{
        AT_STRING = 0,
        AT_INTEGER = 1,
        AT_BOOLEAN = 2,
        AT_DOUBLE = 3
    };
    ATTRIBUTE_TYPE type;
    std::string name;
    std::string value;
};*/

class Activatable{  
    public:
        virtual bool activate();            //Start Component (Start Middleware etc)
        virtual bool passivate();           //Stop Component 
        //virtual bool startup(std::map<std::string, Attribute> attributes); //Configure Component
        //virtual bool teardown();


        const bool is_active();
        //const bool is_started();
   private:
        bool active_ = false;
        //bool started_ = false;
};

#endif //ACTIVATABLE_H