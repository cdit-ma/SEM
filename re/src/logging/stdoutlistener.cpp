#include <boost/program_options.hpp>
#include <network/protocols/epmstdout/epmstdout.pb.h>
#include <network/subscriber.hpp>
#include <types/socketaddress.hpp>
#include <types/uuid.h>

bool running_ = true;

auto HandleMessage(re::network::protocol::epmstdout::Message& message) -> void
{
    switch(message.Action_case()) {
        case re::network::protocol::epmstdout::Message::kOutput: {
            std::cout << message.output();
            break;
        }
        case re::network::protocol::epmstdout::Message::kShutdown: {
            running_ = false;
            break;
        }
        case re::network::protocol::epmstdout::Message::ACTION_NOT_SET:
            break;
    }
}
auto main(int argc, char** argv) -> int
{
    namespace po = boost::program_options;
    std::string experiment_uuid_string{};
    std::string qpid_broker_endpoint_string{};

    po::options_description options{"StdOutListener Options"};

    options.add_options()("qpid_broker_endpoint,e",
                          po::value<std::string>(&qpid_broker_endpoint_string)->required(),
                          "qpid broker endpoint");
    options.add_options()("experiment_uuid,i",
                          po::value<std::string>(&experiment_uuid_string)->required(),
                          "experiment uuid to listen for");
    po::variables_map parsed_options;
    po::store(po::parse_command_line(argc, argv, options), parsed_options);
    po::notify(parsed_options);


    auto uuid = re::types::Uuid{experiment_uuid_string};
    auto qpid_broker_endpoint = re::types::SocketAddress(qpid_broker_endpoint_string);

    auto listen_topic = uuid.to_string() + "_epm_std_out";

    using MessageType = re::network::protocol::epmstdout::Message;
    auto subscriber = re::network::Subscriber<MessageType>{qpid_broker_endpoint, listen_topic, ""};
    subscriber.run([](MessageType message) { HandleMessage(message); });

    while(running_) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}