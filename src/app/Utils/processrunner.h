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
struct ProcessResult {
    friend class ProcessRunner;
public:
    bool cancelled = false;
    bool success = false;
    int error_code = 0;
    QStringList standard_output;
    QStringList standard_error;
private:
    QProcess* process = nullptr;
};

struct HTTPResult {
    bool success = false;
    int error_code = 0;
    QString standard_output;
    QString location_header;
};

class ProcessRunner : public QObject
{
    Q_OBJECT
    
public:
    explicit ProcessRunner(QObject* parent = nullptr);

    QProcessEnvironment RunEnvVarScript(const QString& program);

    ProcessResult RunProcess(const QString& program, const QStringList& args, const QString& directory = "", QProcessEnvironment env = QProcessEnvironment());
    
    HTTPResult HTTPGet(const QNetworkRequest& request);
    HTTPResult HTTPPost(const QNetworkRequest& request, const QByteArray& post_data = QByteArray());
    HTTPResult HTTPPostMulti(const QNetworkRequest& request, QHttpMultiPart* post_data);

signals:
    void Cancel();
    void GotProcessStdOutLine(QString line);
    void GotProcessStdErrLine(QString line);
    
private:
    ProcessResult RunProcess_(const QString& program, const QStringList& args, const QString& directory, const QProcessEnvironment& env , bool emit_stdout);
    QNetworkAccessManager* GetNetworkAccessManager();
    HTTPResult WaitForNetworkReply(QNetworkReply* reply);

    QNetworkAccessManager* network_access_manager_ = nullptr;
    QProcessEnvironment global_vars;
};

#endif // PROCESSRUNNER_H
