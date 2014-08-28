#include "model.h"
#include "QDebug"
#include <QObject>
#include <QMap>

Model::Model(): QObject()
{
    qDebug() << "Constructed New Model";

    this->parentGraph = new Graph("Parent Graph");

    int componentCount = this->parentGraph->getChildren().size();
    loadCount=0;
    emit setComponentCount(componentCount);

    buildGraphMLKey("width","double","node");
    buildGraphMLKey("height","double","node");
}

Model::~Model()
{
    qDebug() << "Killing Model";
    delete parentGraph;

}

bool Model::importGraphML(QString inputGraphML, GraphMLContainer *currentParent)
{
    int lines = inputGraphML.count("\n");

    //Construct a Stream Reader for the XML graph
    QXmlStreamReader xml(inputGraphML);

    //Key Lookup provides a way for the original key "id" to be linked with the internal object GraphMLKey
    QMap<QString , GraphMLKey*> keyLookup;

    //Node lookup provides a way for the original edge source/target ID's to be linked with the internal object GraphMLContainer
    QMap<QString, GraphMLContainer *> nodeLookup;

    //A list for storing the current Parse <data> tags owned by a <node>
    QVector<GraphMLData*> currentNodeData;

    //A list storing all the Edge information (source, target, data)
    QVector<EdgeStruct> currentEdges;

    GraphMLKey * currentKey;

    //Used to keep track of state inside the XML
    GraphML::KIND nowParsing = GraphML::NONE;
    GraphML::KIND nowInside = GraphML::NONE;

    //Used to store the ID of the node we are to construct
    QString nodeID = "";

    QString graphID = "";

    //Used to store the ID of the new node we will construct later.
    QString newNodeID = "";

    if(currentParent == NULL){
        currentParent = this->getGraph();
    }
    //Counts the number of </node> elements we encounter to correctly traverse to the correct insertion point.
    int parentDepth = 0;

    if (xml.hasError()) {
        qDebug() << "ERRORS YO";
        return false;
    }

     int componentCount = this->parentGraph->getChildren().size();


    while(!xml.atEnd()){
        double percentage = ((double)xml.lineNumber() / (double)lines) * 100;

        emit currentAction_UpdateProgress(percentage);
        //Read the next tag
        xml.readNext();

        //Get the tagName
        QString tagName = xml.name().toString();

        if(tagName == "edge"){
            //Construct an Intermediate struct containing the information about the Edge
            if(xml.isStartElement()){
                //Parse the Edge element into a EdgeStruct object
                EdgeStruct newEdge;
                newEdge.id = getAttribute(xml, "id");
                newEdge.linenumber = xml.lineNumber();
                newEdge.source = getAttribute(xml, "source");
                newEdge.target = getAttribute(xml, "target");

                //Append to the list of Edges found.
                currentEdges.append(newEdge);
                //Set the current object type to EDGE.
                nowInside = GraphML::EDGE;
            }
            if(xml.isEndElement()){
                //Set the current object type to NONE.
                nowInside = GraphML::NONE;
            }
        }else if(tagName == "data"){
            if(xml.isStartElement()){
                //Get the datas corresponding Key ID
                QString keyID = getAttribute(xml, "key");

                //Check to see if we parsed that key already.
                if(!keyLookup.contains(keyID)){
                    qCritical() << QString("Line #%1: Cannot find the <key> to match the <data key=\"%2\">").arg(QString::number(xml.lineNumber()),keyID);
                    return false;
                }

                //Get the value of the value of the data tag.
                QString dataValue = xml.readElementText();

                //Construct a GraphMLData object out of the xml, using the key found in keyLookup
                GraphMLData *data = new GraphMLData(keyLookup[keyID], dataValue);

                //Attach the data to the current object.
                switch(nowInside){
                //Attach the Data to the TempEdge if we are currently inside an Edge.
                case GraphML::EDGE:{
                    currentEdges.last().data.append(data);
                    break;
                }

                    //Attach the Data to the list of Data to be attached to the node.
                case GraphML::NODE:{
                    currentNodeData.append(data);
                    break;
                }
                default:
                    //Delete the newly constructed object. We don't need it
                    delete(data);
                }
            }
            if(xml.isEndElement()){
            }
        }else if(tagName == "key"){
            if(xml.isStartElement()){
                nowInside = GraphML::KEY;
                //Parse the Attribute Definition.
                currentKey = parseGraphMLKey(xml);

                //Get the Key ID.
                QString keyID = getAttribute(xml,"id");

                //Place the key in the lookup Map.
                keyLookup.insert(keyID, currentKey);
            }
            if(xml.isEndElement()){
                nowInside = GraphML::NONE;
            }
        }else if(tagName =="default"){
            if(nowInside == GraphML::KEY){
                QString defaultValue = xml.readElementText();
                currentKey->setDefaultValue(defaultValue);
            }
        }else if(tagName == "node"){
            //If we have found a new <node> it means we should build the previous <node id=nodeID> node.
            if(xml.isStartElement()){
                //Get the ID of the Node, W
                newNodeID = getAttribute(xml, "id");

                nowInside = GraphML::NODE;
                nowParsing = GraphML::NODE;
            }
            if(xml.isEndElement()){
                //Increase the depth, as we have seen another </node>
                parentDepth++;
                //We have reached the end of a Node, therefore not inside a Node anymore.
                nowInside = GraphML::NONE;
            }
        }else if(tagName == "graph"){
            if(xml.isStartElement()){
                //Get the ID of the Graph, we want to point this at the current Parent.
                graphID = getAttribute(xml, "id");
            }

        }else{
            if(xml.isEndDocument()){
                //Construct the final Node
                nowParsing = GraphML::NODE;
            }else{
                nowParsing = GraphML::NONE;
            }
        }

        if(nowParsing == GraphML::NODE){
            //If we have a nodeID to build
            if(nodeID != ""){
                //Construct the specialised Node
                Node* newNode = this->parseGraphMLNode(nodeID, currentNodeData);
                emit setComponentCount(componentCount+=2);
                //Clear the Node Data List.
                currentNodeData.clear();

                //Adopt the new Node into the currentParent
                if(currentParent->isAdoptLegal(newNode)){
                    currentParent->adopt(newNode);

                    //Construct a GraphMLData object out of the xml, using the key found in keyLookup

                }else{
                    qCritical() << QString("Line #%1: Node Cannot Adopt child Node!").arg(xml.lineNumber());
                    return false;
                }

                //Set the currentParent to the Node Construced
                currentParent = newNode;


                //Navigate back to the correct parent.
                while(parentDepth > 0){
                    currentParent = currentParent->getParent();

                    //Check if the parent is a graph or a node!
                    //We only care about if it's not a graph!
                    Graph* graph = dynamic_cast<Graph*> (currentParent);
                    if(graph != 0){
                        parentDepth --;
                    }
                }

                //Add the new Node to the lookup table.
                nodeLookup.insert(nodeID, newNode);


                //Construct in GUI
                emit constructNodeItem(newNode);

                //If we have encountered a Graph object, we should point it to it's parent Node to allow links to Graph's
                if(graphID != ""){
                    nodeLookup.insert(graphID, newNode);
                    graphID = "";
                }
            }
            //Set the current nodeID to equal the newly found NodeID.
            nodeID = newNodeID;
        }
    }

    if (xml.hasError()) {
        qCritical() << QString("XML Error!");
        return false;
    }



    //Construct the Edges from the EdgeTemp objects
    for(int i =0; i< currentEdges.size(); i++){
        EdgeStruct edge = currentEdges[i];
        GraphMLContainer* s = nodeLookup[edge.source];
        GraphMLContainer* d = nodeLookup[edge.target];

        if(s != 0 && d != 0){
            if(s->isEdgeLegal(d)){
                Edge* newEdge = new Edge(s, d);
                newEdge->attachData(edge.data);
                edges.append(newEdge);

                qCritical() << "Emitting ConstructEdgeItem";
                emit constructEdgeItem(newEdge);
            }else{
                qDebug() << s->toString() << " to " << d->toString();
                qCritical() << QString("Line #%1: Edge Not Valid!").arg(QString::number(edge.linenumber));
                return false;
            }
        }else{
            qCritical() << "Edge is Illegal!";
            return false;
        }
    }


    return true;
}



QString Model::exportGraphML()
{

    emit currentAction_ShowProgress(true);
    emit currentAction_UpdateProgress(50);

    QString returnable = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    returnable +="<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns\">\n";

    for(int i=0; i < this->keys.size();i++){
        returnable += this->keys[i]->toGraphML(1);
    }
    returnable += this->parentGraph->toGraphML(1);

    returnable +="</graphml>\n";

    emit currentAction_UpdateProgress(100);

    emit currentAction_ShowProgress(false);

    return returnable;
}

QVector<Edge *> Model::getAllEdges()
{
    return this->edges;

}

Graph *Model::getGraph()
{
    return this->parentGraph;
}

void Model::constructedGraphML(GraphMLContainer *newlyCreated)
{
    emit constructNodeItemNew(newlyCreated);
}

void Model::model_MakeChildNode(Node *parent)
{


    qCritical()<<"Model::model_MakeChildNode";
    Node* newPort;
    QString kind;
    outputEvent = !outputEvent;

    if(outputEvent){
        newPort = new InputEventPort("NewInEventPort");
        kind = "InEventPort";
    }else{
        newPort = new OutputEventPort("NewOutEventPort");
        kind = "OutEventPort";
    }
    GraphMLKey* x = buildGraphMLKey("x","double","node");
    GraphMLKey* y = buildGraphMLKey("y","double","node");
    GraphMLKey* k = buildGraphMLKey("kind","string","node");
    GraphMLKey* t = buildGraphMLKey("type","string","node");
    GraphMLKey* l = buildGraphMLKey("label","string","node");

    GraphMLData* xD = new GraphMLData(x,QString::number(0));
    GraphMLData* yD = new GraphMLData(y,QString::number(0));
    GraphMLData* kD = new GraphMLData(k, kind);
    GraphMLData* tD = new GraphMLData(t, "TYPE");
    GraphMLData* lD = new GraphMLData(l, kind);

    newPort->attachData(xD);
    newPort->attachData(yD);
    newPort->attachData(tD);
    newPort->attachData(kD);
    newPort->attachData(lD);


    if(parent->isAdoptLegal(newPort)){
        parent->adopt(newPort);
        connect(newPort, SIGNAL(deleteGUI(GraphMLContainer*)),this,SLOT(deleteUIComponent(GraphMLContainer*)));
        connect(newPort, SIGNAL(constructGUI(GraphMLContainer*)),this, SLOT(constructedGraphML(GraphMLContainer*)));

        emit newPort->constructGUI(newPort);
    }else{
        delete newPort;
    }
}


void Model::init_ImportGraphML(QStringList inputGraphMLData, GraphMLContainer *currentParent)
{
    emit enableGUI(false);
    emit currentAction_ShowProgress(true);

    int files = inputGraphMLData.size();

    for (int i = 0; i < files ; i++){
        //Update Dialogs etc.

        emit currentAction_UpdateProgress(0,QString("Importing GraphML file %1 / %2").arg(QString::number(i + 1), QString::number(files)));

        QString currentGraphMLData = inputGraphMLData.at(i);
        bool result = importGraphML(currentGraphMLData, currentParent);
    }

    emit currentAction_ShowProgress(false);
    emit enableGUI(true);
}

void Model::init_ExportGraphML(QString file)
{
    emit enableGUI(false);


    emit currentAction_UpdateProgress(0, QString("Exporting Model to GraphML"));

    QString data = exportGraphML();
    emit returnExportedGraphMLData(file, data);



    emit enableGUI(true);

}

void Model::deleteUIComponent(GraphMLContainer *comp)
{
    emit removeUIComponent(comp);

    //disconnect(comp, SIGNAL(deleteGUI(GraphMLContainer*)),this,SLOT(deleteUIComponent(GraphMLContainer*)));
}

void Model::updatePosition(GraphMLContainer *comp, QPointF pos)
{
    comp->setDataValue("x", QString::number(pos.x()));
    comp->setDataValue("y", QString::number(pos.y()));
}

GraphMLKey*  Model::parseGraphMLKey(QXmlStreamReader &xml)
{
    QString name = getAttribute(xml,"attr.name");
    QString typeStr = getAttribute(xml,"attr.type");
    QString forStr = getAttribute(xml,"for");

    return buildGraphMLKey(name,typeStr,forStr);
}

GraphMLKey *Model::buildGraphMLKey(QString name, QString type, QString forString)
{
    GraphMLKey *attribute = new GraphMLKey(name, type, forString);

    for(int i = 0 ; i < keys.size(); i ++){
        if(keys[i]->operator ==(*attribute)){
            delete attribute;
            return this->keys[i];
        }
    }

    this->keys.append(attribute);
    return attribute;
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
    connect(newNode, SIGNAL(deleteGUI(GraphMLContainer*)),this,SLOT(deleteUIComponent(GraphMLContainer*)));
    connect(newNode, SIGNAL(constructGUI(GraphMLContainer*)),this, SLOT(constructedGraphML(GraphMLContainer*)));

    emit newNode->constructGUI(newNode);



    return newNode;
}


QString Model::getAttribute(QXmlStreamReader &xml, QString attrID)
{
    QXmlStreamAttributes attributes = xml.attributes();

    if(attributes.hasAttribute(attrID)){
        return attributes.value(attrID).toString();
    }else{
        qCritical() << "Expecting Attribute key" <<attrID;
        return "";
    }

}

void Model::reset()
{
    removeKeys();

    delete parentGraph;
    parentGraph = new Graph("Parent Graph");
}

void Model::removeKeys()
{
    //Delete all Children
    while(!keys.isEmpty()){
        GraphMLKey* current = keys.first();
        keys.removeFirst();
        delete current;
    }

}

int Model::getNodeCount()
{
    return this->parentGraph->getChildren().size();
}


