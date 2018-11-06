/* logan
 * Copyright (C) 2016-2017 The University of Adelaide
 *
 * This file is part of "logan"
 *
 * "logan" is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * "logan" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef LOGAN_SERVER_PROTOHANDLERS_SYSTEMEVENT_H
#define LOGAN_SERVER_PROTOHANDLERS_SYSTEMEVENT_H

#ifdef _WIN32
#define NOMINMAX 1
#endif //_WIN32

#include <unordered_map>
#include <memory>
#include <set>
#include <mutex>

#include <zmq/protoreceiver/protoreceiver.h>
#include <proto/systemevent/systemevent.pb.h>

#include "../../sqlitedatabase.h"
#include "../../protohandler.h"
#include "../../tableinsert.h"
#include "../../table.h"

namespace SystemEvent{    
    class ProtoHandler : public ::ProtoHandler{
        public:
            ProtoHandler(SQLiteDatabase& database);
            ~ProtoHandler();
            void BindCallbacks(zmq::ProtoReceiver& receiver);
        private:
            Table& GetTable(const std::string& table_name);
            bool GotTable(const std::string& table_name);

            void ExecuteTableStatement(TableInsert& row);

            //Table creation
            void CreateSystemStatusTable();
            void CreateSystemInfoTable();
            void CreateCpuTable();
            void CreateFileSystemTable();
            void CreateFileSystemInfoTable();
            void CreateInterfaceTable();
            void CreateInterfaceInfoTable();
            void CreateProcessTable();
            void CreateProcessInfoTable();

            //Callback functions
            void ProcessStatusEvent(const SystemEvent::StatusEvent& status);
            void ProcessInfoEvent(const SystemEvent::InfoEvent& info);

            //Add/Bind columns functions
            static void AddInfoColumns(Table& table);
            static void BindInfoColumns(TableInsert& row, const std::string& time, const std::string& host_name, const int64_t message_id);

            std::mutex mutex_;
            uint64_t rx_count_ = 0;

            SQLiteDatabase& database_;
            std::unordered_map<std::string, std::unique_ptr<Table> > tables_;
            std::set<std::string> registered_nodes_;
    };
};

#endif //LOGAN_SERVER_HARDWAREPROTOHANDLER_H
