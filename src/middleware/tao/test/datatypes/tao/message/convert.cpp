#include "convert.h"
#include "messageC.h"
#include <iostream>

// Translate from Base -> Middleware
Test::Message* tao::translate(const Base::Message& value){
	auto out = new Test::Message();
	out->inst_name = value.get_instName().c_str();
	out->time = value.get_messageID();
	return out;
};

// Translate from Middleware -> Base
Base::Message* tao::translate(const Test::Message& value){
	auto out = new Base::Message();
	out->set_instName(std::string(value.inst_name));
	out->set_messageID(value.time);
	return out;
};

