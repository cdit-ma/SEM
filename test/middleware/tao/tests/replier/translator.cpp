#include <core/ports/translator.h>
#include "../../../base/basic.hpp"
#include "messageC.h"

namespace Base {
	template <>
	std::unique_ptr<::Basic3> Translator<::Base::Basic2, ::Basic3>::BaseToMiddleware(const ::Base::Basic2& value){
		auto out = std::unique_ptr<::Basic3>(new ::Basic3());
		out->str_val = value.str_val.c_str();
		
		out->strings.length(value.str_vals.size());
		int count = 0;
		for(const auto& str : value.str_vals){
			out->strings[count++] = str.c_str();
		}
		return out;
	};

	template <>
	std::unique_ptr<::Base::Basic2> Translator<::Base::Basic2, ::Basic3>::MiddlewareToBase(const ::Basic3& value){
		auto out = std::unique_ptr<::Base::Basic2>(new ::Base::Basic2());
		out->str_val = value.str_val;

		for(int i = 0 ; i < value.strings.length(); i++){
			const auto& str = value.strings[i];
			out->str_vals.emplace_back(str);
		}

		return out;
	};
};