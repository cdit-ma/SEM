#ifndef TEMPENTITY_H
#define TEMPENTITY_H

#include "../Controllers/modelcontroller.h"
#include "data.h"
#include "entity.h"

#include <QObject>

class TempEntity
{
public:
    TempEntity(Entity::ENTITY_KIND entityKind, TempEntity* parent=0);
    ~TempEntity();
    void setLineNumber(int lineNumber);

    void setResetPosition();
    bool shouldConstruct();
    bool ignoreConstruct();
    void setIgnoreConstruction(bool ignore=true);
    bool isTop();

    int getRetryCount();
    void incrementRetryCount();
    void resetIncrementCount();

    void setSource(Node* src);
    void setDestination(Node* dst);
    Node* getSource();
    Node* getDestination();

    void setID(QString ID);
    void setPrevID(int ID);
    bool hasPrevID();
    int getPrevID();
    void setActualID(int ID);

    void appendEdgeKind(EDGE_KIND edgeKind);
    void removeEdgeKind(EDGE_KIND edgeKind);
    EDGE_KIND getEdgeKind();
    bool hasEdgeKind();

    QVariant getData(QString key);

    TempEntity* getParentEntity();
    void setParentID(QString ID);
    void setActualParentID(int ID);
    QString getKind();

    QString getParentID();
    int getActualParentID();
    bool gotActualID();
    bool gotActualParentID();

    ReadOnlyState getReadOnlyState();
    bool gotReadOnlyState();



    int getLineNumber();
    void addData(Data* data);
    QList<Data*> getData();
    QList<Data*> takeDataList();
    void clearData();

    QString getOriginalID();
    int getActualID();

    bool isNode();
    bool isEdge();


    void setSrcID(QString ID);
    void setDstID(QString ID);
    void setActualSrcID(int ID);
    void setActualDstID(int ID);
    QString getSrcID();
    QString getDstID();

    int getActualSrcID();
    int getActualDstID();


private:
    ReadOnlyState readOnlyState;
    int lineNumber;
    TempEntity* parent;
    QString ID;
    QString nodeKind;

    int actualID;
    int oldID;
    int actualParentID;
    QString parentID;
    QString srcID;
    QString dstID;
    int actualSrcID;
    int actualDstID;

    Node* src;
    Node* dst;

    QList<EDGE_KIND> edgeKinds;

    int retryCount;
    bool ignoreConstruction;

    bool _resetPosition;
    bool isReadOnly;
    QList<Data*> dataList;
    Entity::ENTITY_KIND entityKind;
};

#endif // TEMPENTITY_H
