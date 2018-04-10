#include "environmentrequester.h"
#include "executionparser/protobufmodelparser.h"
#include <iostream>

int main(int argc, char** argv){


    std::string model_path(argv[1]);
    ProtobufModelParser parser(model_path);

    auto message = parser.ControlMessage();


    EnvironmentRequester* requester = new EnvironmentRequester("tcp://192.168.111.230:22334", message->experiment_id());

    requester->Init();
    requester->Start();
    
    std::this_thread::sleep_for(std::chrono::seconds(1));


    auto response = requester->AddDeployment(*message);


    std::cout << response.DebugString() << std::endl;


    requester->End();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    delete requester;

    return 0;
}
