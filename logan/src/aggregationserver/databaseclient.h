#ifndef LOGAN_DATABASECLIENT_H
#define LOGAN_DATABASECLIENT_H

#include <mutex>
#include <optional>
#include <string_view>
#include <vector>

#include <pqxx/pqxx>

#include <iostream>

#include "unboundedtimerange.hpp"

// Hack for backwards compatibility with pqxx 4.0 and earlier where row was named tuple
#ifndef PQXX_H_ROW
namespace pqxx {
using row = tuple;
}
#endif

class DatabaseClient {
public:
    using port_graphml_pair = std::pair<std::string, std::string>;

    // REVIEW(Mitch): consider a ConnectionConfig struct, also string_view
    explicit DatabaseClient(const std::string& connection_details);
    ~DatabaseClient();

    // Remove at earliest convenience
    void Test();

    // REVIEW(Jackson): Should this be virtual?  NO
    virtual void InitSchemaFrom(const std::string& sql_string);

    void CreateTable(const std::string& table_name,
                     const std::vector<std::pair<std::string, std::string>>& columns);

    int InsertValues(const std::string& table_name,
                     const std::vector<std::string>& columns,
                     const std::vector<std::string>& values);

    int InsertValuesUnique(const std::string& table_name,
                           const std::vector<std::string>& columns,
                           const std::vector<std::string>& values,
                           const std::vector<std::string>& unique_col);

    void
    InsertPubSubValues(int from_port_id, const std::string& to_port_graphml, int experiment_run_id);

    void
    InsertReqRepValues(int from_port_id, const std::string& to_port_graphml, int experiment_run_id);

    pqxx::result GetValues(const std::string& table_name,
                           const std::vector<std::string>& columns,
                                 std::optional<std::string_view> query = std::nullopt);

    int GetID(const std::string& table_name, const std::string& query);

    std::optional<int> GetMaxValue(const std::string& table_name,
                                   const std::string& column,
                                   const std::string& where_query);

    pqxx::result GetPortLifecycleEventInfo(int experiment_run_id,
                                           const std::string& start_time,
                                                 const std::string& end_time,
                                                 const std::vector<std::string>& condition_columns,
                                                 const std::vector<std::string>& condition_values);

    pqxx::result GetPortEventInfo(int experiment_run_id,
                                  const std::string& start_time,
                                  const std::string& end_time,
                                  const std::vector<std::string>& condition_columns,
                                        const std::vector<std::string>& condition_values);

    pqxx::result GetWorkloadEventInfo(int experiment_run_id,
                                      const std::string& start_time,
                                            const std::string& end_time,
                                            const std::vector<std::string>& condition_columns,
                                            const std::vector<std::string>& condition_values);

    pqxx::result GetMarkerInfo(int experiment_run_id,
                               const std::string& start_time = "",
                                     const std::string& end_time = "",
                                     const std::vector<std::string>& condition_columns = {},
                                     const std::vector<std::string>& condition_values = {});

    pqxx::result GetCPUUtilInfo(int experiment_run_id,
                                const std::string& start_time,
                                      const std::string& end_time,
                                      const std::vector<std::string>& condition_columns,
                                      const std::vector<std::string>& condition_values);

    pqxx::result GetMemUtilInfo(int experiment_run_id,
                                const std::string& start_time,
                                      const std::string& end_time,
                                      const std::vector<std::string>& condition_columns,
                                      const std::vector<std::string>& condition_values);

    pqxx::result GetNetworkUtilInfo(int experiment_run_id,
                                    const re::types::UnboundedTimeRange<re::types::time_point>& time_range,
                                    const std::vector<std::string>& condition_columns,
                                    const std::vector<std::string>& condition_values);

    std::vector<port_graphml_pair> GetPubSubConnectionIDs(int experiment_run_id);
    std::vector<port_graphml_pair> GetReqRepConnectionIDs(int experiment_run_id);

    void UpdateShutdownTime(int experiment_run_id, const std::string& end_time);

    void UpdateLastSampleTime(int experiment_run_id, const std::string& sample_time);

    template<typename T> std::string quote(const T& val) { return connection_.quote(val); }

    static std::string StringToPSQLTimestamp(const std::string& str);

private:
    static std::string BuildWhereAllEqualClause(const std::vector<std::string>& cols,
                                                const std::vector<std::string>& vals);
    static std::string BuildWhereAnyEqualClause(const std::vector<std::string>& cols,
                                                const std::vector<std::string>& vals);
    static std::string BuildWhereLikeClause(const std::vector<std::string>& cols,
                                            const std::vector<std::string>& vals);
    static std::string BuildColTuple(const std::vector<std::string>& cols);

    pqxx::work& AcquireBatchedTransaction();
    void ReleaseBatchedTransaction();
    // Acquire batch transaction lock before flushing
    void FlushBatchedTransaction();
    void AbortBatchedTransaction();

    pqxx::connection connection_;
    pqxx::connection batched_connection_;

    std::unique_ptr<pqxx::work> batched_transaction_;
    unsigned int batched_write_count_ = 0;

    std::mutex conn_mutex_;
    std::mutex batched_transaction_mutex_;
};

#endif // LOGAN_DATABASECLIENT_H
