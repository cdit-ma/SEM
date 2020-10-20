#ifndef RE_REQUESTER_H
#define RE_REQUESTER_H

#include <qpid/messaging/Address.h>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>
#include <string>
#include "serializable.hpp"
#include "proto_serializable.hpp"
#include "socketaddress.hpp"
#include "timeout.hpp"
#include <utility>

namespace re::network {

template<typename RequestType, typename ReplyType> class Requester {
    std::string topic_;
    std::string subject_;
    types::SocketAddress broker_address_;

    qpid::messaging::Connection connection_;
    qpid::messaging::Session session_ = nullptr;

public:
    class exception : public std::runtime_error {
    public:
        explicit exception(const std::string& what_arg) : std::runtime_error(what_arg){};
    };
    Requester(const types::SocketAddress& broker_address,
              std::string_view topic,
              std::string_view subject) :
        broker_address_{broker_address},
        topic_{topic},
        subject_{subject},
        connection_{broker_address.to_string()}
    {
        connection_.open();
        session_ = connection_.createSession();
    }

    auto request(RequestType message, const types::Timeout& timeout) -> ReplyType
    {
        try {
            qpid::messaging::Sender sender = session_.createSender(
                build_address_string(topic_, subject_));
            qpid::messaging::Receiver receiver{session_.createReceiver("# ; {assert: sender}")};
            qpid::messaging::Address response_queue{receiver.getAddress()};

            qpid::messaging::Message request;
            request.setReplyTo(response_queue);
            request.setContent(types::Serializable<RequestType>::serialize(message));
            sender.send(request);
            qpid::messaging::Message response;
            if(std::holds_alternative<types::NeverTimeout>(timeout)) {
                response = receiver.fetch();
            } else if(std::holds_alternative<std::chrono::milliseconds>(timeout)) {
                response = receiver.fetch(qpid::messaging::Duration{
                    static_cast<uint64_t>(std::get<std::chrono::milliseconds>(timeout).count())});
            }
            session_.acknowledge(true);
            return types::Serializable<ReplyType>::deserialize(response.getContent());
        } catch(const qpid::messaging::ResolutionError& ex) {
            throw exception("Could not connect to receiver on topic: " + topic_);
        }
    }

private:
    static auto
    build_address_string(const std::string& topic, const std::string& subject) -> std::string
    {
        return topic + "/" + subject + " ; {create: receiver, delete: receiver, assert: sender}";
    }
};

} // namespace re::network
#endif // RE_REQUESTER_H
