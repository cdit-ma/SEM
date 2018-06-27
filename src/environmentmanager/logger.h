#ifndef ENVIRONMENT_MANAGER_LOGGER_H
#define ENVIRONMENT_MANAGER_LOGGER_H

#include <mutex>
#include <unordered_map>
#include "uniquequeue.hpp"
#include <memory>
#include <proto/controlmessage/controlmessage.pb.h>

namespace EnvironmentManager{
class Environment;
class Experiment;
class Node;
class Logger{
    public:
        enum class Type{
            None = 0,
            Client = 1,
            Server = 2,
        };
        enum class Mode{
            Off = 0,
            Live = 1,
            Cached = 2,
        };
        Logger(Environment& environment, Node& parent, const NodeManager::Logger& logger);
        ~Logger();

        void ConfigureConnections();

        std::string GetId() const;
        Logger::Type GetType() const;


        Node& GetNode() const;
        Experiment& GetExperiment() const;

        void SetPublisherPort(const std::string& publisher_port);
        std::string GetPublisherPort() const;

        void SetFrequency(const double frequency);
        double GetFrequency() const;

        void SetMode(const Mode mode);
        Mode GetMode() const;

        void AddProcess(const std::string& process);
        void RemoveProcess(const std::string& process);
        std::vector<std::string> GetProcesses() const;


        void AddClientId(const std::string& client_id);
        std::vector<std::string> GetClientIds() const;

        void AddClientAddress(const std::string& client_address);
        std::vector<std::string> GetClientAddresses() const;

        void SetDbFileName(const std::string& file_name);
        std::string GetDbFileName() const;


        void SetDirty();
        bool IsDirty();

        NodeManager::Logger* GetUpdate();

        NodeManager::Logger* GetProto();


        NodeManager::Logger* GetDeploymentMessage() const;

    private:
        Node& node_;
        Environment& environment_;
        std::string id_;
        Type type_;

        double frequency_;
        Mode mode_;
        std::string publisher_port_;
        std::set<std::string> processes_;

        std::set<std::string> client_ids_;
        std::set<std::string> client_addresses_;
        std::string db_file_name_;


        bool dirty_;

};
};

#endif //ENVIRONMENT_MANAGER_COMPONENT_H