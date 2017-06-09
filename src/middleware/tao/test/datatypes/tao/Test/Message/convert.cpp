#include "convert.h"
#include "messageC.h"

// Translate from Base -> Middleware
Test::Message* tao::translate(const Base::Aggregate *src){
	auto dst_ = new Test::Message();

	dst_->inst_name = src->get_Member().c_str();
	return dst_;
};

// Translate from Middleware -> Base
Base::Aggregate* tao::translate(const Test::Message *src){
	auto dst_ = new Base::Aggregate();

	dst_->Member() = src->inst_name;
	return dst_;
};

