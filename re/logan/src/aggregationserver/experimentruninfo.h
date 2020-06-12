//
// Created by DIG-Jackson on 8/10/2019.
//

#ifndef RE_EXPERIMENTRUNINFO_H
#define RE_EXPERIMENTRUNINFO_H

#include <map>
#include <memory>
#include <string>

#include "modeleventprotohandler.h"
#include "systemeventprotohandler.h"
#include <zmq/protoreceiver/protoreceiver.h>

struct ExperimentRunInfo {
    std::string name;
    int job_num;
    bool running;
    int experiment_run_id;
    std::unique_ptr<zmq::ProtoReceiver> receiver;
    std::unique_ptr<SystemEventProtoHandler> system_handler;
    std::unique_ptr<ModelEventProtoHandler> model_handler;

    std::map<std::string, int> hostname_node_id_cache;
};

#endif // RE_EXPERIMENTRUNINFO_H
