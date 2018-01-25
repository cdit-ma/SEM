#include <core/libcomponentexport.h>

// Include the component impl
#include "senderimpl.h"

Component* ConstructComponent(const std::string& name){
	return new SenderImpl(name);
};
