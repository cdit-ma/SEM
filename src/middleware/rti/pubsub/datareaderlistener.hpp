#ifndef RTI_DATAREADERLISTENER_H
#define RTI_DATAREADERLISTENER_H

#include <core/ports/pubsub/subscriberport.hpp>

#include <rti/rti.hpp>

namespace rti{
    //Forward Declare
    template <class BaseType, class RtiType> class SubscriberPort;

    template <class BaseType, class RtiType> class DataReaderListener: public dds::sub::NoOpDataReaderListener<RtiType>
    {
        public:
            DataReaderListener(rti::SubscriberPort<BaseType, RtiType> *port){
                this->port_ = port;
            };
            void on_data_available(dds::sub::DataReader<RtiType> &reader){
                try{
                    for(const auto& sample : reader.take()){
                        //Translate and callback into the component for each valid message we receive
                        if(sample->info().valid()){
                            try{
                                auto m = ::Base::Translator<BaseType, RtiType>::MiddlewareToBase(sample->data());
                                if(m){
                                    port_->EnqueueMessage(std::move(m));
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
            rti::SubscriberPort<BaseType, RtiType>* port_ = 0;
    };
};

#endif //RTI_DATAREADERLISTENER_H
