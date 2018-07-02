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

class Table;

class SQLiteDatabase;
namespace re_common{
    class UserEvent;
    class LifecycleEvent;
    class WorkloadEvent;
    class ComponentUtilizationEvent;
    class MessageEvent;
}

class ModelProtoHandler : public ProtoHandler{
    public:
        ModelProtoHandler(SQLiteDatabase& database);
        ~ModelProtoHandler();

        void BindCallbacks(zmq::ProtoReceiver& receiver);
        
    private:
        //Table creation
        void CreatePortEventTable();
        void CreateComponentEventTable();
        void CreateUserEventTable();
        void CreateWorkloadEventTable();
        void CreateComponentUtilizationTable();

        //Callback functions
        void ProcessLifecycleEvent(const re_common::LifecycleEvent& message);
        void ProcessUserEvent(const re_common::UserEvent& message);
        void ProcessWorkloadEvent(const re_common::WorkloadEvent& message);
        void ProcessComponentUtilizationEvent(const re_common::ComponentUtilizationEvent& message);

        //Members
        SQLiteDatabase& database_;
        std::unordered_map<std::string, Table*> table_map_;
        std::set<std::string> registered_nodes_;
};

#endif //LOGAN_SERVER_MODELPROTOHANDLER_H
