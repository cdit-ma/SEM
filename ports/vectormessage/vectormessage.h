#ifndef VECTORMESSAGE_H
#define VECTORMESSAGE_H

#include "basemessage.h"
#include <string>
#include <vector>

class VectorMessage : public BaseMessage{            

public:
    VectorMessage();
    void dataName(const std::string val);
    void data(std::vector<long> val);

    std::string dataName();
    std::vector<long> data();
    std::vector<long>& data_ptr();
private:
    std::vector<long> data_;
    std::string dataName_;
};

#endif //VECTORMESSAGE_H