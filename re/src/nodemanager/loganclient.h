#ifndef CORE_NODEMANAGER_LOGANCLIENT_H
#define CORE_NODEMANAGER_LOGANCLIENT_H

#include <memory>
#include "activatable.h"

class LogController;

class LoganClient : public Activatable{
    public:
        // REVIEW (Mitch): The underlying Activatable is never instantiated.
        LoganClient(const std::string& id);
        ~LoganClient();

        void SetEndpoint(const std::string& address, const std::string& port);
        void SetFrequency(double frequency);
        void SetProcesses(const std::vector<std::string>& processes);
        void AddProcess(const std::string& process);
        void SetLiveMode(bool live_mode);
    protected:
        void HandleActivate();
        void HandleTerminate();
    private:
        std::unique_ptr<LogController> logan_client_;
        
        bool live_mode_ = true;
        double frequency_ = 1;
        // REVIEW (Mitch): Replace with endpoint class.
        std::string endpoint_;
        // REVIEW (Mitch): Rename to "process_names_"
        std::vector<std::string> processes_;
};
#endif //CORE_NODEMANAGER_LOGANCLIENT_H