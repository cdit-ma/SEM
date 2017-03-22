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

#include "protohandler.h"

#include <map>
#include <set>
#include <google/protobuf/message_lite.h>

class Table;

class ModelProtoHandler : public ProtoHandler{
    public:
        ModelProtoHandler();
        ~ModelProtoHandler();

        void ConstructTables(SQLiteDatabase* database);
        void BindCallbacks(zmq::ProtoReceiver* receiver);
        
    private:
        //Table creation
        void CreatePortEventTable();
        void CreateComponentEventTable();
        void CreateMessageEventTable();
        void CreateUserEventTable();
        void CreateWorkloadEventTable();
        void CreateComponentUtilizationTable();

        //Callback functions
        void ProcessLifecycleEvent(google::protobuf::MessageLite* message);
        void ProcessMessageEvent(google::protobuf::MessageLite* message);
        void ProcessUserEvent(google::protobuf::MessageLite* message);
        void ProcessWorkloadEvent(google::protobuf::MessageLite* message);
        void ProcessComponentUtilizationEvent(google::protobuf::MessageLite* message);

        //Members
        SQLiteDatabase* database_;
        std::map<std::string, Table*> table_map_;
        std::set<std::string> registered_nodes_;
};

#endif //LOGAN_SERVER_MODELPROTOHANDLER_H
