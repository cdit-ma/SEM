#ifndef RTI_DDSHELPER_H
#define RTI_DDSHELPER_H

#include <string>
#include <mutex>
#include "../core/globalinterfaces.hpp"

//Include RTI DDS Headers
#include <rti/rti.hpp>

namespace rti{
    class DdsHelper{
        public:
            static DdsHelper* get_dds_helper();
            dds::domain::DomainParticipant get_participant(int domain);
            dds::pub::Publisher get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name);
            dds::sub::Subscriber get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name);
            std::mutex mutex;

        private:
            static DdsHelper* singleton_;
            static std::mutex global_mutex_;
            

            
    };
};

#endif //RTI_DDSHELPER_H