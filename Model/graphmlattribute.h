#ifndef GRAPHMLATTRIBUTE_H
#define GRAPHMLATTRIBUTE_H

#include "QString"
#include "graphml.h"
class GraphMLAttribute
{
public:
    enum TYPE {BOOLEAN, INT, LONG, FLOAT, DOUBLE, STRING};

    GraphMLAttribute(QString id, QString name, QString typeStr, QString forStr);

    QString toGraphML(qint32 indentationLevel=0);
    QString getID() const;
    QString getName() const;
    TYPE getType() const;
    GraphML::KIND getKind() const;
private:
    GraphML::KIND kind;
    QString kindStr;

    TYPE type;
    QString typeStr;

    QString name;
    QString id;

};

#endif // GRAPHMLATTRIBUTE_H
