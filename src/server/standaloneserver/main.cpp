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
#include <iostream>
#include <vector>
#include <boost/program_options.hpp>

#include "cmakevars.h"

#include "../server.h"




std::mutex mutex_;
std::condition_variable lock_condition_;

std::string DEFAULT_FILE = "out.sql";

void signal_handler(int sig)
{
    //Gain the lock so we can notify to terminate
    std::unique_lock<std::mutex> lock(mutex_);
    lock_condition_.notify_all();
}

int main(int ac, char** av)
{
    //Connect the SIGINT/SIGTERM signals to our handler.
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    //Variables to store the input parameters
    std::string database_path;
    std::string experiment_id;
    std::vector<std::string> client_addresses;

    //Parse command line options
    boost::program_options::options_description desc(LONG_VERSION " Options");
    desc.add_options()("clients,c", boost::program_options::value<std::vector<std::string> >(&client_addresses)->multitoken()->required(), "logan_client endpoints to register against (ie tcp://192.168.1.1:5555)");
    desc.add_options()("database,d", boost::program_options::value<std::string>(&database_path)->default_value(DEFAULT_FILE), "Output SQLite Database file path.");
    desc.add_options()("help,h", "Display help");

    //Construct a variable_map
    boost::program_options::variables_map vm;

    try{
        //Parse Argument variables
        boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);
        boost::program_options::notify(vm);
    }catch(boost::program_options::error& e) {
        std::cerr << "Arg Error: " << e.what() << std::endl << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }

    //Print output
    std::cout << "-------[ " LONG_VERSION " ]-------" << std::endl;
    std::cout << "* Database: " << database_path << std::endl;
    for(int i = 0; i < client_addresses.size(); i++){
        if(i == 0){
            std::cout << "* Clients:" << std::endl;
        }
        std::cout <<"** " << client_addresses[i] << std::endl;
    }
    std::cout << "---------------------------------" << std::endl;

    {
        //Construct a Server to interface between our ZMQ messaging infrastructure and SQLite
        Server server(database_path, client_addresses);
        server.Start();

        std::cout << "* Started Logging." << std::endl;
        std::unique_lock<std::mutex> lock(mutex_);
        //Wait for the signal_handler to notify for exit
        lock_condition_.wait(lock);
        std::cout << "* Stopping Logging." << std::endl;
    }
    return 0;
}
