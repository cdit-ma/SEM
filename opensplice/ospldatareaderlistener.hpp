#ifndef OSPLDATAREADERLISTENER_H
#define OSPLDATAREADERLISTENER_H

#include "../globalinterfaces.h"

#include <dds/dds.hpp>

namespace ospl{
    //Forward Declare
    template <class T, class S> class Ospl_InEventPort;

    template <class T, class S> class DataReaderListener: public dds::sub::NoOpDataReaderListener<S>
    {
        public:
            DataReaderListener(Ospl_InEventPort<T, S> *port){
                this->port_ = port;
            };
            void on_data_available(dds::sub::DataReader<S> &reader){
                port_->notify();
            };
        private:
            Ospl_InEventPort<T,S>* port_;
    };
};

#endif //OSPLDATAREADERLISTENER_H
