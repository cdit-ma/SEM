#ifndef BEHAVIOURNODE_H
#define BEHAVIOURNODE_H
#include "../node.h"
#include <QHash>
#include <QList>
class Parameter;

class ParameterRequirement{
public:
    ParameterRequirement(QString name, QString type, bool inputParameter = true, QString defaultValue="");
    QString getName();
    QString getType();
    QString getValue();
    bool isInputParameter();
    bool isReturnParameter();
    bool matches(Parameter* p);
private:
    QString name;
    QString type;
    QString value;
    bool inputParam;
};

class BehaviourNode : public Node
{
public:
    BehaviourNode(bool isStart = false, bool isEnd = false, bool restrictDepth = true, Node::NODE_TYPE type = NT_NODE);
    bool isStart();
    bool isEnd();
    bool isUnconnectable();


    QList<ParameterRequirement*> getParameters();
    bool hasParameters();
public:
    void setUnconnectable(bool unconnectable);
    bool canConnect(Node* attachableObject);
    virtual bool canAdoptChild(Node* child);

private:
    bool needsParameter(Parameter* p);
    bool _isStart;
    bool _isEnd;
    bool _restrictDepth;
    bool _isUnconnectable;

public:
    static void addParameter(QString kind, QString name, QString type, bool inputParameter = true, QString defaultValue="");
    static QHash<QString, QList<ParameterRequirement*> > _parameters;
};

#endif // BEHAVIOURNODE_H
