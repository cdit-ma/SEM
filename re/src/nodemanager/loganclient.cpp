#include "loganclient.h"
#include "logcontroller.h"
#include <iostream>

LoganClient::LoganClient(const std::string& id){
    set_name(id);
    logan_client_ = std::unique_ptr<LogController>(new LogController());
}

LoganClient::~LoganClient(){
    Activatable::Terminate();
    logan_client_.reset();
}

void LoganClient::HandleActivate(){
    logan_client_->Start(endpoint_, frequency_, processes_, live_mode_);
}

void LoganClient::HandleTerminate(){
    logan_client_->Stop();
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