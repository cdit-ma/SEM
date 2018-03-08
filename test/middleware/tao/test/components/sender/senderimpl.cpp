#include "senderimpl.h"

SenderImpl::SenderImpl(const std::string& name) : SenderInt(name){
	// [PeriodicEvent] PeriodicEvent <179>
	AddPeriodicCallback("PeriodicEvent", std::bind(&SenderImpl::Periodic_PeriodicEvent, this));
};

void SenderImpl::Periodic_PeriodicEvent(){
	// [OutEventPortImpl] tx <174>
	Base::Message message_175;
	message_175.set_instName(get_name());
	message_175.set_messageID(count());
	message_175.set_content(message());
	Out_tx(message_175);
	count() += 1;
};

void SenderImpl::set_count(const int& value){
	count_ = value;
};

const int& SenderImpl::get_count() const{
	return count_;
};

int& SenderImpl::count(){
	return count_;
};

