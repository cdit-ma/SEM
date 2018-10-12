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

#ifndef LOGAN_SERVER_MODELPROTOHANDLER_H
#define LOGAN_SERVER_MODELPROTOHANDLER_H

#include "../protohandler.h"

#include <unordered_map>
#include <set>
#include <google/protobuf/message_lite.h>
#include <memory>

#include "../table.h"

class SQLiteDatabase;
namespace re_common{
    class LifecycleEvent;
    class WorkloadEvent;
    class UtilizationEvent;
}

class ModelProtoHandler : public ProtoHandler{
    public:
        ModelProtoHandler(SQLiteDatabase& database);
        ~ModelProtoHandler();

        void BindCallbacks(zmq::ProtoReceiver& receiver);
        
    private:
        //Table creation
        void CreateLifecycleTable();
        void CreateWorkloadTable();
        void CreateUtilizationTable();

        //Callback functions
        void ProcessLifecycleEvent(const re_common::LifecycleEvent& message);
        void ProcessWorkloadEvent(const re_common::WorkloadEvent& message);
        void ProcessUtilizationEvent(const re_common::UtilizationEvent& message);

        Table& GetTable(const std::string& table_name);
        bool GotTable(const std::string& table_name);

        //Members
        SQLiteDatabase& database_;

        std::unordered_map<std::string, std::unique_ptr<Table> > tables_;
        std::set<std::string> registered_nodes_;
};

#endif //LOGAN_SERVER_MODELPROTOHANDLER_H
