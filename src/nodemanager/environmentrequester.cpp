#include "environmentrequester.h"

EnvironmentRequester::EnvironmentRequester(const std::string& deployment_id, const std::string& deployment_info){
    deployment_id_ = deployment_id;
    deployment_info_ = deployment_info;
}

long EnvironmentRequester::Tick(){
    std::unique_lock<std::mutex>(clock_mutex_);
    clock_++;
    return clock_;
}

long EnvironmentRequester::SetClock(long incoming_clock){
    std::unique_lock<std::mutex>(clock_mutex_);
    clock_ = std::max(incoming_clock, clock_) + 1;
    return clock_;
}

long EnvironmentRequester::GetClock(){
    return clock_;
}

void EnvironmentRequester::Start(){

}

void EnvironmentRequester::End(){

}


int EnvironmentRequester::GetPort(const std::string& component_id, const std::string& component_info){

    int port = 0;

    auto response = SendRequest("ASSIGNMENT_REQUEST", component_id);

    port = std::stoi(response.get());

    return port;
}

std::future<std::string> EnvironmentRequester::SendRequest(const std::string& request_type, const std::string& request){

    std::promise<std::string>* request_promise = new std::promise<std::string>();
    std::future<std::string> request_future = request_promise->get_future();
    Request request_struct = {request_type, request, request_promise};

    std::unique_lock<std::mutex> lock(request_queue_lock_);
    request_queue_.push(request_struct);
    lock.unlock();
    request_queue_cv_.notify_one();
    return request_future;
}

