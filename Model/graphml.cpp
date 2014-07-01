#include "graphml.h"

qint32 GraphML::_id = 0;

GraphML::GraphML(GraphML::KIND kind, QString name)
{
    this->id = ++_id;
    this->kind = kind;

    this->setName(name);

}

GraphML::~GraphML()
{
    //Destructor.
}

void GraphML::setName(QString name)
{
    this->name = name;
}

GraphML::KIND GraphML::getKind() const
{
    return this->kind;
}

QString GraphML::getName() const
{
    return this->name;
}

QVector<GraphMLData *> GraphML::getData()
{
    return this->attachedData;
}

void GraphML::attachData(GraphMLData *data)
{
    this->attachedData.append(data);
}

void GraphML::attachData(QVector<GraphMLData *> data)
{
    while(!data.isEmpty()){
        this->attachData(data.front());
        data.pop_front();
    }
}

qint32 GraphML::getID() const
{
    return this->id;
}
