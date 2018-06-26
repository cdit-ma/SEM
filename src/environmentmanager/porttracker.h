#ifndef ENVIRONMENT_MANAGER_PORTTRACKER_H
#define ENVIRONMENT_MANAGER_PORTTRACKER_H

#include "uniquequeue.hpp"
#include <unordered_map>
#include <mutex>

namespace EnvironmentManager{
struct EventPort{
    std::string id;
    std::string guid;
    std::string type;
    std::string node_ip;
    std::string port_number;
    std::string topic;

    //list of publisher ids
    std::vector<std::string> connected_ports;

    std::string endpoint;

};
class PortTracker{
    public:
        PortTracker(const std::string& ip_address, unique_queue<int> port_set){
            ip_ = ip_address;
            available_ports_ = port_set;
        }
        std::string GetPort(){
            std::unique_lock<std::mutex> lock(port_mutex_);
            if(available_ports_.empty()){
                return "";
            }
            auto port = available_ports_.front();
            available_ports_.pop();
            auto port_str =  std::to_string(port);
            std::cout << "Port Acquired: " << name_ << " : " << port_str << std::endl;
            return port_str;
        };

        void FreePort(const std::string& port){
            std::unique_lock<std::mutex> lock(port_mutex_);
            int port_number;
            try{
                port_number = std::stoi(port);
                if(available_ports_.push(port_number)){
                    std::cout << "Port Freed: " << name_ << " : " << port << std::endl;
                }
            }
            catch(const std::invalid_argument& ex){
                std::cerr << "Could not free port <\'" << port <<"\'>, port string could not be converted to int." << std::endl;
                std::cerr << ex.what() << std::endl;
            }
            catch(const std::out_of_range& ex){
                std::cerr << "Could not free port, port # out of range for int." << std::endl;
                std::cerr << ex.what() << std::endl;
            }
            catch(...){
                std::cerr << "Unknown exception thrown in Node::FreePort" << std::endl;
            }
        }
        std::string GetName(){
            return name_;
        };

        void SetName(const std::string& name){
            name_ = name;
        };
        std::string GetIp(){
            return ip_;
        };
        void IncrementComponentCount(){
            component_count_++;
        }
    private:
        std::string name_;
        std::string ip_;
        int component_count_ = 0;
        std::mutex port_mutex_;
        unique_queue<int> available_ports_;

        //eventport guid -> port number assigned
        std::unordered_map<std::string, std::string> used_ports_;
};
};

#endif //ENVIRONMENT_MANAGER_PORTTRACKER_H