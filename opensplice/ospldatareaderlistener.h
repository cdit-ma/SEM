#ifndef OSPLDATAREADERLISTENER_H
#define OSPLDATAREADERLISTENER_H

#include "../globalinterfaces.h"

#include <dds/dds.hpp>

template class<class T> DataReaderListener: public dds::sub::NoOpDataReaderListener<T>
{
public:
    DataReaderListener(InEventPort* port) {this->port_ = port;};
    
    void on_sample_lost(dds::sub::DataReader<T> &reader, const dds::core::status::SampleLostStatus &status){};
    void on_subscription_matched(dds::sub::DataReader<T> &reader, const dds::core::status::SubscriptionMatchedStatus &status){};
    void on_sample_rejected(dds::sub::DataReader<T> &reader, const dds::core::status::SampleRejectedStatus &status){};
    void on_requested_incompatible_qos(dds::sub::DataReader<T> &reader, const dds::core::status::RequestedIncompatibleQosStatus &status){};
    void on_requested_deadline_missed(dds::sub::DataReader<T> &reader, const dds::core::status::RequestedDeadlineMissedStatus &status){};
    void on_liveliness_changed(dds::sub::DataReader<T> &reader, const dds::core::status::LivelinessChangedStatus &status){};
    void on_data_available(dds::sub::DataReader<T> &reader){this->port_->recieve()};
private:
    InEventPort* port_;
};

#endif //OSPLDATAREADERLISTENER_H