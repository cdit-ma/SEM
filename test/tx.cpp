#include <memory>
#include <iostream>
#include <chrono>
#include <thread>
#include <list>
#include <algorithm>

#include <re_common/zmq/protorequester/protorequester.hpp>
#include <proto/controlmessage/controlmessage.pb.h>
#include "../zmq/zmqutils.hpp"


int main(int argc, char** argv){
    std::string function_name("lelbs");
    std::string function2_name("lelbs2");
    std::string address("tcp://192.168.111.187:13373");

    auto proto_requester = std::unique_ptr<zmq::ProtoRequester>(new zmq::ProtoRequester(address));

    std::list< std::future< std::unique_ptr< NodeManager::ControlMessage> > > requests;
    int i = 0;
    int sent = 0;
    auto start = std::chrono::steady_clock::now();
    while(++i <= 200){
        //std::this_thread::sleep_for(std::chrono::milliseconds(2));

        NodeManager::Node nm;
        NodeManager::ControlMessage cm;
        cm.set_host_name("Testi Bois");

        auto f = proto_requester->SendRequest<NodeManager::Node, NodeManager::ControlMessage>(function_name, nm, 20000);
        requests.push_back(std::move(f));
        sent ++;
    }

    auto tx_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    std::cerr << "Sent " << sent << " Requests In: " <<  tx_time.count() << std::endl;

    
    int success = 0;
    int failed = 0;
    int ded = 0;
    int timeout = 0;

    for(auto& f : requests){
        try{
            auto response = f.get();
            success ++;
        }catch(const zmq::TimeoutException& ex){
            timeout ++;
        }catch(const zmq::RMIException & ex){
            failed ++;
        }
        catch(const std::exception& ex){
            ded ++;
        }
    }
    auto rx_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);

    std::cerr << "Sent " << sent << " Requests In: " <<  tx_time.count() << std::endl;
    std::cerr << "RXd Requests In: " <<  rx_time.count() << std::endl;
    std::cerr << "Sent: " << sent << std::endl;
    std::cerr << "success: " << success << std::endl;
    std::cerr << "RMI FAILED: " << failed << std::endl;
    std::cerr << "ded: " << ded << std::endl;
    std::cerr << "timeout: " << timeout << std::endl;
    return 0;
}