#include <core/ports/translator.h>
#include "../../../base/basic.hpp"
#include "messageC.h"

namespace Base {
	template <>
	::Basic2* Translator<::Base::Basic2, ::Basic2>::BaseToMiddleware(const ::Base::Basic2& value){
		auto out = new ::Basic2();
		out->str_val = value.str_val.c_str();
		
		out->strings.length(value.str_vals.size());
		int count = 0;
		for(const auto& str : value.str_vals){
			out->strings[count++] = str.c_str();
		}
		return out;
	};

	template <>
	::Base::Basic2* Translator<::Base::Basic2, ::Basic2>::MiddlewareToBase(const ::Basic2& value){
		auto out = new ::Base::Basic2();
		out->str_val = value.str_val;

		for(int i = 0 ; i < value.strings.length(); i++){
			const auto& str = value.strings[i];
			out->str_vals.emplace_back(str);
		}
		return out;
	};
};