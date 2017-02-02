#ifndef RTI_DDSHELPER_H
#define RTI_DDSHELPER_H


#include <unordered_map>
#include <string>
#include <mutex>

//Include RTI DDS Headers
#include <rti/rti.hpp>

namespace rti{
    class DdsHelperS{
        public:
            static DdsHelperS* get_dds_helper();
            dds::domain::DomainParticipant get_participant(int domain);
            dds::pub::Publisher get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name);
            dds::sub::Subscriber get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name);
        private:
            //Lookups for OpenSplice
            std::unordered_map<std::string, dds::pub::Publisher> publisher_lookup_;
            std::unordered_map<std::string, dds::sub::Subscriber> subscriber_lookup_;

            std::mutex mutex_;
            static std::mutex global_mutex_;
            static DdsHelperS* singleton_;
    };
};

#endif //RTI_DDSHELPER_H