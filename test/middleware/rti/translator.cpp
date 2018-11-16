#include <core/ports/translator.h>
#include "../base/basic.hpp"
#include "basic.hpp"

namespace Base {
	template <>
	std::unique_ptr<::Basic> Translator<::Base::Basic, ::Basic>::BaseToMiddleware(const ::Base::Basic& value){
		auto out = std::unique_ptr<::Basic>(new ::Basic());
		out->str_val(value.str_val);
		out->guid_val(value.guid_val);
		out->int_val(value.int_val);
		return out;
	};

	template <>
	std::unique_ptr<::Base::Basic> Translator<::Base::Basic, ::Basic>::MiddlewareToBase(const ::Basic& value){
		auto out = std::unique_ptr<::Base::Basic>(new ::Base::Basic());
		out->str_val = value.str_val();
		out->guid_val = value.guid_val();
		out->int_val = value.int_val();
		return out;
	};
};