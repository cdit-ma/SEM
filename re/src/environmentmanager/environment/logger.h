#ifndef ENVIRONMENT_MANAGER_LOGGER_H
#define ENVIRONMENT_MANAGER_LOGGER_H

#include "uniquequeue.hpp"
#include <memory>
#include <mutex>
#include "controlmessage.pb.h"
#include "socketaddress.hpp"
#include <unordered_map>

namespace re::EnvironmentManager{
class Environment;
class Experiment;
class Node;
class Container;
class Logger{
    public:
        enum class Type{
            None = 0,
            Model = 1,
            Client = 2,
            Server = 3,
            Experiment = 4,
        };
        enum class Mode{
            Off = 0,
            Live = 1,
            Cached = 2,
        };
        Logger(Environment& environment, Container& parent, const NodeManager::Logger& logger);
        Logger(Environment& environment, Container& parent, Type type, Mode mode);
        ~Logger();


        [[nodiscard]] std::string GetId() const;
        [[nodiscard]] Logger::Type GetType() const;


        [[nodiscard]] Container& GetContainer() const;
        [[nodiscard]] Node& GetNode() const;

        [[nodiscard]] auto GetPublisherEndpoint() const -> types::SocketAddress;

        void SetFrequency(double frequency);
        [[nodiscard]] double GetFrequency() const;

        void SetMode(Mode mode);
        [[nodiscard]] Mode GetMode() const;

        void AddProcess(const std::string& process);
        const std::set<std::string>& GetProcesses();

        void AddConnectedClientId(const std::string& client_id);
        const std::set<std::string>&  GetConnectedClientIds();

        void SetDbFileName(const std::string& file_name);
        [[nodiscard]] std::string GetDbFileName() const;


        void SetDirty();
        bool IsDirty();

        std::unique_ptr<NodeManager::Logger> GetProto(bool full_update);

        static const std::string MODEL_LOGGER_ID;
        static const std::string EXPERIMENT_LOGGER_ID;
    private:
        Container& parent_;
        Environment& environment_;
        std::string id_;
        Type type_;

        double frequency_;
        Mode mode_;
        types::SocketAddress publisher_endpoint_;
        std::set<std::string> processes_;

        std::set<std::string> connected_client_ids_;
        std::string db_file_name_;


        bool dirty_ = false;

        static Type TranslateProtoType(NodeManager::Logger::Type type);
        static NodeManager::Logger::Type TranslateInternalType(Type type);

        static Mode TranslateProtoMode(NodeManager::Logger::Mode mode);
        static NodeManager::Logger::Mode TranslateInternalMode(Mode mode);
};
};

#endif //ENVIRONMENT_MANAGER_COMPONENT_H