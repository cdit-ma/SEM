#include <zmq.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <climits>
#include <tuple>

#include "deploymentrule.h"
#include "deploymentgenerator.h"
#include "environment.h"
#include "deploymentrules/zmq/zmqrule.h"
#include "deploymentrules/dds/ddsrule.h"
#include "deploymentrules/tao/taorule.h"

#include "../nodemanager/executionparser/protobufmodelparser.h"

// REVIEW (Mitch): What is this and why is it still here?
int main(int argc, char **argv){


    std::string model_path(argv[1]);
    ProtobufModelParser parser(model_path, "experiment_id");
    std::cout << parser.GetDeploymentJSON() << std::endl;

    auto message = parser.ControlMessage();

    //std::cout << message->DebugString() << std::endl;

    Environment* environment = new Environment();

    environment->AddExperiment(message->experiment_id());

    DeploymentGenerator generator(*environment);
    generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Zmq::DeploymentRule(*environment)));
    generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Dds::DeploymentRule(*environment)));
    generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Tao::DeploymentRule(*environment)));

    generator.PopulateDeployment(*message);


    std::cout << "==============================================" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << std::endl;

    std::cout << message->DebugString() << std::endl;

    return 0;
}
