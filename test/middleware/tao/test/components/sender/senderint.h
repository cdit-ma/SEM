#ifndef COMPONENT_SENDER_INT_H
#define COMPONENT_SENDER_INT_H

#include <core/component.h>
#include <core/eventports/ineventport.hpp>
#include <core/eventports/outeventport.hpp>
#include <string>

// Include required base Aggregate header files
#include "message/message.h"

// [Component] Sender <159>
class SenderInt : public ::Component{
	public:
		 SenderInt(const std::string& name);

	protected:
		// [OutEventPort] tx <161>
		bool Out_tx(const Base::Message& m);

	public:
		// [Attribute] message <160>
		void set_message(const std::string& value);
		const std::string& get_message() const;
		std::string& message();
	private:
		std::shared_ptr<Attribute> message_;
};
#endif // COMPONENT_SENDER_INT_H
