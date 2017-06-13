#ifndef TAO_OUTEVENTPORT_H
#define TAO_OUTEVENTPORT_H

#include "../../core/eventports/outeventport.hpp"
#include "../../core/modellogger.h"
#include <string>
#include <mutex>
#include <iostream>
#include <thread>
#include <list>

namespace tao{
     template <class T, class S, class R> class OutEventPort: public ::OutEventPort<T>{
        public:
            OutEventPort(Component* component, std::string name);
            void tx(T* message);

            void Startup(std::map<std::string, ::Attribute*> attributes);
            bool Teardown();

            bool Activate();
            bool Passivate();
        private:
            void setup_loop();
            std::thread* setup_thread_ = 0;

            std::mutex control_mutex_;

            bool configured_ = false;
            bool activated_ = false;

            std::string object_key_;
            std::vector<std::string> end_points_;


            std::list<R*> writers_;
            R* writer_ = 0;
    }; 
};

template <class T, class S, class R>
void tao::OutEventPort<T, S, R>::tx(T* message){
    //if(this->is_active() && writer_ != 0){
        
        //De-reference the message and send
        for(auto writer : writers_){
            try{
                auto m = tao::translate(message);
                writer->send(*m);
                delete m;
            }catch(...){
                //Exception!
            }
        }
        
        ::OutEventPort<T>::tx(message);
    //}
};

template <class T, class S, class R>
tao::OutEventPort<T, S, R>::OutEventPort(Component* component, std::string name):
::OutEventPort<T>(component, name, "tao"){};

template <class T, class S, class R>
void tao::OutEventPort<T, S, R>::Startup(std::map<std::string, ::Attribute*> attributes){
    std::lock_guard<std::mutex> lock(control_mutex_);

    std::cout << "tao::OutEventPort" << std::endl;
};

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::Teardown(){
    Passivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    configured_ = false;
    return ::OutEventPort<T>::Teardown();
};

template <class T, class S, class R>
void tao::OutEventPort<T, S, R>::setup_loop(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    //Magic setup
    

    //char * arg[size * 2 + 1];

    char *argv[] = {"-ORBInitRef", "LoggingServer=corbaloc:iiop:192.168.111.90:50002/LoggingServer", "-ORBInitRef", "LoggingServer2=corbaloc:iiop:192.168.111.90:50003/LoggingServer2"};
    int argc = sizeof(argv) / sizeof(char*) - 1;


    //Get a pointer to the orb
    auto orb = CORBA::ORB_init (argc, argv, "UNIUQUE");

    std::cout <<"ARGC: " << argc << std::endl;;
    std::cout << "RESOLVE?" << std::endl;

    //Keep trying

    //Get the reference to the RootPOA
    auto obj = orb->resolve_initial_references("RootPOA");

    std::cout << "RESOLVED" << std::endl;
    
    auto root_poa = ::PortableServer::POA::_narrow(obj);

    std::string reference_str = "LoggingServer";

    std::cout << "RESOLVE2?" << std::endl;

    std::list<std::string> references = {"LoggingServer", "LoggingServer2"};
    

    
    while(true){
        try{
            for(auto r : references){
                std::cout << "Resolving: " << r << std::endl;
                CORBA::Object_var ref_obj = orb->resolve_initial_references(r.c_str());
                if(!ref_obj){
                    std::cerr << "Failed to resolve Reference '" << r << "'" << std::endl;
                }else{
                    //Convert the ref_obj into a typed writer
                    auto writer_ = R::_narrow(ref_obj);
                    if(writer_){
                        std::cout << "FOT REFERENCE: "  << r << std::endl;
                        writers_.push_back(writer_);
                        references.remove(r);
                        break;
                    }
                }
            }
        }
        catch(...){
            std::cout << "Exception:" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "RESOLVED2" << std::endl;
    ::OutEventPort<T>::Activate();
    activated_ = true;
}

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::Activate(){
    {
        std::lock_guard<std::mutex> lock(control_mutex_);
        if(activated_){
            return true;
        }
    }
    //Only construct if we aren't activated.
    setup_thread_ = new std::thread(&tao::OutEventPort<T, S, R>::setup_loop, this);
};

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    writer_ = 0;
    return ::OutEventPort<T>::Passivate();
};

#endif //TAO_OUTEVENTPORT_H
