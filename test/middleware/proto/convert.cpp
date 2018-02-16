#include "convert.h"

// Translate from 'Base' -> 'proto'
template <>
::Basic* proto::translate<Base::Basic, Basic>(const Base::Basic& value){
	auto out = new Basic();
	out->set_str_val(value.str_val);
	out->set_int_val(value.int_val);
	out->set_guid_val(value.guid_val);
	return out;
};

// Translate from 'proto' -> 'Base'
template <>
Base::Basic* proto::translate<Base::Basic, Basic>(const ::Basic& value){
	auto out = new Base::Basic();
	out->str_val = value.str_val();
	out->int_val = value.int_val();
	out->guid_val = value.guid_val();
	return out;
};
