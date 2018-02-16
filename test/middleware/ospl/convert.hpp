#ifndef TEST_OSPL_CONVERT_H
#define TEST_OSPL_CONVERT_H

// Include the 'ospl' translate header from re
#include <middleware/ospl/translate.h>

#include "../base/basic.h"
#include "basic.hpp"

// Translate from 'Base' -> 'ospl'
template <>
::Basic* ospl::translate<Base::Basic, Basic>(const Base::Basic& value){
	auto out = new Basic();
	out->str_val(value.str_val);
	out->guid_val(value.guid_val);
	out->int_val(value.int_val);
	return out;
};

// Translate from 'ospl' -> 'Base'
template <>
Base::Basic* ospl::translate<Base::Basic, Basic>(const ::Basic& value){
	auto out = new Base::Basic();
	out->str_val = value.str_val();
	out->guid_val = value.guid_val();
	out->int_val = value.int_val();
	return out;
};

#endif //TEST_OSPL_CONVERT_H