#include "qpidhelper.h"

qpid::QpidHelper* qpid::QpidHelper::qpid_singleton_ = 0;

qpid::QpidHelper* qpid::QpidHelper::get_qpid_helper(){
    if(qpid_singleton_ == 0){
        qpid_singleton_ = new QpidHelper();
    }
    return qpid_singleton_;
}

qpid::messaging::Connection* qpid::QpidHelper::get_connection(std::string broker){
    qpid::messaging::Connection* con = new qpid::messaging::Connection(broker);
    return con;
}