#ifndef OSPL_DATAREADERLISTENER_H
#define OSPL_DATAREADERLISTENER_H

#include "../core/eventports/ineventport.hpp"

#include <dds/dds.hpp>

namespace ospl{
    //Forward Declare
    template <class T, class S> class InEventPort;

    template <class T, class S> class DataReaderListener: public dds::sub::NoOpDataReaderListener<S>
    {
        public:
            DataReaderListener(ospl::InEventPort<T, S> *port){
                this->port_ = port;
            };
            void on_data_available(dds::sub::DataReader<S> &reader){
                port_->notify();
            };
        private:
            ospl::InEventPort<T,S>* port_;
    };
};

#endif //OSPL_DATAREADERLISTENER_H
