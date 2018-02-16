#ifndef PROTO_BASIC_CONVERT_HPP
#define PROTO_BASIC_CONVERT_HPP

// Include the 'proto' translate header from re
#include <middleware/proto/translate.h>

#include "../base/basic.h"
#include "basic.pb.h"


namespace proto{
	// Translate from 'Base' -> 'proto'
	template <>
	::Basic* translate<Base::Basic, Basic>(const Base::Basic& value){
		auto out = new Basic();
		out->set_str_val(value.str_val);
		out->set_int_val(value.int_val);
		return out;
	};

	// Translate from 'proto' -> 'Base'
	template <>
	Base::Basic* translate<Base::Basic, Basic>(const ::Basic& value){
		auto out = new Base::Basic();
		out->str_val = value.str_val();
		out->int_val = value.int_val();

		return out;
	};

	// Translate from 'Base' -> 'proto'
	template <>
	Base::Basic* decode<Base::Basic, Basic>(const std::string& value){
		Basic proto_obj;
		proto_obj.ParseFromString(value);
		return proto::translate<Base::Basic, Basic>(proto_obj);
	};

	// Translate from 'proto' -> 'Base'
	template <>
	std::string encode<Base::Basic, Basic>(const Base::Basic& value){
		std::string str_value;
		auto proto_obj = proto::translate<Base::Basic, Basic>(value);
		proto_obj->SerializeToString(&str_value);
		delete proto_obj;
		return str_value;
	};
};
/*
	// Translate from 'Base' -> 'proto'
	template <>
	::Basic* proto::translate<Base::Basic, Basic>(const Base::Basic& value){
		auto out = new Basic();
		out->set_str_val(value.str_val);
		out->set_int_val(value.int_val);
		return out;
	};

	// Translate from 'proto' -> 'Base'
	template <>
	Base::Basic* proto::translate<Base::Basic, Basic>(const ::Basic& value){
		auto out = new Base::Basic();
		out->str_val = value.str_val();
		out->int_val = value.int_val();

		return out;
	};

	// Translate from 'Base' -> 'proto'
	template <>
	Base::Basic* proto::decode<Base::Basic, Basic>(const std::string& value){
		Basic proto_obj;
		proto_obj.ParseFromString(value);
		return proto::translate<Base::Basic, Basic>(proto_obj);
	};

	// Translate from 'proto' -> 'Base'
	template <>
	std::string proto::encode<Base::Basic, Basic>(const Base::Basic& value){
		std::string str_value;
		auto proto_obj = proto::translate<Base::Basic, Basic>(value);
		proto_obj->SerializeToString(&str_value);
		delete proto_obj;
		return str_value;
	};*/

#endif //PROTO_BASIC_CONVERT_HPP
