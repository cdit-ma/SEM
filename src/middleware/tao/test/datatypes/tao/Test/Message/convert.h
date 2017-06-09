#ifndef TAO_AGGREGATE_CONVERT_H
#define TAO_AGGREGATE_CONVERT_H

// Include the base type
#include "../../../base/aggregate/aggregate.h"

// Forward declare the concrete type
namespace Test{
	class Message;	
}
namespace tao{
	// Translate Functions
	Test::Message* translate(const Base::Aggregate* val);
	Base::Aggregate* translate(const Test::Message* val);
};

#endif //TAO_AGGREGATE_CONVERT_H
