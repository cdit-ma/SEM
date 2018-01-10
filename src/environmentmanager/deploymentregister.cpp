#include "deploymentregister.h"
#include <iostream>
#include <chrono>
#include <exception>

const std::string DeploymentRegister::SUCCESS = "SUCCESS";
const std::string DeploymentRegister::ERROR = "ERROR";

DeploymentRegister::DeploymentRegister(const std::string& ip_addr, const std::string& registration_port){
    ip_addr_ = ip_addr;
    registration_port_ = registration_port;

    context_ = new zmq::context_t(1);
}

void DeploymentRegister::Start(){
    registration_loop_ = new std::thread(&DeploymentRegister::RegistrationLoop, this);
}

void DeploymentRegister::AddDeployment(const std::string& port_no, const std::string& deployment_info){
    std::unique_lock<std::mutex> lock(register_mutex_);
    std::cout << "Add deployment: " << deployment_info << " on port: " << port_no << std::endl;
    deployment_map_[port_no] = deployment_info;
}

void DeploymentRegister::RemoveDeployment(const std::string& port_no){
    std::unique_lock<std::mutex> lock(register_mutex_);
    std::cout << "Remove deployment: " << deployment_map_[port_no] << " on port: " << port_no << std::endl;
    deployment_map_.erase(port_no);
}

std::string DeploymentRegister::GetDeploymentInfo() const{
    std::string out;

    for(auto element : deployment_map_){
        out += " " + element.second;
    }
    return out;
}

std::string DeploymentRegister::GetDeploymentInfo(const std::string& name) const{
    throw new std::bad_function_call;
    return "";
}

void DeploymentRegister::RegistrationLoop(){

    zmq::socket_t* rep = new zmq::socket_t(*context_, ZMQ_REP);

    try{
        rep->bind(TCPify(ip_addr_, registration_port_));
    }
    catch(zmq::error_t& e){
        throw new std::invalid_argument("Could not bind reply socket in registration loop. IP_ADDR: " + ip_addr_ +
                                        " Port: " + registration_port_);
    }
    
    while(true){
        //Receive deployment information
        zmq::message_t request_type_msg;
        zmq::message_t request_contents_msg;

        rep->recv(&request_type_msg);
        rep->recv(&request_contents_msg);

        auto request_type = std::string(static_cast<const char*>(request_type_msg.data()), request_type_msg.size());
        auto request_contents = std::string(static_cast<const char*>(request_contents_msg.data()), request_contents_msg.size());

        if(request_type.compare("DEPLOYMENT") == 0){
            //Push work onto new thread with port number promise
            std::promise<std::string> port_promise;
            std::future<std::string> port_future = port_promise.get_future();
            std::string port;

            auto hb_thread = new std::thread(&DeploymentRegister::HeartbeatLoop, this, std::move(port_promise));
            hb_threads_.push_back(hb_thread);

            try{
                //Wait for port assignment from heartbeat loop, .get() will throw if out of ports.
                port = port_future.get();
            }
            catch(const std::exception& e){
                std::cout << "Exception: " << e.what() << " (Probably out of ports)" << std::endl;

                std::string error_msg("Exception thrown by deployment register: ");
                error_msg += e.what();

                SendTwoPartReply(rep, ERROR, error_msg);

                //Continue with no state change
                continue;
            }
            AddDeployment(port, request_contents);

            //Reply with enpoint to send heartbeats and status updates to
            SendTwoPartReply(rep, SUCCESS, TCPify(ip_addr_, port));
        }

        else if(request_type.compare("QUERY") == 0){
            std::string info = GetDeploymentInfo();
            SendTwoPartReply(rep, SUCCESS, info);
        }

        else{
            std::cout << "Recieved unknown request type: " << request_type << std::endl;
            SendTwoPartReply(rep, ERROR, "Unknown request type");
        }
    }
}

void DeploymentRegister::QueryLoop(){
    throw new std::bad_function_call;
    while(true){
    }
}

void DeploymentRegister::HeartbeatLoop(std::promise<std::string> port_promise){
    zmq::socket_t hb_soc(*context_, ZMQ_REP);

    std::string assigned_port;
    try{
        //Bind to random port on local ip address
        hb_soc.bind(TCPify(ip_addr_, "*"));

        //Retrieve bound port
        char sockopt[1024];
        size_t size = sizeof(sockopt);
        hb_soc.getsockopt(ZMQ_LAST_ENDPOINT, &sockopt, &size);
        std::string sockopt_str(sockopt);

        //Extract port number from endpoint string "tcp://###.###.###.###:####"
        assigned_port = sockopt_str.substr(sockopt_str.find_last_of(":") + 1);

        port_promise.set_value(assigned_port);

    }catch(zmq::error_t ex){
        //Set our promise as exception and exit if we can't find a free port.
        port_promise.set_exception(std::current_exception());
        return;
    }
    std::string ack_str("ACK");

    //Initialise our poll item list
    zmq::pollitem_t item = {hb_soc, 0, ZMQ_POLLIN, 0};
    std::vector<zmq::pollitem_t> sockets = {item};

    //Wait for first heartbeat, allow more time for this one in case of server congestion
    int initial_events = zmq::poll(sockets, INITIAL_TIMEOUT);

    if(initial_events >= 1){
        zmq::message_t initial_message;
        hb_soc.recv(&initial_message);
        zmq::message_t initial_ack(ack_str.begin(), ack_str.end());
        hb_soc.send(initial_ack);
    }
    //Break out early if we never get our first heartbeat
    else{
        RemoveDeployment(assigned_port);
        return;
    }

    //Wait for heartbeats
    while(true){
        zmq::message_t hb_message;

        //Poll zmq socket for heartbeat message, time out after HEARTBEAT_TIMEOUT milliseconds
        int events = zmq::poll(sockets, HEARTBEAT_TIMEOUT);

        if(events < 1){
            break;
        }

        hb_soc.recv(&hb_message);

        zmq::message_t ack(ack_str.begin(), ack_str.end());
        hb_soc.send(ack);
    }

    //Broken out of heartbeat loop at this point, remove deployment
    RemoveDeployment(assigned_port);
}

std::string DeploymentRegister::TCPify(const std::string& ip_address, const std::string& port) const{
    return std::string("tcp://" + ip_address + ":" + port);

}

std::string DeploymentRegister::TCPify(const std::string& ip_address, int port) const{
    return std::string("tcp://" + ip_address + ":" + std::to_string(port));
}

void DeploymentRegister::SendTwoPartReply(zmq::socket_t* socket, const std::string& part_one,
                                                                 const std::string& part_two){
    zmq::message_t part_one_msg(part_one.begin(), part_one.end());
    zmq::message_t part_two_msg(part_two.begin(), part_two.end());

    try{
        socket->send(part_one_msg, ZMQ_SNDMORE);
        socket->send(part_two_msg);
    }
    catch(std::exception e){
        std::cout << e.what() << std::endl;
    }
}
