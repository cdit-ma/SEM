#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include <QList>
#include <QHash>
#include "node.h"

class NodeFactory
{
private:
    struct NodeLookupStruct{
        Node::NODE_KIND kind;
        QString kind_str;
        std::function<Node* ()> constructor;
        Node* node = 0;
    };
public:
    static QList<Data*> getDefaultData(QString nodeKind);
    static QList<Data*> getDefaultData(Node::NODE_KIND nodeKind);
    static Node* createNode(QString nodeKind);
    static Node* createNode(Node::NODE_KIND nodeKind);
    static QString getNodeKindString(Node::NODE_KIND nodeKind);
    static QList<Node::NODE_KIND> getNodeKinds();
    static Node::NODE_KIND getNodeKind(QString kind);

protected:
    NodeFactory();
    ~NodeFactory();
    static NodeFactory* getFactory();

    QHash<Node::NODE_KIND, NodeLookupStruct*> nodeLookup;
    QHash<QString, Node::NODE_KIND> nodeKindLookup;

    Node* _createNode(Node::NODE_KIND kind);
    QList<Data*> _getDefaultData(Node::NODE_KIND kind);
private:
    void addKind(Node::NODE_KIND kind, QString kind_str, std::function<Node* ()> constructor);
    static NodeFactory* factory;
};

#endif // NODEFACTORY_H
