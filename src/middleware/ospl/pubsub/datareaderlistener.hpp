#ifndef OSPL_DATAREADERLISTENER_H
#define OSPL_DATAREADERLISTENER_H

#include <core/ports/pubsub/subscriberport.hpp>

#include <dds/dds.hpp>

namespace ospl{
    //Forward Declare
    template <class BaseType, class OsplType> class SubscriberPort;

    template <class BaseType, class OsplType> class DataReaderListener: public dds::sub::NoOpDataReaderListener<OsplType>
    {
        public:
            DataReaderListener(ospl::SubscriberPort<BaseType, OsplType> *port){
                this->port_ = port;
            };
            void on_data_available(dds::sub::DataReader<OsplType> &reader){
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
            ospl::SubscriberPort<BaseType, OsplType>* port_;
    };
};

#endif //OSPL_DATAREADERLISTENER_H
