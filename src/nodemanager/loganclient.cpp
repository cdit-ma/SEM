#include "loganclient.h"
#include "logan/src/client/logcontroller.h"

LoganClient::LoganClient(const std::string& id){
    set_name(id);
    logan_client_ = std::unique_ptr<LogController>(new LogController());
}

LoganClient::~LoganClient(){
    Activatable::Terminate();
}

bool LoganClient::HandleActivate(){
    logan_client_->Start(endpoint_, frequency_, processes_, live_mode_);
    return true;
}

bool LoganClient::HandlePassivate(){
    return true;
}

bool LoganClient::HandleConfigure(){
    return true;
}


bool LoganClient::HandleTerminate(){
    logan_client_->Stop();
    return true;
}

void LoganClient::SetEndpoint(const std::string& address, const std::string& port){
    if(address.size() && port.size()){
        endpoint_ = "tcp://" + address + ":" + port;
    }
}

void LoganClient::SetFrequency(double frequency){
    frequency_ = frequency;
}

void LoganClient::SetProcesses(const std::vector<std::string>& processes){
    for(const auto& process : processes){
        AddProcess(process);
    }
}

void LoganClient::AddProcess(const std::string& process){
    processes_.emplace_back(process);
}

void LoganClient::SetLiveMode(bool live_mode){
    live_mode_ = live_mode;
}