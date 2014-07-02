#ifndef GRAPHMLKEY_H
#define GRAPHMLKEY_H
#include <QString>
#include "graphml.h"

class GraphMLKey: public GraphML
{
public:
    enum TYPE {BOOLEAN, INT, LONG, FLOAT, DOUBLE, STRING};
    GraphMLKey(QString name, QString typeStr, QString forStr);

    bool operator==(const GraphMLKey &other) const;

    QString toGraphML(qint32 indentationLevel=0);
    TYPE getType() const;
    GraphML::KIND getKind() const;
private:

    GraphML::KIND kind;
    QString kindStr;
    TYPE type;
    QString typeStr;

    QString name;
    QString id;

    static int _Did;

    // GraphML interface
public:
    QString toString();
};

#endif // GRAPHMLKEY_H
