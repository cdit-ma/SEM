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
JenkinsManager::JenkinsManager(QString cliPath, QString url, QString username, QString password, QString token)
{
    //Register the Types used as parameters JenkinsRequest so signals/slots can be connected.
    qRegisterMetaType<QPair<QByteArray,QByteArray> >();
    qRegisterMetaType<Jenkins_JobParameters>("Jenkins_JobParameters");
    qRegisterMetaType<JOB_STATE>("JOB_STATE");

    //Set instance variables.
    this->cliPath = cliPath;
    this->url = url;
    this->username = username;
    this->password = password;
    this->token = token;
}

bool JenkinsManager::hasValidSettings()
{
    return cliPath != "" && url != "" && username != "" && password != "" && token != "";
}

/**
 * @brief JenkinsManager::getJenkinsRequestThread Constructs a new JenkinsRequest object on it's own thread.
 * @return A JenkinsRequest object.
 */
JenkinsRequest *JenkinsManager::getJenkinsRequest(QObject *parent, bool deleteOnCompletion)
{
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


void JenkinsManager::jenkinsRequestFinished(JenkinsRequest *request)
{
   int number = requests.removeAll(request);
   // qCritical() << "Jenkins Request ID: " << number << "Terminated!";
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

/**
 * @brief JenkinsManager::getAuthenticatedRequest - Returns an Authenticated QNetworkRequest.
 * @param url - The URL to request.
 * @return - A QNetworkRequest Object which contains the token.
 */
QNetworkRequest JenkinsManager::getAuthenticatedRequest(QString url)
{
    QNetworkRequest request;
    //Set the URL
    request.setUrl(QUrl(url));

    // HTTP Basic authentication header value: base64(username:password)
    QString concatenated = username + ":" + token;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;

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
    return "java -jar jenkins-cli.jar -s " + url;
}

/**
 * @brief JenkinsManager::getCLILoginSuffix Gets the parameters for to authenticate commands on the Jenkins Server. Uses stored Username and Password.
 * @return A QString containing the command prefix.
 */
QStringList JenkinsManager::getCLILoginSuffix()
{
    QStringList returnable;
    returnable << "--username " + username;
    returnable << "--password " + password;
    return returnable;
}

/**
 * @brief JenkinsManager::getCLIPath Gets the Path which contains the jenkins-cli.jar file.
 * @return The Path (Relative OR Absolute) to the Jenkins CLI .jar file.
 */
QString JenkinsManager::getCLIPath()
{
    return cliPath;
}

/**
 * @brief JenkinsManager::getCLICommand wraps the plain CLI command with the CLIPrefix and getCLILoginSuffix so it can be executed.
 * @param cliCommand The command to issue the Jenkins CLI server.
 * @return The ready to execute terminal command.
 */
QStringList JenkinsManager::getCLICommand(QString cliCommand)
{
    QStringList returnable;
    returnable.append(getCLIPrefix() + " " +cliCommand);
    returnable.append(getCLILoginSuffix());
    qCritical() << returnable;
    return returnable;
}

