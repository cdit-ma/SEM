#include "convert.h"

// Including 'proto' generated header
#include "message.pb.h"

// Translate from base -> 'proto'
Message* proto::translate(const Base::Message& value){
	auto out = new Message();
	out->set_instname(value.get_instName());
	out->set_messageid(value.get_messageID());
	out->set_content(value.get_content());
	return out;
};

// Translate from 'proto' -> base
Base::Message* proto::translate(const Message& value){
	auto out = new Base::Message();
	out->set_instName(value.instname());
	out->set_messageID(value.messageid());
	out->set_content(value.content());
	return out;
};

template <>
Base::Message* proto::decode<Message>(const std::string& value){
	Message out;
	out.ParseFromString(value);
	return translate(out);
};

std::string proto::encode(const Base::Message& value){
	std::string out;
	auto pb = proto::translate(value);
	pb->SerializeToString(&out);
	delete pb;
	return out;
};

