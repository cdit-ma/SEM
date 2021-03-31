#ifdef _WIN32
#define NOMINMAX
#endif

#include "logcontroller.h"

#include <algorithm>
#include <chrono>
#include <iostream>

#include "systemevent.pb.h"
#include "cachedprotowriter.h"
#include "monitor.h"
#include "systeminfohandler.h"
#include <google/protobuf/util/json_util.h>

using namespace sem::logging::netdata;

//Constructor used for print only call
LogController::LogController():
    listener_id_(system_.RegisterListener()),
    netdata_receiver_(std::make_unique<tcp_receiver>(3456)),
    json_parser_(std::make_unique<chart_json_parser>()),
    netdata_aggregator_(std::make_unique<device_sample_aggregator>())
{
    json_parser_->register_listener(netdata_aggregator_,
                                    protobuf::chart::nvidia_smi);
    netdata_receiver_->register_stream_listener(json_parser_);
}

std::string LogController::GetSystemInfoJson(){
    system_.Update();

    auto info = system_.GetSystemInfo(listener_id_);

    std::string output;
    google::protobuf::util::JsonOptions options;
    options.add_whitespace = true;

    if(info){
        google::protobuf::util::MessageToJsonString(*info, &output, options);
    }

    return output;
}

void LogController::Start(const std::string& publisher_endpoint, double frequency, const std::vector<std::string>& processes, const bool& live_mode){
    std::lock_guard<std::mutex> lock(future_mutex_);
    
    if(!logging_future_.valid()){
        {
            //Reset the interupt flag
            std::lock_guard<std::mutex> lock(interupt_mutex_);
            interupt_ = false;
        }
        
        //Validate the frequency
        frequency = std::min(10.0, frequency);
        frequency = std::max(1.0 / 60.0, frequency);

        //Ignore the system
        system_.ClearMonitoredProcesses();
        //Subscribe to our desired process names
        for(const auto& process_name : processes){
            system_.MonitorProcess(process_name);
        }

        std::promise<void> startup_promise;
        auto startup_future = startup_promise.get_future();
        
        logging_future_ = std::async(std::launch::async, &LogController::LogThread, this, publisher_endpoint, frequency, live_mode, std::move(startup_promise));

        if(startup_future.valid()){
            startup_future.get();
        }
    }
}

void LogController::Stop(){
    std::lock_guard<std::mutex> lock(future_mutex_);

    InteruptLogThread();
    if(logging_future_.valid()){
        logging_future_.get();
    }
}

void LogController::InteruptLogThread(){
    {
        std::lock_guard<std::mutex> lock(interupt_mutex_);
        interupt_ = true;
    }
    interupt_condition_.notify_all();
}

LogController::~LogController(){
    Stop();
}

void LogController::GotNewConnection(int a, std::string b){
    //Enqueue
    QueueOneTimeInfo();
}

void LogController::QueueOneTimeInfo(){
    send_onetime_info_ = true;
}

void LogController::LogThread(const std::string publisher_endpoint, const double frequency, const bool& live_mode, std::promise<void> startup_promise){
    std::unique_ptr<zmq::ProtoWriter> writer;
    if(live_mode){
        writer = std::unique_ptr<zmq::ProtoWriter>(new zmq::ProtoWriter());
    }else{
        writer = std::unique_ptr<zmq::ProtoWriter>(new zmq::CachedProtoWriter());
    }

    {
        {
            //Attach monitor
            writer->RegisterMonitorCallback(ZMQ_EVENT_ACCEPTED, std::bind(&LogController::GotNewConnection, this, std::placeholders::_1, std::placeholders::_2));
        }

        if(!writer->BindPublisherSocket(publisher_endpoint)){
            std::runtime_error error("Writer cannot bind publisher endpoint : '" + publisher_endpoint + "'");
            //Set our promise as exception and exit if we can't find a free port.
            startup_promise.set_exception(std::make_exception_ptr(error));
            throw error;
        }else{
            startup_promise.set_value();
        }
        
        //Get the duration in milliseconds

        const auto tick_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(1)) / frequency;

        //We need to sleep for at least 1 second, if our duration is less than 1 second, calculate the offset to get at least 1 second
        auto last_duration = tick_duration - std::chrono::milliseconds(1000);

        while(true){
            auto sleep_duration = tick_duration - last_duration;
            {
                std::unique_lock<std::mutex> lock(interupt_mutex_);
                interupt_condition_.wait_for(lock, sleep_duration, [&]{return interupt_.load();});
                if(interupt_){
                    break;
                }
            }

            auto start = std::chrono::steady_clock::now();
            
            //Whenever a new server connects, send one time information, using our client address as the topic
            if(send_onetime_info_){
                send_onetime_info_ = false;
                auto message = system_.GetSystemInfo(listener_id_);
                if(message){
                    if (netdata_aggregator_) {
                        auto device_data = netdata_aggregator_->retrieve_device_info(message->hostname());
                        if (device_data) {
                            auto message_samples = message->add_device_info();
                            message_samples->CopyFrom(*device_data);
                        }
                    }
                    writer->PushMessage(std::move(message));
                }
            }

            {
                system_.Update();
                //Send the tick'd information to all servers
                auto message = system_.GetSystemStatus(listener_id_);
                if(message){
                    if (netdata_aggregator_) {
                        auto device_data = netdata_aggregator_->retrieve_device_metrics(message->hostname());
                        if (device_data) {
                            auto message_samples = message->add_device_samples();
                            message_samples->CopyFrom(*device_data);
                        }
                    }
                    writer->PushMessage(std::move(message));
                }
            }

            //Calculate the duration we should sleep for
            auto end = std::chrono::steady_clock::now();
            last_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        }
        writer->Terminate();
        std::cout << "* Logged " << writer->GetTxCount() << " messages." << std::endl;
        writer.reset();
    }
}
