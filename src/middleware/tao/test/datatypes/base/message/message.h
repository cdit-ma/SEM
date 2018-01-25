#ifndef BASE_MESSAGE_H
#define BASE_MESSAGE_H

#include <core/basemessage.h>
#include <string>
namespace Base{
	class Message : public ::BaseMessage{
	public:
		// [Member] instName <156>
		void set_instName(const std::string& value);
		const std::string& get_instName() const;
		std::string& instName();
	private:
		std::string instName_;
	public:
		// [Member] messageID <157>
		void set_messageID(const int& value);
		const int& get_messageID() const;
		int& messageID();
	private:
		int messageID_;
	public:
		// [Member] content <158>
		void set_content(const std::string& value);
		const std::string& get_content() const;
		std::string& content();
	private:
		std::string content_;
	};
}; // Base
#endif // BASE_MESSAGE_H
