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
#include <QAuthenticator>
#include <QUrlQuery>

#define TIME_OUT_MS 15000

/**
 * @brief JenkinsRequest::JenkinsRequest Constructor for a JenkinsRequest Thread. A JenkinsRequest Thread is used for a single request from the Jenkins Server. Connects all request SIGNALS to there matching private SLOTS.
 * @param m The JenkinsManager which spawned this JenkinsRequest.
 */
JenkinsRequest::JenkinsRequest(JenkinsManager *m)
{
    managers = 0;
    manager = m;
    terminated = false;
    //networkManager = 0;
    timeOutMS = TIME_OUT_MS;

    //Connect the unexpected Termination signal to a slot.
    connect(this, SIGNAL(unexpectedTermination()), this, SLOT(_unexpectedTermination()));
    connect(this, SIGNAL(requestFinished()), this, SLOT(deleteLater()));

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
    if(managers){
        managers->deleteLater();
    }
}


/**
 * @brief JenkinsRequest::wget Uses a QNetworkAccessManager to request a URL. Will timeout after a period of not recieving any data.
 * @param url - The Url to request.
 * @return - The returncode of the action and the ByteArray downloaded from the url. Can be empty if the Request Failed, or there was nothing to Download.
 */
QPair<int, QByteArray> JenkinsRequest::wget(QString url)
{
    //The returnable byteArray.
    QByteArray byteArray;
    int returnCode = -1;


    QNetworkAccessManager* networkManager = getNetworkManager();

    if(networkManager && manager){
        qCritical() << "Requesting URL: " << url;

        //Construct the post request, Authenticated
        QNetworkRequest request = manager->getAuthenticatedRequest(url);


        //Request the URL from the networkManager.
        QNetworkReply* reply =  networkManager->get(request);

        //Wait for the reply.
        QPair<int, QByteArray> response = waitForReply(reply);
        returnCode = response.first;
        byteArray = response.second;

        //Free up the memory of the Network Reply
        reply->deleteLater();
    }

    //Return the byteArray
    return QPair<int, QByteArray>(returnCode, byteArray);
}

/**
 * @brief JenkinsRequest::post Uses a QNetworkAccessManager to post to a URL with the Data Provided.
 * @param url - The Url to post.
 * @param data - The ByteArray for the data to post. Defaults to Empty.
 * @return - The returncode of the action and the ByteArray response from the url posted to. Can be empty if the Request Failed, or there was nothing returned.
 */
QPair<int, QByteArray> JenkinsRequest::post(QString url, QByteArray data)
{
    QByteArray byteArray;

    QNetworkAccessManager* networkManager = getNetworkManager();
    int returnCode = -1;
    if(networkManager && manager){
        //qCritical() << "Posting to URL: " << url;
        //Construct the post request, Authenticated
        QNetworkRequest request = manager->getAuthenticatedRequest(url);

        //Post to the URL from the networkManager.
        QNetworkReply* reply =  networkManager->post(request, data);

        //Wait for the reply.
        QPair<int, QByteArray> response = waitForReply(reply);
        returnCode = response.first;
        byteArray = response.second;

        //Free up the memory of the Network Reply
        reply->deleteLater();
    }

    return QPair<int, QByteArray>(returnCode, byteArray);
}

/**
 * @brief JenkinsRequest::waitForReply Waits for either a timeout or a termination or a finished signal on the QNetworkReply, returns the data
 * @param reply - The Network request reply object.
 * @return - The returncode of the action and the Data returned from the request.
 */
QPair<int, QByteArray> JenkinsRequest::waitForReply(QNetworkReply *reply)
{
    QByteArray byteArray;
    bool processing = true;

    //Construct a timeout timer.
    QTimer* timeOutTimer = new QTimer();
    timeOutTimer->setSingleShot(true);
    int returnCode = -1;

    while(processing){
        bool timedOut = false;
        qint64 bytesAvailable = reply->bytesAvailable();

        //If the NetworkReply is running but we have no bytesLeft in our Buffer, wait for more data!
        if(reply->isRunning() && bytesAvailable == 0){
            //qCritical() << "Process is Running, waiting for new data";



            //Construct a EventLoop which waits for the QNetworkReply to be finished, or more data to become available, or the timer to finish.
            QEventLoop waitLoop;
            connect(reply, SIGNAL(readyRead()), &waitLoop, SLOT(quit()));
            connect(reply, SIGNAL(finished()), &waitLoop, SLOT(quit()));
            //Connect the unexpectedTermination to the wait loop.
            connect(this, SIGNAL(unexpectedTermination()), &waitLoop, SLOT(quit()));
            connect(timeOutTimer, SIGNAL(timeout()), &waitLoop, SLOT(quit()));

            //Start the timer.
            timeOutTimer->start(timeOutMS);

            //Check the state of the timer.
            timedOut = !timeOutTimer->isActive();

            //Stop the timer.
            timeOutTimer->stop();

            //After the readyRead signal, or timeout, update the available bytes
            bytesAvailable = reply->bytesAvailable();

            //Wait for something to quit the EventLoop
            waitLoop.exec();
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
                qCritical() << "Timed Out: " << reply->url();
            }
            if(reply->error() != QNetworkReply::NoError){
                emit requestFailed();
                qCritical() << "QNetwork Error: " << reply->errorString();
            }else{
                returnCode = 0;
            }
            //Break the loop.
            processing = false;
        }
    }
    //Delete the timer
    timeOutTimer->deleteLater();
    return QPair<int, QByteArray>(returnCode, byteArray);
}

/**
 * @brief JenkinsRequest::runProcess Runs a QProcess which will lock until the command passed has completed. Will run for the life of the commands execution. Produces gotLiveCLIOutput signals as data becomes available. Will return the entire Standard Output of the command.
 * @param command The complete command to execute.
 * @return The returncode of the action and the entire output of the process's execution.
 */
QPair<int, QByteArray> JenkinsRequest::runProcess(QString command)
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

        //If the process is running or starting but we have no bytesLeft in our Buffer, wait for more data!
        if(process->state() != QProcess::NotRunning && bytesAvailable == 0){
            //qCritical() << "Process is Running, waiting for new data";

            //Construct a EventLoop which waits for the QNetworkReply to be finished, or more data to become available.
            QEventLoop waitLoop;
            connect(process, SIGNAL(readyRead()), &waitLoop, SLOT(quit()));
            connect(process, SIGNAL(finished(int)), &waitLoop, SLOT(quit()));
            connect(this, SIGNAL(unexpectedTermination()), &waitLoop, SLOT(quit()));

            //Wait for something to quit the EventLoop
            waitLoop.exec();

            //After the readyRead signal, or timeout, update the bytesAvailable
            bytesAvailable = process->bytesAvailable();

            //Check for termination.
            if(terminated){
                //qCritical() << "Terminating the Jenkins Request!";
                //Terminate the process.
                process->terminate();
                bytesAvailable = 0;
            }
        }

        //If we have bytes, read them, and output them!
        if(bytesAvailable > 0){
            QByteArray newData = process->read(bytesAvailable);

            QString stringData(newData);
            if(stringData.length() > 0){
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


    int returnCode = process->exitCode();
    //Free the memory of the Process
    delete process;
    //Return the byte Array.
    return QPair<int, QByteArray>(returnCode, byteArray);
}

/**
 * @brief JenkinsRequest::waitForValidSettings Waits for the JenkinsManager to have validated Settings. Will time out after timeOutMS.
 * @return true/false for valid settings.
 */
bool JenkinsRequest::waitForValidSettings()
{
    if(manager){
        if(manager->hasValidatedSettings()){
            return true;
        }
    }

    QTimer* timeOutTimer = new QTimer();
    timeOutTimer->setSingleShot(true);

    QEventLoop waitLoop;
    connect(timeOutTimer, SIGNAL(timeout()), &waitLoop, SLOT(quit()));
    connect(manager, SIGNAL(settingsValidationComplete()), &waitLoop, SLOT(quit()));
    connect(this, SIGNAL(unexpectedTermination()), &waitLoop, SLOT(quit()));

    //Start the timer
    timeOutTimer->start(4 * timeOutMS);

    //Wait for something to quit the EventLoop
    waitLoop.exec();
    bool timedOut = !timeOutTimer->isActive();

    //Stop the timer
    timeOutTimer->stop();

    //Delete the timer
    timeOutTimer->deleteLater();


    //Check for termination
    if(terminated || timedOut){
        qCritical() << "Terminating the Jenkins Request!";
        return false;
    }else{
        //
        //Return what the value of the validation returned.
        return manager->hasValidatedSettings();
    }
}


/**
 * @brief JenkinsRequest::getNetworkManager Gets a new QNetworkAccessManager to use, Connects the Authentication request into the JenkinsManager
 * @return The QNetworkAccess Manager
 */
QNetworkAccessManager *JenkinsRequest::getNetworkManager()
{
    //If we don't have a networkManager, we should construct one.
    if(!managers){
        managers = new QNetworkAccessManager();
    }
    return managers;
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
	if(!manager){
		return QJsonDocument();
	}

    QJsonDocument configuration = manager->getJobConfiguration(jobName);

    //If the configuration is NULL or we are to re-request the information. Get the configuration.
    if(configuration.isNull() || reRequest){
        //Construct the url to request using wget.
        QString configurationURL  = manager->getURL() + "job/" + jobName + "/api/json";

        //Get the the data from the URL using wget.
        QPair<int, QByteArray> response = wget(configurationURL);


        //Construct a JSON document from the data from wget.
        configuration = QJsonDocument::fromJson(response.second);

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
     if(waitForValidSettings()){
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

    }else{
        emit requestFailed();
    }
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

    //Construct a timeout timer.
    QTimer* timeOutTimer = new QTimer();
    timeOutTimer->setSingleShot(true);

    //While the job is either building or not yet existant.
    while(currentState == BUILDING || currentState == NO_JOB){
        //qCritical() << "GETTING JOB STATE" << jobName << buildNumber << activeConfiguration;
        //If the state has changed, or it's the first time into this loop, emit a gotJobStateChange signal.
        if(currentState != previousState || firstTime){
            emit gotJobStateChange(this->jobName, this->buildNumber, this->activeConfiguration, currentState);
            previousState = currentState;
            firstTime = false;
        }

        //Construct a EventLoop which waits for the timer to finish.
        QEventLoop waitLoop;
        connect(timeOutTimer, SIGNAL(timeout()), &waitLoop, SLOT(quit()));
        connect(this, SIGNAL(unexpectedTermination()), &waitLoop, SLOT(quit()));

        //Start the timer
        timeOutTimer->start(timeOutMS);

        //Wait for something to quit the EventLoop
        waitLoop.exec();

        //Stop the timer
        timeOutTimer->stop();

        //Check for termination
        if(terminated){
            //qCritical() << "Terminating the Jenkins Request!";
            break;
        }

        //Update the current state.
        currentState = _getJobState(jobName, buildNumber, activeConfiguration);
    }

    //Delete the timer
    timeOutTimer->deleteLater();

    //emit a gotJobStateChange signal as the job is either BUILT or FAILED
    emit gotJobStateChange(this->jobName, this->buildNumber, this->activeConfiguration, currentState);
    //Call the SIGNAL to teardown the JenkinsRequest
    emit requestFinished();
}

void JenkinsRequest::validateJenkinsSettings()
{
    QString command = "login";

    //Execute the Wrapped CLI Command in a process. Will produce gotLiveCLIOutput as data becomes available.

    QPair<int, QByteArray> response = runProcess(manager->getCLICommand(command));
    QString result = "";
    bool success = false;
    if(response.first == 0){
        success = true;
    }else if(response.first == -1){
        result = "Authentication Failed as user: " + manager->getUsername() + " Failed!";
    }else if(response.first >= 1){
        result = "Cannot Reach Server Address";
    }
    emit gotSettingsValidationResponse(success, result);

    emit requestFinished();
}

void JenkinsRequest::waitForJobNumber(QString jobName, int buildNumber, QString activeConfiguration, QString outputChunk)
{
    Q_UNUSED(buildNumber);
    Q_UNUSED(activeConfiguration);

    if(waitingOnNumber){
        //Parse the returned data as a String

        currentOutput += outputChunk;

        bool isMatrix = _isJobAMatrixProject(jobName);

        int buildNumber = -1;

        //If Job is MultiConfiguration. Output tells us which job Number.
        if(isMatrix){
            //Started [jobName] #[buildNumber]
            //Construct a Regex Expression to match and get the Number.
            QRegularExpression regex("Started " + jobName + " #([0-9]+)");

            QRegularExpressionMatch resultMatch = regex.match(currentOutput);
            if(resultMatch.hasMatch()){
                QString match = resultMatch.captured(1);
                //If toInt fails, buildNumber will be 0
                buildNumber = match.toInt();
                if(buildNumber == 0){
                    buildNumber = -1;
                }
            }
        }
        if(buildNumber > 0){
            //Gets the Job State of the Root Job, and will call the SIGNAL to teardown the JenkinsRequest
            getJobState(jobName, buildNumber, "");
            waitingOnNumber = false;
        }
    }
}

/**
 * @brief JenkinsRequest::runGroovyScript Executes a Groovy Script(File) on the Jenkins Server and returns the data
 * @param groovyScriptPath The Absolute path to the Groovy Script file.
 */
void JenkinsRequest::runGroovyScript(QString groovyScriptPath)
{
    if(waitForValidSettings()){
        //Construct the console CLI request command
        QString command = "groovy " + groovyScriptPath;

        //Execute the Wrapped CLI Command in a process. Will produce gotLiveCLIOutput as data becomes available.
        QPair<int, QByteArray> response = runProcess(manager->getCLICommand(command));

        //Parse the returned data as a String
        QString consoleOutput = QString(response.second);

        //Call the SIGNAL to send the data back.
        emit gotGroovyScriptOutput(consoleOutput);
    }else{
        emit requestFailed();
    }
    //Call the SIGNAL to teardown the JenkinsRequest
    emit requestFinished();
}

/**
 * @brief JenkinsRequest::getJobConsoleOutput - Requests the Console Output from a job.
 * @param jobName
 * @param buildNumber
 * @param activeConfiguration
 */
void JenkinsRequest::getJobConsoleOutput(QString jobName, int buildNumber, QString activeConfiguration)
{
    if(waitForValidSettings()){
        //Store the parameters provided.
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


        //Get the current state of the job.
        JOB_STATE currentState = _getJobState(this->jobName, this->buildNumber, this->activeConfiguration);
        QString previousData = "";

        QString consoleURL = manager->getURL() + "job/" + jobName + "/" + buildNo + "/consoleText";

        //Construct a timeout timer.
        QTimer* timeOutTimer = new QTimer();
        timeOutTimer->setSingleShot(true);

        //While the job is either building or not yet existant.
        while(currentState == BUILDING || currentState == NO_JOB){


            //Construct a EventLoop which waits for the timer to finish.
            QEventLoop waitLoop;
            connect(timeOutTimer, SIGNAL(timeout()), &waitLoop, SLOT(quit()));
            connect(this, SIGNAL(unexpectedTermination()), &waitLoop, SLOT(quit()));

            timeOutTimer->start(2000);

            //Wait for something to quit the EventLoop
            waitLoop.exec();
            timeOutTimer->stop();

            //Check for termination
            if(terminated){
                //qCritical() << "Terminating the Jenkins Request!";
                break;
            }

            //Update the console data.
            if(currentState == BUILDING){
                //Get console output from wget
                QPair<int, QByteArray> response = wget(consoleURL);
                QString currentData = response.second;
                if(currentData.size() > previousData.size()){
                    int from = previousData.size();
                    //Send a signal with the live output.
                    emit gotLiveCLIOutput(this->jobName, this->buildNumber, this->activeConfiguration, currentData.mid(from));
                    previousData = currentData;
                }
            }

            //Update the current state.
            currentState = _getJobState(this->jobName, this->buildNumber, this->activeConfiguration);
        }
        //Delete the timer
        timeOutTimer->deleteLater();
        //emit a gotJobStateChange signal as the job is either BUILT or FAILED
        emit gotJobConsoleOutput(this->jobName, this->buildNumber, this->activeConfiguration, previousData);

    }else{
        emit requestFailed();
    }
    //Call the SIGNAL to teardown the JenkinsRequest
    emit requestFinished();

}


/**
 * @brief JenkinsRequest::getJobActiveConfigurations Gets the list of activeConfigurations (Nodes running the job, includes a blank node to represent the Master Job) the Jenkins job provided has. Emits the gotJobactiveConfigurations signal with the output. Emits a requestFinished signal on completion.
 * @param jobName The name of the Jenkins job
 */
void JenkinsRequest::getJobActiveConfigurations(QString jobName)
{
    //Wait for valid settings.
    if(waitForValidSettings()){

        //Store the jobName
        storeRequestParameters(jobName);

        //Get the list of configurations.
        QStringList configurationList = _getJobActiveConfigurations(jobName);

        //Call the SIGNAL to send the data back.
        emit gotJobActiveConfigurations(jobName, configurationList);
    }else{
        emit requestFailed();
    }

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
    if(waitForValidSettings()){
        QString command = "build " + jobName + " ";

        //Serialize arguments.
        foreach(Jenkins_Job_Parameter parameter, jobParameters){
            command += "-p " + parameter.name + "=" + parameter.value + " ";
        }

        storeRequestParameters(jobName);


        //Add options to pipe the output of the root job.
        command += "-s -v ";

        waitingOnNumber = true;
        currentOutput = "";

        connect(this, SIGNAL(gotLiveCLIOutput(QString,int,QString,QString)), this, SLOT(waitForJobNumber(QString, int, QString, QString)));

        //Execute the Wrapped CLI Command in a process. Will produce gotLiveCLIOutput as data becomes available.
        QPair<int, QByteArray> response = runProcess(manager->getCLICommand(command));
    }else{
         emit requestFailed();
         emit requestFinished();
    }
}

/**
 * @brief JenkinsRequest::stopJob Sends a request to (and waits) abort a Job on the Jenkins Server.
 * @param jobName The name of the Jenkins job
 * @param buildNumber The build number of the job
 * @param activeConfiguration The name of the Active Configuration (Can be empty for the root job)
 */
void JenkinsRequest::stopJob(QString jobName, int buildNumber, QString activeConfiguration)
{
    if(waitForValidSettings()){
        if(activeConfiguration != ""){
            jobName += "/" + activeConfiguration;
        }

        QString stopURL  = manager->getURL() + "job/" + jobName + "/" + QString::number(buildNumber) + "/stop";
        post(stopURL);
    }else{
        emit requestFailed();
    }
    emit requestFinished();
}

/**
 * @brief JenkinsRequest::_unexpectedTermination Called when this JenkinsRequest JenkinsManager is destroyed.
 * Sets the state of a terminating variable. and unsets the JenkinsManag.er
 */
void JenkinsRequest::_unexpectedTermination()
{
    terminated = true;
    //Try unset JenkinsManger
    manager = 0;
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
            }else if(result == "ABORTED"){
                return ABORTED;
            }else{
                return FAILED;
            }
        }
    }
    return NO_JOB;
}

