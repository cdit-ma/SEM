#ifndef RTI_DDSHELPER_H
#define RTI_DDSHELPER_H

#include <string>
#include <mutex>

//Include RTI DDS Headers
#include <rti/rti.hpp>

namespace rti{
    class Helper{
        public:
            dds::domain::DomainParticipant get_participant(int domain);
            dds::pub::Publisher get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name);
            dds::sub::Subscriber get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name);
            std::unique_lock<std::mutex> obtain_lock();
        public:
            std::mutex mutex_;
    };
    //Static getter functions
    Helper& get_dds_helper();
};

#endif //RTI_DDSHELPER_H