#include "environmentcontroller.h"
#include <boost/program_options.hpp>
#include <google/protobuf/util/json_util.h>
#include <iostream>
#include <vector>

std::string ExperimentListToJson(const std::vector<std::string>& experiment_names)
{
    std::ostringstream s_stream;
    s_stream << "{\"experiment_names\":["
             << "\n";

    for(int i = 0; i < experiment_names.size(); i++) {
        s_stream << "\t"
                 << "\"";
        s_stream << experiment_names.at(i);
        s_stream << "\"";
        if(i != experiment_names.size() - 1) {
            s_stream << ",";
        }
        s_stream << "\n";
    }
    s_stream << "]}";
    return s_stream.str();
}

int main(int argc, char** argv)
{
    std::string environment_manager_endpoint;
    std::string experiment_name;
    std::string graphml_path;
    std::string experiment_uuid;

    int experiment_duration = -1;

    boost::program_options::options_description desc("Environment Manager Controller Options");
    desc.add_options()("experiment-name,n",
                       boost::program_options::value<std::string>(&experiment_name),
                       "Name of experiment.");
    desc.add_options()(
        "environment-manager,e",
        boost::program_options::value<std::string>(&environment_manager_endpoint)->required(),
        "TCP endpoint of Environment Manager to connect to.");
    desc.add_options()("shutdown-experiment,s", "Shutdown experiment <experiment-name> use with "
                                                "<regex> for regex matching.");
    desc.add_options()("get-qpid-broker-endpoint,q", "Get the registered qpid broker endpoint.");
    desc.add_options()("get-tao-cosnaming-endpoint,t", "Get the registered TAO CosNaming "
                                                       "endpoint.");
    desc.add_options()("regex,r", "Use Regex Matching for Shutdown experiment.");
    desc.add_options()("add-experiment,a",
                       boost::program_options::value<std::string>(&graphml_path),
                       "Deployment graphml file path.");
    desc.add_options()("start-experiment,g",
                       boost::program_options::value<std::string>(&experiment_uuid),
                       "Start experiment with uuid given as arg.");
    desc.add_options()("experiment-duration,d",
                       boost::program_options::value<int>(&experiment_duration),
                       "Duration in seconds to run experiment for.");
    desc.add_options()("list-experiments,l", "List running experiments.");
    desc.add_options()("inspect-experiment,i", "Inspect an experiment");
    desc.add_options()("help,h", "Display help");

    // Construct a variable_map
    boost::program_options::variables_map vm;

    try {
        // Parse Argument variables
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc),
                                      vm);
        boost::program_options::notify(vm);
    } catch(const boost::program_options::error& e) {
        std::cerr << "Arg Error: " << e.what() << std::endl << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }
    if(vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    EnvironmentManager::EnvironmentController controller(environment_manager_endpoint);

    try {
        if(vm.count("shutdown-experiment") && vm.count("experiment-name")) {
            bool is_regex = vm.count("regex");
            const auto& experiments = controller.ShutdownExperiment(experiment_name, is_regex);
            std::cout << ExperimentListToJson(experiments) << std::endl;
        }

        else if(vm.count("add-experiment") && vm.count("experiment-name")) {
            auto result = controller.AddExperiment(experiment_name, graphml_path,
                                                   experiment_duration);
            std::string output;
            google::protobuf::util::JsonOptions options;
            options.add_whitespace = true;
            options.always_print_primitive_fields = true;

            if(result) {
                google::protobuf::util::MessageToJsonString(*result, &output, options);
                std::cout << output << std::endl;
            } else {
                return 1;
            }
        }

        else if(vm.count("start-experiment") && !experiment_uuid.empty()) {
            auto uuid = re::types::Uuid{experiment_uuid};
            auto success = controller.StartExperiment(uuid);
            if(success) {
                std::cout << "Started experiment: " << uuid << std::endl;
            }
        }
        else if(vm.count("list-experiments")) {
            const auto& experiments = controller.ListExperiments();
            std::cout << ExperimentListToJson(experiments) << std::endl;
        } else if(vm.count("inspect-experiment") && vm.count("experiment-name")) {
            std::cout << controller.InspectExperiment(experiment_name) << std::endl;
        } else if(vm.count("get-qpid-broker-endpoint")) {
            std::cout << controller.GetQpidBrokerEndpoint() << std::endl;
        } else if(vm.count("get-tao-cosnaming-endpoint")) {
            std::cout << controller.GetTaoCosnamingBrokerEndpoint() << std::endl;
        } else {
            std::cout << desc << std::endl;
            return 1;
        }
    } catch(const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}