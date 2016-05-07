#include "jenkinsmanager.h"
#include "jenkinsrequest.h"

/**
 * @brief JenkinsManager::JenkinsManager JenkinsManager Constructor. Constructs a Manager to spawn JenkinsRequest threads. Handles the server information of the Jenkins Server for the request.
 * @param cliPath The Path in which the jenkins-cli.jar file originates.
 * @param url The URL for the Jenkins Server (ie. http://129.127.9.91:2488)
 * @param username The username for the Jenkins Server
 * @param password The password for the Jenkins Server
 * @todo Discover way of removing password from .ini
 */
JenkinsManager::JenkinsManager(QString cliBinaryPath)
{
    //Register the Types used as parameters JenkinsRequest so signals/slots can be connected.
    qRegisterMetaType<QPair<QByteArray,QByteArray> >();
    qRegisterMetaType<Jenkins_JobParameters>("Jenkins_JobParameters");
    qRegisterMetaType<JOB_STATE>("JOB_STATE");

    //Set instance variables.
    this->cliBinaryPath = cliBinaryPath;
    settingsValidating = false;
    urlChanged = false;
}

QString JenkinsManager::getUsername()
{
    return username;
}

/**
 * @brief JenkinsManager::setURL Sets the URL for the Jenkins Server. Devalidates the settings for the JenkinsManager upon change.
 * @param url The URL to set.
 */
void JenkinsManager::setURL(QString url)
{
    //If the url we are setting is different, update it.
    if(this->url != url){
        if(!url.endsWith("/")){
            url += "/";
        }
        this->url = url;
        urlChanged = true;
        //Devalidate the settings.
        settingsValidated = false;
    }
}

/**
 * @brief JenkinsManager::setUsername Sets the Username for the Jenkins Server. Devalidates the settings for the JenkinsManager upon change.
 * @param username The username to set.
 */
void JenkinsManager::setUsername(QString username)
{
    //If the username we are setting is different, update it.
    if(this->username != username){
        this->username = username;
        //Devalidate the settings.
        settingsValidated = false;
    }
}

/**
 * @brief JenkinsManager::setPassword Sets the Password for the Jenkins Server. Devalidates the settings for the JenkinsManager upon change.
 * @param password The Password to set.
 */
void JenkinsManager::setPassword(QString password)
{
    //If the password we are setting is different, update.
    if(this->password != password){
        this->password = password;
        //Devalidate the settings.
        settingsValidated = false;
    }
}

/**
 * @brief JenkinsManager::setPassword Sets the User Token for the Jenkins Server.
 * @param password The token to set.
 */
void JenkinsManager::setToken(QString token)
{
    //If the username we are setting is different update.
    if(this->token != token){
        this->token = token;
    }
}

void JenkinsManager::setJobName(QString jobName)
{
    if(this->jobName != jobName){
        this->jobName = jobName;
        //Devalidate the settings.
        settingsValidated = false;
    }
}

/**
 * @brief JenkinsManager::hasSettings Returns true if we have a non-empty url, username and password
 * @return Has all settings required
 */
bool JenkinsManager::hasSettings()
{
    return !(token.isEmpty() || password.isEmpty() || username.isEmpty() || url.isEmpty() || jobName.isEmpty());
}

/**
 * @brief JenkinsManager::hasValidSettings Returns whether or not the JenkinsManager has validated settings.
 * @return Valid Settings?
 */
bool JenkinsManager::hasValidatedSettings()
{
    return settingsValidated;
}

/**
 * @brief JenkinsManager::getJenkinsRequestThread Constructs and returns a new JenkinsRequest object on it's own thread.
 * If JenkinsManager doesn't have hasValidatedSettings(), JenkinsManager will call validateJenkinsSettings().
 * Which will prevent the JenkinsRequest object from querying any network information until the settings have been validated!
 * @return A JenkinsRequest object.
 */
JenkinsRequest *JenkinsManager::getJenkinsRequest(QObject *parent, bool deleteOnCompletion)
{
    if(!settingsValidated && !settingsValidating){
        validateJenkinsSettings();
    }
    //Construct and start a new QThread
    QThread* thread = new QThread();
    thread->start();

    //Construct a new JenkinsRequest, with the JenkinsManager passed as a parameter
    JenkinsRequest* request = new JenkinsRequest(this);

    //Move the JenkinsRequest to the thread
    request->moveToThread(thread);

    if(deleteOnCompletion){
        connect(request, SIGNAL(requestFinished()), request, SLOT(deleteLater()));
    }

    if(parent){
        //If the parent has been destroyed
        connect(parent, SIGNAL(destroyed()), request, SIGNAL(unexpectedTermination()));
    }

    //Connect the destruction of the thread to delete the request
    //connect(thread, SIGNAL(finished()), request, SLOT(deleteLater()));

    //Connect the destruction of the request to terminate the thread.
    connect(request, SIGNAL(destroyed()), thread, SLOT(quit()));
    //Connect the destruction of the request to delete the thread.
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));


    requests.append(request);
    //qCritical() << "Got: " << requests.size() << " Active Threads";
    return request;
}

void JenkinsManager::validateSettings()
{
    if(!settingsValidating){
        validateJenkinsSettings();
    }
}

void JenkinsManager::gotSettingsValidationResponse(bool valid, QString message)
{
    settingsValidating  = false;
    settingsValidated = valid;

    if(urlChanged && valid){
        //If our Jenkins url changed we should clear the job configs
        clearJobConfigurations();
        urlChanged = false;
    }

    //Make a Window
    //Tell Jenkins Requests that we are validated.
    emit settingsValidationComplete(valid, message);
}

/**
 * @brief JenkinsManager::validateJenkinsSettings Constructs a JenkinsRequest Thread to validate the Jenkins Settings.
 */
void JenkinsManager::validateJenkinsSettings()
{
    settingsValidating = true;

    JenkinsRequest* jR = getJenkinsRequest();
    connect(this, SIGNAL(tryValidateSettings()), jR, SLOT(validateJenkinsSettings()));
    connect(jR, SIGNAL(gotSettingsValidationResponse(bool,QString)), this, SLOT(gotSettingsValidationResponse(bool,QString)));

    emit tryValidateSettings();
    disconnect(this, SIGNAL(tryValidateSettings()), jR, SLOT(validateJenkinsSettings()));
}


void JenkinsManager::jenkinsRequestFinished(JenkinsRequest *request)
{
   requests.removeAll(request);
}

/**
 * @brief JenkinsManager::getURL Gets the Jenkins URL
 * @return the Jenkins URL.
 */
QString JenkinsManager::getURL()
{
    return url;
}

/**
 * @brief JenkinsManager::storeJobConfiguration Stores the JSON configuration provided in a hash so JenkinsRequest threads may request the data.
 * @param jobName The name of the Job for this configuration. (Can have an activeConfiguration specified ie. CUTS-Build/master=Gouda14/)
 * @param json The JSON configuration of the Jenkins Job.
 */
void JenkinsManager::storeJobConfiguration(QString jobName, QJsonDocument json)
{
    jobsJSON[jobName] = json;
}

void JenkinsManager::clearJobConfigurations()
{
    jobsJSON.clear();
}

/**
 * @brief JenkinsManager::getAuthenticatedRequest - Returns an Authenticated QNetworkRequest.
 * @param url - The URL to request.
 * @return - A QNetworkRequest Object which contains the token.
 */
QNetworkRequest JenkinsManager::getAuthenticatedRequest(QString url, bool auth)
{
    QNetworkRequest request;
    //Set the URL
    request.setUrl(QUrl(url));

    // HTTP Basic authentication header value: base64(username:password)

    QString headerData = "";
    if(auth){
        QString concatenated = username + ":" + token;
        QByteArray data = concatenated.toLocal8Bit().toBase64();
        headerData = "Basic " + data;
    }

    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
    request.setRawHeader("User-Agent", "JenkinsManager-Request");
    request.setRawHeader("Authorization", headerData.toLocal8Bit());

    return request;
}


/**
 * @brief JenkinsManager::getJobConfiguration Gets the JSON configuration for the Jenkins Job. It may return a Null QJsonDocument if there is none stored.
 * @param jobName The Name of the Job to return the JSON document for.
 * @return The JSON configuration for the Jenkins Job. Can be Null if there is no configuration stored under than jobName.
 */
QJsonDocument JenkinsManager::getJobConfiguration(QString jobName)
{    
    return jobsJSON[jobName];
}

/**
 * @brief JenkinsManager::getCLIPrefix Gets the java -jar prefix for the jenkins-cli call. Uses the stored URL.
 * @return A QString containing the command prefix.
 */
QString JenkinsManager::getCLIPrefix()
{
    return "java -jar jenkins-cli.jar -s " + url + " ";
}

/**
 * @brief JenkinsManager::getCLILoginSuffix Gets the parameters for to authenticate commands on the Jenkins Server. Uses stored Username and Password.
 * @return A QString containing the command prefix.
 */
QString JenkinsManager::getCLILoginSuffix()
{
    return " --username " + username + " --password " + password;
}

/**
 * @brief JenkinsManager::getCLIPath Gets the Path which contains the jenkins-cli.jar file.
 * @return The Path (Relative OR Absolute) to the Jenkins CLI .jar file.
 */
QString JenkinsManager::getCLIPath()
{
    return cliBinaryPath;
}


/**
 * @brief JenkinsManager::getCLICommand wraps the plain CLI command with the CLIPrefix and getCLILoginSuffix so it can be executed.
 * @param cliCommand The command to issue the Jenkins CLI server.
 * @return The ready to execute terminal command.
 */
QString JenkinsManager::getCLICommand(QString cliCommand)
{
    return getCLIPrefix() + cliCommand + getCLILoginSuffix();
}

QString JenkinsManager::getJobName()
{
    return jobName;
}

