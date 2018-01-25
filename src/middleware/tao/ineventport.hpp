#ifndef TAO_INEVENTPORT_H
#define TAO_INEVENTPORT_H

#include <core/eventports/ineventport.hpp>

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
            InEventPort(std::weak_ptr<Component> component, std::string name, std::function<void (T&) > callback_function);
            ~InEventPort(){
                Activatable::Terminate();
            };
            protected:
                bool HandleConfigure();
                bool HandlePassivate();
                bool HandleTerminate();
            private:
                void enqueue(const S* message);
                void recv_loop();


            private:
                std::queue<const S*> message_queue_;
                std::thread* rec_thread_ = 0;
                Receiver<S, R>* receiver_ = 0;

                std::mutex receive_mutex_;
                std::condition_variable receive_lock_condition_;

                std::mutex thread_state_mutex_;
                ThreadState thread_state_;
                std::condition_variable thread_state_condition_;
                
                bool interupt_ = false;
                std::mutex control_mutex_;
                std::thread* recv_thread_ = 0;

                std::shared_ptr<Attribute> publisher_name_;
                std::shared_ptr<Attribute> publisher_address_;
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
tao::InEventPort<T, S, R>::InEventPort(std::weak_ptr<Component> component, std::string name, std::function<void (T&) > callback_function):
::InEventPort<T>(component, name, callback_function, "tao"){
    publisher_address_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "publisher_address").lock();
    publisher_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "publisher_name").lock();
};


template <class T, class S, class R>
bool tao::InEventPort<T, S, R>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    bool valid = publisher_name_->String().length() >= 0 && publisher_address_->String().length() >= 0;
    if(valid && ::InEventPort<T>::HandleConfigure()){
        if(!recv_thread_){
            {
                std::unique_lock<std::mutex> lock(receive_mutex_);
                interupt_ = false;
            }
            std::unique_lock<std::mutex> lock(thread_state_mutex_);
            thread_state_ = ThreadState::WAITING;
            recv_thread_ = new std::thread(&tao::InEventPort<T, S, R>::recv_loop, this);
            thread_state_condition_.wait(lock, [=]{return thread_state_ != ThreadState::WAITING;});
            std::cout << "YOYOOYOY" << std::endl;
            return thread_state_ == ThreadState::STARTED;
        }
    }
    return false;
};

template <class T, class S, class R>
void tao::InEventPort<T, S, R>::enqueue(const S* message){
    //Gain mutex lock and append message to queue
    std::unique_lock<std::mutex> lock(receive_mutex_);
    message_queue_.push(message);
    receive_lock_condition_.notify_all();
}



template <class T, class S, class R>
bool tao::InEventPort<T, S, R>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::HandlePassivate()){ 
        //Set the terminate state in the passivation
        std::lock_guard<std::mutex> lock(receive_mutex_);
        interupt_ = true;
        receive_lock_condition_.notify_all();
        return true;
    }
    return false;
};


template <class T, class S, class R>
bool tao::InEventPort<T, S, R>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::HandleTerminate()){
        if(recv_thread_){
            //Join our tect thread
            recv_thread_->join();
            delete recv_thread_;
            recv_thread_ = 0;
        }
        return true;
    }
    return false;
};

template <class T, class S, class R>
void tao::InEventPort<T, S, R>::recv_loop(){
    auto state = ThreadState::STARTED;
    
    //Construct a unique ID
    std::stringstream ss;
    ss << "tao::InEventPort" << "_" << std::this_thread::get_id();
    auto unique_id = ss.str();


    std::string publisher_address = publisher_address_->String();
    auto publisher_address_cstr = publisher_address.c_str();
    //Construct the args for the TAO orb
    int orb_argc = 0;
    auto orb_argv = new char*[2];
    orb_argv[orb_argc++] = (char*) "-ORBEndpoint";
    orb_argv[orb_argc++] = &(publisher_address[0]);
    
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
    CORBA::OctetSeq_var obj_id = PortableServer::string_to_ObjectId(publisher_name_->String().c_str());
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
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Failed to resolve IOR Table"));
        state = ThreadState::ERROR_;
    }else{
        //Bind the IOR into the IOR table, so that others can look it up
        ior_table->bind(publisher_name_->String().c_str(), ior);

        //Activate the POA
        poa_manager->activate();
    }

    std::cout << "HELLO" << std::endl;
    
    
    //Define a queue to swap
    std::queue<const S*> queue;

    //Change the state to be Configured
    {
        std::cout << "CIONFIGURE" << std::endl;
        std::lock_guard<std::mutex> lock(thread_state_mutex_);
        std::cout << "CIONFIGURE2" << std::endl;
        thread_state_ = state;
        thread_state_condition_.notify_all();
    }


    bool run = true;
    while(run){
        {
            //Wait for next message
            std::unique_lock<std::mutex> lock(receive_mutex_);
            //Check to see if we've been interupted before sleeping the first time
            if(interupt_){
                break;
            }
            receive_lock_condition_.wait(lock);
            
            //Upon wake, read all messages, then die.
            if(interupt_){
                run = false;
            }

            //Swap out the queue's and release the mutex (Even if we are in active)
            message_queue_.swap(queue);
        }

        //Process the queue
        while(!queue.empty()){
            //Take first element
            auto tao_message = queue.front();
            //Translate between TAO and base middleware
            auto base_message = translate(*tao_message);
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
