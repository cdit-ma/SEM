//
// Created by Cathlyn on 15/01/2020.
//

#ifndef MEDEA_PROJECT_DUMMYRESPONSEBUILDER_H
#define MEDEA_PROJECT_DUMMYRESPONSEBUILDER_H

#include "aggregationmessage.pb.h"
#include <google/protobuf/util/time_util.h>
#include <QDateTime>

using google::protobuf::util::TimeUtil;

class DummyResponseBuilder {
public:

    // Return the start time of the "Passthrough" experiment with experiment_run_id #9
    static int64_t getStartTimeMS()
    {
        QDateTime dt;
        dt.setDate(QDate(2019, 11, 18));
        dt.setTime(QTime(18, 29, 19));
        return dt.toMSecsSinceEpoch();
    }

    // Return the end time of the "Passthrough" experiment experiment_run_id #9
    static int64_t getEndTimeMS()
    {
        QDateTime dt;
        dt.setDate(QDate(2019, 11, 18));
        dt.setTime(QTime(18, 29, 47));
        return dt.toMSecsSinceEpoch();
    }

    // Return an empty NetworkUtilisationResponse
    static AggServer::NetworkUtilisationResponse getEmptyResponse()
    {
        AggServer::NetworkUtilisationResponse response;
        return response;
    }

    static AggServer::NetworkUtilisationResponse getSingleEventResponse()
    {
        AggServer::NetworkUtilisationResponse response;
        auto node_events = constructNodeNetworkEvents(response);
        auto interface_events = constructInterfaceNetworkEvents(*node_events);

        // Add a NetworkUtilisationEvent to the InterfaceNetworkEvents
        fillEvent(*interface_events->add_events(),
                  1000,
                  1000,
                  1000,
                  1500,
                  getStartTimeMS() + 10000);

        return response;
    }

    static AggServer::NetworkUtilisationResponse getMultiEventsResponse()
    {
        AggServer::NetworkUtilisationResponse response;
        auto node_events = constructNodeNetworkEvents(response);
        auto interface_events = constructInterfaceNetworkEvents(*node_events);

        srand(10);

        // Add NetworkUtilisationEvents to the InterfaceNetworkEvents
        float random_proportion = rand() / (float)RAND_MAX;
        int num_events = 5 + 10 * random_proportion;
        auto time = getStartTimeMS();
        auto random_range = (getEndTimeMS() - getStartTimeMS()) / num_events;

        for (int i = 0; i < num_events; i++) {
            time += random_range;
            fillEvent(*interface_events->add_events(),
                      0,
                      0,
                      1000 + (rand() % 10) * 100,
                      1000 + (rand() % 10) * 100,
                      time);
        }

        return response;
    }

private:
    static AggServer::NodeNetworkEvents* constructNodeNetworkEvents(AggServer::NetworkUtilisationResponse& response)
    {
        // Add a NodeNetworkEvents to the NetworkUtilisationResponse
        auto node_events = response.add_node_network_events();
        setNodeInfo(*node_events->mutable_node_info());
        return node_events;
    }

    static AggServer::InterfaceNetworkEvents* constructInterfaceNetworkEvents(AggServer::NodeNetworkEvents& node_network_events)
    {
        // Add an InterfaceNetworkEvents to the NodeNetworkEvents
        auto interface_events = node_network_events.add_events();
        interface_events->set_interface_mac_addr(getMacAddress());
        return interface_events;
    }

    static void fillEvent(AggServer::NetworkUtilisationEvent& event,
                          int64_t packets_sent,
                          int64_t packets_received,
                          int64_t bytes_sent,
                          int64_t bytes_received,
                          int64_t time_ms)
    {
        event.set_packets_sent(packets_sent);
        event.set_packets_received(packets_received);
        event.set_bytes_sent(bytes_sent);
        event.set_bytes_received(bytes_received);
        *event.mutable_time() = TimeUtil::MillisecondsToTimestamp(time_ms);
    }

    static void setNodeInfo(AggServer::Node& node)
    {
        node.set_hostname("testNode");
        node.set_ip("192.168.111.240");
    }

    static const std::string& getMacAddress()
    {
        static std::string interface_mac_addr = "90-2B-34-DF-F0-38";
        return interface_mac_addr;
    }
};

#endif //MEDEA_PROJECT_DUMMYRESPONSEBUILDER_H
