#ifndef QPID_QPIDHELPER_H
#define QPID_QPIDHELPER_H

namespace qpid{
    class QpidHelper{
        public:
            static QpidHelper& get_qpid_helper();
    };
};

#endif //QPID_QPIDHELPER_H