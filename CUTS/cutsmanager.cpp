#include "cutsmanager.h"
#include <QFile>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QDir>
#include <QDebug>
#include <QPair>
#include <QProcess>
#include <QEventLoop>
#include <QFileInfo>
#include <QRegularExpressionMatchIterator>
#include <QMessageBox>
#include <QTime>
CUTSManager::CUTSManager()
{
    //Initialize starting variables;
    executingProcessCount = 0;
    xslFailCount = 0;
    MAX_EXECUTING_PROCESSES = 1;
}

CUTSManager::~CUTSManager()
{
    //Destructor
}

void CUTSManager::setXalanJPath(QString xalanPath)
{
    //Enforce a trailing /
    if(!xalanPath.endsWith("/")){
        xalanPath += "/";
    }
    this->xalanJPath = xalanPath;
}

void CUTSManager::setXSLTransformPath(QString transformPath)
{
    //Enforce a trailing /
    if(!transformPath.endsWith("/")){
        transformPath += "/";
    }
    this->XSLTransformPath = transformPath;

}

void CUTSManager::setCUTSConfigScriptPath(QString configureScriptPath)
{
    this->configureScriptPath = configureScriptPath;
}

void CUTSManager::setScriptsPath(QString path)
{
    scriptsPath = path;
}

///
/// \brief CUTSManager::executeXSLGeneration Generates all required code artifacts for the execution of the graphml document.
/// \param graphmlPath The path to the graphml document to generate code for.
/// \param outputPath The path to the place the built files into.
///
void CUTSManager::executeXSLGeneration(QString graphmlPath, QString outputPath)
{
    checkForVisualStudio();
    //Ensure output directory exists.
    if(!ensureDirectory(outputPath)){
        emit executedXSLGeneration(false, "Cannot construct Output directory.");
    }

    //Ensure graphml file exists
    if(!isFileReadable(graphmlPath)){
        emit executedXSLGeneration(false, "File: '" + graphmlPath + "'' either doesn't exist or isn't readable!");
        return;
    }

    //Run generation.
    processGraphml(graphmlPath, outputPath);
}

void CUTSManager::executeMWCGeneration(QString mwcPath)
{

    //get the environment for the mwc generation
    QProcessEnvironment configuredEnv = getEnvFromScript(configureScriptPath);

    QString ACE_ROOT = configuredEnv.value("ACE_ROOT");
    QString CUTS_ROOT = configuredEnv.value("CUTS_ROOT");


    QString program = "perl";
    QStringList args;
    QString type;
    QString mwcFilePath = mwcPath + "HelloWorld.mwc";

    //Check if windows or *nix
    #ifdef _WIN32
        type = "vc" + msbuildVersion;
    #else
        type = "gnuace";
    #endif

    args << ACE_ROOT + "/bin/mwc.pl";
    args << "-type" << type << "-feature_file" << CUTS_ROOT + "/default.features" << mwcFilePath;

    QProcess* process = new QProcess(this);
    process->setProcessEnvironment(configuredEnv);

    //Run MWC Generation
    qCritical() << program << args;
    connect(this, SIGNAL(_gotLiveOutput(QString)), this, SIGNAL(gotLiveMWCOutput(QString)));

    process->start(program, args);

    QString output = monitorProcess(process);
    emit gotLiveMWCOutput(" FINAL: \n" + output);
    disconnect(this, SIGNAL(_gotLiveOutput(QString)), this, SIGNAL(gotLiveMWCOutput(QString)));

    emit executedMWCGeneration(true);
}

void CUTSManager::executeCPPCompilation(QString makePath)
{
    //get the environment for the mwc generation
    QProcessEnvironment configuredEnv = getEnvFromScript(configureScriptPath);


    QString CUTS_ROOT = configuredEnv.value("CUTS_ROOT");

    QString program;
    QStringList args;

    //Check if windows or *nix
    #ifdef _WIN32
        program = msbuildPath;

        if(MAX_EXECUTING_PROCESSES > 1){
            //Multithread
            args << "/m";
            args << "/verbosity:minimal";
        }
        args << makePath + "HelloWorld.sln";
    #else
        program = "make";

        if(MAX_EXECUTING_PROCESSES > 1){
            //Multithread
            args << "-j" << QString::number(MAX_EXECUTING_PROCESSES);
        }

        QString slnPath;
        args << "-C" << makePath << "`cat \"" + CUTS_ROOT + "/default.features\"`";
    #endif


    QProcess* process = new QProcess(this);
    process->setProcessEnvironment(configuredEnv);


    //emit gotLiveCPPOutput("Starting: " + program);
    process->start(program, args);

    connect(this, SIGNAL(_gotLiveOutput(QString)), this, SIGNAL(gotLiveCPPOutput(QString)));

    QString output = monitorProcess(process);

    emit gotLiveCPPOutput(" FINAL: \n" + output);
    disconnect(this, SIGNAL(_gotLiveOutput(QString)), this, SIGNAL(gotLiveCPPOutput(QString)));




    emit executedCPPCompilation(true);

}

void CUTSManager::executeCUTS(QString graphmlPath, int executionTime)
{
    //get the environment for the mwc generation
    QProcessEnvironment configuredEnv = getEnvFromScript(configureScriptPath);

    QProcess* process = new QProcess(this);
    process->setProcessEnvironment(configuredEnv);

    QString path = getGraphmlPath(graphmlPath);
    QString modelName = getGraphmlName(graphmlPath);




    QString program = "perl";
    QStringList args;
    args << scriptsPath + "runCuts.pl";

    emit gotLiveCUTSOutput("Starting: " + program + " " + args.join(" "));
    process->setWorkingDirectory(path);
    process->start(program, args);

    connect(this, SIGNAL(_gotLiveOutput(QString)), this, SIGNAL(gotLiveCUTSOutput(QString)));

    QString output = monitorProcess(process);

    emit gotLiveCUTSOutput(" FINAL: \n" + output);
    disconnect(this, SIGNAL(_gotLiveOutput(QString)), this, SIGNAL(gotLiveCUTSOutput(QString)));




    emit executedCUTS(true);




}

void CUTSManager::processGraphml(QString graphmlPath, QString outputPath)
{
    //Run preprocess generation on the graphml, this will be used as the input to all transforms.
    QString processedGraphmlPath = preProcessIDL(graphmlPath, outputPath);

    if(!isFileReadable(processedGraphmlPath)){
        emit executedXSLGeneration(false, "Preprocessing graphml document failed!");
        return;
    }

    QFile graphmlFile(processedGraphmlPath);
    if(!graphmlFile.open(QIODevice::ReadOnly)){
        emit executedXSLGeneration(false, "Cannot read Preprocessed graphml document!");
        return;
    }

    //Construct a QXmlQuery object to inspect the graphml.
    QXmlQuery* query = new QXmlQuery();
    //Bind the variable doc, to the path of the graphml file.
    query->bindVariable("doc", &graphmlFile);


    QHash<QString, QString> keys;
    QList<QPair<QString, QString> > edges;
    QHash<QString, QString> components;

    QStringList deployedComponents;
    QStringList deployedComponentInstances;
    QStringList hardwareIDs;
    QStringList IDLs;

    //Generates code for all components regardless of whether they are deployed.
    bool generateAllComponents = true;


    //Get the id and attr.name attributes of each <key> entities from the graphml
    QXmlResultItems* keysXML = evaluateQuery2List(query, "doc($doc)//gml:graphml/gml:key[@for='node']");
    QXmlItem keyXML = keysXML->next();

    while(!keyXML.isNull()){
        QString ID = evaluateQuery2String(query, "@id/string()", &keyXML);
        QString name = evaluateQuery2String(query, "@attr.name/string()", &keyXML);
        keys[name] = ID;
        keyXML = keysXML->next();
    }

    //Get the ID's of the source/target of each <edge> entities from the graphml
    QXmlResultItems* edgesXML = evaluateQuery2List(query, "doc($doc)//gml:edge");
    QXmlItem edgeXML = edgesXML->next();

    while(!edgeXML.isNull()){
        QString sourceID = evaluateQuery2String(query, "@source/string()", &edgeXML);
        QString targetID = evaluateQuery2String(query, "@target/string()", &edgeXML);
        QPair<QString, QString> edge(sourceID, targetID);
        edges << edge;
        edgeXML = edgesXML->next();
    }

    //Get the label of each <node> entity of kind "IDL"
    QXmlResultItems* idlsXML = evaluateQuery2List(query, "doc($doc)//gml:node[gml:data[@key='" + keys["kind"] + "' and string()='IDL']]");
    QXmlItem idlXML = idlsXML->next();

    while(!idlXML.isNull()){
        QString idl = evaluateQuery2String(query, "gml:data[@key='" + keys["label"] + "']/string()", &idlXML);
        IDLs << idl;
        idlXML = idlsXML->next();
    }

    //Get the ID's of each <node> entity of kind "Hardware*" (HardwareNode/HardwareCluster)
    QXmlResultItems* hardwaresXML = evaluateQuery2List(query, "doc($doc)//gml:node[gml:data[@key='" + keys["kind"] + "' and starts-with(string(),'Hardware')]]");
    QXmlItem hardwareXML = hardwaresXML->next();

    while(!hardwareXML.isNull()){
        QString ID = evaluateQuery2String(query, "@id/string()", &hardwareXML);
        hardwareIDs << ID;
        hardwareXML = hardwaresXML->next();
    }

    //Get the ID and label of each <node> entity of kind "Component"
    QXmlResultItems* componentsXML = evaluateQuery2List(query, "doc($doc)//gml:node[gml:data[@key='" + keys["kind"] + "' and string() = 'Component']]");
    QXmlItem componentXML = componentsXML->next();

    while(!componentXML.isNull()){
        QString ID = evaluateQuery2String(query, "@id/string()", &componentXML);
        QString label = evaluateQuery2String(query, "gml:data[@key='" + keys["label"] + "']/string()", &componentXML);
        components[ID] = label;
        componentXML = componentsXML->next();
    }

    //Get the ID and label of each <node> entity of kind "ComponentInstance"
    QXmlResultItems* componentInstancesXML = evaluateQuery2List(query, "doc($doc)//gml:node[gml:data[@key='" + keys["kind"] + "' and string() = 'ComponentInstance']]");
    QXmlItem componentInstanceXML = componentInstancesXML->next();

    while(!componentInstanceXML.isNull()){
        QString longLabel;
        QString definitionLabel;
        bool isDeployed = false;

        QString instanceID = evaluateQuery2String(query, "@id/string()", &componentInstanceXML);

        //Find the connected Component definition for this instance.
        QPair<QString,QString> edge;
        foreach(edge, edges){
            if(edge.first == instanceID && components.keys().contains(edge.second)){
                //Got match.
                definitionLabel = components[edge.second];
                break;
            }
        }

        //Recurse up parent.
        QXmlItem parent = componentInstanceXML;

        //Get information about parent, deployment and label.
        while(!parent.isNull()){
            QString label = evaluateQuery2String(query, "gml:data[@key='" + keys["label"] + "']/string()", &parent);
            QString ID = evaluateQuery2String(query, "@id/string()", &parent);
            QString kind = evaluateQuery2String(query, "gml:data[@key='" + keys["kind"] + "']/string()", &parent);

            //If the kind isn't a Component or ComponentAssembly, break loop.
            if(!kind.startsWith("Component")){
                break;
            }

            //Check if parent is deployed.
            foreach(edge, edges){
                if(edge.first == ID && hardwareIDs.contains(edge.second)){
                    isDeployed = true;
                    break;
                }
            }

            //Construct the long label of the ComponentInstance
            if(longLabel.isEmpty()){
                longLabel += label;
            }else{
                longLabel = label + "%%" + longLabel;
            }

            //Select parent
            QXmlResultItems* parentXML = evaluateQuery2List(query, "../..", &parent);
            parent = parentXML->next();
        }

        //If this ComponentInstance is deployed, or we are generating all Component Code.
        if(generateAllComponents || isDeployed){
            //Append the Component definition, if it's not been seen before.
            if(!deployedComponents.contains(definitionLabel)){
                deployedComponents << definitionLabel;
            }

            //Append the Long label of the Component Instance, if it's not been seen before.
            if(!deployedComponentInstances.contains(longLabel)){
                deployedComponentInstances << longLabel;
            }
        }

        componentInstanceXML = componentInstancesXML->next();
    }

    //Close the graphmlFile.
    graphmlFile.close();

    QStringList mpcFiles;

    //Foreach Component, we expect one Impl.mpc file.
    foreach(QString component, deployedComponents){
        mpcFiles << component + "Impl.mpc";
    }

    //Foreach IDL, we expect one .mpc file.
    foreach(QString IDL, IDLs){
        mpcFiles << IDL + ".mpc";
    }

    QStringList hardwareNodes;
    hardwareNodes << "localhost";

    //Check queue length.
    if(queue.size() != 0){
        qCritical() << "Clearing Queue";
        queue.clear();
    }

    queueComponentGeneration(processedGraphmlPath, deployedComponents, outputPath);
    queueComponentInstanceGeneration(processedGraphmlPath,deployedComponentInstances, outputPath);
    queueIDLGeneration(processedGraphmlPath,IDLs, outputPath);
    queueDeploymentGeneration(processedGraphmlPath, mpcFiles, outputPath);
    queueHardwareGeneration(processedGraphmlPath, hardwareNodes, outputPath);

    //Start Queue
    processQueue();
}

bool CUTSManager::ensureDirectory(QString dirPath)
{
    QDir dir(dirPath);
    if(!dir.exists()){
        //Construct directory.
        if(!dir.mkpath(".")){
            return false;
        }
    }
    return true;
}

QString CUTSManager::monitorProcess(QProcess *process)
{
    //The returnable byteArray.
    QString returnable;


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

            //Wait for something to quit the EventLoop
            waitLoop.exec();

            //After the readyRead signal, or timeout, update the bytesAvailable
            bytesAvailable = process->bytesAvailable();
        }

        //If we have bytes, read them, and output them!
        if(bytesAvailable > 0){
            QByteArray newData = process->read(bytesAvailable);

            QString stringData(newData);
            if(stringData.length() > 0){
                //Send a signal with the live output.
                emit _gotLiveOutput(stringData);
            }

            //Add them to the returnable data.
            returnable += stringData;
        }

        //If the process is at the end of the buffer and the Process is no longer Running. Break the loop.
        if(process->atEnd() && process->state() == QProcess::NotRunning){
            //qCritical() << "Process is finished!";
            processing = false;
        }
    }
    return returnable;
}

void CUTSManager::checkForVisualStudio()
{
    QProcessEnvironment systemEnvironment = QProcessEnvironment::systemEnvironment();
    QString vsToolsPath;
    QString vsVersion;

    //VS100COMNTOOLS
    QRegularExpression regex("VS([0-9]*)0COMNTOOLS");
    foreach(QString key, systemEnvironment.keys()){
        QRegularExpressionMatch match = regex.match(key);
        if(match.hasMatch()){
            qCritical() << match.captured(1);
            vsVersion = match.captured(2);
            QString value = systemEnvironment.value(key);
            vsToolsPath = value;
        }
    }
    qCritical() << vsVersion;
    if(vsToolsPath == ""){
        QMessageBox::critical(0, "Cannot find Visual Studio", "Visual Studio cannot be found!", QMessageBox::Ok);
        return;
    }

    QProcessEnvironment vsEnv = getEnvFromScript("\"" + vsToolsPath+"VCVarsQueryRegistry.bat\" 32bit No64bit");


    QString dotNetFrameworkPath = vsEnv.value("FRAMEWORKDIR32");
    QString dotNetFrameworkVersion = vsEnv.value("FRAMEWORKVERSION32");


    msbuildPath = dotNetFrameworkPath + dotNetFrameworkVersion + "/msbuild.exe";
    msbuildVersion = vsVersion;

}

void CUTSManager::setMaxThreadCount(int limit){
    MAX_EXECUTING_PROCESSES = limit;
}


void CUTSManager::processFinished(int code, QProcess::ExitStatus)
{
    QProcess* process = dynamic_cast<QProcess*>(sender());
    if(process){
        //Get the Process, so we can find the matching output file path.
        QString outputFilePath = processHash[process];

        if(code != 0){
            xslFailCount ++;
        }
        //Emit a Signal saying that the file has been produced.
        emit fileIsGenerated(outputFilePath, code == 0);
    }
    //Decrement the currentRunningCount
    executingProcessCount--;

    //Move onto the next items in the Queue.
    processQueue();
}


/**
 * @brief CUTSManager::getEnvFromScript Executes a .bat/.sh script and constructs a QProcessEnvironment to put the newly set environment variables into.
 * @param scriptPath The path to the script file.
 * @return
 */
QProcessEnvironment CUTSManager::getEnvFromScript(QString scriptPath)
{
    QProcess* process = new QProcess(this);


    QString program;
    //Check if windows or *nix
    QString command;
    #ifdef _WIN32
        program = "cmd.exe";
        command = scriptPath + "&set&exit\n";
    #else
        program = "/bin/sh";
        //Have to pass a path to the .sh script
        command = ". " + scriptPath + ";set;exit\n";
    #endif

    process->start(program);
    process->waitForStarted();

    //Run the command
    process->write(command.toUtf8());

    //Wait for the process to finish.
    process->waitForFinished(2000);

    //Get the output.
    QString commandOutput = process->readAllStandardOutput();

    //Compile Regex.
    QRegularExpression regex("([^=]*)=(.*)");

    QProcessEnvironment newEnvironment;
    //Ignore oldEnvironment
    //QProcessEnvironment systemEnvironment = QProcessEnvironment::systemEnvironment();

      foreach(QString line, commandOutput.split('\n')){
        QRegularExpressionMatch match = regex.match(line);
        if(match.hasMatch()){
            QString key = match.captured(1).trimmed();
            QString value = match.captured(2).trimmed();
            if(key.length() > 0){
                //Ignore the system environment keys?
                //if(!systemEnvironment.contains(key)){
                    newEnvironment.insert(key, value);
                //}
            }
       }
    }
    return newEnvironment;
}


/**
 * @brief CUTSManager::wrapGraphmlQuery Prepends the namespace definition for the graphml doctype to a query.
 * @param query the query to execute
 * @return the query with namespace definition attached.
 */
QString CUTSManager::wrapGraphmlQuery(QString query)
{
    return "declare namespace gml = \"http://graphml.graphdrawing.org/xmlns\"; " + query;
}

/**
 * @brief CUTSManager::evaluateQuery2String Runs an XPath query on an XML document, which returns a String
 * @param query The root query document.
 * @param queryStr The Stringd XPath query.
 * @param item the item to query (or null)
 * @return A string containing the result of the query.
 */
QString CUTSManager::evaluateQuery2String(QXmlQuery* query, QString queryStr, QXmlItem* item)
{
    QString value;
    if(item && !item->isNull()){
        query->setFocus(*item);
    }
    query->setQuery(wrapGraphmlQuery(queryStr));
    query->evaluateTo(&value);
    return value.trimmed();
}

/**
 * @brief CUTSManager::isFileReadable Checks to see if a file exists, is non-empty and is readable.
 * @param filePath The path to the file to check
 * @return
 */
bool CUTSManager::isFileReadable(QString filePath)
{
    QFile file(filePath);
    QFileInfo fileInfo = QFileInfo(file);
    return fileInfo.isFile() && fileInfo.size() > 0 && fileInfo.isReadable();
}

/**
 * @brief CUTSManager::runQuery2List Runs an XPath query on an XML document, which returns a list of QXmlItems
 * @param query The root query document.
 * @param queryStr The Stringd XPath Query
 * @param item The item to query (or null)
 * @return a QXmlResultItem list containing the results of the query.
 */
QXmlResultItems *CUTSManager::evaluateQuery2List(QXmlQuery* query, QString queryStr, QXmlItem* item)
{
    //Construct a results List.
    QXmlResultItems* results = new QXmlResultItems();

    //If we have been passed an QXmlItem to focus on, use it.
    if(item && !item->isNull()){
        query->setFocus(*item);
    }
    //run and evaluate query.
    query->setQuery(wrapGraphmlQuery(queryStr));
    query->evaluateTo(results);
    return results;
}

/**
 * @brief CUTSManager::generateComponentArtifacts Queues the XSL transforms required to build the *.mpc, *.h and *.cpp files for each Component
 * @param components The list of Components
 * @param outputPath  The output path for the generated files.
 */
void CUTSManager::queueComponentGeneration(QString graphmlPath, QStringList components, QString outputPath)
{
    QStringList transformsExts;
    transformsExts << "mpc" << "cpp" << "h";
    foreach(QString transformExt, transformsExts){
        foreach(QString component, components){
            //Construct the parameters
            QStringList parameters;
            parameters << "File" << component + "Impl." + transformExt;
            QString outputFile = outputPath + component + "Impl." + transformExt;
            QString xslFile = XSLTransformPath + "graphml2" + transformExt + ".xsl";

            //Queue the XSL Transform
            queueXSLTransform(graphmlPath, outputFile, xslFile, parameters);
        }
    }
}

/**
 * @brief CUTSManager::queueComponentInstanceGeneration Queues the XSL transforms required to build the *.dpd files for each ComponentInstance
 * @param componentInstances The list of ComponentInstances
 * @param outputPath The output path for the generated files.
 */
void CUTSManager::queueComponentInstanceGeneration(QString graphmlPath, QStringList componentInstances, QString outputPath)
{
    foreach(QString componentInstance, componentInstances){
        //Construct the parameters
        QStringList parameters;
        parameters << "ComponentInstance" << componentInstance;
        QString outputFile = outputPath + componentInstance + "%%QoS.dpd";
        QString xslFile = XSLTransformPath + "graphml2dpd.xsl";

        //Queue the XSL Transform
        queueXSLTransform(graphmlPath, outputFile, xslFile, parameters);
    }
}

/**
 * @brief CUTSManager::queueIDLGeneration Queues the XSL transforms required to build the *.idl & *.mpc files for each IDL
 * @param idls The list of IDLs
 * @param outputPath The output path for the generated files.
 */
void CUTSManager::queueIDLGeneration(QString graphmlPath, QStringList idls, QString outputPath)
{
    QStringList transformsExts;
    transformsExts << "idl" << "mpc";
    foreach(QString transformsExt, transformsExts){
        foreach(QString idl, idls){
            //Construct the parameters
            QStringList parameters;
            parameters << "File" <<  idl + "." + transformsExt;
            QString outputFile = outputPath + idl + "." + transformsExt;
            QString xslFile = XSLTransformPath + "graphml2" + transformsExt + ".xsl";

            //Queue the XSL Transform
            queueXSLTransform(graphmlPath, outputFile, xslFile, parameters);
        }
    }
}

/**
 * @brief CUTSManager::queueDeploymentGeneration Queues the XSL transforms required to build the *.mwc, *.cdd, *.cdp & *.ddd files for the Model
 * @param outputPath The output path for the generated files.
 */
void CUTSManager::queueDeploymentGeneration(QString graphmlPath, QStringList mpcFiles, QString outputPath)
{
    QString modelName = getGraphmlName(graphmlPath);

    QStringList mwcParams;
    mwcParams << "FileList";
    mwcParams << mpcFiles.join(",");

    QStringList transformsExts;
    transformsExts << "cdd" << "cdp" << "ddd";

    QString descriptorPath = outputPath + "descriptors/";

    if(!ensureDirectory(descriptorPath)){
        qCritical() << "Can't make directory!";
    }

    foreach(QString transformExt, transformsExts){
        queueXSLTransform(graphmlPath, descriptorPath + modelName + "." + transformExt, XSLTransformPath + "graphml2" + transformExt + ".xsl", QStringList());
    }

    queueXSLTransform(graphmlPath, outputPath + modelName + ".mwc", XSLTransformPath + "graphml2mwc.xsl", mwcParams);
}

/**
 * @brief CUTSManager::queueHardwareGeneration
 * @param hardwareNodes The list of HardwareNodes to generate machine configs
 * @param outputPath The output path for the generated files.
 */
void CUTSManager::queueHardwareGeneration(QString graphmlPath, QStringList hardwareNodes, QString outputPath)
{
    //TODO
}

QString CUTSManager::preProcessIDL(QString inputFilePath, QString outputPath)
{
    //Start a QProcess for this program
    QProcess* process = new QProcess(this);
    process->setWorkingDirectory(XSLTransformPath);

    QString outFileName = outputPath + getGraphmlName(inputFilePath) + ".graphml";

    //Construct the arguments for the xsl transform
    QStringList arguments;
    arguments << "-jar" << xalanJPath + "xalan.jar";
    arguments << "-in" << inputFilePath;
    arguments << "-xsl" << XSLTransformPath + "PreprocessIDL.xsl";
    arguments << "-out" << outFileName;

    //Construct a wait loop to make sure this transform happens first.
    QEventLoop waitLoop;
    connect(process, SIGNAL(finished(int)), &waitLoop, SLOT(quit()));

    //Execute the QProcess
    process->start("java", arguments);

    //Wait for The process to exit the loop.
    waitLoop.exec();

    //Return the filepath of the new Graphml file.
    return outFileName;
}

void CUTSManager::queueXSLTransform(QString inputFilePath, QString outputFilePath, QString xslFilePath, QStringList parameters)
{
    QStringList arguments;
    arguments << "-jar" << xalanJPath + "xalan.jar";
    arguments << "-in" << inputFilePath;
    arguments << "-xsl" << xslFilePath;
    arguments << "-out" << outputFilePath;
    if(parameters.size() > 0){
        arguments << "-param";
        arguments += parameters;
    }

    //Emit that we are to Generate this file.
    emit fileToGenerate(outputFilePath);

    //Construct and fill a Struct to contain the parameters for the spawned Process.
    ProcessStruct ps;
    ps.arguments = arguments;
    ps.outputFilePath = outputFilePath;
    ps.program = "java";
    queue.append(ps);
}

/**
 * @brief CUTS::processQueue Ensures that there are MAX_EXECUTING_PROCESSES number of QProcesses currently executing, until there is nothing left in the queue to execute.
 */
void CUTSManager::processQueue()
{
    //While there isn't enough processes currently executing
    while(executingProcessCount <  MAX_EXECUTING_PROCESSES){
        if(queue.isEmpty()){
            //If we are on our last process.
            if(executingProcessCount == 0){
                QString errorString;
                if(xslFailCount > 0){
                    errorString = QString::number(xslFailCount) + " XSL generations failed!";
                }
                emit executedMWCGeneration(xslFailCount > 0, errorString);
            }
            //AT END
            //Queue is empty, so don't continue;
            break;
        }
        //Enqueue next process.
        ProcessStruct ps = queue.takeFirst();
        executeProcess(ps.program, ps.arguments, ps.outputFilePath);
    }
}

/**
 * @brief CUTS::executeProcess Executes a Program, with Arguments.
 * @param program - The Program to execute
 * @param arguments - The list of arguments for the program.
 * @param outputFilePath - The desired output filepath for the file.
 */
void CUTSManager::executeProcess(QString program, QStringList arguments, QString outputFilePath)
{
    //Start a QProcess for this program
    QProcess* process = new QProcess(this);
    process->setWorkingDirectory(XSLTransformPath);

    //Connect the Process' finished Signal
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));

    //Execute the QProcess
    process->start(program, arguments);

    //Store the Process in the hash so we can workout when all files are finished
    processHash[process] = outputFilePath;
    //Increment the number or currently running Process
    executingProcessCount++;
}

QString CUTSManager::getGraphmlName(QString filePath)
{
    QFile file(filePath);
    QFileInfo fileInfo = QFileInfo(file);
    return fileInfo.baseName();
}

QString CUTSManager::getGraphmlPath(QString filePath)
{
    QFile file(filePath);
    QFileInfo fileInfo = QFileInfo(file);
    return fileInfo.absolutePath();
}
