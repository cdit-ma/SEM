#include <core/libcomponentexport.h>

// Include the component impl
#include "receiverimpl.h"

Component* ConstructComponent(const std::string& name){
	return new ReceiverImpl(name);
};
