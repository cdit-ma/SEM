#include "model.h"
#include <QDebug>


Model::Model(): Node()
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

bool Model::canAdoptChild(Node *child)
{

    return true;
}

QString Model::toGraphML(qint32 indentationLevel)
{
    return "";
}

bool Model::canConnect(Node* attachableObject)
{
    return false;
}
