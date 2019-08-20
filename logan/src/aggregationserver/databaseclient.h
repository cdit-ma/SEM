#ifndef LOGAN_DATABASECLIENT_H
#define LOGAN_DATABASECLIENT_H

#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <pqxx/pqxx>

#include <iostream>

// Hack for backwards compatibility with pqxx 4.0 and earlier where row was named tuple
#ifndef PQXX_H_ROW
namespace pqxx {
using row = tuple;
}
#endif

class DatabaseClient {
public:
    DatabaseClient(const std::string& connection_details);
    ~DatabaseClient();
    void Connect(const std::string& connection_string){};

    void InitSchemaFrom(const std::string& sql_string);

    void CreateTable(const std::string& table_name,
                     const std::vector<std::pair<std::string, std::string>>& columns);

    int InsertValues(const std::string& table_name,
                     const std::vector<std::string>& columns,
                     const std::vector<std::string>& values);

    int InsertValuesUnique(const std::string& table_name,
                           const std::vector<std::string>& columns,
                           const std::vector<std::string>& values,
                           const std::vector<std::string>& unique_col);

    const pqxx::result GetValues(const std::string table_name,
                                 const std::vector<std::string>& columns,
                                 const std::string& query = "");

    const pqxx::result GetValuesLike(const std::string table_name,
                                     const std::vector<std::string>& columns,
                                     const std::string& query = "");

    int GetID(const std::string& table_name, const std::string& query);

    std::optional<int> GetMaxValue(const std::string& table_name,
                                   const std::string& column,
                                   const std::string& where_query);

    std::string EscapeString(const std::string& str);

    const pqxx::result GetPortLifecycleEventInfo(int experiment_run_id,
                                                 std::string start_time,
                                                 std::string end_time,
                                                 const std::vector<std::string>& condition_columns,
                                                 const std::vector<std::string>& condition_values);

    const pqxx::result GetWorkloadEventInfo(int experiment_run_id,
                                            std::string start_time,
                                            std::string end_time,
                                            const std::vector<std::string>& condition_columns,
                                            const std::vector<std::string>& condition_values);

    const pqxx::result GetMarkerInfo(int experiment_run_id,
                                     std::string start_time = "",
                                     std::string end_time = "",
                                     const std::vector<std::string>& condition_columns = {},
                                     const std::vector<std::string>& condition_values = {});

    const pqxx::result GetCPUUtilInfo(int experiment_run_id,
                                      std::string start_time,
                                      std::string end_time,
                                      const std::vector<std::string>& condition_columns,
                                      const std::vector<std::string>& condition_values);

    const pqxx::result GetMemUtilInfo(int experiment_run_id,
                                      std::string start_time,
                                      std::string end_time,
                                      const std::vector<std::string>& condition_columns,
                                      const std::vector<std::string>& condition_values);

    void UpdateShutdownTime(int experiment_run_id, const std::string& end_time);

    void UpdateLastSampleTime(int experiment_run_id, const std::string& sample_time);

    static std::string StringToPSQLTimestamp(const std::string& str);

private:
    const std::string BuildWhereAllEqualClause(const std::vector<std::string>& cols,
                                               const std::vector<std::string>& vals);
    const std::string BuildWhereAnyEqualClause(const std::vector<std::string>& cols,
                                               const std::vector<std::string>& vals);
    const std::string BuildWhereLikeClause(const std::vector<std::string>& cols,
                                           const std::vector<std::string>& vals);
    const std::string BuildColTuple(const std::vector<std::string>& cols);

    pqxx::work& AquireBatchedTransaction();
    void ReleaseBatchedTransaction();
    // Aquire batch transaction lock before flushing
    void FlushBatchedTransaction();
    void AbortBatchedTransaction();

    pqxx::connection connection_;
    pqxx::connection batched_connection_;

    std::unique_ptr<pqxx::work> batched_transaction_;
    unsigned int batched_write_count_;

    std::mutex conn_mutex_;
    std::mutex batched_transaction_mutex_;
};

#endif // LOGAN_DATABASECLIENT_H
