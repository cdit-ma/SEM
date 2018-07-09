#ifndef ENVIRONMENT_MANAGER_LOGGER_H
#define ENVIRONMENT_MANAGER_LOGGER_H

#include <mutex>
#include <unordered_map>
#include "uniquequeue.hpp"
#include <memory>
#include <re_common/proto/controlmessage/controlmessage.pb.h>

namespace EnvironmentManager{
class Environment;
class Experiment;
class Node;
class Logger{
    public:
        enum class Type{
            None = 0,
            Model = 1,
            Client = 2,
            Server = 3,
        };
        enum class Mode{
            Off = 0,
            Live = 1,
            Cached = 2,
        };
        Logger(Environment& environment, Node& parent, const NodeManager::Logger& logger);
        Logger(Environment& environment, Node& parent, Type type, Mode mode);
        ~Logger();


        std::string GetId() const;
        Logger::Type GetType() const;


        Node& GetNode() const;
        Experiment& GetExperiment() const;

        void SetPublisherPort(const std::string& publisher_port);
        std::string GetPublisherPort() const;

        std::string GetPublisherEndpoint() const;

        void SetFrequency(const double frequency);
        double GetFrequency() const;

        void SetMode(const Mode mode);
        Mode GetMode() const;

        void AddProcess(const std::string& process);
        const std::set<std::string>& GetProcesses();

        void AddConnectedClientId(const std::string& client_id);
        const std::set<std::string>&  GetConnectedClientIds();

        void SetDbFileName(const std::string& file_name);
        std::string GetDbFileName() const;


        void SetDirty();
        bool IsDirty();

        NodeManager::Logger* GetUpdate();
        NodeManager::Logger* GetProto();

    private:
        Node& node_;
        Environment& environment_;
        std::string id_;
        Type type_;

        double frequency_;
        Mode mode_;
        std::string publisher_port_;
        std::set<std::string> processes_;

        std::set<std::string> connected_client_ids_;
        std::string db_file_name_;


        bool dirty_ = false;

        static Type TranslateProtoType(const NodeManager::Logger::Type type);
        static NodeManager::Logger::Type TranslateInternalType(const Type type);

        static Mode TranslateProtoMode(const NodeManager::Logger::Mode mode);
        static NodeManager::Logger::Mode TranslateInternalMode(const Mode mode);
};
};

#endif //ENVIRONMENT_MANAGER_COMPONENT_H