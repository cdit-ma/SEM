#ifndef PROTO_MESSAGE_CONVERT_H
#define PROTO_MESSAGE_CONVERT_H

// Include the base type
#include "base/message/message.h"

class Message;

namespace proto{
	// Translate from Base -> 'proto'
	Message* translate(const Base::Message& value);
	// Translate from 'proto' -> Base
	Base::Message* translate(const Message& value);

	// Helper Functions
	template <class T> Base::Message* decode(const std::string& value);
	std::string encode(const Base::Message& value);

	// Forward declare the templated decode function with the concrete type
	template <> Base::Message* decode<Message>(const std::string& value);
}; // proto

#endif // PROTO_MESSAGE_CONVERT_H
