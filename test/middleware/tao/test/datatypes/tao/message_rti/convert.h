#ifndef RTI_MESSAGE_CONVERT_H
#define RTI_MESSAGE_CONVERT_H

// Include the base type
#include "base/message/message.h"

class Message;

namespace rti{
	// Translate from Base -> 'rti'
	Message* translate(const Base::Message& value);
	// Translate from 'rti' -> Base
	Base::Message* translate(const Message& value);
}; // rti

#endif // RTI_MESSAGE_CONVERT_H
