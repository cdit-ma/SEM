#include "databaseclient.h"

#include <iostream>
#include <sstream>

#include <algorithm>

#include <chrono>

#include "prototimerange.hpp"
#include "utils.h"

using namespace re::types;

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

// REVIEW(Jackson): Name this ExecuteArbitrarySQL, or actually figure out the purpose of the
//  interface and engineer appropriately
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

    query_stream << "INSERT INTO " << table_name;
    if(columns.empty()) {
        throw std::invalid_argument("Attempting to insert values into table " + table_name
                                    + " with 0 columns provided");
    } else {
        query_stream << " (";
        for(size_t i = 0; i < columns.size() - 1; i++) {
            query_stream << columns.at(i) << ',';
        }
        query_stream << columns.at(columns.size() - 1) << ')';
    }

    query_stream << std::endl << " VALUES (";
    for(size_t i = 0; i < values.size() - 1; i++) {
        query_stream << values.at(i) << ',';
    }
    query_stream << values.at(values.size() - 1) << ")" << std::endl;
    query_stream << "RETURNING " << strip_schema(table_name) << "." << id_column << ";"
                 << std::endl;

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        auto start = std::chrono::steady_clock::now();
        auto& transaction = AcquireBatchedTransaction();

        auto made_transaction = std::chrono::steady_clock::now();

        try {
            auto&& result = transaction.exec(query_stream.str());

            ReleaseBatchedTransaction();
            FlushBatchedTransaction(); // TODO: remove this once we can actually figure out when
                                       //  things are going down

            std::string lower_id_column(id_column);
            std::transform(lower_id_column.begin(), lower_id_column.end(), lower_id_column.begin(),
                           ::tolower);
            unsigned int id_colnum = result.column_number(lower_id_column);

            for(const auto& row : result) {
                for(unsigned int colnum = 0; colnum < row.size(); colnum++) {
                    if(colnum == id_colnum) {
                        int id_value = row[colnum].as<int>();
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
        auto& transaction = AcquireBatchedTransaction();

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
        auto& transaction = AcquireBatchedTransaction();

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
    /*
    Reference SQL for inserting uniquely and returning value if it already exists

    Broad notes: the two separate parts involve first attempting to insert the values if they are
        new, then afterwards using a select to return values to the client.

    For more detailed explanation see UPSERT here:
    https://www.postgresql.org/docs/9.5/sql-insert.html

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

    std::string id_column = strip_schema(table_name) + "ID";
    int id_value = -1;
    std::vector<std::string> unique_vals = std::vector<std::string>(unique_cols.size());

    std::stringstream query_stream;

    query_stream << "WITH i AS (" << std::endl;
    query_stream << "INSERT INTO " << connection_.esc(table_name);
    if(columns.empty()) {
        throw std::invalid_argument("Attempting to insert values uniquely into table " + table_name
                                    + " with 0 columns provided");
    } else {
        // Iterate through column names
        query_stream << " (";
        for(size_t i = 0; i < columns.size() - 1; i++) {
            query_stream << connection_.esc(columns.at(i)) << ',';
            // Pull out the values stored in a unique column to be associated with the unique
            // columns
            for(size_t j = 0; j < unique_cols.size(); j++) {
                if(unique_cols.at(j) == columns.at(i)) {
                    unique_vals.at(j) = values.at(i);
                }
            }
        }
        size_t last_col = columns.size() - 1;
        query_stream << columns.at(last_col) << ')';
        for(size_t j = 0; j < unique_cols.size(); j++) {
            if(unique_cols.at(j) == columns.at(last_col)) {
                unique_vals.at(j) = values.at(last_col);
            }
        }
    }

    query_stream << std::endl << " VALUES (";
    for(size_t i = 0; i < values.size() - 1; i++) {
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
    query_stream << "LIMIT 1" << std::endl;

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

    return id_value;
}

pqxx::result DatabaseClient::GetValues(const std::string& table_name,
                                       const std::vector<std::string>& columns,
                                       std::optional<std::string_view> query)
{
    std::stringstream query_stream;

    query_stream << "SELECT ";
    if(columns.empty()) {
        throw std::invalid_argument("Attempting to get values from table " + table_name
                                    + " with 0 columns provided");
    } else {
        for(size_t i = 0; i < columns.size() - 1; i++) {
            query_stream << columns.at(i) << ", ";
        }
        query_stream << columns.at(columns.size() - 1);
    }
    query_stream << std::endl;
    query_stream << " FROM " << table_name << std::endl;
    if(query.has_value()) {
        query_stream << " WHERE (" << *query << ")";
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

    // REVIEW(Jackson): Looks like unreachable code
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

pqxx::result
DatabaseClient::GetPortLifecycleEventInfo(int experiment_run_id,
                                          const std::string& start_time,
                                          const std::string& end_time,
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

    if(!condition_columns.empty()) {
        query_stream << BuildWhereLikeClause(condition_columns, condition_values) << " AND ";
    } else {
        query_stream << "WHERE ";
    }
    query_stream << "Node.ExperimentRunID = " << experiment_run_id << " AND ";

    query_stream << "PortLifecycleEvent.SampleTime >= '" << start_time << "'";

    // REVIEW(Jackson): This should be optional rather than implicit 0 sentinel value
    if(end_time != AggServer::FormatTimestamp(0.0)) {
        query_stream << "AND PortLifecycleEvent.SampleTime <= '" << end_time << "'";
    }
    query_stream << " ORDER BY PortLifecycleEvent.SampleTime" << std::endl;

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

pqxx::result DatabaseClient::GetPortEventInfo(int experiment_run_id,
                                              const std::string& start_time,
                                              const std::string& end_time,
                                              const std::vector<std::string>& condition_columns,
                                              const std::vector<std::string>& condition_values)
{
    std::stringstream query_stream;

    query_stream << "SELECT PortEvent.Type, " << StringToPSQLTimestamp("SampleTime")
                 << " AS SampleTime, porteventsequencenum AS SequenceNum, PortEvent.Message AS "
                    "Message, \n";
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
    query_stream << "FROM PortEvent INNER JOIN Port ON PortEvent.PortID = Port.PortID\n";
    query_stream << "   INNER JOIN ComponentInstance ON Port.ComponentInstanceID = "
                    "ComponentInstance.ComponentInstanceID\n";
    query_stream << "   INNER JOIN Component ON ComponentInstance.ComponentID = "
                    "Component.ComponentID\n";
    query_stream << "   INNER JOIN Container ON ComponentInstance.ContainerID = "
                    "Container.ContainerID\n";
    query_stream << "   INNER JOIN Node ON Container.NodeID = Node.NodeID\n";

    if(condition_columns.empty()) {
        query_stream << "WHERE ";
    } else {
        query_stream << BuildWhereLikeClause(condition_columns, condition_values) << " AND ";
    }
    query_stream << "Node.ExperimentRunID = " << experiment_run_id << " AND ";

    query_stream << "PortEvent.SampleTime >= '" << start_time << "'";

    if(end_time != AggServer::FormatTimestamp(0.0)) {
        query_stream << "AND PortEvent.SampleTime <= '" << end_time << "'";
    }
    query_stream << " ORDER BY PortEvent.SampleTime" << std::endl;

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        pqxx::work transaction(connection_, "GetPortEventTransaction");
        const auto& pg_result = transaction.exec(query_stream.str());
        transaction.commit();

        return pg_result;
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while querying PortEvents: " << e.what() << std::endl;
        throw;
    }
}

pqxx::result DatabaseClient::GetWorkloadEventInfo(int experiment_run_id,
                                                  const std::string& start_time,
                                                  const std::string& end_time,
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

    if(!condition_columns.empty()) {
        query_stream << BuildWhereLikeClause(condition_columns, condition_values) << " AND ";
    } else {
        query_stream << "WHERE ";
    }
    query_stream << "Node.ExperimentRunID = " << experiment_run_id << " AND ";

    query_stream << "WorkloadEvent.SampleTime >= '" << start_time << "'";

    if(end_time != AggServer::FormatTimestamp(0.0)) {
        query_stream << "AND WorkloadEvent.SampleTime <= '" << end_time << "'";
    }
    query_stream << " ORDER BY WorkloadEvent.SampleTime" << std::endl;

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

pqxx::result DatabaseClient::GetMarkerInfo(int experiment_run_id,
                                           const std::string& start_time,
                                           const std::string& end_time,
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

    if(!condition_columns.empty()) {
        query_stream << BuildWhereLikeClause(condition_columns, condition_values) << " AND ";
    }

    query_stream << "Node.ExperimentRunID = " << experiment_run_id;

    if(!start_time.empty()) {
        query_stream << " AND WorkloadEvent.SampleTime >= '" << start_time << "'";
    }

    if(!end_time.empty()) {
        query_stream << " AND WorkloadEvent.SampleTime <= '" << end_time << "'";
    }
    query_stream << " ORDER BY Label, WorkloadEvent.WorkloadID, WorkloadEvent.SampleTime"
                 << std::endl;

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

pqxx::result DatabaseClient::GetCPUUtilInfo(int experiment_run_id,
                                            const std::string& start_time,
                                            const std::string& end_time,
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

    if(!condition_columns.empty()) {
        query_stream << BuildWhereLikeClause(condition_columns, condition_values) << " AND ";
    } else {
        query_stream << "WHERE ";
    }
    query_stream << "Node.ExperimentRunID = " << experiment_run_id << " AND ";

    query_stream << "Hardware.SystemStatus.SampleTime >= '" << start_time << "'";

    if(end_time != AggServer::FormatTimestamp(0.0)) {
        query_stream << "AND Hardware.SystemStatus.SampleTime <= '" << end_time << "'";
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

pqxx::result DatabaseClient::GetMemUtilInfo(int experiment_run_id,
                                            const std::string& start_time,
                                            const std::string& end_time,
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

    if(!condition_columns.empty()) {
        query_stream << BuildWhereLikeClause(condition_columns, condition_values) << " AND ";
    } else {
        query_stream << "WHERE ";
    }
    query_stream << "Node.ExperimentRunID = " << experiment_run_id << "   AND ";

    query_stream << "Hardware.SystemStatus.SampleTime >= '" << start_time << "'";

    if(end_time != AggServer::FormatTimestamp(0.0)) {
        query_stream << "AND Hardware.SystemStatus.SampleTime <= '" << end_time << "'";
    }
    query_stream << " ORDER BY Node.HostName, Hardware.SystemStatus.SampleTime ASC" << std::endl;

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

pqxx::result
DatabaseClient::GetNetworkUtilInfo(int experiment_run_id,
                                   const re::types::UnboundedTimeRange<time_point>& time_range,
                                   const std::vector<std::string>& condition_columns,
                                   const std::vector<std::string>& condition_values)
{
    std::stringstream query_stream;

    query_stream << "SELECT Hardware.InterfaceStatus.BytesReceived AS BytesReceived, "
                 << "Hardware.InterfaceStatus.BytesTransmitted AS BytesSent, "
                 << "Hardware.InterfaceStatus.PacketsReceived AS BytesReceived, "
                 << "Hardware.InterfaceStatus.PacketsTransmitted AS PacketsSent, "
                 << StringToPSQLTimestamp("SampleTime") << " AS SampleTime,\n";
    query_stream << "   Interface.Name AS InterfaceName, Interface.MAC AS MAC, "
                 << "Interface.ipv4 AS IPv4,\n";
    query_stream << "   Node.Hostname AS NodeHostname, Node.IP AS NodeIP, Node.GraphmlID AS "
                    "NodeGraphmlID, Node.ExperimentRunID AS RunID\n";
    query_stream << "FROM Hardware.InterfaceStatus INNER JOIN Hardware.Interface ON "
                    "Hardware.InterfaceStatus.InterfaceID = Hardware.Interface.InterfaceID\n";
    query_stream << "   INNER JOIN Node ON Hardware.Interface.NodeID = Node.NodeID\n";

    if(!condition_columns.empty()) {
        query_stream << BuildWhereLikeClause(condition_columns, condition_values) << " AND ";
    } else {
        query_stream << " WHERE ";
    }
    query_stream << "Node.ExperimentRunID = " << experiment_run_id;

    if(time_range.Start().has_value()) {
        query_stream << " AND Hardware.InterfaceStatus.SampleTime >= '"
                     << re::types::StringFromTimepoint(*time_range.Start()) << "'";
    }

    if(time_range.End().has_value()) {
        query_stream << " AND Hardware.InterfaceStatus.SampleTime <= '"
                     << re::types::StringFromTimepoint(*time_range.End()) << "'";
    }

    query_stream << " ORDER BY Node.HostName, Hardware.InterfaceStatus.SampleTime ASC" << std::endl;

    std::lock_guard<std::mutex> conn_guard(conn_mutex_);

    try {
        pqxx::work transaction(connection_, "GetNetworkUtilisationTransaction");
        const auto& pg_result = transaction.exec(query_stream.str());
        transaction.commit();

        return pg_result;
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while querying NetworkUtilisation info: " << e.what()
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

/**
 * Updates the last sample time to be at least the value of the newly provided sample time
 * @param experiment_run_id
 * @param sample_time See https://www.ietf.org/rfc/rfc3339.txt for format
 */
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

// REVIEW(Jackson): This function does the opposite of what it appears to say it does
std::string DatabaseClient::StringToPSQLTimestamp(const std::string& str)
{
    // to_char((<COLUMN_NAME>::timestamp), 'YYYY-MM-DD"T"HH24:MI:SS.US"Z"')
    return "to_char((" + str + R"_(::timestamp), 'YYYY-MM-DD"T"HH24:MI:SS.US"Z"'))_";
}

std::string DatabaseClient::BuildWhereAllEqualClause(const std::vector<std::string>& cols,
                                                     const std::vector<std::string>& vals)
{
    // "WHERE ( col1 = 'val1' AND col2 = 'val2')"
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

std::string DatabaseClient::BuildWhereAnyEqualClause(const std::vector<std::string>& cols,
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

std::string DatabaseClient::BuildWhereLikeClause(const std::vector<std::string>& cols,
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

// REVIEW(Mitchell): Implementation???
std::string DatabaseClient::BuildColTuple(const std::vector<std::string>& cols)
{
    std::stringstream tuple_stream;

    tuple_stream << "(";

    auto col = cols.begin();
    if(col == cols.end()) {
        tuple_stream << ")";
        return tuple_stream.str();
    }

    tuple_stream << *col;
    col++;

    while(col != cols.end()) {
        tuple_stream << ", " << *col;
        col++;
    }

    tuple_stream << ")";

    return tuple_stream.str();
}

pqxx::work& DatabaseClient::AcquireBatchedTransaction()
{
    std::lock_guard<std::mutex> trans_guard(batched_transaction_mutex_);
    static unsigned int total_batched_transactions_ = 0;

    if(batched_transaction_ == nullptr) {
        batched_transaction_ = std::make_unique<pqxx::work>(
            batched_connection_,
            "BatchedTransaction" + std::to_string(total_batched_transactions_));
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

std::vector<DatabaseClient::port_graphml_pair>
DatabaseClient::GetPubSubConnectionIDs(int experiment_run_id)
{
    std::vector<port_graphml_pair> id_pairs;

    std::stringstream query_stream;
    query_stream <<
        R"psql(SELECT from_port.graphmlid AS from_port_graphmlid, to_port.graphmlid as to_port_graphmlid
            FROM PubSubConnection
                JOIN Port as from_port ON from_port.portid = pubportid
                JOIN Port as to_port ON to_port.portid = subportid
                JOIN componentinstance ON from_port.componentinstanceid = componentinstance.componentinstanceid
                JOIN component using (componentid)
                WHERE experimentrunid = )psql"
                 << experiment_run_id;

    try {
        pqxx::work transaction(connection_, "GetPubSubConnectionGraphmlIDs");
        const auto& pg_result = transaction.exec(query_stream.str());
        transaction.commit();
        for(const auto& row : pg_result) {
            id_pairs.emplace_back(row["from_port_graphmlid"].as<std::string>(),
                                  row["to_port_graphmlid"].as<std::string>());
        }

        return id_pairs;
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while querying PubSubConnection info: " << e.what()
                  << std::endl;
        throw;
    }
}

std::vector<DatabaseClient::port_graphml_pair>
DatabaseClient::GetReqRepConnectionIDs(int experiment_run_id)
{
    std::vector<port_graphml_pair> id_pairs;

    std::stringstream query_stream;
    query_stream <<
        R"psql(SELECT from_port.graphmlid AS from_port_graphmlid, to_port.graphmlid as to_port_graphmlid
            FROM ReqRepConnection
                JOIN Port AS from_port ON from_port.portid = reqportid
                JOIN Port AS to_port ON to_port.portid = repportid
                JOIN componentinstance ON from_port.componentinstanceid = componentinstance.componentinstanceid
                JOIN component using (componentid)
                WHERE experimentrunid = )psql"
                 << experiment_run_id;

    try {
        pqxx::work transaction(connection_, "GetReqRepConnectionGraphmlIDs");
        const auto& pg_result = transaction.exec(query_stream.str());
        transaction.commit();
        for(const auto& row : pg_result) {
            id_pairs.emplace_back(row["from_port_graphmlid"].as<std::string>(),
                                  row["to_port_graphmlid"].as<std::string>());
        }

        return id_pairs;
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while querying ReqRepConnection info: " << e.what()
                  << std::endl;
        throw;
    }
}
