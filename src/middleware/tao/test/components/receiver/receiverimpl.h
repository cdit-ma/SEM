#ifndef COMPONENT_RECEIVER_IMPL_H
#define COMPONENT_RECEIVER_IMPL_H

#include "receiverint.h"
// Include Worker Header Files
#include <workers/utility/utility_worker.h>

// [ComponentImpl] Receiver <184>
class ReceiverImpl : public ReceiverInt{
	public:
		 ReceiverImpl(const std::string& name);
	protected:
		// [InEventPortImpl] rx <185>
		void In_rx(Base::Message& m);
	private:
		// Declare Worker Variables
		// [WorkerProcess] Log <190>
		std::shared_ptr<Utility_Worker> utility_worker_;
};
#endif // COMPONENT_RECEIVER_IMPL_H
