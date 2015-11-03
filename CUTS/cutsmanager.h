#ifndef CUTSMANAGER_H
#define CUTSMANAGER_H

#include <QObject>
#include <QHash>
#include <QXmlResultItems>
#include <QProcess>
#include <QQueue>

//A struct used to store variables for a QProcess
struct ProcessStruct{
    QString program;
    QStringList arguments;
    QString outputFilePath;
};


class CUTSManager: public QObject{
    Q_OBJECT
public:
    //Constructor
    CUTSManager();
    ~CUTSManager();

    //Setters
    void setMaxThreadCount(int limit);
    void setXalanJPath(QString xalanJPath);
    void setXSLTransformPath(QString XSLTransformPath);
    void setCUTSConfigScriptPath(QString configureScriptPath);
    void setScriptsPath(QString path);


signals:
    void killProcesses();
    void localDeploymentOkay();
    //Emitted by the slot queueXSLTransform
    void fileToGenerate(QString filePath);
    //Emitted by the slot executeProcess
    void fileIsGenerated(QString filePath, bool success);

    //Emitted bt the slot executeXSLGeneration
    void executedXSLGeneration(bool success, QString errorString="");
    //Emitted bt the slot executeMWCGeneration
    void executedMWCGeneration(bool success, QString errorString="");
    //Emitted bt the slot executeCPPCompilation
    void executedCPPCompilation(bool success, QString errorString="");

    //Emitted bt the slot executeCUTS
    void executedCUTS(bool success, QString errorString="");

    //Emmited by the slot executeXMETransformation
    void gotXMETransformation(bool success, QString errorString, QString path);

    //Used to send live console output from the executeMWCGeneration
    void gotLiveMWCOutput(QString output);
    //Used to send live console output from the executeCPPGeneration
    void gotLiveCPPOutput(QString output);
    void gotLiveCUTSOutput(QString output);



    void _gotLiveOutput(QString output);
private slots:

    //Generates all required artifacts for the provided graphml document.
    void executeXSLGeneration(QString graphmlPath, QString outputPath);
    //Generates the MWC workspace based on the provided mwc file.
    void executeMWCGeneration(QString mwcPath);
    //Compiles all CPP artifacts, based on the provided make file.
    void executeCPPCompilation(QString makePath);


    void executeXMETransformation(QString xmePath, QString outputFilePath);

    //Runs CUTS execution
    void executeCUTS(QString path, int executionTime=60);

    //Called once a QProcess finishes executing
    void processFinished(int code, QProcess::ExitStatus status);



private:
    void processGraphml(QString graphmlPath, QString outputPath);


private:
    bool ensureDirectory(QString dirPath);
    QString monitorProcess(QProcess* process);
    bool checkForCPPCompiler();
    bool isFileReadable(QString file);

    QString wrapGraphmlQuery(QString query);
    QString evaluateQuery2String(QXmlQuery* query, QString queryStr, QXmlItem* item=0);
    QXmlResultItems* evaluateQuery2List(QXmlQuery* query, QString queryStr, QXmlItem* item=0);



    QProcessEnvironment getEnvFromScript(QString scriptPath);

    QString getGraphmlName(QString file);
    QString getGraphmlPath(QString filePath);

    void queueComponentGeneration(QString graphmlPath, QStringList components, QString outputPath);
    void queueComponentInstanceGeneration(QString graphmlPath, QStringList componentInstances, QString outputPath);
    void queueIDLGeneration(QString graphmlPath, QStringList idls, QString outputPath);
    void queueDeploymentGeneration(QString graphmlPath, QStringList mpcFiles, QString outputPath);
    void queueHardwareGeneration(QString graphmlPath, QStringList hardwareNodes, QString outputPath);

    QString preProcessIDL(QString inputFilePath, QString outputPath);

    void queueXSLTransform(QString inputFilePath, QString outputFilePath, QString xslFilePath, QStringList parameters);

    void executeProcess(QString program, QStringList arguments, QString outputFilePath="");

    void processQueue();








    QString XSLTransformPath;
    QString xalanJPath;
    QString scriptsPath;
    QString configureScriptPath;

    QString msbuildPath;
    QString msbuildVersion;
    QString modelName;
    QString modelMiddleware;
    bool gotCPPCompiler;

    QList<QProcess*> processes;
    //A Queue used to store the Process' which need to be executed
    QQueue<ProcessStruct> queue;
    //Used to store the current number of executing Process'
    int executingProcessCount;
    int xslFailCount;
    int MAX_EXECUTING_PROCESSES;


    //A Hash to keep track of the QProcess' and their output files.
    QHash<QProcess*, QString> processHash;

    QProcessEnvironment CUTS_ENVIRONMENT;
};

#endif //CUTSMANAGER_H
