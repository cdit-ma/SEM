#include <signal.h>
#include <boost/program_options.hpp>
#include <util/execution.hpp>
#include <iostream>

#include "cmakevars.h"
#include "logcontroller.h"

Execution execution;

void signal_handler (int signal_value){
    execution.Interrupt();
}


int main(int ac, char** av){

    // Set up string constants inside execution context
    const std::string program_name = "logan_server";
    // Pull logan version from cmakevars.h
    const std::string pretty_program_name = program_name + LOGAN_VERSION;

    //Handle the SIGINT/SIGTERM signal
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    //Parse command line options
    bool live_mode = false;
    std::vector<std::string> processes;
    double log_frequency = 1.0;
    std::string address;
    std::string port;
    boost::program_options::options_description desc(pretty_program_name + " Options");
    desc.add_options()("address,a", boost::program_options::value<std::string>(&address), "IP address of logging client node.");
    desc.add_options()("port,p", boost::program_options::value<std::string>(&port), "Local port to attach logging client to.");
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
    if(vm.count("system-info-print")){
        std::cout << log_controller.GetSystemInfoJson() << std::endl;
        return 0;
    }

    //Check for valid args
    bool valid_args = address.size() && port.size();

    //On invalid args or help arg, print options and exit.
    if(!valid_args || vm.count("help")){
        std::cout << desc << std::endl;
        std::cout << "Please ully qualified endpoint is specified (-a & -p)." << std::endl;
        return valid_args;
    }

    //Max 10x a second, Min 1 every minute
    std::string logger_endpoint = "tcp://" + address + ":" + port;
    {
        log_controller.Start(logger_endpoint, log_frequency, processes, live_mode);
     
        //Print output
        std::cout << "-------[ " + pretty_program_name + " ]-------" << std::endl;
        std::cout << "* Logging Endpoint: " << logger_endpoint << std::endl;
        std::cout << "* Logging Frequency: " << log_frequency << "Hz" << std::endl;
        std::cout << "* Logging Mode: " << (live_mode ? "Live" : "Cached") << std::endl;
        if(processes.size()){
            std::cout << "* Monitored Processes:" << std::endl;
            for(const auto& process : processes){
                std::cout << "** " << process << std::endl;
            }
        }
        std::cout << "-------------[ Logging ]-------------" << std::endl;
        std::cout << "* Starting logging." << std::endl;
        execution.Start();
        std::cout << "* Tearing down logging." << std::endl;
    }
    return 0;
}
