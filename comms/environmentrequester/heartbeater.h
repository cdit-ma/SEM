#ifndef ENVIRONMENTREQESTER_HEARTBEATER_H
#define ENVIRONMENTREQESTER_HEARTBEATER_H

#include "../protorequester/protorequester.hpp"

class Heartbeater{
    public:
        Heartbeater(const int heartbeat_period, zmq::ProtoRequester& requester);
        void Start();
        void Terminate();

    private:
        void HeartbeatLoop();

        zmq::ProtoRequester& requester_;
};

#endif //ENVIRONMENTREQESTER_HEARTBEATER_H