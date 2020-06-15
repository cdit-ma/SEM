#ifndef GRAPHMLPRINTER_H
#define GRAPHMLPRINTER_H

#include <QString>
#include <QTextStream>
#include <QList>
#include <QSet>

class Entity;
class Node;
class Edge;
class Data;
class Key;

class GraphmlPrinter
{
    public:
        enum class ExportFlags{
            EXPORT_ALL_EDGES,
            HUMAN_READABLE_DATA,
            STRIP_VISUAL_DATA
        };
        GraphmlPrinter(const QSet<ExportFlags>& flags);
        GraphmlPrinter();

        QString ToGraphml(const QList<Entity*>& entities);
        QString ToGraphml(const Node& node);

        static bool IsKeyVisual(const QString& key_name);
    private:
        void ToGraphmlStream(const Node& node, QTextStream& stream, int indent_depth);
        void ToGraphmlStream(const Edge& edge, QTextStream& stream, int indent_depth);
        void ToGraphmlStream(const Key& key, QTextStream& stream, int indent_depth);
        void ToGraphmlStream(const Data& data, QTextStream& stream, int indent_depth);
        void DatasToGraphmlStream(const Entity& data, QTextStream& stream, int indent_depth);
        QString GetKeyID(const Key& key);
        bool ExportKey(const Key& key);

        bool IsFlagSet(const ExportFlags& flag) const;
        
        const QSet<ExportFlags> flags_;
};

inline uint qHash(GraphmlPrinter::ExportFlags key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
};

#endif // GRAPHMLPRINTER_H
