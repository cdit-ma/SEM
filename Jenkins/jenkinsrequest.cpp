#include "jenkinsrequest.h"
#include "jenkinsmanager.h"
#include <QNetworkReply>
#include <QTimer>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#include <QRegularExpression>

#define TIME_OUT_MS 15000

/**
 * @brief JenkinsRequest::JenkinsRequest Constructor for a JenkinsRequest Thread. A JenkinsRequest Thread is used for a single request from the Jenkins Server. Connects all request SIGNALS to there matching private SLOTS.
 * @param m The JenkinsManager which spawned this JenkinsRequest.
 */
JenkinsRequest::JenkinsRequest(JenkinsManager *m)
{
    manager = m;
    //networkManager = 0;
    timeOutMS = TIME_OUT_MS;

    if(!manager){
        qCritical() << "Cannot Construct JenkinsRequest, JenkinsManager provided is NULL";
    }
}

/**
 * @brief JenkinsRequest::~JenkinsRequest Destructor, will remove this JenkinsRequest from the JenkinsManager's Vector of JenkinsRequest Objects.
 */
JenkinsRequest::~JenkinsRequest()
{
    //qCritical() << "Removing this Jenkins Request";
    //Tell the manager this Request is done.
    if(manager){
        manager->jenkinsRequestFinished(this);
    }
    while(managers.size() > 0){
        managers.takeFirst()->deleteLater();
    }
}

/**
 * @brief JenkinsRequest::wget Uses a QNetworkAccessManager to request a URL. Will timeout after a period of not recieving any data.
 * @param url - The Url to request.
 * @return - The ByteArray downloaded from the url. Can be empty if the Request Failed, or there was nothing to Download.
 */
QByteArray JenkinsRequest::wget(QString url)
{
    //The returnable byteArray.
    QByteArray byteArray;

    //If we don't have a networkManager, we should construct one.
    QNetworkAccessManager *networkManager = new QNetworkAccessManager();

    if(!networkManager){
        qCritical() << "Cannot Construct Network Manager";
        return byteArray;
    }
    managers.append(networkManager);

    //qCritical() << "Requesting URL: " << url;

    //Request the URL from the networkManager.
    QNetworkReply* reply =  networkManager->get(QNetworkRequest(QUrl(url)));

    bool processing = true;
    while(processing){
        bool timedOut = false;
        qint64 bytesAvailable = reply->bytesAvailable();

        //If the NetworkReply is running but we have no bytesLeft in our Buffer, wait for more data!
        if(reply->isRunning() && bytesAvailable == 0){
            //qCritical() << "Process is Running, waiting for new data";

            //Construct a timeout timer.
            QTimer* timeOutTimer = new QTimer();
            timeOutTimer->setSingleShot(true);

            //Construct a EventLoop which waits for the QNetworkReply to be finished, or more data to become available, or the timer to finish.
            QEventLoop waitLoop;
            connect(reply, SIGNAL(readyRead()), &waitLoop, SLOT(quit()));
            connect(reply, SIGNAL(finished()), &waitLoop, SLOT(quit()));
            connect(timeOutTimer, SIGNAL(timeout()), &waitLoop, SLOT(quit()));

            //Start the timer.
            timeOutTimer->start(timeOutMS);

            //Wait for something to quit the EventLoop
            waitLoop.exec();

            //Check the state of the timer.
            timedOut = !timeOutTimer->isActive();

            //Stop the timer.
            timeOutTimer->stop();

            //Clear the memory of the timer
            timeOutTimer->deleteLater();

            //After the readyRead signal, or timeout, update the available bytes
            bytesAvailable = reply->bytesAvailable();
        }

        //If we have bytes, read them and append them to the data we already have.
        if(bytesAvailable > 0){
            QByteArray newData = reply->read(bytesAvailable);
            //Add them to the returnable data.
            byteArray += newData;
        }

        //If the process is at the end of the buffer and either the Process is finished Running or we Timed out.
        if(reply->atEnd() && (reply->isFinished() || timedOut)){
            if(timedOut){
                emit requestFailed();
                qCritical() << "wget: Timed Out: " << url;
            }
            if(reply->error() != QNetworkReply::NoError){
                emit requestFailed();
                qCritical() << "QNetwork Error: " << reply->errorString();
            }
            //Break the loop.
            processing = false;
        }
    }
    //Free up the memory of the Network Reply
    reply->deleteLater();


    //delete networkManager;
    //networkManager->deleteLater();

    //Return the byteArray
    return byteArray;
}

/**
 * @brief JenkinsRequest::runProcess Runs a QProcess which will lock until the command passed has completed. Will run for the life of the commands execution. Produces gotLiveCLIOutput signals as data becomes available. Will return the entire Standard Output of the command.
 * @param command The complete command to execute.
 * @return The entire output of the process's execution.
 */
QByteArray JenkinsRequest::runProcess(QString command)
{
    //The returnable byteArray.
    QByteArray byteArray;

    //Construct and setup the process
    QProcess* process = new QProcess();
    process->setReadChannel(QProcess::StandardOutput);
    process->setWorkingDirectory(manager->getCLIPath());
    process->start(command);

    bool processing = true;
    while(processing){
        qint64 bytesAvailable = process->bytesAvailable();

        //If the process is running but we have no bytesLeft in our Buffer, wait for more data!
        if(process->state() == QProcess::Running && bytesAvailable == 0){
            //qCritical() << "Process is Running, waiting for new data";

            //Construct a EventLoop which waits for the QNetworkReply to be finished, or more data to become available.
            QEventLoop waitLoop;
            connect(process, SIGNAL(readyRead()), &waitLoop, SLOT(quit()));
            connect(process, SIGNAL(finished(int)), &waitLoop, SLOT(quit()));

            //Wait for something to quit the EventLoop
            waitLoop.exec();

            //After the readyRead signal, or timeout, update the bytesAvailable
            bytesAvailable = process->bytesAvailable();
        }

        //If we have bytes, read them, and output them!
        if(bytesAvailable > 0){
            QByteArray newData = process->read(bytesAvailable);

            QString stringData(newData);
            if(stringData != ""){
                //Send a signal with the live output.
                gotLiveCLIOutput(this->jobName, this->buildNumber, this->activeConfiguration, stringData);
            }

            //Add them to the returnable data.
            byteArray += newData;
        }

        //If the process is at the end of the buffer and the Process is no longer Running. Break the loop.
        if(process->atEnd() && process->state() == QProcess::NotRunning){
            //qCritical() << "Process is finished!";
            processing = false;
        }
    }
    //Free the memory of the Process
    delete process;
    //Return the byte Array.
    return byteArray;
}

/**
 * @brief JenkinsRequest::getJobConfiguration Gets the JSON configuration of a Jenkins job. Will store this configuration in it's JenkinsManager if it requests new information.
 * @param jobName The name of the Jenkins job
 * @param buildNumber The build number of the job (Can be -1 for the root job)
 * @param activeConfiguration The name of the Active Configuration (Can be empty for the root job)
 * @param reRequest If set to true, this function will request fresh JSON data from the Jenkins Server.
 * @return The Configuration in JSON format for the provided job.
 */
QJsonDocument JenkinsRequest::getJobConfiguration(QString jobName, int buildNumber, QString activeConfiguration, bool reRequest)
{
    //If we have been provided a valid activeConfiguration, append this to the jobName
    if(activeConfiguration != ""){
        jobName += "/" + activeConfiguration;
    }

    //If we have been provided a valid build number, append this to the jobName
    if(buildNumber > -1){
        jobName += "/" + QString::number(buildNumber);
    }

    //Check if the JenkinsManager has an old Job Configuration for this jobName.
    QJsonDocument configuration = manager->getJobConfiguration(jobName);

    //If the configuration is NULL or we are to re-request the information. Get the configuration.
    if(configuration.isNull() || reRequest){
        //Construct the url to request using wget.
        QString configurationURL  = manager->getURL() + "job/" + jobName + "/api/json";

        //Get the the data from the URL using wget.
        QByteArray data = wget(configurationURL);

        //Construct a JSON document from the data from wget.
        configuration = QJsonDocument::fromJson(data);

        //If we have a valid JSON document, store it in the manager for future use.
        if(!configuration.isNull()){
            manager->storeJobConfiguration(jobName, configuration);
        }
    }

    //return the configuration.
    return configuration;

}

/**
 * @brief JenkinsRequest::storeRequestParameters Stores the parameters of the Request to be used by returning signals.
 * @param jobName The name of the Jenkins job
 * @param buildNumber The build number of the job (Can be -1 for the root job)
 * @param activeConfiguration The activeConfigurationonfiguration (Can be empty for the root job)
 */
void JenkinsRequest::storeRequestParameters(QString jobName, int buildNumber, QString activeConfiguration)
{
    this->jobName = jobName;
    this->activeConfiguration = activeConfiguration;
    this->buildNumber = buildNumber;
}

/**
 * @brief JenkinsRequest::getJobParameters Gets the List of Build Parameters for the Jenkins job, with name JobName. Emits a singular gotJobParameters signal after data has been retrieved from the Jenkins Server. Emits a requestFinished signal on completion.
 * @param jobName The name of the Jenkins job
 */
void JenkinsRequest::getJobParameters(QString jobName)
{
    //Store the parameters provided.
    storeRequestParameters(jobName);

    Jenkins_JobParameters jobParameters;

    //Get the configuration for the Job.
    QJsonDocument configuration = getJobConfiguration(jobName);

    if(!configuration.isNull()){
        QJsonObject configData = configuration.object();

        //Get the parameterDefinitions Array from the actions Array.
        QJsonArray parameters = configData["actions"].toArray()[0].toObject()["parameterDefinitions"].toArray();

        //For each parameter in the parameters Array, add it to the returnable list.
        foreach(QJsonValue parameter, parameters){
            QJsonObject parameterData = parameter.toObject();

            Jenkins_Job_Parameter jobParameter;

            //Fill the data in the Jenkins_Job_Parameter from the JSON object.
            jobParameter.name = parameterData["name"].toString();
            //The type is in format [type]ParameterDefinition, so trim this.
            jobParameter.type = parameterData["type"].toString().replace("ParameterDefinition","");
            jobParameter.description = parameterData["description"].toString();
            jobParameter.defaultValue = parameterData["defaultParameterValue"].toObject()["value"].toString();

            //Add it the jobParameter List
            jobParameters.append(jobParameter);
        }
    }

    //Emit the SIGNAL to send the data back.
    emit gotJobParameters(jobName, jobParameters);

    //Emit the SIGNAL to state the JenkinsRequest
    emit requestFinished();
}

/**
 * @brief JenkinsRequest::getJobState Will periodically get the Job State of a Jenkins job (whilst it the Jenkins Server says it's building). Will emit a gotJobStateChange signal at every state change. Emits a requestFinished signal on completion.
 * @param jobName The name of the Jenkins job
 * @param buildNumber The build number of the job
 * @param activeConfiguration The name of the Active Configuration (Can be empty for the root job)
 */
void JenkinsRequest::getJobState(QString jobName, int buildNumber, QString activeConfiguration)
{
    //Store the parameters provided.
    storeRequestParameters(jobName, buildNumber, activeConfiguration);

    bool firstTime = true;

    //Get the current state of the job.
    JOB_STATE currentState = _getJobState(jobName, buildNumber, activeConfiguration);
    JOB_STATE previousState = currentState;

    //While the job is either building or not yet existant.
    while(currentState == BUILDING || currentState == NO_JOB){
        //qCritical() << "GETTING JOB STATE" << jobName << buildNumber << activeConfiguration;
        //If the state has changed, or it's the first time into this loop, emit a gotJobStateChange signal.
        if(currentState != previousState || firstTime){
            emit gotJobStateChange(this->jobName, this->buildNumber, this->activeConfiguration, currentState);
            previousState = currentState;
            firstTime = false;
        }

        //Construct a timeout timer.
        QTimer* timeOutTimer = new QTimer();
        timeOutTimer->setSingleShot(true);

        //Construct a EventLoop which waits for the timer to finish.
        QEventLoop waitLoop;
        connect(timeOutTimer, SIGNAL(timeout()), &waitLoop, SLOT(quit()));
        timeOutTimer->start(timeOutMS);

        //Wait for something to quit the EventLoop
        waitLoop.exec();
        timeOutTimer->stop();
        timeOutTimer->deleteLater();

        //Update the current state.
        currentState = _getJobState(jobName, buildNumber, activeConfiguration);
    }

    //emit a gotJobStateChange signal as the job is either BUILT or FAILED
    emit gotJobStateChange(this->jobName, this->buildNumber, this->activeConfiguration, currentState);
    //Call the SIGNAL to teardown the JenkinsRequest
    emit requestFinished();
}

/**
 * @brief JenkinsRequest::runGroovyScript Executes a Groovy Script(File) on the Jenkins Server and returns the data
 * @param groovyScriptPath The Absolute path to the Groovy Script file.
 */
void JenkinsRequest::runGroovyScript(QString groovyScriptPath)
{
    //Construct the console CLI request command
    QString command = "groovy " + groovyScriptPath;

    //Execute the Wrapped CLI Command in a process. Will produce gotLiveCLIOutput as data becomes available.
    QByteArray commandData = runProcess(manager->getCLICommand(command));

    //Parse the returned data as a String
    QString consoleOutput = QString(commandData);

    //Call the SIGNAL to send the data back.
    emit gotGroovyScriptOutput(consoleOutput);

    //Call the SIGNAL to teardown the JenkinsRequest
    emit requestFinished();
}

/**
 * @brief JenkinsRequest::getJobConsoleOutput Gets the console output of a Jenkins job. If the job is still running, it will emit the gotLiveCLIOutput as data becomes available in the pipe. Once Finished it will Emit a gotJobConsoleOutput signal. Emits a requestFinished signal on completion.
 * @param jobName The name of the Jenkins job
 * @param buildNumber The build number of the job
 * @param activeConfiguration The name of the Active Configuration (Can be empty for the root job)
 */
void JenkinsRequest::getJobConsoleOutput(QString jobName, int buildNumber, QString activeConfiguration)
{
    //Store the Parameters so that other signals can send the appropriate data.
    storeRequestParameters(jobName, buildNumber, activeConfiguration);

    //If we have been provided a valid activeConfiguration, append this to the jobName
    if(activeConfiguration != ""){
        jobName += "/" + activeConfiguration;
    }

    QString buildNo = "";
    //If we have been provided a valid build number, append this to the jobName
    if(buildNumber > -1){
        buildNo = QString::number(buildNumber);
    }

    //Construct the console CLI request command
    QString command = "console " + jobName + " " + buildNo + " -f";

    //Execute the Wrapped CLI Command in a process. Will produce gotLiveCLIOutput as data becomes available.
    QByteArray commandData = runProcess(manager->getCLICommand(command));

    //Parse the returned data as a String
    QString consoleOutput = QString(commandData);

    //Call the SIGNAL to send the data back.
    emit gotJobConsoleOutput(this->jobName, this->buildNumber, this->activeConfiguration, consoleOutput);

    //Call the SIGNAL to teardown the JenkinsRequest
    emit requestFinished();

}


/**
 * @brief JenkinsRequest::getJobActiveConfigurations Gets the list of activeConfigurations (Nodes running the job, includes a blank node to represent the Master Job) the Jenkins job provided has. Emits the gotJobactiveConfigurations signal with the output. Emits a requestFinished signal on completion.
 * @param jobName The name of the Jenkins job
 */
void JenkinsRequest::getJobActiveConfigurations(QString jobName)
{
    //Store the jobName
    storeRequestParameters(jobName);

    //Get the list of configurations.
    QStringList configurationList = _getJobActiveConfigurations(jobName);

    //Call the SIGNAL to send the data back.
    emit gotJobActiveConfigurations(jobName, configurationList);
    //Call the SIGNAL to teardown the JenkinsRequest
    emit requestFinished();
}

/**
 * @brief JenkinsRequest::buildJob Starts and waits for a Job to build on the Jenkins Server. Calls getJobState to emit the state changes of the Job.
 * @param jobName The name of the Jenkins job
 * @param jobParameters A list of parameters for the Jenkis Job
 */
void JenkinsRequest::buildJob(QString jobName, Jenkins_JobParameters jobParameters)
{
    QString command = "build " + jobName + " ";

    //Serialize arguments.
    foreach(Jenkins_Job_Parameter parameter, jobParameters){
        command += "-p " + parameter.name + "=" + parameter.value + " ";
    }

   //Add options to pipe the output of the root job.
   command += "-s -v ";

   //Execute the Wrapped CLI Command in a process. Will produce gotLiveCLIOutput as data becomes available.
   QByteArray commandData = runProcess(manager->getCLICommand(command));

    //Parse the returned data as a String
   QString result = QString(commandData);

   int buildNumber = -1;

   //If Job is MultiConfiguration. Output tells us which job Number.
   if(_isJobAMatrixProject(jobName)){
       //Started [jobName] #[buildNumber]
       //Construct a Regex Expression to match and get the Number.
       QRegularExpression regex("Started " + jobName + " #([0-9]+)");

       QRegularExpressionMatch resultMatch = regex.match(result);
       if(resultMatch.hasMatch()){
           QString match = resultMatch.captured(1);
           //If toInt fails, buildNumber will be 0
           buildNumber = match.toInt();
           if(buildNumber == 0){
               buildNumber = -1;
           }
       }
   }

   //Gets the Job State of the Root Job, and will call the SIGNAL to teardown the JenkinsRequest
   getJobState(jobName, buildNumber, "");
}

/**
 * @brief JenkinsRequest::_getJobActiveConfigurations Gets the list of ActiveConfigurations (Nodes running the Job) for a Jenkins job. Will return a list including "" to represent the root node. Trys to use a local copy of the Configuration.
 * @param jobName The name of the Jenkins job
 * @return A list of ActiveConfiguration names
 */
QStringList JenkinsRequest::_getJobActiveConfigurations(QString jobName)
{
    QStringList configurationList;
    //Add a blank Configuration to represent the parent job
    configurationList.append("");
    //Get the jobJSON from the JenkinsManager (Or request a copy if none has been requested)
    QJsonDocument configuration  = getJobConfiguration(jobName);

    //If the jobJSON isn't null, check for activeConfigurations.
    if(!configuration.isNull()){
        QJsonObject configData = configuration.object();

        //Get the array which contains the activeConfigurations.
        QJsonArray activeConfigurations = configData["activeConfigurations"].toArray();

        //Append The name of the Active Configurations to the configurationList
        foreach(QJsonValue activeConfiguration, activeConfigurations){
            configurationList.append(activeConfiguration.toObject()["name"].toString());
        }
    }
    return configurationList;
}

/**
 * @brief JenkinsRequest::_isJobAMatrixProject Checks if a Jenkins job is a Matrix (Or Multi-Configuration) Job by checking how many ActiveConfigurations the Job has.
 * @param jobName The name of the Jenkins job
 * @return
 */
bool JenkinsRequest::_isJobAMatrixProject(QString jobName)
{
    return _getJobActiveConfigurations(jobName).size() > 1;
}

/**
 * @brief JenkinsRequest::_getJobState Gets the current state of a Jenkins job.
 * @param jobName The name of the Jenkins job
 * @param buildNumber The build number of the job (Can be -1 for the root job)
 * @param activeConfiguration The name of the Active Configuration (Can be empty for the root job)
 * @return The state of the Jenkins job.
 */
JOB_STATE JenkinsRequest::_getJobState(QString jobName, int buildNumber, QString activeConfiguration)
{
    //Get the JSON data about this job.
    QJsonDocument configuration  = getJobConfiguration(jobName, buildNumber, activeConfiguration, true);

    //If the jobJSON isn't null, check for activeConfigurations.
    if(!configuration.isNull()){
        QJsonObject configData = configuration.object();

        //Get the array which contains the result.
        bool building = configData["building"].toBool();
        if(building){
            return BUILDING;
        }else{
            QString result = configData["result"].toString();
            if(result == "SUCCESS"){
                return BUILT;
            }else{
                return FAILED;
            }
        }
    }
    return NO_JOB;
}

