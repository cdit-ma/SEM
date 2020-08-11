#ifndef OSPL_DATAREADERLISTENER_H
#define OSPL_DATAREADERLISTENER_H

#include "ports/pubsub/subscriberport.hpp"

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
                            try{
                                auto message = ::Base::Translator<BaseType, OsplType>::MiddlewareToBase(sample->data());
                                if(message){
                                    port_->EnqueueMessage(std::move(message));
                                }
                            }catch(const std::exception& ex){
                                std::string error_str("Failed to translate subscribed message: ");
                                port_->ProcessGeneralException(error_str + ex.what());
                            }
                        }
                    }
                }catch(const std::exception& ex){
                    port_->ProcessGeneralException(std::string("Unable to process samples") + ex.what());
                }
            };
        private:
            ospl::SubscriberPort<BaseType, OsplType>* port_;
    };
};

#endif //OSPL_DATAREADERLISTENER_H
