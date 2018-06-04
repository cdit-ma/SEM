#include <core/ports/translator.h>

#include "../base/basic.h"
#include "basic_DCPS.hpp"

namespace Base {
	template <>
	::Basic* Translator<::Base::Basic, ::Basic>::BaseToMiddleware(const ::Base::Basic& value){
		auto out = new ::Basic();
		out->str_val(value.str_val);
		out->guid_val(value.guid_val);
		out->int_val(value.int_val);
		return out;
	};
	
	template <>
	::Base::Basic* Translator<::Base::Basic, ::Basic>::MiddlewareToBase(const ::Basic& value){
		auto out = new ::Base::Basic();
		out->str_val = value.str_val();
		out->guid_val = value.guid_val();
		out->int_val = value.int_val();
		return out;
	};
};