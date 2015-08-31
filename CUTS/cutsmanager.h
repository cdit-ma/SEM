#ifndef CUTSMANAGER_H
#define CUTSMANAGER_H

#include <QObject>
#include <QHash>
#include <QXmlResultItems>
#include <QProcess>
#include <QQueue>

struct ProcessStruct{
    QString program;
    QStringList arguments;
    QString outputFilePath;
};

class CUTSManager: public QObject{
    Q_OBJECT

public:
    //Constructor
    CUTSManager(QString xalanPath);
    ~CUTSManager();

    void setThreadLimit(int limit);
    void setXalanPath(QString xalanPath);
    void setTransformPath(QString transformPath);

    void runTransforms(QString graphml_path, QString output_path);
signals:
    void generatingFile(QString file_path);
    void generatedFile(QString file_path, bool success);

private slots:
    void processFinished(int code, QProcess::ExitStatus status);
    void processGraphML(QString graphml_file);
private:
    QString getGraphmlName(QString file);
    QString getGraphmlPath(QString filePath);
    QString wrapQuery(QString query);
    QString getQuery(QXmlQuery* query, QString queryStr, QXmlItem* item=0);
    bool doesFileExist(QString file);

    QXmlResultItems* getQueryList(QXmlQuery* query, QString queryStr, QXmlItem* item=0);

    void generateComponentArtifacts(QStringList components);
    void generateComponentInstanceArtifacts(QStringList componentInstances);
    void generateIDLArtifacts(QStringList idls);
    void generateModelArtifacts(QStringList mpcFiles);
    void generateHardwareArtifacts(QStringList hardwareNodes);

    QString preProcessIDL(QString inputFilePath);

    void queueXSLTransform(QString inputFilePath, QString outputFilePath, QString xslFilePath, QStringList parameters);

    void executeProcess(QString program, QStringList arguments, QString outputFilePath="");

    void processQueue();








    QString transformPath;
    QString xalanPath;
    QString outputPath;
    QString graphmlPath;

    //A Queue used to store the Process' which need to be executed
    QQueue<ProcessStruct> queue;
    //Used to store the current number of executing Process'
    int executingProcessCount;
    int MAX_EXECUTING_PROCESSES;

    //A Hash to keep track of the QProcess' and their output files.
    QHash<QProcess*, QString> processHash;


};

#endif //CUTSMANAGER_H
