#ifndef CDITMA_LOG_H
#define CDITMA_LOG_H

#include <iostream>

namespace cdit {

    enum class Verbosity{DEBUG, INFO, WARNING, ERROR, FATAL};
    
    class Logger {
        public:
            static Logger& logger();
            std::ostream& GetStream(Verbosity verbosity);
        protected:
            Logger();
            ~Logger();
        private:
            static Logger log_singleton;
            
            int VerbosityLevel(const Verbosity verbosity);
            std::string VerbosityString(const Verbosity verbosity);
            
            bool first_log = true;
            std::ostream* actual_output = 0;
            std::ostream* null_stream = 0;
            
            cdit::Verbosity verbosity_level = Verbosity::DEBUG;
    };

    static std::ostream& Log(const Verbosity verbosity);
};

cdit::Logger cdit::Logger::log_singleton = Logger();

cdit::Logger::Logger()
{
    null_stream = new std::ostream(0);
    actual_output = &std::cout;
};

cdit::Logger::~Logger()
{
    delete null_stream;
};

cdit::Logger& cdit::Logger::logger()
{
    return log_singleton;
}

std::ostream& cdit::Logger::Logger::GetStream(Verbosity verbosity)
{
    if (VerbosityLevel(verbosity) < VerbosityLevel(verbosity_level)) {
        if (!first_log) {
            *actual_output << std::endl;
        } else {
            first_log = false; 
        }
        *actual_output << VerbosityString(verbosity);
        return *actual_output;
    }
    return *null_stream;
}

std::ostream& cdit::Log(cdit::Verbosity verbosity)
{
    return cdit::Logger::logger().GetStream(verbosity);
};

int cdit::Logger::VerbosityLevel(const cdit::Verbosity verbosity)
{
    switch (verbosity) {
        case cdit::Verbosity::DEBUG:
            return 5;
        case cdit::Verbosity::INFO:
            return 4;
        case cdit::Verbosity::WARNING:
            return 3;
        case cdit::Verbosity::ERROR:
            return 2;
        case cdit::Verbosity::FATAL:
            return 1;
    }
};

std::string cdit::Logger::VerbosityString(const cdit::Verbosity verbosity)
{
    switch (verbosity) {
        case cdit::Verbosity::DEBUG:
            return "[DEBUG] ";
        case cdit::Verbosity::INFO:
            return "[INFO] ";
        case cdit::Verbosity::WARNING:
            return "[WARNING] ";
        case cdit::Verbosity::ERROR:
            return "[ERROR] ";
        case cdit::Verbosity::FATAL:
            return "[FATAL] ";
    }
};

using namespace cdit;

qInstallMessageHandler(QtMessageLogger);
void QtMessageLogger(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    Verbosity verbosity = Verbosity::DEBUG;
    switch (type) {
    case QtDebugMsg:
        verbosity = Verbosity::DEBUG;
        break;
    case QtInfoMsg:
        verbosity = Verbosity::INFO;
        break;
    case QtWarningMsg:
        verbosity = Verbosity::WARNING;
        break;
    case QtCriticalMsg:
        verbosity = Verbosity::ERROR;
        break;
    case QtFatalMsg:
        verbosity = Verbosity::FATAL;
        break;
    }
    Log(verbosity) << localMsg.constData();
}

#endif // CDITMA_LOG_H