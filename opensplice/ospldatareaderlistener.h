#ifndef OSPLDATAREADERLISTENER_H
#define OSPLDATAREADERLISTENER_H

#include "../globalinterfaces.h"

#include <dds/dds.hpp>


namespace ospl{
    class DataReaderListener: public dds::sub::AnyDataReaderListener{
        public:
            DataReaderListener(InEventPort* port){this->port_ = port};

            void on_data_available (AnyDataReader &reader){port_->recieve();};
            void on_liveliness_changed (AnyDataReader &reader, const dds::core::status::LivelinessChangedStatus &status){};
            void on_requested_deadline_missed (AnyDataReader &reader, const dds::core::status::RequestedDeadlineMissedStatus &status);
            void on_requested_incompatible_qos (AnyDataReader &reader, const dds::core::status::RequestedIncompatibleQosStatus &status){};
            void on_sample_lost (AnyDataReader &reader, const dds::core::status::SampleLostStatus &status){};
            void on_sample_rejected (AnyDataReader &reader, const dds::core::status::SampleRejectedStatus &status){};
            void on_subscription_matched (AnyDataReader &reader, const dds::core::status::SubscriptionMatchedStatus &status){};
        private:
            InEventPort* port_;
    };
};

#endif //OSPLDATAREADERLISTENER_H