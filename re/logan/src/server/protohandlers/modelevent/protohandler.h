#ifndef LOGAN_SERVER_PROTOHANDLERS_MODELEVENT_H
#define LOGAN_SERVER_PROTOHANDLERS_MODELEVENT_H

#include <unordered_map>
#include <memory>
#include <set>

#include <mutex>
#include "protoreceiver.h"
#include "modelevent.pb.h"

#include "../../sqlitedatabase.h"
#include "../../protohandler.h"
#include "../../tableinsert.h"
#include "../../table.h"

namespace ModelEvent{
    class ProtoHandler : public ::ProtoHandler{
    public:
        ProtoHandler(SQLiteDatabase& database);
        ~ProtoHandler();
        void BindCallbacks(zmq::ProtoReceiver& receiver);
    private:
        Table& GetTable(const std::string& table_name);
        bool GotTable(const std::string& table_name);

        //Table creation
        void CreateLifecycleTable();
        void CreateWorkloadTable();
        void CreateUtilizationTable();

        //Callback functions
        void ProcessLifecycleEvent(const ModelEvent::LifecycleEvent& message);
        void ProcessWorkloadEvent(const ModelEvent::WorkloadEvent& message);
        void ProcessUtilizationEvent(const ModelEvent::UtilizationEvent& message);

        //Add columns functions
        static void AddInfoColumns(Table& table);
        static void AddComponentColumns(Table& table);
        static void AddPortColumns(Table& table);
        static void AddWorkerColumns(Table& table);

        //Bind columns functions
        void BindInfoColumns(TableInsert& row, const ModelEvent::Info& info);
        static void BindComponentColumns(TableInsert& row, const ModelEvent::Component& component);
        static void BindWorkerColumns(TableInsert& row, const ModelEvent::Worker& worker);
        static void BindPortColumns(TableInsert& row, const ModelEvent::Port& port);

        std::mutex mutex_;
        uint64_t rx_count_ = 0;
        SQLiteDatabase& database_;
        std::unordered_map<std::string, std::unique_ptr<Table> > tables_;
    };
};

#endif //LOGAN_SERVER_PROTOHANDLERS_MODELEVENT_H
