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

#include <signal.h>
#include <condition_variable>
#include <mutex>
#include <string>
#include <functional>
#include <boost/program_options.hpp>
#include <iostream>

#include "cmakevars.h"

#include "logcontroller.h"

std::condition_variable lock_condition_;
std::mutex mutex_;

void signal_handler (int signal_value){
    //Gain the lock so we can notify to terminate
    std::unique_lock<std::mutex> lock(mutex_);
    lock_condition_.notify_all();
}


int main(int ac, char** av){

    //Handle the SIGINT/SIGTERM signal
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    //Parse command line options
    bool live_mode = false;
    std::vector<std::string> processes;
    double log_frequency = 1.0;
    std::string address;
    std::string port;
    std::string environment_manager_address;
    std::string experiment_id;
    boost::program_options::options_description desc(LONG_VERSION " Options");
    desc.add_options()("address,a", boost::program_options::value<std::string>(&address), "IP address of logging client node.");
    desc.add_options()("port,p", boost::program_options::value<std::string>(&port), "Local port to attach logging client to.");
    desc.add_options()("experiment-id,n", boost::program_options::value<std::string>(&experiment_id), "Experiment id, if using environment manager.");
    desc.add_options()("frequency,f", boost::program_options::value<double>(&log_frequency)->default_value(log_frequency), "Logging frequency (Hz)");
    desc.add_options()("process,P", boost::program_options::value<std::vector<std::string> >(&processes)->multitoken(), "Process names to log (ie logan_client)");
    desc.add_options()("live-mode,l", boost::program_options::value<bool>(&live_mode)->default_value(live_mode), "Produce data live");
    desc.add_options()("system-info-print,s", "Print system info then exit");
    desc.add_options()("help,h", "Display help");

    boost::program_options::variables_map vm;
    //Catch invalid options
    try{
        boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);
        boost::program_options::notify(vm);
    }catch(boost::program_options::error& e) {
        std::cerr << "Arg Error: " << e.what() << std::endl << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }
    LogController log_controller;

    //Print system info json and exit.
    if(vm.count("system_info_print")){
        std::cout << log_controller.GetSystemInfoJson() << std::endl;
        return 0;
    }

    //Check for valid args
    //Need address at all times AND ( (environment manager address AND experiment id) OR a port).
    bool valid_args = !address.empty() && ((!environment_manager_address.empty() && !experiment_id.empty()) || !port.empty());

    //On invalid args or help arg, print options and exit.
    if(!valid_args || vm.count("help")){
        std::cout << desc << std::endl;
        std::cout << "If no fully qualified endpoint is specified (-a & -p), a fully qualified environment manager endpoint and experiment-id must be (-a & -e & -n)." << std::endl;
        return valid_args;
    }


    std::string logger_endpoint = "tcp://" + address + ":" + port;
    {
        //Print output
        std::cout << "-------[ " LONG_VERSION " ]-------" << std::endl;
        std::cout << "* Logging Endpoint: " << logger_endpoint << std::endl;
        std::cout << "* Logging Frequency: " << log_frequency << "Hz" << std::endl;
        std::cout << "* Logging Mode: " << (live_mode ? "Live" : "Cached") << std::endl;
        for(int i = 0; i < processes.size(); i++){
            if(i == 0){
                std::cout << "* Monitored Processes:" << std::endl;
            }
            std::cout << "** " << processes[i] << std::endl;
        }
        std::cout << "-------------[ Logging ]-------------" << std::endl;

        if(!log_controller.Start(logger_endpoint, log_frequency, processes, live_mode)){
            return 1;
        }

        {
            std::cout << "* Starting logging." << std::endl;
            std::unique_lock<std::mutex> lock(mutex_);
            //Wait for the signal_handler to notify for exit
            lock_condition_.wait(lock);
        }


        if(!log_controller.Stop()){
            
            return 1;
        }
    }
    return 0;
}
