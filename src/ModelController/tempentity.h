#ifndef TEMPENTITY_H
#define TEMPENTITY_H

#include <QObject>
#include "kinds.h"
#include "nodekinds.h"
#include "edgekinds.h"

class Data;
class TempEntity
{
public:
    TempEntity(GRAPHML_KIND kind, TempEntity* parent = 0);
    ~TempEntity();

    bool isNode();
    bool isEdge();
    
    void setLineNumber(int line_number);
    int getLineNumber();
    
    TempEntity* getParent();

    QString getKind();

    void setIDStr(QString id_str);
    bool gotPreviousID();
    int getPreviousID();

    void setID(int ID);
    int getID();
    bool gotID();

    QList<TempEntity*> getChildren();

    void setSourceIDStr(QString id);
    QString getSourceIDStr();
    int getSourceIDInt();
    int getTargetIDInt();
    
    void setTargetIDStr(QString id);
    QString getTargetIDStr();

    void setSourceID(int id);
    int getSourceID();
    
    void setTargetID(int id);
    int getTargetID();


    
    void appendEdgeKinds(QList<EDGE_KIND> edgeKinds);
    void appendEdgeKind(EDGE_KIND edgeKind);
    void removeEdgeKind(EDGE_KIND edgeKind);
    EDGE_KIND getEdgeKind();
    bool gotEdgeKind();
    
    QVariant getDataValue(QString key);
    bool gotData(QString key);
    void addData(Data* data);
    QList<Data*> getData();
    QList<Data*> takeDataList();
    void clearData();
private:
    void addChild(TempEntity* child);

    TempEntity* parent = 0;
    
    QList<TempEntity*> children;
    QList<Data*> data_list;
    QList<EDGE_KIND> edge_kinds;
    
    int line_number = -1;
    QString id_str;
    QString kind_str;

    int previous_id = -1;
    int actual_id = -1;

    QString source_id_str;
    QString target_id_str;

    int source_id = -1;
    int target_id = -1;

    GRAPHML_KIND graphml_kind = GRAPHML_KIND::NONE;
};

#endif // TEMPENTITY_H
