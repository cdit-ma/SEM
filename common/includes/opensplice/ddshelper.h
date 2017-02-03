#ifndef OSPL_DDSHELPER_H
#define OSPL_DDSHELPER_H

#include <unordered_map>
#include <string>
#include <mutex>

//Include OpenSplice DDS Headers
#include <dds/dds.hpp>

namespace ospl{
    class DdsHelper{
        public:
            //Static getter functions
            static DdsHelper* get_dds_helper();
            static std::mutex global_mutex_;

        public:
            dds::domain::DomainParticipant get_participant(int domain);
            dds::pub::Publisher get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name);
            dds::sub::Subscriber get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name);
            std::mutex mutex;
        private:
            //Lookups for OpenSplice
            std::unordered_map<std::string, dds::pub::Publisher> publisher_lookup_;
            std::unordered_map<std::string, dds::sub::Subscriber> subscriber_lookup_;
            static DdsHelper* singleton_;
    };
};

#endif //OSPL_DDSHELPER_H