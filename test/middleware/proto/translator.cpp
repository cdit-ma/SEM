#include <core/ports/translator.h>

#include "../base/basic.h"
#include "basic.pb.h"

namespace Base {
	template <>
	::Basic* Translator<::Base::Basic, ::Basic>::BaseToMiddleware(const ::Base::Basic& value){
		auto out = new ::Basic();
		out->set_str_val(value.str_val);
		out->set_int_val(value.int_val);
		out->set_guid_val(value.guid_val);
		return out;
	};

	// Translate from 'proto' -> 'Base'
	template <>
	::Base::Basic* Translator<::Base::Basic, ::Basic>::MiddlewareToBase(const ::Basic& value){
		auto out = new ::Base::Basic();
		out->str_val = value.str_val();
		out->int_val = value.int_val();
		out->guid_val = value.guid_val();
		return out;
	};
};