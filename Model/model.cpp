#include "model.h"
#include "QDebug"
#include <QMap>

Model::Model()
{
    qDebug() << "Constructed New Model";
    this->parentGraph = new Graph("Parent Graph");

}

Model::~Model()
{

}



bool Model::importGraphML(QString inputGraphML, GraphMLContainer *parent)
{
    QXmlStreamReader xml(inputGraphML);


    QVector<GraphMLData*> nodeData;
    QVector<TempEdge> currentEdges;

    QMap<QString, GraphMLContainer *> graphMap;


    GraphMLContainer* currentParent = parent;


    GraphML::KIND nowParsing = GraphML::NONE;
    GraphML::KIND nowInside = GraphML::NONE;

    QString currentID("");
    QString previousID("");
    int parentDepth = 0;

    while(!xml.atEnd()){
        xml.readNext();

        QString tagName = xml.name().toString();

        if(tagName == "edge"){
            if(xml.isStartElement()){
                TempEdge currentEdge = parseEdge(xml);
                currentEdges.append(currentEdge);
                nowInside = GraphML::EDGE;
            }
        }else if(tagName == "data"){
            if(xml.isStartElement()){
                GraphMLData* data = parseDataAttribute(xml);

                switch(nowInside){
                    //Attach the Data to the TempEdge if we are currently inside an Edge.
                    case GraphML::EDGE:{
                        currentEdges.last().data.append(data);
                        break;
                    }

                    //Attach the Data to the list of Data to be attached to the node.
                    case GraphML::NODE:{
                        nodeData.append(data);
                    }
                }
            }
        }else if(tagName == "key"){
            if(xml.isStartElement()){
                //Parse the Attribute Definition
                GraphMLAttribute* attribute = parseKeyAttribute(xml);

                if(!this->attributeTypes.contains(attribute)){
                    attributeTypes.append(attribute);
                }
            }
        }else if(tagName == "node"){
            if(xml.isStartElement()){
                nowInside = GraphML::NODE;

                //Get the ID of the new Node
                currentID = getAttribute(xml, "id");

                nowParsing = GraphML::NODE;
            }
            if(xml.isEndElement()){
                nowInside = GraphML::NONE;
                parentDepth++;
            }
        }else{

            if(xml.isEndDocument()){
                nowParsing = GraphML::NODE;
            }else{
                nowParsing = GraphML::NONE;
            }
        }

        if(nowParsing == GraphML::NODE){
            if(previousID != ""){
                GraphMLContainer* newNode = new HardwareNode(previousID);

                qDebug () << "DATA SIZE: " << nodeData.size();
                newNode->attachData(nodeData);
                nodeData.clear();
                qDebug() << "Adopting Parent";
                //Adopt
                if(currentParent->isAdoptLegal(newNode)){
                    currentParent->adopt(newNode);
                }

                qDebug() << "Parent Depth : " <<QString::number(parentDepth);


                if(parentDepth == 0){
                    currentParent = newNode;
                }else{
                    while(parentDepth > 1){
                        parentDepth --;
                        currentParent = currentParent->getParent();
                    }
                }
                parentDepth = 0;

                graphMap.insert(previousID, newNode);
            }

            previousID = currentID;

        }
    }

    if (xml.hasError()) {
    }


    for(int i =0; i<currentEdges.size(); i++){
        TempEdge edge = currentEdges[i];
        qDebug() << edge.source << "<->" << edge.target;
        GraphMLContainer* s = graphMap[edge.source];
        GraphMLContainer* d = graphMap[edge.target];

        //qDebug() << "GG";
        //qDebug() <<"Edge " << s->toString() << "<->" << d->toString();

       // if(s->isEdgeLegal(d)){
             Edge* newEdge = new Edge(s,d);
        // }
    }


    this->output = output;

    //DO MAGIC!
    return false;
}


QString Model::exportGraphML()
{

    QString returnable = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    returnable +="<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns\">\n";

    for(int i=0; i < this->attributeTypes.size();i++){
        returnable += this->attributeTypes[i]->toGraphML(1);
    }
    returnable += this->parentGraph->toGraphML(1);

    returnable +="</graphml>\n";

    return returnable;

}

Graph *Model::getGraph()
{
    return this->parentGraph;
}

GraphMLAttribute*  Model::parseKeyAttribute(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();

    QString name;
    QString id;
    QString typeStr;
    QString forStr;


    if(attributes.hasAttribute("id")){
        id = attributes.value("id").toString();
    }else{
        qDebug() << "Attribute must contain id";
    }

    if(attributes.hasAttribute("attr.name")){
        name = attributes.value("attr.name").toString();
    }else{
        qDebug() << "Attribute must contain attr.name";
    }

    if(attributes.hasAttribute("attr.type")){
        typeStr = attributes.value("attr.type").toString();

    }else{
        qDebug() << "Attribute must contain attr.type";
    }

    if(attributes.hasAttribute("for")){
        forStr = attributes.value("for").toString();

    }else{
        qDebug() << "Attribute must contain for";
    }

    GraphMLAttribute *attribute = new GraphMLAttribute(id, name, typeStr, forStr);
    return attribute;
}

GraphMLData* Model::parseDataAttribute(QXmlStreamReader &xml)
{
    QString key = getAttribute(xml, "key");
    QString value = xml.readElementText();

    GraphMLAttribute *attr = this->getGraphMLAttribute(key);

    GraphMLData *data = new GraphMLData(attr, value);
    qDebug() << data->getType()->getName() <<":"<<data->getValue();
    return data;
}

QString Model::getAttribute(QXmlStreamReader &xml, QString attrID)
{
    QXmlStreamAttributes attributes = xml.attributes();

    if(attributes.hasAttribute(attrID)){
        return attributes.value(attrID).toString();
    }else{

        qDebug() << "<data> must contain Attribute key";
        return "";
    }

}

void Model::parseGraph(QXmlStreamReader &xml)
{

}

void Model::parseNode(QXmlStreamReader &xml)
{

}

TempEdge Model::parseEdge(QXmlStreamReader &xml)
{
    TempEdge tE;
    tE.id = getAttribute(xml, "id");
    tE.source = getAttribute(xml, "source");
    tE.target = getAttribute(xml, "target");
    return tE;
}

GraphMLAttribute *Model::getGraphMLAttribute(QString key)
{
    for(int i=0;i<this->attributeTypes.size();i++){
        if(this->attributeTypes[i]->getID() == key){
            return this->attributeTypes[i];
        }
    }
    return 0;
}
