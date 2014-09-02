#include "model.h"
#include "QDebug"
#include <QObject>
#include <QMap>

Model::Model(): QObject()
{
    qDebug() << "Model::Model()";

    //Construct Parent Graph.
    this->parentGraph = new Graph("ParentGraph");
}

Model::~Model()
{
    qDebug() << "Model::~Model()";
    delete parentGraph;
}

bool Model::importGraphML(QString inputGraphML, GraphMLContainer *currentParent)
{
    qDebug() << "Model::importGraphML()";

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

    //Used to store the ID of the graph we are to construct
    QString graphID = "";

    //Used to store the ID of the new node we will construct later.
    QString newNodeID = "";

    //If we have been passed no parent, set it as the graph of this Model.
    if(currentParent == NULL){
        currentParent = this->getGraph();
    }



    //Construct a Stream Reader for the XML graph
    QXmlStreamReader xmlErrorChecking(inputGraphML);

    //Check for Errors
    while(!xmlErrorChecking.atEnd()){
        xmlErrorChecking.readNext();
        if (xmlErrorChecking.hasError()){
            qCritical() << "Model::importGraphML() << Parsing Error!";
            qCritical() << "\t" << xmlErrorChecking.errorString();
            return false;
        }
    }

    //Now we know we have no errors, so read Stream again.
    QXmlStreamReader xml(inputGraphML);

    //Get the number of lines in the input GraphML XML String.
    float lineCount = inputGraphML.count("\n") / 100;


    //Counts the number of </node> elements we encounter to correctly traverse to the correct insertion point.
    int parentDepth = 0;

    //While the document has more lines.
    while(!xml.atEnd()){
        //Read the next tag
        xml.readNext();

        //Calculate the current percentage
        float lineNumber = xml.lineNumber();
        double percentage = (lineNumber * 100 / lineCount);
        emit view_UpdateProgressDialog(percentage);

        //Get the tagName
        QString tagName = xml.name().toString();

        if(tagName == "edge"){
            //Construct an Intermediate struct containing the information about the Edge
            if(xml.isStartElement()){
                //Parse the Edge element into a EdgeStruct object
                EdgeStruct newEdge;
                newEdge.id = getAttribute(xml, "id");
                newEdge.lineNumber = lineNumber;
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
                //Get the ID of the Node
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

                //Clear the Node Data List.
                currentNodeData.clear();

                //Adopt the new Node into the currentParent
                if(currentParent->isAdoptLegal(newNode)){
                    currentParent->adopt(newNode);
                }else{
                    qCritical() << QString("Line #%1: Node Cannot Adopt child Node!").arg(xml.lineNumber());
                    //Delete the newly created node.
                    delete newNode;
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
                //emit constructNodeItem(newNode);

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

    //Construct the Edges from the EdgeTemp objects
    for(int i =0; i< currentEdges.size(); i++){
        EdgeStruct edge = currentEdges[i];
        GraphMLContainer* s = nodeLookup[edge.source];
        GraphMLContainer* d = nodeLookup[edge.target];

        if(s != 0 && d != 0){
            if(s->isEdgeLegal(d)){
                //Construct the edge, and attach the data.
                Edge* newEdge = new Edge(s, d);
                newEdge->attachData(edge.data);

                //Add the edge to the list of edges constructed.
                edges.append(newEdge);

                connect(newEdge, SIGNAL(constructGUI(Edge*)),this, SLOT(model_ConstructGUIEdge(Edge*)));
                connect(newEdge, SIGNAL(destructGUI(Edge*)), this, SLOT(model_DestructGUIEdge(Edge*)));
                emit newEdge->constructGUI(newEdge);

            }else{
                qCritical() << QString("Line #%1: Edge Not Valid!").arg(QString::number(edge.lineNumber));
                return false;
            }
        }else{
            qCritical() << "Edge is Illegal!";
            return false;
        }
    }


    return true;
}


Graph *Model::getGraph()
{
    return this->parentGraph;
}

void Model::model_ConstructGUINode(GraphMLContainer *node)
{
    emit view_ConstructGUINode(node);
}

void Model::model_ConstructGUIEdge(Edge *edge)
{
    emit view_ConstructGUIEdge(edge);
}

void Model::model_DestructGUINode(GraphMLContainer *node)
{
    emit view_DestructGUINode(node);
}

void Model::model_DestructGUIEdge(Edge *edge)
{
    emit view_DestructGUIEdge(edge);
}

QString Model::exportGraphML(QVector<GraphMLContainer *> nodes)
{
    //outputs for the stringed output.
    QString keyXML, edgeXML, nodeXML;

    //Vectors for the contained entities
    QVector<GraphMLKey*> containedKeys;
    QVector<Edge*> containedEdges;
    QVector<GraphMLContainer*> containedNodes;

    float size = nodes.size() * 2;

    float count = 0;
    foreach(GraphMLContainer* node, nodes)
    {
        //Add this node to the list of nodes contained.
        if(!containedNodes.contains(node)){
            containedNodes.append(node);
        }

        //Get all keys used by this node.
        foreach(GraphMLKey* key, node->getKeys())
        {
            //Add the <key> tag to the list of Keys contained.
            if(!containedKeys.contains(key)){
                containedKeys.append(key);
                keyXML += key->toGraphML(1);
            }
        }


        //Get all children nodes and append them to
        foreach(GraphMLContainer* child, node->getChildren())
        {
            //Add the child node to the list of nodes contained.
            if(!containedNodes.contains(child)){
                containedNodes.append(child);
            }
        }

        count++;
        double value = (count/size) * 100;
        emit view_UpdateProgressDialog(value);
    }

    foreach(GraphMLContainer* node, nodes)
    {
        foreach(Edge* edge, node->getEdges())
        {
            if(!containedEdges.contains(edge)){
                GraphMLContainer* src = edge->getSource();
                GraphMLContainer* dst = edge->getDestination();

                //If this edge is encapsulated by the current selection we should export it.
                if(containedNodes.contains(src) && containedNodes.contains(dst)){
                    containedEdges.append(edge);
                    edgeXML += edge->toGraphML(2);

                    //Get the Edges attached keys.
                    foreach(GraphMLKey* key, edge->getKeys())
                    {
                        if(!containedKeys.contains(key)){
                            containedKeys.append(key);
                            keyXML += key->toGraphML(1);
                        }
                    }
                }
            }
        }
        nodeXML += node->toGraphML(2);

        count++;
        double value = (count/size) * 100;
        emit view_UpdateProgressDialog(value);
    }

    //XMLize the output.
    QString returnable = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    returnable +="<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns\">\n";
    returnable += keyXML;
    returnable +="\n\t<graph edgedefault=\"directed\" id=\"parentGraph0\">\n";
    returnable += nodeXML;
    returnable += edgeXML;
    returnable += "\t</graph>\n";
    returnable += "</graphml>";


    return returnable;

}

QVector<GraphMLContainer *> Model::getChildren(int depth)
{
    return parentGraph->getChildren(depth);
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
    GraphMLKey* x = buildGraphMLKey("x", "double", "node");
    GraphMLKey* y = buildGraphMLKey("y", "double", "node");
    GraphMLKey* k = buildGraphMLKey("kind", "string", "node");
    GraphMLKey* t = buildGraphMLKey("type", "string", "node");
    GraphMLKey* l = buildGraphMLKey("label", "string", "node");

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
        connect(newPort, SIGNAL(constructGUI(GraphMLContainer*)),this, SLOT(model_ConstructGUINode(GraphMLContainer*)));
        connect(newPort, SIGNAL(destructGUI(GraphMLContainer*)), this, SLOT(model_DestructGUINode(GraphMLContainer*)));

        emit newPort->constructGUI(newPort);
    }else{
        delete newPort;
    }
}


void Model::init_ImportGraphML(QStringList inputGraphMLData, GraphMLContainer *currentParent)
{
    emit view_EnableGUI(false);
    emit view_UpdateProgressDialog(true);

    int files = inputGraphMLData.size();

    for (int i = 0; i < files ; i++){
        //Update Dialogs etc.
        emit view_UpdateProgressDialog(0,QString("Importing GraphML file %1 / %2").arg(QString::number(i + 1), QString::number(files)));

        QString currentGraphMLData = inputGraphMLData.at(i);
        bool result = importGraphML(currentGraphMLData, currentParent);
    }


    emit view_UpdateProgressDialog(false);
    emit view_EnableGUI(true);
}

void Model::init_ExportGraphML(QString file)
{
    emit view_EnableGUI(false);

    emit view_UpdateProgressDialog(true);
    emit view_UpdateProgressDialog(0, QString("Exporting Model to GraphML"));

    QString data = exportGraphML(parentGraph->getChildren(0));

    emit view_ReturnExportedData(file, data);

    emit view_UpdateProgressDialog(false);
    emit view_EnableGUI(true);

}


void Model::updatePosition(GraphMLContainer *comp, QPointF pos)
{
    comp->updateDataValue("x", QString::number(pos.x()));
    comp->updateDataValue("y", QString::number(pos.y()));
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
    connect(newNode, SIGNAL(constructGUI(GraphMLContainer*)),this, SLOT(model_ConstructGUINode(GraphMLContainer*)));
    connect(newNode, SIGNAL(destructGUI(GraphMLContainer*)), this, SLOT(model_DestructGUINode(GraphMLContainer*)));

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
