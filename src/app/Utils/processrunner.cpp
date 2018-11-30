#include "processrunner.h"
#include <QProcess>
#include <QDebug>
#include <QEventLoop>
#include <QThread>
#include <QHttpMultiPart>


ProcessRunner::ProcessRunner(QObject *parent) : QObject(parent)
{
    network_access_manager_ = 0;
    //Register Exit Status
    qRegisterMetaType<QProcess::ExitStatus>();
    qRegisterMetaType<ProcessResult>("ProcessResult");
    qRegisterMetaType<HTTPResult>("HTTPResult");
    global_vars = QProcessEnvironment::systemEnvironment();
}

/**
 * @brief CUTSManager::getEnvFromScript Executes a .bat/.sh script and constructs a QProcessEnvironment to put the newly set environment variables into.
 * @param scriptPath The path to the script file.
 * @return
 */
 QProcessEnvironment ProcessRunner::RunEnvVarScript(QString scriptPath)
 {
    //Copy the global environment_vars
    QProcessEnvironment env_vars = global_vars;
    QProcessEnvironment black_list_vars;
    
    QProcess process;
    //Run with an empty environment
    process.setProcessEnvironment(black_list_vars);

    QString program;
    QString shell_command;
    QString print_vars;
    
    #ifdef _WIN32
        program = "cmd.exe";
        print_vars = "set&exit\n";
        shell_command = scriptPath + ">NUL&" + print_vars;
    #else
        program = "/bin/bash";
        print_vars = "set;exit\n";
        shell_command = ". " + scriptPath + ">/dev/null 2>&1;" + print_vars;
    #endif

 
    //Print the env_vars for an empty shell environment(So we can ignore these keys)
    process.start(program);
    process.waitForStarted();
    process.write(print_vars.toUtf8());
    process.waitForFinished(-1);

    while(process.canReadLine()){
        auto line = process.readLine().simplified();
        auto split_pos = line.indexOf('=');
        if(split_pos > 0){
            auto key = line.left(split_pos);
            auto value = line.mid(split_pos + 1);
            //Any keys we get, we want to ignore them
            black_list_vars.insert(key, value);
        }
    }
    
    //Clear our environment again
    process.setProcessEnvironment(env_vars);

    //Run the shell script, and then print the env_vars
    process.start(program);
    process.waitForStarted();
    process.write(shell_command.toUtf8());
    process.waitForFinished(-1);

    while(process.canReadLine()){
        auto line = process.readLine().simplified();
        auto split_pos = line.indexOf('=');
        if(split_pos > 0){
            auto key = line.left(split_pos);
            auto value = line.mid(split_pos + 1);
            
            auto black_list_value = black_list_vars.value(key);

            //Only care about the environment variables which exist soly in
            if(black_list_value != value){
                env_vars.insert(key, value);
            }
        }
    }
    return env_vars;
 }

ProcessResult ProcessRunner::RunProcess(QString program, QStringList args, QString directory, QProcessEnvironment env){
    if(env.isEmpty()){
        env = global_vars;
    }
    auto result = RunProcess_(program, args, directory, env, true);
    
    //Clean up the QProcess and return
    delete result.process;
    result.process = 0;
    return result;
}

ProcessResult ProcessRunner::RunProcess_(QString program, QStringList args, QString directory, QProcessEnvironment env, bool emit_stdout){
    ProcessResult result;
    
    //Construct and setup the process
    result.process = new QProcess();
    if(!env.isEmpty()){
        result.process->setProcessEnvironment(env);
    }


    //Set working directory if we have one
    if(!directory.isEmpty()){
        result.process->setWorkingDirectory(directory);
    }

    //qCritical() << "Running: Program: '" << program << "' " << args.join(" ") << " IN: " << directory;

    //Start the program
    result.process->start(program, args);
    //Try and start
    bool started = result.process->waitForStarted();
    while(started){
        //If started
        if(result.process->state() != QProcess::NotRunning){
            bool cancelled = false;
            //Construct a wait loop
            QEventLoop waitLoop;

            //Connect to Standard output/error signals
            connect(result.process, &QProcess::readyReadStandardError, &waitLoop, &QEventLoop::quit);
            connect(result.process, &QProcess::readyReadStandardOutput, &waitLoop, &QEventLoop::quit);

            //Connect to Process finish Signal (Use lambda to quit the waitloop
            connect(result.process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, [&waitLoop](int, QProcess::ExitStatus) {waitLoop.quit();});

            //Set a flag that we were cancelled
            connect(this, &ProcessRunner::Cancel, this, [&waitLoop, &cancelled](){cancelled = true; waitLoop.quit();});

            //Block until a signal notifies
            waitLoop.exec();

            if(cancelled){
                emit GotProcessStdErrLine("**ProcessRunner Interupted**");

                //Forcefully terminate
                result.cancelled = true;
                result.process->terminate();
                result.process->waitForFinished(-1);
                if(result.process->state() != QProcess::NotRunning){
                    result.process->kill();
                }
                //Break out
                break;
            }
        }

        //Read StandardOutput
        result.process->setReadChannel(QProcess::StandardOutput);
        while(result.process->canReadLine()){
            QString line = result.process->readLine();
            line = line.remove('\n');
            line = line.remove('\r');
            result.standard_output << line;
            emit GotProcessStdOutLine(line);
        }

        //Read StandardError
        result.process->setReadChannel(QProcess::StandardError);
        while(result.process->canReadLine()){
            QString line = result.process->readLine();
            line = line.remove('\n');
            line = line.remove('\r');
            result.standard_error << line;
            emit GotProcessStdErrLine(line);
        }

        //Check if we are still running
        if(result.process->state() == QProcess::NotRunning){
            break;
        }
    }

    if(started){
        //Set the result
        result.error_code = result.process->exitCode();
        result.success = result.process->exitStatus() == QProcess::NormalExit && result.error_code == 0;
    }

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

HTTPResult ProcessRunner::HTTPPostMulti(QNetworkRequest request, QHttpMultiPart* post_data)
{
    HTTPResult result;

    auto network_access_manager = GetNetworkAccessManager();

    if(network_access_manager && request.url().isValid()){
        QScopedPointer<QNetworkReply> reply(network_access_manager->post(request, post_data));
        //Post to the URL from the networkManager.
        result = WaitForNetworkReply(reply.get());
    }
    delete post_data;
    
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
