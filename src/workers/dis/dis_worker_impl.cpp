#include "dis_worker_impl.h"
#include <iostream>

Dis_Worker_Impl::Dis_Worker_Impl(){

}

Dis_Worker_Impl::~Dis_Worker_Impl(){
    Disconnect();
}

void Dis_Worker_Impl::Connect(const std::string& ip_address, const int port){
    std::lock_guard<std::mutex> lock(future_mutex_);
    if(!kdis_future_.valid() && !callback_future_.valid()){
        std::unique_ptr<KDIS::NETWORK::Connection> connection;
        try{
            connection = std::unique_ptr<KDIS::NETWORK::Connection>(new KDIS::NETWORK::Connection(ip_address, port, false, true));
        }catch(const std::exception& ex){
            throw std::runtime_error("Cannot create KDIS Connection: " + ip_address + " " + std::to_string(port) + ": " + ex.what());
        }
        //start async tasks for both threads
        kdis_future_ = std::async(std::launch::async, &Dis_Worker_Impl::ProcessEvents, this, std::move(connection));
        callback_future_ = std::async(std::launch::async, &Dis_Worker_Impl::ProcessQueue, this);
    }
}

void Dis_Worker_Impl::Disconnect(){
    std::lock_guard<std::mutex> lock(future_mutex_);
    if(kdis_future_.valid() && callback_future_.valid()){
        terminate_kdis_ = true;
        terminate_queue_ = true;
        queue_condition_.notify_all();
        kdis_future_.get();
        callback_future_.get();
    }
}

void Dis_Worker_Impl::SetPduCallback(std::function<void (const KDIS::PDU::Header &)> func){
    callback_function_ = func;
}

void Dis_Worker_Impl::ProcessEvents(std::unique_ptr<KDIS::NETWORK::Connection> connection){
    //Set max blocking time
    connection->SetBlockingTimeOut(0, 500000);
    connection->GetPDU_Factory()->AddFilter( new KDIS::UTILS::FactoryFilterExerciseID( 1 ) );

    while(terminate_kdis_.load() == false){
        try{
            // Note: GetNextPDU supports PDU Bundling, which Receive does not.
            auto pdu_ptr = connection->GetNextPDU().release();
            

            if(pdu_ptr){
                {
                    std::lock_guard<std::mutex> lock(queue_mutex_);
                    pdu_queue_.emplace(std::unique_ptr<KDIS::PDU::Header>(pdu_ptr));
                }
                queue_condition_.notify_all();
            }
        }catch(const std::exception & e){
            std::cout << e.what() << std::endl;
        }
    }
    connection.reset();
}

void Dis_Worker_Impl::ProcessQueue(){
    std::queue< std::unique_ptr<KDIS::PDU::Header> > swap_queue;
    
    while(true){
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_condition_.wait(lock, [this]{
                //Wake up if the termination flag has been set
                return terminate_queue_.load() || !pdu_queue_.empty();
            });
                
            //Swap out the queue's, and release the mutex
            pdu_queue_.swap(swap_queue);
            
            if(terminate_queue_.load()){
                //Shutdown
                break;
            }
        }
        
        while(swap_queue.empty() == false){
            auto pdu = std::move(swap_queue.front());
            swap_queue.pop();
            if(callback_function_){
                try{
                    callback_function_(*pdu);
                }catch(const std::exception& ex){

                }
            }
        }
    }
}
