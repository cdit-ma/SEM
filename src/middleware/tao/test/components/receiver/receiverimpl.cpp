#include "receiverimpl.h"

ReceiverImpl::ReceiverImpl(const std::string& name) : ReceiverInt(name){
	// Declare Worker Variables
	utility_worker_ = AddTypedWorker<Utility_Worker>(*this, "utility_worker");
};

void ReceiverImpl::In_rx(Base::Message& m){
	utility_worker_->Log("Received Message[%d] from '%s' = '%s'", true, m.messageID(), m.instName().c_str(), m.content().c_str());
};

