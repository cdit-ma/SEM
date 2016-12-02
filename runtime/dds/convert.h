#ifndef DDS_CONVERT_H
#define DDS_CONVERT_H

#include "message.hpp"


#include "../message.h"


    test_dds::Message message_to_dds(::Message* message);
    ::Message* dds_to_message(test_dds::Message message);

#endif // DDS_CONVERT_H
