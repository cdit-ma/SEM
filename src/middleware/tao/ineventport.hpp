#ifndef TAO_INEVENTPORT_H
#define TAO_INEVENTPORT_H

#include "../../core/eventports/ineventport.hpp"

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <sstream>

#include "tao/IORTable/IORTable.h"

//Inspired by
//https://www.codeproject.com/Articles/24863/A-Simple-C-Client-Server-in-CORBA

namespace tao{
    template <class S, class R> class Receiver: public R{
        public:
            Receiver(CORBA::ORB_ptr orb, std::function<void (const S*) > callback);
            void send(const S& message);
        private:
            std::function<void (const S*) > callback_function_;
            //Use an ORB reference to shutdown the application.
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
            void enqueue(const S* message);
            void receive_loop();
            std::queue<const S*> message_queue_;
            
            std::thread* rec_thread_ = 0;
            Receiver<S, R>* receiver_ = 0;

            std::mutex receive_mutex_;
            std::mutex control_mutex_;
            std::condition_variable receive_lock_condition_;

            bool configured_ = false;
            bool passivate_ = false;


            std::string object_key_;
            std::string end_point_;
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
        //We need to make a copy of the message
        //The ORB destroys the message when it falls out of scope
        //The tao::InEventPort is responsible to free memory
        auto tao_message = new S(message);
        callback_function_(tao_message);
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
    //Gain the control mutex
    std::lock_guard<std::mutex> lock(control_mutex_);
    //Only run if we can activate
    if(::InEventPort<T>::Activate()){
        //Construct a thread to handle TAO
        rec_thread_ = new std::thread(&tao::InEventPort<T, S, R>::receive_loop, this);
        return true;
    }
    return false;
};

template <class T, class S, class R>
bool tao::InEventPort<T, S, R>::Passivate(){
    //Gain the control mutex so we can notify the lock condition
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    if(::InEventPort<T>::Passivate()){
        {
            //Gain the receive mutex
            std::unique_lock<std::mutex> lock(receive_mutex_);
            receive_lock_condition_.notify_all();
        }
        if(rec_thread_){
            //Join our recieve thread
            rec_thread_->join();
            delete rec_thread_;
            rec_thread_ = 0;
        }
        return true;
    }
    return false;
};

template <class T, class S, class R>
bool tao::InEventPort<T, S, R>::Teardown(){
    Passivate();
    return ::InEventPort<T>::Teardown();
};

template <class T, class S, class R>
tao::InEventPort<T, S, R>::InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function):
::InEventPort<T>(component, name, callback_function, "tao"){
    object_key_ = "TestServer";
    end_point_ = "iiop://192.168.111.90:50002";
};

template <class T, class S, class R>
void tao::InEventPort<T, S, R>::receive_loop(){
    //Construct a unique ID
    std::stringstream ss;
    ss << "TAO_INEVENTPORT_" << "_" << std::this_thread::get_id();
    auto unique_id = ss.str();

    //Construct the args for the TAO orb
    int orb_argc = 0;
    auto orb_argv = new char*[2];
    orb_argv[orb_argc++] = (char*)"-ORBEndpoint";
    orb_argv[orb_argc++] = &(end_point_[0]);
    
    //Initialize the orb with our custom endpoints
    auto orb = CORBA::ORB_init(orb_argc, orb_argv, unique_id.c_str());

    //Get the reference to the RootPOA
    auto root_poa_ref = orb->resolve_initial_references("RootPOA");
    auto root_poa = ::PortableServer::POA::_narrow(root_poa_ref);

    //Construct a list for the policies
    CORBA::PolicyList policies(2);
    policies.length(2);
    //We are specifying a known ID
    policies[0] = root_poa->create_id_assignment_policy (PortableServer::USER_ID);
    //We also want the IOR to remain persistant
    policies[1] = root_poa->create_lifespan_policy (PortableServer::PERSISTENT);

    //Get the POAManager of the RootPOA.
    PortableServer::POAManager_var poa_manager = root_poa->the_POAManager();

    //Create the child POA
    auto child_poa = root_poa->create_POA(unique_id.c_str(), poa_manager, policies);

    //Destroy the policy elements
    for (auto i = 0; i < policies.length (); i ++){
        policies[i]->destroy();
    }

    //Construct a callback function (Lambda) to call enqueue message
    auto callback = [=](const S * s){enqueue(s);};
    //Construct a Receiver which implements the TAO interface
    auto receiver = new Receiver<S, R>(orb, callback);

    //Convert the object key string into an object_id
    CORBA::OctetSeq_var obj_id = PortableServer::string_to_ObjectId(object_key_.c_str());
    //Activate the receiver we instantiated with the obj_id
    child_poa->activate_object_with_id(obj_id, receiver);
    //Get the reference to the obj, using the obj_id
    auto obj_ref = child_poa->id_to_reference(obj_id);
    //Get the IOR from the object
    auto ior = orb->object_to_string(obj_ref);

    //Get the IORTable for the orb
    auto ior_ref = orb->resolve_initial_references("IORTable");
    //Cast into concrete IORTable class
    auto ior_table = IORTable::Table::_narrow(ior_ref);


    if(!ior_table){
        std::cerr << "Failed to resolve IOR Table" << std::endl;
        return;
    }

    //Bind the IOR into the IOR table, so that others can look it up
    ior_table->bind(object_key_.c_str(), ior);

    //Activate the POA
    poa_manager->activate();
    
    //Define a queue to swap
    std::queue<const S*> queue;

    bool running = true;
    while(running){
        {
            //Gain Mutex
            std::unique_lock<std::mutex> lock(receive_mutex_);
            running = this->is_active();
            if(running){
                //Only wait for the next notify if we are running
                receive_lock_condition_.wait(lock);
            }
            //Swap out the queue's and release the mutex (Even if we are in active)
            message_queue_.swap(queue);
        }

        //Process the queue
        while(!queue.empty()){
            //Take first element
            auto tao_message = queue.front();
            //Translate between TAO and base middleware
            auto base_message = translate(tao_message);
            //Enqueue the Base middleware message into the EventPort super class
            this->EnqueueMessage(base_message);
            //Pop the queue
            queue.pop();
            //Free the TAO message
            delete tao_message;
        }
    }
    
    //Shutdown the ORB
    orb->destroy();
    delete receiver;
};

#endif //TAO_INEVENTPORT_H
