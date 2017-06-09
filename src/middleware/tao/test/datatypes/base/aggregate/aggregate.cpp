#include "aggregate.h"

void Base::Aggregate::set_Member(std::string val){
	this->Member_ = val;
};

void Base::Aggregate::set_Member(std::string* val){
	if(val){
		this->Member_ = *val;
	}
};

std::string Base::Aggregate::get_Member() const {
	return this->Member_;
};

std::string& Base::Aggregate::Member(){
	return this->Member_;
};

