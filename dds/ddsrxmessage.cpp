#include "ddsrxmessage.h"
#include "convert.h"


#include <iostream>

dds_rxMessage::dds_rxMessage(rxMessageInt* component,rxMessageInt* component, DDSSubscriber* subscriber){
    this->component_ = component;
    this->subscriber_ = subscriber;
    
    rec_thread_ = new std::thread(&zmq_rxMessage::recieve, this);
}

void zmq_rxMessage::rxMessage(Message* message){
    component_->rxMessage(message);
}

void zmq_rxMessage::recieve(){
    DDS_DataReaderQos reader_qos;
    
    
    reader_ = subscriber->create_datareader("TEST",
                                                      reader_qos, 
                                                      NULL,
                                                        DDS_STATUS_MASK_ALL);
    
    while(true){
		test_dds::MessageSeq                data_seq; // holder for sequence of user data type Foo
        DDS_SampleInfoSeq      info_seq; // holder for sequence of DDS_SampleInfo
        long                  max_samples       = DDS_LENGTH_UNLIMITED;
        DDS_SampleStateMask   sample_state_mask = DDS_NOT_READ_SAMPLE_STATE;
        DDS_ViewStateMask     view_state_mask = DDS_ANY_VIEW_STATE;
        DDS_InstanceStateMask instance_state_mask = DDS_ANY_INSTANCE_STATE;
        
        reader_->read(data_seq, info_seq,
                 max_samples,
                       sample_state_mask, 
                       view_state_mask,
                       instance_state_mask);
        for(int i = 0; i < data_seq.length(); ++i) {
            std::cout << "GOT DATA" << i << std::endl;
            // use... data_seq[i] ...
        }       
    }

}