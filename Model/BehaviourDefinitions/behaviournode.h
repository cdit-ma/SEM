#ifndef BEHAVIOURNODE_H
#define BEHAVIOURNODE_H
#include "../node.h"
#include <QHash>
#include <QList>
class Parameter;

class ParameterRequirement{
public:
    ParameterRequirement(QString nodeKind, QString className, QString functionName, QString parameterName, QString type, bool inputParameter = true, QString defaultValue="");
    QString getName();
    QString getType();
    QString getValue();
    QString getFunctionName();
    QString getClassName();

    bool isInputParameter();
    bool isReturnParameter();
    bool matches(Parameter* p);
private:
    QString nodeKind;
    QString className;
    QString functionName;
    QString parameterName;
    QString parameterType;
    QString value;

    bool inputParameter;
};

class BehaviourNode : public Node
{
public:
    BehaviourNode(bool isStart = false, bool isEnd = false, bool restrictDepth = true, Node::NODE_TYPE type = NT_NODE);
    bool isStart();
    bool isEnd();
    bool isUnconnectable();


    QList<ParameterRequirement*> getAllParameters();
    QList<ParameterRequirement*> getNeededParameters();
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
    QList<ParameterRequirement*> neededParameters;

public:
    static void addParameter(QString nodeKind, QString className, QString functionName, QString parameterName, QString parameterType, bool inputParameter = true, QString defaultValue="");
    static QList<ParameterRequirement*> getParameters(QString nodeKind);
    static QHash<QString, QList<ParameterRequirement*> > _parameters;
};

#endif // BEHAVIOURNODE_H
