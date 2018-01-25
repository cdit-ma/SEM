#include "convert.h"

// Including 'rti' generated header
#include "message.hpp"

// Translate from base -> 'rti'
Message* rti::translate(const Base::Message& value){
	auto out = new Message();
	out->instname(value.get_instName());
	out->messageid(value.get_messageID());
	out->content(value.get_content());
	return out;
};

// Translate from 'rti' -> base
Base::Message* rti::translate(const Message& value){
	auto out = new Base::Message();
	out->set_instName(value.instname());
	out->set_messageID(value.messageid());
	out->set_content(value.content());
	return out;
};

