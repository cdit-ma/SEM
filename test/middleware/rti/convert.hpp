#ifndef TEST_RTI_CONVERT_H
#define TEST_RTI_CONVERT_H

// Include the 'rti' translate header from re
#include <middleware/rti/translate.h>

#include "../base/basic.h"
#include "basic.hpp"

// Translate from 'Base' -> 'proto'
template <>
::Basic* rti::translate<Base::Basic, Basic>(const Base::Basic& value){
	auto out = new Basic();
	out->str_val(value.str_val);
	out->guid_val(value.guid_val);
	out->int_val(value.int_val);
	return out;
};

// Translate from 'proto' -> 'Base'
template <>
Base::Basic* rti::translate<Base::Basic, Basic>(const ::Basic& value){
	auto out = new Base::Basic();
	out->str_val = value.str_val();
	out->guid_val = value.guid_val();
	out->int_val = value.int_val();
	return out;
};

#endif //TEST_RTI_CONVERT_H