#ifndef RE_REPLIER_HPP
#define RE_REPLIER_HPP

#include <future>
#include <qpid/messaging/Address.h>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>
#include <string>
#include <types/serializable.hpp>
#include <types/proto/serializable.hpp>
#include <types/socketaddress.hpp>
#include <utility>
#include <iostream>
namespace re::network {
template<typename RequestType, typename ReplyType> class Replier {
    std::string topic_;
    std::string subject_;
    types::SocketAddress broker_address_;

    qpid::messaging::Connection connection_;
    qpid::messaging::Session session_ = nullptr;

    std::future<void> run_handle_;

public:
    Replier(types::SocketAddress broker_address, std::string_view topic, std::string_view subject) :
        broker_address_{broker_address},
        topic_{topic},
        subject_{subject},
        connection_{broker_address.to_string()}
    {
        connection_.open();
        session_ = connection_.createSession();
    };

    auto run(std::function<ReplyType(RequestType)> handler) -> void
    {
        run_handle_ = std::async(std::launch::async, &Replier<RequestType, ReplyType>::run_internal,
                                 this, handler);
    };

    auto stop() -> void
    {
        if(!session_.isNull() && session_.isValid()) {
            session_.close();
        }
        if(!connection_.isNull() && connection_.isValid() && connection_.isOpen()) {
            connection_.close();
        }
        if(run_handle_.valid()) {
            run_handle_.wait_for(std::chrono::seconds(1));
        }
    }

    ~Replier() { stop(); }

private:
    auto run_internal(const std::function<ReplyType(RequestType)> handler) -> void
    {
        try {
            auto receiver = session_.createReceiver(build_creation_string(topic_, subject_));
            qpid::messaging::Message message;
            while(receiver.fetch(message)) {
                const auto& reply_address = message.getReplyTo();

                if(!reply_address) {
                    session_.reject(message);
                    std::cerr << "No reply address set on message: "
                              << build_creation_string(topic_, subject_) << std::endl;
                    continue;
                }

                try {
                    qpid::messaging::Sender sender = session_.createSender(reply_address);
                    ReplyType reply_message = handler(
                        re::types::Serializable<RequestType>::deserialize(message.getContent()));
                    sender.send(re::types::Serializable<ReplyType>::serialize(reply_message));
                    // TODO: Investigate whether we should ack before or after sending.
                    session_.acknowledge();
                    sender.close();
                } catch(const qpid::messaging::ResolutionError& ex) {
                    try {
                        session_.reject(message);
                    } catch(const std::exception& ex) {
                        // TODO: Unsure what to do here
                        std::cout << "Exception thrown while rejecting message in qpid replier"
                                  << std::endl;
                    }
                    continue;
                }
            }
            connection_.close();
        } catch(const std::exception& ex) {
            if(!session_.isNull() && session_.isValid()) {
                session_.sync(true);
                session_.close();
            }
            if(!connection_.isNull() && connection_.isOpen()) {
                connection_.close();
            }
            std::cout << "Closing qpid replier on topic: " << topic_ << "/" << subject_
                      << std::endl;
            return;
        }
        connection_.close();
    }

    static auto
    build_creation_string(const std::string& topic, const std::string& subject) -> std::string
    {
        // This string contains both the topic + subject of the queue to set up on the broker as
        // well as configuration details.
        // We configure our queue to create if we create a receiver,
        // delete if we close a receiver,
        // cause an exception if we try to connect and the queue doesn't exist as a sender,
        // and finally to auto delete if we have no more subscribers.
        return topic + "/" + subject
               + " ; {create: receiver, delete: receiver, assert: sender, "
                 "node:{x-declare:{auto-delete:True}}}";
    }
};
} // namespace re::network
#endif // RE_REPLIER_HPP
