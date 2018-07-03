/* logan
 * Copyright (C) 2016-2017 The University of Adelaide
 *
 * This file is part of "logan"
 *
 * "logan" is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * "logan" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
 
#ifdef _WIN32
#define NOMINMAX
#endif

#include "logcontroller.h"

#include <chrono>
#include <iostream>
#include <algorithm>

#include "sigarsysteminfo.h"

#include <re_common/proto/systemstatus/systemstatus.pb.h>
#include <re_common/zmq/protowriter/cachedprotowriter.h>
#include <re_common/zmq/protowriter/monitor.h>
#include <google/protobuf/util/json_util.h>

//Constructor used for print only call
LogController::LogController():
    system_(SigarSystemInfo::GetSystemInfo()),
    listener_id_(system_.RegisterListener())
{

}

std::string LogController::GetSystemInfoJson(){
    //Let sigar do its thing for 1 second
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    system_.Update();

    auto info = system_.GetSystemInfo(listener_id_);

    std::string output;
    google::protobuf::util::JsonOptions options;
    options.add_whitespace = true;

    if(info){
        google::protobuf::util::MessageToJsonString(*info, &output, options);
        delete info;
    }

    return output;
}

void LogController::Start(const std::string& publisher_endpoint, double frequency, const std::vector<std::string>& processes, const bool& live_mode){
    std::lock_guard<std::mutex> lock(state_mutex_);
    if(!logging_future_.valid()){
        //Validate the frequency
        frequency = std::min(10.0, frequency);
        frequency = std::max(1.0 / 60.0, frequency);

        std::lock_guard<std::mutex> lock(interupt_mutex_);
        interupt_ = false;
        logging_future_ = std::async(std::launch::async, &LogController::LogThread, this, publisher_endpoint, frequency, processes, live_mode);
    }
}

void LogController::Stop(){
    std::lock_guard<std::mutex> lock(state_mutex_);
    if(logging_future_.valid()){
        InteruptLogThread();
        logging_future_.get();
    }
}

void LogController::InteruptLogThread(){
    std::lock_guard<std::mutex> lock(interupt_mutex_);
    interupt_ = true;
    log_condition_.notify_all();
}

LogController::~LogController(){
    Stop();
}

void LogController::GotNewConnection(int, std::string){
    //Enqueue
    QueueOneTimeInfo();
}

void LogController::QueueOneTimeInfo(){
    std::lock_guard<std::mutex> lock(one_time_mutex_);
    send_onetime_info_ = true;
}

void LogController::LogThread(const std::string& publisher_endpoint, const double& frequency, const std::vector<std::string>& processes, const bool& live_mode){
    auto writer = live_mode ? std::unique_ptr<zmq::ProtoWriter>(new zmq::ProtoWriter()) : std::unique_ptr<zmq::ProtoWriter>(new zmq::CachedProtoWriter());
    {
        {
            //Attach monitor
            auto monitor = std::unique_ptr<zmq::Monitor>(new zmq::Monitor());
            monitor->RegisterEventCallback(std::bind(&LogController::GotNewConnection, this, std::placeholders::_1, std::placeholders::_2));
            writer->AttachMonitor(std::move(monitor), ZMQ_EVENT_ACCEPTED);
        }

        if(!writer->BindPublisherSocket(publisher_endpoint)){
            std::cerr << "Writer cannot bind publisher endpoint '" << publisher_endpoint << "'" << std::endl;
            return;
        }
        
        //Get the duration in milliseconds
        auto tick_duration = std::chrono::milliseconds(static_cast<int>(1000.0 / std::max(0.0, frequency)));

        system_.ignore_processes();
        //Subscribe to our desired process names
        for(const auto& process_name : processes){
            system_.monitor_processes(process_name);
        }

        //We need to sleep for at least 1 second, if our duration is less than 1 second, calculate the offset to get at least 1 second
        auto last_duration = std::min(std::chrono::milliseconds(-1000) + tick_duration, std::chrono::milliseconds(0));

        double last_update = 0;
        while(true){
            auto sleep_duration = tick_duration - last_duration;
            {
                std::unique_lock<std::mutex> lock(interupt_mutex_);
                log_condition_.wait_for(lock, sleep_duration, [this]{return interupt_;});
                
                if(interupt_){
                    break;
                }
            }

            auto start = std::chrono::steady_clock::now();
            {
                //Whenever a new server connects, send one time information, using our client address as the topic
                std::lock_guard<std::mutex> lock(one_time_mutex_);
                if(send_onetime_info_){
                    auto message = system_.GetSystemInfo(listener_id_);
                    if(message){
                        writer->PushMessage(message);
                        send_onetime_info_ = false;
                    }
                }
            }

            {
                system_.Update();
                //Send the tick'd information to all servers
                
                auto message = system_.GetSystemStatus(listener_id_);
                if(message){
                    writer->PushMessage(message);
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
