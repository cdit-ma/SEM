#ifndef BASE_BASIC_H
#define BASE_BASIC_H

// Include Statements
#include "basemessage.h"
#include <string>
#include <iostream>
#include <vector>

namespace Base{
	class Basic : public ::BaseMessage{
		public:
			int int_val;
			std::string str_val;
			std::string guid_val;
	};

	class Basic2 : public ::BaseMessage{
		public:
			std::string str_val;
			std::vector<std::string> str_vals;
	};
};

#endif //BASE_BASIC_H
