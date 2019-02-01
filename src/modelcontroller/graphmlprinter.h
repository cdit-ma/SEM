#ifndef GRAPHMLPRINTER_H
#define GRAPHMLPRINTER_H

#include <QString>
#include <QTextStream>
#include <QList>

class Entity;
class Node;
class Edge;
class Data;
class Key;

class GraphmlPrinter
{
    public:
        static QString ToGraphml(const QList<Entity*>& entities, bool all_edges, bool human_readible=false);
        static QString ToGraphml(const Node& node);
    private:
        static void ToGraphmlStream(const Node& node, QTextStream& stream, int indent_depth, bool human_readible);
        static void ToGraphmlStream(const Edge& edge, QTextStream& stream, int indent_depth, bool human_readible);
        static void ToGraphmlStream(const Key& key, QTextStream& stream, int indent_depth, bool human_readible);
        static void ToGraphmlStream(const Data& data, QTextStream& stream, int indent_depth, bool human_readible);
        static void DatasToGraphmlStream(const Entity& data, QTextStream& stream, int indent_depth, bool human_readible);
        static QString GetKeyID(const Key& key, bool human_readible);
};

#endif // GRAPHMLPRINTER_H
