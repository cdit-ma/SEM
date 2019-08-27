#include "databaseclient.h"

#include <iostream>
#include <sstream>

#include <algorithm>

#include <chrono>

#include "utils.h"

DatabaseClient::DatabaseClient(const std::string& connection_details) :
    connection_(connection_details),
    batched_connection_(connection_details)
{
}

DatabaseClient::~DatabaseClient()
{
    std::lock_guard<std::mutex> trans_lock(batched_transaction_mutex_);
    FlushBatchedTransaction();
}

void DatabaseClient::InitSchemaFrom(const std::string& sql_string)
{
    try {
        pqxx::work transaction(connection_, "InitSchemaTransaction");
        transaction.exec(sql_string);
        transaction.commit();
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while trying to initialise the database schema: "
                  << e.what() << std::endl;
        throw;
    }
}

void DatabaseClient::CreateTable(const std::string& table_name,
                                 const std::vector<std::pair<std::string, std::string>>& columns)
{
    std::stringstream query_stream;

    query_stream << "CREATE TABLE " << table_name << "(" << std::endl;
    const auto& last_col = std::prev(columns.end());
    std::for_each(columns.begin(), last_col,
                  [&query_stream](const std::pair<std::string, std::string>& column_def) {
                      query_stream << column_def.first << " " << column_def.second << ','
                                   << std::endl;
                  });
    query_stream << last_col->first << " " << last_col->second << std::endl;
    query_stream << ")" << std::endl;

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        pqxx::work transaction(connection_, "CreateTableTransaction");
        transaction.exec(query_stream.str());
        transaction.commit();
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while trying to create a table in the database: "
                  << e.what() << std::endl;
        throw;
    }
}

int DatabaseClient::InsertValues(const std::string& table_name,
                                 const std::vector<std::string>& columns,
                                 const std::vector<std::string>& values)
{
    std::stringstream query_stream;
    std::string id_column = strip_schema(table_name) + "ID";
    int id_value = -1;

    query_stream << "INSERT INTO " << table_name;
    if(columns.size() > 0) {
        query_stream << " (";
        for(unsigned int i = 0; i < columns.size() - 1; i++) {
            query_stream << columns.at(i) << ',';
        }
        query_stream << columns.at(columns.size() - 1) << ')';
    }

    query_stream << std::endl << " VALUES (";
    for(unsigned int i = 0; i < values.size() - 1; i++) {
        query_stream << values.at(i) << ',';
    }
    query_stream << values.at(values.size() - 1) << ")" << std::endl;
    query_stream << "RETURNING " << strip_schema(table_name) << "." << id_column << ";"
                 << std::endl;

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        auto start = std::chrono::steady_clock::now();
        auto& transaction = AquireBatchedTransaction();

        auto made_transaction = std::chrono::steady_clock::now();

        try {
            auto&& result = transaction.exec(query_stream.str());

            auto executed_trans = std::chrono::steady_clock::now();
            ReleaseBatchedTransaction();
            FlushBatchedTransaction(); // TODO: remove this once we can actually figure out when
                                       // things are going down

            auto all_done = std::chrono::steady_clock::now();
            auto create_delay = std::chrono::duration_cast<std::chrono::microseconds>(
                made_transaction - start);
            auto execute_delay = std::chrono::duration_cast<std::chrono::microseconds>(
                executed_trans - made_transaction);
            auto commit_delay = std::chrono::duration_cast<std::chrono::microseconds>(
                all_done - executed_trans);

            // std::cerr << "Create : " << create_delay.count() << ", execute: " <<
            // execute_delay.count() << ", commit: " << commit_delay.count() << '\n';

            std::string lower_id_column(id_column);
            std::transform(lower_id_column.begin(), lower_id_column.end(), lower_id_column.begin(),
                           ::tolower);
            unsigned int id_colnum = result.column_number(lower_id_column);

            for(const auto& row : result) {
                for(unsigned int colnum = 0; colnum < row.size(); colnum++) {
                    if(colnum == id_colnum) {
                        id_value = row[colnum].as<int>();
                        return id_value;
                    }
                }
            }
        } catch(const std::exception& ex) {
            std::cerr << "Failing on exec??\n";
            AbortBatchedTransaction();
            throw;
        }
        throw std::runtime_error("ID associated with values not found in database query result");
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while trying to insert values into the database: "
                  << e.what() << std::endl;
        std::cerr << query_stream.str() << std::endl;
        std::cerr << "ID column name = " << id_column << std::endl;
        throw;
    }
}

void DatabaseClient::InsertPubSubValues(int from_port_id,
                                        const std::string& to_port_graphml,
                                        int experiment_run_id)
{
    std::stringstream query_stream;

    query_stream << "INSERT INTO PubSubConnection (PubPortID, SubPortID)";

    query_stream << " VALUES (" << quote(from_port_id) << ", getPortFromGraphml("
                 << quote(experiment_run_id) << ", " << quote(to_port_graphml) << "))";
    query_stream << " ON CONFLICT DO NOTHING";

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        auto start = std::chrono::steady_clock::now();
        auto& transaction = AquireBatchedTransaction();

        auto made_transaction = std::chrono::steady_clock::now();

        try {
            auto&& result = transaction.exec(query_stream.str());

            auto executed_trans = std::chrono::steady_clock::now();
            ReleaseBatchedTransaction();
            FlushBatchedTransaction(); // TODO: remove this once we can actually figure out when
                                       // things are going down

        } catch(const std::exception& ex) {
            std::cerr << "Exception thrown on exec for batched transaction\n";
            AbortBatchedTransaction();
            throw;
        }
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while trying to insert values into the database: "
                  << e.what() << std::endl;
        std::cerr << query_stream.str() << std::endl;
        throw;
    }
}

void DatabaseClient::InsertReqRepValues(int from_port_id,
                                        const std::string& to_port_graphml,
                                        int experiment_run_id)
{
    std::stringstream query_stream;

    query_stream << "INSERT INTO ReqRepConnection (ReqPortID, RepPortID)";

    query_stream << " VALUES (" << quote(from_port_id) << ", getPortFromGraphml("
                 << quote(experiment_run_id) << ", " << quote(to_port_graphml) << "))";
    query_stream << " ON CONFLICT DO NOTHING";

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        auto start = std::chrono::steady_clock::now();
        auto& transaction = AquireBatchedTransaction();

        auto made_transaction = std::chrono::steady_clock::now();

        try {
            auto&& result = transaction.exec(query_stream.str());

            auto executed_trans = std::chrono::steady_clock::now();
            ReleaseBatchedTransaction();
            FlushBatchedTransaction(); // TODO: remove this once we can actually figure out when
                                       // things are going down

        } catch(const std::exception& ex) {
            std::cerr << "Exception thrown on exec for batched transaction\n";
            AbortBatchedTransaction();
            throw;
        }
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while trying to insert values into the database: "
                  << e.what() << std::endl;
        std::cerr << query_stream.str() << std::endl;
        throw;
    }
}

int DatabaseClient::InsertValuesUnique(const std::string& table_name,
                                       const std::vector<std::string>& columns,
                                       const std::vector<std::string>& values,
                                       const std::vector<std::string>& unique_cols)
{
    std::string id_column = strip_schema(table_name) + "ID";
    int id_value = -1;
    std::vector<std::string> unique_vals = std::vector<std::string>(unique_cols.size());

    std::stringstream query_stream;

    query_stream << "WITH i AS (" << std::endl;
    query_stream << "INSERT INTO " << connection_.esc(table_name);
    if(columns.size() > 0) {
        // Iterate through column names
        query_stream << " (";
        for(unsigned int i = 0; i < columns.size() - 1; i++) {
            query_stream << connection_.esc(columns.at(i)) << ',';
            // Pull out the values stored in a unique column to be associated with the unique
            // columns
            for(unsigned int j = 0; j < unique_cols.size(); j++) {
                if(unique_cols.at(j) == columns.at(i)) {
                    unique_vals.at(j) = values.at(i);
                }
            }
        }
        int last_col = columns.size() - 1;
        query_stream << columns.at(last_col) << ')';
        for(unsigned int j = 0; j < unique_cols.size(); j++) {
            if(unique_cols.at(j) == columns.at(last_col)) {
                unique_vals.at(j) = values.at(last_col);
            }
        }
    }

    query_stream << std::endl << " VALUES (";
    for(unsigned int i = 0; i < values.size() - 1; i++) {
        query_stream << connection_.quote(values.at(i)) << ",";
    }
    query_stream << connection_.quote(values.at(values.size() - 1)) << ")" << std::endl;

    query_stream << "ON CONFLICT " << BuildColTuple(unique_cols) << " DO UPDATE" << std::endl;
    // query_stream << "ON CONFLICT  ON CONSTRAINT " << BuildColTuple(unique_cols) << " DO UPDATE"
    // << std::endl;
    query_stream << "SET " << id_column << " = -1 WHERE FALSE" << std::endl;

    query_stream << "RETURNING " << id_column << ")" << std::endl;

    query_stream << "SELECT " << id_column << " FROM i" << std::endl;
    query_stream << "UNION ALL" << std::endl;
    query_stream << "SELECT " << id_column << " FROM " << table_name << " "
                 << BuildWhereAllEqualClause(unique_cols, unique_vals) << std::endl;
    // query_stream << "SELECT " << id_column << " FROM " << table_name << " WHERE " << unique_col
    // << " = " << connection_.quote(unique_val) << std::endl;
    query_stream << "LIMIT 1" << std::endl;

    // std::cout << query_stream.str() << std::endl;

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        pqxx::work transaction(connection_, "InsertValuesUniqueTransaction");
        auto&& result = transaction.exec(query_stream.str());
        transaction.commit();

        std::string lower_id_column(id_column);
        std::transform(lower_id_column.begin(), lower_id_column.end(), lower_id_column.begin(),
                       ::tolower);
        unsigned int id_colnum = result.column_number(lower_id_column);

        for(const auto& row : result) {
            for(unsigned int colnum = 0; colnum < row.size(); colnum++) {
                if(colnum == id_colnum) {
                    id_value = row.at(colnum).as<int>();
                    return id_value;
                }
            }
        }
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while inserting values uniquely: " << e.what()
                  << std::endl;
        throw;
    }
    /*

    WITH ins AS (
      INSERT INTO city
            (name_city , country , province , region , cap , nationality )
      VALUES(name_city1, country1, province1, region1, cap1, nationality1)
      ON     CONFLICT (name_city) DO UPDATE
      SET    name_city = NULL WHERE FALSE  -- never executed, but locks the row!
      RETURNING id_city
      )
    SELECT id_city FROM ins
    UNION  ALL
    SELECT id_city FROM city WHERE name_city = name_city1  -- only executed if no INSERT
    LIMIT  1;

    */

    return id_value;
}

const pqxx::result DatabaseClient::GetValues(const std::string table_name,
                                             const std::vector<std::string>& columns,
                                             const std::string& query)
{
    std::stringstream query_stream;

    query_stream << "SELECT ";
    if(columns.size() > 0) {
        for(unsigned int i = 0; i < columns.size() - 1; i++) {
            query_stream << /*connection_.quote(*/ columns.at(i) /*)*/ << ", ";
        }
        query_stream << /*connection_.quote(*/ columns.at(columns.size() - 1) /*)*/;
    }
    query_stream << std::endl;
    query_stream << " FROM " << table_name << std::endl;
    if(query != "") {
        query_stream << " WHERE (" << query << ")";
    }
    query_stream << ";" << std::endl;

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        pqxx::work transaction(connection_, "GetValuesTransaction");
        const auto& pg_result = transaction.exec(query_stream.str());
        transaction.commit();

        return pg_result;
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while querying values from the database: " << std::endl;
        std::cerr << query_stream.str() << std::endl;
        std::cerr << e.what() << std::endl;
        throw;
    }
}

int DatabaseClient::GetID(const std::string& table_name, const std::string& query)
{
    std::string&& id_column_name = table_name + "ID";

    const auto& results = GetValues(table_name, {id_column_name}, query);

    if(results.empty()) {
        throw std::runtime_error("No matching ID found in table " + table_name
                                 + " for condition :\n" + query);
    }

    if(results.size() > 1) {
        throw std::runtime_error("More than one " + id_column_name
                                 + " appears to satisfy the following condition:\n" + query + "\n"
                                 + std::to_string(results.size()));
    }

    int id_column_num = results.column_number(id_column_name);
    for(const auto& row : results) {
        return row.at(id_column_num).as<int>();
    }

    throw std::runtime_error(
        "Did not find ID amongst returned database columns when calling GetID on " + table_name);
}

std::optional<int> DatabaseClient::GetMaxValue(const std::string& table_name,
                                               const std::string& column,
                                               const std::string& where_query)
{
    std::stringstream query_stream;

    query_stream << "SELECT max(" << column << ") as maxval FROM " << table_name;
    query_stream << " WHERE (" << where_query << ");";

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        pqxx::work transaction(connection_, "GetMaxValueTransaction");
        const auto& pg_result = transaction.exec(query_stream.str());
        transaction.commit();

        if(pg_result.empty()) {
            return std::optional<int>();
        }

        if(pg_result.size() > 1) {
            throw std::runtime_error("Returned multiple max results in table '" + table_name
                                     + "' matching query '" + where_query
                                     + "'; only one was expected.");
        }

        for(const auto& row : pg_result) {
            const auto& result = row["maxval"];
            if(result.is_null()) {
                return std::optional<int>();
            }
            return result.as<int>();
        }
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while querying values from the database: " << std::endl;
        std::cerr << query_stream.str() << std::endl;
        std::cerr << e.what() << std::endl;
        throw;
    }
}

const pqxx::result
DatabaseClient::GetPortLifecycleEventInfo(int experiment_run_id,
                                          std::string start_time,
                                          std::string end_time,
                                          const std::vector<std::string>& condition_columns,
                                          const std::vector<std::string>& condition_values)
{
    std::stringstream query_stream;

    query_stream << "SELECT PortLifecycleEvent.Type, " << StringToPSQLTimestamp("SampleTime")
                 << " AS SampleTime,\n";
    query_stream << "   Port.Name AS PortName, Port.Type AS PortType, Port.Kind AS PortKind, "
                    "Port.Path AS PortPath, Port.Middleware, Port.GraphmlID AS PortGraphmlID, \n";
    query_stream << "   ComponentInstance.Name AS ComponentInstanceName, ComponentInstance.Path AS "
                    "ComponentInstancePath, ComponentInstance.GraphmlID AS "
                    "ComponentInstanceGraphmlID,\n";
    query_stream << "   Component.Name AS ComponentName, Component.GraphmlID AS "
                    "ComponentGraphmlID, Component.ExperimentRunID AS RunID,\n";
    query_stream << "   Container.Name AS ContainerName, Container.Type as ContainerType, "
                    "Container.GraphmlID AS ContainerGraphmlID,\n";
    query_stream << "   Node.Hostname AS NodeHostname, Node.IP AS NodeIP, Node.GraphmlID AS "
                    "NodeGraphmlID\n";
    query_stream << "FROM PortLifecycleEvent INNER JOIN Port ON PortLifecycleEvent.PortID = "
                    "Port.PortID\n";
    query_stream << "   INNER JOIN ComponentInstance ON Port.ComponentInstanceID = "
                    "ComponentInstance.ComponentInstanceID\n";
    query_stream << "   INNER JOIN Component ON ComponentInstance.ComponentID = "
                    "Component.ComponentID\n";
    query_stream << "   INNER JOIN Container ON ComponentInstance.ContainerID = "
                    "Container.ContainerID\n";
    query_stream << "   INNER JOIN Node ON Container.NodeID = Node.NodeID\n";

    if(condition_columns.size() != 0) {
        query_stream << BuildWhereLikeClause(condition_columns, condition_values) << " AND ";
    } else {
        query_stream << "WHERE ";
    }
    query_stream << "Node.ExperimentRunID = " << experiment_run_id << " AND ";

    query_stream << "PortLifecycleEvent.SampleTime >= '"
                 << /*connection_.quote(*/ start_time /*)*/ << "'";

    if(end_time != AggServer::FormatTimestamp(0.0)) {
        query_stream << "AND PortLifecycleEvent.SampleTime <= '"
                     << /*connection_.quote(*/ end_time /*)*/ << "'";
    }
    query_stream << " ORDER BY PortLifecycleEvent.SampleTime";
    query_stream << std::endl;

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        pqxx::work transaction(connection_, "GetPortLifecycleEventTransaction");
        const auto& pg_result = transaction.exec(query_stream.str());
        transaction.commit();

        return pg_result;
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while querying PortLifecycleEvents: " << e.what()
                  << std::endl;
        throw;
    }
}

const pqxx::result
DatabaseClient::GetWorkloadEventInfo(int experiment_run_id,
                                     std::string start_time,
                                     std::string end_time,
                                     const std::vector<std::string>& condition_columns,
                                     const std::vector<std::string>& condition_values)
{
    std::stringstream query_stream;

    query_stream << "SELECT WorkloadEvent.Type, " << StringToPSQLTimestamp("SampleTime")
                 << " AS SampleTime, WorkloadEvent.Function AS FunctionName, "
                    "WorkloadEvent.Arguments AS Arguments,\n";
    query_stream << "   WorkerInstance.Name AS WorkerInstanceName, WorkerInstance.Path AS "
                    "WorkerInstancePath, WorkerInstance.GraphmlID AS WorkerInstanceGraphmlID, \n";
    query_stream << "   Worker.Name AS WorkerName, Worker.GraphmlID AS WorkerGraphmlID, \n";
    query_stream << "   ComponentInstance.Name AS ComponentInstanceName, ComponentInstance.Path AS "
                    "ComponentInstancePath, ComponentInstance.GraphmlID AS "
                    "ComponentInstanceGraphmlID,\n";
    query_stream << "   Component.Name AS ComponentName, Component.GraphmlID AS "
                    "ComponentGraphmlID, Component.ExperimentRunID AS RunID,\n";
    query_stream << "   Container.Name AS ContainerName, Container.Type as ContainerType, "
                    "Container.GraphmlID AS ContainerGraphmlID,\n";
    query_stream << "   Node.Hostname AS NodeHostname, Node.IP AS NodeIP, Node.GraphmlID AS "
                    "NodeGraphmlID\n";
    query_stream << "FROM WorkloadEvent INNER JOIN WorkerInstance ON "
                    "WorkloadEvent.WorkerInstanceID = WorkerInstance.WorkerInstanceID\n";
    query_stream << "   INNER JOIN Worker ON WorkerInstance.WorkerID = Worker.WorkerID\n";
    query_stream << "   INNER JOIN ComponentInstance ON WorkerInstance.ComponentInstanceID = "
                    "ComponentInstance.ComponentInstanceID\n";
    query_stream << "   INNER JOIN Component ON ComponentInstance.ComponentID = "
                    "Component.ComponentID\n";
    query_stream << "   INNER JOIN Container ON ComponentInstance.ContainerID = "
                    "Container.ContainerID\n";
    query_stream << "   INNER JOIN Node ON Container.NodeID = Node.NodeID\n";

    if(condition_columns.size() != 0) {
        query_stream << BuildWhereLikeClause(condition_columns, condition_values) << " AND ";
    } else {
        query_stream << "WHERE ";
    }
    query_stream << "Node.ExperimentRunID = " << experiment_run_id << " AND ";

    query_stream << "WorkloadEvent.SampleTime >= '" << start_time << "'";

    if(end_time != AggServer::FormatTimestamp(0.0)) {
        query_stream << "AND WorkloadEvent.SampleTime <= '"
                     << /*connection_.quote(*/ end_time /*)*/ << "'";
    }
    query_stream << " ORDER BY WorkloadEvent.SampleTime";
    query_stream << std::endl;

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        pqxx::work transaction(connection_, "GetWorkloadEventsTransaction");
        const auto& pg_result = transaction.exec(query_stream.str());
        transaction.commit();

        return pg_result;
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while querying Workload info: " << e.what()
                  << std::endl;
        throw;
    }
}

const pqxx::result DatabaseClient::GetMarkerInfo(int experiment_run_id,
                                                 std::string start_time,
                                                 std::string end_time,
                                                 const std::vector<std::string>& condition_columns,
                                                 const std::vector<std::string>& condition_values)
{
    std::stringstream query_stream;

    query_stream << "SELECT WorkloadEvent.Type, " << StringToPSQLTimestamp("SampleTime")
                 << " AS SampleTime, WorkloadEvent.Function AS FunctionName, "
                    "WorkloadEvent.Arguments AS Label, WorkloadEvent.WorkloadID AS WorkloadID,\n";
    query_stream << "   WorkerInstance.Name AS WorkerInstanceName, WorkerInstance.Path AS "
                    "WorkerInstancePath, WorkerInstance.GraphmlID AS WorkerInstanceGraphmlID, \n";
    query_stream << "   Worker.Name AS WorkerName, Worker.GraphmlID AS WorkerGraphmlID, \n";
    query_stream << "   ComponentInstance.Name AS ComponentInstanceName, ComponentInstance.Path AS "
                    "ComponentInstancePath, ComponentInstance.GraphmlID AS "
                    "ComponentInstanceGraphmlID,\n";
    query_stream << "   Component.Name AS ComponentName, Component.GraphmlID AS "
                    "ComponentGraphmlID, Component.ExperimentRunID AS RunID,\n";
    query_stream << "   Container.Name AS ContainerName, Container.Type as ContainerType, "
                    "Container.GraphmlID AS ContainerGraphmlID,\n";
    query_stream << "   Node.Hostname AS NodeHostname, Node.IP AS NodeIP, Node.GraphmlID AS "
                    "NodeGraphmlID\n";
    query_stream << "FROM WorkloadEvent INNER JOIN WorkerInstance ON "
                    "WorkloadEvent.WorkerInstanceID = WorkerInstance.WorkerInstanceID\n";
    query_stream << "   INNER JOIN Worker ON WorkerInstance.WorkerID = Worker.WorkerID\n";
    query_stream << "   INNER JOIN ComponentInstance ON WorkerInstance.ComponentInstanceID = "
                    "ComponentInstance.ComponentInstanceID\n";
    query_stream << "   INNER JOIN Component ON ComponentInstance.ComponentID = "
                    "Component.ComponentID\n";
    query_stream << "   INNER JOIN Container ON ComponentInstance.ContainerID = "
                    "Container.ContainerID\n";
    query_stream << "   INNER JOIN Node ON Container.NodeID = Node.NodeID\n";
    query_stream << "WHERE WorkloadEvent.Type = 'MARKER' AND ";

    if(condition_columns.size() != 0) {
        query_stream << BuildWhereLikeClause(condition_columns, condition_values) << " AND ";
    }

    query_stream << "Node.ExperimentRunID = " << experiment_run_id;

    if(!start_time.empty()) {
        query_stream << " AND WorkloadEvent.SampleTime >= '" << start_time << "'";
    }

    if(!end_time.empty()) {
        query_stream << " AND WorkloadEvent.SampleTime <= '" << end_time << "'";
    }
    query_stream << " ORDER BY Label, WorkloadEvent.WorkloadID, WorkloadEvent.SampleTime";
    query_stream << std::endl;

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        pqxx::work transaction(connection_, "GetMarkerTransaction");
        const auto& pg_result = transaction.exec(query_stream.str());
        transaction.commit();

        return pg_result;
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while querying Marker info: " << e.what() << std::endl;
        throw;
    }
}

const pqxx::result DatabaseClient::GetCPUUtilInfo(int experiment_run_id,
                                                  std::string start_time,
                                                  std::string end_time,
                                                  const std::vector<std::string>& condition_columns,
                                                  const std::vector<std::string>& condition_values)
{
    std::stringstream query_stream;

    query_stream << "SELECT Hardware.SystemStatus.CPUUtilisation AS CPUUtilisation, "
                 << StringToPSQLTimestamp("SampleTime") << " AS SampleTime,\n";
    query_stream << "   Node.Hostname AS NodeHostname, Node.IP AS NodeIP, Node.GraphmlID AS "
                    "NodeGraphmlID, Node.ExperimentRunID AS RunID\n";
    query_stream << "FROM Hardware.SystemStatus INNER JOIN Hardware.System ON "
                    "Hardware.SystemStatus.SystemID = Hardware.System.SystemID\n";
    query_stream << "   INNER JOIN Node ON Hardware.System.NodeID = Node.NodeID\n";

    if(condition_columns.size() != 0) {
        query_stream << BuildWhereLikeClause(condition_columns, condition_values) << " AND ";
    } else {
        query_stream << "WHERE ";
    }
    query_stream << "Node.ExperimentRunID = " << experiment_run_id << " AND ";

    query_stream << "Hardware.SystemStatus.SampleTime >= '" << start_time << "'";

    if(end_time != AggServer::FormatTimestamp(0.0)) {
        query_stream << "AND Hardware.SystemStatus.SampleTime <= '"
                     << /*connection_.quote(*/ end_time /*)*/ << "'";
    }
    query_stream << " ORDER BY Node.HostName, Hardware.SystemStatus.SampleTime ASC";
    query_stream << std::endl;

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        pqxx::work transaction(connection_, "GetCPUUtilisationTransaction");
        const auto& pg_result = transaction.exec(query_stream.str());
        transaction.commit();

        return pg_result;
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while querying CPUUtilisation info: " << e.what()
                  << std::endl;
        throw;
    }
}

const pqxx::result DatabaseClient::GetMemUtilInfo(int experiment_run_id,
                                                  std::string start_time,
                                                  std::string end_time,
                                                  const std::vector<std::string>& condition_columns,
                                                  const std::vector<std::string>& condition_values)
{
    std::stringstream query_stream;

    query_stream << "SELECT Hardware.SystemStatus.PhysMemUtilisation AS PhysMemUtilisation, "
                 << StringToPSQLTimestamp("SampleTime") << " AS SampleTime,\n";
    query_stream << "   Node.Hostname AS NodeHostname, Node.IP AS NodeIP, Node.GraphmlID AS "
                    "NodeGraphmlID, Node.ExperimentRunID AS RunID\n";
    query_stream << "FROM Hardware.SystemStatus INNER JOIN Hardware.System ON "
                    "Hardware.SystemStatus.SystemID = Hardware.System.SystemID\n";
    query_stream << "   INNER JOIN Node ON Hardware.System.NodeID = Node.NodeID\n";

    if(condition_columns.size() != 0) {
        query_stream << BuildWhereLikeClause(condition_columns, condition_values) << " AND ";
    } else {
        query_stream << "WHERE ";
    }
    query_stream << "Node.ExperimentRunID = " << experiment_run_id << "   AND ";

    query_stream << "Hardware.SystemStatus.SampleTime >= '" << start_time << "'";

    if(end_time != AggServer::FormatTimestamp(0.0)) {
        query_stream << "AND Hardware.SystemStatus.SampleTime <= '"
                     << /*connection_.quote(*/ end_time /*)*/ << "'";
    }
    query_stream << " ORDER BY Node.HostName, Hardware.SystemStatus.SampleTime ASC";
    query_stream << std::endl;

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        pqxx::work transaction(connection_, "GetMemUtilisationTransaction");
        const auto& pg_result = transaction.exec(query_stream.str());
        transaction.commit();

        return pg_result;
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while querying MemUtilisation info: " << e.what()
                  << std::endl;
        throw;
    }
}

void DatabaseClient::UpdateShutdownTime(int experiment_run_id, const std::string& end_time)
{
    std::stringstream query_stream;

    query_stream << "UPDATE ExperimentRun SET EndTime = " << connection_.quote(end_time);
    query_stream << "WHERE ExperimentRunID = " << experiment_run_id;

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        pqxx::work transaction(connection_, "UpdateShutdownTransaction");
        const auto& pg_result = transaction.exec(query_stream.str());
        transaction.commit();
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while updating experiment run shutdown time: "
                  << e.what() << std::endl;
        throw;
    }
}

void DatabaseClient::UpdateLastSampleTime(int experiment_run_id, const std::string& sample_time)
{
    std::stringstream query_stream;

    std::string time_val = connection_.quote(sample_time);

    query_stream << "UPDATE ExperimentRun SET LastUpdated = " << time_val
                 << " WHERE (ExperimentRunID = " << experiment_run_id << " AND (LastUpdated < "
                 << time_val << "::timestamp OR LastUpdated IS NULL))";

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        pqxx::work transaction(connection_, "UpdateLastSampleTransaction");
        const auto& pg_result = transaction.exec(query_stream.str());
        transaction.commit();
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while updating experiment run's last sample time: "
                  << e.what() << std::endl;
        throw;
    }
}

std::string DatabaseClient::StringToPSQLTimestamp(const std::string& str)
{
    return "to_char((" + str + "::timestamp), 'YYYY-MM-DD\"T\"HH24:MI:SS.US\"Z\"')";
}

const std::string DatabaseClient::BuildWhereAllEqualClause(const std::vector<std::string>& cols,
                                                           const std::vector<std::string>& vals)
{
    std::stringstream where_stream;

    where_stream << "WHERE (";

    auto col = cols.begin();
    auto val = vals.begin();
    while(col != cols.end() || val != vals.end()) {
        where_stream << *col << " = '" << *val << "'";
        col++;
        val++;
        if(col != cols.end()) {
            where_stream << " AND ";
        }
    }

    where_stream << ")";

    return where_stream.str();
}

const std::string DatabaseClient::BuildWhereAnyEqualClause(const std::vector<std::string>& cols,
                                                           const std::vector<std::string>& vals)
{
    std::stringstream where_stream;

    where_stream << "WHERE (";

    auto col = cols.begin();
    auto val = vals.begin();
    while(col != cols.end() || val != vals.end()) {
        where_stream << *col << " = '" << *val << "'";
        col++;
        val++;
        if(col != cols.end()) {
            where_stream << " OR ";
        }
    }

    where_stream << ")";

    return where_stream.str();
}

const std::string DatabaseClient::BuildWhereLikeClause(const std::vector<std::string>& cols,
                                                       const std::vector<std::string>& vals)
{
    std::stringstream where_stream;

    where_stream << "WHERE (";

    auto col = cols.begin();
    auto val = vals.begin();
    while(col != cols.end() || val != vals.end()) {
        where_stream << *col << " LIKE '" << *val << "'";
        col++;
        val++;
        if(col != cols.end()) {
            where_stream << " OR ";
        }
    }

    where_stream << ")";

    return where_stream.str();
}

const std::string DatabaseClient::BuildColTuple(const std::vector<std::string>& cols)
{
    std::stringstream tuple_stream;

    tuple_stream << "(";

    auto col = cols.begin();
    if(col == cols.end()) {
        tuple_stream << ")";
        return tuple_stream.str();
    };

    tuple_stream << *col;
    col++;

    while(col != cols.end()) {
        tuple_stream << ", " << *col;
        col++;
    }

    tuple_stream << ")";

    return tuple_stream.str();
}

pqxx::work& DatabaseClient::AquireBatchedTransaction()
{
    std::lock_guard<std::mutex> trans_guard(batched_transaction_mutex_);
    static unsigned int total_batched_transactions_ = 0;

    if(batched_transaction_ == nullptr) {
        batched_transaction_ = std::unique_ptr<pqxx::work>(
            new pqxx::work(batched_connection_,
                           "BatchedTransaction" + std::to_string(total_batched_transactions_)));
        batched_write_count_ = 0;
        total_batched_transactions_++;
    }
    return *batched_transaction_;
}

void DatabaseClient::ReleaseBatchedTransaction()
{
    std::lock_guard<std::mutex> trans_guard(batched_transaction_mutex_);

    batched_write_count_++;
    if(batched_write_count_ > 1000) {
        FlushBatchedTransaction();
    }
}

void DatabaseClient::FlushBatchedTransaction()
{
    if(batched_transaction_ == nullptr) {
        return;
    }
    try {
        batched_transaction_->commit();
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while attempting to commit a batched transaction"
                  << std::endl;
        batched_transaction_.reset();
        throw;
    }
    batched_transaction_.reset();
}

void DatabaseClient::AbortBatchedTransaction()
{
    if(batched_transaction_ == nullptr) {
        return;
    }

    batched_transaction_->abort();
    batched_transaction_.reset();
}
