#include "graphml.h"
#include "graphmldata.h"
#include <qdebug>
int GraphML::_Uid = 0;

GraphML::GraphML(GraphML::KIND kind, QString name)
{
    this->Uid = ++_Uid;

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

QString GraphML::getID() const
{
    return this->id;
}

QString GraphML::getData(QString keyName)
{
    for(int i=0; i < this->attachedData.size(); i++){
        if(this->attachedData[i]->getKey()->getName() == keyName){
            return this->attachedData[i]->getValue();
        }
    }
    qCritical() <<"GraphML Object does not contain Data!";
    return "";

}

QVector<GraphMLData *> GraphML::getData()
{
    return this->attachedData;
}

void GraphML::attachData(GraphMLData *data)
{
    GraphML::KIND keyKind = data->getKey()->getForKind();

    if(keyKind == this->getKind() || (keyKind == GraphML::ALL && this->getKind() < GraphML::ALL)){
        this->attachedData.append(data);
    }else{
        qCritical() << "Cannot attach <data> to this object. Wrong Kind!";
        qCritical() << data->getKey()->getForKind() << " != " << this->getKind();
    }
}

void GraphML::attachData(QVector<GraphMLData *> data)
{
    while(!data.isEmpty()){
        this->attachData(data.front());
        data.pop_front();
    }
}

void GraphML::setID(QString id)
{
    this->id = id;
}

