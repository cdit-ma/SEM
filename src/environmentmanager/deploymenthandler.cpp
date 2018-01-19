#include "deploymenthandler.h"

DeploymentHandler::DeploymentHandler(const Environment* env, zmq::context_t* context, std::promise<std::string> port_promise, const std::string& info){

    environment_ = env;
    info_ = info;
    port_promise_ = port_promise;
    handler_thread_ = new std::thread(&DeploymentHandler::Init, this);
}

void DeploymentHandler::Init(){
    handler_socket_ = new zmq::socket_t(*context_, ZMQ_REP);

    std::string assigned_port;
    try{
        //Bind to random port on local ip address
        handler_socket_->bind(TCPify(ip_addr_, "*"));

        //Retrieve bound port
        char sockopt[1024];
        size_t size = sizeof(sockopt);
        handler_socket_->getsockopt(ZMQ_LAST_ENDPOINT, &sockopt, &size);
        std::string sockopt_str(sockopt);

        //Extract port number from endpoint string "tcp://###.###.###.###:####"
        assigned_port = sockopt_str.substr(sockopt_str.find_last_of(":") + 1);


        port_promise_.set_value(assigned_port);

    }catch(zmq::error_t ex){
        //Set our promise as exception and exit if we can't find a free port.
        port_promise_.set_exception(std::current_exception());
        return;
    }

    //Infinitely loop while deployment requests ports
    while(true){

        //Expecting two part message, type + contents
        zmq::message_t request_type_msg;
        zmq::message_t request_contents_msg;
        handler_socket_->recv(&request_type_msg);
        handler_socket_->recv(&request_contents_msg);

        std::string request_type(static_cast<const char*>(request_type_msg.data()), request_type_msg.size());
        std::string request_contents(static_cast<const char*>(request_contents_msg.data()), request_contents_msg.size());

        if(request_type.compare("ASSIGNMENT_REQUEST") == 0){

            std::string component_id;

            std::string header("ASSIGNMENT_REPLY");
            std::string port_string = AssignPort(component_id);
            std::string response(port_string);

            SendTwoPartReply(handler_socket_, header, response);
        }

        else if(request_type.compare("END_ASSIGNMENT") == 0){
            break;
        }

        //Unrecognised message type
        else{
            std::cout << "ERROR: Unrecognised message type passed to DeploymentHandler" << std::endl;
            SendTwoPartReply(handler_socket_, "ERROR", "Unrecognised message type passed to DeploymentHandler");
        }
    }

    //Start heartbeat to track liveness of deployment
    //Use heartbeat to receive any updates re. components addition/removal
    HeartbeatLoop();
}

void DeploymentHandler::HeartbeatLoop(){

    std::string ack_str("ACK");

    //Initialise our poll item list
    std::vector<zmq::pollitem_t> sockets = {{handler_socket_, 0, ZMQ_POLLIN, 0}};

    //Wait for first heartbeat, allow more time for this one in case of server congestion
    int initial_events = zmq::poll(sockets, INITIAL_TIMEOUT);

    if(initial_events >= 1){
        zmq::message_t initial_message;
        handler_socket_->recv(&initial_message);
        zmq::message_t initial_ack(ack_str.begin(), ack_str.end());
        handler_socket_->send(initial_ack);
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
            handler_socket_->recv(&hb_message);

            zmq::message_t ack(ack_str.begin(), ack_str.end());
            handler_socket_->send(ack);
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
