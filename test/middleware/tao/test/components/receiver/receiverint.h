#ifndef COMPONENT_RECEIVER_INT_H
#define COMPONENT_RECEIVER_INT_H

#include <core/component.h>
#include <core/eventports/ineventport.hpp>
#include <core/eventports/outeventport.hpp>
#include <string>

// Include required base Aggregate header files
#include "message/message.h"

// [Component] Receiver <166>
class ReceiverInt : public ::Component{
	public:
		 ReceiverInt(const std::string& name);

	protected:
		// [InEventPort] rx <167>
		virtual void In_rx(Base::Message& m) = 0;

};
#endif // COMPONENT_RECEIVER_INT_H
