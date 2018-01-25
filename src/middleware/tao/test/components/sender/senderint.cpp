#include "senderint.h"

SenderInt::SenderInt(const std::string& name) : Component(name){
	// [Attribute] message <160>
	message_ = ConstructAttribute(ATTRIBUTE_TYPE::STRING, "message").lock();
};

bool SenderInt::Out_tx(const Base::Message& m){
	auto p = GetTypedEventPort<OutEventPort<Base::Message> >("tx");
	if(p){
		p->tx(m);
	return true;
	};
return false;
};

void SenderInt::set_message(const std::string& value){
	message_->set_String(value);
};

const std::string& SenderInt::get_message() const{
	return message_->String();
};

std::string& SenderInt::message(){
	return message_->String();
};

