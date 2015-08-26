#include "CUTS.h"
#include <QFile>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QDir>
#include <QDebug>
#include <QPair>

#include <QFileInfo>


CUTS::CUTS(QString graphmlPath, QString xalanPath, QString transformPath)
{
    setXalanPath(xalanPath);
    setTransformPath(transformPath);
    runTransforms(graphmlPath, "/Users/dan/Desktop/Test2/");
}

CUTS::~CUTS()
{

}

void CUTS::setXalanPath(QString xalanPath)
{
    this->xalanPath = xalanPath;
}

void CUTS::setTransformPath(QString transformPath)
{
    this->transformPath = transformPath;
}

void CUTS::runTransforms(QString graphml_path, QString output_path)
{
    QDir dir(output_path);
    if (!dir.exists()) {
        qCritical() << "Making Directory";
        dir.mkpath(".");
    }

    QFile xmlFile(graphml_path);

    if (!xmlFile.exists() || !xmlFile.open(QIODevice::ReadOnly))
        return;


    outputPath = output_path;
    graphmlPath = graphml_path;


    processGraphML(graphml_path);
    qCritical() << "OUTPUT";
}

void CUTS::xslFinished(int code, QProcess::ExitStatus status)
{
    QProcess* senderProcess = dynamic_cast<QProcess*>(sender());
    if(senderProcess){
        qCritical() << processHash[senderProcess] << code;
    }
}

void CUTS::processGraphML(QString graphml_file)
{
    QFile xmlFile(graphml_file);
    if (!xmlFile.exists() || !xmlFile.open(QIODevice::ReadOnly))
        return;

    //Check for Deployed Components.
    bool allComponents = false;


    QXmlQuery* query = new QXmlQuery();
    query->bindVariable("doc", &xmlFile);

    QHash<QString, QString> keyIDs;
    QList<QPair<QString, QString> > edges;
    QHash<QString, QString> componentDefs;

    QStringList deployedComponentDefs;
    QStringList deployedComponentInstances;
    QStringList hardwareIDs;
    QStringList IDLs;

    //Get the List of Keys.
    QXmlResultItems* key_xml = getQueryList(query, "doc($doc)//gml:graphml/gml:key[@for='node']");
    QXmlItem item = key_xml->next();

    while (!item.isNull()) {
        QString ID = getQuery(query, "@id/string()", &item);
        QString keyName = getQuery(query, "@attr.name/string()", &item);

        keyIDs[keyName] = ID;
        //Next Item
        item = key_xml->next();
    }

    QXmlResultItems* edge_xml = getQueryList(query, "doc($doc)//gml:edge");
    item = edge_xml->next();

    while (!item.isNull()) {
        QString source = getQuery(query, "@source/string()", &item);
        QString target = getQuery(query, "@target/string()", &item);

        edges.append(QPair<QString, QString>(source, target));

        //Next Item
        item = edge_xml->next();
    }

    //Get the list of Hardware*
    QXmlResultItems* idl_xml = getQueryList(query, "doc($doc)//gml:node[gml:data[@key='" + keyIDs["kind"] + "' and string()='IDL']]");
    item = idl_xml->next();

    while (!item.isNull()) {
        //Get ID of the Hardware(Node|Cluster)
        QString label = getQuery(query, "gml:data[@key='" + keyIDs["label"] + "']/string()", &item);
        IDLs << label;

        //Next Item
        item = idl_xml->next();
    }


    //Get the list of Hardware*
    QXmlResultItems* hardware_xml = getQueryList(query, "doc($doc)//gml:node[gml:data[@key='" + keyIDs["kind"] + "' and starts-with(string(),'Hardware')]]");
    item = hardware_xml->next();

    while (!item.isNull()) {
        //Get ID of the Hardware(Node|Cluster)
        hardwareIDs << getQuery(query, "@id/string()", &item);;

        //Next Item
        item = hardware_xml->next();
    }

    //Get Component Definitions
    QXmlResultItems* component_xml = getQueryList(query, "doc($doc)//gml:node[gml:data[@key='" + keyIDs["kind"] + "' and string() = 'Component']]");
    item = component_xml->next();

    while (!item.isNull()){
        QString ID = getQuery(query, "@id/string()", &item);
        QString label = getQuery(query, "gml:data[@key='" + keyIDs["label"] + "']/string()", &item);
        componentDefs[ID] = label;
        item = component_xml->next();
    }

    //Get Component Instances
    QXmlResultItems* componentInstance_xml = getQueryList(query, "doc($doc)//gml:node[gml:data[@key='" + keyIDs["kind"] + "' and string() = 'ComponentInstance']]");
    item = componentInstance_xml->next();

    while (!item.isNull()){
        QString longLabel;
        QString definitionName;

        bool isDeployed = false;
        QString ID = getQuery(query, "@id/string()", &item);

        QPair<QString,QString> edge;
        foreach(edge, edges){
            if(componentDefs.keys().contains(edge.second) && edge.first == ID){
                definitionName = componentDefs[edge.second];
            }
        }

        QXmlItem parent = item;

        //Recurse up parents!
        while(!parent.isNull()){
            QString label = getQuery(query, "gml:data[@key='" + keyIDs["label"] + "']/string()", &parent);
            QString parentID = getQuery(query, "@id/string()", &parent);
            QString kind = getQuery(query, "gml:data[@key='" + keyIDs["kind"] + "']/string()", &parent);

            qCritical() << label;
            //Check if deployed.
            foreach(edge, edges){
                if(hardwareIDs.contains(edge.second) && edge.first == parentID){
                    isDeployed = true;
                }
            }

            //If kind isn't a Component or an Assembly, break loop.
            if(!kind.startsWith("Component")){
                break;
            }
            //Setup LongLabel
            if(longLabel.isEmpty()){
                longLabel = label;
            }else{
                longLabel = label + "%%" + longLabel;
            }
            //Select parent of parent.
            QXmlResultItems* parent_xml = getQueryList(query, "../..", &parent);
            parent = parent_xml->next();
        }


        //Add it to the list of used ComponentDefs
        if((allComponents || isDeployed) && !deployedComponentDefs.contains(definitionName)){
            deployedComponentDefs << definitionName;
        }
        //Add instance to the list of used ComponentInstances
        if((allComponents || isDeployed) && !deployedComponentInstances.contains(longLabel)){
            deployedComponentInstances << longLabel;
        }
        item = componentInstance_xml->next();
    }

    generateComponentArtifacts(deployedComponentDefs);
    generateComponentInstanceArtifacts(deployedComponentInstances);
    generateIDLArtifacts(IDLs);

    QStringList mpcFiles;
    //Construct a list of MPC files
    foreach(QString component, deployedComponentDefs){
        mpcFiles << component + "Impl.mpc";
    }
    foreach(QString IDL, IDLs){
        mpcFiles << IDL + ".mpc";
    }

    generateModelArtifacts(mpcFiles);
    //qCritical() << "Deployed Definitions: " << deployedComponentDefs;
    //qCritical() << "Deployed Instances: " << deployedC1omponentInstances;

    return;


}

QString CUTS::wrapQuery(QString query)
{
    return "declare namespace gml = \"http://graphml.graphdrawing.org/xmlns\"; " + query;
}

QString CUTS::getQuery(QXmlQuery* query, QString queryStr, QXmlItem* item)
{
    QString value;
    if(item && !item->isNull()){
        query->setFocus(*item);
    }
    query->setQuery(wrapQuery(queryStr));
    query->evaluateTo(&value);
    return value.trimmed();
}

QXmlResultItems *CUTS::getQueryList(QXmlQuery* query, QString queryStr, QXmlItem* item)
{
    QXmlResultItems* results = new QXmlResultItems();
    if(item && !item->isNull()){
        query->setFocus(*item);
    }
    query->setQuery(wrapQuery(queryStr));
    query->evaluateTo(results);
    return results;
}

void CUTS::generateComponentArtifacts(QStringList components)
{
    QStringList transforms;
    transforms << "mpc" << "cpp" << "h";
    foreach(QString transform, transforms){
        foreach(QString component, components){
            QStringList parameters;
            parameters << "File" << component + "Impl." + transform;
            QString outputFile = outputPath + component + "Impl." + transform;
            QString xslFile = transformPath + "graphml2" + transform + ".xsl";
            runXSLTransform(graphmlPath, outputFile, xslFile, parameters);
        }
    }
}

void CUTS::generateComponentInstanceArtifacts(QStringList componentInstances)
{
    QStringList transforms;
    transforms << "dpd";
    foreach(QString transform, transforms){
        foreach(QString componentInstance, componentInstances){
            QStringList parameters;
            parameters << "ComponentInstance" << componentInstance;
            QString outputFile = outputPath + componentInstance + "%%QoS.dpd";
            QString xslFile = transformPath + "graphml2" + transform + ".xsl";
            runXSLTransform(graphmlPath, outputFile, xslFile, parameters);
        }
    }

}

void CUTS::generateIDLArtifacts(QStringList idls)
{
    QStringList transforms;
    transforms << "idl" << "mpc";
    foreach(QString transform, transforms){
        foreach(QString idl, idls){
            QStringList parameters;
            qCritical() << idl;
            parameters << "File" <<  idl + "." + transform;
            QString outputFile = outputPath + idl + "." + transform;
            QString xslFile = transformPath + "graphml2" + transform + ".xsl";
            runXSLTransform(graphmlPath, outputFile, xslFile, parameters);
        }
    }

}

void CUTS::generateModelArtifacts(QStringList mpcFiles)
{
    QString modelName = getGraphmlName(graphmlPath);

    runXSLTransform(graphmlPath, outputPath + modelName + ".cdd", transformPath + "graphml2cdd.xsl", QStringList());
    runXSLTransform(graphmlPath, outputPath + modelName + ".cdp", transformPath + "graphml2cdp.xsl", QStringList());
    runXSLTransform(graphmlPath, outputPath + modelName + ".ddd", transformPath + "graphml2ddd.xsl", QStringList());

    QStringList mwcParams;
    mwcParams << "FileList" << "\"" + mpcFiles.join(",") + "\"";
    runXSLTransform(graphmlPath, outputPath + modelName + ".mwc", transformPath + "graphml2mwc.xsl", mwcParams);
}

void CUTS::runXSLTransform(QString inputFilePath, QString outputFilePath, QString xslFilePath, QStringList parameters)
{
    QProcess* xslProcess = new QProcess(this);
    xslProcess->setWorkingDirectory(transformPath);

    QStringList arguments;
    arguments << "-jar" << xalanPath + "xalan.jar";
    arguments << "-in" << inputFilePath;
    arguments << "-xsl" << xslFilePath;
    arguments << "-out" << outputFilePath;
    if(parameters.size() > 0){
        arguments << "-param";
        arguments += parameters;
    }
    qCritical() << arguments.join(" ");
    connect(xslProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(xslFinished(int,QProcess::ExitStatus)));


    xslProcess->start("java", arguments);

    processHash[xslProcess] = outputFilePath;
}

QString CUTS::getGraphmlName(QString file)
{
    QFileInfo fileInfo = QFileInfo(QFile(file));
    return fileInfo.baseName();
}
