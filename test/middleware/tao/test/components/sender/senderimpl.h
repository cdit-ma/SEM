#ifndef COMPONENT_SENDER_IMPL_H
#define COMPONENT_SENDER_IMPL_H

#include "senderint.h"

// [ComponentImpl] Sender <172>
class SenderImpl : public SenderInt{
	public:
		 SenderImpl(const std::string& name);
	protected:
		// [PeriodicEvent] PeriodicEvent <179>
		void Periodic_PeriodicEvent();
	public:
		// [Variable] count <180>
		void set_count(const int& value);
		const int& get_count() const;
		int& count();
	private:
		int count_;
};
#endif // COMPONENT_SENDER_IMPL_H
