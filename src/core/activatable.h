#ifndef ACTIVATABLE_H
#define ACTIVATABLE_H

#include <string>
#include <map>
#include <mutex>
#include <condition_variable>

#include "attribute.h"


class ModelLogger;

class Activatable{
    enum class STATE{
        PASSIVE = 0,
        ACTIVE = 1,
        DEAD = 2,
    };

    public: 
        virtual ~Activatable();
        const std::string get_name();
        const std::string get_id();
        const std::string get_type();
        void set_name(std::string name);
        void set_id(std::string id);
        void set_type(std::string type);
        Activatable::STATE get_state();
        bool is_active();
        ModelLogger* logger();

        virtual bool Activate();            //Start Component (Start Middleware etc)
        virtual bool Passivate();           //Stop Component 
        virtual bool Teardown();            //Teardown
        
        
        void StartupFinished();
        void WaitForActivate();
        void WaitForStartup();
        std::shared_ptr<Attribute> GetAttribute(std::string name);
   private:
        bool active_ = false;
        bool startup_finished_ = false;

        std::string name_;
        std::string type_;
        std::string id_;

        std::mutex state_mutex_;
        std::mutex startup_mutex_;
        std::mutex attributes_mutex_;
        std::condition_variable activate_condition_;
        std::condition_variable startup_condition_;

        Activatable::STATE state_ = Activatable::STATE::PASSIVE;

        std::map<std::string, std::shared_ptr<Attribute> > attributes_;
   protected:
        std::shared_ptr<Attribute> AddAttribute(std::shared_ptr<Attribute> attribute);
};

#endif //ACTIVATABLE_H