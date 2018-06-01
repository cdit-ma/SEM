#ifndef OSPL_DATAREADERLISTENER_H
#define OSPL_DATAREADERLISTENER_H

#include "../../core/eventports/ineventport.hpp"

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
                try{
                    for(const auto& sample : reader.take()){
                        //Translate and callback into the component for each valid message we receive
                        if(sample->info().valid()){
                            auto m = port_->translator.MiddlewareToBase(sample->data());
                            if(m){
                                port_->EnqueueMessage(m);
                            }
                        }
                    }
                }catch(const std::exception& ex){
                    Log(Severity::ERROR_).Context(port_).Func(__func__).Msg(std::string("Unable to process samples") + ex.what());
                }
            };
        private:
            ospl::InEventPort<T,S>* port_;
    };
};

#endif //OSPL_DATAREADERLISTENER_H
