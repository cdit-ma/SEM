#ifndef TEMPENTITY_H
#define TEMPENTITY_H

#include <QObject>
#include <QVariant>
#include "kinds.h"
#include "nodekinds.h"
#include "edgekinds.h"
#include <QMap>
#include <QQueue>

class TempEntity
{
public:
    TempEntity(GRAPHML_KIND kind, TempEntity* parent = 0);
    ~TempEntity();

    bool isNode() const;
    bool isEdge() const;
    
    void setLineNumber(int line_number);
    int getLineNumber();
    
    TempEntity* getParent();

    QString getKind();

    NODE_KIND getNodeKind();

    void setIDStr(QString id_str);
    QString getIDStr();
    bool gotPreviousID();
    int getPreviousID();

    void setID(int ID);
    int getID();
    bool gotID();
    void setImplicitlyConstructed();
    bool isImplicitlyConstructed();

    QList<TempEntity*> getChildren();
    
    const QList<NODE_KIND>& getParentStack();

    void setSourceIDStr(QString id);
    QString getSourceIDStr();
    int getSourceIDInt();;
    int getTargetIDInt();;
    
    void setTargetIDStr(QString id);
    QString getTargetIDStr();

    void setSourceID(int id);
    int getSourceID();
    
    void setTargetID(int id);
    int getTargetID() const;

    bool isUUIDMatched();
    void setUUIDMatched(bool uuid_matched);

    
    void appendEdgeKinds(QList<EDGE_KIND> edgeKinds);
    void appendEdgeKind(EDGE_KIND edgeKind);
    void removeEdgeKind(EDGE_KIND edgeKind);
    EDGE_KIND getEdgeKind();
    bool gotEdgeKind();
    
    QVariant getDataValue(QString key);
    bool gotData(QString key_name);
    void addData(QString key_name, QVariant value);
    QList<QString> getKeys();
    void removeData(QString key_name);
    void clearData();

    void AddImplicitlyConstructedNodeID(NODE_KIND kind, int id);
    void AddImplicitlyConstructedEdgeID(EDGE_KIND kind, int id);
    
    bool GotImplicitlyConstructedNodeID(NODE_KIND kind);
    bool GotImplicitlyConstructedEdgeID(EDGE_KIND kind);

    QQueue<int> GetLeftOverImplicitlyConstructedNodeIds();

    int TakeNextImplicitlyConstructedNodeID(NODE_KIND kind);
    int TakeNextImplicitlyConstructedEdgeID(EDGE_KIND kind);
private:
    void addChild(TempEntity* child);

    TempEntity* parent = 0;
    

    QHash<QString, QVariant> data;
    QList<TempEntity*> children;
    QList<EDGE_KIND> edge_kinds;

    QMap<NODE_KIND, QQueue<int> > implicit_nodes;
    QMap<EDGE_KIND, QQueue<int> > implicit_edges;
    
    int line_number = -1;
    QString id_str;
    QString kind_str;
    bool uuid_matched = false;

    int previous_id = -1;
    int actual_id = -1;

    QString source_id_str;
    QString target_id_str;

    int source_id = -1;
    int target_id = -1;

    bool implicitly_constructed = false;
    GRAPHML_KIND graphml_kind = GRAPHML_KIND::NONE;

    NODE_KIND node_kind = NODE_KIND::NONE;
protected:
    QList<NODE_KIND> parent_stack;
};

#endif // TEMPENTITY_H
