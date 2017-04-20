#include "processrunner.h"
#include <QProcess>
#include <QDebug>
#include <QEventLoop>
#include <QtConcurrent/QtConcurrentRun>

ProcessRunner::ProcessRunner(bool terminateOnFinished) : QObject()
{
    terminate = terminateOnFinished;

    connect(this, &ProcessRunner::Terminate, this, &QObject::deleteLater);
}

ProcessRunner::~ProcessRunner()
{
    qCritical() << "DED!";
}

QFuture<void> ProcessRunner::RunProcess(QString program, QStringList args, QString directory)
{
    auto future = QtConcurrent::run(QThreadPool::globalInstance(), this, &ProcessRunner::RunProcess_, program, args, directory);

    return future;
}

CommandResult ProcessRunner::RunProcess_(QString program, QStringList args, QString directory)
{
    //Reset
    auto result = CommandResult();
    //Register
    qRegisterMetaType<QProcess::ExitStatus>();

    //Construct and setup the process
    QProcess* process = new QProcess();

    if(!directory.isEmpty()){
        process->setWorkingDirectory(directory);
    }

    qCritical() << "RunProcess: " << program << args.join(" ");
    qCritical() << "In Directory: " << directory;
    process->start(program, args);

    while(true){
        bool finished = false;
        bool read_ready = false;

        if(process->state() != QProcess::NotRunning){
            //Construct a wait loop
            QEventLoop waitLoop;

            //Connect to Standard output/error signals
            connect(process, &QProcess::readyReadStandardError, this, [&waitLoop, &read_ready](){waitLoop.quit(); read_ready = true;});
            connect(process, &QProcess::readyReadStandardOutput, this, [&waitLoop, &read_ready](){waitLoop.quit(); read_ready = true;});

            //Connect to exit status
            connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, [&waitLoop, &finished](int, QProcess::ExitStatus) {waitLoop.quit(); finished = true;});

            //Block until a signal notifies
            waitLoop.exec();
        }

        //Read StandardOutput
        process->setReadChannel(QProcess::StandardOutput);
        while(process->canReadLine()){
            QString line = process->readLine();
            result.standard_output << line;
            //qCritical() << line;
            emit GotStandardOutLine(line);
        }

        //Read StandardError
        process->setReadChannel(QProcess::StandardError);
        while(process->canReadLine()){
            QString line = process->readLine();
            result.standard_error << line;
            //qCritical() << line;
            emit GotStandardErrLine(line);
        }

        //Check if we are still running
        if(process->state() == QProcess::NotRunning){
            break;
        }
    }

    result.error_code = process->exitCode();
    result.success = process->exitStatus() == QProcess::NormalExit;
    if(result.success){
        result.error_code = 0;
    }else{
        result.error_code = 1;
    }

    delete process;
    emit Finished(result);
    if(terminate){
        emit Terminate();
    }

    return result;
}

