#include "receiverint.h"

ReceiverInt::ReceiverInt(const std::string& name) : Component(name){
	// [InEventPort] rx <167>
	AddCallback<Base::Message>("rx", std::bind(&ReceiverInt::In_rx, this, std::placeholders::_1));
};

