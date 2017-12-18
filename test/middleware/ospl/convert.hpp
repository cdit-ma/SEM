#ifndef OSPL_TEST_CONVERT_H
#define OSPL_TEST_CONVERT_H

#include <iostream>

#include "base_basic.h"
#include "basic_DCPS.hpp"


// Forward declare the concrete type
class Aggregate;
namespace ospl{
	// Translate Functions
	Basic* translate(const Base::Basic& src){
		auto dst_ = new Basic();
		dst_->str_val(src.str_val);
		dst_->int_val(src.int_val);
		return dst_;
	};

	Base::Basic* translate(const Basic& src){
		auto dst_ = new Base::Basic();
		dst_->str_val = src.str_val();
		dst_->int_val = src.int_val();
		return dst_;
	};
};

#endif //OSPL_TEST_CONVERT_H
