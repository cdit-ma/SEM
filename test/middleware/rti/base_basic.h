#ifndef RTI_BASE_BASIC_H
#define RTI_BASE_BASIC_H

// Include Statements
#include <core/basemessage.h>
#include <string>
namespace Base{
	class Basic : public ::BaseMessage{
		public:
			int int_val;
			std::string str_val;
			std::string guid_val;
	};
};

#endif //RTI_BASE_BASIC_H
