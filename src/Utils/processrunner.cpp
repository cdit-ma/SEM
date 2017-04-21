#include "processrunner.h"
#include <QProcess>
#include <QDebug>
#include <QEventLoop>
#include <QThread>


ProcessRunner::ProcessRunner(QObject *parent) : QObject(parent)
{
    network_access_manager_ = 0;
    //Register Exit Status
    qRegisterMetaType<QProcess::ExitStatus>();
    qRegisterMetaType<ProcessResult>("ProcessResult");
    qRegisterMetaType<HTTPResult>("HTTPResult");
}

ProcessResult ProcessRunner::RunProcess(QString program, QStringList args, QString directory)
{
    ProcessResult result;

    //Construct and setup the process
    QProcess process;

    //Set working directory if we have one
    if(!directory.isEmpty()){
        process.setWorkingDirectory(directory);
    }

    //Start the program
    process.start(program, args);

    while(true){
        if(process.state() != QProcess::NotRunning){
            bool cancelled = false;
            //Construct a wait loop
            QEventLoop waitLoop;

            //Connect to Standard output/error signals
            connect(&process, &QProcess::readyReadStandardError, &waitLoop, &QEventLoop::quit);
            connect(&process, &QProcess::readyReadStandardOutput, &waitLoop, &QEventLoop::quit);

            //Connect to Process finish Signal (Use lambda to quit the waitloop
            connect(&process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, [&waitLoop](int, QProcess::ExitStatus) {waitLoop.quit();});

            //Set a flag that we were cancelled
            connect(this, &ProcessRunner::Cancel, this, [&waitLoop, &cancelled](){cancelled = true; waitLoop.quit();});

            //Block until a signal notifies
            waitLoop.exec();

            if(cancelled){
                //Break out
                break;
            }
        }

        //Read StandardOutput
        process.setReadChannel(QProcess::StandardOutput);
        while(process.canReadLine()){
            QString line = process.readLine();
            result.standard_output << line;
            emit GotProcessStdOutLine(line);
        }

        //Read StandardError
        process.setReadChannel(QProcess::StandardError);
        while(process.canReadLine()){
            QString line = process.readLine();
            result.standard_error << line;
            emit GotProcessStdErrtLine(line);
        }

        //Check if we are still running
        if(process.state() == QProcess::NotRunning){
            break;
        }
    }

    //Set the result
    result.error_code = process.exitCode();
    result.success = process.exitStatus() == QProcess::NormalExit && result.error_code == 0;

    return result;
}



HTTPResult ProcessRunner::HTTPGet(QNetworkRequest request)
{
    HTTPResult result;

    QNetworkAccessManager* network_access_manager = GetNetworkAccessManager();

    if(network_access_manager && request.url().isValid()){
        //Post to the URL from the networkManager.
        QNetworkReply* reply =  network_access_manager->get(request);
        result = WaitForNetworkReply(reply);
        //Free up the memory of the Network Reply
        delete reply;
    }
    return result;
}

QNetworkAccessManager *ProcessRunner::GetNetworkAccessManager()
{
    if(!network_access_manager_){
        network_access_manager_ = new QNetworkAccessManager(this);
    }
    return network_access_manager_;
}

HTTPResult ProcessRunner::HTTPPost(QNetworkRequest request, QByteArray post_data)
{
    HTTPResult result;

    QNetworkAccessManager* network_access_manager = GetNetworkAccessManager();

    if(network_access_manager && request.url().isValid()){
        //Post to the URL from the networkManager.
        QNetworkReply* reply =  network_access_manager->post(request, post_data);
        result = WaitForNetworkReply(reply);
        //Free up the memory of the Network Reply
        delete reply;
    }
    return result;
}
HTTPResult ProcessRunner::WaitForNetworkReply(QNetworkReply *reply)
{
    HTTPResult result;

    while(true){
        if(reply->isRunning()){
            bool cancelled = false;
            //Construct a wait loop
            QEventLoop waitLoop;

            //Connect to Standard output/error signals
            connect(reply, &QNetworkReply::readyRead, &waitLoop, &QEventLoop::quit);
            connect(reply, &QNetworkReply::finished, &waitLoop, &QEventLoop::quit);

            //Set a flag that we were cancelled
            connect(this, &ProcessRunner::Cancel, this, [&waitLoop, &cancelled](){cancelled = true; waitLoop.quit();});

            //Block until a signal notifies
            waitLoop.exec();

            if(cancelled){
                break;
            }
        }

        qint64 bytes_available = reply->bytesAvailable();

        if(bytes_available > 0){
            //Attach Data.
            result.standard_output.append(reply->read(bytes_available));
        }

        if(bytes_available == 0 && reply->isFinished()){
            break;
        }
    }

    result.error_code = reply->error();
    result.success = reply->error() == QNetworkReply::NoError;
    result.location_header = reply->header(QNetworkRequest::LocationHeader).toString();
    return result;
}
