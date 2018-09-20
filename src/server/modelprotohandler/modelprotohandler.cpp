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

#include "modelprotohandler.h"

#include <functional>
#include <chrono>

#include "../sqlitedatabase.h"
#include "../table.h"
#include "../tableinsert.h"

#include <proto/modelevent/modelevent.pb.h>
#include <zmq/protoreceiver/protoreceiver.h>

//Types
const std::string LOGAN_DECIMAL = "DECIMAL";
const std::string LOGAN_VARCHAR = "VARCHAR";
const std::string LOGAN_INT = "INTEGER";

//Common column names
const std::string LOGAN_TIMEOFDAY = "timeofday";
const std::string LOGAN_HOSTNAME = "hostname";
const std::string LOGAN_MESSAGE_ID = "id";
const std::string LOGAN_NAME = "name";
const std::string LOGAN_TYPE = "type";
const std::string LOGAN_MESSAGE = "message";
const std::string LOGAN_EXPERIMENT_NAME = "experiment_name";



//Common column names
const std::string LOGAN_COMPONENT_NAME = "component_name";
const std::string LOGAN_COMPONENT_ID = "component_id";
const std::string LOGAN_COMPONENT_TYPE = "component_type";
const std::string LOGAN_PORT_NAME = "port_name";
const std::string LOGAN_PORT_ID = "port_id";
const std::string LOGAN_PORT_KIND = "port_kind";
const std::string LOGAN_PORT_TYPE = "port_type";
const std::string LOGAN_PORT_MIDDLEWARE = "port_middleware";
const std::string LOGAN_WORKER_NAME = "worker_name";
const std::string LOGAN_WORKER_TYPE = "worker_type";
const std::string LOGAN_EVENT = "event";


//Model Table names
const std::string LOGAN_LIFECYCLE_PORT_TABLE = "Model_Lifecycle_EventPort";
const std::string LOGAN_LIFECYCLE_COMPONENT_TABLE = "Model_Lifecycle_Component";
const std::string LOGAN_EVENT_USER_TABLE = "Model_Event_User";
const std::string LOGAN_EVENT_WORKLOAD_TABLE = "Model_Event_Workload";
const std::string LOGAN_EVENT_COMPONENT_TABLE = "Model_Event_Component";

ModelProtoHandler::ModelProtoHandler(SQLiteDatabase& database):
    ProtoHandler(),
    database_(database)
{
    CreatePortEventTable();
    CreateComponentEventTable();
    CreateUserEventTable();
    CreateWorkloadEventTable();
    CreateComponentUtilizationTable();
}

void ModelProtoHandler::BindCallbacks(zmq::ProtoReceiver& receiver){
    receiver.RegisterProtoCallback<re_common::UserEvent>(std::bind(&ModelProtoHandler::ProcessUserEvent, this, std::placeholders::_1));
    receiver.RegisterProtoCallback<re_common::LifecycleEvent>(std::bind(&ModelProtoHandler::ProcessLifecycleEvent, this, std::placeholders::_1));
    receiver.RegisterProtoCallback<re_common::WorkloadEvent>(std::bind(&ModelProtoHandler::ProcessWorkloadEvent, this, std::placeholders::_1));
    receiver.RegisterProtoCallback<re_common::ComponentUtilizationEvent>(std::bind(&ModelProtoHandler::ProcessComponentUtilizationEvent, this, std::placeholders::_1));
}


void ModelProtoHandler::CreatePortEventTable(){
    if(table_map_.count(LOGAN_LIFECYCLE_PORT_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_LIFECYCLE_PORT_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_EXPERIMENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_TYPE, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_KIND, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_TYPE, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_MIDDLEWARE, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_EVENT, LOGAN_VARCHAR);
    t->Finalize();

    table_map_[LOGAN_LIFECYCLE_PORT_TABLE] = t;
    database_.QueueSqlStatement(t->get_table_construct_statement());
}

void ModelProtoHandler::CreateComponentEventTable(){
    if(table_map_.count(LOGAN_LIFECYCLE_COMPONENT_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_LIFECYCLE_COMPONENT_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_EXPERIMENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_TYPE, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_EVENT, LOGAN_VARCHAR);
    t->Finalize();

    table_map_[LOGAN_LIFECYCLE_COMPONENT_TABLE] = t;
    database_.QueueSqlStatement(t->get_table_construct_statement());
}

void ModelProtoHandler::CreateUserEventTable(){
    if(table_map_.count(LOGAN_EVENT_USER_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_EVENT_USER_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_EXPERIMENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_TYPE, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_TYPE, LOGAN_VARCHAR);
    t->Finalize();

    table_map_[LOGAN_EVENT_USER_TABLE] = t;
    database_.QueueSqlStatement(t->get_table_construct_statement());
}

void ModelProtoHandler::CreateWorkloadEventTable(){
    if(table_map_.count(LOGAN_EVENT_WORKLOAD_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_EVENT_WORKLOAD_TABLE);
    //Info
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_EXPERIMENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    //Component specific
    t->AddColumn(LOGAN_COMPONENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_TYPE, LOGAN_VARCHAR);

    //Workload specific info
    t->AddColumn("workload_id", LOGAN_INT);
    t->AddColumn(LOGAN_WORKER_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_WORKER_TYPE, LOGAN_VARCHAR);
    t->AddColumn("function", LOGAN_VARCHAR);
    t->AddColumn("event_type", LOGAN_VARCHAR);
    t->AddColumn("args", LOGAN_VARCHAR);

    t->Finalize();
    table_map_[LOGAN_EVENT_WORKLOAD_TABLE] = t;
    database_.QueueSqlStatement(t->get_table_construct_statement());
}

void ModelProtoHandler::CreateComponentUtilizationTable(){
    if(table_map_.count(LOGAN_EVENT_COMPONENT_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_EVENT_COMPONENT_TABLE);
    //Info
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_EXPERIMENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    //Component specific
    t->AddColumn(LOGAN_COMPONENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_TYPE, LOGAN_VARCHAR);
    //Port specific
    t->AddColumn(LOGAN_PORT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_KIND, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_TYPE, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_MIDDLEWARE, LOGAN_VARCHAR);

    t->AddColumn("port_event_id", LOGAN_INT);
    t->AddColumn(LOGAN_TYPE, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE, LOGAN_VARCHAR);
    t->Finalize();
    table_map_[LOGAN_EVENT_COMPONENT_TABLE] = t;
    database_.QueueSqlStatement(t->get_table_construct_statement());
}

void ModelProtoHandler::ProcessLifecycleEvent(const re_common::LifecycleEvent& event){
    if(event.has_port() && event.has_component()){
        //Process port event
        auto ins = table_map_[LOGAN_LIFECYCLE_PORT_TABLE]->get_insert_statement();
        ins.BindDouble(LOGAN_TIMEOFDAY, event.info().timestamp());
        ins.BindString(LOGAN_EXPERIMENT_NAME, event.info().experiment_name());
        ins.BindString(LOGAN_HOSTNAME, event.info().hostname());
        ins.BindString(LOGAN_COMPONENT_NAME, event.component().name());
        ins.BindString(LOGAN_COMPONENT_ID, event.component().id());
        ins.BindString(LOGAN_COMPONENT_TYPE, event.component().type());
        ins.BindString(LOGAN_PORT_NAME, event.port().name());
        ins.BindString(LOGAN_PORT_ID, event.port().id());
        ins.BindString(LOGAN_PORT_KIND, re_common::Port::Kind_Name(event.port().kind()));
        ins.BindString(LOGAN_PORT_TYPE, event.port().type());
        ins.BindString(LOGAN_PORT_MIDDLEWARE, event.port().middleware());
        ins.BindString(LOGAN_EVENT, re_common::LifecycleEvent::Type_Name(event.type()));

        database_.QueueSqlStatement(ins.get_statement());
    }

    else if(event.has_component()){
            auto ins = table_map_[LOGAN_LIFECYCLE_COMPONENT_TABLE]->get_insert_statement();
            ins.BindDouble(LOGAN_TIMEOFDAY, event.info().timestamp());
            ins.BindString(LOGAN_EXPERIMENT_NAME, event.info().experiment_name());
            ins.BindString(LOGAN_HOSTNAME, event.info().hostname());
            ins.BindString(LOGAN_COMPONENT_NAME, event.component().name());
            ins.BindString(LOGAN_COMPONENT_ID, event.component().id());
            ins.BindString(LOGAN_COMPONENT_TYPE, event.component().type());
            ins.BindString(LOGAN_EVENT, re_common::LifecycleEvent::Type_Name(event.type()));
            database_.QueueSqlStatement(ins.get_statement());
    }
}

void ModelProtoHandler::ProcessUserEvent(const re_common::UserEvent& event){
    auto ins = table_map_[LOGAN_EVENT_USER_TABLE]->get_insert_statement();
    ins.BindDouble(LOGAN_TIMEOFDAY, event.info().timestamp());
    ins.BindString(LOGAN_HOSTNAME, event.info().hostname());
    ins.BindString(LOGAN_COMPONENT_NAME, event.component().name());
    ins.BindString(LOGAN_COMPONENT_ID, event.component().id());
    ins.BindString(LOGAN_COMPONENT_TYPE, event.component().type());
    ins.BindString(LOGAN_MESSAGE, event.message());
    ins.BindString(LOGAN_TYPE, re_common::UserEvent::Type_Name(event.type()));
    database_.QueueSqlStatement(ins.get_statement());
}

void ModelProtoHandler::ProcessWorkloadEvent(const re_common::WorkloadEvent& event){
    auto ins = table_map_[LOGAN_EVENT_WORKLOAD_TABLE]->get_insert_statement();
    //Info
    ins.BindDouble(LOGAN_TIMEOFDAY, event.info().timestamp());
    ins.BindString(LOGAN_EXPERIMENT_NAME, event.info().experiment_name());
    ins.BindString(LOGAN_HOSTNAME, event.info().hostname());

    //Component
    ins.BindString(LOGAN_COMPONENT_ID, event.component().id());
    ins.BindString(LOGAN_COMPONENT_NAME, event.component().name());
    ins.BindString(LOGAN_COMPONENT_TYPE, event.component().type());

    //Workload
    ins.BindString(LOGAN_WORKER_NAME, event.name());
    ins.BindInt("workload_id", event.id());
    ins.BindString(LOGAN_WORKER_TYPE, event.type());
    ins.BindString("function", event.function());
    ins.BindString("event_type", re_common::WorkloadEvent::Type_Name(event.event_type()));
    ins.BindString("args", event.args());
    database_.QueueSqlStatement(ins.get_statement());
}

void ModelProtoHandler::ProcessComponentUtilizationEvent(const re_common::ComponentUtilizationEvent& event){
    auto ins = table_map_[LOGAN_EVENT_COMPONENT_TABLE]->get_insert_statement();

    //Info
    ins.BindDouble(LOGAN_TIMEOFDAY, event.info().timestamp());
    ins.BindString(LOGAN_EXPERIMENT_NAME, event.info().experiment_name());
    ins.BindString(LOGAN_HOSTNAME, event.info().hostname());

    //Component
    ins.BindString(LOGAN_COMPONENT_NAME, event.component().name());
    ins.BindString(LOGAN_COMPONENT_ID, event.component().id());
    ins.BindString(LOGAN_COMPONENT_TYPE, event.component().type());

    //Port
    ins.BindString(LOGAN_PORT_NAME, event.port().name());
    ins.BindString(LOGAN_PORT_ID, event.port().id());
    ins.BindString(LOGAN_PORT_KIND, re_common::Port::Kind_Name(event.port().kind()));
    ins.BindString(LOGAN_PORT_TYPE, event.port().type());
    ins.BindString(LOGAN_PORT_MIDDLEWARE, event.port().middleware());

    ins.BindInt("port_event_id", event.port_event_id());
    ins.BindString(LOGAN_TYPE, re_common::ComponentUtilizationEvent::Type_Name(event.type()));
    ins.BindString(LOGAN_MESSAGE, event.message());

    database_.QueueSqlStatement(ins.get_statement());
}
