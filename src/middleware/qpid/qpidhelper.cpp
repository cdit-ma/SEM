#include "qpidhelper.h"

qpid::QpidHelper& qpid::QpidHelper::get_qpid_helper(){
    static qpid::QpidHelper q;
    return q;
};