#ifndef CUTS_H
#define CUTS_H

#include <QObject>
#include <QHash>
#include <QXmlResultItems>

class CUTS: public QObject{
    Q_OBJECT

public:
    //Constructor
    CUTS(QString graphmlPath, QString xalanPath, QString transformPath);
    ~CUTS();


    void setXalanPath(QString xalanPath);
    void setTransformPath(QString transformPath);

    void runTransforms(QString graphml_path, QString output_path);

private slots:
    void processGraphML(QString graphml_file);
private:
    QString wrapQuery(QString query);
    QString getQuery(QXmlQuery* query, QString queryStr, QXmlItem* item=0);
    QXmlResultItems* getQueryList(QXmlQuery* query, QString queryStr, QXmlItem* item=0);

    void generateComponentArtifacts(QStringList components);
    void generateComponentInstanceArtifacts(QStringList componentInstances);

    void runXSLTransform(QString inputFilePath, QString outputFilePath, QString xslFilePath, QStringList parameters);

    /*


    void generateIDLArtifacts(QStringList idls);
    void generateModelArtifacts(QStringList mpcFiles);
    void generateDeploymentArtifacts(QStringList deployedNodes);


    QStringList getComponentInstanceLongNames(QString graphml_file);



    QString getGraphmlName(QString file);

    */


    QString transformPath;
    QString xalanPath;
    QString outputPath;
    QString graphmlPath;

    QHash<QString, QString> idHash;


};

#endif //CUTS_H
