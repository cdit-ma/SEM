#ifndef PROTO_BASIC_CONVERT_H
#define PROTO_BASIC_CONVERT_H

// Include the 'proto' translate header from re
#include <middleware/proto/translate.h>

#include "../base/basic.h"
class Basic;

namespace proto{
	template <>
	::Basic* translate<Base::Basic, ::Basic>(const Base::Basic& value);
	
	template <>
	Base::Basic* translate<Base::Basic, ::Basic>(const ::Basic& value);

	template <>
	Base::Basic* decode<Base::Basic, ::Basic>(const std::string& value);
	
	template <>
	std::string encode<Base::Basic, ::Basic>(const Base::Basic& value);
}; // proto

#endif //PROTO_BASIC_CONVERT_H
