#include <core/ports/translator.h>

#include "base/message/message.h"
#include "messageC.h"

namespace Base {
	template<>
	::Test::Message* Translator<::Base::Message, ::Test::Message>::BaseToMiddleware(const ::Base::Message& value){
		auto out = new ::Test::Message();
		out->inst_name = value.get_instName().c_str();
		out->time = value.get_messageID();
		return out;
	};

	template<>
	::Base::Message* Translator<::Base::Message, ::Test::Message>::MiddlewareToBase(const ::Test::Message& value){
		auto out = new ::Base::Message();
		out->set_instName(std::string(value.inst_name));
		out->set_messageID(value.time);
		return out;
	};
}