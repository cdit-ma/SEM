#ifndef CONVERT_H
#define CONVERT_H

#include <iostream>

#include "base_basic.h"
#include "basic.hpp"


// Forward declare the concrete type
class Aggregate;
namespace rti{
	// Translate Functions
	Basic* translate(const Base::Basic& src){
		auto dst_ = new Basic();
		dst_->str_val() = src.str_val;
		dst_->int_val() = src.int_val;
		return dst_;
	};

	Base::Basic* translate(const Basic& src){
		auto dst_ = new Base::Basic();
		dst_->str_val = src.str_val();
		dst_->int_val = src.int_val();
		return dst_;
	};
};

#endif //PROTO_AGGREGATE_CONVERT_H
