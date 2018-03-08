#ifndef RTI_DATAREADERLISTENER_H
#define RTI_DATAREADERLISTENER_H

#include "../../core/eventports/ineventport.hpp"

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
            rti::InEventPort<T,S>* port_ = 0;
    };
};

#endif //RTI_DATAREADERLISTENER_H
