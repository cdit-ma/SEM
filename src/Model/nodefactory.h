#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include <QHash>
#include "node.h"

class NodeFactory
{
private:
    struct NodeLookupStruct{
        Node::NODE_KIND kind;
        QString kind_str;
        std::function<Node* ()> constructor;
    };
public:
    static Node* createNode(QString nodeKind);
    static Node* createNode(Node::NODE_KIND nodeKind);
    static QString getNodeKindString(Node::NODE_KIND nodeKind);
    static QList<Node::NODE_KIND> getNodeKinds();
    static Node::NODE_KIND getNodeKind(QString kind);

protected:
    NodeFactory();
    static NodeFactory* getFactory();

    QHash<Node::NODE_KIND, NodeLookupStruct*> nodeLookup;
    QHash<QString, Node::NODE_KIND> nodeKindLookup;

    Node* _createNode(Node::NODE_KIND kind);
private:
    void addKind(Node::NODE_KIND kind, QString kind_str, std::function<Node* ()> constructor);
    static NodeFactory* factory;
};

#endif // NODEFACTORY_H
