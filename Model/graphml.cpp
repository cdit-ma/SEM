#include "graphml.h"
#include "graphmldata.h"
#include <QDebug>
int GraphML::_Uid = 0;

GraphML::GraphML(GraphML::KIND kind, QString name):QObject(0)
{
    this->Uid = ++_Uid;
    this->kind = kind;
    this->setName(name);
    this->_deleting = false;
    setGenerated(false);
}

bool GraphML::isNode()
{
    return kind == GraphML::NODE;
}

bool GraphML::isEdge()
{
    return kind == GraphML::EDGE;
}

bool GraphML::isDeleting()
{
    return _deleting;
}

GraphML::~GraphML()
{
    _deleting = true;
    removeData();
}


void GraphML::removeData(){
    while(attachedData.size() > 0){
        GraphMLData* data = attachedData.takeFirst();
        if(data){
            data->setParent(0);
        }

        data->deleteLater();
    }
}

bool GraphML::wasGenerated()
{
    return generated;
}

void GraphML::setGenerated(bool isGenerated)
{
    generated = isGenerated;
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

QString GraphML::getID()
{
    return QString::number(Uid);
}


void GraphML::updateDataValue(QString keyName, QString value)
{
    GraphMLData* data = getData(keyName);

    if(data != 0){
        data->setValue(value);
    }else{
#ifdef DEBUG_MODE
        qWarning() << "GraphML Object does not contain Data: " << keyName << "!";
#endif
    }
}

QString GraphML::getDataValue(QString keyName)
{
    GraphMLData* data = getData(keyName);

    if(data != 0){
        return data->getValue();
    }else{
#ifdef DEBUG_MODE
        qWarning() <<"GraphML::getDataValue() << GraphML Object does not contain Data: " << keyName;
#endif
        return "";
    }
}

GraphMLData *GraphML::getData(QString keyName)
{
    if(!_deleting){
        for(int i=0; i < attachedData.size(); i++){
            if(attachedData[i]->getKey() != 0){
                if(attachedData[i]->getKey()->getName() == keyName){
                    return this->attachedData[i];
                }
            }
        }
    }
    return 0;
}

GraphMLData *GraphML::getData(GraphMLKey *key)
{
    if(!_deleting){
        for(int i=0; i < attachedData.size(); i++){
            if(attachedData[i]->getKey() == key){
                return this->attachedData[i];
            }
        }
      }
    return 0;

}

bool GraphML::containsData(GraphMLData *data)
{
    return attachedData.contains(data);
}


QList<GraphMLData *> GraphML::getData()
{
    return this->attachedData;
}



void GraphML::attachData(GraphMLData *data)
{
    if(data != 0){
        GraphML::KIND keyKind = data->getKey()->getForKind();

        if(keyKind == this->getKind() || (keyKind == GraphML::ALL && this->getKind() < GraphML::ALL)){

            foreach(GraphMLData* cData, attachedData){
                if(cData->getKey() == data->getKey()){
#ifdef DEBUG_MODE
                    qWarning() << "Got Duplicate Data";
#endif
                    cData->setValue(data->getValue());
                    return;
                }
            }
            if(data){
                data->setParent(this);
                attachedData.append(data);
                emit dataAdded(data);
            }
        }else{

#ifdef DEBUG_MODE
            qWarning() << "Cannot attach <data> to this object. Wrong Kind!";
            qWarning() << data->getKey()->getForKind() << " != " << this->getKind();
#endif
        }
    }
}

void GraphML::removeData(GraphMLData *data)
{
    int index = attachedData.indexOf(data);
    if(index >= 0){
        attachedData.removeAt(index);
        data->setParent(0);
        emit dataRemoved(data->getID());
    }
}

void GraphML::attachData(QList<GraphMLData *> data)
{
    while(!data.isEmpty()){
        attachData(data.takeFirst());
    }
}





