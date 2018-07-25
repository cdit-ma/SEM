#include <core/ports/translator.h>
#include "../../base/basic.hpp"
#include "messageC.h"

namespace Base {
	template <>
	::Basic* Translator<::Base::Basic, ::Basic>::BaseToMiddleware(const ::Base::Basic& value){
		auto out = new ::Basic();
		out->str_val = value.str_val.c_str();
		out->guid_val = value.guid_val.c_str();
		out->int_val = value.int_val;
		return out;
	};

	template <>
	::Base::Basic* Translator<::Base::Basic, ::Basic>::MiddlewareToBase(const ::Basic& value){
		auto out = new ::Base::Basic();
		out->str_val = value.str_val;
		out->guid_val = value.guid_val;
		out->int_val = value.int_val;
		return out;
	};
};