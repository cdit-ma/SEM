#ifndef TEMPENTITY_H
#define TEMPENTITY_H

#include <QObject>
#include "data.h"
#include "entity.h"
#include "Controller/controller.h"

class TempEntity
{
public:
    TempEntity(Entity::ENTITY_KIND entityKind, TempEntity* parent=0);
    void setLineNumber(int lineNumber);

    bool isTop();


    void setID(QString ID);
    void setActualID(int ID);


    TempEntity* getParentEntity();
    void setParentID(QString ID);
    void setActualParentID(int ID);

    QString getParentID();
    int getActualParentID();
    bool gotActualID();
    bool gotActualParentID();

    ReadOnlyState getReadOnlyState();
    bool gotReadOnlyState();



    void addData(Data* data);
    QList<Data*> getData();

    QString getOriginalID();
    int getActualID();

    bool isNode();
    bool isEdge();


    void setSrcID(QString ID);
    void setDstID(QString ID);
    QString getSrcID();
    QString getDstID();


private:
    ReadOnlyState readOnlyState;
    int lineNumber;
    TempEntity* parent;
    QString ID;
    int actualID;
    int actualParentID;
    QString parentID;
    QString srcID;
    QString dstID;

    bool isReadOnly;
    QList<Data*> dataList;
    Entity::ENTITY_KIND entityKind;
};

#endif // TEMPENTITY_H
