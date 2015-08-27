#ifndef CUTS_H
#define CUTS_H

#include <QObject>
#include <QHash>
#include <QXmlResultItems>
#include <QProcess>

class CUTS: public QObject{
    Q_OBJECT

public:
    //Constructor
    CUTS(QString xalanPath, QString transformPath);
    ~CUTS();


    void setXalanPath(QString xalanPath);
    void setTransformPath(QString transformPath);

    void runTransforms(QString graphml_path, QString output_path);
signals:
    void generatingFile(QString file_path);
    void generatedFile(QString file_path, bool success);

private slots:
    void xslFinished(int code, QProcess::ExitStatus status);
    void processGraphML(QString graphml_file);
private:
    QString wrapQuery(QString query);
    QString getQuery(QXmlQuery* query, QString queryStr, QXmlItem* item=0);
    QXmlResultItems* getQueryList(QXmlQuery* query, QString queryStr, QXmlItem* item=0);

    void generateComponentArtifacts(QStringList components);
    void generateComponentInstanceArtifacts(QStringList componentInstances);
    void generateIDLArtifacts(QStringList idls);
    void generateModelArtifacts(QStringList mpcFiles);
    void generateHardwareArtifacts(QStringList hardwareNodes);

    void runXSLTransform(QString inputFilePath, QString outputFilePath, QString xslFilePath, QStringList parameters);


     QString getGraphmlName(QString file);
    /*


    void generateIDLArtifacts(QStringList idls);
    void generateModelArtifacts(QStringList mpcFiles);
    void generateDeploymentArtifacts(QStringList deployedNodes);


    QStringList getComponentInstanceLongNames(QString graphml_file);





    */


    QString transformPath;
    QString xalanPath;
    QString outputPath;
    QString graphmlPath;

    QHash<QString, QString> idHash;

    QHash<QProcess*, QString> processHash;


};

#endif //CUTS_H
