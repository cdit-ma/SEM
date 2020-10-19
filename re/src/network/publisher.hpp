#ifndef RE_PUBLISHER_HPP
#define RE_PUBLISHER_HPP

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
template<typename PublishType> class Publisher {
    std::string topic_;
    std::string subject_;
    types::SocketAddress broker_address_;

    qpid::messaging::Connection connection_;
    qpid::messaging::Session session_ = nullptr;

public:
    Publisher(const types::SocketAddress& broker_address,
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

    ~Publisher()
    {
        session_.close();
        connection_.close();
    }

    auto publish(PublishType message) -> void
    {
        try {
            qpid::messaging::Sender sender = session_.createSender(
                build_address_string(topic_, subject_));
            qpid::messaging::Message request;
            request.setContent(types::Serializable<PublishType>::serialize(message));
            sender.send(request);

            session_.acknowledge(true);
        } catch(const qpid::messaging::ResolutionError& ex) {
            throw std::invalid_argument("Could not connect to receiver on topic: " + topic_);
        }
    }
private:
    static auto
    build_address_string(const std::string& topic, const std::string& subject) -> std::string
    {
        return topic + "/" + subject + " ; {create: always, delete: receiver}";
    }
};
} // namespace re::network
#endif // RE_PUBLISHER_HPP
