#include "aggregate.h"
#include "../data.h"

#include "../../edgekinds.h"



Aggregate::Aggregate(EntityFactory* factory) : Node(factory, NODE_KIND::AGGREGATE, "Aggregate"){
	auto node_kind = NODE_KIND::AGGREGATE;
	QString kind_string = "Aggregate";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Aggregate();});

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "namespace", QVariant::String);
    RegisterDefaultData(factory, node_kind, "comment", QVariant::String);
};

Aggregate::Aggregate(): Node(NODE_KIND::AGGREGATE)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::AGGREGATE);

    setInstanceKind(NODE_KIND::AGGREGATE_INSTANCE);
    

    connect(this, &Node::dataChanged, this, &Aggregate::updateType);
}


QString Aggregate::getType()
{
    QString agg_namespace = getDataValue("namespace").toString();
    QString agg_label = getDataValue("label").toString();
    return agg_namespace + "::" + agg_label;
    //return agg_namespace + "::" + agg_label;
}

void Aggregate::updateType(int ID, QString keyName)
{
    if(keyName == "label" || keyName == "namespace"){
        //Get Data
        Data* d = getData("type");
        if(d){
            d->setValue(getType());
        }
    }
}

bool Aggregate::canAdoptChild(Node *child)
{
    if(!child){
        return false;
    }

    NODE_KIND kind = child->getNodeKind();

    switch(kind){
    case NODE_KIND::ENUM_INSTANCE:
        break;
    case NODE_KIND::AGGREGATE_INSTANCE:
        break;
    case NODE_KIND::MEMBER:
        break;
    case NODE_KIND::VECTOR:
        break;
    case NODE_KIND::VECTOR_INSTANCE:{
        Node* vector = child->getDefinition();
        if(vector && vector->hasChildren()){
            //Check first child of vector.
            Node* vectorChild = vector->getChildren(0)[0];

            //If first child has a definition.
            if(vectorChild && vectorChild->getDefinition()){
               Node* aggregate = vectorChild->getDefinition();
               if(this == aggregate || isAncestorOf(aggregate)){
                   return false;
               }
            }
        }
        break;
    }
    default:
        return false;
    }

    return Node::canAdoptChild(child);
}

bool Aggregate::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}


