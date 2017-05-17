#ifndef TEMPENTITY_H
#define TEMPENTITY_H

#include <QObject>


#include "kinds.h"
#include "nodekinds.h"
#include "edgekinds.h"
#include "modelcontroller.h"

class Data;


class TempEntity
{
public:
    TempEntity(GRAPHML_KIND entityKind, TempEntity* parent=0);
    ~TempEntity();
    void setLineNumber(int lineNumber);

    QList<TempEntity*> getChildren();
    void setResetPosition();
    bool shouldConstruct();
    bool ignoreConstruct();
    void setIgnoreConstruction(bool ignore=true);
    void setIgnoreVisuals(bool ignore=true);
    bool ignoreVisuals();
    bool isTop();

    int getRetryCount();
    void incrementRetryCount();
    void resetIncrementCount();

    void addChild(TempEntity* child);

    void setSource(Node* src);
    void setDestination(Node* dst);
    Node* getSource();
    Node* getDestination();

    void setID(QString ID);
    void setPrevID(int ID);
    bool hasPrevID();
    int getPrevID();
    void setActualID(int ID);

    void appendEdgeKinds(QList<EDGE_KIND> edgeKinds);
    void appendEdgeKind(EDGE_KIND edgeKind);
    void removeEdgeKind(EDGE_KIND edgeKind);
    EDGE_KIND getEdgeKind();
    bool hasEdgeKind();

    QVariant getData(QString key);
    bool gotData(QString key);

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
    QList<TempEntity*> children;

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
    GRAPHML_KIND entityKind;
};

#endif // TEMPENTITY_H
