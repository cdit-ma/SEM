#include "message.h"

void Base::Message::set_instName(const std::string& value){
	instName_ = value;
};

const std::string& Base::Message::get_instName() const{
	return instName_;
};

std::string& Base::Message::instName(){
	return instName_;
};

void Base::Message::set_messageID(const int& value){
	messageID_ = value;
};

const int& Base::Message::get_messageID() const{
	return messageID_;
};

int& Base::Message::messageID(){
	return messageID_;
};

void Base::Message::set_content(const std::string& value){
	content_ = value;
};

const std::string& Base::Message::get_content() const{
	return content_;
};

std::string& Base::Message::content(){
	return content_;
};

