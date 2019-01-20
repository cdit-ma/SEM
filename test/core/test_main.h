#pragma once

#include "gtest/gtest.h"
#include <boost/program_options.hpp>
#include <comms/environmentcontroller/environmentcontroller.h>

namespace cditma{
  static std::string environment_manager_endpoint;
  static std::string node_name;
  std::once_flag request_qpid;
  std::once_flag request_tao;
  const std::string& GetQpidBrokerAddress();
  const std::string& GetTaoNamingServerAddress();
  const std::string& GetNodeName(){
      return node_name;
  }
};

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    
    boost::program_options::options_description options("Test Options");
    using namespace cditma;
    options.add_options()("environment-manager,e", boost::program_options::value<std::string>(&environment_manager_endpoint)->required(), "TCP endpoint of Environment Manager to connect to.");
    options.add_options()("node-name,n", boost::program_options::value<std::string>(&node_name), "Name of the node");

    try{
        boost::program_options::variables_map vm;
        auto parsed_args = boost::program_options::command_line_parser(argc, argv).options(options).allow_unregistered().run();
        boost::program_options::store(parsed_args, vm);
        boost::program_options::notify(vm);
    }catch(...){
        environment_manager_endpoint = "127.0.0.1:20000";
        node_name = "";
        std::cerr << "* Environment-manager flag not set, trying: " << environment_manager_endpoint << std::endl;
    }
    return RUN_ALL_TESTS();
}

namespace cditma{
    const std::string& GetQpidBrokerAddress(){
        static std::string endpoint;

        std::call_once(request_qpid, [](){
            try{
                EnvironmentManager::EnvironmentController controller(environment_manager_endpoint);
                endpoint = controller.GetQpidBrokerEndpoint();
            }catch(const std::exception& ex){
                std::cerr << ex.what() << std::endl;
            }
        });
        
        if(endpoint.size()){
            return endpoint;
        }

        throw std::runtime_error("Cannot GetQpidBrokerAddress");
    };

    const std::string& GetTaoNamingServerAddress(){
        static std::string endpoint;

        std::call_once(request_tao, [](){
            try{
                EnvironmentManager::EnvironmentController controller(environment_manager_endpoint);
                endpoint = controller.GetTaoCosnamingBrokerEndpoint();
            }catch(const std::exception& ex){
                std::cerr << ex.what() << std::endl;
            }
        });
        
        if(endpoint.size()){
            return endpoint;
        }

        throw std::runtime_error("Cannot GetTaoNamingServerAddress");
    };
};