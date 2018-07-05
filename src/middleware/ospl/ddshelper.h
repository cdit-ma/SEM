#ifndef OSPL_DDSHELPER_H
#define OSPL_DDSHELPER_H

#include <unordered_map>
#include <string>
#include <mutex>

//Include OpenSplice DDS Headers
#include <dds/dds.hpp>

namespace ospl{
    class Helper{
        public:
            dds::domain::DomainParticipant get_participant(int domain);
            dds::pub::Publisher get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name);
            dds::sub::Subscriber get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name);
            std::unique_lock<std::mutex> obtain_lock();
        private:
            std::mutex mutex_;
            std::unordered_map<std::string, dds::pub::Publisher> publisher_lookup_;
            std::unordered_map<std::string, dds::sub::Subscriber> subscriber_lookup_;
    };
    //Static getter functions
    Helper& get_dds_helper();
};

#endif //OSPL_DDSHELPER_H