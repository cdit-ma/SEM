#ifndef TAO_INEVENTPORT_H
#define TAO_INEVENTPORT_H

#include "../../core/eventports/ineventport.hpp"

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "tao/IORTable/IORTable.h"


namespace tao{

    template <class S, class R> class Receiver: public virtual R{
        public:
            Receiver(CORBA::ORB_ptr orb);
            void send(const S& message);
        private:
        // Use an ORB reference to shutdown the application.
        CORBA::ORB_var orb_;
    };

     template <class T, class S, class R> class InEventPort: public ::InEventPort<T>{
        public:
            InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function);
            void notify();

            void Startup(std::map<std::string, ::Attribute*> attributes);
            bool Teardown();

            bool Activate();
            bool Passivate();
            void send(const S& message);
        private:
            void receive_loop();
            
            std::thread* rec_thread_ = 0;

            std::mutex notify_mutex_;
            std::mutex control_mutex_;

            std::condition_variable notify_lock_condition_;

            Receiver<S, R>* receiver_;

            bool configured_ = false;
            bool passivate_ = false;

            std::string object_key_;
            std::vector<std::string> end_points_;
    }; 
};



template <class S, class R>
tao::Receiver<S, R>::Receiver(CORBA::ORB_ptr orb): orb_(CORBA::ORB::_duplicate (orb))
{
};

template <class S, class R>
void tao::Receiver<S, R>::send(const S& message){
    std::cout << "GOT MESSAGE" << std::endl;
};



template <class T, class S, class R>
void tao::InEventPort<T, S, R>::Startup(std::map<std::string, ::Attribute*> attributes){
    {
        std::lock_guard<std::mutex> lock(control_mutex_);
    }
};


template <class T, class S, class R>
bool tao::InEventPort<T, S, R>::Activate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    passivate_ = false;
    //if(configured_){
    rec_thread_ = new std::thread(&tao::InEventPort<T, S, R>::receive_loop, this);
    //}
    return ::InEventPort<T>::Activate();
};

template <class T, class S, class R>
bool tao::InEventPort<T, S, R>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    passivate_ = true;
    if(rec_thread_){
        //Unlock the rec thread
        notify();

        //Join our zmq_thread
        rec_thread_->join();
        delete rec_thread_;
        rec_thread_ = 0;
    }
    
    return ::InEventPort<T>::Passivate();
};


template <class T, class S, class R>
bool tao::InEventPort<T, S, R>::Teardown(){
    Passivate();

    std::lock_guard<std::mutex> lock(control_mutex_);
    configured_ = false;
    return ::InEventPort<T>::Teardown();
};


template <class T, class S, class R>
void tao::InEventPort<T, S, R>::notify(){
    //Called by the DataReaderListener to notify our InEventPort thread to get new data
    std::unique_lock<std::mutex> lock(notify_mutex_);
    notify_lock_condition_.notify_all();
};


template <class T, class S, class R>
tao::InEventPort<T, S, R>::InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function):
::InEventPort<T>(component, name, callback_function, "tao"){
};

template <class T, class S, class R>
void tao::InEventPort<T, S, R>::receive_loop(){
    //"-ORBInitRef", "LoggingServer=corbaloc:iiop:192.168.111.90:50001/LoggingServer"
    
    char *argv[] = {"program name", "-ORBEndpoint", "iiop://192.168.111.90:50002", NULL};
    int argc = sizeof(argv) / sizeof(char*) - 1;

    std::cout << "INIT" << std::endl;
    
    //Initialize the orb
    auto orb = CORBA::ORB_init (argc, argv, "qweqsadsdsd");

    //Get the reference to the RootPOA
    auto obj = orb->resolve_initial_references("RootPOA");
    auto root_poa = ::PortableServer::POA::_narrow(obj);

    std::cout << "ROOT" << std::endl;

    // Construct the policy list for the LoggingServerPOA.
    CORBA::PolicyList policies (2);
    policies.length (2);
    policies[0] = root_poa->create_id_assignment_policy (PortableServer::USER_ID);
    policies[1] = root_poa->create_lifespan_policy (PortableServer::PERSISTENT);

    // Get the POAManager of the RootPOA.
    PortableServer::POAManager_var poa_manager = root_poa->the_POAManager();

    // Create the child POA for the test logger factory servants.
    auto child_poa = root_poa->create_POA("LoggingServerPOA", root_poa->the_POAManager(), policies);

     // Destroy the POA policies
    for (::CORBA::ULong i = 0; i < policies.length (); ++ i){
        policies[i]->destroy();
    }

    receiver_ = new Receiver<S, R>(orb);

    /*
    // Activate object WITH OUT ID
    PortableServer::ObjectId_var myObjID = child_poa->activate_object(hello_impl);
    // Get a CORBA reference with the POA through the servant
    CORBA::Object_var o = child_poa->servant_to_reference(hello_impl);
    // The reference is converted to a character string
    CORBA::String_var ior = orb->object_to_string(o);
    */

    //Activate WITH ID
    //Convert our string into an object_id
    CORBA::OctetSeq_var obj_id = PortableServer::string_to_ObjectId ("Stock_Factory");
    //Activate the object with the obj_id
    child_poa->activate_object_with_id(obj_id, receiver_);
    //Get the reference to the obj, using the obj_id
    auto obj_ref = child_poa->id_to_reference (obj_id);
    //Get the IOR from the object
    auto ior = orb->object_to_string (obj_ref);

    //Register with the IOR Table
    //Get the IORTable for the application
    auto temp = orb->resolve_initial_references ("IORTable");
    //Cast into concrete class
    auto ior_table = IORTable::Table::_narrow (temp);

    if(!ior_table){
        std::cerr << "Failed to resolve IOR Table" << std::endl;
    }

    std::cout << ior << std::endl;

    //Bind the IOR file into the IOR table
    ior_table->bind("LoggingServer", ior);

    //Activate the POA
    poa_manager->activate();
    std::cout << "ACTIVATED?!" << std::endl;


    std::cout << "RUNNING!" << std::endl;
    //Run the ORB
    orb->run();
    orb->destroy();
    std::cout << "END!" << std::endl;

};

#endif //tao_INEVENTPORT_H
