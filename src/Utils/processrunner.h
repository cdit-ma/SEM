#ifndef PROCESSRUNNER_H
#define PROCESSRUNNER_H

#include <QObject>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcessEnvironment>
struct ProcessResult{
    bool success = false;
    int error_code = 0;
    QStringList standard_output;
    QStringList standard_error;
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

    ProcessResult RunProcess(QString program, QStringList args, QString directory="");
    HTTPResult HTTPGet(QNetworkRequest request);
    HTTPResult HTTPPost(QNetworkRequest request, QByteArray post_data = QByteArray());

    QString getEnvVar(QString key);
signals:
    void Cancel();
    void GotProcessStdOutLine(QString line);
    void GotProcessStdErrLine(QString line);
private:
    HTTPResult WaitForNetworkReply(QNetworkReply* reply);
    QNetworkAccessManager* GetNetworkAccessManager();
    QNetworkAccessManager* network_access_manager_;
    QProcessEnvironment global_vars;
};

#endif // PROCESSRUNNER_H
