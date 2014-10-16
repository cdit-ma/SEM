#include "model.h"
#include <QDebug>


Model::Model(QString name): GraphMLContainer(GraphML::GRAPH, name)
{
    qCritical() << "Constructed Model.";


}

Model::~Model()
{
    removeEdges();
    removeChildren();
    qCritical() << "Destructed Model.";
}

QString Model::toString()
{
    return QString("Model[%1]: "+this->getName()).arg(this->getID());
}

bool Model::isAdoptLegal(GraphMLContainer *child)
{

    return true;
}

QString Model::toGraphML(qint32 indentationLevel)
{
    return "";
}

bool Model::isEdgeLegal(GraphMLContainer *attachableObject)
{
    return false;
}
