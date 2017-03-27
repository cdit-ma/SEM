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

#ifndef LOGAN_SERVER_SERVER_H
#define LOGAN_SERVER_SERVER_H

#include <string>
#include <vector>
#include <map>
#include <set>

class SQLiteDatabase;
namespace zmq{ class ProtoReceiver; }
class ProtoHandler;

class Server{
    public:
        Server(std::string database_path, std::vector<std::string> addresses);
        ~Server();

        void AddProtoHandler(ProtoHandler* handler);
        void Start();

    private:
        SQLiteDatabase* database_ = 0;
        zmq::ProtoReceiver* receiver_ = 0;
        bool started_ = false;

        std::vector<ProtoHandler*> handler_list_;
};

#endif //LOGAN_SERVER_SERVER_H
