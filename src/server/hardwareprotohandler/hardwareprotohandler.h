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

#ifndef LOGAN_SERVER_HARDWAREPROTOHANDLER_H
#define LOGAN_SERVER_HARDWAREPROTOHANDLER_H

#include "../protohandler.h"

#include <unordered_map>
#include <set>
#include <google/protobuf/message_lite.h>

class Table;
class SQLiteDatabase;
namespace re_common{
    class SystemInfo;
    class SystemStatus;
}

class HardwareProtoHandler : public ProtoHandler{
    public:
        HardwareProtoHandler(SQLiteDatabase& database);
        ~HardwareProtoHandler();

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
        void ProcessSystemStatus(const re_common::SystemStatus& status);
        void ProcessOneTimeSystemInfo(const re_common::SystemInfo& info);

        //Members
        SQLiteDatabase& database_;
        std::unordered_map<std::string, Table*> table_map_;
        std::set<std::string> registered_nodes_;
};

#endif //LOGAN_SERVER_HARDWAREPROTOHANDLER_H
