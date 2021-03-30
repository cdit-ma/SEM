//
// Created by mitchell on 8/2/21.
//
#include "environment_control_service.grpc.pb.h"
#include "epm_output_control.h"
#include "socketaddress.hpp"
#include "uuid.h"
#include <boost/program_options.hpp>
#include <grpc_util/grpc_util.h>
#include <grpc_util/server.h>
#include <grpc_util/serverlifetimemanagerimpl.h>
#include <iostream>
#include <variant>
#include <vector>
#include "modelparser.h"

template<class... Ts> struct overloaded : Ts... {
    using Ts::operator()...;
};
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::string ExperimentListToJson(const std::vector<std::string>& experiment_ids)
{
    std::ostringstream s_stream;
    s_stream << "{\"experiment_ids\":["
             << "\n";

    for(int i = 0; i < experiment_ids.size(); i++) {
        s_stream << "\t"
                 << "\"";
        s_stream << experiment_ids.at(i);
        s_stream << "\"";
        if(i != experiment_ids.size() - 1) {
            s_stream << ",";
        }
        s_stream << "\n";
    }
    s_stream << "]}";
    return s_stream.str();
}

namespace arg_parsing {

struct AddOptions {
    sem::types::SocketAddress environment_manager_endpoint;
    std::string experiment_name;
    std::string graphml_path;
};
struct LaunchOptions {
    sem::types::SocketAddress environment_manager_endpoint;
    sem::types::Uuid experiment_uuid;
    int duration;
    sem::types::Ipv4 std_out_listener_address;
};

struct ShutdownOptions {
    sem::types::SocketAddress environment_manager_endpoint;
    sem::types::Uuid uuid;
};

struct InspectOptions {
    sem::types::SocketAddress environment_manager_endpoint;
    sem::types::Uuid uuid;
};

struct ListOptions {
    sem::types::SocketAddress environment_manager_endpoint;
};

struct FailedToParseOptions {
    std::string message;
};

struct HelpOptions {
    std::string message;
};

using Mode = std::variant<AddOptions,
                          LaunchOptions,
                          ShutdownOptions,
                          InspectOptions,
                          ListOptions,
                          FailedToParseOptions,
                          HelpOptions>;

auto check_options(boost::program_options::variables_map vm,
                   const std::vector<std::string>& required_options) -> bool
{
    return std::all_of(required_options.begin(), required_options.end(),
                       [vm](const auto& required_option) {
                           return vm.count(required_option) == 1;
                       });
}

// Should constexpr these, 20 when will you save me?
std::vector<std::string> add_required_options{"add", "environment_manager_endpoint",
                                              "experiment_graphml_path", "experiment_name"};
std::vector<std::string> launch_required_options{"launch", "environment_manager_endpoint",
                                                 "experiment_uuid", "experiment_duration",
                                                 "std_out_listener_ip"};

std::vector<std::string> shutdown_required_options{"shutdown", "environment_manager_endpoint",
                                                   "experiment_uuid"};
std::vector<std::string> inspect_required_options{"inspect", "environment_manager_endpoint",
                                                  "experiment_uuid"};
std::vector<std::string> list_required_options{"list", "environment_manager_endpoint"};
std::vector<std::string> help_required_options{"help"};

auto parse_options(int argc, char** argv) -> Mode
{
    using namespace sem::types;
    namespace po = boost::program_options;
    po::options_description desc("SEM Controller options. All modes require "
                                 "environment_manager_endpoint to be specified.");
    desc.add_options()("add,a", "[MODE] Add experiment. Requires experiment_name, graphml_path, "
                                "std_out_listener_ip");
    desc.add_options()("launch,g", "[MODE] Launch experiment. Requires experiment_uuid, "
                                   "duration.");
    desc.add_options()("shutdown,s", "[MODE] Shut down experiment. Requires experiment_uuid.");
    desc.add_options()("inspect,i", "[MODE] Inspect experiment. Requires experiment_uuid.");
    desc.add_options()("list,l", "[MODE] List running experiments.");
    desc.add_options()("help,h", "[MODE] Display this message.");

    std::string experiment_uuid{};
    std::string experiment_name{};
    std::string experiment_graphml_path{};
    std::string environment_manager_endpoint{};
    std::string std_out_listener_endpoint{};
    int duration{};
    desc.add_options()("experiment_uuid,u", po::value<std::string>(&experiment_uuid));
    desc.add_options()("experiment_name,n", po::value<std::string>(&experiment_name));
    desc.add_options()("environment_manager_endpoint,e",
                       po::value<std::string>(&environment_manager_endpoint));
    desc.add_options()("experiment_graphml_path,f",
                       po::value<std::string>(&experiment_graphml_path));
    desc.add_options()("experiment_duration,d", po::value<int>(&duration));
    desc.add_options()("std_out_listener_ip,o", po::value<std::string>(&std_out_listener_endpoint));
    // Construct a variable_map
    po::variables_map vm;
    try {
        // Parse Argument variables
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch(const po::error& e) {
        return FailedToParseOptions{e.what()};
    }
    if(vm.count("help")) {
        std::cout << desc << std::endl;
        return HelpOptions{};
    }

    std::array<size_t, 5> modes{vm.count("add"), vm.count("launch"), vm.count("shutdown"),
                                vm.count("inspect"), vm.count("list")};
    size_t mode_count{0};
    for(auto v : modes) {
        mode_count += v;
    }
    if(mode_count == 0) {
        return FailedToParseOptions{"No mode specified. Use --help for list of valid modes."};
    } else if(mode_count > 1) {
        return FailedToParseOptions{"Too many modes specified."};
    }

    try {
        if(check_options(vm, add_required_options)) {
            return AddOptions{SocketAddress{environment_manager_endpoint}, experiment_name,
                              experiment_graphml_path};
        } else if(check_options(vm, launch_required_options)) {
            return LaunchOptions{SocketAddress{environment_manager_endpoint}, Uuid{experiment_uuid},
                                 duration, Ipv4{std_out_listener_endpoint}};
        } else if(check_options(vm, shutdown_required_options)) {
            return ShutdownOptions{SocketAddress{environment_manager_endpoint},
                                   Uuid{experiment_uuid}};
        } else if(check_options(vm, inspect_required_options)) {
            return InspectOptions{SocketAddress{environment_manager_endpoint},
                                  Uuid{experiment_uuid}};
        } else if(check_options(vm, list_required_options)) {
            return ListOptions{SocketAddress{environment_manager_endpoint}};
        } else if(check_options(vm, help_required_options)) {
            return HelpOptions{};
        } else {
            return FailedToParseOptions{"Invalid mode!"};
        }
    } catch(const std::exception& ex) {
        return FailedToParseOptions{ex.what()};
    }
}

} // namespace arg_parsing

auto add_experiment(const arg_parsing::AddOptions& options) -> void
{
    namespace ServiceNamespace = sem::network::services::environment_manager;
    grpc::ClientContext context;
    auto stub = sem::grpc_util::get_stub<ServiceNamespace::EnvironmentControl>(
        options.environment_manager_endpoint);

    auto model_proto = re::ModelParser::ModelParser::ParseModel(options.graphml_path, options.experiment_name);

    ServiceNamespace::AddExperimentRequest request;
    request.set_experiment_manifest("experiment manifest");
    request.set_experiment_name(options.experiment_name);
    ServiceNamespace::AddExperimentResponse response;
    auto status = stub->AddExperiment(&context, request, &response);

    if(!status.ok()) {
        throw std::runtime_error("gRPC call failed in add_experiment");
    }

    // Output experiment uuid, we need this to be the only data pushed to std out s.t. jenkins can
    // grab it
    std::cout << response.experiment_uuid() << std::endl;
}
auto launch_experiment(const arg_parsing::LaunchOptions& options) -> void
{
    // start std out listener
    auto listener_address = options.std_out_listener_address;

    using namespace sem::grpc_util;
    auto lifetime_manager = ServerLifetimeManagerImpl();

    // Set up controller for std out listener and std out listener
    auto output_control_service = std::make_shared<sem::environment_controller::epm_output_control>(
        lifetime_manager, listener_address);
    auto server = LifetimeManagedServer(lifetime_manager, listener_address,
                                        {output_control_service});

    auto control_endpoint = server.endpoints().at(listener_address);
    auto listener_endpoint = output_control_service->get_listener_endpoint();

    // send experiment registration
    namespace ServiceNamespace = sem::network::services::environment_manager;
    grpc::ClientContext context;
    auto stub = sem::grpc_util::get_stub<ServiceNamespace::EnvironmentControl>(
        options.environment_manager_endpoint);

    ServiceNamespace::StartExperimentRequest request;
    request.set_experiment_uuid(options.experiment_uuid.to_string());
    request.set_listener_endpoint(listener_endpoint.to_string());
    request.set_listener_control_endpoint(control_endpoint.to_string());
    request.set_duration(options.duration);
    ServiceNamespace::StartExperimentResponse response;
    auto status = stub->StartExperiment(&context, request, &response);

    if(!status.ok()) {
        throw std::runtime_error("Grpc request failed in launch_experiment");
    }

    lifetime_manager.wait();
}

auto shutdown_experiment(const arg_parsing::ShutdownOptions& options) -> void
{
    // send experiment shut down request
    namespace ServiceNamespace = sem::network::services::environment_manager;
    grpc::ClientContext context;
    auto stub = sem::grpc_util::get_stub<ServiceNamespace::EnvironmentControl>(
        options.environment_manager_endpoint);

    ServiceNamespace::ShutdownExperimentRequest request;
    request.set_experiment_uuid(options.uuid.to_string());
    ServiceNamespace::ShutdownExperimentResponse response;
    auto status = stub->ShutdownExperiment(&context, request, &response);

    if(!status.ok()) {
        throw std::runtime_error("Grpc request failed in shutdown_experiment");
    }
}

auto list_experiments(const arg_parsing::ListOptions& options) -> void
{
    // send list_experiments request
    namespace ServiceNamespace = sem::network::services::environment_manager;
    grpc::ClientContext context;
    auto stub = sem::grpc_util::get_stub<ServiceNamespace::EnvironmentControl>(
        options.environment_manager_endpoint);

    ServiceNamespace::ListExperimentsRequest request;
    ServiceNamespace::ListExperimentsResponse response;
    auto status = stub->ListExperiments(&context, request, &response);

    if(!status.ok()) {
        throw std::runtime_error("Grpc request failed in list_experiments");
    }

    if(!response.success()) {
        throw std::runtime_error("Grpc request failed in list_experiments (server side)");
    }

    std::vector<std::string> experiment_ids(response.experiment_uuids().begin(),
                                            response.experiment_uuids().end());
    std::cout << ExperimentListToJson(experiment_ids) << std::endl;
}

auto inspect_experiment(const arg_parsing::InspectOptions& options) -> void
{
    // send inspect experiment message
    namespace ServiceNamespace = sem::network::services::environment_manager;
    grpc::ClientContext context;
    auto stub = sem::grpc_util::get_stub<ServiceNamespace::EnvironmentControl>(
        options.environment_manager_endpoint);

    ServiceNamespace::InspectExperimentRequest request;
    request.set_experiment_uuid(options.uuid.to_string());
    ServiceNamespace::InspectExperimentResponse response;
    auto status = stub->InspectExperiment(&context, request, &response);

    if(!status.ok()) {
        throw std::runtime_error("Grpc request failed in inspect_experiment");
    }
}

auto main(int argc, char** argv) -> int
{
    auto mode = arg_parsing::parse_options(argc, argv);

    std::visit(
        overloaded{
            [](const arg_parsing::AddOptions& options) { add_experiment(options); },
            [](const arg_parsing::LaunchOptions& options) { launch_experiment(options); },
            [](const arg_parsing::ShutdownOptions& options) { shutdown_experiment(options); },
            [](const arg_parsing::InspectOptions& options) { inspect_experiment(options); },
            [](const arg_parsing::ListOptions& options) { list_experiments(options); },
            [](const arg_parsing::
                   HelpOptions&) { /* do nothing, we print help options in our parse function */ },
            [](const arg_parsing::FailedToParseOptions& failure) {
                std::cout << failure.message << std::endl;
            }},
        mode);

    return 0;
}
