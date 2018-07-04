#ifndef CORE_NODEMANAGER_LOGANCLIENT_H
#define CORE_NODEMANAGER_LOGANCLIENT_H

#include <memory>
#include <core/activatable.h>

class LogController;

class LoganClient : public Activatable{
    public:
        LoganClient(const std::string& id);
        ~LoganClient();

        void SetEndpoint(const std::string& address, const std::string& port);
        void SetFrequency(double frequency);
        void SetProcesses(const std::vector<std::string>& processes);
        void AddProcess(const std::string& process);
        void SetLiveMode(bool live_mode);
    protected:
        bool HandleActivate();
        bool HandleConfigure();
        bool HandlePassivate();
        bool HandleTerminate();
    private:
        std::unique_ptr<LogController> logan_client_;
        
        bool live_mode_ = true;
        double frequency_ = 1;
        std::string endpoint_;
        std::vector<std::string> processes_;
};
#endif //CORE_NODEMANAGER_LOGANCLIENT_H