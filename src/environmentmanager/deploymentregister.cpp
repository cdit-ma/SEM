#include "deploymentregister.h"
#include <iostream>
#include <chrono>
#include <exception>
#include <src/nodemanager/controlmessage/controlmessage.pb.h>

const std::string DeploymentRegister::SUCCESS = "SUCCESS";
const std::string DeploymentRegister::ERROR = "ERROR";

DeploymentRegister::DeploymentRegister(const std::string& ip_addr, const std::string& registration_port){
    ip_addr_ = ip_addr;
    registration_port_ = registration_port;

    auto hint_iterator = available_ports_.begin();
    for(int i = PORT_RANGE_MIN; i < PORT_RANGE_MAX; i++){
        available_ports_.insert(hint_iterator, i);
        hint_iterator++;
    }

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

//Handle request for deployment information at specific location or of specific component name
std::string DeploymentRegister::HandleQuery(const std::string& request){
    std::string response("");
    std::unique_lock<std::mutex> lock(register_mutex_);

    //TODO: Handle requests without hardcoding
    if(size_t pos = request.find("PORT:") != std::string::npos){
        std::string port_no = request.substr(pos+4);

        if(deployment_map_.count(port_no)){
            response = deployment_map_.at(port_no);
        }
        else{
            response = "Error: Port not found";
        }
    }
    return response;
}

//Main registration loop, passes request workload off to other threads
//NOTHROW
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

        //Handle new deployment manager contact
        if(request_type.compare("DEPLOYMENT") == 0){
            //Push work onto new thread with port number promise
            std::promise<std::string> port_promise;
            std::future<std::string> port_future = port_promise.get_future();
            std::string port;

            HandleDeployment(std::move(port_promise), request_contents);
            
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

            //Reply with enpoint to send heartbeats and status updates to
            SendTwoPartReply(rep, SUCCESS, TCPify(ip_addr_, port));
        }

        else if(request_type.compare("QUERY") == 0){
            std::string response = HandleQuery(request_contents);
            SendTwoPartReply(rep, SUCCESS, response);
        }

        else{
            std::cout << "Recieved unknown request type: " << request_type << std::endl;
            SendTwoPartReply(rep, ERROR, "Unknown request type");
        }
    }
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

void DeploymentRegister::HandleDeployment(std::promise<std::string> port_promise, const std::string& info){
    auto handler_thread = new std::thread(&DeploymentRegister::DeploymentHandler, this, std::move(port_promise), info);
    handler_threads_.push_back(handler_thread);
}

void DeploymentRegister::DeploymentHandler(std::promise<std::string> port_promise, const std::string& info){
    zmq::socket_t* handler_socket = new zmq::socket_t(*context_, ZMQ_REP);

    std::string assigned_port;
    try{
        //Bind to random port on local ip address
        handler_socket->bind(TCPify(ip_addr_, "*"));

        //Retrieve bound port
        char sockopt[1024];
        size_t size = sizeof(sockopt);
        handler_socket->getsockopt(ZMQ_LAST_ENDPOINT, &sockopt, &size);
        std::string sockopt_str(sockopt);

        //Extract port number from endpoint string "tcp://###.###.###.###:####"
        assigned_port = sockopt_str.substr(sockopt_str.find_last_of(":") + 1);

        port_promise.set_value(assigned_port);

    }catch(zmq::error_t ex){
        //Set our promise as exception and exit if we can't find a free port.
        port_promise.set_exception(std::current_exception());
        return;
    }


    //Infinitely loop while deployment requests ports
    while(true){

        //Expecting two part message, type + contents
        zmq::message_t request_type_msg;
        zmq::message_t request_contents_msg;
        handler_socket->recv(&request_type_msg);
        handler_socket->recv(&request_contents_msg);

        std::string request_type(static_cast<const char*>(request_type_msg.data()), request_type_msg.size());
        std::string request_contents(static_cast<const char*>(request_contents_msg.data()), request_contents_msg.size());

        if(request_type.compare("ASSIGNMENT_REQUEST") == 0){

            std::string component_id;

            std::string header("ASSIGNMENT_REPLY");
            std::string port_string = AssignPort(component_id);
            std::string response(port_string);

            SendTwoPartReply(handler_socket, header, response);
        }

        else if(request_type.compare("END_ASSIGNMENT") == 0){
            break;
        }

        //Unrecognised message type
        else{
            std::cout << "ERROR: Unrecognised message type passed to DeploymentHandler" << std::endl;
            SendTwoPartReply(handler_socket, "ERROR", "Unrecognised message type passed to DeploymentHandler");
        }
    }

    //Start heartbeat to track liveness of deployment
    //Use heartbeat to receive any updates re. components addition/removal
    HeartbeatLoop(handler_socket);

}

void DeploymentRegister::HeartbeatLoop(zmq::socket_t* hb_socket){
    
    std::string ack_str("ACK");

    //Initialise our poll item list
    std::vector<zmq::pollitem_t> sockets = {{hb_socket, 0, ZMQ_POLLIN, 0}};

    //Wait for first heartbeat, allow more time for this one in case of server congestion
    int initial_events = zmq::poll(sockets, INITIAL_TIMEOUT);

    if(initial_events >= 1){
        zmq::message_t initial_message;
        hb_socket->recv(&initial_message);
        zmq::message_t initial_ack(ack_str.begin(), ack_str.end());
        hb_socket->send(initial_ack);
    }
    //Break out early if we never get our first heartbeat
    else{
        RemoveDeployment(assigned_port);
        return;
    }

    int interval = INITIAL_INTERVAL;
    int liveness = HEARTBEAT_LIVENESS;

    //Wait for heartbeats
    while(true){
        zmq::message_t hb_message;

        //Poll zmq socket for heartbeat message, time out after HEARTBEAT_TIMEOUT milliseconds
        int events = zmq::poll(sockets, HEARTBEAT_INTERVAL);

        if(events >= 1){
            //Reset
            liveness = HEARTBEAT_LIVENESS;
            interval = INITIAL_INTERVAL;
            hb_socket->recv(&hb_message);

            zmq::message_t ack(ack_str.begin(), ack_str.end());
            hb_socket->send(ack);
        }
        else if(--liveness == 0){
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if(interval < MAX_INTERVAL){
                interval *= 2;
            }
            else{
                break;
            }
            liveness = HEARTBEAT_LIVENESS;
        }
    }

    //Broken out of heartbeat loop at this point, remove deployment
    RemoveDeployment(assigned_port);
}

std::string DeploymentRegister::AssignPort(const std::string& component_id){
    std::unique_lock<std::mutex> lock(port_mutex_);
    //Get first available port, store then erase it
    auto it = available_ports_.begin();
    int next_port = *it;
    available_ports_.erase(it);

    component_port_map_[component_id] = std::to_string(next_port);
    return std::to_string(next_port);
}

void DeploymentRegister::UnassignPort(const std::string& component_id){
    std::unique_lock<std::mutex> lock(port_mutex_);

    int port = std::stoi(component_port_map_[component_id]);
    component_port_map_.erase(component_id);

    //Return port to available port set
    available_ports_.insert(port);

}
