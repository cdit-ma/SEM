#ifndef CONVERT_H
#define CONVERT_H

#include <iostream>

#include "base_basic.h"
#include "basic.pb.h"

// Forward declare the concrete type
class Aggregate;
namespace proto{
	// Translate Functions
	Basic* translate(const Base::Basic& src){
		auto dst_ = new Basic();
		dst_->set_str_val(src.str_val);
		dst_->set_int_val(src.int_val);
		return dst_;
	};

	Base::Basic* translate(const Basic& src){
		auto dst_ = new Base::Basic();
		dst_->str_val = src.str_val();
		dst_->int_val = src.int_val();
		return dst_;
	};

	std::string encode(const Base::Basic& src){
		std::string out_;
		auto pb_ = translate(src);
		pb_->SerializeToString(&out_);
		//delete pb_;
		return out_;
	};

	template <class T> Base::Basic* decode(const std::string val);

	// Forward declare the decode function with concrete type
	template <> Base::Basic* decode<Basic>(const std::string src){
		Basic out_;
		out_.ParseFromString(src);
		return translate(out_);
	};
};

#endif //PROTO_AGGREGATE_CONVERT_H
