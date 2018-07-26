#ifndef PROCESSRUNNER_H
#define PROCESSRUNNER_H

#include <QObject>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcessEnvironment>
#include <QProcess>

class ProcessRunner;
struct ProcessResult{
    friend class ProcessRunner;
public:
    bool cancelled = false;
    bool success = false;
    int error_code = 0;
    QStringList standard_output;
    QStringList standard_error;
private:
    QProcess* process = 0;
};

struct HTTPResult{
    bool success = false;
    int error_code = 0;
    QString standard_output;
    QString location_header;
};

class ProcessRunner : public QObject
{
    Q_OBJECT
public:
    ProcessRunner(QObject* parent = 0);

    QProcessEnvironment RunEnvVarScript(QString program);

    ProcessResult RunProcess(QString program, QStringList args, QString directory="", QProcessEnvironment env = QProcessEnvironment());
    
    HTTPResult HTTPGet(QNetworkRequest request);
    HTTPResult HTTPPost(QNetworkRequest request, QByteArray post_data = QByteArray());
    HTTPResult HTTPPostMulti(QNetworkRequest request, QHttpMultiPart* post_data);

signals:
    void Cancel();
    void GotProcessStdOutLine(QString line);
    void GotProcessStdErrLine(QString line);
private:
    ProcessResult RunProcess_(QString program, QStringList args, QString directory, QProcessEnvironment env , bool emit_stdout);
    QNetworkAccessManager* GetNetworkAccessManager();
    HTTPResult WaitForNetworkReply(QNetworkReply* reply);
private:
    QNetworkAccessManager* network_access_manager_;
    QProcessEnvironment global_vars;
};

#endif // PROCESSRUNNER_H
