#ifndef PORTS_AGGREGATE_H
#define PORTS_AGGREGATE_H

// Include Statements
#include <core/basemessage.h>
#include <string>
namespace Base{
		class Aggregate : public ::BaseMessage{
			// Basic Member: Member <String>
			public:
				void set_Member(std::string val);
				void set_Member(std::string* val);
				std::string get_Member() const;
				std::string& Member();
			private:
				std::string Member_;
	};
};

#endif //PORTS_AGGREGATE_H
