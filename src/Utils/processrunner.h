#ifndef PROCESSRUNNER_H
#define PROCESSRUNNER_H

#include <QStringList>
#include <QObject>
#include <QFuture>

struct CommandResult{
    bool success = false;
    int error_code = 0;
    QStringList standard_output;
    QStringList standard_error;
};

class ProcessRunner : public QObject
{
    Q_OBJECT
public:
    ProcessRunner(bool terminateOnFinished=true);
    ~ProcessRunner();
    QFuture<void> RunProcess(QString program, QStringList args, QString directory="");
signals:
    void GotStandardOutLine(QString line);
    void GotStandardErrLine(QString line);
    void Finished(CommandResult results);
    void Terminate();
private:
    bool terminate;
    CommandResult RunProcess_(QString program, QStringList args, QString directory="");
};

#endif // PROCESSRUNNER_H
