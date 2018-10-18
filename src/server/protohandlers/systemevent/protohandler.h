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
#include <set>
#include <google/protobuf/message_lite.h>

#include "../../protohandler.h"
#include "../../table.h"

class SQLiteDatabase;
namespace SystemEvent{
    class StatusEvent;
    class InfoEvent;
    
    class ProtoHandler : public ::ProtoHandler{
        public:
            ProtoHandler(SQLiteDatabase& database);
            ~ProtoHandler();

            void BindCallbacks(zmq::ProtoReceiver& receiver);
        private:
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

            //Members
            SQLiteDatabase& database_;
            std::unordered_map<std::string, Table*> table_map_;
            std::set<std::string> registered_nodes_;
    };
};

#endif //LOGAN_SERVER_HARDWAREPROTOHANDLER_H
