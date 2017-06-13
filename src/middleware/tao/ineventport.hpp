#ifndef TAO_INEVENTPORT_H
#define TAO_INEVENTPORT_H

#include "../../core/eventports/ineventport.hpp"

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include "tao/IORTable/IORTable.h"

namespace tao{

    template <class S, class R> class Receiver: public R{
        public:
            Receiver(CORBA::ORB_ptr orb, std::function<void (const S*) > callback);
            void send(const S& message);
        private:
            std::function<void (const S*) > callback_function_;
            // Use an ORB reference to shutdown the application.
            CORBA::ORB_var orb_;
    };

     template <class T, class S, class R> class InEventPort: public ::InEventPort<T>{
        public:
            InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function);

            void Startup(std::map<std::string, ::Attribute*> attributes);
            bool Teardown();

            bool Activate();
            bool Passivate();
        private:
            //call back function
            void enqueue(const S* message);
            
            void receive_loop();
            
            std::thread* rec_thread_ = 0;
            Receiver<S, R>* receiver_ = 0;

            std::mutex receive_mutex_;
            std::mutex control_mutex_;
            std::condition_variable receive_lock_condition_;


            std::queue<const S*> message_queue_;



            bool configured_ = false;
            bool passivate_ = false;

            std::string object_key_;
            std::vector<std::string> end_points_;
    }; 
};

template <class S, class R>
tao::Receiver<S, R>::Receiver(CORBA::ORB_ptr orb, std::function<void (const S*) > callback):
    orb_(CORBA::ORB::_duplicate (orb)){
        callback_function_ = callback;
};

template <class S, class R>
void tao::Receiver<S, R>::send(const S& message){
    //Callback into the InEventPort
    if(callback_function_){
        //Take a copy, so we can cache it like kings
        //Needs to be deleted after we deal with it
        auto copy = new S(message);
        callback_function_(copy);
    }
};

template <class T, class S, class R>
void tao::InEventPort<T, S, R>::enqueue(const S* message){
    //Gain mutex lock and append message to queue
    std::unique_lock<std::mutex> lock(receive_mutex_);
    message_queue_.push(message);
    receive_lock_condition_.notify_all();
}


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
    
    {
        //Gain the notification mutex
        std::unique_lock<std::mutex> lock(receive_mutex_);
        passivate_ = true;
        receive_lock_condition_.notify_all();
    }

    if(rec_thread_){
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
tao::InEventPort<T, S, R>::InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function):
::InEventPort<T>(component, name, callback_function, "tao"){
};

template <class T, class S, class R>
void tao::InEventPort<T, S, R>::receive_loop(){
    //Get Unique id
    std::stringstream ss;
    ss << __func__ << "_" << std::this_thread::get_id();
    auto unique_id = ss.str();

    std::list<std::string> endpoints_ = {"iiop://192.168.111.90:50002"};

    int argc = 0;
    auto argv = new char*[endpoints_.size() * 2];

    for(auto &endpoint : endpoints_){
        argv[argc++] = "-ORBEndpoint";
        argv[argc++] = &(endpoint[0]);
    }
    
    //Initialize the orb
    auto orb = CORBA::ORB_init(argc, argv, unique_id.c_str());

    //for(auto &endpoint : endpoints_){
    //    std::cout << endpoint << std::endl;
    //}

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

    auto callback = [=](const S * s){enqueue(s);};
    auto receiver = new Receiver<S, R>(orb, callback);

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
    child_poa->activate_object_with_id(obj_id, receiver);
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
    
    //Process the queue
    std::queue<const S*> queue_;
    while(true){
        {
            //Gain Mutex
            std::unique_lock<std::mutex> lock(receive_mutex_);
            //Wait for the next notify
            receive_lock_condition_.wait(lock);
            
            //Terminate on passivation
            if(passivate_){
                //break out
                break;
            }
            //Swap out the queue's and release the mutex
            message_queue_.swap(queue_);
        }

        //Process the queue
        while(!queue_.empty()){
            auto m = queue_.front();
            auto m2 = translate(m);
            this->EnqueueMessage(m2);
            queue_.pop();
            //Free memory lads
            delete m;
        }
    }
    //Destroy
    orb->destroy();
    delete receiver;
    std::cout << "Teardown reciever" << std::endl;
};

#endif //tao_INEVENTPORT_H
