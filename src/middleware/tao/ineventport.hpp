#ifndef TAO_INEVENTPORT_H
#define TAO_INEVENTPORT_H

#include <core/eventports/ineventport.hpp>

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <middleware/tao/helper.h>

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
                
                CORBA::ORB_var orb_ = 0;

                bool interupt_ = false;
                std::mutex control_mutex_;
                std::thread* recv_thread_ = 0;

                std::shared_ptr<Attribute> publisher_name_;
                std::shared_ptr<Attribute> orb_endpoint_;
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
    orb_endpoint_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "orb_endpoint").lock();
    publisher_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "publisher_name").lock();
};


template <class T, class S, class R>
bool tao::InEventPort<T, S, R>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    bool valid = orb_endpoint_->String().length() >= 0 && publisher_name_->String().length() >= 0;
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
    try{
        auto state = ThreadState::STARTED;
        
        
        auto orb_endpoint  = orb_endpoint_->String();
        auto helper = tao::TaoHelper::get_tao_helper();
        std::cout << "ORB ENDPOINT: " << orb_endpoint << std::endl;
        orb_ = helper->get_orb(orb_endpoint);

        std::cout << "ORB ENDPOINT: " << orb_endpoint << std::endl;

        R* reader_impl = 0;
        if(!orb_){
            state = ThreadState::TERMINATE;
        }else{
            auto publisher_name  = publisher_name_->String();

            std::cout << "Publisher: " << publisher_name << std::endl;
            //Construct a callback function (Lambda) to call enqueue message
            //Construct a Receiver which implements the TAO interface
            reader_impl = new Receiver<S, R>(orb_, [=](const S * s){enqueue(s);});

            // Create the child POA for the test logger factory servants.
            auto poa = helper->get_poa(orb_, publisher_name);
            
            //Activate WITH ID
            //Convert our string into an object_id
            helper->register_servant(orb_, poa, reader_impl, publisher_name);
        
            poa->the_POAManager()->activate();
        }

        //Change the state to be Configured
        {
            std::lock_guard<std::mutex> lock(thread_state_mutex_);
            thread_state_ = state;
            thread_state_condition_.notify_all();
        }

        //Define a queue to swap
        std::queue<const S*> queue;
        
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

        PortableServer::POA_var poa = reader_impl->_default_POA ();
        PortableServer::ObjectId_var id = poa->servant_to_id (reader_impl);
        poa->deactivate_object (id.in ());
        poa->destroy (true, true);
    }catch(...){
        std::cerr << "exceptiopn" << std::endl;
    }
};

#endif //TAO_INEVENTPORT_H
