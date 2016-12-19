#ifndef RTI_DATAREADERLISTENER_H
#define RTI_DATAREADERLISTENER_H

#include "../globalinterfaces.h"

#include <rti/rti.hpp>

namespace rti{
    //Forward Declare
    template <class T, class S> class InEventPort;

    template <class T, class S> class DataReaderListener: public dds::sub::NoOpDataReaderListener<S>
    {
        public:
            DataReaderListener(rti::InEventPort<T,S> *port){
                this->port_ = port;
            };
            void on_data_available(dds::sub::DataReader<S> &reader){
                port_->notify();
            };
        private:
            rti::InEventPort<T,S>* port_;
    };
};

#endif //RTI_DATAREADERLISTENER_H
