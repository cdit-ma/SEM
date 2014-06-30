#include "model.h"
#include "QDebug"
Model::Model()
{
    qDebug() << "Constructed new Model";
    this->parentGraph = new Graph("Parent Graph");
}

Model::~Model()
{

}



bool Model::importGraphML(QString inputGraphML, GraphML *parent)
{

    //qDebug()<<inputGraphML;
    QXmlStreamReader xml(inputGraphML);


    int level=0;
    QString output("");
    parent =  this->getGraph();

    GraphML::KIND kind;
    bool gotBool = false;
    QString ID("");
    while (!xml.atEnd()) {
        xml.readNext();

        if(xml.isStartElement()){
            if(gotBool){
             //Construct Parent?!
                if( kind == GraphML::GRAPH){
                    Graph *newGraph = new Graph(ID);
                    //Attach it
                    parent->adopt(newGraph);
                    parent = newGraph;

                }else if(kind == GraphML::NODE){
                    HardwareNode *newNode = new HardwareNode(ID);
                    //Attach it
                    parent->adopt(newNode);
                    parent = newNode;
                }
            }
           if(xml.name() != "data" && xml.name() != "key" && xml.name() != "graphml"){

               QXmlStreamAttributes attributes = xml.attributes();
               if(attributes.hasAttribute("id")) {
                  ID = attributes.value("id").toString();
                  gotBool = true;
               }

               if(xml.name() == "graph"){
                    //kind= GraphML::GRAPH;
                    qDebug() << "Got Graph";
               }else if(xml.name() == "node"){
                   kind = GraphML::NODE;
                   qDebug() << "Got Node";
               }else if(xml.name() == "edge"){
                   kind = GraphML::EDGE;
                    qDebug() << "Got Edge";
               }else{
                   output += "<"+xml.name().toString()+">\n";
                   //continue;
               }

           }
        }
       if(xml.isEndElement())
       {
           if(xml.name() != "data" && xml.name() != "key"  && xml.name() != "graphml")
           {
               qDebug () <<"End of current Element";
               parent = parent->getParent();
               output += "</"+xml.name().toString()+">\n";
           }
       }
    }


    if (xml.hasError()) {
    }
    this->output = output;
    this->parentGraph = (Graph *)parent;

    //DO MAGIC!
    return false;
}

void Model::parseGraphML(QXmlStreamReader xml, GraphML *parent)
{

}

QString Model::exportGraphML()
{

    QString returnable = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    returnable +="<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns\">\n";

    for(int i=0; i < this->attributeTypes.size();i++){
        returnable += this->attributeTypes[i]->toGraphML(1);
    }

    returnable +="</graphml>\n";

    return returnable;

}

Graph *Model::getGraph()
{
    return this->parentGraph;
}

void Model::parseDataAttribute(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();

    QString name;
    QString id;
    QString typeStr;
    QString forStr;

    GraphMLAttribute::TYPE type;
    GraphML::KIND forKind;


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

    attributeTypes.append(attribute);

}

void Model::parseGraph(QXmlStreamReader &xml)
{

}

void Model::parseNode(QXmlStreamReader &xml)
{

}

void Model::parseEdge(QXmlStreamReader &xml)
{

}
