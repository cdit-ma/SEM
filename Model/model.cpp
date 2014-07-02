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

bool Model::importGraphML(QString inputGraphML, GraphMLContainer *currentParent)
{
    //Construct a Stream Reader for the XML graph
    QXmlStreamReader xml(inputGraphML);

    //Key Lookup provides a way for the original key "id" to be linked with the internal object GraphMLKey
    QMap<QString , GraphMLKey*> keyLookup;

    //Node lookup provides a way for the original edge source/target ID's to be linked with the internal object GraphMLContainer
    QMap<QString, GraphMLContainer *> nodeLookup;

    //A list for storing the current Parse <data> tags owned by a <node>
    QVector<GraphMLData*> currentNodeData;

    //A list storing all the Edge information (source, target, data)
    QVector<TempEdge> currentEdges;


    //Used to keep track of state inside the XML
    GraphML::KIND nowParsing = GraphML::NONE;
    GraphML::KIND nowInside = GraphML::NONE;

    //Used to store the ID of the current and previously discovered node ID
    QString nodeID = "";
    QString prevNodeID = "";

    //Counts the number of </node> elements we encounter to correctly traverse to the correct insertion point.
    int parentDepth = 0;

    while(!xml.atEnd()){
        //Read the next tag
        xml.readNext();

        //Get the tagName
        QString tagName = xml.name().toString();

        if(tagName == "edge"){
            if(xml.isStartElement()){
                //Parse the Edge element into a TempEdge object
                currentEdges.append(parseEdge(xml));
                nowInside = GraphML::EDGE;
            }
            if(xml.isEndElement()){
                nowInside = GraphML::NONE;
            }
        }else if(tagName == "data"){
            if(xml.isStartElement()){
                //Get the corresponding Key ID
                QString keyID = getAttribute(xml, "key");

                //Check to see if we have a matching Key already parsed.
                if(!keyLookup.contains(keyID)){
                    qCritical() << "Not Matching Key!";
                    return false;
                }

                //Construct a GraphMLData object out of the xml, using the key found in keyLookup
                GraphMLData* data = parseGraphMLData(xml, keyLookup[keyID]);

                //Check what element we are currently traversing
                switch(nowInside){
                //Attach the Data to the TempEdge if we are currently inside an Edge.
                case GraphML::EDGE:{
                    currentEdges.last().data.append(data);
                    break;
                }

                    //Attach the Data to the list of Data to be attached to the node.
                case GraphML::NODE:{
                    currentNodeData.append(data);
                }
                }
            }
            if(xml.isEndElement()){
            }
        }else if(tagName == "key"){
            if(xml.isStartElement()){
                //Parse the Attribute Definition.
                GraphMLKey* attribute = parseGraphMLKey(xml);

                //Get the original Key ID.
                QString originalKeyID = getAttribute(xml,"id");

                //Place the key in the lookup Map.
                keyLookup.insert(originalKeyID, attribute);
            }
        }else if(tagName == "node"){
            if(xml.isStartElement()){
                //Get the ID of the Node
                //prevNodeID = nodeID;
                nodeID = getAttribute(xml, "id");

                nowInside = GraphML::NODE;
                nowParsing = GraphML::NODE;
            }
            if(xml.isEndElement()){
                //Increase the depth, as we have seen another </node>
                parentDepth++;
                //We have reached the end of a Node, therefore not inside a Node anymore.
                nowInside = GraphML::NONE;
            }
        }else{
            if(xml.isEndDocument()){
                //Construct a Node
                nowParsing = GraphML::NODE;
            }else{
                nowParsing = GraphML::NONE;
            }
        }

        if(nowParsing == GraphML::NODE){
            if(prevNodeID != ""){
                //Construct the specialised Node
                Node* newNode = this->parseGraphMLNode(prevNodeID, currentNodeData);

                //Clear the Node Data List.
                currentNodeData.clear();


                if(currentParent->isAdoptLegal(newNode)){
                    currentParent->adopt(newNode);
                }else{
                    qCritical() << "Node Cannot Adopt child Node!";
                    return false;
                }

                //Set the currentParent to the Node Construced
                currentParent = newNode;
                //Increment the depth
                parentDepth ++;

                //Navigate back to the correct parent.
                while(parentDepth > 1){
                    currentParent = currentParent->getParent();
                    //Check if the parent is a graph or a node!
                    //We only care about if it's not a graph!
                    Graph* graph = dynamic_cast<Graph*> (currentParent);
                    if(graph != 0){
                        parentDepth --;
                    }
                }

                parentDepth = 0;

                nodeLookup.insert(prevNodeID, newNode);
            }
            prevNodeID = nodeID;
        }
    }

    if (xml.hasError()) {

    }

    //Construct the Edges from the EdgeTemp objects
    for(int i =0; i< currentEdges.size(); i++){
        TempEdge edge = currentEdges[i];
        GraphMLContainer* s = nodeLookup[edge.source];
        GraphMLContainer* d = nodeLookup[edge.target];

        if(s->isEdgeLegal(d)){
            Edge* newEdge = new Edge(s, d);
            newEdge->attachData(edge.data);
        }else{
            qDebug() << s->toString() << " to " << d->toString();
            qCritical() << "Edge Not Valid!";
            return false;
        }
    }
    return true;
}


QString Model::exportGraphML()
{

    QString returnable = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    returnable +="<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns\">\n";

    for(int i=0; i < this->keys.size();i++){
        returnable += this->keys[i]->toGraphML(1);
    }
    returnable += this->parentGraph->toGraphML(1);

    returnable +="</graphml>\n";

    return returnable;

}

Graph *Model::getGraph()
{
    return this->parentGraph;
}

GraphMLKey*  Model::parseGraphMLKey(QXmlStreamReader &xml)
{
    QString name = getAttribute(xml,"attr.name");
    QString typeStr = getAttribute(xml,"attr.type");;
    QString forStr = getAttribute(xml,"for");;

    GraphMLKey *attribute = new GraphMLKey(name, typeStr, forStr);

    for(int i = 0 ; i < this->keys.size(); i ++){
        if( this->keys[i]->operator ==(*attribute)){
            delete(attribute);
            return this->keys[i];
        }
    }

    this->keys.append(attribute);
    return attribute;
}


GraphMLData* Model::parseGraphMLData(QXmlStreamReader& xml, GraphMLKey* attachedKey)
{
    QString value = xml.readElementText();

    GraphMLData *data = new GraphMLData(attachedKey, value);
    return data;
}


Node *Model::parseGraphMLNode(QString ID, QVector<GraphMLData *> data)
{

    Node *newNode;

    QString kind;
    //Get kind from nodeData.
    for(int i=0; i < data.size(); i++){
        if(data[i]->getKey()->getName() == "kind"){
            kind = data[i]->getValue();
        }
    }

    if(kind == "ComponentAssembly"){
        newNode = new ComponentAssembly(ID);
    }else if(kind == "ComponentInstance"){
        newNode = new ComponentInstance(ID);
    }else if(kind == "Attribute"){
        newNode = new Attribute(ID);
    }else if(kind == "OutEventPort"){
        newNode = new OutputEventPort(ID);
    }else if(kind == "InEventPort"){
        newNode = new InputEventPort(ID);
    }else{
        qDebug() << "Kind:"<<kind << "Not implemented";
    }

    newNode->attachData(data);


    return newNode;
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

TempEdge Model::parseEdge(QXmlStreamReader &xml)
{
    TempEdge tE;
    tE.id = getAttribute(xml, "id");
    tE.source = getAttribute(xml, "source");
    tE.target = getAttribute(xml, "target");

    return tE;
}

