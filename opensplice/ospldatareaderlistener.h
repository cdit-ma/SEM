#ifndef OSPLDATAREADERLISTENER_H
#define OSPLDATAREADERLISTENER_H

#include "../globalinterfaces.h"

#include <dds/dds.hpp>

template <class T> class DataReaderListener: public dds::sub::NoOpDataReaderListener<T>
{
public:
    DataReaderListener(InEventPort* port){
        this->port_ = port;
    };
    void on_data_available(dds::sub::DataReader<T> &reader){
        port_->recieve();
    };
private:
    InEventPort* port_;
};


#endif //OSPLDATAREADERLISTENER_H
