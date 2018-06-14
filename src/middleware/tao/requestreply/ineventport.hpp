#ifndef TAO_PORT_REPLIER_HPP
#define TAO_PORT_REPLIER_HPP

#include <core/threadmanager.h>
#include <core/ports/requestreply/replierport.hpp>
#include <middleware/tao/taohelper.h>

//Inspired by
//https://www.codeproject.com/Articles/24863/A-Simple-C-Client-Server-in-CORBA

namespace tao{

    template <class TaoReplyType, class TaoRequestType> class PortBroker{
        public:
            virtual TaoReplyType enqueue(const S& message) = 0;
    };

    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType>
    class ReplierPort : public ::ReplierPort<BaseReplyType, BaseRequestType>{

    template <class T, class S, class R> class InEventPort: public ::InEventPort<T>, public Enqueuer<S>{
        public:
            InEventPort(std::weak_ptr<Component> component, std::string name, std::function<void (T&) > callback_function);
            ~InEventPort(){
                Activatable::Terminate();
            };

            void enqueue(const S& message);

            protected:
                bool HandleConfigure();
                bool HandlePassivate();
                bool HandleTerminate();
            private:
                void recv_loop();


            private:
                ::Base::Translator<T, S> translater;

                R* recv = 0;
                std::queue<const S*> message_queue_;
                std::thread* rec_thread_ = 0;

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
void tao::InEventPort<T, S, R>::enqueue(const S& message){
    //Gain mutex lock and append message to queue
    std::unique_lock<std::mutex> lock(receive_mutex_);
    message_queue_.push(new S(message));
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
        auto& helper = tao::TaoHelper::get_tao_helper();
        std::cout << "IN ORB ENDPOINT: " << orb_endpoint << std::endl;
        orb_ = helper.get_orb(orb_endpoint);

        std::cout << "IN ORB ENDPOINT: " << orb_endpoint << std::endl;

        //R* reader_impl = 0;
        if(!orb_){
            state = ThreadState::TERMINATE;
            std::cerr << "CAN'T GET ORB" << std::endl;
        }else{
            auto publisher_name  = publisher_name_->String();

            std::cout << "IN Publisher: " << publisher_name << std::endl;
            //Construct a callback function (Lambda) to call enqueue message

            // Create the child POA for the test logger factory servants.
            auto poa = helper.get_poa(orb_, publisher_name);


            recv = new R(*this);
            
            //Activate WITH ID
            //Convert our string into an object_id
            helper.register_servant(orb_, poa, recv, publisher_name);
        
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
                std::cerr<< "LOL" << std::endl;
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
                auto base_message = translater.MiddlewareToBase(*tao_message);
                //Enqueue the Base middleware message into the EventPort super class
                this->EnqueueMessage(base_message);
                //Pop the queue
                queue.pop();
                //Free the TAO message
                delete tao_message;
            }
        }

        PortableServer::POA_var poa = recv->_default_POA ();
        PortableServer::ObjectId_var id = poa->servant_to_id (recv);
        poa->deactivate_object (id.in ());
        poa->destroy (true, true);
    }catch(...){
        std::cerr << "exceptiopn" << std::endl;
    }
};


#endif //TAO_INEVENTPORT_H
